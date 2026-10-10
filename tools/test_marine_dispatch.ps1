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
$body = $source.Substring($source.IndexOf('// Marine task deferred bounds'),$source.IndexOf('// Marine task construction')-$source.IndexOf('// Marine task deferred bounds'))
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" int __fastcall MarineTask_SetBounds(BYTE*,void*,const DWORD*,const DWORD*);
extern "C" void __fastcall MarineTask_UpdateMotion(BYTE*,void*,const float*);
extern "C" void __fastcall MarineTask_Frame(BYTE*,void*,DWORD,DWORD);
KING_ALIAS("@MarineTask_SetBounds@16","@TestBounds@16")
KING_ALIAS("@MarineTask_UpdateMotion@12","@TestMotion@12")
KING_ALIAS("@MarineTask_Frame@16","@TestFrame@16")
KING_ALIAS("@MarineTask_DrawPending@20","@TestAdjust@20")
'@
$labels=@{ '5dbcc0'='@TestMatrix@12'; '5d32f0'='@TestEnable@12'; '5b4fc0'='@TestQuery@12'; '60ea90'='@TestAdjust@20' }
foreach($label in $labels.Keys) { $body=$body.Replace('ASM_PROC(FUN_'+$label+')','extern "C" void FUN_'+$label+'(); KING_ALIAS("_FUN_'+$label+'","'+$labels[$label]+'")') }
$cpp = Join-Path $outputDir 'marine-dispatch-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '$L_60e610'='@Reference_MarineTask_DeferredBounds@8'; '$L_60e740'='@Reference_MarineTask_DispatchUpdate@16'; 'FUN_4bdef0'='@Reference_VectorSet@20'; 'FUN_4bff80'='_Reference_LengthSq' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$bindings=''
foreach($label in $labels.Keys) { $reference=$reference.Replace('FUN_'+$label,$labels[$label]);$bindings+='EXTERN '+$labels[$label]+":PROC`n" }
$extra=@{ '60dd80'='@TestBounds@16'; '60de50'='@TestMotion@12'; '60df90'='@TestFrame@16' }
foreach($label in $extra.Keys) { $reference=$reference.Replace('FUN_'+$label,$extra[$label]);$bindings+='EXTERN '+$extra[$label]+":PROC`n" }
$variables=@{ '702620'='g_mt_axisX'; '702624'='g_mt_axisY'; '702628'='g_mt_axisZ'; '7025b8'='g_mt_motionState'; '720908'='g_mt_frameValue' }
foreach($label in $variables.Keys) { $name=$variables[$label];$bindings+='EXTERN _'+$name+":DWORD`n"+'$L_'+$label+' TEXTEQU <_'+$name+">`n" }
$asm = Join-Path $outputDir 'marine-dispatch-reference.asm'
Set-Content $asm (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj = Join-Path $outputDir 'marine-dispatch-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-dispatch-reference.obj'
$testObj = Join-Path $outputDir 'marine-dispatch-test.obj'
# Windows treats executable names containing "patch" as installer candidates.
$exe = Join-Path $outputDir 'marine-task-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine dispatcher compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_dispatch.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine dispatcher differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine deferred bounds and update dispatch match MASM in 6,144 comparisons.'
