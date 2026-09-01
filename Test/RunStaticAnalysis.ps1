# Auralix C Library - static analysis of the PC-tested module sources
#
#   Leg 1  clang-tidy    (.clang-tidy config, compile_commands.json from conftest)
#   Leg 2  cppcheck      (the only tool exploring #ifdef combinations)
#   Leg 3  gcc -fanalyzer (arm-gcc 14.2.Rel1, interprocedural path analysis)
#
# Exit != 0 on any finding. Usage:  powershell -File RunStaticAnalysis.ps1

$ErrorActionPreference = "Stop"
$test  = $PSScriptRoot
$clib  = Split-Path $test
$build = Join-Path $test "build"
. "$PSScriptRoot\ToolPaths.ps1"

# module sources under analysis (extend per module)
$sources = @("$clib\alxFifo.c", "$clib\alxBound.c", "$test\alxFifoTestHelpers.c", "$test\alxFifoAsanSmoke.c")

New-Item -ItemType Directory -Force "$build\analysis" | Out-Null
if (-not (Test-Path "$build\compile_commands.json")) {
    python -m pytest -q --collect-only | Out-Null   # triggers conftest build + compile DB
}

# --- Leg 0: codespell (spelling gate on the module set) ----------------------
python -m codespell_lib @sources
if ($LASTEXITCODE -ne 0) { throw "Leg 0 FAILED: codespell findings above" }
Write-Host "Leg 0 (codespell): CLEAN"

# --- Leg 1: clang-tidy -------------------------------------------------------
$srcArgs = ($sources | ForEach-Object { '"' + $_ + '"' }) -join ' '
cmd /s /c """$llvm\clang-tidy.exe"" --quiet -p ""$build"" $srcArgs 2>nul"
if ($LASTEXITCODE -ne 0) { throw "Leg 1 FAILED: clang-tidy findings above" }
Write-Host "Leg 1 (clang-tidy): CLEAN"

# --- Leg 2: cppcheck ---------------------------------------------------------
if (Test-Path $cppcheck) {
    & $cppcheck --std=c99 --platform=unix32 --enable=warning,style,performance,portability `
        --inline-suppr --error-exitcode=1 --quiet `
        --suppress=missingIncludeSystem --suppress=unusedFunction `
        -I $test -I $clib -I "$clib\Mcu" @sources
    if ($LASTEXITCODE -ne 0) { throw "Leg 2 FAILED: cppcheck findings above" }
    # second pass: host pointer model (x64); the unix32 pass above models the 32-bit target
    & $cppcheck --std=c99 --platform=win64 --enable=warning,portability `
        --inline-suppr --error-exitcode=1 --quiet `
        --suppress=missingIncludeSystem --suppress=unusedFunction `
        -I $test -I $clib -I "$clib\Mcu" @sources
    if ($LASTEXITCODE -ne 0) { throw "Leg 2 FAILED: cppcheck (win64 pass) findings above" }
    Write-Host "Leg 2 (cppcheck unix32+win64): CLEAN"
} else {
    Write-Host "Leg 2 (cppcheck): SKIPPED - not installed"
}

# --- Leg 3: gcc -fanalyzer (arm-gcc, target flags) ---------------------------
$log = "$build\analysis\fanalyzer.txt"
Set-Content $log -Value ""
foreach ($src in $sources) {
    $name = [IO.Path]::GetFileNameWithoutExtension($src)
    cmd /s /c """$armgcc"" -c -std=gnu99 -mcpu=cortex-m0plus -mthumb -fanalyzer -I ""$test"" -I ""$clib"" -I ""$clib\Mcu"" ""$src"" -o ""$build\analysis\$name.o"" 2>>""$log"""
    if ($LASTEXITCODE -ne 0) { Get-Content $log; throw "Leg 3 FAILED: $name did not compile" }
}
if (Select-String -Path $log -Pattern "-Wanalyzer" -Quiet) {
    Get-Content $log; throw "Leg 3 FAILED: -fanalyzer findings above"
}
Write-Host "Leg 3 (gcc -fanalyzer): CLEAN"
Write-Host ""
Write-Host "STATIC ANALYSIS CLEAN"
