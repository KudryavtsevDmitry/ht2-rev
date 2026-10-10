$ErrorActionPreference='Stop'
$repo=Split-Path $PSScriptRoot -Parent
$make=Get-Content "$repo\Makefile" -Raw
$vc=[regex]::Match($make,'(?m)^VCDIR = (.+)\r?$').Groups[1].Value.Trim()
$sdk=[regex]::Match($make,'(?m)^SDKINC = (.+)\r?$').Groups[1].Value.Trim()
$out="$repo\build-test"
New-Item -ItemType Directory -Path $out -Force | Out-Null
$flags=@('/nologo','/c','/O2','/GS-','/Zl',"/I$vc\include","/I$sdk\um","/I$sdk\shared","/I$sdk\ucrt",'/arch:IA32')
$source=Get-Content "$repo\src\gridbase.cpp" -Raw
$globals='#include "../src/king.h"'+"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" {`nint _fltused=0;`n"
$bindings=''
foreach($v in [regex]::Matches($source,'ASM_VAR\(([^,]+),([^,]+),(\$L_[0-9a-f]+)\)')) {
 $type=$v.Groups[1].Value;$name=$v.Groups[2].Value;$label=$v.Groups[3].Value
 if($type -eq 'GridBaseTriple') {$type='DWORD';$globals+="$type ${name}[3];`n"}elseif($type -eq 'GridBaseLabel') {$type='DWORD';$globals+="$type ${name}[16];`n"}else{$globals+="$type $name;`n"}
 $bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"
}
$globals+="}`n"
Set-Content "$out\gridbase-globals.h" $globals
$source=$source.Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"')
Set-Content "$out\gridbase-actual.cpp" $source
$masm=Get-Content "$repo\king.masm" -Raw
$methods=@{'$L_607130'='@Reference_GridBase_SetCamera@16';'FUN_607190'='@Reference_GridBase_Destruct@8';'$L_607170'='@Reference_GridBase_Delete@12';'$L_6071c0'='@Reference_GridBase_DeleteAlternate@12';'FUN_607220'='@Reference_GridBase_SetCameraAndOrigin@20';'FUN_607350'='@Reference_TerrainCell_SetTriangles@16';'$L_607200'='_Reference_GridBase_InitScale';'$L_6072d0'='_Reference_GridBase_InitSecondaryScale';'$L_6072f0'='_Reference_GridBase_InitBlend';'$L_607320'='_Reference_GridBase_InitOrigin';'$L_6073f0'='_Reference_GridBase_InitPending'}
$reference=''
foreach($label in $methods.Keys){$reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n"}
foreach($label in $methods.Keys){$reference=$reference.Replace($label,$methods[$label])}
$calls=@{'??2@YAPAXI@Z'='_TestAllocate';'??3@YAXPAX@Z'='_TestDelete'}
foreach($label in $calls.Keys){$reference=$reference.Replace($label,$calls[$label]);$bindings+="EXTERN $($calls[$label]):PROC`n"}
$bindings+='$L_72437c TEXTEQU <_g_gb_origin+4>'+"`n"+'$L_724380 TEXTEQU <_g_gb_origin+8>'+"`n"
Set-Content "$out\gridbase-reference.asm" (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$bin="$vc\bin\HostX86\x86"
& "$bin\cl.exe" @flags "/Fo$out\gridbase-actual.obj" "$out\gridbase-actual.cpp"
if($LASTEXITCODE){throw 'Actual compile failed'}
& "$bin\ml.exe" /nologo /c "/Fo$out\gridbase-reference.obj" "$out\gridbase-reference.asm"
if($LASTEXITCODE){throw 'Reference compile failed'}
& "$bin\cl.exe" @flags "/Fo$out\gridbase-test.obj" "$repo\tools\test_gridbase.cpp"
if($LASTEXITCODE){throw 'Test compile failed'}
$sdkLib=Join-Path (Split-Path (Split-Path $sdk -Parent) -Parent) ('Lib\'+(Split-Path $sdk -Leaf)+'\um\x86')
& "$bin\link.exe" /nologo "$out\gridbase-actual.obj" "$out\gridbase-reference.obj" "$out\gridbase-test.obj" /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$out\gridbase-test.exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE){throw 'Test link failed'}
& "$out\gridbase-test.exe"
if($LASTEXITCODE){throw "Grid differential failed: $LASTEXITCODE"}
