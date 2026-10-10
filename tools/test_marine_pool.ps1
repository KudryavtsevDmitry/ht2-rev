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
$body = $source.Substring($source.IndexOf('// Marine particle initialization'),$source.IndexOf('// Marine mesh coordinate mapping')-$source.IndexOf('// Marine particle initialization')) + $source.Substring($source.IndexOf('// Marine particle pool allocation'))
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
KING_ALIAS("_rand","_TestRand")
KING_ALIAS("_malloc","_TestMalloc")
KING_ALIAS("_srand","_TestSrand")
extern "C" BYTE* g_marineApplicationState;
#define timeGetTime TestClock
extern "C" DWORD __stdcall TestClock();
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-pool-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_612f60'='@Reference_MarineParticlePool_BuildGeometry@16'; 'FUN_612e30'='@Reference_MarineParticlePool_Construct@24'; 'FUN_612eb0'='@Reference_MarineParticlePool_Add@16'; 'FUN_612f20'='@Reference_MarineParticlePool_Remove@12'; 'FUN_613280'='@Reference_MarineParticlePool_CanAdd@8'; 'FUN_6132c0'='@Reference_MarineParticlePool_SetFloor@12'; 'FUN_6132d0'='@Reference_MarineParticlePool_SetLifetime@12'; 'FUN_612cf0'='@Reference_MarineParticle_Start@16'; 'FUN_612da0'='@Reference_MarineParticle_SetLifetime@12'; 'FUN_612db0'='@Reference_MarineParticle_Update@12' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$reference += [regex]::Match($masm, '(?s)__ftol PROC.*?__ftol ENDP').Value
$bindings = "EXTERN _g_mp_scale:DWORD, _g_mp_randomUnit:DWORD, _g_mp_angularFactor:DWORD, _g_mp_seconds:DWORD`nEXTERN _TestRand:PROC, _TestClock@0:PROC, _TestMalloc:PROC, _TestSrand:PROC`nEXTERN _g_mpool_enabled:DWORD, _g_mpool_angle:DWORD, _g_mpool_colorScale:DWORD, _g_mpool_half:DWORD, _g_marineApplicationState:DWORD`n"
$reference=$reference.Replace('$L_725f78','_g_mp_scale').Replace('$L_64c254','_g_mp_randomUnit').Replace('$L_64c304','_g_mp_angularFactor').Replace('$L_64c948','_g_mp_seconds').Replace('$L_725e30','_g_mpool_enabled').Replace('$L_64c334','_g_mpool_angle').Replace('$L_64c48c','_g_mpool_colorScale').Replace('$L_64c258','_g_mpool_half').Replace('$L_6cec6c','_g_marineApplicationState').Replace('call _malloc','call _TestMalloc').Replace('call _srand','call _TestSrand').Replace('call _rand','call _TestRand').Replace('call DWORD PTR __imp__timeGetTime@0','call _TestClock@0')
$asm = Join-Path $outputDir 'marine-pool-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-pool-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-pool-reference.obj'
$testObj = Join-Path $outputDir 'marine-pool-test.obj'
$exe = Join-Path $outputDir 'marine-pool-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine particle pool compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_pool.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine particle pool differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine particle pool allocation, emission and geometry match MASM in 21,504 comparisons.'
