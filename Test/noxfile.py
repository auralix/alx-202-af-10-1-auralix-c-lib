"""Auralix C Library - the verification lanes (the process is in Test/README.md).

Run from Test/ inside its uv environment (`uv sync --locked` once):

    uv run nox -l                                   list the lanes = the pipeline stages
    uv run nox                                      build, test, analyze, sanitize, coverage
    uv run nox -s coverage                          one lane
    uv run nox -s mutate -- --sample 5 alxFifo.c    MUTATE (report-only); --sample 0 = every mutant
    uv run nox -s test -- -k Fifo                   arguments after -- go to pytest

One nox session per stage, named after it, running in Test/.venv (no second environment per lane), the
same shape as the Auralix Python lib's noxfile and the device repos'. Evidence under build/<stage>/; the
dev lane (pytest) writes to build/ itself. What lives here is what is THIS repository's: the source lists,
the defines, the .def files, the mutation hooks. Everything a second C repository would need identically
comes from the Python lib: the generic gates (alx.verify.ascii_gate / readme_gate / c_style /
coverage_gate / mutation), the lane vocabulary (alx.verify.lanes) and the host toolchain and DLL build
mechanics (alx.c_lib.host_build).
"""

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

import nox

from alx.c_lib import host_build as hb
from alx.verify import lanes

TEST = Path(__file__).resolve().parent
CLIB = TEST.parent
BUILD = TEST / "build"
PYTHON = sys.executable
tc = hb.Toolchain()   # where the tools are on THIS machine (ALX_* variables), and the vcvars environment

nox.options.default_venv_backend = "none"
nox.options.sessions = list(lanes.DEFAULT_SESSIONS)
VENDOR = ["--exclude", "Ext", "--exclude", "FatFs", "--exclude", "mcuboot", "--exclude", "Usbh"]   # third-party folders, not gated

# ---- the module sets (extend per module; keep in sync with conftest's group declarations) -------------------
ANALYSIS_SOURCES = [CLIB / "alxFifo.c", CLIB / "alxBound.c",
                    TEST / "alxFifoTestHelpers.c", TEST / "alxBoundTestHelpers.c", TEST / "alxFifoSanSmoke.c"]
STYLE_FILES = ANALYSIS_SOURCES + [CLIB / "alxFifo.h", CLIB / "alxBound.h"]
INCLUDE_DIRS = [TEST, CLIB, CLIB / "Mcu"]
CL_INCLUDES = [f"/I{d}" for d in INCLUDE_DIRS]
GNU99 = ["/clang:-std=gnu99"]

FIFO_SOURCES = [CLIB / "alxFifo.c", CLIB / "alxBound.c", TEST / "alxFifoTestHelpers.c", TEST / "alxBoundTestHelpers.c"]
FIFO_DEF = TEST / "alxFifoTest.def"

# CLI group (Tier 2: real alxCli + param stack over the fakes), asserts ON = the code as shipped
# (alxParamItem.c has side effects inside its asserts). KEEP IN SYNC WITH conftest.CLI_SOURCES_STRICT/_CLOSURE.
CLI_ASSERTS = ["-DALX_CLI_ASSERT_RST_ENABLE", "-DALX_SERIAL_PORT_ASSERT_RST_ENABLE", "-DALX_FIFO_ASSERT_RST_ENABLE",
               "-DALX_BOUND_ASSERT_RST_ENABLE", "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE", "-DALX_PARAM_MGMT_ASSERT_RST_ENABLE",
               "-DALX_FTOA_ASSERT_RST_ENABLE", "-DALX_RANGE_ASSERT_RST_ENABLE", "-DALX_ID_ASSERT_RST_ENABLE"]
CLI_CLOSURE = [CLIB / "alxParamItem.c", CLIB / "alxParamMgmt.c", CLIB / "alxFtoa.c", CLIB / "alxRange.c"]
CLI_STRICT = [CLIB / "alxCli.c", CLIB / "alxFifo.c", CLIB / "alxBound.c", TEST / "alxSerialPortFake.c",
              TEST / "alxParamKvStoreFake.c", TEST / "alxIdFake.c", TEST / "alxAssertPc.c", TEST / "alxCliTestHelpers.c"]
CLI_DEF = TEST / "alxCliTest.def"
CLI_TESTS = ["test_alxCli.py"]

# MemSafe group: real alxMemSafe/alxCrc/alxParamGroup/alxParamStore over alxMemRawFake.
# KEEP IN SYNC WITH conftest.MEMSAFE_SOURCES_STRICT/_CLOSURE/_ASSERT_DEFINES.
MS_ASSERTS = ["-DALX_MEM_SAFE_ASSERT_RST_ENABLE", "-DALX_MEM_RAW_ASSERT_RST_ENABLE", "-DALX_CRC_ASSERT_RST_ENABLE",
              "-DALX_PARAM_GROUP_ASSERT_RST_ENABLE", "-DALX_PARAM_STORE_ASSERT_RST_ENABLE",
              "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE", "-DALX_BOUND_ASSERT_RST_ENABLE", "-DALX_FTOA_ASSERT_RST_ENABLE",
              "-DALX_RANGE_ASSERT_RST_ENABLE"]
MS_CLOSURE = [CLIB / "alxParamGroup.c", CLIB / "alxParamStore.c", CLIB / "alxParamItem.c", CLIB / "alxFtoa.c",
              CLIB / "alxRange.c"]
MS_STRICT = [CLIB / "alxMemSafe.c", CLIB / "alxCrc.c", CLIB / "alxBound.c", TEST / "alxMemRawFake.c",
             TEST / "alxParamKvStoreFake.c", TEST / "alxAssertPc.c", TEST / "alxMemSafeTestHelpers.c"]
MS_DEF = TEST / "alxMemSafeTest.def"
MS_TESTS = ["test_alxCrc.py", "test_alxMemSafe.py", "test_alxParamGroup.py", "test_alxParamStore.py"]

UBSAN = list(hb.UBSAN)                 # the instrumented variants are the library's flag sets: one
ASAN_UBSAN = list(hb.ASAN_UBSAN)       # definition for every C repository, not a copy per noxfile
PROFILE = list(hb.PROFILE)


# ---- helpers (the lane vocabulary itself is alx.verify.lanes) ----------------------------------------------
def _strs(paths) -> list:
    return [str(p) for p in paths]


def _fresh_dev_build(session: nox.Session) -> None:
    """The dev gate every lane starts with: conftest's -Werror DLL build must be fresh (compile DB too)."""
    session.run(PYTHON, "-m", "pytest", "-q", "--collect-only", silent=True)


def _clang_cl(session: nox.Session, *args, cwd: Path = None) -> None:
    """clang-cl inside the vcvars environment; cwd = where the objects of a /c compile land."""
    if cwd is None:
        session.run(str(tc.compiler(hb.MSVC)), *args, env=tc.environment(), external=True)
    else:
        with session.chdir(cwd):
            session.run(str(tc.compiler(hb.MSVC)), *args, env=tc.environment(), external=True)


def _closure_objects(session: nox.Session, out_dir: Path, sources, defines, flags) -> list:
    """Two-step group build, step 1: the closure sources compiled with warnings off (conftest's shape)."""
    out_dir.mkdir(parents=True, exist_ok=True)
    _clang_cl(session, *GNU99, *flags, "/w", "-D_CRT_SECURE_NO_WARNINGS", *defines, *CL_INCLUDES, "/c", *_strs(sources),
              cwd=out_dir)
    return _strs(sorted(out_dir.glob("*.obj")))


def _dll(session: nox.Session, dll: Path, sources, def_file: Path, flags, defines=(), objects=()) -> None:
    """Two-step group build, step 2 (or the whole build of a one-step group): the strict sources into the DLL."""
    _clang_cl(session, "/LD", *GNU99, *flags, *defines, *CL_INCLUDES, *_strs(sources), *objects,
              f"/Fe:{dll}", f"/Fo{dll.parent}\\", "/link", f"/DEF:{def_file}")


def _pytest(session: nox.Session, env: dict, *args) -> None:
    session.run(PYTHON, "-m", "pytest", "-q", *args, env=env)


def _write(path: Path, text: str) -> None:
    path.write_text(text, encoding="ascii", errors="replace")


# ---- the lanes ---------------------------------------------------------------------------------------------
@nox.session
def build(session: nox.Session) -> None:
    """BUILD - HOST: every test group's DLL with conftest's -Werror recipe (clang, gnu99) + compile_commands.json."""
    session.run(PYTHON, "-m", "pytest", "-q", "--collect-only")
    session.log(f"BUILD CLEAN - {sorted(p.name for p in BUILD.glob('*.dll'))} in {BUILD}")


@nox.session
def test(session: nox.Session) -> None:
    """TEST - HOST: the suite over the dev DLLs; evidence build/pytest_report.xml + .html."""
    session.run(PYTHON, "-m", "pytest", *session.posargs)


@nox.session
def analyze(session: nox.Session) -> None:
    """ANALYZE: 0 codespell + ASCII + README + style gates, 1 clang-tidy, 2 cppcheck (unix32 + win64), 3 gcc -fanalyzer."""
    out = lanes.evidence_dir(TEST, "analyze")
    if not (BUILD / "compile_commands.json").exists():
        _fresh_dev_build(session)
    session.log("Stage 0: codespell, ASCII gate, README gate, style gate")
    session.run(PYTHON, "-m", "codespell_lib", *_strs(ANALYSIS_SOURCES))
    session.run(PYTHON, "-m", "alx.verify.ascii_gate", str(CLIB), *VENDOR, "--out", str(out / "ascii_gate.txt"))
    session.run(PYTHON, "-m", "alx.verify.readme_gate", str(CLIB), *VENDOR, "--out", str(out / "readme_gate.txt"))
    session.run(PYTHON, "-m", "alx.verify.c_style", *_strs(STYLE_FILES), "--out", str(out / "c_style.txt"))
    session.log("Stage 1: clang-tidy")
    session.run(str(tc.llvm("clang-tidy")), "--quiet", "-p", str(BUILD), *_strs(ANALYSIS_SOURCES), external=True,
                stderr=subprocess.DEVNULL)
    session.log("Stage 2: cppcheck (unix32 = the Cortex-M widths, plain char UNSIGNED per AAPCS; win64 = the host)")
    common = ["--std=c99", "--inline-suppr", "--error-exitcode=1", "--quiet", "--suppress=missingIncludeSystem",
              "--suppress=unusedFunction", *[f"-I{d}" for d in INCLUDE_DIRS], *_strs(ANALYSIS_SOURCES)]
    cpp = str(tc.cppcheck())
    session.run(cpp, "--platform=unix32", "--funsigned-char", "--enable=warning,style,performance,portability",
                *common, external=True)
    session.run(cpp, "--platform=win64", "--enable=warning,portability", *common, external=True)
    session.log("Stage 3: gcc -fanalyzer (arm-gcc, target flags)")
    log = out / "fanalyzer.txt"
    lines = []
    for src in ANALYSIS_SOURCES:
        result = subprocess.run([str(tc.armgcc()), "-c", "-std=gnu99", "-mcpu=cortex-m0plus", "-mthumb", "-fanalyzer",
                                 *[f"-I{d}" for d in INCLUDE_DIRS], str(src), "-o", str(out / f"{src.stem}.o")],
                                capture_output=True, text=True)
        lines.append(result.stderr)
        if result.returncode != 0:
            _write(log, "".join(lines))
            session.error(f"Stage 3 FAILED: {src.name} did not compile - see {log}")
    _write(log, "".join(lines))
    if "-Wanalyzer" in "".join(lines):
        session.error(f"Stage 3 FAILED: -fanalyzer findings - see {log}")
    session.log(f"ANALYZE CLEAN - evidence in {out}")


@nox.session
def sanitize(session: nox.Session) -> None:
    """SANITIZE: 1 native ASan+UBSan smoke exe (diagnostics), 2 UBSan DLL per test group under its suite."""
    _fresh_dev_build(session)
    asan, ubsan = lanes.evidence_dir(TEST, "sanitize", "asan"), lanes.evidence_dir(TEST, "sanitize", "ubsan")
    session.log("Stage 1: native ASan+UBSan smoke exe")
    _clang_cl(session, *GNU99, *ASAN_UBSAN, "/Z7", "/MT", *CL_INCLUDES,
              str(CLIB / "alxFifo.c"), str(CLIB / "alxBound.c"), str(TEST / "alxFifoSanSmoke.c"),
              f"/Fe:{asan / 'alxFifoSanSmoke.exe'}", f"/Fo{asan}\\")
    shutil.copy(tc.asan_runtime(), asan)
    session.run(str(asan / "alxFifoSanSmoke.exe"), external=True)
    session.log("Stage 2: UBSan FIFO DLL, full suite")
    _dll(session, ubsan / "alxFifoTest.dll", FIFO_SOURCES, FIFO_DEF, UBSAN)
    _pytest(session, {"ALX_FIFO_TEST_DLL": str(ubsan / "alxFifoTest.dll")})
    session.log("Stage 2b: UBSan CLI DLL, CLI suite")
    objs = _closure_objects(session, ubsan / "cliClosure", CLI_CLOSURE, CLI_ASSERTS, UBSAN)
    _dll(session, ubsan / "alxCliTest.dll", CLI_STRICT, CLI_DEF, UBSAN, ["-D_CRT_SECURE_NO_WARNINGS", *CLI_ASSERTS], objs)
    _pytest(session, {"ALX_CLI_TEST_DLL": str(ubsan / "alxCliTest.dll")}, *CLI_TESTS)
    session.log("Stage 2c: UBSan MemSafe DLL, MemSafe group suite")
    objs = _closure_objects(session, ubsan / "memsafeClosure", MS_CLOSURE, MS_ASSERTS, UBSAN)
    _dll(session, ubsan / "alxMemSafeTest.dll", MS_STRICT, MS_DEF, UBSAN, ["-D_CRT_SECURE_NO_WARNINGS", *MS_ASSERTS], objs)
    _pytest(session, {"ALX_MEMSAFE_TEST_DLL": str(ubsan / "alxMemSafeTest.dll")}, *MS_TESTS)
    session.log("SANITIZE CLEAN")


def _llvm_cov_group(session: nox.Session, out: Path, dll: Path, env_var: str, tests, metrics: str, gated) -> None:
    """Run the group's suite against its instrumented DLL, merge the profiles, report, export, gate."""
    for old in out.glob("*.profraw"):
        old.unlink()
    _pytest(session, {env_var: str(dll), "LLVM_PROFILE_FILE": str(out / "%m-%p.profraw")}, *tests)
    profdata = out / "merged.profdata"
    session.run(str(tc.llvm("llvm-profdata")), "merge", "-sparse", *_strs(out.glob("*.profraw")), "-o", str(profdata),
                external=True)
    cov = str(tc.llvm("llvm-cov"))
    report = session.run(cov, "report", str(dll), f"-instr-profile={profdata}", external=True, silent=True)
    _write(out / "coverage_report.txt", report)
    session.log(report)
    _write(out / "lcov.info", session.run(cov, "export", str(dll), f"-instr-profile={profdata}", "-format=lcov",
                                         external=True, silent=True))
    session.run(PYTHON, "-m", "lcov_cobertura", str(out / "lcov.info"), "--output", str(out / "coverage_c.xml"),
                "--base-dir", str(CLIB))
    session.run(cov, "show", str(dll), f"-instr-profile={profdata}", "-format=html", f"-output-dir={out / 'html'}",
                "-show-branches=count", "-show-line-counts", external=True, silent=True)
    _write(out / "summary.json", session.run(cov, "export", str(dll), f"-instr-profile={profdata}", "-summary-only",
                                            external=True, silent=True))
    session.run(PYTHON, "-m", "alx.verify.coverage_gate", str(out / "summary.json"), "--metrics", metrics,
                "--out", str(out / "coverage_gate.txt"), *gated)
    session.log(f"html: {out / 'html' / 'index.html'}   cobertura: {out / 'coverage_c.xml'}")


@nox.session
def coverage(session: nox.Session) -> None:
    """COVERAGE: clang-instrumented DLL per test group, the same suite, llvm-cov + cobertura, gate per file."""
    _fresh_dev_build(session)
    out = lanes.evidence_dir(TEST, "coverage")
    _dll(session, out / "alxFifoTest.dll", FIFO_SOURCES, FIFO_DEF, PROFILE)
    _llvm_cov_group(session, out, out / "alxFifoTest.dll", "ALX_FIFO_TEST_DLL", [],
                    "lines,branches,regions,functions", ["alxFifo.c", "alxBound.c"])
    # MemSafe group: gate = functions 100 %; lines/branches are REPORTED - alxMemSafe.c keeps three blocks unreachable
    # with asserts ON and alxCrc.c has `break` after `return` plus assert-guarded default branches (task notes).
    ms = lanes.evidence_dir(TEST, "coverage", "memsafe")
    objs = _closure_objects(session, ms / "closure", MS_CLOSURE, MS_ASSERTS, PROFILE)
    _dll(session, ms / "alxMemSafeTest.dll", MS_STRICT, MS_DEF, PROFILE, ["-D_CRT_SECURE_NO_WARNINGS", *MS_ASSERTS], objs)
    _llvm_cov_group(session, ms, ms / "alxMemSafeTest.dll", "ALX_MEMSAFE_TEST_DLL", MS_TESTS,
                    "functions", ["alxCrc.c", "alxMemSafe.c"])
    session.log("COVERAGE GATES PASS")


@nox.session
def mutate(session: nox.Session) -> None:
    """MUTATE (report-only): alx.verify.mutation with the C hooks; `-- [--sample N] [alxFoo.c ...]`."""
    ap = argparse.ArgumentParser(prog="nox -s mutate --")
    ap.add_argument("--sample", type=int, default=150, help="mutants per source, 0 = all (slow)")
    ap.add_argument("sources", nargs="*", default=["alxFifo.c"], help="library sources (default alxFifo.c)")
    args = ap.parse_args(session.posargs)
    _fresh_dev_build(session)
    hooks = f"{Path(PYTHON).as_posix()} Test/Verify/mutation_hooks.py"   # templates take POSIX paths, relative to --root
    session.run(PYTHON, "-m", "alx.verify.mutation", "--root", str(CLIB), "--tests-dir", "Test",
                "--out", str(BUILD / "mutate"), "--sample", str(args.sample), "--seed", "1514",
                "--check-cmd", f"{hooks} check {{mutant}}", "--fingerprint-cmd", f"{hooks} fingerprint {{mutant}}",
                "--rebuild-cmd", f"{hooks} rebuild", *args.sources)
