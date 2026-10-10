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
$body = $source.Substring($source.IndexOf('// Marine object lifetime'), $source.IndexOf('// Marine graph ownership')-$source.IndexOf('// Marine object lifetime'))
$body=$body.Replace('extern "C" void __fastcall MarineGreed_Destruct(BYTE*, void*);','extern "C" void __fastcall MarineGreed_Destruct(BYTE*, void*); KING_ALIAS("@MarineGreed_Destruct@8","@TestMeshBase@8")')
$body=$body.Replace('extern "C" DWORD* __cdecl MarineGeometry_Interpolate(DWORD*, const DWORD*, const DWORD*, float);','extern "C" DWORD* __cdecl MarineGeometry_Interpolate(DWORD*, const DWORD*, const DWORD*, float); KING_ALIAS("_MarineGeometry_Interpolate","_TestInterpolate")')
$release=[regex]::Match($source,'(?s)extern "C" void __fastcall MarineTask_ReleaseMesh[(].*?\n[}]').Value
$body=$release+"`n"+$body
$labels=@{ '60fee0'='@TestSetup@24'; '60fc10'='@TestMotion@20'; '60cdd0'='@TestMeshBase@8'; '5ef780'='@TestDetach@8'; '5d3290'='@TestSecondaryBase@8'; '5ef660'='@TestTaskBase@8'; '61c350'='_TestInterpolate' }
foreach($label in $labels.Keys) { $body=$body.Replace('ASM_PROC(FUN_'+$label+')', 'extern "C" void FUN_'+$label+'(); KING_ALIAS("_FUN_'+$label+'", "'+$labels[$label]+'")') }
$header = "#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`ntypedef char AsmLabel[1];`nextern `"C`" unsigned __stdcall TestTime();`n#define timeGetTime TestTime`n"
$cpp = Join-Path $outputDir 'marine-lifetime-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_60db70'=@('MarineMesh_Create',36); 'FUN_60dce0'=@('MarineMesh_Destruct',8); '$L_60dcc0'=@('MarineMesh_Delete',12); 'FUN_60e4a0'=@('MarineTask_Destruct',8); '$L_60e480'=@('MarineTask_Delete',12); '$L_60f040'=@('MarineTask_DeleteSecondary',12); 'FUN_608ef0'=@('MarineGeometry_Snapshot',16); 'FUN_60f0d0'=@('MarineTransition_Create',32); 'FUN_60f1c0'=@('MarineTransition_Advance',12); 'FUN_60dd60'=@('MarineTask_ReleaseMesh',8) }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, '@Reference_'+$methods[$label][0]+'@'+$methods[$label][1]) }
$bindings='EXTERN ??2@YAPAXI@Z:PROC'+"`n"+'EXTERN ??3@YAXPAX@Z:PROC'+"`n"+'EXTERN _TestTimePointer:DWORD'+"`n"
foreach($label in $labels.Keys) { $reference=$reference.Replace('FUN_'+$label,$labels[$label]); $bindings+='EXTERN '+$labels[$label]+":PROC`n" }
$reference=$reference.Replace('__imp__timeGetTime@0','_TestTimePointer')
$variables=@{ '652ff8'='g_marineMeshVtable'; '653040'='g_marineTaskVtable'; '653020'='g_marineTaskSecondaryVtable' }
foreach($label in $variables.Keys) { $name=$variables[$label]; $bindings+='EXTERN _'+$name+":BYTE`n"+'$L_'+$label+' TEXTEQU <_'+$name+">`n" }
$bindings+=".data`n"+'$L_649fb8 DWORD 0'+"`n"+'$L_64a057 DWORD 0'+"`n"+'$L_64a086 DWORD 0'+"`n"
$asm = Join-Path $outputDir 'marine-lifetime-reference.asm'
Set-Content $asm (".686`n.model flat`nASSUME FS:NOTHING`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-lifetime-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-lifetime-reference.obj'
$testObj = Join-Path $outputDir 'marine-lifetime-test.obj'
$exe = Join-Path $outputDir 'marine-lifetime-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine lifetime compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_lifetime.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine lifetime differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine object lifetimes and geometry transitions match MASM in 576 comparisons.'
