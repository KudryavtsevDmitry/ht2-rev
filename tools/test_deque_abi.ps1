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
$flags = @('/nologo', '/c', '/O2', '/GS-', '/Zl', "/I$vc\include", "/I$sdk\um", "/I$sdk\shared", "/I$sdk\ucrt")

$source = Get-Content (Join-Path $repo 'src\scheval.cpp') -Raw
$body = [regex]::Match($source, '(?s)extern "C" void __(?:stdcall|cdecl) Deque_AllocPages\(.*?\n\}').Value
if (!$body) { throw 'Deque_AllocPages implementation not found' }
$header = @'
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern "C" void* __cdecl malloc(unsigned);
extern "C" void FUN_4581a0();
template<class T> void CallC(T fn, int) { fn(); }
'@
$allocator = Join-Path $outputDir 'allocator-test.cpp'
$allocatorObj = Join-Path $outputDir 'allocator-test.obj'
$testObj = Join-Path $outputDir 'deque-test.obj'
$exe = Join-Path $outputDir 'deque-test.exe'
Set-Content $allocator ($header + "`n" + $body)
& $compiler @flags "/Fo$allocatorObj" $allocator
if ($LASTEXITCODE) { throw 'Allocator compilation failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_deque_abi.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $allocatorObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Deque ABI test failed: exit code $LASTEXITCODE" }
Write-Output 'Deque allocator ABI and page allocation tests passed.'
