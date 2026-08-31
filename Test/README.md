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
- ASan smoke: `alxFifoAsanSmoke.c` builds with `/fsanitize=address` into
  `build/alxFifoAsanSmoke.exe` (see conftest build command in git history / notes).
