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
$body = $source.Substring($source.IndexOf('// Marine mesh work-item'),$source.IndexOf('// Marine deferred node insertion')-$source.IndexOf('// Marine mesh work-item'))
foreach($name in @('MarineQueue_LinkBefore','MarineQueue_SpliceRun')) {
    $body = [regex]::Match($source,'(?s)static void '+$name+'[(].*?\n}').Value+"`n"+$body
}
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-workqueue-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_613510'='@Reference_MarineQueue_RebuildWorkItems@8'; 'FUN_6135f0'='@Reference_MarineQueue_SetWorkPriority@16'; 'FUN_613620'='@Reference_MarineQueue_InsertWorkItem@12'; 'FUN_6136f0'='@Reference_MarineQueue_RemoveLink@12'; 'FUN_613790'='@Reference_MarineQueue_TopWorkItem@8'; 'FUN_6137d0'='@Reference_MarineQueue_Empty@8' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$asm = Join-Path $outputDir 'marine-workqueue-reference.asm'
Set-Content $asm (".686`n.model flat`n.code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-workqueue-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-workqueue-reference.obj'
$testObj = Join-Path $outputDir 'marine-workqueue-test.obj'
$exe = Join-Path $outputDir 'marine-workqueue-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine work queue compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_workqueue.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine work queue differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine work queue operations match MASM in 58,320 complete-state comparisons.'
