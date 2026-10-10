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
$source = Get-Content (Join-Path $repo 'src\aitask.cpp') -Raw
$body = [regex]::Match($source, '(?s)extern "C" void __cdecl AI_ShuffleState_Init\(.*?\n\}').Value
if (!$body) { throw 'Shuffle initializer not found' }
$header = "#include `"../src/king.h`"`nextern `"C`" { extern DWORD g_aiShuffleState[55], g_aiShuffleIndex, g_aiShuffleOtherIndex; }`n"
$cpp = Join-Path $outputDir 'shuffle-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = [regex]::Match($masm, '(?s)\$L_4838c0 PROC.*?\$L_4838c0 ENDP').Value
if (!$reference) { throw 'MASM shuffle initializer not found' }
$reference = $reference.Replace('$L_4838c0', '_Reference_Shuffle_Init')
$bindings = "EXTERN _g_aiShuffleState:DWORD`nEXTERN _g_aiShuffleIndex:DWORD`nEXTERN _g_aiShuffleOtherIndex:DWORD`n"
$bindings += '$L_68d6d8 TEXTEQU <_g_aiShuffleState>' + "`n" + '$L_68d7b0 TEXTEQU <_g_aiShuffleState+216>' + "`n" + '$L_68d7b4 TEXTEQU <_g_aiShuffleIndex>' + "`n" + '$L_68d7b8 TEXTEQU <_g_aiShuffleOtherIndex>' + "`n"
$asm = Join-Path $outputDir 'shuffle-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'shuffle-actual.obj'
$referenceObj = Join-Path $outputDir 'shuffle-reference.obj'
$testObj = Join-Path $outputDir 'shuffle-test.obj'
$exe = Join-Path $outputDir 'shuffle-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Shuffle compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_ai_shuffle.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Shuffle differential test failed: exit code $LASTEXITCODE" }
Write-Output 'All 55 AI shuffle-state words and both indices match MASM after three resets.'
