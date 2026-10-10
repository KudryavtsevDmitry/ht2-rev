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
$body = $source.Substring($source.IndexOf('// Marine 4x4 matrix'),$source.IndexOf('// Marine particle rendering')-$source.IndexOf('// Marine 4x4 matrix'))
$init = Get-Content (Join-Path $repo 'src\marineinit.cpp') -Raw
$body += [regex]::Match($init,'(?s)extern "C" DWORD\* __cdecl MarineInit_Identity.*?\n}').Value
foreach($name in @('MarineParticle_Update','MarineParticlePool_Remove')) {
    $body += [regex]::Match($source,'(?s)extern "C" [^\n]+ '+$name+'[(].*?\n}').Value+"`n"
}
$body += $source.Substring($source.IndexOf('// Marine particle rendering'))
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
extern "C" DWORD g_mpool_enabled;
extern "C" float g_mp_seconds;
extern "C" void FUN_5b1020(); KING_ALIAS("_FUN_5b1020","@TestBind@24")
#define timeGetTime TestClock
extern "C" DWORD __stdcall TestClock();
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-render-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_60a880'='_Reference_MarineMatrix_Zero'; 'FUN_60a900'='_Reference_MarineMatrix_RotateZ'; 'FUN_60a960'='_Reference_MarineMatrix_Translate'; 'FUN_60a9b0'='_Reference_MarineMatrix_Multiply'; 'FUN_60a8b0'='_Reference_MarineInit_Identity'; 'FUN_613120'='@Reference_MarineParticlePool_Render@20'; 'FUN_612f20'='@Reference_MarineParticlePool_Remove@12'; 'FUN_612db0'='@Reference_MarineParticle_Update@12' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$reference=$reference.Replace('FUN_5b1020','@TestBind@24').Replace('call DWORD PTR __imp__timeGetTime@0','call _TestClock@0').Replace('$L_725e30','_g_mpool_enabled').Replace('$L_725f38','_g_mpool_restoreMatrix').Replace('$L_64c948','_g_mp_seconds')
$bindings="EXTERN @TestBind@24:PROC, _TestClock@0:PROC`nEXTERN _g_mpool_enabled:DWORD, _g_mpool_restoreMatrix:DWORD, _g_mp_seconds:DWORD`n"
$asm = Join-Path $outputDir 'marine-render-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-render-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-render-reference.obj'
$testObj = Join-Path $outputDir 'marine-render-test.obj'
$exe = Join-Path $outputDir 'marine-render-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine particle rendering compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_render.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine particle rendering differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine particle rendering matches MASM in 3,072 state and callback comparisons.'
