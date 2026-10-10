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
$source = Get-Content (Join-Path $repo 'src\schitem.cpp') -Raw
$single = [regex]::Match($source, '(?s)extern "C" BYTE\*\* __stdcall ValueList_Erase\(.*?\n\}').Value
$rangeBody = [regex]::Match($source, '(?s)extern "C" BYTE\*\* __fastcall SchInputList_EraseRange\(.*?\n\}').Value
$body = $single + "`n" + $rangeBody
if (!$body) { throw 'Script list implementation not found' }
$header = "#include `"../src/king.h`"`n#define F(type, p, off) (*(type*)((BYTE*)(p) + (off)))`nextern `"C`" void __cdecl TestFree(void*, int);`nstatic void AiFree(void* node, int size) { TestFree(node, size); }`n"
$cpp = Join-Path $outputDir 'list-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$range = [regex]::Match($masm, '(?s)FUN_4b0a80 PROC.*?FUN_4b0a80 ENDP').Value
$one = [regex]::Match($masm, '(?s)FUN_4b0ac0 PROC.*?FUN_4b0ac0 ENDP').Value
if (!$range -or !$one) { throw 'MASM reference implementation not found' }
$reference = ($range + "`n" + $one).Replace('FUN_4b0a80', '@Reference_EraseRange@20').Replace('FUN_4b0ac0', '@Reference_EraseOne@16').Replace('FUN_44b4a0', '_TestFree')
$asm = Join-Path $outputDir 'list-reference.asm'
Set-Content $asm (".686`n.model flat`nEXTERN _TestFree:PROC`n.code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'list-actual.obj'
$referenceObj = Join-Path $outputDir 'list-reference.obj'
$testObj = Join-Path $outputDir 'list-test.obj'
$exe = Join-Path $outputDir 'list-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'List compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_script_list.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Script list differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Script list erasure matches MASM for empty ranges, shared references, final release and immediate values.'
