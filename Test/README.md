# Auralix C Library - PC Unit Test Stack

**Spec V1 (draft, ALX-1514 pilot). Normative for testing Tier 1/2 c-lib modules on PC.**
Rationale, measurements and rejected alternatives: internal engineering wiki
(AuralixCCppLibWiki) and the ALX-1514 task record. This file says WHAT; the wiki says WHY.
Rule: this spec states only what IS implemented and enforceable - normative changes land
in the same commit as their implementation. Status/backlog live in Jira and the wiki, never here.

## 1. Scope - module tiers

| Tier | Definition | Tested how |
|---|---|---|
| 1 | pure C over memory, no hardware-shaped dependency (alxFifo, alxCrc, alxCanParser, ...) | directly via this stack |
| 2 | pure logic calling extern hardware-shaped APIs (alxCli over AlxSerialPort, alxMemSafe over AlxMemRaw) | this stack + link-time fakes (section 8) |
| 3 | MCU ports (`Mcu/**`), registers/ISRs | NOT here - target/HIL only |

## 2. Quick start

```
cd Test
python -m pytest                          # dev loop (~0.3 s)
powershell -File RunCoverage.ps1          # coverage variant -> txt + HTML + cobertura
powershell -File RunSanitizers.ps1        # ASan+UBSan legs -> "SANITIZERS CLEAN"
```
Reproducible environment: `uv sync --locked && uv run pytest` (uv.lock + .python-version committed).

## 3. The stack, one table

| Layer | Standard | Proves |
|---|---|---|
| Test harness | pytest + ctypes over a per-module DLL built from the REAL sources | the module contract |
| Module compile (dev) | **clang, `-std=gnu99`** (dialect parity with the arm-gcc target build), warning set section 5, `-Werror` | code compiles clean in the SHIPPED dialect |
| Coverage variant | clang `-fprofile-instr-generate -fcoverage-mapping` + llvm-cov | 100% line+branch on changed code (measured, never claimed) |
| Sanitizer variants | native ASan+UBSan smoke exe (diagnostics) + UBSan DLL under the full suite (coverage vehicle) | memory safety, no UB |
| Static analysis | clang-tidy (module gate) / cppcheck / gcc `-fanalyzer` - see section 7 | bug classes tests cannot see |
| Evidence | junit XML + HTML report per run; coverage txt/HTML/cobertura; sanitizer logs | every claim has an artifact |

## 4. C dialect - NORMATIVE

Host test builds compile **`-std=gnu99`** - the exact dialect of the target firmware build
(arm-none-eabi-gcc). Never test a dialect you do not ship.
- MSVC (`cl.exe`) has no C99 mode (floor C11) and cannot parse the library's GNU extensions
  -> **not used** for module builds. clang is the host compiler.
- Warning lane runs at **`-O1`** (`-Wnull-dereference`, `-Warray-bounds` etc. are inert at -O0).
  Coverage/debug legs stay `-O0 -g`.
- **clang-cl trap**: under clang-cl, `-Wall` means MSVC `/Wall` = clang `-Weverything`.
  Pass GNU flags as `/clang:-Wall`, or use the plain `clang` driver.

## 5. Warning set - NORMATIVE (host lane, blanket -Werror)

```
-Wall -Wextra
-Wshadow -Wstrict-prototypes -Wold-style-definition
-Wmissing-prototypes -Wmissing-declarations -Wmissing-variable-declarations
-Wredundant-decls -Wnested-externs -Wbad-function-cast
-Wcast-qual -Wwrite-strings -Wundef -Wvla -Walloca -Wjump-misses-init
-Wswitch-enum -Wswitch-default -Wenum-conversion
-Wformat=2 -Wfloat-equal -Wdouble-promotion -Wimplicit-fallthrough
-Wnull-dereference -Wunused -Wunused-macros -Wno-unused-parameter
```
Deferred until measured/cleaned: `-Wconversion -Wsign-conversion`; `-Wpedantic` report-only.
Blanket `-Werror` applies to the HOST lane only, never to target/release builds.

## 6. Per-module structure - NORMATIVE

For module `alxFoo`:
```
Test/
  alxFooTestHelpers.c      opaque handle New/Delete + status-enum getter functions
  alxFooTest.def           DLL exports (public API + helpers)
  test_<JIRAKEY>_alxfoo.py tests; names test_<JIRAKEY>_P<n>_<what> map to TaskSpec PROOF rows
```
Conventions (all mandatory):
- Tests use ONLY the public module API via opaque handles - no struct mirroring in Python.
- Enum values come from exported getters, never hardcoded.
- Out-buffers are poison-filled (0xAA) and checked beyond the NUL terminator.
- Backing buffers in helpers are poisoned with a DELIMITER byte so over-scans fail
  deterministically, not by luck.
- Property tests compare against a Python reference model with fixed seeds.
- Expected values come from the contract / model / recorded goldens - never from the
  implementation under test.
- Sealed proof: the test commit precedes the implementation commit and is demonstrated RED
  (precedent: ALX-1514, commits 7c98879 -> 1efb914).
- One DLL per module/test-group: link-time fakes must never collide.

## 7. Static analysis on the module - normative order (legs land per module as adopted)

1. **clang-tidy** (config `.clang-tidy`, target-mode flags) - gate: zero findings in
   `WarningsAsErrors` set. Never bare `NOLINT`; always `NOLINT(check)` + reason.
2. **cppcheck** `--std=c99 --platform=unix32 --enable=warning,style,portability` - one pass per
   `#ifdef` configuration the module has. Never `--enable=all` in a gate (unusedFunction is
   documented broken for libraries).
3. **gcc `-fanalyzer`** (arm-gcc 14.2.Rel1, analysis-only leg) - interprocedural leak/UAF/taint.

## 8. Tier 2 - fakes and shims

- The test DLL provides its own implementation of the hardware-shaped extern API
  (e.g. fake `AlxSerialPort_*` backed by an `AlxFifo`) - injection by the linker, zero source
  changes to the module under test.
- CMSIS intrinsics used by some modules (`NVIC_SystemReset`, `__disable_irq`, ...) get a 2-line
  shim in the helpers file.
- The fake is test infrastructure: it lives in `Test/`, is itself poison-disciplined, and its
  behavior is asserted in at least one meta-test.

## 9. Depth ladder + gates (the instruction vocabulary)

L1 smoke -> L2 contract (every documented behavior/status) -> L3 boundary (BVA + wrap/state tour)
-> L4 adversarial (property/model tests, negative tests, sanitizers) -> **L5 qualified**
(L4 + measured 100% line+branch on changed code + mutation spot-checks killed + red-before-green
evidence). **Default instruction for Tier 1/2 modules: "Test at L5".**

Zero-test guards: pytest exit code 5 is never swallowed; junit `tests=` count is the
minimum-count assert once CI exists.
