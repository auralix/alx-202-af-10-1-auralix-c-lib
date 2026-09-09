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
	- `Test/noxfile.py` -> `build`
- **Files - Generated**
	- `Test/build/alx<Module>Test.dll` -> `Test/build/alxFifoTest.dll`

## TEST - HOST
- **Tools**
	- python >= 3.10
	- pytest + plugins: pytest-html, pytest-timeout, pytest-randomly
	- ctypes
	- uv (`Test/.venv` = the locked environment of the dev loop and of every lane)
	- nox (the lane runner: one session per pipeline stage, `uv run nox -s <stage>`)
	- Auralix Python lib `alx.verify.evidence` (pytest plugin: proof token and `req` marker -> junit properties)
- **Files - Config**
	- `Test/pyproject.toml` (dependency `alx-202-2-af-1-auralix-py-lib @ git+...@v0.1.0`)
	- `Test/uv.lock`
	- `Test/.python-version`
- **Files - Code**
	- `Test/conftest.py`
	- `Test/noxfile.py` -> `test`
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
	- `Test/noxfile.py` -> `analyze`
	- `Test/Verify/toolchain.py` (tool locations, vcvars environment)
	- `Test/Verify/style_gate.py`
- **Files - Generated**
	- `Test/build/analyze/ascii_gate.txt`
	- `Test/build/analyze/fanalyzer.txt`

## SANITIZE
- **Tools**
	- clang-cl ASan + UBSan `-fsanitize=address,undefined` -> Stage 1 = `alx<Module>SanSmoke.exe` -> `alxFifoSanSmoke.exe`
	- clang-cl UBSan `-fsanitize=undefined` -> Stage 2 = `alx<Module>Test.dll` & pytest `test_alx<Module>.py`, one stage per test group (asserts ON, as shipped) -> `alxFifoTest.dll` & `test_alxFifo.py`
- **Files - Code**
	- `Test/noxfile.py` -> `sanitize`
	- `Test/alx<Module>SanSmoke.c` -> `Test/alxFifoSanSmoke.c`
- **Files - Generated**
	- `Test/build/sanitize/asan/alx<Module>SanSmoke.exe` -> `Test/build/sanitize/asan/alxFifoSanSmoke.exe`
	- `Test/build/sanitize/asan/clang_rt.asan_dynamic-x86_64.dll`
	- `Test/build/sanitize/ubsan/alx<Module>Test.dll` -> `Test/build/sanitize/ubsan/alxFifoTest.dll`

## COVERAGE
- **Tools**
	- clang-cl `-fprofile-instr-generate -fcoverage-mapping`
	- llvm-profdata + llvm-cov
	- lcov-cobertura
	- `alx.verify.coverage_gate` (Python lib; gate over llvm-cov's summary.json, `--metrics`)
- **Files - Code**
	- `Test/noxfile.py` -> `coverage`
- **Files - Generated**
	- `Test/build/coverage/alx<Module>Test.dll` -> `Test/build/coverage/alxFifoTest.dll`
	- `Test/build/coverage/*.profraw` -> `merged.profdata`
	- `Test/build/coverage/coverage_report.txt` + `html/index.html`
	- `Test/build/coverage/lcov.info` -> `coverage_c.xml` (cobertura)
	- `Test/build/coverage/summary.json` (gate input), `coverage_gate.txt` (gate report)
	- `Test/build/coverage/<group>/` (further test groups, same set of files)

## MUTATE
- **Tools**
	- `alx.verify.mutation` (Python lib: the driver - generate, plant, run the mirror test module, classify, restore, report)
	- universalmutator (mutant generation)
	- clang `-fsyntax-only` + TCE object-compare (the C hooks: check, fingerprint) + conftest's `-Werror` DLL rebuild (hook: rebuild)
- **Files - Code**
	- `Test/noxfile.py` -> `mutate`
	- `Test/Verify/mutation_hooks.py`
- **Files - Generated**
	- `Test/build/mutate/mutants/<module>/`
	- `Test/build/mutate/survivors/*.diff`
	- `Test/build/mutate/report.txt`, `results.json`
	- `Test/build/mutate/backup/` (the planted source, until restored)

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
	- `Test/pyproject.toml` + `Test/uv.lock` (uv project; the Python lib from the `Sub/` gitlink as an editable path source)
- **Files - Code** (device repo)
	- `Test/conftest.py` (bench roles; product values: MCU, memory map, supply policy)
	- `Test/test_<subsystem>.py`
	- `Test/noxfile.py` -> `hil` (BUILD - TARGET, FLASH, TEST - TARGET in one session)
	- `Test/flash.py` (FLASH through the Python lib's debug probe)
	- `Test/<product table>.py` (firmware variables watched over the debug probe, mechanism `alx.fw.live_watch`) -> `Test/dut_watch.py`
	- instrument drivers and probe adapters come from the Python lib (`alx.psu`, `alx.debug_probe`), never from the device repo
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
uv sync --locked                               # once: Test/.venv with every tool; the lanes run in it
python -m pytest                               # dev loop (Test/.venv active, or uv run pytest)
uv run nox -l                                  # the lanes = the pipeline stages
uv run nox                                     # build, test, analyze, sanitize, coverage
uv run nox -s coverage                         # one lane
uv run nox -s mutate -- --sample 5 alxFifo.c   # mutation (report-only); --sample 0 = every mutant
```

`Test/` = the suite (tests, conftest, C helpers and fakes); `Test/Verify/` = the lane side (`toolchain.py` =
where the tools are and the vcvars environment; the C-specific `style_gate.py` and `mutation_hooks.py`);
`Test/noxfile.py` = the lane runner: one nox session per pipeline stage, named after it, running in
`Test/.venv` (`uv run nox`). The generic gates come from the Auralix Python lib (`python -m alx.verify.<gate>`),
pinned by tag in `pyproject.toml`. Tool locations are machine configuration: `ALX_LLVM_DIR` / `ALX_ARMGCC` /
`ALX_CPPCHECK` override the reference-bench defaults in `Verify/toolchain.py`. A missing tool fails its
lane, never skips it.

Naming across the Auralix repositories: folders follow the repository's convention (`Test/`, `Test/Verify/`
here; `tests/`, `alx/verify/` in the Python lib; `Tests/`, `Tests/Verify/` in the C# lib); files follow the
convention of their language wherever they live (Python snake_case, C `alx` + CamelCase, pytest
`test_<module as spelled>.py`); the same WORD names the same role everywhere, only the casing changes. Lane
names are the stage words (`build test analyze sanitize coverage mutate`) in every repository, and so are
the evidence folders `build/<stage>/`.

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
- Mutation (report-only): `alx.verify.mutation` drives it (`uv run nox -s mutate -- alx<Module>.c`):
  universalmutator mutants of the source, `-fsyntax-only` check and object-code fingerprint (TCE) as the
  C hooks, each mutant planted, the stale test DLL rebuilt under `-Werror` (failure = KILLED_COMPILE), the
  source's MIRROR test module `test_alx<Module>.py` run (`-x`, fixed order), source restored (crash-safe
  backup). Survivors -> `build/mutate/survivors/*.diff`; a real hole gets a killing test (P-group
  "mutation-driven hardening"), an equivalent mutant gets a note. 100 % is not the target.
- `build/` layout: root = dev lane; one subfolder per lane, named after the stage (`analyze/`, `sanitize/asan/`,
  `sanitize/ubsan/`, `coverage/`, `mutate/`).
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
  --exclude FatFs --exclude mcuboot --exclude Usbh`, ANALYZE stage 0).
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
