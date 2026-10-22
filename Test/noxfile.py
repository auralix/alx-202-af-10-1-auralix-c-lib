"""Auralix C Library - the verification lanes (described in the wiki: C Library Verification System Stack).

Run from Test/ inside its uv environment (`uv sync --locked` once):

    uv run nox -l                                   list the lanes = the pipeline stages
    uv run nox                                      build, test, analyze, sanitize, coverage
    uv run nox -s coverage                          one lane
    uv run nox -s mutate -- --sample 5 alxFifo.c    MUTATE (report-only); --sample 0 = every mutant
    uv run nox -s matrix                            every alx file in every configuration, then its gate
    uv run nox -s matrix -- 0001 0045               the rows whose id or name starts so (no gate)
    uv run nox -s test -- -k Fifo                   arguments after -- go to pytest

One nox session per stage, named after it, running in Test/.venv (no second environment per lane), the
same shape as the Auralix Python lib's noxfile and the device repos'. Evidence under build/<stage>/; the
dev lane (pytest) writes to build/ itself. What lives here is what is THIS repository's: the source lists,
the defines, the export lists, the mutation hooks. Everything a second C repository would need identically
comes from the Python lib: the generic gates (alx.verify.gates.ascii / c_style / c_format / c_types /
coverage_gate / mutation), the lane vocabulary (alx.verify.lanes) and the host toolchain and DLL build
mechanics (alx.verify.host_build).
"""

import argparse
import ast
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

import nox
from alx.verify import host_build as hb
from alx.verify import lanes

TEST = Path(__file__).resolve().parent
CLIB = TEST.parent
BUILD = TEST / "build"
CONFIG = TEST / "config"
HOST = TEST / "host"
HELPERS = HOST / "helpers"
FAKES = HOST / "fakes"
CHECKS = HOST / "checks"
PYTHON = sys.executable
tc = hb.Toolchain()   # where the tools are on THIS machine (ALX_* variables), and the vcvars environment
# repository build recipes and DLL_GROUPS; no pytest lifecycle dependency; harness is installed
# into this project's environment (pyproject.toml, [build-system])
from harness.host import build as host_recipe  # noqa: E402
from harness.host import matrix as host_matrix  # noqa: E402

nox.options.default_venv_backend = "none"
nox.options.sessions = list(lanes.DEFAULT_SESSIONS)
# third-party folders, not gated
VENDOR = ["--exclude", "FatFs", "--exclude", "mcuboot", "--exclude", "Usbh"]

# ---- the module sets (extend per module; keep in sync with host_build's group declarations) -------------------
ANALYSIS_SOURCES = [CLIB / "alxFifo.c", CLIB / "alxBound.c",
                    HELPERS / "alxFifoTestHelpers.c", HELPERS / "alxBoundTestHelpers.c",
                    CHECKS / "alxFifoSanSmokeCheck.c"]
# every C file this suite OWNS is style-gated, whatever module it serves; the LIBRARY sources
# gated here are still only the ones ANALYSIS_SOURCES covers (see the Jira task, item A13)
# The C style gate reads only what is listed here. Until 12.09 that list was ANALYSIS_SOURCES -
# which exists to name what clang-tidy and cppcheck analyse, 2 of ~30 library modules - plus the
# Test/ sources, so 138 of the library's 140 .c/.h were never checked at all. Nothing chose that;
# the style gate simply borrowed another lane's list and nobody widened it.
#
# It is now every library source EXCEPT the ones that cannot pass yet, named so the debt is visible
# and shrinks by deletion rather than being rediscovered. Measured 12.09: 163 findings in 26 files.
# Since goal 7 it reads Mcu/ and Ext/ at every depth: the MCU backends' 119 doc-block separators
# and 5 ternaries were fixed then.
#
# COMMENTS - was 84 findings in 16 files, all "spaces in field separator (tabs only)". Swept 12.09:
# 54 doc blocks, 85 lines, a 1:1 space-for-tab swap that changed no content. The set is empty and
# stays here as the place a new straggler would be named rather than quietly excluded.
STYLE_PENDING_COMMENTS = []

# TERNARIES - a macro body that must expand to an EXPRESSION (alxBuild.h's compile-time character
# out of __DATE__, ALX_RTC_DAYS_IN_YEAR(yr)) is rule 1's own exemption since the Python library's
# c_style exempts a #define body (goal 7); every other ternary is written as if/else.

# Decision 12's other two gates (A74), one pending list each, the same pattern.
#
# FORMAT - every format string of the library and of Test/host against TV's table: the PRI and
# SCN macros, the bare %d %c %s %p %f %%, %.*s with (int)len on the call. alxBoot.c's frozen
# file-name parser is the gate's single exemption, decided by TV. Measured 2026-10-07: six
# findings, all in the harness (%u and %02X), fixed. The set is empty and stays here as the place
# a new one would be named.
FORMAT_PENDING = []
FORMAT_EXEMPT = ["alxBoot.c:sscanf"]

# TYPES - every declaration a library file makes, read from clang's AST (Stage 7) with the compile
# commands of the analysed matrix rows: every file, Ext/ and Mcu/ included, judged in the
# configurations that compile it, and the fakes with the host build's flags. The twelve, void and
# the library's own types, nothing else; AlxFtoa's int precision and long intPart became int32_t (goal 7).
TYPES_JOBS = 8   # AST dumps at once - each holds a whole translation unit's AST in memory
# The enums, structs and typedefs of the library's own headers are decision 12's own types: three Ext
# drivers name their register types without the Alx prefix.
TYPES_PREFIXES = ["Alx", "Ads114s08", "Adxl355", "Bq24600"]
# littlefs's block-device callbacks, written under the prefix: they keep the signature littlefs gives them.
TYPES_CALLBACKS = [f"AlxFs_Lfs_{medium}_{op}" for medium in ("FlashInt", "Mmc")
                   for op in ("ReadBlock", "ProgBlock", "EraseBlock", "SyncBlock", "Lock", "Unlock")]
# The frozen firmware file-name parser (decision 12): its sscanf count and its %n target stay int.
TYPES_EXEMPT = ["alxBoot.c:charConsumed", "alxBoot.c:sscanfStatus"]
# What the generators write into the root for a build, ignored by git; their templates are judged.
TYPES_GENERATED = ["alxBoot_GENERATED.h", "alxBootMetadata_GENERATED.h", "alxBuild_GENERATED.h"]
# Decision 12 rule 1 as TV amended it on 2026-10-22 (D55): a vendor's object - a register block, a HAL
# or SDK handle, a configuration, an RTOS handle, a file-system object, a va_list - is declared as the
# vendor's type in the backend that talks to that vendor and nowhere else: Mcu/ and the glue modules
# alxOs*, alxFs, alxBoot and alxTrace (which is in Mcu/). The gate lists a backend's vendor types and
# fails a vendor type anywhere else; a never-declared type (int, size_t) fails in a backend too.
TYPES_BACKENDS = ["Mcu", "alxOs*.[ch]", "alxFs.[ch]", "alxBoot.[ch]"]

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
#                                        AND TESTED HERE (the MemSafe group, test_crc.py).
#   alxRange.c               296        AlxRange_CheckArr is an unimplemented stub: `// TV: TODO`,
#                                        ALX_RANGE_ASSERT(false), nothing else. The P517 shape and
#                                        worse - AlxMemRaw at least returned a defined, wrong 0.
#                                        Compiled by the MemSafe group; exported by no export list and
#                                        called by nothing on this machine.
#   alxOsMutex.c             118        AlxOsMutex_IsUnlocked
#   alxOsThread.c            260        AlxOsThread_Join
#   alxOsEventFlagGroup.c    213,297,374  _Clear, _Wait, _Sync
#
# The four alxOs sites only bite a build with NEITHER ALX_FREE_RTOS nor ALX_ZEPHYR, where the
# bodies are #if dispatch that selects nothing. Every one of the eight wants a default return, and
# that is a source change in modules no test drives - so they are NAMED rather than fixed in the
# dark, and the other 48 are gated from here on.
#
# Recounted 2026-10-12 (goal 7): alxRange.c and the three alxOs modules return on every path now,
# and alxCrc.c's two defaults return 0 after their assertion. The set is empty and stays as the
# place a new one would be named.
RETURN_TYPE_PENDING: set[str] = set()

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
# alxNet.c and alxSocket.c left the set on 2026-10-02: they went back to their empty skeletons,
# which have no second tier, so ALX_C_LIB alone compiles them.
#
# alxBuild.c is compiled only by a product that selects a driver, so the set is a fact about this
# repository, not a defect. It is asserted in BOTH directions: a second module going quiet is a
# regression, and alxBuild.c coming back to life means the set is stale.
#
# The fakes of three MCU modules join them for the same reason: their only function is the
# module's constructor, which exists under the module header's family guard, so without a family
# they are empty too. They are compiled for real by a consumer that selects a family. The SPI
# fake left the set on 2026-10-02 with its model (A92), which is portable.
SYNTAX_EMPTY_TU = {"alxBuild.c",
                   "alxDacFake.c", "alxIoPinIrqFake.c", "alxRtcFake.c"}

# Decision 14 (TV, 2026-10-09): an assertion checks only variables, and a trace too (goal 7). A
# disabled one keeps its expression compiled and never evaluates it, so a call inside it runs in one
# configuration and not in another. Stage 0 runs the Python library's c_assert gate over every
# library file; it replaced Stage 5c and its list of calls allowed as pure, which has no list.

_STYLE_PENDING = {*STYLE_PENDING_COMMENTS}
_STYLE_SKIP_DIRS = {"FatFs", "mcuboot", "Usbh", "Test", "build", "Doc"}


def _library_style_files(pending=_STYLE_PENDING) -> list:
    """Every library .c/.h - the root, Mcu/ and Ext/ at every depth - its pending list excluded."""
    found = []
    for path in sorted([*CLIB.glob("*.[ch]"), *(CLIB / "Mcu").rglob("*.[ch]"), *(CLIB / "Ext").rglob("*.[ch]")]):
        if set(path.relative_to(CLIB).parts[:-1]) & _STYLE_SKIP_DIRS:
            continue
        if path.relative_to(CLIB).as_posix() in pending:
            continue
        found.append(path)
    return found


STYLE_FILES = [*_library_style_files(), *sorted(HOST.rglob("*.c"))]
FORMAT_FILES = [*_library_style_files(set(FORMAT_PENDING)), *sorted(HOST.rglob("*.[ch]"))]
LIBRARY_FILES = _library_style_files(set())
# Target-only translation units: compiled by ANALYZE stage 4 with arm-gcc, never linked, one compile
# per part variant. (source, variants to compile it under).
LAYOUT_CHECKS = [(CHECKS / "alxIna228RegSizeCheck.c", ["ALX_INA238", "ALX_INA228"])]
INCLUDE_DIRS = host_recipe.INCLUDES

# alxAssertFake.c is in every group now: it holds the assertion counter every export list exports
# and the suite's per-test check reads. Declared by host_build.FIFO_SOURCES.
FIFO_SOURCES = host_recipe.FIFO_SOURCES
FIFO_EXPORTS = host_recipe.FIFO_EXPORTS

# CLI group (Tier 2: real alxCli + param stack over the fakes), asserts ON = the code as shipped
# (alxParamItem.c has side effects inside its asserts). Declared by host_build.CLI_SOURCES_STRICT/_CLOSURE.
CLI_ASSERTS = host_recipe.CLI_ASSERT_DEFINES
CLI_CLOSURE = host_recipe.CLI_SOURCES_CLOSURE
CLI_STRICT = host_recipe.CLI_SOURCES_STRICT
CLI_EXPORTS = host_recipe.CLI_EXPORTS
CLI_TESTS = ["test_cli.py"]

# MemSafe group: real alxMemSafe/alxCrc/alxParamGroup/alxParamStore over alxMemRawFake.
# Source membership and assertion policy come from the same recipe as ordinary test builds.
MS_ASSERTS = host_recipe._assert_defines(host_recipe.MEMSAFE_SOURCES_STRICT, host_recipe.MEMSAFE_SOURCES_CLOSURE)
MS_CLOSURE = host_recipe.MEMSAFE_SOURCES_CLOSURE
MS_STRICT = host_recipe.MEMSAFE_SOURCES_STRICT
MS_EXPORTS = host_recipe.MEMSAFE_EXPORTS
# Every test file that drives THIS group's DLL, which is not the same list as "the tests I was
# thinking about when I wrote the gate". The four modules below were missing, and because the gate
# names only alxCrc.c and alxMemSafe.c, nothing complained - the report simply stated that
# alxParamItem.c was 4.14 % covered and 96 of its 105 functions never ran, while test_param_item.py
# sat next to it exercising them. A coverage report that quietly omits a module's tests is worse
# than no report: it reads as a measurement of the library and is really a measurement of this list.
MS_TESTS = ["test_crc.py", "test_mem_safe.py", "test_param_group.py", "test_param_store.py",
            "test_param_store_groups.py",
            "test_param_item.py", "test_param_item_meta.py",
            "test_param_item_enum.py", "test_param_item_buff.py",
            "test_param_item_kv.py",
            "test_range.py",
            "test_ftoa.py"]

# The sanitize lane runs the same group with one file held back. alxFtoa.c:110 assigns a double to
# an int32_t - 32 bits here and on the target alike - and a value past 2147483647 is then a conversion
# C99 6.3.1.4 leaves undefined. That defect is already sealed as P420, which proves it prints
# punctuation instead of digits; what a strict xfail cannot do is survive UBSan, because the lane
# builds with -fno-sanitize-recover and the process ABORTS rather than failing an assertion.
# So the file stays in COVERAGE, where it measures the module at 100 %, and waits here for the fix.
# Drop this list the moment P420 comes off.
MS_TESTS_UBSAN = [t for t in MS_TESTS if t != "test_ftoa.py"]

# The list above is hand-written on purpose, because the thing that decides membership is not the
# file's NAME - test_bound.py drives the general DLL and test_param_item_reset.py drives the
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
    drivers = {p.name for p in sorted((TEST / "tests").rglob("test_*.py"))
               if _fixtures_requested(p) & MS_FIXTURES}
    missing = sorted(drivers - set(MS_TESTS))
    stale = sorted(t for t in MS_TESTS if t not in drivers)
    if missing or stale:
        raise RuntimeError(
            f"MS_TESTS is out of step with the tests that drive the MemSafe DLL: "
            f"missing {missing}, listed but no longer driving it {stale}"
        )

# Id group: the real alxId over the IO pin fake, asserts ON = the code as shipped, alxId.c under
# the strict set since goal 7. Declared by host_build.ID_SOURCES_STRICT/_CLOSURE and with what
# host_build._assert_defines derives from them.
#
# In SANITIZE from the group's first test: the identity block is read out of flash at a fixed
# 32-bit address through a packed struct, which is exactly the shape UBSan has something to say
# about. In COVERAGE since the suite arrived (ALX-1553 P468-P505) and gated on FUNCTIONS the way
# the MemSafe group is, which is what the note here previously said it would take.
ID_ASSERTS = host_recipe._assert_defines(host_recipe.ID_SOURCES_STRICT, host_recipe.ID_SOURCES_CLOSURE)
ID_CLOSURE = host_recipe.ID_SOURCES_CLOSURE
ID_STRICT = host_recipe.ID_SOURCES_STRICT
ID_EXPORTS = host_recipe.ID_EXPORTS
ID_TESTS = ["test_id.py"]

# Assert group (ALX-1553): the funnel every ALX_*_ASSERT in the library goes through, built TWICE
# from almost the same list. All three handlers are ALX_WEAK and a strong definition displaces a
# weak one for the whole image, so the two sides cannot both be reachable in one DLL: the WEAK DLL
# omits alxAssertFake.c and runs alxAssert.c's own bodies, the other links it and is displaced.
# alxAssertTestHelpers_Bkpt.c is in the second list only - the weak AlxAssert_Bkpt is __debugbreak() on
# this host and does not return, so a caller for it must not exist where that default would run.
# No assert defines for either: alxAssert.h declares no enable macro of its own, which is right for
# the mechanism rather than one of its clients. Declared by host_build.ASSERT_WEAK_SOURCES and
# host_build.ASSERT_SOURCES.
#
# In SANITIZE from the group's first test. NOT in COVERAGE, and that is a decision rather than an
# omission: two of alxAssert.c's three functions are reachable from a DLL and the third terminates
# the process, so a functions gate could only be 2 of 3 - a number picked to fit, which is exactly
# what the Id group's note below refuses to do. Gating this module needs the out-of-process exe
# that AlxAssert_Bkpt requires anyway (the ASan smoke exe in stage 1 is the shape), and that is a
# lane change of its own.
ASSERT_WEAK_SOURCES = host_recipe.ASSERT_WEAK_SOURCES
ASSERT_WEAK_EXPORTS = host_recipe.ASSERT_WEAK_EXPORTS
ASSERT_SOURCES = host_recipe.ASSERT_SOURCES
ASSERT_EXPORTS = host_recipe.ASSERT_EXPORTS
ASSERT_TESTS = ["test_assert.py"]

# MemRaw group (ALX-1553): the raw-memory contract, five ALX_WEAK symbols that no file in the
# library implements strongly. Two DLLs for the same reason the Assert group has two - once with
# the weak defaults intact, once with alxMemRawTestHelpers_Override.c displacing four of the five (DeInit
# is left weak on purpose, so displacement is visibly per symbol). Note that alxMemRawFake.c, which
# the MemSafe group links INSTEAD of this module, is in neither list: it would displace all five
# and leave nothing to test. Declared by host_build.MEMRAW_SOURCES / MEMRAW_OVR_SOURCES and
# with what host_build._assert_defines derives from them.
MEMRAW_ASSERTS = ["-DALX_MEM_RAW_ASSERT_RST_ENABLE"]
MEMRAW_SOURCES = host_recipe.MEMRAW_SOURCES
MEMRAW_EXPORTS = host_recipe.MEMRAW_EXPORTS
MEMRAW_OVR_SOURCES = host_recipe.MEMRAW_OVR_SOURCES
MEMRAW_OVR_EXPORTS = host_recipe.MEMRAW_OVR_EXPORTS
MEMRAW_TESTS = ["test_mem_raw.py"]

# Neither module is in ANALYSIS_SOURCES, and both were tried. alxMemRaw.c cannot enter it: that
# list feeds STYLE_FILES too, and the C style gate reports alxMemRaw.c:46 (spaces rather than tabs
# after @param[in,out]) - a finding in a library file this suite may not edit. Adding alxAssert.c
# alone would put one of a pair in and leave the other out for a reason that has nothing to do with
# either module. Widening the analyze lane over library sources is a decision of its own; it is
# recorded here so the next reader does not think it was overlooked.
#
# Test/config/alxAssert.cfg is a cppcheck library definition Stage 2 does NOT pass, also on purpose. It
# marks AlxAssert_Rst and AlxAssert_Bkpt noreturn, which is what stops cppcheck reasoning about the
# code after an assertion as reachable with the asserted condition false - measured worth 21 of 35
# findings in a firmware this library serves, and measured NOT to be achievable by declaring the
# handlers noreturn in alxAssert.h, which changes nothing at c99 or c11. It would do nothing there:
# ANALYSIS_SOURCES is compiled with no assert defines, so the modules Stage 2 scans have their
# assertions compiled out and the findings never arise. Its effect is proved by test_assert.py P515,
# which is where a consumer should look before adopting it. Stage 6 passes it, because the matrix
# rows compile their assertions, and tells clang-tidy the same through Test/config/alxAssertAnalyzer.h.

COVERAGE_ASSERT_OFF = "-DALX_ASSERT_OFF(expr)=((void)0)"   # the coverage builds' disabled assertion
COVERAGE_TRACE_OFF = "-DALX_TRACE_OFF(...)=((void)0)"      # and their disabled trace
ASSERT_ENABLE = re.compile(r"-DALX_[A-Z0-9_]+_ASSERT_(BKPT|TRACE|RST)_ENABLE")
# An enabled trace is `do { AlxTrace_WriteLevel(...); } while (false)`, and clang maps that `while (false)`
# as a branch never taken: every executed trace site cost a branch (goal 8, measured on alxLin.c - 15 of
# its 18 half-taken branches were trace sites). The three groups that enable traces (Fifo, MemSafe, Lin)
# prove them in their variants session; the coverage build takes them out as it takes the assertions.
TRACE_ENABLE = re.compile(r"-DALX_[A-Z0-9_]+_TRACE_ENABLE|-DALX_TRACE_LEVEL=.*")
# The coverage lane disables every group's assertions and says so (ALX_ASSERTS_OFF=1): the 15 tests that
# read the assertion record, or whose sealed defect is an assertion, expect what such a build gives
# (conftest's asserts_on, ASSERTS_ON). Until goal 8 their six groups kept their assertions on
# (COVERAGE_ASSERTS_ON), and every assertion then cost a branch nothing takes - the failing side of
# `ASSERT(cond)`, both sides of `ASSERT(false)`.
# The files the per-group gates held at 100 % of functions until goal 7 (MemSafe, Id, MemRaw), now in
# the queue below for their other metrics: their functions stay gated.
COVERAGE_FUNCTIONS = ["alxCrc.c", "alxMemSafe.c", "alxParamGroup.c", "alxId.c", "alxMemRaw.c"]
# The library files under 100 % on a gated metric, with what they measured (%, 2026-10-12): the queue of
# tests to write - goal 7 step 9 wrote none. A file leaves the list when it reaches 100 % on every
# metric; the gate fails until it does.
COVERAGE_PENDING: dict[str, str] = {}
# A97 done (goal 8, 2026-10-22): every measured file at 100% of lines, branches and functions; a line no host
# test can reach carries an lcov exclusion marker with its reason, A93's among them (Ext/alxAds114s08.c,
# Ext/alxAdxl355.c, Ext/alxAds8678.c: the host's layout of enumeration and mixed-type bit-fields).
UBSAN = list(hb.UBSAN)                 # the instrumented variants are the library's flag sets: one
ASAN_UBSAN = list(hb.ASAN_UBSAN)       # definition for every C repository, not a copy per noxfile
PROFILE = list(hb.PROFILE)


# ---- helpers (the lane vocabulary itself is alx.verify.lanes) ----------------------------------------------
def _strs(paths) -> list:
    return [str(p) for p in paths]


def _dev_build(session: nox.Session) -> list:
    """Build every stale group with harness.host.build's -Werror recipe; return the targets that are missing.

    Collection alone does NOT do this: the DLLs are built by the fixtures, which --collect-only
    never runs. Measured 10.09 - with alxFifoTest.dll deleted, the BUILD lane reported
    "BUILD CLEAN" listing the other two and exited 0.
    """
    for target, deps, make in host_recipe.DLL_GROUPS:
        if hb.needs_build(target, deps):
            session.log(f"building {target.name}")
            make()
    host_recipe.write_compile_db()
    return [t.name for t, _, _ in host_recipe.DLL_GROUPS if not t.exists()]


def _fresh_dev_build(session: nox.Session) -> None:
    """The dev gate every lane starts with: the -Werror DLLs and the compile database are current."""
    missing = _dev_build(session)
    if missing:
        session.error(f"BUILD FAILED - the recipe did not produce {missing}")
    session.run(PYTHON, "-m", "pytest", "-q", "--collect-only", silent=True)


def _instrumented_dll(dll: Path, strict, exports, flags, *, closure=(), defines=(), obj_dir=None) -> None:
    """A group's DLL again with instrumentation: host_build's clang-cl recipe over this group's lists.

    The recipe is the Python library's, the same one the dev build and every consumer use, so an
    instrumented DLL cannot be a different program from the one the suite tests. With a closure it
    is the two-step build, the closure compiled with warnings off into obj_dir; the defines reach
    both steps.
    """
    hb.build_dll(tc, out=dll, strict=strict, closure=closure, includes=INCLUDE_DIRS, defines=defines,
                 def_file=hb.write_def_file(host_recipe.BUILD_DIR, exports), flags=flags, obj_dir=obj_dir,
                 driver=hb.MSVC)


def _test_paths(args) -> list:
    """Group test lists name their modules by file name; resolve each to its one file under tests/."""
    out = []
    for a in args:
        if isinstance(a, str) and a.startswith("test_") and a.endswith(".py"):
            hits = sorted((TEST / "tests").rglob(a))
            if len(hits) != 1:
                raise FileNotFoundError(f"{a}: {len(hits)} matches under tests/, expected one")
            out.append(str(hits[0]))
        else:
            out.append(a)
    return out


def _pytest(session: nox.Session, env: dict, *args) -> None:
    session.run(PYTHON, "-m", "pytest", "-q", *_test_paths(args), env=env)


def _write(path: Path, text: str) -> None:
    path.write_text(text, encoding="ascii", errors="replace")


# ---- the lanes ---------------------------------------------------------------------------------------------
@nox.session
def build(session: nox.Session) -> None:
    """BUILD - HOST: every group's DLL with the harness's -Werror recipe and compile_commands.json."""
    missing = _dev_build(session)
    if missing:
        session.error(f"BUILD FAILED - the recipe did not produce {missing}")
    session.run(PYTHON, "-m", "pytest", "-q", "--collect-only")
    session.log(f"BUILD CLEAN - {sorted(t.name for t, _, _ in host_recipe.DLL_GROUPS)} in {BUILD}")


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

    The recipes come from harness.host.build.VARIANT_GROUPS rather than being restated here. The sanitize and
    coverage lanes each keep their own copy of a group's source list behind a Declared comment;
    a seventh copy would defeat the point of having a table.
    """
    out = lanes.evidence_dir(TEST, "variants")
    rows, failed = [], []

    for group in sorted(host_recipe.VARIANT_GROUPS):
        for variant in sorted(host_recipe.VARIANTS):
            dll = host_recipe._variant_dll(group, variant)
            form, module_trace, level = host_recipe.VARIANTS[variant]
            try:
                host_recipe._build_variant_dll(group, variant)
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




def _rel(files) -> list:
    """Paths relative to Test/, where nox runs: 350-odd absolute ones overflow a Windows command line."""
    return [str(Path("..") / f.relative_to(CLIB)) for f in files]

@nox.session
def analyze(session: nox.Session) -> None:
    """ANALYZE: 0 text gates, 1 clang-tidy, 2 cppcheck, 3 -fanalyzer, 4-5 compiles, 6 matrix analysers, 7 types."""
    out = lanes.evidence_dir(TEST, "analyze")
    if not (BUILD / "compile_commands.json").exists():
        _fresh_dev_build(session)
    session.log("Stage 0: codespell, ASCII, C style, format and assert gates, fake style gate, ruff (shared profile)")
    session.run(PYTHON, "-m", "codespell_lib", *_rel(STYLE_FILES))
    session.run(PYTHON, "-m", "alx.verify.gates.ascii", str(CLIB), *VENDOR, "--out", str(out / "ascii_gate.txt"))
    session.run(PYTHON, "-m", "alx.verify.gates.c_style", *_rel(STYLE_FILES), "--out", str(out / "c_style.txt"))
    session.run(PYTHON, "-m", "alx.verify.gates.c_format", *_rel(FORMAT_FILES),
                *[a for spot in FORMAT_EXEMPT for a in ("--exempt", spot)], "--out", str(out / "c_format.txt"))
    session.run(PYTHON, "-m", "alx.verify.gates.c_assert", *_rel(LIBRARY_FILES), "--out", str(out / "c_assert.txt"))
    # every fake a consumer links follows the one convention, family guards checked against the headers
    session.run(PYTHON, "-m", "alx.verify.gates.fake_style", *_strs(sorted(FAKES.glob("*.c"))),
                "--headers", *_strs([CLIB, CLIB / "Mcu", CLIB / "Mcu" / "McuStm32", CLIB / "Ext"]),
                "--out", str(out / "fake_style.txt"))
    # this folder's Python is gated by the SAME profile the library gates its own tests with, so the two
    # repositories cannot drift into two dialects - see alx/verify/ruff_tests.toml for what a test may waive
    session.run(PYTHON, "-m", "ruff", "check", "--config", str(lanes.ruff_tests_config()),
                str(TEST), "--output-file", str(out / "ruff.txt"))
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
    # The fakes under host/fakes/ join both questions: they are the library's own files, a consumer
    # links them, and the fakes group builds them only with the host compiler - this is where the
    # target compiler reads them too.
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
    for src in sorted([*CLIB.glob("*.c"), *FAKES.glob("*.c")]):
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
    for src in sorted([*CLIB.glob("*.c"), *FAKES.glob("*.c")]):
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

    session.log("Stage 6: clang-tidy and cppcheck over the analysed matrix rows, configured and checks off")
    # harness.host.matrix.ANALYSIS_ROWS between them compile every code line of the library. The
    # Python library's runner (D33) reads each source of each row through both analysers twice, the
    # second time with every assertion and trace disabled, and the gate fails on any finding and on
    # a line the rows stopped compiling. Evidence: build/analyze/matrix/, report.md first.
    analysis = out / "matrix"
    shutil.rmtree(analysis, ignore_errors=True)
    host_matrix.analyze(analysis, log=session.log)
    session.run(PYTHON, "-m", "alx.verify.gates.analyzers", str(analysis / "report.json"),
                "--out", str(out / "analyzers_gate.txt"))

    session.log("Stage 7: does every library file DECLARE only decision 12's types? (clang's AST)")
    # Every analysed row's compile commands as Stage 6 wrote them - each file judged in the
    # configurations that compile it, Ext/ and Mcu/ included - and the fakes with the host build's
    # flags. The gate judges every declaration a file under the root makes: vendor trees, the vendor
    # fakes (shim), build output and the venv excepted.
    types_db = out / "types_compile_commands.json"
    hb.write_compile_db(types_db, sorted(FAKES.glob("*.c")), [*host_recipe.DB_ARGUMENTS, "-DALX_C_LIB"],
                        directory=BUILD)
    entries = [*json.loads((analysis / "compile_commands.json").read_text(encoding="utf-8")),
               *json.loads(types_db.read_text(encoding="utf-8"))]
    types_db.write_text(json.dumps(entries, indent=1), encoding="utf-8")
    session.run(PYTHON, "-m", "alx.verify.gates.c_types", str(types_db), "--root", str(CLIB), *VENDOR,
                *[a for name in ("Doc", "build", ".venv", "shim", *TYPES_GENERATED) for a in ("--exclude", name)],
                *[a for name in TYPES_PREFIXES for a in ("--prefix", name)],
                *[a for name in TYPES_CALLBACKS for a in ("--callback", name)],
                *[a for spot in TYPES_EXEMPT for a in ("--exempt", spot)],
                *[a for name in TYPES_BACKENDS for a in ("--backend", name)],
                "--jobs", str(TYPES_JOBS), "--clang", str(tc.llvm("clang")), "--out", str(out / "c_types.txt"))


    session.log(f"ANALYZE CLEAN - evidence in {out}")


@nox.session
def sanitize(session: nox.Session) -> None:
    """SANITIZE: 1 host ASan+UBSan smoke exe (diagnostics), 2 UBSan DLL per test group under its suite."""
    _guard(session, _check_ms_tests)
    _fresh_dev_build(session)
    asan, ubsan = lanes.evidence_dir(TEST, "sanitize", "asan"), lanes.evidence_dir(TEST, "sanitize", "ubsan")
    session.log("Stage 1: host ASan+UBSan smoke exe")
    hb.build_exe(tc, out=asan / "alxFifoSanSmokeCheck.exe",
                 sources=[CLIB / "alxFifo.c", CLIB / "alxBound.c", CHECKS / "alxFifoSanSmokeCheck.c"],
                 includes=INCLUDE_DIRS, flags=[*ASAN_UBSAN, "/Z7", "/MT"], driver=hb.MSVC)
    shutil.copy(tc.asan_runtime(), asan)
    session.run(str(asan / "alxFifoSanSmokeCheck.exe"), external=True)
    session.log("Stage 2: UBSan FIFO DLL, full suite")
    _instrumented_dll(ubsan / "alxFifoTest.dll", FIFO_SOURCES, FIFO_EXPORTS, UBSAN)
    _pytest(session, {"ALX_FIFO_TEST_DLL": str(ubsan / "alxFifoTest.dll")})
    session.log("Stage 2b: UBSan CLI DLL, CLI suite")
    _instrumented_dll(ubsan / "alxCliTest.dll", CLI_STRICT, CLI_EXPORTS, UBSAN, closure=CLI_CLOSURE,
                      defines=CLI_ASSERTS, obj_dir=ubsan / "cliClosure")
    _pytest(session, {"ALX_CLI_TEST_DLL": str(ubsan / "alxCliTest.dll")}, *CLI_TESTS)
    session.log("Stage 2c: UBSan MemSafe DLL, MemSafe group suite")
    _instrumented_dll(ubsan / "alxMemSafeTest.dll", MS_STRICT, MS_EXPORTS, UBSAN, closure=MS_CLOSURE,
                      defines=MS_ASSERTS, obj_dir=ubsan / "memsafeClosure")
    _pytest(session, {"ALX_MEMSAFE_TEST_DLL": str(ubsan / "alxMemSafeTest.dll")}, *MS_TESTS_UBSAN)
    session.log("Stage 2d: UBSan Id DLL, Id suite")
    _instrumented_dll(ubsan / "alxIdTest.dll", ID_STRICT, ID_EXPORTS, UBSAN, closure=ID_CLOSURE,
                      defines=ID_ASSERTS, obj_dir=ubsan / "idClosure")
    _pytest(session, {"ALX_ID_TEST_DLL": str(ubsan / "alxIdTest.dll")}, *ID_TESTS)
    session.log("Stage 2e: UBSan Assert DLLs (weak defaults and displaced), Assert suite")
    # Both DLLs in one pytest run: the suite's whole point is the comparison between them, so a run
    # that had only one of them instrumented would leave half of every comparison uninstrumented.
    _instrumented_dll(ubsan / "alxAssertWeakTest.dll", ASSERT_WEAK_SOURCES, ASSERT_WEAK_EXPORTS, UBSAN)
    _instrumented_dll(ubsan / "alxAssertTest.dll", ASSERT_SOURCES, ASSERT_EXPORTS, UBSAN)
    _pytest(session, {"ALX_ASSERT_WEAK_TEST_DLL": str(ubsan / "alxAssertWeakTest.dll"),
                      "ALX_ASSERT_TEST_DLL": str(ubsan / "alxAssertTest.dll")}, *ASSERT_TESTS)
    session.log("Stage 2f: UBSan MemRaw DLLs (weak defaults and a product override), MemRaw suite")
    _instrumented_dll(ubsan / "alxMemRawTest.dll", MEMRAW_SOURCES, MEMRAW_EXPORTS, UBSAN,
                      defines=MEMRAW_ASSERTS)
    _instrumented_dll(ubsan / "alxMemRawOvrTest.dll", MEMRAW_OVR_SOURCES, MEMRAW_OVR_EXPORTS, UBSAN,
                      defines=MEMRAW_ASSERTS)
    _pytest(session, {"ALX_MEMRAW_TEST_DLL": str(ubsan / "alxMemRawTest.dll"),
                      "ALX_MEMRAW_OVR_TEST_DLL": str(ubsan / "alxMemRawOvrTest.dll")}, *MEMRAW_TESTS)
    session.log("SANITIZE CLEAN")


@nox.session
def coverage(session: nox.Session) -> None:
    """COVERAGE: every group's DLL instrumented, the whole suite against them, llvm-cov per library file, gate."""
    # Every group of harness.host.build as its builder states it (host_recipe.recipes()), rebuilt with
    # clang's instrumentation and its assertions disabled: a disabled assertion keeps its expression in a
    # sizeof (decision 14), a disabled trace its arguments, and clang's coverage maps the && || ?: of that
    # operand as branches that never run, so a coverage build disables both with nothing at all - the
    # traces by dropping a group's trace defines (TRACE_ENABLE), since an enabled one is a do-while whose
    # `while (false)` is a branch never taken. The tests that read the assertion record expect none
    # (ALX_ASSERTS_OFF). One run of the whole suite reaches
    # every DLL through its fixture's variable, and llvm-cov reads them together, so a file several groups
    # compile is measured over all of them. Coverage builds are measured, never analysed.
    _fresh_dev_build(session)
    out = lanes.evidence_dir(TEST, "coverage")
    for old in out.glob("*.profraw"):
        old.unlink()
    env = {"LLVM_PROFILE_FILE": str(out / "%m-%p.profraw"), "ALX_ASSERTS_OFF": "1"}
    dlls = []
    for recipe in host_recipe.recipes():
        variable = host_recipe.dll_variable(recipe.dll)
        defines = list(recipe.defines)
        defines = [d for d in defines if not ASSERT_ENABLE.fullmatch(d) and not TRACE_ENABLE.fullmatch(d)]
        dll = out / recipe.dll.name
        _instrumented_dll(dll, recipe.strict, recipe.exports, PROFILE, closure=recipe.closure,
                          defines=[*defines, COVERAGE_ASSERT_OFF, COVERAGE_TRACE_OFF],
                          obj_dir=out / "closure" / recipe.dll.stem)
        env[variable] = str(dll)
        dlls.append(dll)
    _pytest(session, env)
    profdata = out / "merged.profdata"
    session.run(str(tc.llvm("llvm-profdata")), "merge", "-sparse", *_strs(sorted(out.glob("*.profraw"))),
                "-o", str(profdata), external=True)
    cov, objects = str(tc.llvm("llvm-cov")), [str(dlls[0]), *[a for d in dlls[1:] for a in ("-object", str(d))]]
    profile = f"-instr-profile={profdata}"
    _write(out / "coverage_report.txt", session.run(cov, "report", *objects, profile, external=True, silent=True))
    _write(out / "lcov.info", session.run(cov, "export", *objects, profile, "-format=lcov", external=True, silent=True))
    session.run(PYTHON, "-m", "lcov_cobertura", str(out / "lcov.info"), "--output", str(out / "coverage_c.xml"),
                "--base-dir", str(CLIB))
    session.run(cov, "show", *objects, profile, "-format=html", f"-output-dir={out / 'html'}",
                "-show-branches=count", "-show-line-counts", external=True, silent=True)
    summary = session.run(cov, "export", *objects, profile, "-summary-only", external=True, silent=True)
    _write(out / "summary.json", summary)
    # The gate reads every library file some group compiles; the ones no group compiles are listed.
    reported = {Path(f["filename"]).resolve() for f in json.loads(summary)["data"][0]["files"]}
    measured = [f for f in LIBRARY_FILES if f.resolve() in reported]
    unmeasured = [f.relative_to(CLIB).as_posix() for f in LIBRARY_FILES if f.resolve() not in reported]
    _write(out / "unmeasured.txt", "".join(f"{f}\n" for f in unmeasured))
    session.log(f"COVERAGE: {len(measured)} library files measured, {len(unmeasured)} compiled by no group "
                f"(unmeasured.txt); html {out / 'html' / 'index.html'}")
    # The sources are gated on the tracefile since goal 8: a line no host test can reach carries one of
    # lcov's exclusion markers with its reason, which the gate honours and lists (the Python library's
    # v0.18.4). Regions are not in a tracefile; with ternaries banned (c_style rule 1) and the short-
    # circuits counted as branches, lines and branches say what regions said. The headers stay on the
    # summary: the tracefile gives them the lines of the macros expanded in the sources.
    sources = [f.relative_to(CLIB).as_posix() for f in measured if f.suffix == ".c"]
    headers = [f.relative_to(CLIB).as_posix() for f in measured if f.suffix == ".h"]
    session.run(PYTHON, "-m", "alx.verify.gates.coverage", str(out / "lcov.info"),
                "--metrics", "lines,branches,functions", "--out", str(out / "coverage_gate.txt"),
                *[a for name in COVERAGE_PENDING for a in ("--pending", name)], *sources)
    session.run(PYTHON, "-m", "alx.verify.gates.coverage", str(out / "summary.json"),
                "--metrics", "lines,branches,regions,functions", "--out", str(out / "coverage_gate_headers.txt"),
                *headers)
    # What the per-group gates this lane replaced held, a file in the queue keeps: every function runs.
    session.run(PYTHON, "-m", "alx.verify.gates.coverage", str(out / "lcov.info"), "--metrics", "functions",
                "--out", str(out / "coverage_gate_functions.txt"), *COVERAGE_FUNCTIONS)
    session.log("COVERAGE GATE PASS")


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
    hooks = f"{Path(PYTHON).as_posix()} -m alx.verify.mutation_hooks"
    # Includes only, no assert defines: the fingerprint is a RELATIVE comparison (does the mutant
    # produce the original's object code?), so both sides only have to agree. Measured 10.09: adding
    # the assert defines drops EQUIVALENT from 156 to 42 on alxFifo.c and triples the run, because
    # more code compiles distinctly. Changing that is a decision of its own, not part of a move.
    flags = " ".join(f"-I{d.as_posix()}" for d in INCLUDE_DIRS)
    work = (BUILD / "mutate" / "_tce").as_posix()
    session.run(PYTHON, "-m", "alx.verify.mutation", "--root", str(CLIB), "--tests-dir", "Test/tests/host/alx",
                "--mirror", "c",
                "--out", str(BUILD / "mutate"), "--sample", str(args.sample), "--seed", "1514",
                "--check-cmd", f"{hooks} check {{mutant}} {flags}",
                "--fingerprint-cmd", f"{hooks} fingerprint {{mutant}} --work {work} {flags}",
                "--rebuild-cmd", f"{hooks} rebuild --groups harness.host.build:DLL_GROUPS",
                *args.sources)


@nox.session
def matrix(session: nox.Session) -> None:
    """MATRIX - HOST: every alx file in every row of config/matrix.toml, then the configs gate.

    Decision 13 (wiki, Design/ConfigurationMatrix.md): a row is one configuration of the library -
    toolchain, platform, features, target, library - and the rows together must compile every line
    of alx code, each one clean, against fakes of everything that is not alx (host/shim/<vendor>/).
    On demand, like mutate and variants: every row is about four hundred compiles. `-- 0001 0045`
    runs the rows whose id or name starts so and skips the gate, which reads only the whole matrix.
    The mechanism is alx.verify.configs; harness.host.matrix holds what the words mean here.
    """
    shutil.rmtree(BUILD / "matrix", ignore_errors=True)
    out = lanes.evidence_dir(TEST, "matrix")
    report = host_matrix.run(out, only=session.posargs, log=session.log)
    session.log(f"MATRIX: {report['coverage']} - {out / 'report.md'}")
    if session.posargs:
        session.log("MATRIX: a subset of the rows, so no gate - it reads the whole matrix only")
        return
    session.run(PYTHON, "-m", "alx.verify.gates.configs", str(out / "report.json"),
                "--out", str(out / "configs_gate.txt"))
