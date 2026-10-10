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
$body = $source.Substring($source.IndexOf('// Marine graph ownership')).Replace('free(', 'TestFree(')
$body=$body.Replace('ASM_PROC(FUN_5e48f0)','extern "C" void FUN_5e48f0(); KING_ALIAS("_FUN_5e48f0","@TestQueueDestruct@8")')
$header = @'
#include "../src/king.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
typedef char AsmLabel[1];
extern "C" void TestFree(void*);
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0","_TestError")
extern "C" AsmLabel g_marineErrorFormat, g_marineDate;
'@
$cpp = Join-Path $outputDir 'marine-graph-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_60cdd0'=@('MarineGreed_Destruct',8); 'FUN_60cf00'=@('MarineTree_Delete',12); '$L_60cf80'=@('MarineGreed_Delete',12); 'FUN_60cd90'=@('MarineVertex_Delete',12) }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, '@Reference_'+$methods[$label][0]+'@'+$methods[$label][1]) }
$reference=$reference.Replace('FUN_5e48f0','@TestQueueDestruct@8').Replace('FUN_60aad0','_TestError').Replace('_free','_TestFree')
$bindings='EXTERN ??3@YAXPAX@Z:PROC'+"`n"+'EXTERN @TestQueueDestruct@8:PROC'+"`n"+'EXTERN _TestError:PROC'+"`n"+'EXTERN _TestFree:PROC'+"`n"
$variables=@{ '652f48'='g_marineBaseVtable'; '684a20'='g_marineTreeFile'; '6724b0'='g_marineErrorFormat'; '6724f8'='g_marineDate' }
foreach($label in $variables.Keys) { $name=$variables[$label]; $bindings+='EXTERN _'+$name+":BYTE`n"+'$L_'+$label+' TEXTEQU <_'+$name+">`n" }
$asm = Join-Path $outputDir 'marine-graph-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-graph-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-graph-reference.obj'
$testObj = Join-Path $outputDir 'marine-graph-test.obj'
$exe = Join-Path $outputDir 'marine-graph-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine graph compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_graph.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine graph differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine graph teardown matches MASM in 4,096 comparisons.'
