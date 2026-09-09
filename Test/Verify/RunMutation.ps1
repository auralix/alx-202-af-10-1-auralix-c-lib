# Auralix C Library - MUTATE lane (tests the tests)
#
# The driver is the Auralix Python lib's alx.verify.mutation, the same procedure its own lane runs:
# universalmutator generates the mutants of each source, the driver plants them one by one, runs the
# source's mirror test module (Test/test_<stem>.py), classifies KILLED / SURVIVED / KILLED_COMPILE /
# STILLBORN / EQUIVALENT, restores the source (a backup copy under build/mutation/backup/ survives a
# crash) and writes build/mutation/report.txt + results.json + survivors/*.diff.
# What is C lives in Verify/mutation_hooks.py: the -fsyntax-only check, the object-code fingerprint
# (trivial compile equivalence) and the rebuild of the stale test DLL with conftest's -Werror recipe.
# REPORT-ONLY: survivors do not fail the run - they are the output to review. 100 % kill rate is not
# the target (equivalent mutants exist).
#
# Usage (from Test/):  powershell -File Verify\RunMutation.ps1 [-Sample 150] [-Sources alxFifo.c[,...]]
#         -Sample 0 = run ALL mutants (slow)

param(
    [int]$Sample = 150,
    [string[]]$Sources = @("alxFifo.c")
)
$ErrorActionPreference = "Stop"
$test  = Split-Path $PSScriptRoot -Parent   # Test/ (this script lives in Test/Verify/)
$clib  = Split-Path $test
$Sources = $Sources | ForEach-Object { $_ -split "," }   # -File passes comma-lists as ONE literal string
Set-Location $test
. "$PSScriptRoot\ToolPaths.ps1"

& $python -m pytest -q --collect-only | Out-Null   # dev gate: -Werror build must be fresh
if ($LASTEXITCODE -ne 0) { throw "dev-lane build/collect failed" }

$py    = $python -replace '\\', '/'                # hook templates take POSIX-style paths
$hooks = "$py Test/Verify/mutation_hooks.py"       # relative to $clib = the driver's --root
& $python -m alx.verify.mutation --root "$clib" --tests-dir Test --out "$test\build\mutation" `
    --sample $Sample --seed 1514 `
    --check-cmd "$hooks check {mutant}" --fingerprint-cmd "$hooks fingerprint {mutant}" --rebuild-cmd "$hooks rebuild" `
    @Sources
if ($LASTEXITCODE -ne 0) { throw "mutation run FAILED (runner error or restoration failure)" }
