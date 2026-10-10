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
$body = $source.Substring($source.IndexOf('// Marine mesh cleanup'), $source.IndexOf('// Marine object lifetime') - $source.IndexOf('// Marine mesh cleanup')).Replace('malloc(', 'TestMalloc(')
$helpers = [regex]::Match($source, '(?s)static BYTE\* MarineGreed_NextNode\(.*?\n\}').Value + "`n" + [regex]::Match($source, '(?s)static void MarineQueue_LinkBefore\(.*?\n\}').Value
$header = "#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" void* TestMalloc(unsigned);`nextern `"C`" BYTE* __fastcall MarineQueue_TopEdge(BYTE*,void*);`nextern `"C`" void FUN_606760(); KING_ALIAS(`"_FUN_606760`",`"_TestSplitEdge`")`n"
$body = $helpers + "`n" + $body
$cpp = Join-Path $outputDir 'marine-cleanup-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '60d780'='PruneVertices'; '60d750'='DrainEdges' }
foreach ($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)FUN_'+$label+' PROC.*?FUN_'+$label+' ENDP')).Value.Replace('FUN_'+$label, '@Reference_'+$methods[$label]+'@8') + "`n" }
$reference = $reference.Replace('_malloc','_TestMalloc').Replace('FUN_60f9d0','@TestTopEdge@8').Replace('FUN_606760','_TestSplitEdge')
$bindings = 'EXTERN ??2@YAPAXI@Z:PROC' + "`n" + 'EXTERN ??3@YAXPAX@Z:PROC' + "`n" + 'EXTERN _TestMalloc:PROC' + "`n" + 'EXTERN @TestTopEdge@8:PROC' + "`n" + 'EXTERN _TestSplitEdge:PROC' + "`n" + 'EXTERN _g_marineUnusedVertices:DWORD' + "`n" + 'EXTERN _g_marineUnusedVertexCount:DWORD' + "`n" + '$L_725d50 TEXTEQU <_g_marineUnusedVertices>' + "`n" + '$L_725d54 TEXTEQU <_g_marineUnusedVertexCount>' + "`n"
$asm = Join-Path $outputDir 'marine-cleanup-reference.asm'
Set-Content $asm (".686`n.model flat`nASSUME FS:NOTHING`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-cleanup-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-cleanup-reference.obj'
$testObj = Join-Path $outputDir 'marine-cleanup-test.obj'
$exe = Join-Path $outputDir 'marine-cleanup-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Queue compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_cleanup.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine queue differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine cleanup matches MASM across exhaustive vertex-reference masks, allocation failures and edge draining.'
