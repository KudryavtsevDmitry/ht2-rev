$ErrorActionPreference='Stop'
$repo=Split-Path $PSScriptRoot -Parent
$make=Get-Content "$repo\Makefile" -Raw
$vc=[regex]::Match($make,'(?m)^VCDIR = (.+)\r?$').Groups[1].Value.Trim()
$sdk=[regex]::Match($make,'(?m)^SDKINC = (.+)\r?$').Groups[1].Value.Trim()
$out="$repo\build-test"
New-Item -ItemType Directory -Path $out -Force | Out-Null
$flags=@('/nologo','/c','/O2','/GS-','/Zl',"/I$vc\include","/I$sdk\um","/I$sdk\shared","/I$sdk\ucrt",'/arch:IA32')
$source=Get-Content "$repo\src\gridtopology.cpp" -Raw
$globals='#include "../src/king.h"'+"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" {`nint _fltused=0;`n"
$bindings=''
foreach($v in [regex]::Matches($source,'ASM_VAR\(([^,]+),([^,]+),(\$L_[0-9a-f]+)\)')) {
 $type=$v.Groups[1].Value;$name=$v.Groups[2].Value;$label=$v.Groups[3].Value
 if($type -eq 'TopologyLabel') {$type='DWORD';$globals+="$type ${name}[16];`n"}else{$globals+="$type $name;`n"}
 $bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"
}
$globals+="}`n"
Set-Content "$out\gridtopology-globals.h" $globals
$source=$source.Replace('#include "king.h"','#include "../src/king.h"').Replace('#include "x87.h"','#include "../src/x87.h"')
Set-Content "$out\gridtopology-actual.cpp" $source
$masm=Get-Content "$repo\king.masm" -Raw
$methods=@{'FUN_606100'='@Reference_GridEdge_Construct@8';'FUN_606170'='@Reference_GridEdge_SetPairs@16';'FUN_6062c0'='@Reference_GridMesh_RemoveTriangle@12';'FUN_606330'='@Reference_GridMesh_CreateTriangle@24';'FUN_6063f0'='@Reference_GridTriangle_Destruct@8';'FUN_606400'='@Reference_GridMesh_CreatePoint@12';'FUN_606440'='@Reference_GridMesh_RemoveEdge@12';'FUN_606750'='@Reference_GridEdge_Destruct@8';'FUN_6069d0'='@Reference_GridBranch_Destruct@8';'FUN_61c1b0'='@Reference_GridTriangle_SetPoints@24'}
$reference=''
foreach($label in $methods.Keys){$reference+=[regex]::Match($masm,'(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP').Value+"`n"}
foreach($label in $methods.Keys){$reference=$reference.Replace($label,$methods[$label])}
$calls=@{'??2@YAPAXI@Z'='_TestAllocate';'??3@YAXPAX@Z'='_TestDelete';'FUN_60aad0'='_TestError';'FUN_60f520'='@MarineQueue_InsertNode@12';'FUN_60f690'='@MarineQueue_RemoveNode@12';'FUN_6136f0'='@MarineQueue_RemoveLink@12'}
foreach($label in $calls.Keys){$reference=$reference.Replace($label,$calls[$label]);$bindings+="EXTERN $($calls[$label]):PROC`n"}
Set-Content "$out\gridtopology-reference.asm" (".686`n.model flat`n"+$bindings+".code`nPUBLIC FUN_60aad0`nFUN_60aad0 PROC`njmp _TestError`nFUN_60aad0 ENDP`n"+$reference+"`nEND")
$bin="$vc\bin\HostX86\x86"
& "$bin\cl.exe" @flags "/Fo$out\gridtopology-actual.obj" "$out\gridtopology-actual.cpp"
if($LASTEXITCODE){throw 'Actual compile failed'}
& "$bin\ml.exe" /nologo /c "/Fo$out\gridtopology-reference.obj" "$out\gridtopology-reference.asm"
if($LASTEXITCODE){throw 'Reference compile failed'}
& "$bin\cl.exe" @flags "/Fo$out\gridtopology-test.obj" "$repo\tools\test_gridtopology.cpp"
if($LASTEXITCODE){throw 'Test compile failed'}
$sdkLib=Join-Path (Split-Path (Split-Path $sdk -Parent) -Parent) ('Lib\'+(Split-Path $sdk -Leaf)+'\um\x86')
& "$bin\link.exe" /nologo "$out\gridtopology-actual.obj" "$out\gridtopology-reference.obj" "$out\gridtopology-test.obj" /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$out\gridtopology-test.exe" "/LIBPATH:$sdkLib" kernel32.lib
if($LASTEXITCODE){throw 'Test link failed'}
& "$out\gridtopology-test.exe"
if($LASTEXITCODE){throw "Grid differential failed: $LASTEXITCODE"}
