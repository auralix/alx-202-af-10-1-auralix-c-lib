# Auralix C Library - Test

---

# Human Notes

## WRITE
- **Tools**
	- Visual Studio Community
	- clangd
	- VisualGDB IntelliSense
- **Files - Config**
	- `.clangd`
	- `.editorconfig`
- **Files - Generated**
	- `Test/build/compile_commands.json`

## COMPILE
- **Tools**
	- clang `-std=gnu99 -O0 -g -Werror` + warning_flags
- **Files - Config**
	- `Test/alxConfig.h`
- **Files - Code**
	- `Test/conftest.py`

## TEST
- **Tools**
	- pytest
	- ctypes
	- uv
- **Files - Config**
	- `Test/pyproject.toml`
	- `Test/uv.lock`
- **Files - Code**
	- `Test/conftest.py`
	- `Test/test_alx<Module>.py` -> `Test/test_alxFifo.py`
	- `Test/alx<Module>TestHelpers.c` -> `Test/alxFifoTestHelpers.c`
	- `Test/alx<FakedModule>Fake.c` -> `Test/alxSerialPortFake.c`
	- `Test/alx<Module>Test.def` -> `Test/alxFifoTest.def`
- **Files - Generated**
	- `Test/build/pytest_report.xml`
	- `Test/build/pytest_report.html`

---

# AI Notes

Tier 1 = pure C modules, tested directly.
Tier 2 = modules with hardware-shaped extern dependencies, tested via link-time fakes.
`Mcu/**` ports are not tested here (target/HIL only).

## Run

```
cd Test
python -m pytest                              # dev loop
powershell -File RunCoverage.ps1              # coverage -> txt + HTML + cobertura
powershell -File RunSanitizers.ps1            # ASan + UBSan
powershell -File RunStaticAnalysis.ps1        # clang-tidy, cppcheck, gcc -fanalyzer
```

Reproducible environment: `uv sync --locked && uv run pytest`.

## Stack

- pytest + ctypes over a per-module DLL built from the real sources.
- Module builds: clang `-std=gnu99 -O0 -g -Werror` + warning set below (= target dialect).
- Coverage: clang `-fprofile-instr-generate` + llvm-cov (line + branch).
- Sanitizers: native ASan+UBSan smoke exe + UBSan DLL under the full suite.
- Evidence per run: `build/pytest_report.xml` (junit), `build/pytest_report.html`.

## Warning set

```
-Wall -Wextra
-Wshadow -Wstrict-prototypes -Wold-style-definition
-Wmissing-prototypes -Wmissing-declarations -Wmissing-variable-declarations
-Wredundant-decls -Wnested-externs -Wbad-function-cast
-Wcast-qual -Wwrite-strings -Wundef -Wvla -Walloca
-Wswitch-enum -Wswitch-default -Wenum-conversion
-Wformat=2 -Wfloat-equal -Wdouble-promotion -Wimplicit-fallthrough
-Wnull-dereference -Wunused -Wunused-macros -Wno-unused-parameter
```

## Per-module files

```
alxFooTestHelpers.c     opaque-handle New/Delete + status-enum getters (+ fakes for Tier 2)
alxFooTest.def          DLL exports
test_alxFoo.py          tests; file = module-scoped, functions = task-scoped: test_<KEY>_P<n>_<what>
```

## Conventions

- Tests use only the public module API via opaque handles; no struct mirroring in Python.
- Enum values via exported getters, never hardcoded.
- Out-buffers poison-filled (0xAA), checked beyond the NUL terminator.
- Helper backing buffers poisoned with a delimiter byte.
- Property tests compare against a Python reference model, fixed seeds.
- Test commit precedes implementation commit and is demonstrated failing first.
- One DLL per module/test-group.
- Library sources are pure ASCII (gated: `ascii_gate.py`, Leg 0 of RunStaticAnalysis).
- Test names keep their ORIGINATING proof token forever; later tasks attach
  `@pytest.mark.req("ALX-<key>-P<n>")` instead of renaming. Proof tokens are mirrored
  into junit XML as `<property>` elements (conftest hook).
