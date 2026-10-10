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
$body = $source.Substring($source.IndexOf('// Two sentinel')).Replace('operator delete(', 'TestFree(')
$update = [regex]::Match($body, '(?s)extern "C" void __fastcall MarineGreed_Update\(.*?\n\}').Value
$body = $body.Replace($update, '')
$header = "#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nextern `"C`" void TestFree(void*);`n"
foreach ($label in @('60f690','6136f0','606750','6069d0','60aad0','60aa60')) { $header += "ASM_PROC(FUN_$label)`n" }
foreach ($name in @('Date','ErrorFormat','File','AssertFormat','Header','Neighbor44','Neighbor52','Neighbor48')) { $header += "extern `"C`" char g_marine$name[1];`n" }
$cpp = Join-Path $outputDir 'marine-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = [regex]::Match($masm, '(?s)FUN_60d540 PROC.*?FUN_60d540 ENDP').Value.Replace('FUN_60d540','@Reference_RemoveNode@12').Replace('??3@YAXPAX@Z','_TestFree')
$bindings = "EXTERN _TestFree:PROC`n"
foreach ($label in @('60f690','6136f0','606750','6069d0','60aad0','60aa60')) { $bindings += "EXTERN FUN_${label}:PROC`n" }
$names = @{ '6724f8'='Date'; '6724b0'='ErrorFormat'; '685ac8'='File'; '6726a4'='AssertFormat'; '685b70'='Header'; '685b44'='Neighbor44'; '685b18'='Neighbor52'; '685aec'='Neighbor48' }
foreach ($label in $names.Keys) { $name = '_g_marine'+$names[$label]; $bindings += "EXTERN ${name}:BYTE`n" + '$L_' + $label + " TEXTEQU <$name>`n" }
$asm = Join-Path $outputDir 'marine-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-reference.obj'
$testObj = Join-Path $outputDir 'marine-test.obj'
$exe = Join-Path $outputDir 'marine-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine node removal matches MASM in 384 graph, sentinel, neighbor and error-path cases.'
