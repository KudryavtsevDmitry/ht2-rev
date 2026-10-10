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
$body = $source.Substring($source.IndexOf('// Priority queues specific'), $source.IndexOf('// Intrusive queue link manipulation') - $source.IndexOf('// Priority queues specific'))
$header = "#include `"../src/king.h`"`n#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))`nASM_PROC(FUN_60f690)`nASM_PROC(FUN_6136f0)`n"
$header += 'extern "C" BYTE* __fastcall MarineQueue_RemoveNode(BYTE*,void*,BYTE*);' + "`n" + 'extern "C" DWORD __fastcall MarineQueue_InsertNode(BYTE*,void*,BYTE*);' + "`n" + 'extern "C" DWORD __fastcall MarineQueue_InsertEdge(BYTE*,void*,BYTE*);' + "`n"
$cpp = Join-Path $outputDir 'marine-queue-actual.cpp'
Set-Content $cpp ($header + $body)
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ '60f4f0'='SetNodePriority'; '60f8d0'='SetEdgePriority'; '60f7b0'='TopNode'; '60f9d0'='TopEdge' }
foreach ($label in $methods.Keys) {
    $bytes = if($methods[$label].StartsWith('Set')) {16} else {8}
    $reference += [regex]::Match($masm, ('(?s)FUN_'+$label+' PROC.*?FUN_'+$label+' ENDP')).Value.Replace('FUN_'+$label, '@Reference_'+$methods[$label]+'@'+$bytes) + "`n"
}
$bindings = "EXTERN FUN_60f690:PROC`nEXTERN FUN_6136f0:PROC`nEXTERN FUN_60f520:PROC`nEXTERN FUN_60f900:PROC`n"
$asm = Join-Path $outputDir 'marine-queue-reference.asm'
Set-Content $asm (".686`n.model flat`n" + $bindings + ".code`n" + $reference + "`nEND")
$actualObj = Join-Path $outputDir 'marine-queue-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-queue-reference.obj'
$testObj = Join-Path $outputDir 'marine-queue-test.obj'
$exe = Join-Path $outputDir 'marine-queue-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Queue compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_queue.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine queue differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine queues match MASM in 512 selection and 48 priority-update cases, including unordered scores and count overflow.'
