# Auralix C Library - PC Unit Tests

pytest + ctypes over per-module DLLs built from the REAL c-lib sources.
Modules covered so far: **alxFifo** (ALX-1514, first occupant - sets the standard).

## Run

Requirements: Python >= 3.10 with `pytest` + `pytest-html`, Visual Studio 2022
(MSVC found automatically via vswhere). Target standard is uv (`uv sync --locked
&& uv run pytest`); until uv is rolled out:

```
cd Test
python -m pytest
```

The DLL is (re)built automatically by a session fixture when any source changed.

## Evidence artifacts (written on every run, gitignored)

| File | For |
|---|---|
| `build/pytest_report.html` | humans - open in browser |
| `build/pytest_report.xml` | machines - junit XML for CI |

## Useful invocations

```
python -m pytest -v              # per-test list
python -m pytest -k "P4"         # one proof group (TaskSpec PROOF row)
python -m pytest -m unit         # only no-hardware tests
```

## Conventions

- Test names: `test_<JIRAKEY>_P<n>_<what>` - `P<n>` maps to the TaskSpec PROOF row.
- One DLL per module/test-group (link-time fakes must not collide).
- Tests use ONLY the public module API via opaque handles from `alx<Module>TestHelpers.c`
  (no struct mirroring in Python); status enums via exported getter functions.
- Out-buffers are poison-filled (0xAA) and checked beyond the NUL; the FIFO backing
  buffer is poisoned with a delimiter byte so over-scans fail deterministically.
- Property tests compare against a Python reference model with fixed seeds.
- Sealed proof: test commit precedes implementation commit; new-contract tests must
  be demonstrated RED first (see ALX-1514 commits 7c98879 -> 1efb914).

## Build variants (one suite, several binaries)

| Command | Compiler | Proves |
|---|---|---|
| `python -m pytest` | MSVC | the contract (dev loop, 0.3 s) |
| `powershell -File RunCoverage.ps1` | clang | 100% line+branch coverage (txt/HTML/cobertura) |
| `powershell -File RunSanitizers.ps1` | clang | memory safety (ASan) + no undefined behavior (UBSan) |

Sanitizer design is measurement-driven (details in RunSanitizers.ps1 header): the native
ASan+UBSan smoke exe is the diagnostics vehicle; the UBSan DLL under the full pytest suite is
the coverage vehicle; MSVC-ASan-DLL-under-python was MEASURED as non-detecting and is not used.
