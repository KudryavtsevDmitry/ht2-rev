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
$body = $source.Substring($source.IndexOf('// Two sentinel'), $source.IndexOf('// Priority queues specific') - $source.IndexOf('// Two sentinel')).Replace('operator delete(', 'TestFree(')
$update = [regex]::Match($body, '(?s)extern "C" void __fastcall MarineGreed_Update\(.*?\n\}').Value
$body = $body.Replace($update, '')
$header = "#define timeGetTime TestTime`nextern `"C`" unsigned __stdcall TestTime();`n#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" void TestFree(void*);`n"
foreach ($label in @('60f690','6136f0','606750','6069d0','60aad0','60aa60','60f4f0','60f7b0','60f9d0','60f8d0','6137d0','606760','605900')) { $header += "ASM_PROC(FUN_$label)`n" }
foreach ($name in @('Date','ErrorFormat','File','AssertFormat','Header','Neighbor44','Neighbor52','Neighbor48')) { $header += "extern `"C`" char g_marine$name[1];`n" }
$header += "extern `"C`" DWORD g_marineRefinementEnabled, g_marineRefinementLimit;`n"
$header += 'extern "C" DWORD __fastcall MarineQueue_SetNodePriority(BYTE*,void*,BYTE*,DWORD);' + "`n" + 'extern "C" DWORD __fastcall MarineQueue_SetEdgePriority(BYTE*,void*,BYTE*,DWORD);' + "`n" + 'extern "C" BYTE* __fastcall MarineQueue_TopNode(BYTE*,void*);' + "`n" + 'extern "C" BYTE* __fastcall MarineQueue_TopEdge(BYTE*,void*);' + "`n"
$header += 'extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE*,void*,BYTE*);' + "`n"
$header += 'extern "C" BYTE* __fastcall MarineQueue_RemoveLink(BYTE*,void*,BYTE*);' + "`n" + 'extern "C" DWORD __fastcall MarineQueue_Empty(BYTE*,void*);' + "`n"
$cpp = Join-Path $outputDir 'marine-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '60d540'='RemoveNode'; '60cfa0'='RefreshPriorities'; '60d180'='Refine'; '60d250'='Emit' }
foreach ($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)FUN_'+$label+' PROC.*?FUN_'+$label+' ENDP')).Value + "`n" }
foreach ($label in $methods.Keys) { $bytes = if($label -eq '60d540') {12} else {8}; $reference = $reference.Replace('FUN_'+$label, '@Reference_'+$methods[$label]+'@'+$bytes) }
$reference = $reference.Replace('??3@YAXPAX@Z','_TestFree').Replace('__imp__timeGetTime@0','_TestTimePointer')
$bindings = "EXTERN _TestFree:PROC`nEXTERN _TestTimePointer:DWORD`nEXTERN _g_marineRefinementEnabled:DWORD`nEXTERN _g_marineRefinementLimit:DWORD`n" + '$L_685abc TEXTEQU <_g_marineRefinementEnabled>' + "`n" + '$L_725e28 TEXTEQU <_g_marineRefinementLimit>' + "`n"
foreach ($label in @('60f690','6136f0','606750','6069d0','60aad0','60aa60','60f4f0','60f7b0','60f9d0','60f8d0','6137d0','606760','605900')) { $bindings += "EXTERN FUN_${label}:PROC`n" }
$names = @{ '6724f8'='Date'; '6724b0'='ErrorFormat'; '685ac8'='File'; '6726a4'='AssertFormat'; '685b70'='Header'; '685b44'='Neighbor44'; '685b18'='Neighbor52'; '685aec'='Neighbor48' }
foreach ($label in $names.Keys) { $name = '_g_marine'+$names[$label]; $bindings += "EXTERN ${name}:BYTE`n" + '$L_' + $label + " TEXTEQU <$name>`n" }
$asm = Join-Path $outputDir 'marine-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-reference.obj'
$testObj = Join-Path $outputDir 'marine-test.obj'
$exe = Join-Path $outputDir 'marine-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine node removal matches MASM in 1536 node-removal, priority, refinement and mesh-output comparisons.'
