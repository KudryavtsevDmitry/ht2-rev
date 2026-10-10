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
$body=$source.Substring($source.IndexOf('// Marine mesh motion-response construction'))
$variables=[regex]::Matches($body,'ASM_VAR\(float, (\w+), (\$L_[0-9a-f]+)\)')
$globals="#include `"../src/king.h`"`nstruct MarineAngleTable {float v[360];};`nstruct MarineTableStorage {float before[360];MarineAngleTable table;};`nextern `"C`" {`nint _fltused=0;`nMarineTableStorage g_testSine,g_testCosine;`n"
$bindings="EXTERN _TestAllocate:PROC`n"
foreach($v in $variables) {
    $name=$v.Groups[1].Value;$label=$v.Groups[2].Value
    $globals+="float $name;`n";$bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"
}
$globals+="}`nstatic float* coefficients[]={"+(($variables | ForEach-Object {'&'+$_.Groups[1].Value}) -join ',')+"};`n"
Set-Content (Join-Path $outputDir 'marine-response-globals.h') $globals
$header=@('#include "../src/king.h"','#include "../src/x87.h"','#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))','struct MarineAngleTable {float v[360];};','struct MarineTableStorage {float before[360];MarineAngleTable table;};','extern "C" MarineTableStorage g_testSine,g_testCosine;','extern "C" void* __cdecl TestAllocate(size_t);') -join "`n"
$body=$body.Replace('struct MarineAngleTable { float v[360]; };','').Replace('ASM_VAR(MarineAngleTable, g_mc_sine, $L_7243bc)','#define g_mc_sine (g_testSine.table)').Replace('ASM_VAR(MarineAngleTable, g_mc_cosine, $L_724964)','#define g_mc_cosine (g_testCosine.table)').Replace('operator new(','TestAllocate(')
$leaf=[regex]::Match($source,'(?s)extern "C" DWORD __fastcall MarineTransition_SetActive[(]BYTE\* self.*?\n}').Value
$cpp=Join-Path $outputDir 'marine-response-actual.cpp'
Set-Content $cpp ($header+"`n"+$leaf+"`n"+$body)
$masm=Get-Content (Join-Path $repo 'king.masm') -Raw
$methods=@{'FUN_60fc10'='@Reference_MarineMesh_CreateMotionResponse@20';'FUN_60ba60'='_Reference_MarineAngle_Sine';'FUN_60b9e0'='_Reference_MarineAngle_Cosine';'FUN_6132e0'='@Reference_TransitionSetActive@12';'FUN_4bff80'='_Reference_LengthSquared'}
$reference=''
foreach($label in $methods.Keys) {$reference+=[regex]::Match($masm,'(?s)'+$label+' PROC.*?'+$label+' ENDP').Value+"`n"}
foreach($label in $methods.Keys) {$reference=$reference.Replace($label,$methods[$label])}
$reference=$reference.Replace('??2@YAPAXI@Z','_TestAllocate')
$reference+=[regex]::Match($masm,'(?s)__ftol PROC.*?__ftol ENDP').Value
$bindings+="EXTERN _g_testSine:DWORD, _g_testCosine:DWORD`n"+'$L_7243bc TEXTEQU <_g_testSine+1440>'+"`n"+'$L_724964 TEXTEQU <_g_testCosine+1440>'+"`n"
$asm=Join-Path $outputDir 'marine-response-reference.asm'
Set-Content $asm (".686`n.model flat`n"+$bindings+".code`nASSUME FS:NOTHING`n"+$reference+"`n"+'$L_64a09b: ret'+"`nEND")
$actualObj=Join-Path $outputDir 'marine-response-actual.obj'
$referenceObj=Join-Path $outputDir 'marine-response-reference.obj'
$testObj=Join-Path $outputDir 'marine-response-test.obj'
$exe=Join-Path $outputDir 'marine-response-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine response compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_response.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine response differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine response construction and angle tables match MASM in 9,216 comparisons.'
