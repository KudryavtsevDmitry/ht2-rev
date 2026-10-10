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
$source = Get-Content (Join-Path $repo 'src\engine.cpp') -Raw
$body = [regex]::Match($source, '(?s)extern "C" void __fastcall SceneTexture_Advance\(.*?\n\}').Value
if (!$body) { throw 'Texture implementation not found' }
$header = "#include `"../src/king.h`"`n#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))`nextern `"C`" void __fastcall Pooled_Set4(BYTE*, void*);`n"
$cpp = Join-Path $outputDir 'texture-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = [regex]::Match($masm, '(?s)FUN_59aa10 PROC.*?FUN_59aa10 ENDP').Value
$ftol = [regex]::Match($masm, '(?s)__ftol PROC.*?__ftol ENDP').Value
if (!$reference -or !$ftol) { throw 'MASM reference implementation not found' }
$reference = $reference.Replace('FUN_59aa10', '@Reference_Advance@8').Replace('FUN_579370', '@Pooled_Set4@8')
$asm = Join-Path $outputDir 'texture-reference.asm'
Set-Content $asm (".686`n.model flat`nEXTERN @Pooled_Set4@8:PROC`n.code`n" + $reference + "`n" + $ftol + "`nEND")
$actualObj = Join-Path $outputDir 'texture-actual.obj'
$referenceObj = Join-Path $outputDir 'texture-reference.obj'
$testObj = Join-Path $outputDir 'texture-test.obj'
$exe = Join-Path $outputDir 'texture-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Texture compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_texture_advance.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Texture differential test failed: exit code $LASTEXITCODE" }
Write-Output '108 texture advancement cases match the original MASM.'
