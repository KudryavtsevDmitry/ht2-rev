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
$body = $source.Substring($source.IndexOf('// Marine task construction'),$source.IndexOf('// Marine pending-state quad')-$source.IndexOf('// Marine task construction'))
$start=$source.IndexOf('__declspec(noinline) static float MarineTask_QuietCoordinate')
$helpers=$source.Substring($start,$source.IndexOf('extern "C" void __fastcall MarineTask_DeferredBounds')-$start)
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
typedef char AsmLabel[1];
extern "C" AsmLabel g_marineTaskVtable,g_marineTaskSecondaryVtable,g_marineErrorFormat,g_marineDate;
extern "C" BYTE* g_marineRecordedState;
extern "C" DWORD g_mt_frameValue;
extern "C" float g_mt_axisX,g_mt_axisY,g_mt_axisZ;
extern "C" BYTE* __fastcall MarineMesh_Create(BYTE*,void*,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD);
extern "C" DWORD __fastcall MarineTask_RebuildBounds(BYTE*,void*,const DWORD*,const DWORD*);
extern "C" BYTE* __fastcall MarineTask_SetMaterial(BYTE*,void*,DWORD);
KING_ALIAS("@MarineMesh_Create@36","@TestMeshCreate@36")
KING_ALIAS("@MarineTask_RebuildBounds@16","@TestRebuild@16")
KING_ALIAS("@MarineTask_SetMaterial@12","@TestMode@12")
extern "C" void FUN_5dbcc0(); KING_ALIAS("_FUN_5dbcc0","@TestMatrix@12")
extern "C" void FUN_60aad0(); KING_ALIAS("_FUN_60aad0","_TestError")
'@
$labels=@{ '5ef6d0'='@TestBase@16'; '5d3110'='@TestSecondary@12'; '5d3570'='@TestRegister@12'; '5d33d0'='@TestSecondaryEnable@12'; '5d9290'='@TestLibrary@12'; '5ef390'='@TestFindNode@12'; '5d2190'='@TestAttach@12' }
foreach($label in $labels.Keys) { $body=$body.Replace('ASM_PROC(FUN_'+$label+')','extern "C" void FUN_'+$label+'(); KING_ALIAS("_FUN_'+$label+'","'+$labels[$label]+'")') }
$cpp = Join-Path $outputDir 'marine-construct-actual.cpp'
Set-Content $cpp ($header + "`n" + $helpers + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference=[regex]::Match($masm,'(?s)FUN_60e0f0 PROC.*?FUN_60e0f0 ENDP').Value.Replace('FUN_60e0f0','@Reference_MarineTask_Construct@12')
$bindings='EXTERN ??2@YAPAXI@Z:PROC'+"`n"
$extra=@{ '60db70'='@TestMeshCreate@36'; '60dde0'='@TestRebuild@16'; '60e0d0'='@TestMode@12'; '5dbcc0'='@TestMatrix@12'; '60aad0'='_TestError' }
foreach($label in $extra.Keys) { $labels[$label]=$extra[$label] }
foreach($label in $labels.Keys) { $reference=$reference.Replace('FUN_'+$label,$labels[$label]);$bindings+='EXTERN '+$labels[$label]+":PROC`n" }
$variables=@{ '650430'='g_marineNodeBaseVtable'; '653040'='g_marineTaskVtable'; '653020'='g_marineTaskSecondaryVtable'; '685c24'='g_marineNodeName'; '672688'='g_marineCommonLibrary'; '67ba78'='g_marineRainNodeName'; '67c178'='g_marineZeroName'; '685bfc'='g_marineTaskFile'; '6724b0'='g_marineErrorFormat'; '6724f8'='g_marineDate'; '653018'='g_marineTaskRadiusSquared'; '725e08'='g_marineRecordedState'; '720908'='g_mt_frameValue'; '702620'='g_mt_axisX'; '702624'='g_mt_axisY'; '702628'='g_mt_axisZ' }
foreach($label in $variables.Keys) { $name=$variables[$label];$bindings+='EXTERN _'+$name+":BYTE`n"+'$L_'+$label+' TEXTEQU <_'+$name+">`n" }
# king.h already binds this shared container method to its MASM symbol.
$reference+="`nFUN_5d3570 PROC`n    jmp @TestRegister@12`nFUN_5d3570 ENDP`n"
$bindings+=".data`n"+'$L_64a011 DWORD 0'+"`n"
$asm = Join-Path $outputDir 'marine-construct-reference.asm'
Set-Content $asm (".686`n.model flat`nASSUME FS:NOTHING`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj = Join-Path $outputDir 'marine-construct-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-construct-reference.obj'
$testObj = Join-Path $outputDir 'marine-construct-test.obj'
$exe = Join-Path $outputDir 'marine-construct-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine constructor compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_construct.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine constructor differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine task construction matches MASM in 1,536 comparisons.'
