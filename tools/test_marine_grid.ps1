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
$body = $source.Substring($source.IndexOf('// Marine mesh coordinate mapping'))
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
'@
$cpp = Join-Path $outputDir 'marine-grid-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_612010'='@Reference_MarineGrid_MapX@12'; 'FUN_612040'='@Reference_MarineGrid_MapY@12'; 'FUN_612070'='@Reference_MarineGrid_Unmap@20'; 'FUN_612760'='@Reference_MarineGrid_Outside@12'; 'FUN_6127e0'='@Reference_MarineGrid_ClassifyTriangle@12'; '$L_612540'='_Reference_MarineGrid_Width'; 'FUN_612550'='_Reference_MarineGrid_Height' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$bindings="EXTERN _g_mg_xLimit:DWORD, _g_mg_yLimit:DWORD, _g_mg_width:DWORD, _g_mg_height:DWORD`n"
$reference=$reference.Replace('$L_65313c','_g_mg_xLimit').Replace('$L_653138','_g_mg_yLimit').Replace('$L_725e50','_g_mg_width').Replace('$L_725e54','_g_mg_height')
$asm = Join-Path $outputDir 'marine-grid-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-grid-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-grid-reference.obj'
$testObj = Join-Path $outputDir 'marine-grid-test.obj'
$exe = Join-Path $outputDir 'marine-grid-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine grid compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_grid.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine grid differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine grid mapping and triangle classification match MASM in 21,504 comparisons.'
