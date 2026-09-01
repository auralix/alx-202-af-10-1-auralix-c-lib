# Auralix C Library - PC unit test C-code coverage run (ALX-1514)
#
# Builds a clang-instrumented DLL variant, runs the SAME pytest suite against it,
# and produces:
#   build/cov/coverage_report.txt   - human summary (also printed)
#   build/cov/html/index.html       - human per-line view (open in browser)
#   build/cov/coverage_c.xml        - cobertura XML (CI: Codecov/GitLab/Sonar/PR rendering)
#   build/cov/lcov.info             - lcov intermediate
#
# Requirements: LLVM (clang-cl, llvm-profdata, llvm-cov), VS2022 (vcvars), lcov-cobertura (pip).
# Usage:  powershell -File RunCoverage.ps1

$ErrorActionPreference = "Stop"
$test  = $PSScriptRoot
$clib  = Split-Path $test
$cov   = Join-Path $test "build\cov"
. "$PSScriptRoot\ToolPaths.ps1"

python -m pytest -q --collect-only | Out-Null   # dev gate: -Werror build must be fresh
if ($LASTEXITCODE -ne 0) { throw "dev-lane build/collect failed" }
New-Item -ItemType Directory -Force $cov | Out-Null
Remove-Item "$cov\*.profraw" -Force -ErrorAction SilentlyContinue

# 1) instrumented DLL (per-module sources; extend list per module)
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fprofile-instr-generate -fcoverage-mapping /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxFifoTestHelpers.c"" /Fe:""$cov\alxFifoTest.dll"" /Fo""$cov""\ /link /DEF:""$test\alxFifoTest.def"""
if ($LASTEXITCODE -ne 0) { throw "coverage DLL build failed" }

# 2) same suite, instrumented binary
$env:ALX_FIFO_TEST_DLL  = "$cov\alxFifoTest.dll"
$env:LLVM_PROFILE_FILE  = "$cov\%m-%p.profraw"
try {
    python -m pytest -q
    if ($LASTEXITCODE -ne 0) { throw "pytest failed (rc=$LASTEXITCODE) - coverage of a red suite is meaningless" }
}
finally {
    Remove-Item Env:ALX_FIFO_TEST_DLL, Env:LLVM_PROFILE_FILE -ErrorAction SilentlyContinue
}

# 3) merge + reports
& "$llvm\llvm-profdata.exe" merge -sparse (Get-ChildItem "$cov\*.profraw").FullName -o "$cov\merged.profdata"
& "$llvm\llvm-cov.exe" report "$cov\alxFifoTest.dll" -instr-profile="$cov\merged.profdata" | Tee-Object "$cov\coverage_report.txt"
& "$llvm\llvm-cov.exe" export "$cov\alxFifoTest.dll" -instr-profile="$cov\merged.profdata" -format=lcov | Out-File -Encoding ascii "$cov\lcov.info"
python -m lcov_cobertura "$cov\lcov.info" --output "$cov\coverage_c.xml" --base-dir "$clib"
& "$llvm\llvm-cov.exe" show "$cov\alxFifoTest.dll" -instr-profile="$cov\merged.profdata" -format=html -output-dir="$cov\html" -show-branches=count -show-line-counts
Write-Host "`ncobertura: $cov\coverage_c.xml"

# GATE: module under test must be 100% covered (a report alone is not a gate)
& "$llvm\llvm-cov.exe" export "$cov\alxFifoTest.dll" -instr-profile="$cov\merged.profdata" -summary-only | Out-File -Encoding ascii "$cov\summary.json"
python "$test\coverage_gate.py" "$cov\summary.json" alxFifo.c
if ($LASTEXITCODE -ne 0) { throw "COVERAGE GATE FAILED - see above" }
Write-Host "html:      $cov\html\index.html"
