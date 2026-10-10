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
$body = $source.Substring($source.IndexOf('// Marine motion response'))
$header = @'
#include "../src/king.h"
#include "../src/x87.h"
#define F(type,p,off) (*(type*)((BYTE*)(p)+(off)))
extern "C" unsigned __stdcall TestTime();
#define timeGetTime TestTime
extern "C" int __fastcall MarineGreed_Emit(BYTE*,void*);
KING_ALIAS("@MarineGreed_Emit@8","@TestEmit@8")
'@
$cpp = Join-Path $outputDir 'marine-motion-actual.cpp'
Set-Content $cpp ($header + "`n" + $body)
$variables = [regex]::Matches($body, 'ASM_VAR\(([^,]+), (\w+), (\$L_[0-9a-f]+)\)')
$globals = "#include `"../src/king.h`"`nextern `"C`" {`nint _fltused=0;`n"
$bindings='EXTERN _TestTimePointer:DWORD'+"`n"+'EXTERN @TestEmit@8:PROC'+"`n"
foreach($variable in $variables) {
    $type=$variable.Groups[1].Value;$name=$variable.Groups[2].Value;$label=$variable.Groups[3].Value
    $globals+="$type $name;`n";$bindings+="EXTERN _${name}:DWORD`n$label TEXTEQU <_$name>`n"
}
$globals+="}`n"+'static float* coefficients[]={'+(($variables | Where-Object { $_.Groups[1].Value -eq 'float' } | ForEach-Object { '&'+$_.Groups[2].Value }) -join ',')+"};`n"
Set-Content (Join-Path $outputDir 'marine-motion-globals.h') $globals
$masm = Get-Content (Join-Path $repo 'king.masm') -Raw
$reference = ''
$methods = @{ 'FUN_60de50'='@Reference_MarineTask_UpdateMotion@12'; 'FUN_60df90'='@Reference_MarineTask_Frame@16'; 'FUN_6132e0'='@Reference_MarineTransition_SetActive@12' }
foreach($label in $methods.Keys) { $reference += [regex]::Match($masm, ('(?s)'+[regex]::Escape($label)+' PROC.*?'+[regex]::Escape($label)+' ENDP')).Value + "`n" }
foreach($label in $methods.Keys) { $reference=$reference.Replace($label, $methods[$label]) }
$reference=$reference.Replace('__imp__timeGetTime@0','_TestTimePointer').Replace('FUN_60d250','@TestEmit@8')
$reference += [regex]::Match($masm, '(?s)__ftol PROC.*?__ftol ENDP').Value
$asm = Join-Path $outputDir 'marine-motion-reference.asm'
Set-Content $asm (".686`n.model flat`n"+$bindings+".code`n"+$reference+"`nEND")
$actualObj = Join-Path $outputDir 'marine-motion-actual.obj'
$referenceObj = Join-Path $outputDir 'marine-motion-reference.obj'
$testObj = Join-Path $outputDir 'marine-motion-test.obj'
$exe = Join-Path $outputDir 'marine-motion-test.exe'
& $compiler @flags "/Fo$actualObj" $cpp
if ($LASTEXITCODE) { throw 'Marine motion compilation failed' }
& (Join-Path $vc 'bin\HostX86\x86\ml.exe') /nologo /c "/Fo$referenceObj" $asm
if ($LASTEXITCODE) { throw 'Reference assembly failed' }
& $compiler @flags "/Fo$testObj" (Join-Path $PSScriptRoot 'test_marine_motion.cpp')
if ($LASTEXITCODE) { throw 'Test compilation failed' }
& $linker /nologo $actualObj $referenceObj $testObj /NODEFAULTLIB /ENTRY:TestEntry /SUBSYSTEM:CONSOLE "/OUT:$exe" "/LIBPATH:$sdkLib" kernel32.lib
if ($LASTEXITCODE) { throw 'Test link failed' }
& $exe
if ($LASTEXITCODE) { throw "Marine motion differential test failed: exit code $LASTEXITCODE" }
Write-Output 'Marine motion, frame processing and transition activation match MASM in 4,608 comparisons.'
