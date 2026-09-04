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

## COMPILE - HOST
- **Tools**
	- clang `-std=gnu99 -O0 -g -Werror` + warning_flags
	- vswhere -> vcvars64 (VS2022 C++ workload) = build env for all lanes
- **Files - Config**
	- `Test/alxConfig.h`
- **Files - Code**
	- `Test/conftest.py`
- **Files - Generated**
	- `Test/build/alxFifoTest.dll`

## TEST - HOST
- **Tools**
	- python >= 3.10
	- pytest + plugins: pytest-html, pytest-timeout, pytest-randomly
	- ctypes
	- uv
- **Files - Config**
	- `Test/pyproject.toml`
	- `Test/uv.lock`
	- `Test/.python-version`
- **Files - Code**
	- `Test/conftest.py`
	- `Test/test_alxFifo.py`
	- `Test/alxFifoTestHelpers.c`
	- `Test/alxFifoTest.def`
	- `Test/alxSerialPortFake.c`
- **Files - Generated**
	- `Test/build/pytest_report.xml`
	- `Test/build/pytest_report.html`

## ANALYZE
- **Tools**
	- codespell + ascii_gate.py + style_gate.py -> Stage 0
	- clang-tidy -> Stage 1
	- cppcheck -> Stage 2
		- `--platform=unix32 --funsigned-char` -> Cortex-M
		- `--platform=win64` -> PC Host
	- arm-gcc 15.2.Rel1 `-fanalyzer` -> Stage 3
- **Files - Config**
	- `.clang-tidy`
- **Files - Code**
	- `Test/RunStaticAnalysis.ps1`
	- `Test/ToolPaths.ps1`
	- `Test/ascii_gate.py`
	- `Test/style_gate.py`
- **Files - Generated**
	- `Test/build/analysis/fanalyzer.txt`

## SANITIZE
- **Tools**
	- clang-cl ASan + UBSan `-fsanitize=address,undefined` -> Stage 1 = `alxFifoSanSmoke.exe`
	- clang-cl UBSan `-fsanitize=undefined` -> Stage 2 = `alxFifoTest.dll` & pytest
	- clang-cl UBSan `-fsanitize=undefined` -> Stage 2b = `alxCliTest.dll` & pytest `test_alxCli.py` (asserts ON, as shipped)
	- clang-cl UBSan `-fsanitize=undefined` -> Stage 2c = `alxMemSafeTest.dll` & pytest MemSafe group (alxCrc/alxMemSafe/alxParamGroup/alxParamStore; asserts ON)
- **Files - Code**
	- `Test/RunSanitizers.ps1`
	- `Test/alxFifoSanSmoke.c`
- **Files - Generated**
	- `Test/build/asan/alxFifoSanSmoke.exe`
	- `Test/build/asan/clang_rt.asan_dynamic-x86_64.dll`
	- `Test/build/ubsan/alxFifoTest.dll`
	- `Test/build/ubsan/alxCliTest.dll`
	- `Test/build/ubsan/alxMemSafeTest.dll`

## COVERAGE
- **Tools**
	- clang-cl `-fprofile-instr-generate -fcoverage-mapping`
	- llvm-profdata + llvm-cov
	- lcov-cobertura
- **Files - Code**
	- `Test/RunCoverage.ps1`
	- `Test/coverage_gate.py` (gate)
- **Files - Generated**
	- `Test/build/cov/alxFifoTest.dll`
	- `Test/build/cov/*.profraw` -> `merged.profdata`
	- `Test/build/cov/coverage_report.txt` + `html/index.html`
	- `Test/build/cov/lcov.info` -> `coverage_c.xml` (cobertura)
	- `Test/build/cov/summary.json` (gate input)
	- `Test/build/cov/memsafe/` (Part B: MemSafe group, same set of files)

## MUTATE
- **Tools**
	- universalmutator (mutant generation)
	- clang `-fsyntax-only` + TCE object-compare (pool filters)
- **Files - Code**
	- `Test/RunMutation.ps1`
	- `Test/mutation_run.py`
- **Files - Generated**
	- `Test/build/mutation/mutants/`
	- `Test/build/mutation/survivors/*.diff`
	- `Test/build/mutation/report.txt`

## COMPILE - TARGET
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
	- `Test/flash.jlink`
- **Files - Generated** (device repo)
	- `Test/build/runs/<timestamp>/`



# AI Notes

---

Tier 1 = pure C modules, tested directly.
Tier 2 = modules with hardware-shaped extern dependencies, tested via link-time fakes.
`Mcu/**` ports are not tested here (target/HIL only).

## Run

```
cd Test
python -m pytest                                                          # dev loop
powershell -NoProfile -ExecutionPolicy Bypass -File RunCoverage.ps1       # coverage + gate
powershell -NoProfile -ExecutionPolicy Bypass -File RunSanitizers.ps1     # ASan + UBSan
powershell -NoProfile -ExecutionPolicy Bypass -File RunStaticAnalysis.ps1 # Stages 0-3
powershell -NoProfile -ExecutionPolicy Bypass -File RunMutation.ps1       # mutation (report-only)
```

Reproducible environment: `uv sync --locked && uv run pytest`.
Tool paths resolve in `ToolPaths.ps1`; override via `ALX_LLVM_DIR` / `ALX_ARMGCC` /
`ALX_CPPCHECK`. A missing tool fails its gate, never skips it.

## Stack

- pytest + ctypes over a per-module DLL built from the real sources.
- Dev DLL: clang `-std=gnu99 -O0 -g -Werror` + warning set below (= target dialect).
  Instrumented variants (sanitizer/coverage): clang-cl inside vcvars, same dialect.
- One suite serves every variant: the `ALX_FIFO_TEST_DLL` env override (conftest fixture)
  points pytest at an instrumented DLL.
- Coverage: clang `-fprofile-instr-generate -fcoverage-mapping` + llvm-cov;
  gate = 100 % lines/branches/regions/functions on gated files (`coverage_gate.py`).
  MemSafe group (Part B): functions gated, lines/branches reported (`--metrics functions`) - alxMemSafe.c
  and alxCrc.c keep assert-guarded unreachable blocks (nonBlocking TODOs, impossible else, default
  branches, `break` after `return`) that no test can execute with asserts ON.
- Sanitizers: native ASan+UBSan smoke exe + UBSan DLLs under the suites (Stage 2 FIFO, 2b CLI, 2c MemSafe).
  UBSAN_OPTIONS: keep `log_path` relative - a drive-letter colon splits the option list.
- Mutation (report-only): universalmutator mutants of the gated sources, each planted,
  rebuilt, suite re-run (`RunMutation.ps1`/`mutation_run.py`). Survivors ->
  `build/mutation/survivors/*.diff`; a real hole gets a killing test (P-group
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
                        module under test (alxSerialPortFake.c, alxParamKvStoreFake.c, alxIdFake.c,
                        alxMemRawFake.c)
```

Test groups (one DLL each): `alxFifoTest.dll` = alxFifo + alxBound; `alxCliTest.dll` = alxCli over
alxSerialPortFake; `alxMemSafeTest.dll` = alxMemSafe + alxCrc (strict) + alxParamGroup + alxParamStore
(closure) over alxMemRawFake - a RAM flash with call counters, fail-nth injection, a power-loss model
(the n-th write is cut after k bytes) and an optional row-erase model, so copy-A/copy-B recovery and
"reboot reads the last or the previous record" are proven on the host. Env overrides for instrumented
variants: `ALX_FIFO_TEST_DLL`, `ALX_CLI_TEST_DLL`, `ALX_MEMSAFE_TEST_DLL`.

## Conventions

- Tests use only the public module API via opaque handles; no struct mirroring in Python.
- Enum values via exported getters, never hardcoded.
- Out-buffers poison-filled (0xAA), checked beyond the NUL terminator.
- Helper backing buffers poisoned with a delimiter byte; helper STRUCTS poisoned 0xFF before the
  Ctor (not 0xAA: clang tests a bool by its low bit, so 0xAA reads as false and hides a
  forgotten flag init - mutation finding ALX-1514).
- Property tests compare against a Python reference model, fixed seeds.
- Test commit precedes implementation commit and is demonstrated failing first.
- One DLL per module/test-group.
- Library sources are pure ASCII (gated: `ascii_gate.py`, Stage 0 of RunStaticAnalysis).
- No ternary operator in gated sources - write if/else (gated: `style_gate.py`, Stage 0).
- Doxygen tag lines: tabs-only field separators; name and description columns each
  aligned within a block, tab stop 4 (gated: `style_gate.py`, Stage 0).
- Test names keep their ORIGINATING proof token forever; later tasks attach
  `@pytest.mark.req("ALX-<key>-P<n>")` instead of renaming. Proof tokens are mirrored
  into junit XML as `<property>` elements (conftest hook).

## Jira

- https://auralix.atlassian.net/browse/ALX-1513
- https://auralix.atlassian.net/browse/ALX-1514
- https://auralix.atlassian.net/browse/ALX-1495
