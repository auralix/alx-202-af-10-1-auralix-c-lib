# Auralix C Library - PC unit test sanitizer run (ALX-1514)
#
# Two proven legs (see ALX-1514 task notes for the measurements behind this design):
#
#   Leg 1 - NATIVE combined ASan+UBSan smoke exe (clang) - the DIAGNOSTICS vehicle:
#           fully instrumented process, reports with exact file:line, exit != 0 on any finding.
#           (Measured: catches planted heap-buffer-overflow and signed-shift UB with location.)
#
#   Leg 2 - UBSan-instrumented DLL under the FULL pytest suite - the COVERAGE vehicle:
#           all behavioral tests run against instrumented code; UB aborts the process
#           (diagnostics are mute under python on Windows - on failure, rerun Leg 1 for the
#           location). Measured: functionally-invisible UB mutant kills the run reliably.
#
#   NOT used: MSVC-ASan DLL under python - MEASURED as non-detecting for DLL-heap OOB on
#   this toolset (VS 17.14), despite loading and running fine. Do not trust it blindly.
#
# Usage:  powershell -File RunSanitizers.ps1

$ErrorActionPreference = "Stop"
$test   = $PSScriptRoot
$clib   = Split-Path $test
$build  = Join-Path $test "build"
. "$PSScriptRoot\ToolPaths.ps1"

python -m pytest -q --collect-only | Out-Null   # dev gate: -Werror build must be fresh
if ($LASTEXITCODE -ne 0) { throw "dev-lane build/collect failed" }
New-Item -ItemType Directory -Force $build, "$build\ubsan" | Out-Null

# --- Leg 1: native combined ASan+UBSan exe (clang) --------------------------
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /clang:-std=gnu99 -fsanitize=address,undefined -fno-sanitize-recover=undefined /Z7 /MT /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxFifoAsanSmoke.c"" /Fe:""$build\alxFifoSanSmoke.exe"" /Fo""$build""\"
if ($LASTEXITCODE -ne 0) { throw "sanitizer smoke exe build failed" }
$resDir = (& "$llvm\clang.exe" -print-resource-dir | Out-String).Trim()
Copy-Item (Join-Path $resDir "lib\windows\clang_rt.asan_dynamic-x86_64.dll") $build -Force   # the compiler's OWN runtime must shadow MSVC's older copy
& "$build\alxFifoSanSmoke.exe"
if ($LASTEXITCODE -ne 0) { throw "Leg 1 FAILED: sanitizer finding in native smoke run (see report above)" }
Write-Host "Leg 1 (native ASan+UBSan smoke): CLEAN"

# --- Leg 2: UBSan DLL + full pytest suite ------------------------------------
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fsanitize=undefined -fno-sanitize-recover=undefined /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxFifoTestHelpers.c"" /Fe:""$build\ubsan\alxFifoTest.dll"" /Fo""$build\ubsan""\ /link /DEF:""$test\alxFifoTest.def"""
if ($LASTEXITCODE -ne 0) { throw "UBSan DLL build failed" }
$env:ALX_FIFO_TEST_DLL = "$build\ubsan\alxFifoTest.dll"
try {
    python -m pytest -q
    if ($LASTEXITCODE -ne 0) { throw "Leg 2 FAILED: suite red or process killed by UBSan (rc=$LASTEXITCODE) - rerun Leg 1 for the location" }
}
finally {
    $env:ALX_FIFO_TEST_DLL = $null
}
Write-Host "Leg 2 (UBSan DLL, full suite): CLEAN"
Write-Host "`nSANITIZERS CLEAN"
