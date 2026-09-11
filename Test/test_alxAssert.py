"""ALX-1553 - alxAssert PC unit tests (Tier 1: the mechanism every assertion in the library uses).

Three macros and three ALX_WEAK handlers, and they are the funnel 2293 ALX_*_ASSERT call sites in
90 modules reach. Nothing had ever compiled them: Test/alxAssertPc.c supplies strong definitions
for all three and is linked into every one of the other groups, so the library's own bodies ran
nowhere in this suite and alxAssert.c appeared in no source list at all.

The group is two DLLs because the handlers are weak. A strong definition displaces a weak one for
the whole image, so one DLL can show the library's own body running or an override displacing it,
never both. ``assert_weak_lib`` is the image without alxAssertPc.c and ``assert_lib`` is the image
with it, and everything worth saying here is a comparison between the two.

The single most important thing this file records is what a failed assertion actually does. The
library's own AlxAssert_Rst is an EMPTY body that returns; nothing in alxAssert.c resets anything.
A shipped board reboots on a broken invariant only because the product wrote a strong override, and
a product that enables ALX_*_ASSERT_RST_ENABLE and forgets to write one gets assertions that are in
effect off - with none of the signals that assertions being off usually gives you.

One handler is missing from both DLLs, and that is what it does rather than an omission:
AlxAssert_Bkpt's weak default runs ALX_BKPT(), which is __debugbreak() on this host and does not
return. It can only be observed out of process, from a built .exe whose exit code is the evidence.

Two tests here drive the toolchain rather than a DLL, because the claims are about compilation:
P514 asks the compiler what the macros do to an unbraced if/else, and P515 asks cppcheck what the
missing noreturn costs and which remedy removes it. Both skip when the tool is absent; the ANALYZE
lane needs both anyway, so a machine that runs the pipeline has them.

Worth knowing while reading: alxAssert.h declares no ALX_<MODULE>_ASSERT_*_ENABLE macro of its own,
so conftest._assert_defines contributes nothing to either DLL. That is correct - this module is the
mechanism, not one of its clients - and it is not the gap it looks like. Related and equally
harmless, alxGlobal.h:279 spells the pattern WITHOUT the _ENABLE suffix (ALX_GLOBAL_ASSERT_BKPT),
which puts it outside that regex; ALX_GLOBAL_ASSERT has zero call sites in the library, so the
naming gap can never bite.

Sealed defects: P513, P514.

Proofs (ALX-1553): P506-P515.
"""

import re
import subprocess  # the toolchain is the SUBJECT of P514 and P515, not an input to them
from pathlib import Path

import pytest
from alx.c_lib import host_build as hb

pytestmark = pytest.mark.unit

TEST_DIR = Path(__file__).parent
CLIB_DIR = TEST_DIR.parent
INCLUDE_DIRS = [TEST_DIR, CLIB_DIR, CLIB_DIR / "Mcu", CLIB_DIR / "Ext"]
CPPCHECK_CFG = TEST_DIR / "alxAssert.cfg"

# The library's own module headers. Test/ is not library source and the three vendor trees are not
# Auralix's; everything else under these three folders is a module that may define an ALX_*_FILE.
LIB_HEADERS = [*sorted(CLIB_DIR.glob("*.h")),
               *sorted((CLIB_DIR / "Mcu").glob("*.h")),
               *sorted((CLIB_DIR / "Ext").glob("*.h"))]
_FILE_CONST = re.compile(r'^\s*#define\s+(ALX_[A-Z0-9_]*_FILE)\s+"([^"]*)"', re.MULTILINE)

# A module's assert block, written the way all 90 of them are written, around a caller that puts an
# assertion inside an unbraced if/else. Compiled twice by P514, once per configuration.
_DANGLE_TU = """\
#include "alxAssert.h"

#define DANGLE_FILE "dangle.c"

#if defined(DANGLE_ASSERT_RST_ENABLE)
	#define DANGLE_ASSERT(expr) ALX_ASSERT_RST(DANGLE_FILE, expr)
#else
	#define DANGLE_ASSERT(expr) do{} while (false)
#endif

int Dangle(int c, int* y);

int Dangle(int c, int* y)
{
	if (c) DANGLE_ASSERT(c > 0); else *y = 1;
	return c;
}
"""
_BRACED_TU = _DANGLE_TU.replace(
    "\tif (c) DANGLE_ASSERT(c > 0); else *y = 1;",
    "\tif (c) { DANGLE_ASSERT(c > 0); } else { *y = 1; }")

# The shape every analyser finding of this class has: assert the pointer, then use it. cppcheck
# reports nullPointerRedundantCheck here because AlxAssert_Rst is declared as returning.
_NORETURN_TU = """\
#include "alxAssert.h"

#define NR_FILE "noreturn.c"
#define NR_ASSERT(expr) ALX_ASSERT_RST(NR_FILE, expr)

int NrUse(int* p);

int NrUse(int* p)
{
	NR_ASSERT(p != NULL);
	return *p;
}
"""
# The remedy the brief assumed: the declaration made noreturn. Redeclaring after the header is the
# same thing to an analyser as editing alxAssert.h would be, and needs no edit to a read-only file.
_NORETURN_DECLARED_TU = _NORETURN_TU.replace(
    '#include "alxAssert.h"\n',
    '#include "alxAssert.h"\n\n_Noreturn void AlxAssert_Rst(const char* file, uint32_t line, const char* fun);\n')


def _tool(which: str) -> Path:
    """Locate one host tool, or skip: these two tests are about the toolchain's opinion."""
    try:
        toolchain = hb.Toolchain()
        found = toolchain.compiler(hb.GNU) if which == "clang" else toolchain.cppcheck()
    except FileNotFoundError as missing:
        pytest.skip(f"{which} not installed: {missing}")
    if not Path(found).exists():
        pytest.skip(f"{which} not installed: {found}")
    return Path(found)


def _clang_syntax_only(tmp_path: Path, source: str, name: str, *defines: str):
    """Compile one translation unit, no linking, and hand the test back what clang said."""
    unit = tmp_path / name
    unit.write_text(source, encoding="ascii")
    argv = [str(_tool("clang")), "-std=gnu99", "-fsyntax-only",
            *[f"-I{d}" for d in INCLUDE_DIRS], *defines, str(unit)]
    return subprocess.run(argv, capture_output=True, text=True, check=False)  # noqa: S603 - fixed argv


def _cppcheck(tmp_path: Path, source: str, name: str, *extra: str) -> str:
    """Run cppcheck over one translation unit and return everything it reported."""
    unit = tmp_path / name
    unit.write_text(source, encoding="ascii")
    argv = [str(_tool("cppcheck")), "--std=c99", "--enable=warning,style", "--quiet",
            "--suppress=missingIncludeSystem", *extra,
            *[f"-I{d}" for d in INCLUDE_DIRS], str(unit)]
    done = subprocess.run(argv, capture_output=True, text=True, check=False)  # noqa: S603 - fixed argv
    return done.stdout + done.stderr


# =====================================================================
# P506 - both sides of a weak symbol, in one suite
# =====================================================================


@pytest.mark.expect_assert(
    "P506: the override DLL records a deliberately failed assertion - that it records one at all is "
    "what proves alxAssertPc.c displaced the library's weak default"
)
def test_ALX1553_P506_the_library_bodies_run_in_one_dll_and_are_displaced_in_the_other(
        assert_weak_lib, assert_lib):
    """The same failing assertion in two images, reaching two different definitions.

    In the weak image AlxAssert_Rst is the library's own body: empty, returning, tracing nothing -
    which is why a shipped board resets on a failed assertion only when a product supplies the
    override. In the same image AlxAssert_Trace IS the library's body and does write, at FTL, even
    though this DLL is compiled with ALX_TRACE_LEVEL_OFF: the compile-time gate is on the
    ALX_TRACE_* macros and the handler does not go through one.

    In the override image both land in alxAssertPc.c instead, which records and returns, and the
    trace recorder stays at zero because the library's weak Trace body never ran. That is the whole
    weak-symbol mechanic, and it is newer than it looks - alxGlobal.h:313-317 emits
    __attribute__((weak)) for clang on Windows only since this branch, and before that lld-link
    rejected the image with a duplicate symbol.
    """
    assert assert_weak_lib.level_configured() == assert_weak_lib.OFF, "this DLL is built with tracing OFF"

    assert_weak_lib.drive("Rst", expr=False)
    assert assert_weak_lib.traces() == 0, "the weak AlxAssert_Rst body is empty - it traces nothing"

    assert_weak_lib.drive("Trace", expr=False)
    assert assert_weak_lib.traces() == 1
    assert assert_weak_lib.trace_level() == assert_weak_lib.FTL

    assert_lib.drive("Rst", expr=False)
    assert assert_lib.asserts() == 1, "alxAssertPc.c did not displace the library's weak AlxAssert_Rst"
    assert assert_lib.first().startswith("RST ")
    assert assert_lib.traces() == 0, "the library's weak AlxAssert_Trace body ran in the override DLL"


# =====================================================================
# P507 - what a failed assertion does to the code after it
# =====================================================================


@pytest.mark.expect_assert(
    "P507: every assertion here is meant to fail - what is being measured is what happens NEXT"
)
def test_ALX1553_P507_a_failed_assertion_returns_and_the_next_statement_runs(assert_weak_lib, assert_lib):
    """None of the three handlers stops anything, in either image, and that is the point.

    This is the asymmetry worth pinning about the whole module. AlxAssert_Rst at alxAssert.c:76-81
    casts its three parameters to void and returns; there is no reset, no loop, no call to anything.
    A product enabling ALX_<MODULE>_ASSERT_RST_ENABLE is enabling a call to a function that does
    nothing, and the reboot everyone associates with a failed Auralix assertion comes entirely from
    a strong override the product writes. Forget the override and the invariant is still checked,
    the branch is still taken, and execution carries straight on through the code the invariant was
    guarding - which is worse than having assertions off, because nothing about the build says so.

    AlxAssert_Trace is the same story with a line of output, and in the override image so is BKPT,
    since alxAssertPc.c records rather than breaking. The weak BKPT default is the one exception in
    the library and it cannot be reached from here: __debugbreak() takes the process with it, which
    is why no caller for it is exported from the weak DLL at all.

    Recorded, not sealed. An empty weak default is a defensible design - it is the hook, and the
    product owns the policy - but the header says nothing about it, and a caller reading
    `void AlxAssert_Rst(...)` beside a macro named RST has no way to learn that the library's own
    version of RST does not reset.
    """
    assert_weak_lib.drive("Rst", expr=False)
    assert assert_weak_lib.reached_after("Rst"), "the library's own AlxAssert_Rst did not return"
    assert assert_weak_lib.call_direct("Rst"), "calling the handler directly did not return either"

    assert_weak_lib.drive("Trace", expr=False)
    assert assert_weak_lib.reached_after("Trace"), "the library's own AlxAssert_Trace did not return"

    for kind in ("Rst", "Trace", "Bkpt"):
        assert_lib.drive(kind, expr=False)
        assert assert_lib.reached_after(kind), f"alxAssertPc's {kind} handler did not return"
    assert assert_lib.asserts() == 3


# =====================================================================
# P508 - where an assertion says it happened
# =====================================================================


def test_ALX1553_P508_the_handler_is_told_the_call_sites_line_and_function(assert_weak_lib):
    """__LINE__ and __func__ expand at the CALL SITE, which is the only reason a report names anything.

    The macros at alxAssert.h:55-57 pass __LINE__ and __func__ as arguments rather than letting the
    handler look anything up, so what arrives is a property of the asserting module's translation
    unit. Without that every one of the 2293 assertions in the library would report the same three
    lines of alxAssert.c.

    The expected line is read out of the DLL rather than written here: each driver in
    alxAssertTestHelpers.c records __LINE__ + 1 immediately above its macro, so editing that file
    cannot leave a stale number in this test. The file is the caller's own constant, and here it
    names the .c the macro is written in - which is what a correct pair looks like, and which P513
    shows no library module manages.
    """
    assert_weak_lib.drive("Trace", expr=False)

    assert assert_weak_lib.trace_line() == assert_weak_lib.line("Trace")
    assert assert_weak_lib.trace_fun() == "AlxAssertTest_DriveTrace"
    assert assert_weak_lib.trace_file() == assert_weak_lib.file() == "alxAssertTestHelpers.c"
    assert assert_weak_lib.trace_text() == (
        f"{assert_weak_lib.file()}:{assert_weak_lib.line('Trace')} in AlxAssertTest_DriveTrace")


# =====================================================================
# P509 - what an assertion that holds costs
# =====================================================================


def test_ALX1553_P509_an_expression_that_holds_reaches_no_handler(assert_weak_lib, assert_lib):
    """`if (expr) {} else { handler }` - the true branch is empty, so a holding invariant costs a test.

    Written the other way round, as `if (!(expr))`, the macro would be identical in behaviour and
    would draw -Wempty-body on nothing; written this way it draws it on the `{}`, which is why the
    empty-then-else inversion is there. Either way the handler is reached only on the false branch,
    so the cost of an assertion that holds is one predictable branch and no call - which is what
    makes it defensible to leave them enabled in a shipped build.
    """
    assert_weak_lib.drive("Rst", expr=True)
    assert_weak_lib.drive("Trace", expr=True)
    assert assert_weak_lib.traces() == 0, "a holding assertion reached the trace handler"
    assert assert_weak_lib.reached_after("Rst")
    assert assert_weak_lib.reached_after("Trace")

    for kind in ("Rst", "Trace", "Bkpt"):
        assert_lib.drive(kind, expr=True)
    assert assert_lib.asserts() == 0, "a holding assertion reached a handler"


# =====================================================================
# P510 - an assertion that is off, against one that is absent
# =====================================================================


@pytest.mark.expect_assert(
    "P510: the enabled form is driven with a false expression so that the side effect inside it is "
    "reached at all - the assertion firing is the vehicle, the counter is the measurement"
)
def test_ALX1553_P510_the_expression_is_evaluated_once_when_compiled_in_and_never_when_not(assert_lib):
    """The difference between an assertion that is off and an assertion that is not there.

    With no ALX_<MODULE>_ASSERT_*_ENABLE macro the module's own header defines its assert as
    `do{} while (false)` and the argument is DISCARDED by the preprocessor: not evaluated, not
    compiled, not present in the object file. So an expression with a side effect silently stops
    happening, and an invariant that is written down stops existing - there is nothing left for a
    mutation to change, which is how the device repository's mutation lane found 257 assertions
    compiled out of its host build on 11.09 and why seventeen groups here were rebuilt with
    assertions on.

    Both expansions stand in one translation unit here, driven by the same probe, so the comparison
    needs no second build. The enabled form evaluates its expression exactly once - once, not twice,
    which matters because the macro mentions `expr` only in the if. The disabled form evaluates it
    zero times, and the direct call underneath is what proves that zero is a real measurement rather
    than a counter nobody wired up.
    """
    assert_lib.drive_side_effect()
    assert assert_lib.side_effects() == 1, "the enabled form evaluated its expression more than once"

    assert_lib.drive_side_effect_off()
    assert assert_lib.side_effects_off() == 0, "the disabled form evaluated its expression"

    assert_lib.bump_off_direct()
    assert assert_lib.side_effects_off() == 1, "the counter the disabled form did not move is dead"


# =====================================================================
# P511 - the trace level does not gate assertions
# =====================================================================


def test_ALX1553_P511_tracing_switched_off_does_not_silence_assertions(assert_weak_lib):
    """ALX_TRACE_LEVEL is a compile-time gate on the ALX_TRACE_* macros, and the handler is not one.

    Mcu/alxTrace.h:93-124 wraps every ALX_TRACE_<LEVEL> in `#if ALX_TRACE_LEVEL >= ...`, so a
    product that sets ALX_TRACE_LEVEL_OFF compiles all of them away and reasonably expects silence.
    The weak AlxAssert_Trace at alxAssert.c:65-68 does not go through ALX_TRACE_FTL; it calls
    AlxTrace_WriteLevel directly with a RUNTIME level argument, so the gate never sees it. Measured
    here against the real build: this DLL is compiled at ALX_TRACE_LEVEL_OFF and the call still
    happens, once, at FTL.

    Recorded rather than sealed, because writing a fatal line on a failed assertion is a reasonable
    thing for the library to want. The cost is worth stating anyway: on a shipped board every failed
    assertion pays an RTC read and two vsnprintf calls inside AlxTrace_WriteLevel, on a device whose
    configuration says tracing is off - and the module that configured the silence has no way to
    tell. It is also why the note in the Id group's declaration ('ALX_TRACE_LEVEL_OFF compiles the
    trace paragraph away') stops at the trace paragraph and does not extend to assertions.
    """
    assert assert_weak_lib.level_configured() == assert_weak_lib.OFF
    assert assert_weak_lib.OFF < assert_weak_lib.FTL, "FTL is meant to be the level OFF is below"

    assert_weak_lib.drive("Trace", expr=False)

    assert assert_weak_lib.traces() == 1, "ALX_TRACE_LEVEL_OFF silenced the assertion's own trace"
    assert assert_weak_lib.trace_level() == assert_weak_lib.FTL


# =====================================================================
# P512 - the three macro forms, and which handler each reaches
# =====================================================================


@pytest.mark.expect_assert(
    "P512: one deliberately failed assertion per macro form - the recorded kind is the measurement"
)
@pytest.mark.parametrize("kind", ["Rst", "Trace", "Bkpt"])
def test_ALX1553_P512_each_macro_form_reaches_its_own_handler(assert_lib, kind):
    """Three forms, three handlers, and until this file the suite had only ever compiled one of them.

    conftest._assert_defines reads ALX_<M>_ASSERT_BKPT_ENABLE out of each module's header purely to
    learn the module's macro prefix and then always emits the RST form, and the three hand-written
    lists beside it are RST-only too. So ALX_ASSERT_BKPT and ALX_ASSERT_TRACE - the form a bench
    build uses and the form a field-diagnostic build uses, both configurations real products ship -
    had never been through a compiler in this repository.

    It has to be the override DLL: the weak BKPT default is __debugbreak() and would take the runner
    down. alxAssertPc.c prefixes the recorded line with the kind, so one string per form says which
    branch of the module's #if / #elif / #elif the header selected and which handler the expansion
    called, and the line and function in it come from the caller as P508 describes.
    """
    expected_file = assert_lib.file_bkpt() if kind == "Bkpt" else assert_lib.file()

    assert_lib.drive(kind, expr=False)

    assert assert_lib.asserts() == 1
    assert assert_lib.first() == (
        f"{kind.upper()} {expected_file}:{assert_lib.line(kind)} in AlxAssertTest_Drive{kind}")


# =====================================================================
# P513 - where an assertion says it happened, across the library
# =====================================================================


@pytest.mark.xfail(strict=True, raises=AssertionError, reason=(
    "DEFECT: no assertion in the library reports a location that exists. The `file` string a "
    "handler receives is the module's ALX_<MODULE>_FILE constant and in all 91 definitions across "
    "the library that constant names the HEADER - alxFifo.h:57 is `#define ALX_FIFO_FILE "
    "\"alxFifo.h\"`, and not one module names its .c. The line beside it is __LINE__, which expands "
    "in the .c. So ALX_FIFO_ASSERT at alxFifo.c:93 reports \"alxFifo.h:93\", and line 93 of "
    "alxFifo.h is the struct member `uint32_t numOfEntries;`. AlxTrace_WriteLevel formats the pair "
    "as \"[%s:%04lu %s]\" (Mcu/alxTrace.c:141), which reads as file:line and is what a field "
    "engineer follows - so the report does not merely fail to locate the assertion, it points "
    "confidently at a real line of a real file that has nothing to do with it. Both components are "
    "individually correct and the pair is never a location, in 91 of 91 modules, on every assert "
    "and every trace. Two readings are possible and the choice is the library owner's: either the "
    "constant is a PATH, in which case it should name the .c (Test/alxAssertTestHelpers.c and "
    "Test/alxAssertBkptCaller.c both do, and P508 shows what a correct pair looks like), or it is a "
    "module TAG, in which case the handlers should stop presenting it beside a line number"))
def test_ALX1553_P513_an_assertion_reports_a_file_the_line_number_belongs_to():
    """Every ALX_*_FILE in the library, read off the headers rather than sampled.

    A source scan rather than a DLL test, because the claim is about a convention and not about one
    module: any single module's constant could be argued either way, and 91 of 91 cannot. Test/ is
    excluded - it is not library source - and so are the three vendor trees.

    A seal that has quietly stopped looking at anything is worse than no seal, so two things guard
    this one. A scan that found nothing would leave the list of offenders empty, the test would pass
    and xfail_strict would fail the run. A scan that found a handful would xfail as usual and hide
    it, which is why the health check below raises a RuntimeError rather than asserting: the marker
    expects AssertionError and nothing else, so a shrunken scan is an error and not a quiet pass.
    """
    found = []
    for header in LIB_HEADERS:
        text = header.read_text(encoding="utf-8", errors="replace")
        found.extend((header.name, name, value) for name, value in _FILE_CONST.findall(text))

    if len(found) <= 50:
        msg = f"the scan found only {len(found)} ALX_*_FILE definitions in {len(LIB_HEADERS)} headers - it is broken"
        raise RuntimeError(msg)

    naming_a_header = [f"{h}: {n} = {v}" for h, n, v in found if not v.endswith(".c")]
    assert naming_a_header == [], (
        f"{len(naming_a_header)} of {len(found)} ALX_*_FILE constants name something other than the "
        f".c their __LINE__ comes from, e.g. {naming_a_header[0]}")


# =====================================================================
# P514 - whether an assertion is usable where a statement is
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the ENABLED macro forms are not statement-safe, and the DISABLED form is. "
    "ALX_ASSERT_RST(file, expr) expands to `if (expr) {} else { AlxAssert_Rst(...); }` with no "
    "do/while wrapper, so at a call site written `if (c) MODULE_ASSERT(x); else y;` the macro's own "
    "trailing `}` plus the call site's `;` leaves the outer else unattached: clang reports "
    "-Wdangling-else and then `error: expected expression`. The same call site compiles clean with "
    "assertions off, because the #else branch IS `do{} while (false)`. Measured on this toolchain, "
    "both ways. The consequence is a build failure that appears only in the configuration a product "
    "ships: a caller writes an unbraced if/else around an assertion, it builds green on the "
    "library's default, and it stops compiling the day that product defines an enable macro - or "
    "the day this suite adds the module to a group, since every group here now builds with "
    "assertions on. The library's own .c files contain no live instance, so this is latent rather "
    "than broken today; it is still the macro's defect and not the caller's. The fix is the wrapper "
    "every C style guide gives for exactly this: `do { if (expr) {} else { ... } } while (false)`, "
    "one line in each of alxAssert.h:55-57, and it makes the three forms behave like the fourth"))
def test_ALX1553_P514_an_assertion_compiles_wherever_a_statement_compiles(tmp_path):
    """Four compiles of two translation units, and the asymmetry is the finding.

    The braced call site is checked first so that a failure here cannot be read as the fixture being
    wrong, and the unbraced one is checked with assertions OFF before it is checked with them on,
    which is what makes the claim 'green today, broken when it matters' rather than 'broken'.
    """
    braced_on = _clang_syntax_only(tmp_path, _BRACED_TU, "braced.c", "-DDANGLE_ASSERT_RST_ENABLE")
    assert braced_on.returncode == 0, f"the braced call site did not compile with assertions on:\n{braced_on.stderr}"

    unbraced_off = _clang_syntax_only(tmp_path, _DANGLE_TU, "dangle.c")
    assert unbraced_off.returncode == 0, (
        f"the unbraced call site did not compile with assertions off:\n{unbraced_off.stderr}")

    unbraced_on = _clang_syntax_only(tmp_path, _DANGLE_TU, "dangle.c", "-DDANGLE_ASSERT_RST_ENABLE")
    assert unbraced_on.returncode == 0, (
        f"the unbraced call site stopped compiling once assertions were enabled:\n{unbraced_on.stderr}")


# =====================================================================
# P515 - the analyser fall-through, and which remedy clears it
# =====================================================================


def test_ALX1553_P515_the_missing_noreturn_and_the_remedy_that_actually_removes_it(tmp_path):
    """What the handlers not being noreturn costs an analyser, and the two candidate fixes measured.

    alxAssert.h:63-65 declares all three handlers as plain `void`, so cppcheck follows the else
    branch of an assertion back out and reasons about the code after it with the asserted condition
    FALSE. On the commonest shape in the library - assert a pointer, then dereference it - that
    produces nullPointerRedundantCheck, reproduced here on a fixture of exactly that shape. In a
    firmware this library serves it is 21 of 35 cppcheck findings, every one of them an artefact of
    the declaration rather than a statement about the code.

    The remedy everyone reaches for does not work, and this test is here to keep that measured
    rather than assumed: adding _Noreturn to the declaration leaves the finding exactly where it
    was. Adding __attribute__((noreturn)) behaves the same, and so does --std=c11. Since that would
    have been a change to a read-only library header in a release deliberately changing as little
    source as possible, the counterfactual is worth more than the symptom.

    Test/alxAssert.cfg does clear it, completely, and it is a Test/-side file that needs no library
    edit at all. The third check is what makes that meaningful: the cfg could equally have silenced
    the file by failing to parse it, so the run under the cfg is checked to still be reporting - the
    unrelated constParameterPointer finding on the same fixture still comes through.

    This repository's own ANALYZE lane has never seen any of this: ANALYSIS_SOURCES passes no assert
    defines to cppcheck, so every module it scans is scanned with its assertions compiled out.
    """
    bare = _cppcheck(tmp_path, _NORETURN_TU, "noreturn.c")
    assert "nullPointerRedundantCheck" in bare, f"the finding did not reproduce:\n{bare}"

    declared = _cppcheck(tmp_path, _NORETURN_DECLARED_TU, "noreturn_declared.c")
    assert "nullPointerRedundantCheck" in declared, (
        "_Noreturn on the declaration cleared the finding - the seal in this test's docstring, and "
        f"the advice not to edit alxAssert.h, are both stale:\n{declared}")

    with_cfg = _cppcheck(tmp_path, _NORETURN_TU, "noreturn.c", f"--library={CPPCHECK_CFG}")
    assert "nullPointerRedundantCheck" not in with_cfg, (
        f"Test/alxAssert.cfg no longer clears the finding:\n{with_cfg}")
    assert "constParameterPointer" in with_cfg, (
        f"the cfg silenced the whole file rather than the one finding:\n{with_cfg}")
