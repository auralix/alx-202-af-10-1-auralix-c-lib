# Auralix C Library - PC unit test sanitizer run (ALX-1514)
#
# Two proven stages (see ALX-1514 task notes for the measurements behind this design):
#
#   Stage 1 - NATIVE combined ASan+UBSan smoke exe (clang) - the DIAGNOSTICS vehicle:
#           fully instrumented process, reports with exact file:line, exit != 0 on any finding.
#           (Measured: catches planted heap-buffer-overflow and signed-shift UB with location.)
#
#   Stage 2 - UBSan-instrumented DLL under the FULL pytest suite - the COVERAGE vehicle:
#           all behavioral tests run against instrumented code; UB aborts the process
#           (diagnostics are mute under python on Windows - on failure, rerun Stage 1 for the
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
# build/ layout rule: root = dev lane + shared artifacts; one subfolder per variant
# (asan/ = Stage 1 exe, ubsan/ = Stage 2 DLL; cov/ and analysis/ follow the same rule)
New-Item -ItemType Directory -Force $build, "$build\asan", "$build\ubsan" | Out-Null

# --- Stage 1: native combined ASan+UBSan exe (clang) --------------------------
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /clang:-std=gnu99 -fsanitize=address,undefined -fno-sanitize-recover=undefined /Z7 /MT /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxFifoSanSmoke.c"" /Fe:""$build\asan\alxFifoSanSmoke.exe"" /Fo""$build\asan""\"
if ($LASTEXITCODE -ne 0) { throw "sanitizer smoke exe build failed" }
$resDir = (& "$llvm\clang.exe" -print-resource-dir | Out-String).Trim()
Copy-Item (Join-Path $resDir "lib\windows\clang_rt.asan_dynamic-x86_64.dll") "$build\asan" -Force   # the compiler's OWN runtime must shadow MSVC's older copy
& "$build\asan\alxFifoSanSmoke.exe"
if ($LASTEXITCODE -ne 0) { throw "Stage 1 FAILED: sanitizer finding in native smoke run (see report above)" }
Write-Host "Stage 1 (native ASan+UBSan smoke): CLEAN"

# --- Stage 2: UBSan DLL + full pytest suite ------------------------------------
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fsanitize=undefined -fno-sanitize-recover=undefined /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxFifoTestHelpers.c"" ""$test\alxBoundTestHelpers.c"" /Fe:""$build\ubsan\alxFifoTest.dll"" /Fo""$build\ubsan""\ /link /DEF:""$test\alxFifoTest.def"""
if ($LASTEXITCODE -ne 0) { throw "UBSan DLL build failed" }
$env:ALX_FIFO_TEST_DLL = "$build\ubsan\alxFifoTest.dll"
try {
    python -m pytest -q
    if ($LASTEXITCODE -ne 0) { throw "Stage 2 FAILED: suite red or process killed by UBSan (rc=$LASTEXITCODE) - rerun Stage 1 for the location" }
}
finally {
    $env:ALX_FIFO_TEST_DLL = $null
}
Write-Host "Stage 2 (UBSan DLL, full suite): CLEAN"

# --- Stage 2b: UBSan CLI DLL (Tier 2: real alxCli + param stack over the fakes) + CLI suite ---
# Same two-step shape as conftest._build_cli_dll: closure objects with warnings off, then the
# strict sources + objects linked into the DLL. Asserts ON everywhere = the code as shipped
# (alxParamItem.c has side effects inside its asserts - with asserts off the numeric values
# format as EMPTY). KEEP THE SOURCE LISTS IN SYNC WITH conftest.CLI_SOURCES_STRICT/_CLOSURE.
$cliAsserts = "-DALX_CLI_ASSERT_RST_ENABLE -DALX_SERIAL_PORT_ASSERT_RST_ENABLE -DALX_FIFO_ASSERT_RST_ENABLE -DALX_BOUND_ASSERT_RST_ENABLE -DALX_PARAM_ITEM_ASSERT_RST_ENABLE -DALX_PARAM_MGMT_ASSERT_RST_ENABLE -DALX_FTOA_ASSERT_RST_ENABLE -DALX_RANGE_ASSERT_RST_ENABLE -DALX_ID_ASSERT_RST_ENABLE"
$cliClosure = "$build\ubsan\cliClosure"
New-Item -ItemType Directory -Force $cliClosure | Out-Null
cmd /s /c "cd /d ""$cliClosure"" && ""$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /clang:-std=gnu99 -fsanitize=undefined -fno-sanitize-recover=undefined /w -D_CRT_SECURE_NO_WARNINGS $cliAsserts /I""$test"" /I""$clib"" /I""$clib\Mcu"" /c ""$clib\alxParamItem.c"" ""$clib\alxParamMgmt.c"" ""$clib\alxFtoa.c"" ""$clib\alxRange.c"""
if ($LASTEXITCODE -ne 0) { throw "UBSan CLI closure build failed" }
$cliObjs = (Get-ChildItem "$cliClosure\*.obj" | ForEach-Object { """$($_.FullName)""" }) -join " "
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fsanitize=undefined -fno-sanitize-recover=undefined -D_CRT_SECURE_NO_WARNINGS $cliAsserts /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxCli.c"" ""$clib\alxFifo.c"" ""$clib\alxBound.c"" ""$test\alxSerialPortFake.c"" ""$test\alxParamKvStoreFake.c"" ""$test\alxIdFake.c"" ""$test\alxAssertPc.c"" ""$test\alxCliTestHelpers.c"" $cliObjs /Fe:""$build\ubsan\alxCliTest.dll"" /Fo""$build\ubsan""\ /link /DEF:""$test\alxCliTest.def"""
if ($LASTEXITCODE -ne 0) { throw "UBSan CLI DLL build failed" }
$env:ALX_CLI_TEST_DLL = "$build\ubsan\alxCliTest.dll"
try {
    python -m pytest -q test_alxCli.py
    if ($LASTEXITCODE -ne 0) { throw "Stage 2b FAILED: CLI suite red or process killed by UBSan (rc=$LASTEXITCODE)" }
}
finally {
    $env:ALX_CLI_TEST_DLL = $null
}
Write-Host "Stage 2b (UBSan CLI DLL, CLI suite): CLEAN"
# --- Stage 2c: UBSan MemSafe DLL (Tier 2: real alxMemSafe/alxCrc/alxParamGroup/alxParamStore over alxMemRawFake)
# + the MemSafe group suite (ALX-1513). Same two-step shape as conftest._build_memsafe_dll; asserts ON = as shipped.
# KEEP THE SOURCE LISTS IN SYNC WITH conftest.MEMSAFE_SOURCES_STRICT/_CLOSURE/_ASSERT_DEFINES.
$msAsserts = "-DALX_MEM_SAFE_ASSERT_RST_ENABLE -DALX_MEM_RAW_ASSERT_RST_ENABLE -DALX_CRC_ASSERT_RST_ENABLE -DALX_PARAM_GROUP_ASSERT_RST_ENABLE -DALX_PARAM_STORE_ASSERT_RST_ENABLE -DALX_PARAM_ITEM_ASSERT_RST_ENABLE -DALX_BOUND_ASSERT_RST_ENABLE -DALX_FTOA_ASSERT_RST_ENABLE -DALX_RANGE_ASSERT_RST_ENABLE"
$msClosure = "$build\ubsan\memsafeClosure"
New-Item -ItemType Directory -Force $msClosure | Out-Null
cmd /s /c "cd /d ""$msClosure"" && ""$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /clang:-std=gnu99 -fsanitize=undefined -fno-sanitize-recover=undefined /w -D_CRT_SECURE_NO_WARNINGS $msAsserts /I""$test"" /I""$clib"" /I""$clib\Mcu"" /c ""$clib\alxParamGroup.c"" ""$clib\alxParamStore.c"" ""$clib\alxParamItem.c"" ""$clib\alxFtoa.c"" ""$clib\alxRange.c"""
if ($LASTEXITCODE -ne 0) { throw "UBSan MemSafe closure build failed" }
$msObjs = (Get-ChildItem "$msClosure\*.obj" | ForEach-Object { """$($_.FullName)""" }) -join " "
cmd /s /c """$vcvars"" >nul 2>&1 && ""$llvm\clang-cl.exe"" /LD /clang:-std=gnu99 -fsanitize=undefined -fno-sanitize-recover=undefined -D_CRT_SECURE_NO_WARNINGS $msAsserts /I""$test"" /I""$clib"" /I""$clib\Mcu"" ""$clib\alxMemSafe.c"" ""$clib\alxCrc.c"" ""$clib\alxBound.c"" ""$test\alxMemRawFake.c"" ""$test\alxParamKvStoreFake.c"" ""$test\alxAssertPc.c"" ""$test\alxMemSafeTestHelpers.c"" $msObjs /Fe:""$build\ubsan\alxMemSafeTest.dll"" /Fo""$build\ubsan""\ /link /DEF:""$test\alxMemSafeTest.def"""
if ($LASTEXITCODE -ne 0) { throw "UBSan MemSafe DLL build failed" }
$env:ALX_MEMSAFE_TEST_DLL = "$build\ubsan\alxMemSafeTest.dll"
try {
    python -m pytest -q test_alxCrc.py test_alxMemSafe.py test_alxParamGroup.py test_alxParamStore.py
    if ($LASTEXITCODE -ne 0) { throw "Stage 2c FAILED: MemSafe group suite red or process killed by UBSan (rc=$LASTEXITCODE)" }
}
finally {
    $env:ALX_MEMSAFE_TEST_DLL = $null
}
Write-Host "Stage 2c (UBSan MemSafe DLL, MemSafe group suite): CLEAN"

Write-Host "`nSANITIZERS CLEAN"
