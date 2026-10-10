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
$body = $source.Substring($source.IndexOf('// Marine pending-state quad'),$source.IndexOf('// Marine particle initialization')-$source.IndexOf('// Marine pending-state quad'))
$body=$body.Replace('ASM_PROC(FUN_5b1020)','extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020","@TestBind@24")')
$header = @'
#include "../src/vecmath.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" DWORD g_mt_frameValue;
'@
$effects=Get-Content (Join-Path $repo 'src\aeffects.cpp') -Raw
$helpers=''
foreach($name in @('Diff','Sum','ScaleBy','Times')) {
    $helpers += [regex]::Match($effects,'(?s)extern "C" Vec3\* __cdecl Vec3_'+$name+'[(].*?\n[}]').Value+"`n"
}
$setBody=[regex]::Match($effects,'(?s)BYTE\* FxMethods::Set2Vec[(].*?\n[}]').Value
$setBody=$setBody.Substring($setBody.IndexOf('{')).Replace('SELF','self')
$helpers+='extern "C" BYTE* __fastcall Fx_Set2Vec(BYTE* self,void*,const Vec3* a,const Vec3* b,int c,int d)'+"`n"+$setBody+"`n"
$cpp=Join-Path $outputDir 'marine-quad-actual.cpp'
Set-Content $cpp ($header+"`n"+$helpers+$body)
$masm=Get-Content (Join-Path $repo 'king.masm') -Raw
$reference=''
$methods=@{ 'FUN_60ea90'='@Reference_MarineTask_DrawPending@20'; 'FUN_5eb490'='@Reference_MarineCamera_HorizontalAngle@8'; 'FUN_5eb4a0'='@Reference_MarineCamera_VerticalAngle@8'; 'FUN_4bdef0'='@Reference_VectorSet@20'; 'FUN_4bdeb0'='@Reference_Set2Vec@24'; 'FUN_4bfef0'='_Reference_Sum'; 'FUN_4be230'='_Reference_Diff'; 'FUN_4c03a0'='_Reference_Times' }
foreach($label in $methods.Keys) { $reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label,$methods[$label]) }
$reference=$reference.Replace('FUN_5b1020','@TestBind@24')
$bindings='EXTERN @TestBind@24:PROC'+"`n"
$variables=@{ '64c264'='g_mq_zero'; '64c250'='g_mq_one'; '64c258'='g_mq_half'; '653008'='g_mq_delay'; '65308c'='g_mq_offsetScale'; '65300c'='g_mq_height'; '653010'='g_mq_lowerHeight'; '720908'='g_mt_frameValue' }
foreach($label in $variables.Keys) { $name=$variables[$label];$bindings+='EXTERN _'+$name+":DWORD`n"+'$L_'+$label+' TEXTEQU <_'+$name+">`n" }
$asm=Join-Path $outputDir 'marine-quad-reference.asm'
Set-Content $asm (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj=Join-Path $outputDir 'marine-quad-actual.obj'
$referenceObj=Join-Path $outputDir 'marine-quad-reference.obj'
$testObj=Join-Path $outputDir 'marine-quad-test.obj'
$exe=Join-Path $outputDir 'marine-quad-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if($LASTEXITCODE) {throw 'Marine quad compilation failed'}
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if($LASTEXITCODE) {throw 'Reference assembly failed'}
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_quad.cpp')
if($LASTEXITCODE) {throw 'Test compilation failed'}
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE) {throw 'Test link failed'}
& $exe
if($LASTEXITCODE) {throw "Marine quad differential test failed: exit code $LASTEXITCODE"}
Write-Output 'Marine quad rendering and camera angles match MASM in 9,216 comparisons.'
