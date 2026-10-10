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
$body = $source.Substring($source.IndexOf('// Marine 4x4 matrix'),$source.IndexOf('// Marine particle rendering')-$source.IndexOf('// Marine 4x4 matrix'))
$init = Get-Content (Join-Path $repo 'src\marineinit.cpp') -Raw
$body += [regex]::Match($init,'(?s)extern "C" DWORD\* __cdecl MarineInit_Identity.*?\n}').Value
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-matrix-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_60a880'='_Reference_MarineMatrix_Zero'; 'FUN_60a900'='_Reference_MarineMatrix_RotateZ'; 'FUN_60a960'='_Reference_MarineMatrix_Translate'; 'FUN_60a9b0'='_Reference_MarineMatrix_Multiply'; 'FUN_60a8b0'='_Reference_MarineInit_Identity' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$asm = Join-Path $outputDir 'marine-matrix-reference.asm'
Set-Content $asm (".686`n.model flat`n.code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-matrix-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-matrix-reference.obj'
$testObj = Join-Path $outputDir 'marine-matrix-test.obj'
$exe = Join-Path $outputDir 'marine-matrix-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine matrix compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_matrix.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine matrix differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine matrix construction and composition match MASM in 61,440 comparisons.'
