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
$source=Get-Content (Join-Path $repo 'src\marinemath.cpp') -Raw
$variables=[regex]::Matches($source,'ASM_VAR\((float|double), (\w+), (\$L_[0-9a-f]+)\)')
$globals='#include "../src/king.h"'+"`n"+'#include "../src/vecmath.h"'+"`nextern `"C`" {`nint _fltused=0;`nfloat g_angleScale;`nfloat g_testSine[720],g_testCosine[720];`nVec3 g_zeroVec;`n"
$bindings="EXTERN _g_angleScale:DWORD,_g_testSine:DWORD,_g_testCosine:DWORD,_g_zeroVec:DWORD`n"+'$L_724960 TEXTEQU <_g_angleScale>'+"`n"+'$L_7243bc TEXTEQU <_g_testSine+1440>'+"`n"+'$L_724964 TEXTEQU <_g_testCosine+1440>'+"`n"+'$L_702650 TEXTEQU <_g_zeroVec>'+"`n"+'$L_702654 TEXTEQU <_g_zeroVec+4>'+"`n"+'$L_702658 TEXTEQU <_g_zeroVec+8>'+"`n"
foreach($v in $variables) {$type=$v.Groups[1].Value;$name=$v.Groups[2].Value;$label=$v.Groups[3].Value;$globals+="$type $name;`n";$bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"}
$globals+="}`nstatic float* floatCoefficients[]={"+(($variables | Where-Object {$_.Groups[1].Value -eq 'float'} | ForEach-Object {'&'+$_.Groups[2].Value}) -join ',')+"};`n"
Set-Content (Join-Path $outputDir 'marinemath-globals.h') $globals
$source=$source.Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"').Replace('#include "vecmath.h"','#include "../src/vecmath.h"')
$marine=Get-Content (Join-Path $repo 'src\marine.cpp') -Raw
$helpers="`nextern `"C`" {extern float g_angleScale;extern float g_testSine[720],g_testCosine[720];}`n"
foreach($name in @('MarineAngle_Sine','MarineAngle_Cosine')) {
 $leaf=[regex]::Match($marine,'(?s)extern "C" double __cdecl '+$name+'[(]float angle.*?\n}').Value
 $helpers+=$leaf.Replace('g_mc_64c264','g_mm_zero').Replace('g_mc_724960','g_angleScale').Replace('g_mc_sine.v','(g_testSine+360)').Replace('g_mc_cosine.v','(g_testCosine+360)')+"`n"
}
$helpers+=[regex]::Match($marine,'(?s)X87 double MarineGrid_Index.*?\n}').Value+"`n"
$helpers+=[regex]::Match($marine,'(?s)extern "C" double __fastcall MarineGrid_MapY[(]BYTE\* self.*?\n}').Value+"`n"
$effects=Get-Content (Join-Path $repo 'src\aeffects.cpp') -Raw
foreach($name in @('Vec3_Diff','Vec3_DivBy','Vec3_LengthSq')) {$helpers+=[regex]::Match($effects,'(?s)extern "C" [^\n]+ '+$name+'[(].*?\n}').Value+"`n"}
$vec=Get-Content (Join-Path $repo 'src\vecmath.cpp') -Raw
$helpers+=[regex]::Match($vec,'(?s)static double SumSquares.*?\n}').Value+"`n"
foreach($name in @('Vec3_Normalized','Vec3_Dot')) {$helpers+=[regex]::Match($vec,'(?s)(?:Vec3\*|double) '+$name+'[(].*?\n}').Value+"`n"}
$cpp=Join-Path $outputDir 'marinemath-actual.cpp';Set-Content $cpp ($source+$helpers)
$masm=Get-Content (Join-Path $repo 'king.masm') -Raw
$methods=@{'$L_610960'='@Reference_MarineGrid_TriangleWeight@24';'$L_610b70'='@Reference_MarineGrid_SurfaceWeight@40';'FUN_6120c0'='@Reference_MarineGrid_ReflectionUV@32';'$L_611e60'='@Reference_MarineGrid_RippleHeight@12';'FUN_6057b0'='@Reference_MarineMaterial_UpdateFrame@12';'$L_611ff0'='@Reference_MarineGrid_SetTime@12';'$L_6137f0'='_Reference_MarineGrid_InitDiameter';'$L_613820'='_Reference_MarineGrid_InitRandomDiameter';'FUN_4bdef0'='@Reference_SetThis@20';'FUN_4bff80'='_Reference_LengthSq';'FUN_4bff50'='_Reference_DivBy';'FUN_4be230'='_Reference_Diff';'FUN_5ab400'='_Reference_Normalized';'FUN_610f00'='_Reference_Dot';'FUN_612040'='@Reference_MapY@12';'FUN_60ba60'='_Reference_Sine';'FUN_60b9e0'='_Reference_Cosine'}
$reference=''
foreach($label in $methods.Keys){$reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n"}
foreach($label in $methods.Keys){$reference=$reference.Replace($label,$methods[$label])}
$reference+=[regex]::Match($masm,'(?s)__ftol PROC.*?__ftol ENDP').Value
$asm=Join-Path $outputDir 'marinemath-reference.asm';Set-Content $asm (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj=Join-Path $outputDir 'marinemath-actual.obj';$referenceObj=Join-Path $outputDir 'marinemath-reference.obj';$testObj=Join-Path $outputDir 'marinemath-test.obj';$exe=Join-Path $outputDir 'marinemath-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if($LASTEXITCODE){throw 'Marine math actual compile failed'}
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if($LASTEXITCODE){throw 'Marine math reference compile failed'}
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marinemath.cpp')
if($LASTEXITCODE){throw 'Marine math test compile failed'}
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE){throw 'Marine math test link failed'}
& $exe
if($LASTEXITCODE){throw "Marine math differential failed: $LASTEXITCODE"}
