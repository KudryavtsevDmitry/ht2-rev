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
$body = $source.Substring($source.IndexOf('// Marine task state and registration helpers')).Replace('ASM_VAR(DWORD, g_marineStartDelay, $L_653008)', 'extern "C" DWORD g_marineStartDelay;')
$header = "#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`n"
$cpp = Join-Path $outputDir 'marine-state-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '60e0d0'='SetMode'; '60e0e0'='GetMode'; '60e560'='Activate'; '60e5e0'='Deactivate'; '60ea70'='SetPendingValue'; '60f1b0'='ClearRegistration'; '60f280'='SetFirstBounds'; '60f2a0'='SetSecondBounds' }
foreach ($label in $methods.Keys) {
    $bytes = if($methods[$label] -match 'Bounds') {16} elseif($methods[$label] -in @('GetMode','ClearRegistration')) {8} else {12}
    $reference += [regex]::Match($masm, ('(?s)FUN_'+$label+' PROC.*?FUN_'+$label+' ENDP')).Value.Replace('FUN_'+$label, '@Reference_'+$methods[$label]+'@'+$bytes) + "`n"
}
$bindings = 'EXTERN _g_marineStartDelay:DWORD' + "`n" + '$L_653008 TEXTEQU <_g_marineStartDelay>' + "`n"
$asm = Join-Path $outputDir 'marine-state-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-state-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-state-reference.obj'
$testObj = Join-Path $outputDir 'marine-state-test.obj'
$exe = Join-Path $outputDir 'marine-state-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Queue compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_state.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine queue differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine task state helpers match MASM in 1024 state and return-value comparisons.'
