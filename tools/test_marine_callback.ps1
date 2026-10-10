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
$source = Get-Content (Join-Path $repo 'src\marine.cpp') -Raw
$body = $source.Substring($source.IndexOf('// Marine task setup and callbacks'), $source.IndexOf('// Marine mesh cleanup') - $source.IndexOf('// Marine task setup and callbacks'))
$header = "#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" void __fastcall MarineGreed_Update(BYTE*,void*);`n"
$cpp = Join-Path $outputDir 'marine-callback-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '605770'='InitPoints'; '60e070'='AllocatePoints'; '60e050'='SetPoints'; '60ea50'='RequestPoints'; '60dd80'='SetBounds'; '60dde0'='RebuildBounds'; '60df70'='ApplyPoint'; '60f000'='ForwardPoint'; '60f010'='ForwardPointCopy'; '60dd60'='ReleaseMesh' }
foreach ($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)FUN_'+$label+' PROC.*?FUN_'+$label+' ENDP')).Value + "`n" }
foreach ($label in $methods.Keys) {
    $bytes = if($methods[$label] -eq 'ReleaseMesh') {8} elseif($methods[$label] -match 'Point$|PointCopy$') {12} else {16}
    $reference = $reference.Replace('FUN_'+$label, '@Reference_'+$methods[$label]+'@'+$bytes)
}
$reference = $reference.Replace('FUN_60d440','@TestUpdate@8').Replace('FUN_60d780','@TestPrune@8')
$bindings = 'EXTERN ??2@YAPAXI@Z:PROC' + "`n" + 'EXTERN @TestUpdate@8:PROC' + "`n" + 'EXTERN @TestPrune@8:PROC' + "`n.data`n" + '$L_649fdb DWORD 0' + "`n"
$asm = Join-Path $outputDir 'marine-callback-reference.asm'
Set-Content $asm (".686`n.model flat`nASSUME FS:NOTHING`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-callback-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-callback-reference.obj'
$testObj = Join-Path $outputDir 'marine-callback-test.obj'
$exe = Join-Path $outputDir 'marine-callback-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Queue compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_callback.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine queue differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine setup and callbacks match MASM in 320 allocation, bounds, forwarding and mesh-release cases.'
