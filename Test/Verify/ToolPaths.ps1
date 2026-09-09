# Resolves tool paths once; every Run*.ps1 dot-sources this AFTER setting $test. Env vars override.
# python = the uv-managed environment of Test/ (uv sync --locked) when it exists, else the PATH python;
# it carries pytest, codespell, lcov-cobertura, universalmutator and the Auralix Python lib (alx.verify).
$python = if (Test-Path "$test\.venv\Scripts\python.exe") { "$test\.venv\Scripts\python.exe" } else { "python" }
$llvm     = if ($env:ALX_LLVM_DIR)  { $env:ALX_LLVM_DIR }  else { "C:\Program Files\LLVM\bin" }
# analysis-only toolchain (Stage 3 -fanalyzer) - deliberately NEWER than the frozen
# per-project production toolchains: findings are source properties, keep the analyzer fresh
$armgcc   = if ($env:ALX_ARMGCC)    { $env:ALX_ARMGCC }    else { "C:\SysGCC\arm-eabi-15-2-1\bin\arm-none-eabi-gcc.exe" }
$cppcheck = if ($env:ALX_CPPCHECK)  { $env:ALX_CPPCHECK }  else { "C:\Program Files\Cppcheck\cppcheck.exe" }
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vswhere -latest -property installationPath
$vcvars = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars)) { throw "vcvars64.bat not found under $vsPath" }
