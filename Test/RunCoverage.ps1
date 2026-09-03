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
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fprofile-instr-generate -fcoverage-mapping /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxFifoTestHelpers.c"" ""$test\alxBoundTestHelpers.c"" /Fe:""$cov\alxFifoTest.dll"" /Fo""$cov""\ /link /DEF:""$test\alxFifoTest.def"""
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
python "$test\coverage_gate.py" "$cov\summary.json" alxFifo.c alxBound.c
if ($LASTEXITCODE -ne 0) { throw "COVERAGE GATE FAILED - see above" }
Write-Host "html:      $cov\html\index.html"

# GATE for this group = functions 100 %; lines/branches are REPORTED: alxMemSafe.c keeps three blocks that are
# unreachable with asserts ON (nonBlocking TODO in Read and Write, the impossible else of the decision table)
# and alxCrc.c has `break` after `return` plus assert-guarded default branches - see the ALX-1513 notes.
# ---- Part B: MemSafe test group (ALX-1513) - real alxMemSafe/alxCrc/alxParamGroup/alxParamStore over
# alxMemRawFake, same two-step shape as conftest._build_memsafe_dll (closure objects, then strict sources).
# KEEP THE SOURCE LISTS IN SYNC WITH conftest.MEMSAFE_SOURCES_STRICT/_CLOSURE/_ASSERT_DEFINES.
$ms = Join-Path $cov "memsafe"
New-Item -ItemType Directory -Force $ms, "$ms\closure" | Out-Null
Remove-Item "$ms\*.profraw" -Force -ErrorAction SilentlyContinue
$msAsserts = "-DALX_MEM_SAFE_ASSERT_RST_ENABLE -DALX_MEM_RAW_ASSERT_RST_ENABLE -DALX_CRC_ASSERT_RST_ENABLE -DALX_PARAM_GROUP_ASSERT_RST_ENABLE -DALX_PARAM_STORE_ASSERT_RST_ENABLE -DALX_PARAM_ITEM_ASSERT_RST_ENABLE -DALX_BOUND_ASSERT_RST_ENABLE -DALX_FTOA_ASSERT_RST_ENABLE -DALX_RANGE_ASSERT_RST_ENABLE"
cmd /s /c "cd /d ""$ms\closure"" && ""$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /clang:-std=gnu99 -fprofile-instr-generate -fcoverage-mapping /w -D_CRT_SECURE_NO_WARNINGS $msAsserts /I""$test"" /I""$clib"" /I""$clib\Mcu"" /c ""$clib\alxParamGroup.c"" ""$clib\alxParamStore.c"" ""$clib\alxParamItem.c"" ""$clib\alxFtoa.c"" ""$clib\alxRange.c"""
if ($LASTEXITCODE -ne 0) { throw "coverage MemSafe closure build failed" }
$msObjs = (Get-ChildItem "$ms\closure\*.obj" | ForEach-Object { """$($_.FullName)""" }) -join " "
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fprofile-instr-generate -fcoverage-mapping -D_CRT_SECURE_NO_WARNINGS $msAsserts /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxMemSafe.c"" ""$clib\alxCrc.c"" ""$clib\alxBound.c"" ""$test\alxMemRawFake.c"" ""$test\alxParamKvStoreFake.c"" ""$test\alxAssertPc.c"" ""$test\alxMemSafeTestHelpers.c"" $msObjs /Fe:""$ms\alxMemSafeTest.dll"" /Fo""$ms""\ /link /DEF:""$test\alxMemSafeTest.def"""
if ($LASTEXITCODE -ne 0) { throw "coverage MemSafe DLL build failed" }
$env:ALX_MEMSAFE_TEST_DLL = "$ms\alxMemSafeTest.dll"
$env:LLVM_PROFILE_FILE    = "$ms\%m-%p.profraw"
try {
    python -m pytest -q test_alxCrc.py test_alxMemSafe.py test_alxParamGroup.py test_alxParamStore.py
    if ($LASTEXITCODE -ne 0) { throw "MemSafe group pytest failed (rc=$LASTEXITCODE)" }
}
finally {
    Remove-Item Env:ALX_MEMSAFE_TEST_DLL, Env:LLVM_PROFILE_FILE -ErrorAction SilentlyContinue
}
& "$llvm\llvm-profdata.exe" merge -sparse (Get-ChildItem "$ms\*.profraw").FullName -o "$ms\merged.profdata"
& "$llvm\llvm-cov.exe" report "$ms\alxMemSafeTest.dll" -instr-profile="$ms\merged.profdata" | Tee-Object "$ms\coverage_report.txt"
& "$llvm\llvm-cov.exe" show "$ms\alxMemSafeTest.dll" -instr-profile="$ms\merged.profdata" -format=html -output-dir="$ms\html" -show-branches=count -show-line-counts
& "$llvm\llvm-cov.exe" export "$ms\alxMemSafeTest.dll" -instr-profile="$ms\merged.profdata" -summary-only | Out-File -Encoding ascii "$ms\summary.json"
python "$test\coverage_gate.py" "$ms\summary.json" alxCrc.c alxMemSafe.c --metrics functions
if ($LASTEXITCODE -ne 0) { throw "COVERAGE GATE (MemSafe group) FAILED - see above" }
Write-Host "html:      $ms\html\index.html"
