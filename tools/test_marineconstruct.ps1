$ErrorActionPreference='Stop'
$repo=Split-Path $PSScriptRoot -Parent
$make=Get-Content "$repo\Makefile" -Raw
$vc=[regex]::Match($make,'(?m)^VCDIR = (.+)\r?$').Groups[1].Value.Trim()
$sdk=[regex]::Match($make,'(?m)^SDKINC = (.+)\r?$').Groups[1].Value.Trim()
$out="$repo\build-test"
New-Item -ItemType Directory -Path $out -Force | Out-Null
$flags=@('/nologo','/c','/O2','/GS-','/Zl',"/I$vc\include","/I$sdk\um","/I$sdk\shared","/I$sdk\ucrt",'/arch:IA32')
$source=Get-Content "$repo\src\marineconstruct.cpp" -Raw
$globals='#include "../src/king.h"'+"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" {`nint _fltused=0;`n"
$bindings=''
foreach($v in [regex]::Matches($source,'ASM_VAR\(([^,]+),([^,]+),(\$L_[0-9a-f]+)\)')) {
 $type=$v.Groups[1].Value;$name=$v.Groups[2].Value;$label=$v.Groups[3].Value
 if($type -eq 'ConstructLabel') {$type='DWORD';$globals+="$type ${name}[16];`n"}else{$globals+="$type $name;`n"}
 $bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"
}
$globals+="}`n"
Set-Content "$out\marineconstruct-globals.h" $globals
$source=$source.Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"')
Set-Content "$out\marineconstruct-actual.cpp" $source
$masm=Get-Content "$repo\king.masm" -Raw
$methods=@{'FUN_607090'='@Reference_MarineGrid_ConstructBase@8';'FUN_605840'='@Reference_MarineMesh_SetCellTriangles@16';'FUN_60fee0'='@Reference_MarineMesh_ConstructGrid@24'}
$reference=''
foreach($label in $methods.Keys){$reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n"}
foreach($label in $methods.Keys){$reference=$reference.Replace($label,$methods[$label])}
$calls=@{'FUN_60d320'='@MarineGreed_Allocate@16';'FUN_60d3a0'='@MarineGreed_Init@8';'FUN_60d440'='@MarineGreed_Update@8';'FUN_60f520'='@MarineQueue_InsertNode@12';'FUN_612970'='@MarineGrid_Trim@12';'FUN_613860'='_MarineGrid_BuildRandomDirections';'FUN_60aad0'='_TestError';'_ceil'='_TestCeil';'_malloc'='_TestAllocate';'_free'='_TestFree';'??2@YAPAXI@Z'='_TestAllocate'}
foreach($label in $calls.Keys){$reference=$reference.Replace($label,$calls[$label]);$bindings+="EXTERN $($calls[$label]):PROC`n"}
$bindings+="EXTERN __imp__timeGetTime@0:DWORD`n"+'$L_64a0c3 TEXTEQU <_TestHandler>'+"`n"
$reference+=[regex]::Match($masm,'(?s)__ftol PROC.*?__ftol ENDP').Value
Set-Content "$out\marineconstruct-reference.asm" (".686`n.model flat`nASSUME FS:NOTHING`n"+$bindings+".code`nPUBLIC FUN_60aad0`nFUN_60aad0 PROC`njmp _TestError`nFUN_60aad0 ENDP`n_TestHandler PROC`nret`n_TestHandler ENDP`n"+$reference+"`nEND")
$bin="$vc\bin\HostX86\x86"
& "$bin\cl.exe" @flags "/Fo$out\marineconstruct-actual.obj" "$out\marineconstruct-actual.cpp"
if($LASTEXITCODE){throw 'Actual compile failed'}
& "$bin\ml.exe" /nologo /c "/Fo$out\marineconstruct-reference.obj" "$out\marineconstruct-reference.asm"
if($LASTEXITCODE){throw 'Reference compile failed'}
& "$bin\cl.exe" @flags "/Fo$out\marineconstruct-test.obj" "$repo\tools\test_marineconstruct.cpp"
if($LASTEXITCODE){throw 'Test compile failed'}
$sdkLib=Join-Path (Split-Path (Split-Path $sdk -Parent) -Parent) ('Lib\'+(Split-Path $sdk -Leaf)+'\um\x86')
& "$bin\link.exe" /nologo "$out\marineconstruct-actual.obj" "$out\marineconstruct-reference.obj" "$out\marineconstruct-test.obj" /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$out\marineconstruct-test.exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE){throw 'Test link failed'}
& "$out\marineconstruct-test.exe"
if($LASTEXITCODE){throw "Grid differential failed: $LASTEXITCODE"}
