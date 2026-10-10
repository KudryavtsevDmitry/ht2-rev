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
$body = $source.Substring($source.IndexOf('// Marine deferred node insertion'),$source.IndexOf('// Marine mesh scratch')-$source.IndexOf('// Marine deferred node insertion'))
foreach($name in @('MarineQueue_RemoveNode','MarineQueue_RebuildNodes')) {
    $body = [regex]::Match($source,'(?s)extern "C" [^\n]+ '+$name+'[(]BYTE\* self.*?\n}').Value+"`n"+$body
}
foreach($name in @('MarineQueue_LinkBefore','MarineQueue_SpliceRun','MarineQueue_NodeNext','MarineQueue_NodePrevious')) {
    $body = [regex]::Match($source,'(?s)static (?:void|BYTE\*) '+$name+'[(].*?\n}').Value+"`n"+$body
}
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
extern "C" float g_mq_zero;
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-deferredqueue-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_613310'='@Reference_MarineQueue_SetDeferredPriority@16'; 'FUN_613340'='@Reference_MarineQueue_InsertDeferredNode@12'; 'FUN_6134c0'='@Reference_MarineQueue_TopDeferredNode@8'; 'FUN_60f690'='@Reference_MarineQueue_RemoveNode@12'; 'FUN_60f390'='@Reference_MarineQueue_RebuildNodes@8' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$reference=$reference.Replace('$L_64c264','_g_mq_zero')
$bindings="EXTERN _g_mq_zero:DWORD`n"
$asm = Join-Path $outputDir 'marine-deferredqueue-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-deferredqueue-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-deferredqueue-reference.obj'
$testObj = Join-Path $outputDir 'marine-deferredqueue-test.obj'
$exe = Join-Path $outputDir 'marine-deferredqueue-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine deferred queue compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_deferredqueue.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine deferred queue differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine deferred queue operations match MASM in 29,160 complete-state comparisons.'
