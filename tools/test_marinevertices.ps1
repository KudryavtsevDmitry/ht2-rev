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
$source=Get-Content (Join-Path $repo 'src\marinevertices.cpp') -Raw
$variables=[regex]::Matches($source,'ASM_VAR\((float|double|Vec3), (\w+), (\$L_[0-9a-f]+)\)')
$globals='#include "../src/king.h"'+"`n"+'#include "../src/vecmath.h"'+"`nextern `"C`" {`nint _fltused=0;`nfloat g_angleScale,g_vertexZero;`nfloat g_testSine[720],g_testCosine[720];`nVec3 g_zeroVec;`n"
$bindings="EXTERN _g_angleScale:DWORD,_g_vertexZero:DWORD,_g_testSine:DWORD,_g_testCosine:DWORD,_g_zeroVec:DWORD`n"+'$L_724960 TEXTEQU <_g_angleScale>'+"`n"+'$L_64c264 TEXTEQU <_g_vertexZero>'+"`n"+'$L_7243bc TEXTEQU <_g_testSine+1440>'+"`n"+'$L_724964 TEXTEQU <_g_testCosine+1440>'+"`n"+'$L_702650 TEXTEQU <_g_zeroVec>'+"`n"+'$L_702654 TEXTEQU <_g_zeroVec+4>'+"`n"+'$L_702658 TEXTEQU <_g_zeroVec+8>'+"`n"
$canon=@{}
foreach($v in $variables){$type=$v.Groups[1].Value;$name=$v.Groups[2].Value;$label=$v.Groups[3].Value;$globals+="$type $name;`n";$bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n";$canon[$label]=$name}
$bindings+='$L_725e7c TEXTEQU <_g_mv_normal+4>'+"`n"+'$L_725e80 TEXTEQU <_g_mv_normal+8>'+"`n"
$globals+="}`nstatic float* floatCoefficients[]={"+(($variables | Where-Object {$_.Groups[1].Value -eq 'float'} | ForEach-Object {'&'+$_.Groups[2].Value}) -join ',')+"};`n"
Set-Content (Join-Path $outputDir 'marinevertices-globals.h') $globals
$source=$source.Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"').Replace('#include "vecmath.h"','#include "../src/vecmath.h"')
$marine=Get-Content (Join-Path $repo 'src\marine.cpp') -Raw
$helpers="`nextern `"C`" {extern float g_angleScale,g_vertexZero;extern float g_testSine[720],g_testCosine[720];}`n"
foreach($name in @('MarineAngle_Sine','MarineAngle_Cosine')){$leaf=[regex]::Match($marine,'(?s)extern "C" double __cdecl '+$name+'[(]float angle.*?\n}').Value;$helpers+=$leaf.Replace('g_mc_64c264','g_vertexZero').Replace('g_mc_724960','g_angleScale').Replace('g_mc_sine.v','(g_testSine+360)').Replace('g_mc_cosine.v','(g_testCosine+360)')+"`n"}
$helpers+=[regex]::Match($marine,'(?s)X87 double MarineGrid_Index.*?\n}').Value+"`n"
foreach($name in @('MarineGrid_MapX','MarineGrid_MapY','MarineGrid_Unmap')){$helpers+=[regex]::Match($marine,'(?s)extern "C" (?:double|void) __fastcall '+$name+'[(]BYTE\* self.*?\n}').Value+"`n"}
$math=Get-Content (Join-Path $repo 'src\marinemath.cpp') -Raw
$projection=[regex]::Match($math,'(?s)extern "C" void __fastcall MarineGrid_ReflectionUV[(]BYTE\* self.*?\n}').Value
foreach($v in [regex]::Matches($math,'ASM_VAR\(([^,]+), (\w+), (\$L_[0-9a-f]+)\)')){if($canon.ContainsKey($v.Groups[3].Value)){$projection=$projection.Replace($v.Groups[2].Value,$canon[$v.Groups[3].Value])}}
$helpers+=$projection+"`n"
$grid=Get-Content (Join-Path $repo 'src\marinegrid.cpp') -Raw
$helpers+=[regex]::Match($grid,'(?s)extern "C" BYTE\* __fastcall MarineGrid_WriteVertex[(]BYTE\* self.*?\n}').Value+"`n"
$effects=Get-Content (Join-Path $repo 'src\aeffects.cpp') -Raw
foreach($name in @('Vec3_DivBy','Vec3_LengthSq')){$helpers+=[regex]::Match($effects,'(?s)extern "C" [^\n]+ '+$name+'[(].*?\n}').Value+"`n"}
$vec=Get-Content (Join-Path $repo 'src\vecmath.cpp') -Raw
$helpers+=[regex]::Match($vec,'(?s)static double SumSquares.*?\n}').Value+"`n"
foreach($name in @('Vec3_Normalized','Vec3_Dot','Vec3_Sub','Vec3_Scaled')){$helpers+=[regex]::Match($vec,'(?s)(?:Vec3\*|double) '+$name+'[(].*?\n}').Value+"`n"}
$cpp=Join-Path $outputDir 'marinevertices-actual.cpp';Set-Content $cpp ($source+$helpers)
$masm=Get-Content (Join-Path $repo 'king.masm') -Raw
$methods=@{'FUN_611530'='@Reference_MarineGrid_UpdateVertex@20';'$L_610f40'='@Reference_MarineGrid_EmitTriangle@24';'FUN_6120c0'='@Reference_ReflectionUV@32';'FUN_6114d0'='@Reference_WriteVertex@40';'FUN_4bdef0'='@Reference_SetThis@20';'FUN_4bff80'='_Reference_LengthSq';'FUN_4bff50'='_Reference_DivBy';'FUN_4bde00'='_Reference_Sub';'FUN_4bdd60'='_Reference_Scaled';'FUN_5ab400'='_Reference_Normalized';'FUN_610f00'='_Reference_Dot';'FUN_612010'='@Reference_MapX@12';'FUN_612040'='@Reference_MapY@12';'FUN_612070'='@Reference_Unmap@20';'FUN_60ba60'='_Reference_Sine';'FUN_60b9e0'='_Reference_Cosine'}
$reference=''
foreach($label in $methods.Keys){$reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n"}
foreach($label in $methods.Keys){$reference=$reference.Replace($label,$methods[$label])}
$reference=$reference.Replace('FUN_613280','@MarineParticlePool_CanAdd@8').Replace('FUN_612eb0','@MarineParticlePool_Add@16')
$bindings+="EXTERN @MarineParticlePool_CanAdd@8:PROC,@MarineParticlePool_Add@16:PROC`n"
$reference+=[regex]::Match($masm,'(?s)__ftol PROC.*?__ftol ENDP').Value
$asm=Join-Path $outputDir 'marinevertices-reference.asm';Set-Content $asm (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj=Join-Path $outputDir 'marinevertices-actual.obj';$referenceObj=Join-Path $outputDir 'marinevertices-reference.obj';$testObj=Join-Path $outputDir 'marinevertices-test.obj';$exe=Join-Path $outputDir 'marinevertices-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if($LASTEXITCODE){throw 'Marine vertices actual compile failed'}
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if($LASTEXITCODE){throw 'Marine vertices reference compile failed'}
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marinevertices.cpp')
if($LASTEXITCODE){throw 'Marine vertices test compile failed'}
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE){throw 'Marine vertices test link failed'}
& $exe
if($LASTEXITCODE){throw "Marine vertices differential failed: $LASTEXITCODE"}
