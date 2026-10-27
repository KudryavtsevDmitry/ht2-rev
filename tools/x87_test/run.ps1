# run.ps1 [CASES] - builds and runs the tests of the procedures that were inline __asm, in $env:TEMP\x87_test.
# small_test.cpp: the player lookup and the rope compare (pasted from src\ai by extract.py) against small_asm.cpp.
# hitch_test.cpp:
# regenerates the C++ from hitch_asm.cpp with branch counters, checks that src\truck\hitch_solver.cpp is the
# same translation, and compares both on CASES random worlds (default 3000) at 53- and 24-bit FPU precision.
# HT_TRACE=1 prints each side's callee sequence. The rdata constants come from the original king.exe
# (old-history:king_original.exe).
param([int]$Cases = 3000)
$ErrorActionPreference = 'Stop'
$repo = Resolve-Path "$PSScriptRoot\..\.."
$out = "$env:TEMP\x87_test"
New-Item -ItemType Directory -Force $out | Out-Null
$vc = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.29.30133'
$sdk = 'C:\Program Files (x86)\Windows Kits\10'
$sv = '10.0.26100.0'

if (-not (Test-Path "$out\king_original.exe")) {
    $p = Start-Process git -ArgumentList "-C `"$repo`" cat-file blob old-history:king_original.exe" -NoNewWindow -Wait -PassThru -RedirectStandardOutput "$out\king_original.exe"
    if ($p.ExitCode) { throw 'no old-history:king_original.exe' }
}

py "$repo\tools\x87_to_c.py" "$PSScriptRoot\hitch_asm.cpp" L_FUN_558f20 Hitch_SolveC "$out\hitch_c.cpp" --cov
py "$repo\tools\x87_to_c.py" "$PSScriptRoot\hitch_asm.cpp" L_FUN_558f20 Hitch_Solve "$out\hitch_plain.cpp"
# the game's file is the plain translation with its own header (the first comment lines)
$a = Get-Content "$out\hitch_plain.cpp" | Where-Object { $_ -notmatch '^//' }
$b = Get-Content "$repo\src\truck\hitch_solver.cpp" | Where-Object { $_ -notmatch '^//' }
if (Compare-Object $a $b -SyncWindow 0) { throw 'src\truck\hitch_solver.cpp is not the current translation of hitch_asm.cpp' }

Set-Location $out
$cl = "$vc\bin\HostX86\x86\cl.exe"
$inc = @("/I$repo\src\common", "/I$vc\include", "/I$sdk\Include\$sv\um", "/I$sdk\Include\$sv\shared", "/I$sdk\Include\$sv\ucrt")
# the game's CFLAGS for the code under test: the compiler's choices decide the rounding
$game = @('/nologo', '/c', '/O2', '/W3', '/GS-', '/Zl', '/GR-', '/EHs-c-', '/arch:IA32') + $inc
& $cl @game "$PSScriptRoot\hitch_asm.cpp" "$out\hitch_c.cpp"
if ($LASTEXITCODE) { throw 'compile failed' }
& $cl /nologo /c /O2 /W3 /EHsc /arch:IA32 /D_CRT_SECURE_NO_WARNINGS @inc "$PSScriptRoot\hitch_test.cpp"
if ($LASTEXITCODE) { throw 'compile failed' }
& "$vc\bin\HostX86\x86\link.exe" /nologo hitch_test.obj hitch_asm.obj hitch_c.obj /OUT:hitch_test.exe /SUBSYSTEM:CONSOLE /LIBPATH:"$vc\lib\x86" /LIBPATH:"$sdk\Lib\$sv\ucrt\x86" /LIBPATH:"$sdk\Lib\$sv\um\x86" libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib
if ($LASTEXITCODE) { throw 'link failed' }
& .\hitch_test.exe $Cases "$out\king_original.exe"
$fail = $LASTEXITCODE

py "$PSScriptRoot\extract.py" "$out\small_gen.cpp" "$repo\src\ai\ai_containers.cpp:RopeIt_At,Rope_Compare" "$repo\src\ai\ai_drivers.cpp:Player_FromId"
& $cl @game "$PSScriptRoot\small_asm.cpp"
if ($LASTEXITCODE) { throw 'compile failed' }
& $cl /nologo /c /O2 /W3 /EHsc /arch:IA32 /D_CRT_SECURE_NO_WARNINGS "/I$out" @inc "$PSScriptRoot\small_test.cpp"
if ($LASTEXITCODE) { throw 'compile failed' }
& "$vc\bin\HostX86\x86\link.exe" /nologo small_test.obj small_asm.obj /OUT:small_test.exe /SUBSYSTEM:CONSOLE /LIBPATH:"$vc\lib\x86" /LIBPATH:"$sdk\Lib\$sv\ucrt\x86" /LIBPATH:"$sdk\Lib\$sv\um\x86" libcmt.lib libvcruntime.lib libucrt.lib kernel32.lib
if ($LASTEXITCODE) { throw 'link failed' }
& .\small_test.exe $Cases
exit ($fail -bor $LASTEXITCODE)
