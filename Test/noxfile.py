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
import ast
import re
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
sys.path.insert(0, str(TEST))
import conftest as cf  # noqa: E402  the source lists, the build recipes and DLL_GROUPS live with the fixtures

nox.options.default_venv_backend = "none"
nox.options.sessions = list(lanes.DEFAULT_SESSIONS)
# third-party folders, not gated
VENDOR = ["--exclude", "Ext", "--exclude", "FatFs", "--exclude", "mcuboot", "--exclude", "Usbh"]

# ---- the module sets (extend per module; keep in sync with conftest's group declarations) -------------------
ANALYSIS_SOURCES = [CLIB / "alxFifo.c", CLIB / "alxBound.c",
                    TEST / "alxFifoTestHelpers.c", TEST / "alxBoundTestHelpers.c", TEST / "alxFifoSanSmoke.c"]
# every C file this suite OWNS is style-gated, whatever module it serves; the LIBRARY sources
# gated here are still only the ones ANALYSIS_SOURCES covers (see the Jira task, item A13)
# The C style gate reads only what is listed here. Until 12.09 that list was ANALYSIS_SOURCES -
# which exists to name what clang-tidy and cppcheck analyse, 2 of ~30 library modules - plus the
# Test/ sources, so 138 of the library's 140 .c/.h were never checked at all. Nothing chose that;
# the style gate simply borrowed another lane's list and nobody widened it.
#
# It is now every library source EXCEPT the ones that cannot pass yet, named so the debt is visible
# and shrinks by deletion rather than being rediscovered. Measured 12.09: 163 findings in 26 files.
#
# COMMENTS - was 84 findings in 16 files, all "spaces in field separator (tabs only)". Swept 12.09:
# 54 doc blocks, 85 lines, a 1:1 space-for-tab swap that changed no content. The set is empty and
# stays here as the place a new straggler would be named rather than quietly excluded.
STYLE_PENDING_COMMENTS = []

# TERNARIES - 29 findings in 10 files, and this half is NOT a cleanup job. Fifteen of them cannot
# be written as if/else in C at all: they are macro bodies that must expand to an EXPRESSION.
# alxBuild.h builds a compile-time character out of __DATE__ and is used where a constant is
# required; ALX_RTC_DAYS_IN_YEAR(yr) is the same shape; alxLfsConfig.h's are littlefs's own
# vendor shims. Rule 1 as written asks those files for something the language does not offer.
#
# So the last ten need a WAIVER MECHANISM in alx.verify.c_style - a per-line marker, or a rule that
# exempts a #define body - before they can be gated. That is a change to the Python library rather
# than to this repository, and it is on the TODO.
STYLE_PENDING_TERNARY = [
    "alxBuild.h", "alxFs.c", "alxGlobal.c", "alxLfsConfig.h", "alxLogger.c", "alxNet.c",
    "alxOsEventFlagGroup.c", "alxOsThread.c", "alxRtc_Global.c", "alxSocket.c",
]

# Stage 5 compiles every PORTABLE library module - the ones that need no vendor SDK. Three cannot
# join: alxAssert.c, alxBoot.c and alxIrq.c reach for CMSIS intrinsics (__BKPT, __disable_irq,
# __get_PRIMASK) that only an MCU family header supplies, and those headers live in the PRODUCT's
# include path, not this repository's. A family-specific check is therefore a product's job.
SYNTAX_NEEDS_CMSIS = {"alxAssert.c", "alxBoot.c", "alxIrq.c"}

# Stage 5b asks a SECOND question of the same 53 sources: does every path out of a non-void
# function return a value? C99 6.9.1/12 makes reading the result of one that does not undefined.
#
# It needs `-c`, not `-fsyntax-only`, and that distinction is the whole reason this went unasked.
# GCC computes "control reaches end of non-void function" in the CFG pass, which -fsyntax-only
# never runs - so Stage 5 above, and my first attempt at this one, were silent on a control case
# as blunt as `bool f(void) { }`. That silence reads as "clean" and is really "not asked".
#
# Measured 12.09 - EIGHT sites in five modules:
#
#   alxCrc.c                 202, 280   AlxCrc_Calc's and AlxCrc_GetLen's `default:` label. Both
#                                        switch on AlxCrc_Config, both end the default case with
#                                        ALX_CRC_ASSERT(false) and `break`, and a `break` out of a
#                                        switch at the end of a function falls off the end. With
#                                        assertions ELIDED - which is what a module carrying no
#                                        ALX_CRC_ASSERT_*_ENABLE compiles to - an out-of-range
#                                        config returns whatever is in r0. THIS MODULE IS BUILT
#                                        AND TESTED HERE (the MemSafe group, test_alxCrc.py).
#   alxRange.c               296        AlxRange_CheckArr is an unimplemented stub: `// TV: TODO`,
#                                        ALX_RANGE_ASSERT(false), nothing else. The P517 shape and
#                                        worse - AlxMemRaw at least returned a defined, wrong 0.
#                                        Compiled by the MemSafe group; exported by no .def and
#                                        called by nothing on this machine.
#   alxOsMutex.c             118        AlxOsMutex_IsUnlocked
#   alxOsThread.c            260        AlxOsThread_Join
#   alxOsEventFlagGroup.c    213,297,374  _Clear, _Wait, _Sync
#
# The four alxOs sites only bite a build with NEITHER ALX_FREE_RTOS nor ALX_ZEPHYR, where the
# bodies are #if dispatch that selects nothing. Every one of the eight wants a default return, and
# that is a source change in modules no test drives - so they are NAMED rather than fixed in the
# dark, and the other 48 are gated from here on.
RETURN_TYPE_PENDING = {"alxCrc.c", "alxRange.c",
                       "alxOsMutex.c", "alxOsThread.c", "alxOsEventFlagGroup.c"}

# And Stage 5b counts the SYMBOLS each object defines, because "it compiled" is not the same claim
# as "it was read". Three of these modules carry a SECOND tier of guard below ALX_C_LIB - alxNet.c
# wants `&& (ALX_FREE_RTOS_CELLULAR || ALX_WIZNET)` - so with ALX_C_LIB alone they preprocess to an
# empty translation unit and pass every check ever pointed at them.
#
# That is the same trap as the 103-empty-files mistake one tier down, and it took a negative
# control to find: a missing return planted in alxNet.c did NOT fail this stage. (The earlier
# control that appeared to work was planted INSIDE the `#if` line and failed as a broken directive
# - it never proved the body was compiled. Plant in a module named below as non-empty.)
#
# These three are compiled only by a product that selects a driver, so the set is a fact about this
# repository, not a defect. It is asserted in BOTH directions: a fourth module going quiet is a
# regression, and one of these three coming back to life means the set is stale.
SYNTAX_EMPTY_TU = {"alxBuild.c", "alxNet.c", "alxSocket.c"}

# Stage 5c asks the question P567 answered the hard way: does any assertion DO the work?
#
# ALX_<M>_ASSERT(expr) with no assert form compiled expands to do {} while(false) taking NO
# parameters, so the preprocessor deletes `expr` entirely. When expr is a predicate that is the
# point. When expr performs the call the function exists to make, the BEHAVIOUR goes with it, and
# nothing reports an error: alxCli.c writes its replies that way and answers 0 bytes instead of 751.
#
# Measured 12.09 - 80 assertions call something, and they split cleanly:
#
#   49 perform the work     alxCli.c 21, alxIoPin_McuZephyr.c 14, alxParamItem.c 7,
#                            alxClk_McuLpc55S6x.c 5, alxNet.c 2
#   29 call a predicate     strlen, AlxAdc_IsClkOk, AlxWdt_IsClkOk and friends - eliding those
#                            loses a check, which is what an assertion is for
#
# The five files are NAMED rather than fixed: the repair is one line each and it is library source.
# What this stage buys is that the list cannot grow. A new assertion that hides a call in a module
# not below fails the lane.
ASSERT_PURE_CALLS = {
    "strlen", "sizeof",
    "AlxAdc_GetCh", "AlxAdc_Ctor_CheckCh", "AlxAdc_Ctor_IsMainClkOk", "AlxAdc_Ctor_IsSysClkOk",
    "AlxAdc_IsClkOk", "AlxWdt_IsClkOk", "AlxCan_Ctor_IsClkOk", "AlxMmc_IsClkOk", "AlxSpi_IsClkOk",
    "AlxI2c_IsClkOk", "AlxI2s_Ctor_IsClkOk", "AlxPwm_CheckIoPins",
    "AlxParamItem_AreEnumArrValFromLowToHigh", "AlxParamItem_IsEnumOnList",
}
ASSERT_SIDE_EFFECT_PENDING = {"alxCli.c", "alxIoPin_McuZephyr.c", "alxParamItem.c",
                              "alxClk_McuLpc55S6x.c", "alxNet.c"}
STRING_LITERAL = re.compile(r"\"(?:[^\"\\]|\\.)*\"")
CHAR_LITERAL = re.compile(r"'(?:[^'\\]|\\.)'")
ELIDED_ASSERT = re.compile(r"ALX_[A-Z0-9_]*ASSERT[A-Z0-9_]*\s*\(")
ELIDED_TRACE = re.compile(r"ALX_[A-Z0-9_]*TRACE[A-Z0-9_]*\s*\(")
ELIDED_NAME = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")

_STYLE_PENDING = {*STYLE_PENDING_COMMENTS, *STYLE_PENDING_TERNARY}
_STYLE_SKIP_DIRS = {"Ext", "FatFs", "mcuboot", "Usbh", "Test", "build", "Doc"}


def _library_style_files() -> list:
    """Every library .c/.h the gate can check today, vendor trees and the pending list excluded."""
    found = []
    for path in sorted(CLIB.glob("*.[ch]")) + sorted(CLIB.glob("*/*.[ch]")):
        if set(path.relative_to(CLIB).parts[:-1]) & _STYLE_SKIP_DIRS:
            continue
        if path.relative_to(CLIB).as_posix() in _STYLE_PENDING:
            continue
        found.append(path)
    return found


STYLE_FILES = [*_library_style_files(), *sorted(TEST.glob("*.c"))]
# Target-only translation units: compiled by ANALYZE stage 4 with arm-gcc, never linked, one compile
# per part variant. (source, variants to compile it under).
LAYOUT_CHECKS = [(TEST / "alxIna228RegSizeCheck.c", ["ALX_INA238", "ALX_INA228"])]
INCLUDE_DIRS = [TEST, CLIB, CLIB / "Mcu", CLIB / "Ext"]
CL_INCLUDES = [f"/I{d}" for d in INCLUDE_DIRS]
GNU99 = ["/clang:-std=gnu99"]

# alxAssertPc.c is in every group now: it holds the assertion counter the .def files export
# and the suite's per-test check reads. KEEP IN SYNC WITH conftest.FIFO_SOURCES.
FIFO_SOURCES = [CLIB / "alxFifo.c", CLIB / "alxBound.c", TEST / "alxFifoTestHelpers.c",
                TEST / "alxBoundTestHelpers.c", TEST / "alxAssertPc.c"]
FIFO_DEF = TEST / "alxFifoTest.def"

# CLI group (Tier 2: real alxCli + param stack over the fakes), asserts ON = the code as shipped
# (alxParamItem.c has side effects inside its asserts). KEEP IN SYNC WITH conftest.CLI_SOURCES_STRICT/_CLOSURE.
CLI_ASSERTS = ["-DALX_CLI_ASSERT_RST_ENABLE", "-DALX_SERIAL_PORT_ASSERT_RST_ENABLE", "-DALX_FIFO_ASSERT_RST_ENABLE",
               "-DALX_BOUND_ASSERT_RST_ENABLE", "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE",
               "-DALX_PARAM_MGMT_ASSERT_RST_ENABLE",
               "-DALX_FTOA_ASSERT_RST_ENABLE", "-DALX_RANGE_ASSERT_RST_ENABLE", "-DALX_ID_ASSERT_RST_ENABLE"]
CLI_CLOSURE = [CLIB / "alxParamItem.c", CLIB / "alxParamMgmt.c", CLIB / "alxFtoa.c", CLIB / "alxRange.c"]
CLI_STRICT = [CLIB / "alxCli.c", CLIB / "alxFifo.c", CLIB / "alxBound.c", TEST / "alxSerialPortFake.c",
              TEST / "alxParamKvStoreFake.c", TEST / "alxIdFake.c", TEST / "alxAssertPc.c",
              TEST / "alxTracePc.c", TEST / "alxCliTestHelpers.c"]
CLI_DEF = TEST / "alxCliTest.def"
CLI_TESTS = ["test_alxCli.py"]

# MemSafe group: real alxMemSafe/alxCrc/alxParamGroup/alxParamStore over alxMemRawFake.
# KEEP IN SYNC WITH conftest.MEMSAFE_SOURCES_STRICT/_CLOSURE/_ASSERT_DEFINES.
MS_ASSERTS = ["-DALX_MEM_SAFE_ASSERT_RST_ENABLE", "-DALX_CRC_ASSERT_RST_ENABLE",
              "-DALX_PARAM_GROUP_ASSERT_RST_ENABLE", "-DALX_PARAM_STORE_ASSERT_RST_ENABLE",
              "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE", "-DALX_BOUND_ASSERT_RST_ENABLE", "-DALX_FTOA_ASSERT_RST_ENABLE",
              "-DALX_RANGE_ASSERT_RST_ENABLE"]
MS_CLOSURE = [CLIB / "alxParamGroup.c", CLIB / "alxParamStore.c", CLIB / "alxParamItem.c", CLIB / "alxFtoa.c",
              CLIB / "alxRange.c"]
MS_STRICT = [CLIB / "alxMemSafe.c", CLIB / "alxCrc.c", CLIB / "alxBound.c", TEST / "alxMemRawFake.c",
             TEST / "alxParamKvStoreFake.c", TEST / "alxAssertPc.c", TEST / "alxTracePc.c",
             TEST / "alxMemSafeTestHelpers.c"]
MS_DEF = TEST / "alxMemSafeTest.def"
# Every test file that drives THIS group's DLL, which is not the same list as "the tests I was
# thinking about when I wrote the gate". The four modules below were missing, and because the gate
# names only alxCrc.c and alxMemSafe.c, nothing complained - the report simply stated that
# alxParamItem.c was 4.14 % covered and 96 of its 105 functions never ran, while test_alxParamItem.py
# sat next to it exercising them. A coverage report that quietly omits a module's tests is worse
# than no report: it reads as a measurement of the library and is really a measurement of this list.
MS_TESTS = ["test_alxCrc.py", "test_alxMemSafe.py", "test_alxParamGroup.py", "test_alxParamStore.py",
            "test_alxParamStore_groups.py",
            "test_alxParamItem.py", "test_alxParamItem_meta.py",
            "test_alxParamItem_enum.py", "test_alxParamItem_buff.py",
            "test_alxParamItem_kv.py",
            "test_alxRange.py",
            "test_alxFtoa.py"]

# The sanitize lane runs the same group with one file held back. alxFtoa.c:110 assigns a double to
# a `long` - 32 bits here and on the target alike - and a value past 2147483647 is then a conversion
# C99 6.3.1.4 leaves undefined. That defect is already sealed as P420, which proves it prints
# punctuation instead of digits; what a strict xfail cannot do is survive UBSan, because the lane
# builds with -fno-sanitize-recover and the process ABORTS rather than failing an assertion.
# So the file stays in COVERAGE, where it measures the module at 100 %, and waits here for the fix.
# Drop this list the moment P420 comes off.
MS_TESTS_UBSAN = [t for t in MS_TESTS if t != "test_alxFtoa.py"]

# The list above is hand-written on purpose, because the thing that decides membership is not the
# file's NAME - test_alxBound.py drives the general DLL and test_alxParamItem_reset.py drives the
# per-configuration ones through the `variants` marker, and both are named after a module in this
# group. Deriving the list from the names looks tidier and is wrong; it was tried.
#
# So the list is CHECKED instead of derived: a test file that asks for one of this group's fixtures
# and is not named here is the omission that cost the report once already, and it is now a lane
# failure rather than a number nobody could see was wrong.
MS_FIXTURES = {"memsafe_lib", "flash", "make_item", "make_meta_item", "make_enum_item",
               "make_buff_item", "make_multi_store", "make_kv_item", "make_store"}


def _fixtures_requested(path: Path) -> set[str]:
    """Every fixture name the file's tests and their helpers take as an argument."""
    tree = ast.parse(path.read_text(encoding="utf-8"))
    return {a.arg for node in ast.walk(tree)
            if isinstance(node, ast.FunctionDef) for a in node.args.args}


def _guard(session: nox.Session, check) -> None:
    """Run a consistency check and turn its failure into a lane failure with the reason."""
    try:
        check()
    except RuntimeError as ex:
        session.error(str(ex))


def _check_ms_tests() -> None:
    """Fail when a test file drives this group's DLL and the coverage list does not know about it."""
    drivers = {p.name for p in sorted(TEST.glob("test_*.py"))
               if _fixtures_requested(p) & MS_FIXTURES}
    missing = sorted(drivers - set(MS_TESTS))
    stale = sorted(t for t in MS_TESTS if t not in drivers)
    if missing or stale:
        raise RuntimeError(
            f"MS_TESTS is out of step with the tests that drive the MemSafe DLL: "
            f"missing {missing}, listed but no longer driving it {stale}"
        )

# Id group: the real alxId over the IO pin fake, asserts ON = the code as shipped. Both closure
# sources are closure for reasons written out at conftest's ID group - alxId.c fails -Wformat and
# -Wint-to-void-pointer-cast, and alxIdTestDateComp.c cannot survive -Werror at all because the
# library's ALX_BUILD_DATE_COMP overflows an int. KEEP IN SYNC WITH conftest.ID_SOURCES_STRICT/
# _CLOSURE and with what conftest._assert_defines derives from them.
#
# In SANITIZE from the group's first test: the identity block is read out of flash at a fixed
# 32-bit address through a packed struct, which is exactly the shape UBSan has something to say
# about. In COVERAGE since the suite arrived (ALX-1553 P468-P505) and gated on FUNCTIONS the way
# the MemSafe group is, which is what the note here previously said it would take.
ID_ASSERTS = ["-DALX_ID_ASSERT_RST_ENABLE"]
ID_CLOSURE = [CLIB / "alxId.c", TEST / "alxIdTestDateComp.c"]
ID_STRICT = [TEST / "alxIdTestHelpers.c", TEST / "alxIoPinFake.c", TEST / "alxAssertPc.c"]
ID_DEF = TEST / "alxIdTest.def"
ID_TESTS = ["test_alxId.py"]

# Assert group (ALX-1553): the funnel every ALX_*_ASSERT in the library goes through, built TWICE
# from almost the same list. All three handlers are ALX_WEAK and a strong definition displaces a
# weak one for the whole image, so the two sides cannot both be reachable in one DLL: the WEAK DLL
# omits alxAssertPc.c and runs alxAssert.c's own bodies, the other links it and is displaced.
# alxAssertBkptCaller.c is in the second list only - the weak AlxAssert_Bkpt is __debugbreak() on
# this host and does not return, so a caller for it must not exist where that default would run.
# No assert defines for either: alxAssert.h declares no enable macro of its own, which is right for
# the mechanism rather than one of its clients. KEEP IN SYNC WITH conftest.ASSERT_WEAK_SOURCES and
# conftest.ASSERT_SOURCES.
#
# In SANITIZE from the group's first test. NOT in COVERAGE, and that is a decision rather than an
# omission: two of alxAssert.c's three functions are reachable from a DLL and the third terminates
# the process, so a functions gate could only be 2 of 3 - a number picked to fit, which is exactly
# what the Id group's note below refuses to do. Gating this module needs the out-of-process exe
# that AlxAssert_Bkpt requires anyway (the ASan smoke exe in stage 1 is the shape), and that is a
# lane change of its own.
ASSERT_WEAK_SOURCES = [CLIB / "alxAssert.c", TEST / "alxAssertTestHelpers.c"]
ASSERT_WEAK_DEF = TEST / "alxAssertWeakTest.def"
ASSERT_SOURCES = [CLIB / "alxAssert.c", TEST / "alxAssertTestHelpers.c", TEST / "alxAssertBkptCaller.c",
                  TEST / "alxAssertPc.c"]
ASSERT_DEF = TEST / "alxAssertTest.def"
ASSERT_TESTS = ["test_alxAssert.py"]

# MemRaw group (ALX-1553): the raw-memory contract, five ALX_WEAK symbols that no file in the
# library implements strongly. Two DLLs for the same reason the Assert group has two - once with
# the weak defaults intact, once with alxMemRawTestOverride.c displacing four of the five (DeInit
# is left weak on purpose, so displacement is visibly per symbol). Note that alxMemRawFake.c, which
# the MemSafe group links INSTEAD of this module, is in neither list: it would displace all five
# and leave nothing to test. KEEP IN SYNC WITH conftest.MEMRAW_SOURCES / MEMRAW_OVR_SOURCES and
# with what conftest._assert_defines derives from them.
MEMRAW_ASSERTS = ["-DALX_MEM_RAW_ASSERT_RST_ENABLE"]
MEMRAW_SOURCES = [CLIB / "alxMemRaw.c", TEST / "alxMemRawTestHelpers.c", TEST / "alxAssertPc.c"]
MEMRAW_DEF = TEST / "alxMemRawTest.def"
MEMRAW_OVR_SOURCES = [*MEMRAW_SOURCES, TEST / "alxMemRawTestOverride.c"]
MEMRAW_OVR_DEF = TEST / "alxMemRawOvrTest.def"
MEMRAW_TESTS = ["test_alxMemRaw.py"]

# Neither module is in ANALYSIS_SOURCES, and both were tried. alxMemRaw.c cannot enter it: that
# list feeds STYLE_FILES too, and the C style gate reports alxMemRaw.c:46 (spaces rather than tabs
# after @param[in,out]) - a finding in a library file this suite may not edit. Adding alxAssert.c
# alone would put one of a pair in and leave the other out for a reason that has nothing to do with
# either module. Widening the analyze lane over library sources is a decision of its own; it is
# recorded here so the next reader does not think it was overlooked.
#
# Test/alxAssert.cfg is a cppcheck library definition this lane does NOT pass, also on purpose. It
# marks AlxAssert_Rst and AlxAssert_Bkpt noreturn, which is what stops cppcheck reasoning about the
# code after an assertion as reachable with the asserted condition false - measured worth 21 of 35
# findings in a firmware this library serves, and measured NOT to be achievable by declaring the
# handlers noreturn in alxAssert.h, which changes nothing at c99 or c11. It would do nothing here:
# ANALYSIS_SOURCES is compiled with no assert defines, so the modules this lane scans have their
# assertions compiled out and the findings never arise. Its effect is proved instead by
# test_alxAssert.py P515, which is where a consumer should look before adopting it.

UBSAN = list(hb.UBSAN)                 # the instrumented variants are the library's flag sets: one
ASAN_UBSAN = list(hb.ASAN_UBSAN)       # definition for every C repository, not a copy per noxfile
PROFILE = list(hb.PROFILE)


# ---- helpers (the lane vocabulary itself is alx.verify.lanes) ----------------------------------------------
def _strs(paths) -> list:
    return [str(p) for p in paths]


def _dev_build(session: nox.Session) -> list:
    """Build every stale group with conftest's -Werror recipe; return the targets that are missing.

    Collection alone does NOT do this: the DLLs are built by the fixtures, which --collect-only
    never runs. Measured 10.09 - with alxFifoTest.dll deleted, the BUILD lane reported
    "BUILD CLEAN" listing the other two and exited 0.
    """
    for target, deps, make in cf.DLL_GROUPS:
        if hb.needs_build(target, deps):
            session.log(f"building {target.name}")
            make()
    cf.write_compile_db()
    return [t.name for t, _, _ in cf.DLL_GROUPS if not t.exists()]


def _fresh_dev_build(session: nox.Session) -> None:
    """The dev gate every lane starts with: the -Werror DLLs and the compile database are current."""
    missing = _dev_build(session)
    if missing:
        session.error(f"BUILD FAILED - the recipe did not produce {missing}")
    session.run(PYTHON, "-m", "pytest", "-q", "--collect-only", silent=True)


def _clang_cl(session: nox.Session, *args, cwd: Path | None = None) -> None:
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
    missing = _dev_build(session)
    if missing:
        session.error(f"BUILD FAILED - the recipe did not produce {missing}")
    session.run(PYTHON, "-m", "pytest", "-q", "--collect-only")
    session.log(f"BUILD CLEAN - {sorted(t.name for t, _, _ in cf.DLL_GROUPS)} in {BUILD}")


@nox.session
def variants(session: nox.Session) -> None:
    """VARIANTS - HOST: every variant group built in every named configuration, then the proofs.

    On demand, like mutate: nox.options.sessions comes from the shared lane list, so adding a
    session here does not join the default run. That is deliberate - this lane multiplies build
    time by len(VARIANTS) and answers a configuration question, not a regression one.

    WHICH PAIRS COMPILE IS MOST OF THE EVIDENCE. `debug` is the only configuration that builds
    alxLin.c's DBG and VRB call sites at all, so a build failure in one cell is the finding, not an
    interruption - every pair is attempted and the matrix is reported at the end. That is the
    opposite of the BUILD lane's fail-fast, on purpose.

    The recipes come from conftest.VARIANT_GROUPS rather than being restated here. The sanitize and
    coverage lanes each keep their own copy of a group's source list behind a KEEP IN SYNC comment;
    a seventh copy would defeat the point of having a table.
    """
    out = lanes.evidence_dir(TEST, "variants")
    rows, failed = [], []

    for group in sorted(cf.VARIANT_GROUPS):
        for variant in sorted(cf.VARIANTS):
            dll = cf._variant_dll(group, variant)
            form, module_trace, level = cf.VARIANTS[variant]
            try:
                cf._build_variant_dll(group, variant)
                built = dll.exists()
            except hb.BuildError as exc:
                built = False
                failed.append(f"{group}/{variant}")
                _write(out / f"build_{group}_{variant}.txt", str(exc))
            session.log(f"{group:6} {variant:14} {'built' if built else 'FAILED'}  {dll.name}")
            rows.append(f"{group:6} {variant:14} {'yes' if built else 'NO':4} "
                        f"assert={form or '-':6} trace={'on' if module_trace else 'off':4} {level}")

    _write(out / "variants_matrix.txt", chr(10).join(rows) + chr(10))
    if failed:
        session.error(f"VARIANTS FAILED to build {sorted(failed)} - see {out}")

    session.run(PYTHON, "-m", "pytest", "-q", "-m", "variants", *session.posargs)
    session.log(f"VARIANTS CLEAN - {len(rows)} configurations, matrix in {out}")


@nox.session
def test(session: nox.Session) -> None:
    """TEST - HOST: the suite over the dev DLLs; evidence build/pytest_report.xml + .html."""
    session.run(PYTHON, "-m", "pytest", *session.posargs)




def _macro_arguments(line: str, open_paren: int) -> str:
    """The text inside the macro's OWN parentheses, balanced.

    Matching to the end of the line instead counts a call that is nowhere near the macro - the
    shape `if (Call() != Alx_Ok) { ALX_X_ASSERT(false); }` is everywhere in this library.
    """
    depth, begin = 0, open_paren + 1
    for i in range(open_paren, len(line)):
        if line[i] == "(":
            depth += 1
            if depth == 1:
                begin = i + 1
        elif line[i] == ")":
            depth -= 1
            if depth == 0:
                return line[begin:i]
    return line[begin:]


def _calls_inside(line: str, macro: "re.Pattern") -> list:
    """Every function a macro's arguments call, with literals removed first.

    Removing them is not tidiness: every trace message in this library names a function, as in
    "FAIL: AlxNet_IsConnected()", and counting those reported 241 trace findings where there are
    none at all.
    """
    found = macro.search(line)
    if not found:
        return []
    inner = _macro_arguments(line, found.end() - 1)
    inner = CHAR_LITERAL.sub("''", STRING_LITERAL.sub('""', inner))
    return [c for c in ELIDED_NAME.findall(inner) if c not in ASSERT_PURE_CALLS]

@nox.session
def analyze(session: nox.Session) -> None:
    """ANALYZE: 0 codespell + ASCII + README + C-style + Python gates, 1 clang-tidy, 2 cppcheck, 3 gcc -fanalyzer."""
    out = lanes.evidence_dir(TEST, "analyze")
    if not (BUILD / "compile_commands.json").exists():
        _fresh_dev_build(session)
    session.log("Stage 0: codespell, ASCII gate, README gate, C style gate, ruff (the shared test profile)")
    session.run(PYTHON, "-m", "codespell_lib", *_strs(ANALYSIS_SOURCES))
    session.run(PYTHON, "-m", "alx.verify.ascii_gate", str(CLIB), *VENDOR, "--out", str(out / "ascii_gate.txt"))
    session.run(PYTHON, "-m", "alx.verify.readme_gate", str(CLIB), *VENDOR, "--out", str(out / "readme_gate.txt"))
    session.run(PYTHON, "-m", "alx.verify.c_style", *_strs(STYLE_FILES), "--out", str(out / "c_style.txt"))
    # this folder's Python is gated by the SAME profile the library gates its own tests with, so the two
    # repositories cannot drift into two dialects - see alx/verify/ruff_tests.toml for what a test may waive
    session.run(PYTHON, "-m", "ruff", "check", "--config", str(lanes.ruff_tests_config()), str(TEST),
                "--output-file", str(out / "ruff.txt"))
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
        result = subprocess.run(  # noqa: S603 - argv is the toolchain path and this repo's own sources
            [str(tc.armgcc()), "-c", "-std=gnu99", "-mcpu=cortex-m0plus", "-mthumb", "-fanalyzer",
             *[f"-I{d}" for d in INCLUDE_DIRS], str(src), "-o", str(out / f"{src.stem}.o")],
            capture_output=True, text=True, check=False)   # the return code is read below
        lines.append(result.stderr)
        if result.returncode != 0:
            _write(log, "".join(lines))
            session.error(f"Stage 3 FAILED: {src.name} did not compile - see {log}")
    _write(log, "".join(lines))
    if "-Wanalyzer" in "".join(lines):
        session.error(f"Stage 3 FAILED: -fanalyzer findings - see {log}")

    session.log("Stage 4: target layout checks (arm-gcc, one compile per part variant)")
    # Compile-time size checks of register overlays, for the compiler that MATTERS. A driver that
    # takes its I2C transfer length from sizeof of an overlay is one ABI away from reading the wrong
    # number of bytes, and the host cannot answer the question: measured on a PC, eleven of the
    # seventeen INA238 overlays are the wrong size and on the target none are. These files are never
    # linked - each check is an array whose length is negative when a size is wrong.
    layout_log = out / "layout_checks.txt"
    reports = []
    for source, variants in LAYOUT_CHECKS:
        for variant in variants:
            result = subprocess.run(  # noqa: S603 - argv is the toolchain path and this repo's own sources
                [str(tc.armgcc()), "-c", "-std=gnu99", "-mcpu=cortex-m7", "-mthumb",
                 f"-D{variant}", *[f"-I{d}" for d in INCLUDE_DIRS], str(source),
                 "-o", str(out / f"{source.stem}_{variant}.o")],
                capture_output=True, text=True, check=False)   # the return code is read below
            reports.append(f"{source.name} -D{variant}: rc={result.returncode}\n{result.stderr}")
            if result.returncode != 0:
                _write(layout_log, "".join(reports))
                session.error(f"Stage 4 FAILED: {source.name} with {variant} - see {layout_log}")
    _write(layout_log, "".join(reports))

    session.log("Stage 5: does every portable library module still COMPILE? (arm-gcc -fsyntax-only)")
    # Measured 12.09: of the library's 56 portable .c files, the host suite compiles 24 and the
    # analysers above read 2. The rest - alxFs, alxLogger, alxNet, alxSocket, alxSd, alxFtp and the
    # alxOs* wrappers among them - were compiled by NOTHING in this repository. They are built only
    # when a product happens to use them, so an API change that breaks one is found by whoever
    # updates next, not here. That is how an uncompiled bring-up source in a product repository came
    # to be broken for an unknown length of time (ALX-1553 A9), and the answer there was the same:
    # a syntax check costs seconds.
    #
    # -DALX_C_LIB and nothing else. The module guard has to be ON or every file preprocesses to
    # nothing and the check passes without reading a line; an MCU family define cannot be added
    # because its header is the product's. So this proves the portable half parses and type-checks,
    # which is exactly the half that has no other cover.
    syntax_log = out / "syntax.txt"
    reports, broken = [], []
    for src in sorted(CLIB.glob("*.c")):
        if src.name in SYNTAX_NEEDS_CMSIS:
            continue
        result = subprocess.run(  # noqa: S603 - argv is the toolchain path and this repo's own sources
            [str(tc.armgcc()), "-fsyntax-only", "-std=gnu99", "-mcpu=cortex-m7", "-mthumb",
             "-DALX_C_LIB", *[f"-I{d}" for d in INCLUDE_DIRS], str(src)],
            capture_output=True, text=True, check=False)   # the return code is read below
        reports.append(f"{src.name}: rc={result.returncode}" + chr(10) + result.stderr)
        if result.returncode != 0:
            broken.append(src.name)
    _write(syntax_log, "".join(reports))
    if broken:
        session.error(f"Stage 5 FAILED: {broken} do not compile - see {syntax_log}")
    session.log(f"Stage 5: {len(reports)} portable modules compile")

    session.log("Stage 5b: does every one of them RETURN? (arm-gcc -c -Werror=return-type)")
    checked, missing, empty = 0, [], []
    for src in sorted(CLIB.glob("*.c")):
        if src.name in SYNTAX_NEEDS_CMSIS or src.name in RETURN_TYPE_PENDING:
            continue
        result = subprocess.run(  # noqa: S603 - argv is the toolchain path and this repo's own sources
            [str(tc.armgcc()), "-c", "-std=gnu99", "-mcpu=cortex-m7", "-mthumb",
             "-Werror=return-type", "-DALX_C_LIB", *[f"-I{d}" for d in INCLUDE_DIRS],
             str(src), "-o", str(out / "return_type.o")],
            capture_output=True, text=True, check=False)   # the return code is read below
        checked += 1
        reports.append(f"[return-type] {src.name}: rc={result.returncode}" + chr(10) + result.stderr)
        if result.returncode != 0:
            missing.append(src.name)
            continue
        symbols = subprocess.run(  # noqa: S603 - argv is the toolchain path and an object we just built
            [str(tc.armgcc().with_name("arm-none-eabi-nm.exe")), "--defined-only",
             str(out / "return_type.o")], capture_output=True, text=True, check=False)
        if not symbols.stdout.strip():
            empty.append(src.name)
    _write(syntax_log, "".join(reports))
    if missing:
        session.error(f"Stage 5b FAILED: {missing} let control reach the end of a non-void "
                      f"function - see {syntax_log}")
    if set(empty) != SYNTAX_EMPTY_TU:
        gone_quiet = sorted(set(empty) - SYNTAX_EMPTY_TU)
        woken_up = sorted(SYNTAX_EMPTY_TU - set(empty))
        session.error(f"Stage 5b FAILED: the empty-translation-unit set is wrong - newly empty "
                      f"(checked but never read) {gone_quiet}, no longer empty (drop from "
                      f"SYNTAX_EMPTY_TU) {woken_up}")
    session.log(f"Stage 5b: {checked - len(empty)} modules return on every path, "
                f"{len(empty)} empty, {len(RETURN_TYPE_PENDING)} known-pending")

    session.log("Stage 5c: does any assertion or trace DO the work instead of reporting it?")
    hiding, pending_seen, traces = {}, set(), {}
    for src in sorted(CLIB.rglob("*.c")):
        if any(p in src.parts for p in ("Test", "build", "Ext")):
            continue
        for number, line in enumerate(src.read_text(encoding="ascii", errors="replace").splitlines(), 1):
            if line.lstrip().startswith("//"):
                continue                      # a commented-out site elides nothing
            calls = _calls_inside(line, ELIDED_ASSERT)
            if calls:
                if src.name in ASSERT_SIDE_EFFECT_PENDING:
                    pending_seen.add(src.name)
                else:
                    hiding.setdefault(src.name, []).append(f"{number}: {calls[0]}")
            calls = _calls_inside(line, ELIDED_TRACE)
            if calls:
                traces.setdefault(src.name, []).append(f"{number}: {calls[0]}")
    if hiding:
        session.error(f"Stage 5c FAILED: an assertion performs the work in {sorted(hiding)} - a "
                      f"build with that module's assertions off loses the behaviour, not the "
                      f"check. Sites: {hiding}")
    if traces:
        session.error(f"Stage 5c FAILED: a TRACE argument performs the work in {sorted(traces)}. "
                      f"Measured 12.09 there were none - either gate is closed and the argument "
                      f"is deleted. Sites: {traces}")
    session.log(f"Stage 5c: no trace does work, {len(pending_seen)} assert files known-pending")


    session.log(f"ANALYZE CLEAN - evidence in {out}")


@nox.session
def sanitize(session: nox.Session) -> None:
    """SANITIZE: 1 native ASan+UBSan smoke exe (diagnostics), 2 UBSan DLL per test group under its suite."""
    _guard(session, _check_ms_tests)
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
    _dll(session, ubsan / "alxCliTest.dll", CLI_STRICT, CLI_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS", *CLI_ASSERTS], objs)
    _pytest(session, {"ALX_CLI_TEST_DLL": str(ubsan / "alxCliTest.dll")}, *CLI_TESTS)
    session.log("Stage 2c: UBSan MemSafe DLL, MemSafe group suite")
    objs = _closure_objects(session, ubsan / "memsafeClosure", MS_CLOSURE, MS_ASSERTS, UBSAN)
    _dll(session, ubsan / "alxMemSafeTest.dll", MS_STRICT, MS_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS", *MS_ASSERTS], objs)
    _pytest(session, {"ALX_MEMSAFE_TEST_DLL": str(ubsan / "alxMemSafeTest.dll")}, *MS_TESTS_UBSAN)
    session.log("Stage 2d: UBSan Id DLL, Id suite")
    objs = _closure_objects(session, ubsan / "idClosure", ID_CLOSURE, ID_ASSERTS, UBSAN)
    _dll(session, ubsan / "alxIdTest.dll", ID_STRICT, ID_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS", *ID_ASSERTS], objs)
    _pytest(session, {"ALX_ID_TEST_DLL": str(ubsan / "alxIdTest.dll")}, *ID_TESTS)
    session.log("Stage 2e: UBSan Assert DLLs (weak defaults and displaced), Assert suite")
    # Both DLLs in one pytest run: the suite's whole point is the comparison between them, so a run
    # that had only one of them instrumented would leave half of every comparison uninstrumented.
    _dll(session, ubsan / "alxAssertWeakTest.dll", ASSERT_WEAK_SOURCES, ASSERT_WEAK_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS"])
    _dll(session, ubsan / "alxAssertTest.dll", ASSERT_SOURCES, ASSERT_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS"])
    _pytest(session, {"ALX_ASSERT_WEAK_TEST_DLL": str(ubsan / "alxAssertWeakTest.dll"),
                      "ALX_ASSERT_TEST_DLL": str(ubsan / "alxAssertTest.dll")}, *ASSERT_TESTS)
    session.log("Stage 2f: UBSan MemRaw DLLs (weak defaults and a product override), MemRaw suite")
    _dll(session, ubsan / "alxMemRawTest.dll", MEMRAW_SOURCES, MEMRAW_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS", *MEMRAW_ASSERTS])
    _dll(session, ubsan / "alxMemRawOvrTest.dll", MEMRAW_OVR_SOURCES, MEMRAW_OVR_DEF, UBSAN,
         ["-D_CRT_SECURE_NO_WARNINGS", *MEMRAW_ASSERTS])
    _pytest(session, {"ALX_MEMRAW_TEST_DLL": str(ubsan / "alxMemRawTest.dll"),
                      "ALX_MEMRAW_OVR_TEST_DLL": str(ubsan / "alxMemRawOvrTest.dll")}, *MEMRAW_TESTS)
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
    _guard(session, _check_ms_tests)
    _fresh_dev_build(session)
    out = lanes.evidence_dir(TEST, "coverage")
    _dll(session, out / "alxFifoTest.dll", FIFO_SOURCES, FIFO_DEF, PROFILE)
    _llvm_cov_group(session, out, out / "alxFifoTest.dll", "ALX_FIFO_TEST_DLL", [],
                    "lines,branches,regions,functions", ["alxFifo.c", "alxBound.c"])
    # MemSafe group: gate = functions 100 %; lines/branches are REPORTED - alxMemSafe.c keeps three blocks unreachable
    # with asserts ON and alxCrc.c has `break` after `return` plus assert-guarded default branches (task notes).
    ms = lanes.evidence_dir(TEST, "coverage", "memsafe")
    objs = _closure_objects(session, ms / "closure", MS_CLOSURE, MS_ASSERTS, PROFILE)
    _dll(session, ms / "alxMemSafeTest.dll", MS_STRICT, MS_DEF, PROFILE,
         ["-D_CRT_SECURE_NO_WARNINGS", *MS_ASSERTS], objs)
    # alxFtoa.c and alxParamGroup.c join the gate now that the report is honest about them; both
    # measure 100 % of functions. alxRange.c is 11 of 12 and cannot - the one function never
    # executed is AlxRange_CheckArr, the unimplemented stub Stage 5b names for falling off the end
    # of a non-void function. alxParamStore.c is 6 of 8 and alxParamItem.c 25 of 105, which is the
    # group's real remaining gap rather than an artefact of the list above.
    _llvm_cov_group(session, ms, ms / "alxMemSafeTest.dll", "ALX_MEMSAFE_TEST_DLL", MS_TESTS,
                    "functions", ["alxCrc.c", "alxMemSafe.c", "alxFtoa.c", "alxParamGroup.c"])
    # Id group: gate = functions 100 %; lines and branches are REPORTED and not gated, on purpose.
    # alxId.c carries whole paragraphs behind #ifdef ALX_STM32 / ALX_CMSIS_CORE that a PC build
    # cannot reach at all, so a line gate here would be a number picked to fit rather than a claim
    # about the tests. (The compiler paragraph used to be one of them, behind ALX_GCC; it is now
    # detected from __clang__ / __GNUC__, so the PC build reaches it - as Clang, correctly.)
    # Every function the module defines IS reached (measured 46 of 46,
    # 94.6 % of lines), which is the claim worth gating: a getter added and never called, or one
    # dropped from alxIdTest.def and so from every test, fails this lane.
    idg = lanes.evidence_dir(TEST, "coverage", "id")
    id_objs = _closure_objects(session, idg / "closure", ID_CLOSURE, ID_ASSERTS, PROFILE)
    _dll(session, idg / "alxIdTest.dll", ID_STRICT, ID_DEF, PROFILE,
         ["-D_CRT_SECURE_NO_WARNINGS", *ID_ASSERTS], id_objs)
    _llvm_cov_group(session, idg, idg / "alxIdTest.dll", "ALX_ID_TEST_DLL", ID_TESTS,
                    "functions", ["alxId.c"])
    # MemRaw group: gate = functions 100 % over alxMemRaw.c, measured from the WEAK DLL and ONLY
    # from there. In the override DLL four of the five bodies are displaced by a strong definition
    # and would report zero executions however many tests ran, so instrumenting that one instead is
    # the way a well-meant addition breaks this lane. All five weak bodies run in the weak DLL,
    # which is what makes 100 % an honest number rather than a chosen one. The Assert group is
    # absent on purpose - the reason is written at its declaration above.
    mr = lanes.evidence_dir(TEST, "coverage", "memraw")
    _dll(session, mr / "alxMemRawTest.dll", MEMRAW_SOURCES, MEMRAW_DEF, PROFILE,
         ["-D_CRT_SECURE_NO_WARNINGS", *MEMRAW_ASSERTS])
    _llvm_cov_group(session, mr, mr / "alxMemRawTest.dll", "ALX_MEMRAW_TEST_DLL", MEMRAW_TESTS,
                    "functions", ["alxMemRaw.c"])
    session.log("COVERAGE GATES PASS")


@nox.session
def mutate(session: nox.Session) -> None:
    """MUTATE (report-only): alx.verify.mutation with the C hooks; `-- [--sample N] [alxFoo.c ...]`."""
    ap = argparse.ArgumentParser(prog="nox -s mutate --")
    ap.add_argument("--sample", type=int, default=150, help="mutants per source, 0 = all (slow)")
    ap.add_argument("sources", nargs="*", default=["alxFifo.c"], help="library sources (default alxFifo.c)")
    args = ap.parse_args(session.posargs)
    _fresh_dev_build(session)
    # The hooks are the Python lib's; only the flags and the group declaration are this repository's.
    # Templates take POSIX paths and run with --root as the working directory.
    hooks = f"{Path(PYTHON).as_posix()} -m alx.c_lib.mutation_hooks"
    # Includes only, no assert defines: the fingerprint is a RELATIVE comparison (does the mutant
    # produce the original's object code?), so both sides only have to agree. Measured 10.09: adding
    # the assert defines drops EQUIVALENT from 156 to 42 on alxFifo.c and triples the run, because
    # more code compiles distinctly. Changing that is a decision of its own, not part of a move.
    flags = " ".join(f"-I{d.as_posix()}" for d in INCLUDE_DIRS)
    work = (BUILD / "mutate" / "_tce").as_posix()
    session.run(PYTHON, "-m", "alx.verify.mutation", "--root", str(CLIB), "--tests-dir", "Test",
                "--out", str(BUILD / "mutate"), "--sample", str(args.sample), "--seed", "1514",
                "--check-cmd", f"{hooks} check {{mutant}} {flags}",
                "--fingerprint-cmd", f"{hooks} fingerprint {{mutant}} --work {work} {flags}",
                "--rebuild-cmd", f"{hooks} rebuild --groups conftest:DLL_GROUPS --sys-path Test",
                *args.sources)
