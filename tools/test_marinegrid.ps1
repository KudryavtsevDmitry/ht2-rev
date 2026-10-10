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
$source=Get-Content (Join-Path $repo 'src\marinegrid.cpp') -Raw
$variables=[regex]::Matches($source,'ASM_VAR\(([^,]+), (\w+), (\$L_[0-9a-f]+)\)')
$globals=$source.Substring(0,$source.IndexOf('ASM_PROC(FUN_5e48f0)')).Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"')
$globals=[regex]::Replace($globals,'ASM_VAR\(([^,]+), (\w+), (\$L_[0-9a-f]+)\)', {param($m) 'extern "C" {'+$m.Groups[1].Value+' '+$m.Groups[2].Value+';}'})
$globals+='extern "C" {int _fltused=0; DWORD g_noise[2600];}'
$noise=@{'g_gridNoise'=0}
$bindings=''
foreach($v in $variables) {
 $type=$v.Groups[1].Value;$name=$v.Groups[2].Value;$label=$v.Groups[3].Value
 if($noise.ContainsKey($name)) {
  $macro='#define '+$name+' (*('+ $type+'*)(g_noise+'+$noise[$name]+'))'
  $globals=$globals.Replace('extern "C" {'+$type+' '+$name+';}',$macro+"`n")
  $source=$source.Replace($v.Value,$macro)
  $bindings+=$label+' TEXTEQU <_g_noise+'+($noise[$name]*4)+">`n"
 } else {$bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"}
}
$bindings+='$L_726bd0 TEXTEQU <_g_noise+3072>'+"`n"+'$L_7277e8 TEXTEQU <_g_noise+6168>'+"`n"+'$L_727be8 TEXTEQU <_g_noise+7192>'+"`n"+'$L_727ff0 TEXTEQU <_g_noise+8224>'+"`n"+'$L_7283f0 TEXTEQU <_g_noise+9248>'+"`n"+"EXTERN _g_noise:DWORD`n"+'$L_727fec TEXTEQU <_g_noise+8220>'+"`n"+'$L_7283ec TEXTEQU <_g_noise+9244>'+"`n"
Set-Content (Join-Path $outputDir 'marinegrid-globals.h') $globals
$source=$source.Replace('operator delete(','TestDelete(').Replace('operator new(','TestAllocate(').Replace('malloc(','TestMalloc(').Replace('srand(','TestSrand(').Replace('rand()','TestRand()')
$source='extern "C" {void TestDelete(void*);void* TestAllocate(unsigned);void* TestMalloc(unsigned);void TestSrand(unsigned);int TestRand();extern unsigned long g_noise[2600];}'+"`n"+$source
$cpp=Join-Path $outputDir 'marinegrid-actual.cpp';Set-Content $cpp $source.Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"')
$masm=Get-Content (Join-Path $repo 'king.masm') -Raw
$methods=@{'$L_610840'='@Reference_MarineGrid_Delete@12';'FUN_610860'='@Reference_MarineGrid_Destruct@8';'FUN_6114d0'='@Reference_MarineGrid_WriteVertex@40';'FUN_6122d0'='@Reference_MarineGrid_Allocate@16';'$L_612420'='@Reference_MarineGrid_Draw@16';'$L_612560'='@Reference_MarineGrid_DesiredWork@8';'$L_6125d0'='@Reference_MarineGrid_TrackCamera@16';'FUN_612880'='@Reference_MarineGrid_TrimBranch@12';'FUN_612970'='@Reference_MarineGrid_Trim@12';'FUN_613810'='_Reference_MarineGrid_QualityScale';'FUN_613860'='_Reference_MarineGrid_BuildRandomDirections'}
$reference=''
foreach($label in $methods.Keys) {$reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n"}
foreach($label in $methods.Keys) {$reference=$reference.Replace($label,$methods[$label])}
$stubs=@{'??3@YAXPAX@Z'='_TestDelete';'??2@YAPAXI@Z'='_TestAllocate';'_malloc'='_TestMalloc';'_srand'='_TestSrand';'_rand'='_TestRand';'FUN_5e48f0'='_TestNoop';'FUN_60bb20'='_TestReset';'FUN_60aad0'='_TestError';'FUN_60aa60'='_TestAssert';'FUN_605810'='@TestMaterial@8';'FUN_5b1020'='@TestBind@24';'FUN_605900'='_TestSplit';'FUN_6064c0'='@TestMerge@16';'FUN_612e30'='@MarineParticlePool_Construct@24';'FUN_6132c0'='@MarineParticlePool_SetFloor@12';'FUN_6132d0'='@MarineParticlePool_SetLifetime@12';'FUN_613120'='@MarineParticlePool_Render@20';'FUN_6127e0'='@MarineGrid_ClassifyTriangle@12';'FUN_60d540'='@MarineGreed_RemoveNode@12';'FUN_60d750'='@MarineMesh_DrainEdges@8';'FUN_60d780'='@MarineMesh_PruneVertices@8'}
foreach($label in $stubs.Keys) {$reference=$reference.Replace($label,$stubs[$label]);$bindings+='EXTERN '+$stubs[$label]+":PROC`n"}
# Inline eligibility comparison is shared by the C++ branch port.
$reference=$reference.Replace('call FUN_60a850','call _TestEligible')
$bindings+="EXTERN _TestEligible:PROC`n"
foreach($label in $stubs.Keys) {if($label.StartsWith('FUN_')) {$reference+='PUBLIC '+$label+"`n"+$label+': jmp '+$stubs[$label]+"`n"}}
$reference+=[regex]::Match($masm,'(?s)__ftol PROC.*?__ftol ENDP').Value
$asm=Join-Path $outputDir 'marinegrid-reference.asm';Set-Content $asm (".686`n.model flat`n"+$bindings+".code`nASSUME FS:NOTHING`n"+$reference+"`n"+'$L_64a0d8: ret'+"`n"+'$L_64a0fb: ret'+"`nEND")
$actualObj=Join-Path $outputDir 'marinegrid-actual.obj';$referenceObj=Join-Path $outputDir 'marinegrid-reference.obj';$testObj=Join-Path $outputDir 'marinegrid-test.obj';$exe=Join-Path $outputDir 'marinegrid-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if($LASTEXITCODE){throw 'Grid actual compile failed'}
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if($LASTEXITCODE){throw 'Grid reference compile failed'}
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marinegrid.cpp')
if($LASTEXITCODE){throw 'Grid test compile failed'}
& $linker /nologo $actualObj $referenceObj $testObj /MAP:build-test\marinegrid.map /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE){throw 'Grid test link failed'}
& $exe
if($LASTEXITCODE){throw "Grid differential failed: $LASTEXITCODE"}
