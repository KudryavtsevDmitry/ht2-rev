# run.ps1 [-Old <commit>] [-Cases N] - differential test of the game's gzip: builds the game at <commit>
# (default HEAD) in a worktree and the working tree here, links each one's objects with gzip_test.cpp
# (a console main instead of WinMain), runs both and compares the packed bytes.
param([string]$Old = 'HEAD', [int]$Cases = 300)
$ErrorActionPreference = 'Stop'
$repo = Resolve-Path "$PSScriptRoot\..\.."
$out = "$env:TEMP\gzip_test"
$vc = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.29.30133'
$sdk = 'C:\Program Files (x86)\Windows Kits\10'
$sv = '10.0.26100.0'
$nmake = "$vc\bin\HostX86\x86\nmake.exe"
New-Item -ItemType Directory -Force $out | Out-Null

function Build-Test([string]$dir, [string]$tag, [string[]]$defs) {
    Push-Location $dir
    & $nmake /nologo | Out-Null
    if ($LASTEXITCODE) { Pop-Location; throw "build failed in $dir" }
    $link = (& $nmake /nologo /n /A king.exe | Where-Object { $_ -match 'link.exe' } | Select-Object -Last 1)
    $cl = "$vc\bin\HostX86\x86\cl.exe"
    & $cl /nologo /c /O2 /D_CRT_SECURE_NO_WARNINGS @defs "/I$vc\include" "/I$sdk\Include\$sv\um" "/I$sdk\Include\$sv\shared" "/I$sdk\Include\$sv\ucrt" "/Fo$out\gzip_test_$tag.obj" "$PSScriptRoot\gzip_test.cpp" | Out-Null
    if ($LASTEXITCODE) { Pop-Location; throw 'compile failed' }
    $link = $link -replace 'king\.res', "`"$out\gzip_test_$tag.obj`"" -replace '/SUBSYSTEM:WINDOWS,5.01', '/SUBSYSTEM:CONSOLE' `
                  -replace '/ENTRY:WinMainCRTStartup', '/ENTRY:mainCRTStartup' -replace '/OUT:king.exe', "/OUT:`"$out\gzip_$tag.exe`"" `
                  -replace '/MAP:king.map', "/MAP:`"$out\gzip_$tag.map`""
    cmd /c $link | Out-Null
    if ($LASTEXITCODE) { Pop-Location; throw "link failed in $dir" }
    Pop-Location
}

$wt = "$out\old"
$ErrorActionPreference = 'Continue'
if (Test-Path $wt) { git -C $repo worktree remove --force $wt 2>&1 | Out-Null }
git -C $repo worktree add --detach $wt $Old 2>&1 | Out-Null
$ErrorActionPreference = 'Stop'
Copy-Item "$repo\king.res" "$wt\king.res"
(Get-Item "$wt\king.res").LastWriteTime = Get-Date    # newer than extractResources.py
try {
    Build-Test $wt 'old' @('/DOLD_GZIP')
    Build-Test $repo 'new' @()
} finally {
    $ErrorActionPreference = 'Continue'
    git -C $repo worktree remove --force $wt 2>&1 | Out-Null
}
& "$out\gzip_old.exe" $Cases "$out\packed_old.bin"
& "$out\gzip_new.exe" $Cases "$out\packed_new.bin"
$a = [IO.File]::ReadAllBytes("$out\packed_old.bin"); $b = [IO.File]::ReadAllBytes("$out\packed_new.bin")
$same = $a.Length -eq $b.Length
if ($same) { for ($i = 0; $i -lt $a.Length; $i++) { if ($a[$i] -ne $b[$i]) { $same = $false; "first difference at byte $i"; break } } }
"packed bytes: old $($a.Length), new $($b.Length), " + $(if ($same) { 'identical' } else { 'DIFFERENT' })
