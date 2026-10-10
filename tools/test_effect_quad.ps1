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
$source = Get-Content (Join-Path $repo 'src\wasm.cpp') -Raw
$body = [regex]::Match($source, '(?s)extern "C" void __fastcall Effect_AnimateQuad\(.*').Value
if (!$body) { throw 'Quad implementation not found' }
$header = "#include `"../src/king.h`"`n#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))`nextern `"C`" void __fastcall SceneTexture_Advance(BYTE*, void*);`nextern `"C`" { double g_effectZero=0.0, g_effectDoubleOne=1.0, g_effectNegativeHalf=-0.5, g_effectHalf=0.5; }`n"
$cpp = Join-Path $outputDir 'quad-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$quad = [regex]::Match($masm, '(?s)FUN_59ac90 PROC.*?FUN_59ac90 ENDP').Value
$move = [regex]::Match($masm, '(?s)\$L_59f000 PROC.*?\$L_59f000 ENDP').Value
if (!$quad -or !$move) { throw 'MASM reference implementation not found' }
$reference = ($quad + "`n" + $move).Replace('FUN_59ac90', '@Reference_AnimateQuad@8').Replace('$L_59f000', '@Reference_MoveQuad@8').Replace('FUN_59aa10', '@SceneTexture_Advance@8')
$bindings = ""
foreach ($pair in @(@('$L_64b678','g_effectZero'), @('$L_64b670','g_effectDoubleOne'), @('$L_64b6b8','g_effectNegativeHalf'), @('$L_64b6e0','g_effectHalf'))) {
    $bindings += 'EXTERN _' + $pair[1] + ':QWORD' + "`n" + $pair[0] + ' TEXTEQU <_' + $pair[1] + '>' + "`n"
}
$asm = Join-Path $outputDir 'quad-reference.asm'
Set-Content $asm (".686`n.model flat`nEXTERN @SceneTexture_Advance@8:PROC`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'quad-actual.obj'
$referenceObj = Join-Path $outputDir 'quad-reference.obj'
$testObj = Join-Path $outputDir 'quad-test.obj'
$exe = Join-Path $outputDir 'quad-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Quad compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_effect_quad.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Quad differential test failed: exit code $LASTEXITCODE" }
Write-Output '108 quad animation/movement cases match the original MASM bit for bit.'
