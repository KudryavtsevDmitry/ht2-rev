$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$makefile = Get-Content (Join-Path $repo 'Makefile') -Raw
$vc = [regex]::Match($makefile, '(?m)^VCDIR = (.+)\r?$').Groups[1].Value.Trim()
$sdk = [regex]::Match($makefile, '(?m)^SDKINC = (.+)\r?$').Groups[1].Value.Trim()
$outputDir = Join-Path $repo 'build-test'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$compiler = Join-Path $vc 'bin\HostX86\x86\cl.exe'
$linker = Join-Path $vc 'bin\HostX86\x86\link.exe'
$sdkLib = Join-Path (Split-Path (Split-Path $sdk -Parent) -Parent) ('Lib\' + (Split-Path $sdk -Leaf) + '\um\x86')
$flags = @('/nologo', '/c', '/O2', '/GS-', '/Zl', "/I$vc\include", "/I$sdk\um", "/I$sdk\shared", "/I$sdk\ucrt", '/arch:IA32')
$source = Get-Content (Join-Path $repo 'src\marineinit.cpp') -Raw
$variables = [regex]::Matches($source, 'ASM_VAR\((\w+), (\w+), (\$L_[0-9a-f]+)\)')
$structs = [regex]::Matches($source, '(?m)^struct .*?; };').Value -join "`n"
$globals = "#include `"../src/king.h`"`n" + $structs + "`nextern `"C`" {`nint _fltused=0;`n"
$bindings = ''
foreach ($variable in $variables) {
    $type=$variable.Groups[1].Value; $name=$variable.Groups[2].Value; $label=$variable.Groups[3].Value
    $globals += "$type $name;`n"
    $bindings += "EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"
    if($type -eq 'MarineTriple') {
        $address=[Convert]::ToInt32($label.Substring(3),16)
        foreach($offset in @(4,8)) { $extra='$L_'+($address+$offset).ToString('x'); $bindings += "$extra TEXTEQU <_${name}+$offset>`n" }
    }
}
$globals += "}`n"
$globals += 'static BYTE* regions[]={' + (($variables | ForEach-Object { '(BYTE*)&'+$_.Groups[2].Value }) -join ',') + "};`n"
$globals += 'static unsigned sizes[]={' + (($variables | ForEach-Object { 'sizeof('+$_.Groups[2].Value+')' }) -join ',') + "};`n"
$methods = @{ '60d950'='DoubleMeshScale'; '60d970'='DoubleSecondaryScale'; '60d990'='BasisX'; '60d9c0'='BasisY'; '60d9f0'='BasisZ'; '60da20'='DefaultMatrix'; '60da40'='Complement'; '60da70'='Interval'; '60daa0'='DefaultNormal'; '60dad0'='DefaultAxis'; '60db00'='DefaultOffset'; '60db30'='SquaredRatio'; '60f070'='AdjustedThreshold'; '60f0a0'='ZeroVector'; '60f2c0'='DoubleAngleScale'; '60f370'='DoubleFinalScale'; '60f2e0'='DefaultLimits'; '60a8b0'='Identity' }
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
foreach($label in $methods.Keys) {
    $prefix=if($label -eq '60a8b0') {'FUN_'} else {'$L_'}
    $original=$prefix+$label
    $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($original)+' PROC.*?'+[regex]::Escape($original)+' ENDP')).Value + "`n"
}
foreach($label in $methods.Keys) { $original=if($label -eq '60a8b0') {'FUN_'+$label} else {'$L_'+$label}; $reference=$reference.Replace($original,'_Reference_'+$methods[$label]) }
$dispatch='static void Run(unsigned method, bool reference) { switch(method) {'+"`n"
$index=0
foreach($label in ($methods.Keys | Sort-Object)) {
    $name=$methods[$label]
    if($name -eq 'Identity') {
        $globals += 'extern "C" DWORD* __cdecl MarineInit_Identity(DWORD*), Reference_Identity(DWORD*);' + "`n"
        $args='g_mi_725d70.v'
    } elseif($name -eq 'DefaultLimits') {
        $globals += 'extern "C" DWORD* __cdecl MarineInit_DefaultLimits(); extern "C" DWORD* __cdecl Reference_DefaultLimits();' + "`n"
        $args=''
    } else {
        $globals += "extern `"C`" void __cdecl MarineInit_$name(); extern `"C`" void __cdecl Reference_$name();`n"
        $args=''
    }
    $dispatch += "case $index`: if(reference) Reference_$name($args); else MarineInit_$name($args); break;`n"
    $index++
}
$dispatch += "} }`n"
$globals += $dispatch
Set-Content (Join-Path $outputDir 'marine-init-test-globals.h') $globals
$cpp=Join-Path $outputDir 'marine-init-actual.cpp'
Set-Content $cpp ($source.Replace('"king.h"','"../src/king.h"').Replace('"x87.h"','"../src/x87.h"'))
$asm=Join-Path $outputDir 'marine-init-reference.asm'
Set-Content $asm (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj=Join-Path $outputDir 'marine-init-actual.obj'
$referenceObj=Join-Path $outputDir 'marine-init-reference.obj'
$testObj=Join-Path $outputDir 'marine-init-test.obj'
$exe=Join-Path $outputDir 'marine-init-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if($LASTEXITCODE) {throw 'Initializer compilation failed'}
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if($LASTEXITCODE) {throw 'Reference assembly failed'}
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_init.cpp')
if($LASTEXITCODE) {throw 'Test compilation failed'}
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE) {throw 'Test link failed'}
& $exe
if($LASTEXITCODE) {throw "Marine initializer comparison failed: exit code $LASTEXITCODE"}
Write-Output '18 marine initialization routines match MASM across 3456 method/input/precision combinations.'
