"""ALX-1553 - the same module, built four ways (Tier 1: configuration, not code).

A library module's behaviour is not one thing. alxFifo has one source file, twelve assertion sites
and three mutually exclusive assert forms plus none, and which of those a product gets is decided
entirely by macros in its own alxConfig.h. Until now this suite built exactly ONE of the resulting
binaries, did not record which, and it was not a combination any product ships: assert-RST like the
shipped configuration, but ALX_TRACE_LEVEL_OFF, which compiles every one of the library's 549 trace
call sites away and DISCARDS their arguments. (549 is the portable modules, 16 files; counting the
MCU ports under Mcu/ as well it is 1006. Measured by counting ALX_<MODULE>_TRACE_<LEVEL> call
sites in every library .c outside Test, Ext, FatFs, mcuboot and Usbh.)

conftest.VARIANTS names the four that are worth building, and this file is what makes them a claim
rather than a build option. Each variant reaches something none of the others do:

    default       what customers get - alxConfig_TEMPLATE.h's own combination. Must stay green.
    off           the elision configuration: no assert macro, no traces, so both collapse to
                  do{} while(false). This is the shape of the biggest defect ALX-1553 found, and
                  P526 is where it is pinned so it cannot come back unnoticed.
    debug         the only variant that COMPILES the DBG and VRB trace call sites at all.
    assert_trace  the form where a failed assertion reports and CONTINUES, rather than resetting.

Two honest limits on what this pilot proves.

alxFifo.c and alxBound.c contain ZERO trace call sites, so nothing HERE can observe the trace axis
at run time - only that all four configurations compile and link. The assert axis is fully
observable and is what P526 and P527 test. **The trace axis is proved in test_alxLin_variants.py**
(P532-P539), on alxLin.c's 23 sites at three levels - the module that straddles the one threshold
these four variants actually cross.

And on this host all three assert handlers return, because Test/alxAssertPc.c records instead of
aborting. So P527 proves the #elif chain in alxFifo.h selected the form the variant asked for -
which is the thing configuration can get wrong - and not what a target does afterwards. What a
target does is alxAssert's own group (P506-P515).

Cross-reference: every variant records the location as alxFifo.h, never alxFifo.c, because
ALX_FIFO_FILE is the header while __LINE__ comes from the source. That is P513, sealed in
test_alxAssert.py across 52 modules; P528 records that it is configuration-independent too.

P531 is the one that shows why this matters beyond the library: a contract this suite
pinned in 2024 turns out to be reachable only on a build with assertions compiled out.

Proofs (ALX-1553): P525-P531.
"""

import pytest

from conftest import (
    DEFAULT_VARIANT,
    FIFO_DLL,
    FIFO_SOURCES,
    VARIANTS,
    _fifo_variant_dll,
    _variant_defines,
)

pytestmark = pytest.mark.variants

# Where AlxFifo_Write says its own precondition is broken. The file is the HEADER - see P513.
WRITE_LEN_ASSERT = "alxFifo.h:191 in AlxFifo_Write"


# ---------------------------------------------------------------------------- the contract ----
def test_ALX1553_P525_the_fifo_contract_is_the_same_in_every_variant(variant_lib):
    """Configuration decides what a broken invariant REPORTS. It must not decide what works.

    The sequence is the one test_alxFifo.py already pins on the default build (P8's partial write
    and partial read), run again on each configuration. Every call here keeps its precondition, so
    the assert form has nothing to say and the four binaries have to agree exactly.
    """
    _name, lib = variant_lib
    f = lib.new(4)
    try:
        assert lib.entries(f) == 0
        assert lib.write(f, b"abc") == lib.OK
        assert lib.entries(f) == 3
        assert lib.write(f, b"de") == lib.ERR_FULL   # 'd' commits, 'e' is dropped
        assert lib.entries(f) == 4
        assert lib.read(f, 2) == (lib.OK, b"ab")
        assert lib.entries(f) == 2
        assert lib.read(f, 4)[0] == lib.ERR_EMPTY    # short read consumes what exists
        assert lib.entries(f) == 0
    finally:
        lib.delete(f)


# ------------------------------------------------------------------------ the assert axis ----
@pytest.mark.expect_assert("a zero-length write breaks 0 < len on purpose - that is the probe")
def test_ALX1553_P526_a_broken_precondition_is_recorded_in_three_variants_and_absent_in_off(variant_lib):
    """The elision defect, pinned.

    AlxFifo_Write asserts 0 < len && len <= me->buffLen. A zero-length write breaks the first half,
    and what happens next is decided by a macro rather than by the code: three variants record it,
    and `off` has nothing left to record because the expression was discarded at compile time.

    The RETURN VALUE is identical in all four - Alx_Err, because the write loop never runs. That is
    exactly why the elision is dangerous: from the caller's side the configuration is invisible.
    """
    name, lib = variant_lib
    form, _trace, _level = VARIANTS[name]
    f = lib.new(4)
    try:
        lib.c.AlxAssertPc_Reset()
        assert lib.write(f, b"") == lib.ERR, "a zero-length write stopped returning Alx_Err"
        count = lib.c.AlxAssertPc_Count()
    finally:
        lib.delete(f)

    if form is None:
        assert count == 0, (
            f"variant {name!r} enables no assert form, so ALX_FIFO_ASSERT is do{{}} while(false) "
            f"and the expression is discarded - yet {count} assertion(s) were recorded"
        )
    else:
        assert count == 1, f"variant {name!r} recorded {count} assertion(s), expected exactly 1"


@pytest.mark.expect_assert("the same zero-length write - P527 reads WHICH handler took it")
def test_ALX1553_P527_the_handler_that_runs_is_the_one_the_variant_asked_for(variant_lib):
    """alxFifo.h picks the assert form with an #elif chain, so ORDER decides when two are defined.

    That is the failure mode a configuration system exists to catch: a product enabling BKPT and RST
    together gets BKPT silently, and nothing tells it. Here exactly one form is defined per variant
    and the recorded handler name is the evidence the chain resolved to it.
    """
    name, lib = variant_lib
    form, _trace, _level = VARIANTS[name]
    f = lib.new(4)
    try:
        lib.c.AlxAssertPc_Reset()
        lib.write(f, b"")
        first = (lib.c.AlxAssertPc_First() or b"").decode("ascii", "replace")
    finally:
        lib.delete(f)

    if form is None:
        assert first == "", f"variant {name!r} has no handler, yet something recorded {first!r}"
    else:
        assert first == f"{form} {WRITE_LEN_ASSERT}"


@pytest.mark.expect_assert("the same zero-length write - P528 reads WHERE it says it happened")
def test_ALX1553_P528_the_reported_location_names_the_header_in_every_variant(variant_lib):
    """CHARACTERIZATION - P513, seen from the configuration side.

    ALX_FIFO_FILE is "alxFifo.h" and __LINE__ comes from alxFifo.c, so every assertion in the
    library reports a file and a line that never occur together. This records that the defect is in
    the macro rather than in any one build: all three enabled variants name the same wrong file.
    Sealed in test_alxAssert.py; nothing here fixes it.
    """
    name, lib = variant_lib
    form, _trace, _level = VARIANTS[name]
    if form is None:
        pytest.skip("`off` records no location at all - that is P526")
    f = lib.new(4)
    try:
        lib.c.AlxAssertPc_Reset()
        lib.write(f, b"")
        first = (lib.c.AlxAssertPc_First() or b"").decode("ascii", "replace")
    finally:
        lib.delete(f)
    assert first.endswith(WRITE_LEN_ASSERT)
    assert ".h:" in first, f"the location stopped naming a header: {first!r}"
    assert ".c:" not in first, f"the location started naming the source file: {first!r}"


@pytest.mark.expect_assert("6 bytes into a 4 byte FIFO - that IS the precondition violation")
def test_ALX1553_P531_a_write_longer_than_the_buffer_is_a_violation_not_a_feature(variant_lib):
    """What the configuration decides here is not the behaviour - it is whether the board survives.

    test_alxFifo.py's P8 pins a write of 6 bytes into a 4 byte FIFO as a partial write: four bytes
    commit and the call returns ErrFull. That is true in all four variants, and it is also a broken
    precondition, because AlxFifo_Write asserts len <= me->buffLen before any of it happens.

    So the pinned contract is only REACHABLE on a board built with assertions off. In `default` -
    the configuration a product ships - the same call hits ALX_ASSERT_RST, and on a target that
    resets the MCU; nothing downstream of the assertion ever runs and no caller sees ErrFull. The
    host cannot show the reset, because alxAssertPc.c records and returns, so the record IS the
    evidence: whether anything was reported is the whole difference between a supported partial
    write and a reboot.

    This is why a suite that builds one configuration and does not name it is not enough. The
    behaviour was pinned in 2024 by a group built with no assert macro at all, and it stayed green
    on a build where the precondition had been compiled out of existence.
    """
    name, lib = variant_lib
    form, _trace, _level = VARIANTS[name]
    f = lib.new(4)
    try:
        lib.c.AlxAssertPc_Reset()
        status = lib.c.AlxFifo_Write(f, b"abcdef", 6)
        count = lib.c.AlxAssertPc_Count()
        entries = lib.entries(f)
        _read_status, data = lib.read(f, 4)
    finally:
        lib.delete(f)

    # identical in all four - which is exactly what makes the elision invisible to a caller
    assert status == lib.ERR_FULL
    assert entries == 4
    assert data == b"abcd"

    if form is None:
        assert count == 0, "`off` reported a precondition it no longer compiles"
    else:
        assert count == 1, (
            f"variant {name!r} let a write of 6 into a 4 byte FIFO through without a word - "
            f"on a target with this assert form the board would not have got this far"
        )


# ----------------------------------------------------------------------- the harness itself ----
def test_ALX1553_P529_the_default_variant_keeps_the_unsuffixed_dll_name():
    """The sanitizer and coverage lanes select a FIFO binary through ALX_FIFO_TEST_DLL, by path.

    `default` therefore has to stay alxFifoTest.dll or those two lanes would build one image and
    run another, and nothing would say so. The other three get a suffix and must all be distinct.
    """
    assert _fifo_variant_dll(DEFAULT_VARIANT) == FIFO_DLL
    others = {name: _fifo_variant_dll(name) for name in VARIANTS if name != DEFAULT_VARIANT}
    assert FIFO_DLL not in others.values(), "a second variant would overwrite the default build"
    assert len(set(others.values())) == len(others), "two variants share one DLL path"


def test_ALX1553_P530_every_variant_is_a_complete_and_unambiguous_configuration():
    """What a build variant has to be before it is worth building.

    Each of these is a way a hand-edited alxConfig.h goes wrong today, with no diagnostic:
    a module left with no assert form at all (the elision), a module given two so the #elif chain
    picks one silently, and a missing ALX_TRACE_LEVEL so the level comes from whatever default the
    header happens to carry.
    """
    modules = {"ALX_FIFO", "ALX_BOUND"}   # the two library modules in this group that assert
    for name, (form, module_trace, level) in VARIANTS.items():
        defines = {d.removeprefix("-D") for d in _variant_defines(name, FIFO_SOURCES)}

        levels = [d for d in defines if d.startswith("ALX_TRACE_LEVEL=")]
        assert levels == [f"ALX_TRACE_LEVEL={level}"], f"{name}: {levels}"

        for module in modules:
            forms = sorted(d for d in defines if d.startswith(f"{module}_ASSERT_"))
            expected = [] if form is None else [f"{module}_ASSERT_{form}_ENABLE"]
            assert forms == expected, f"{name}/{module}: {forms}"

            traces = sorted(d for d in defines if d == f"{module}_TRACE_ENABLE")
            assert traces == ([f"{module}_TRACE_ENABLE"] if module_trace else []), f"{name}/{module}"

        unexpected = {d for d in defines
                      if not d.startswith("ALX_TRACE_LEVEL=")
                      and not any(d.startswith(f"{m}_") for m in modules)}
        assert not unexpected, f"{name}: a macro for a module this group does not compile: {unexpected}"
