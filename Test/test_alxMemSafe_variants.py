"""ALX-1553 - the build variants on a THIRD group, and the one that cannot show the threshold.

alxFifo carries the assert axis and has no trace sites at all; alxLin carries the trace axis and
straddles the one threshold the four variants cross. This group is here for two things neither of
those can give:

  1  it is the first group in the matrix built in TWO STEPS - strict sources plus a warnings-off
     closure - so it exercises the half of _build_variant_dll that fifo and lin leave untouched;
  2  it is 46 trace sites across FOUR modules and THREE levels, and measuring it says plainly what
     the present four variants can and cannot prove.

Measured 12.09, by level and by module:

    alxMemSafe.c      21    WRN
    alxParamStore.c   11              INF
    alxParamGroup.c    7     2 WRN     5 INF
    alxParamItem.c     7              2 INF   5 ERR
    ---------------------------------------------
    46 sites           ERR  5   WRN 23   INF 18

**Every one of them is at or below INF(4).** The four variants set ALX_TRACE_LEVEL to OFF(0),
INF(4) and VRB(6), so `default`, `assert_trace` and `debug` compile an IDENTICAL set here - all 46 -
and only `off` differs. This group therefore proves gate A, the module's own ALX_<M>_TRACE_ENABLE,
and says nothing whatever about gate B.

That is not a gap in the tests, it is a measurement of the variant set, and P546 states it so the
next person does not have to rediscover it: separating WRN from INF needs a FIFTH variant at
ALX_TRACE_LEVEL_WRN(3), which would keep this group's 23 WRN and elide its 18 INF. Four variants is
TV's decision (11.09) and a fifth is not something a test file should assume.

One thing had to go first. The group used to carry a hand-written MEMSAFE_ASSERT_DEFINES that
hard-coded assert-RST for nine modules; it would have made `off` build as RST and lose the elision
configuration that is the whole point of that variant. Eight of its nine entries were exactly what
_assert_defines() derives, and the ninth named a module this group does not compile.

Proofs (ALX-1553): P544-P547.
"""

import pytest

from conftest import MEMSAFE_SOURCES_CLOSURE, MEMSAFE_SOURCES_STRICT, VARIANTS, _variant_defines

pytestmark = pytest.mark.variants

# Mcu/alxTrace.h:82-88
LEVEL_OFF, LEVEL_ERR, LEVEL_WRN, LEVEL_INF, LEVEL_VRB = 0, 2, 3, 4, 6
LEVEL_OF_NAME = {"ALX_TRACE_LEVEL_OFF": LEVEL_OFF, "ALX_TRACE_LEVEL_INF": LEVEL_INF,
                 "ALX_TRACE_LEVEL_VRB": LEVEL_VRB}

# the modules in this group that carry traces, and their macro prefixes
TRACE_MODULES = ("ALX_MEM_SAFE", "ALX_PARAM_STORE", "ALX_PARAM_GROUP", "ALX_PARAM_ITEM")

VALUE = bytes([0, 0, 50, 100, 70])


def _drive_a_failure(lib) -> None:
    """Make the safe store fail a write, which is what its WRN sites report.

    AlxMemRaw_Init failing for ever is the bluntest fault the fake offers, and it takes the module
    down every retry path it has - which is where the traces are.
    """
    lib.fake_reset()
    ctx = lib.new(tries=3, raw_tries=3)
    lib.fail_at(lib.INIT, lib.ALWAYS)
    lib.write(ctx, VALUE)
    lib.delete(ctx)


def test_ALX1553_P544_the_binary_reports_the_level_its_variant_asked_for(memsafe_variant_lib):
    """A two-step build has two compiler invocations, and both must get the same configuration.

    fifo and lin are built in one step, so this is the first check that the closure half - compiled
    with warnings off, separately - is handed the same -D flags as the strict half. Reading
    ALX_TRACE_LEVEL back out of the linked image is what proves they agreed.
    """
    name, lib = memsafe_variant_lib
    _form, _module_trace, level_name = VARIANTS[name]
    assert lib.trace_level_configured() == LEVEL_OF_NAME[level_name]


def test_ALX1553_P545_the_modules_own_switch_silences_all_forty_six_sites(memsafe_variant_lib):
    """Gate A across four modules at once: `off` compiles none of them, the rest compile all.

    Driven rather than asserted from the defines - a failing write walks the retry paths where this
    group's WRN sites live, and the recorder counts what actually happened.
    """
    name, lib = memsafe_variant_lib
    _form, module_trace, _level = VARIANTS[name]

    lib.trace_reset()
    _drive_a_failure(lib)
    recorded = lib.traces()

    if module_trace:
        assert recorded > 0, (
            f"variant {name!r} enables this group's traces, yet a failing write reported nothing"
        )
    else:
        assert recorded == 0, (
            f"variant {name!r} defines no ALX_<M>_TRACE_ENABLE, so every site is "
            f"do{{}} while(false) - yet {recorded} were recorded"
        )


def test_ALX1553_P546_this_group_cannot_separate_WRN_from_INF_and_that_is_the_measurement(
        memsafe_variant_lib):
    """CHARACTERIZATION of the VARIANT SET, not of the library.

    Every one of this group's 46 sites is at or below INF(4), so the three variants that enable
    traces - default and assert_trace at INF, debug at VRB - compile exactly the same set. Nothing
    here can show a threshold, and a test that quietly passed would let a reader believe otherwise.

    Separating the 23 WRN from the 18 INF wants a fifth variant at ALX_TRACE_LEVEL_WRN(3). Four is
    TV's decision; this records what the fourth leaves unproved rather than assuming a fifth.
    """
    name, lib = memsafe_variant_lib
    _form, module_trace, _level = VARIANTS[name]
    if not module_trace:
        pytest.skip("`off` compiles no sites at all - that is P545")

    lib.trace_reset()
    _drive_a_failure(lib)

    # no site in this group sits above INF, so nothing can land at DBG or VRB whatever the level is
    assert lib.traces_at(LEVEL_VRB) == 0
    assert lib.traces_at(LEVEL_INF + 1) == 0
    # and what does land is only ever the levels the four modules declare
    assert lib.traces() == sum(lib.traces_at(lvl) for lvl in (LEVEL_ERR, LEVEL_WRN, LEVEL_INF))


def test_ALX1553_P547_every_module_in_the_group_is_configured_by_the_derived_defines():
    """The hand-written list is gone, so the derivation has to cover what it used to.

    MEMSAFE_ASSERT_DEFINES named nine modules by hand. Eight were exactly what _variant_defines
    derives from each module's own header; the ninth, ALX_MEM_RAW_ASSERT_RST_ENABLE, named a module
    this group does not compile - it links alxMemRawFake.c, not alxMemRaw.c - so it had never done
    anything. This asserts the four trace-bearing modules are still configured, which is the part a
    silent regression would take away.
    """
    for name, (form, module_trace, level) in VARIANTS.items():
        defines = {d.removeprefix("-D")
                   for d in _variant_defines(name, MEMSAFE_SOURCES_STRICT, MEMSAFE_SOURCES_CLOSURE)}

        assert [d for d in defines if d.startswith("ALX_TRACE_LEVEL=")] == [f"ALX_TRACE_LEVEL={level}"]

        for module in TRACE_MODULES:
            trace = f"{module}_TRACE_ENABLE"
            assert (trace in defines) is bool(module_trace), f"{name}/{module}: {trace}"

            forms = sorted(d for d in defines if d.startswith(f"{module}_ASSERT_"))
            assert forms == ([] if form is None else [f"{module}_ASSERT_{form}_ENABLE"]), \
                f"{name}/{module}: {forms}"

        assert "ALX_MEM_RAW_ASSERT_RST_ENABLE" not in defines, \
            "the inert define for a module this group does not compile has come back"
