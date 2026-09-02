# Auralix C Library - QUALIFY: mutation run (tests the tests)
#
# universalmutator generates mutants of the gated sources; mutation_run.py plants
# each one, rebuilds the dev DLL and runs the suite. Suite red/hang = KILLED,
# green = SURVIVED (see build/mutation/survivors/*.diff), no-compile = STILLBORN.
# REPORT-ONLY: survivors do not fail the run - they are the output to review.
# 100% kill rate is not the target (equivalent mutants exist).
#
# Usage:  powershell -File RunMutation.ps1 [-Sample 150] [-Sources alxFifo.c[,...]]
#         -Sample 0 = run ALL mutants (slow: ~888 x ~8 s for alxFifo.c)

param(
    [int]$Sample = 150,
    [string[]]$Sources = @("alxFifo.c")
)
$ErrorActionPreference = "Stop"
$test = $PSScriptRoot
$Sources = $Sources | ForEach-Object { $_ -split "," }   # -File passes comma-lists as ONE literal string

python -m pytest -q --collect-only | Out-Null   # dev gate: -Werror build must be fresh
if ($LASTEXITCODE -ne 0) { throw "dev-lane build/collect failed" }

python "$test\mutation_run.py" --sample $Sample $Sources
if ($LASTEXITCODE -ne 0) { throw "mutation run FAILED (runner error or restoration failure)" }
