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
$body = $source.Substring($source.IndexOf('// Marine mesh scratch')).Replace('operator delete(', 'TestDelete(')
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
extern "C" void __cdecl TestDelete(void*);
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-scratch-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '$L_610f20'='@Reference_MarineScratch_ResetCounts@8'; '$L_611d90'='@Reference_MarineScratch_AppendPoint@12'; '$L_611e00'='@Reference_MarineScratch_ReleaseAuxiliary@8'; '$L_611e40'='@Reference_MarineScratch_ReleasePoints@8' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$reference=$reference.Replace('??3@YAXPAX@Z','_TestDelete').Replace('$L_6530c0','_g_ms_recordType')
$bindings="EXTERN _TestDelete:PROC, _g_ms_recordType:DWORD`n"
$asm = Join-Path $outputDir 'marine-scratch-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-scratch-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-scratch-reference.obj'
$testObj = Join-Path $outputDir 'marine-scratch-test.obj'
$exe = Join-Path $outputDir 'marine-scratch-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine scratch compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_scratch.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine scratch differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine scratch routines match MASM in 12,288 state and callback comparisons.'
