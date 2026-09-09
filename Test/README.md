# Auralix C Library - Test

---




# Human Notes

---

# Verification Pipeline

## WRITE
- **Tools**
	- Visual Studio Community
	- clangd
	- VisualGDB IntelliSense
- **Files - Config**
	- `.clangd`
	- `.editorconfig`
	- `.gitattributes`
- **Files - Generated**
	- `Test/build/compile_commands.json`

## BUILD - HOST
- **Tools**
	- clang `-std=gnu99 -O0 -g -Werror` + warning_flags
	- vswhere -> vcvars64 (VS2022 C++ workload) = build env for all lanes
- **Files - Config**
	- `Test/alxConfig.h`
- **Files - Code**
	- `Test/conftest.py`
- **Files - Generated**
	- `Test/build/alx<Module>Test.dll` -> `Test/build/alxFifoTest.dll`

## TEST - HOST
- **Tools**
	- python >= 3.10
	- pytest + plugins: pytest-html, pytest-timeout, pytest-randomly
	- ctypes
	- uv (`Test/.venv` = the locked environment every lane script uses)
	- Auralix Python lib `alx.verify.evidence` (pytest plugin: proof token and `req` marker -> junit properties)
- **Files - Config**
	- `Test/pyproject.toml` (dependency `alx-202-2-af-1-auralix-py-lib @ git+...@v0.1.0`)
	- `Test/uv.lock`
	- `Test/.python-version`
- **Files - Code**
	- `Test/conftest.py`
	- `Test/test_alx<Module>.py` -> `Test/test_alxFifo.py`
	- `Test/alx<Module>TestHelpers.c` -> `Test/alxFifoTestHelpers.c`
	- `Test/alx<Module>Test.def` -> `Test/alxFifoTest.def`
	- `Test/alx<FakedModule>Fake.c` -> `Test/alxSerialPortFake.c`
- **Files - Generated**
	- `Test/build/pytest_report.xml`
	- `Test/build/pytest_report.html`

## ANALYZE
- **Tools**
	- codespell + `alx.verify.ascii_gate` (Python lib, `--exclude` for the vendor folders) + style_gate.py -> Stage 0
	- clang-tidy -> Stage 1
	- cppcheck -> Stage 2
		- `--platform=unix32 --funsigned-char` -> Cortex-M
		- `--platform=win64` -> PC Host
	- arm-gcc 15.2.Rel1 `-fanalyzer` -> Stage 3
- **Files - Config**
	- `.clang-tidy`
- **Files - Code**
	- `Test/Verify/RunStaticAnalysis.ps1`
	- `Test/Verify/ToolPaths.ps1`
	- `Test/Verify/style_gate.py`
- **Files - Generated**
	- `Test/build/analysis/ascii_gate.txt`
	- `Test/build/analysis/fanalyzer.txt`

## SANITIZE
- **Tools**
	- clang-cl ASan + UBSan `-fsanitize=address,undefined` -> Stage 1 = `alx<Module>SanSmoke.exe` -> `alxFifoSanSmoke.exe`
	- clang-cl UBSan `-fsanitize=undefined` -> Stage 2 = `alx<Module>Test.dll` & pytest `test_alx<Module>.py`, one stage per test group (asserts ON, as shipped) -> `alxFifoTest.dll` & `test_alxFifo.py`
- **Files - Code**
	- `Test/Verify/RunSanitizers.ps1`
	- `Test/alx<Module>SanSmoke.c` -> `Test/alxFifoSanSmoke.c`
- **Files - Generated**
	- `Test/build/asan/alx<Module>SanSmoke.exe` -> `Test/build/asan/alxFifoSanSmoke.exe`
	- `Test/build/asan/clang_rt.asan_dynamic-x86_64.dll`
	- `Test/build/ubsan/alx<Module>Test.dll` -> `Test/build/ubsan/alxFifoTest.dll`

## COVERAGE
- **Tools**
	- clang-cl `-fprofile-instr-generate -fcoverage-mapping`
	- llvm-profdata + llvm-cov
	- lcov-cobertura
	- `alx.verify.coverage_gate` (Python lib; gate over llvm-cov's summary.json, `--metrics`)
- **Files - Code**
	- `Test/Verify/RunCoverage.ps1`
- **Files - Generated**
	- `Test/build/cov/alx<Module>Test.dll` -> `Test/build/cov/alxFifoTest.dll`
	- `Test/build/cov/*.profraw` -> `merged.profdata`
	- `Test/build/cov/coverage_report.txt` + `html/index.html`
	- `Test/build/cov/lcov.info` -> `coverage_c.xml` (cobertura)
	- `Test/build/cov/summary.json` (gate input), `coverage_gate.txt` (gate report)
	- `Test/build/cov/<group>/` (further test groups, same set of files)

## MUTATE
- **Tools**
	- `alx.verify.mutation` (Python lib: the driver - generate, plant, run the mirror test module, classify, restore, report)
	- universalmutator (mutant generation)
	- clang `-fsyntax-only` + TCE object-compare (the C hooks: check, fingerprint) + conftest's `-Werror` DLL rebuild (hook: rebuild)
- **Files - Code**
	- `Test/Verify/RunMutation.ps1`
	- `Test/Verify/mutation_hooks.py`
- **Files - Generated**
	- `Test/build/mutation/mutants/<module>/`
	- `Test/build/mutation/survivors/*.diff`
	- `Test/build/mutation/report.txt`, `results.json`
	- `Test/build/mutation/backup/` (the planted source, until restored)

## BUILD - TARGET
- **Tools**
	- VisualGDB/MSBuild (today) -> CMake + Ninja (future)
	- arm-none-eabi 10.3.1 + newlib-nano
	- Backlog: `-Wl,--print-memory-usage` + `-Wstack-usage=<bytes>` gates
- **Files - Config** (device repo)
	- `*.vcxproj` / `mcu.props`
	- `*.vgdbsettings`
- **Files - Generated** (device repo)
	- `.bin` / `.elf` / `.map`

## TEST - TARGET
- **Tools**
	- pytest = bench orchestrator
	- instrument drivers per bus (Python libs: serial, CAN, Ethernet, Modbus/PLC, SCPI/VISA, GPIO/relay, debug probe, ...)
- **Files - Config** (device repo)
	- `Test/pyproject.toml`
- **Files - Code** (device repo)
	- `Test/conftest.py`
	- `Test/test_<subsystem>.py`
	- `Test/RunHil.ps1`
	- `Test/<step>.jlink` -> `Test/flash.jlink`
	- `Test/<instrument>.py` (bench instrument driver) -> `Test/owon_p4603.py`
	- `Test/<observer>.py` (firmware RAM view over the debug probe) -> `Test/dut_ram.py`
- **Files - Generated** (device repo)
	- `Test/build/runs/<timestamp>/`



# AI Notes

---

This file holds rules and facts only, in the form abstract name -> one FIFO example; task details, module
inventories and per-group lists belong to the Jira task and its Task folder notes.

Tier 1 = pure C modules, tested directly.
Tier 2 = modules with hardware-shaped extern dependencies, tested via link-time fakes.
`Mcu/**` ports are not tested here (target/HIL only).

## Run

```
cd Test
uv sync --locked                                                                 # once: Test/.venv with every tool
python -m pytest                                                                 # dev loop (Test/.venv active, or uv run pytest)
powershell -NoProfile -ExecutionPolicy Bypass -File Verify\RunCoverage.ps1       # coverage + gate
powershell -NoProfile -ExecutionPolicy Bypass -File Verify\RunSanitizers.ps1     # ASan + UBSan
powershell -NoProfile -ExecutionPolicy Bypass -File Verify\RunStaticAnalysis.ps1 # Stages 0-3
powershell -NoProfile -ExecutionPolicy Bypass -File Verify\RunMutation.ps1       # mutation (report-only)
```

`Test/` = the suite (tests, conftest, C helpers and fakes); `Test/Verify/` = the lane side (the `Run*.ps1`
runners, `ToolPaths.ps1`, the C-specific `style_gate.py` and `mutation_hooks.py`). The generic gates come
from the Auralix Python lib (`python -m alx.verify.<gate>`), pinned by tag in `pyproject.toml`; the lane
scripts run the python of `Test/.venv` when it exists, else the PATH python (`ToolPaths.ps1`).
Tool paths resolve in `Verify/ToolPaths.ps1`; override via `ALX_LLVM_DIR` / `ALX_ARMGCC` /
`ALX_CPPCHECK`. A missing tool fails its gate, never skips it.

## Stack

- pytest + ctypes over a per-module DLL built from the real sources.
- Dev DLL: clang `-std=gnu99 -O0 -g -Werror` + warning set below (= target dialect).
  Instrumented variants (sanitizer/coverage): clang-cl inside vcvars, same dialect.
- One suite serves every variant: the `ALX_<GROUP>_TEST_DLL` env override (conftest fixture) points pytest
  at an instrumented DLL -> `ALX_FIFO_TEST_DLL`.
- Coverage: clang `-fprofile-instr-generate -fcoverage-mapping` + llvm-cov;
  gate = 100 % lines/branches/regions/functions on gated files (`alx.verify.coverage_gate` over
  `summary.json`, `--metrics lines,branches,regions,functions`). A gated file with assert-guarded
  unreachable code is gated on functions only (`--metrics functions`); lines/branches are reported.
  A metric with nothing to cover counts as 100 % (llvm prints 0 %).
- Sanitizers: native ASan+UBSan smoke exe + one UBSan DLL per test group under its suite.
  UBSAN_OPTIONS: keep `log_path` relative - a drive-letter colon splits the option list.
- Mutation (report-only): `alx.verify.mutation` drives it (`Verify/RunMutation.ps1 -Sources alx<Module>.c`):
  universalmutator mutants of the source, `-fsyntax-only` check and object-code fingerprint (TCE) as the
  C hooks, each mutant planted, the stale test DLL rebuilt under `-Werror` (failure = KILLED_COMPILE), the
  source's MIRROR test module `test_alx<Module>.py` run (`-x`, fixed order), source restored (crash-safe
  backup). Survivors -> `build/mutation/survivors/*.diff`; a real hole gets a killing test (P-group
  "mutation-driven hardening"), an equivalent mutant gets a note. 100 % is not the target.
- `build/` layout: root = dev lane; one subfolder per variant (`asan/`, `ubsan/`, `cov/`, `analysis/`, `mutation/`).
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
alxFooTestHelpers.c     opaque-handle New/Delete + status-enum getters
alxFooTest.def          DLL exports
test_alxFoo.py          tests; file = module-scoped, functions = task-scoped: test_<KEY>_P<n>_<what>
alxBarFake.c            Tier-2 link-time fake - named by the FAKED module (Bar), never by the
                        module under test -> alxSerialPortFake.c
```

Test groups (one DLL each) are declared in `conftest.py`: strict sources (warning set, -Werror) + closure
sources (-w) + fakes.

## Conventions

- Tests use only the public module API via opaque handles; no struct mirroring in Python.
- Enum values via exported getters, never hardcoded.
- Out-buffers poison-filled (0xAA), checked beyond the NUL terminator.
- Helper backing buffers poisoned with a delimiter byte; helper STRUCTS poisoned 0xFF before the
  Ctor (not 0xAA: clang tests a bool by its low bit, so 0xAA reads as false and hides a
  forgotten flag init - mutation finding ALX-1514).
- Property tests compare against a Python reference model, fixed seeds.
- Test commit precedes implementation commit and is demonstrated failing first.
- A known defect is sealed as `xfail(strict=True)` with the finding as the reason; it XPASSes when fixed
  and the marker is removed in the green commit. `xfail(strict=False)` only for a documented flaky
  product behaviour, never to hide a red test.
- CHARACTERIZATION tests (docstring prefix) pin behaviour that is not a requirement, so a change is noticed.
- Metric tests record numbers (junit properties, run log) and assert only a sanity bound.
- The repository is pure ASCII outside the vendor folders (gated: `alx.verify.ascii_gate --exclude Ext
  --exclude FatFs --exclude mcuboot --exclude Usbh`, Stage 0 of RunStaticAnalysis).
- No ternary operator in gated sources - write if/else (gated: `style_gate.py`, Stage 0).
- Doxygen tag lines: tabs-only field separators; name and description columns each
  aligned within a block, tab stop 4 (gated: `style_gate.py`, Stage 0).
- Test names keep their ORIGINATING proof token forever; later tasks attach
  `@pytest.mark.req("ALX-<key>-P<n>")` instead of renaming. Proof tokens are mirrored
  into junit XML as `<property>` elements by the Python lib's plugin (`pytest_plugins =
  ("alx.verify.evidence",)` in conftest).

## Jira

- https://auralix.atlassian.net/browse/ALX-1513
- https://auralix.atlassian.net/browse/ALX-1514
- https://auralix.atlassian.net/browse/ALX-1495
- https://auralix.atlassian.net/browse/ALX-1544
