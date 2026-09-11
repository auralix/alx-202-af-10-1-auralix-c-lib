"""ALX-1553 - the TRACE half of the build variants (Tier 1: configuration, not code).

test_alxFifo_variants.py proved the assert axis and could prove nothing at all about traces:
alxFifo.c and alxBound.c contain ZERO trace call sites, so all four variants of that group differ
only in their assert form and the ALX_TRACE_LEVEL each one sets is compiled but never reached.
This file is the other half, on alxLin.c - 23 trace call sites at three levels (16 WRN, 6 DBG,
1 VRB), which is the only module in the library that straddles the one threshold the four variants
actually cross AND already has a test group.

TWO GATES, AND THEY ARE INDEPENDENT. A trace call site survives compilation only if both open:

    gate A   ALX_LIN_TRACE_ENABLE           alxLin.h:74     the module's own switch
    gate B   ALX_TRACE_LEVEL >= <level>     alxTrace.h:93   the global threshold

Either one closed replaces the macro with `do {} while(false)` TAKING NO PARAMETERS, so the file,
the line, the function and every variadic argument are deleted by the preprocessor and never
evaluated. That is the same shape as the assertion elision this task started from (NotesClaude 3g),
and it is why the tests below count ARGUMENT EVALUATIONS and not just calls: a trace that is off
and a trace that is absent look identical from the outside, and only the argument tells them apart.

The measured matrix, which is what this file pins:

    variant        ALX_TRACE_LEVEL   probe args W/D/V   alxLin.c sites WRN/DBG/VRB
    off            OFF  (0)          0 / 0 / 0          0 / 0 / 0     gate A closed
    default        INF  (4)          1 / 0 / 0          1 / 0 / 0     gate B at INF
    assert_trace   INF  (4)          1 / 0 / 0          1 / 0 / 0     same as default
    debug          VRB  (6)          1 / 1 / 1          1 / 1 / 1     everything compiled

Note what the second and third rows say: `default` is what products ship, and in it two thirds of
alxLin.c's trace call sites do not exist. The six DBG and the one VRB are only ever compiled by
`debug`, so until this file nothing in the suite had ever built them at all.

How a trace is observable here at all: Test/alxTracePc.c SUPPLIES AlxTrace_WriteLevel and the
alxTrace instance. Unlike alxAssertPc.c it displaces nothing - both symbols live in Mcu/alxTrace.c,
which no test group compiles, so switching this group's traces on used to fail to link with exactly
those two undefined and no others. Measured, and that measurement is the whole reason the recorder
is a file rather than a linker argument.

Honest limits. Nothing here tests RENDERED output: the recorder does (void)format on purpose, so
alxTrace.c's double vsnprintf and its 256-byte truncation stay untested, as they have always been.
No variant sits at FTL, ERR, WRN or DBG, so only the INF/DBG boundary is crossed - the WRN/INF one
needs a fifth variant and a module that has INF sites, which is alxMemSafe's group, not this one.
And all 23 of alxLin.c's sites are failure reports, so this says nothing about a trace on a normal
path.

Proofs (ALX-1553): P532-P540.
"""

from pathlib import Path

import pytest

import conftest
from conftest import LIN_SOURCES, VARIANTS, _variant_defines

pytestmark = pytest.mark.variants

# Mcu/alxTrace.h:82-88. Named here rather than imported because the C header is the authority and a
# test asserting against a copy of it should say so out loud.
LEVEL_OFF, LEVEL_WRN, LEVEL_INF, LEVEL_DBG, LEVEL_VRB = 0, 3, 4, 5, 6

LEVEL_OF_NAME = {"ALX_TRACE_LEVEL_OFF": LEVEL_OFF, "ALX_TRACE_LEVEL_INF": LEVEL_INF,
                 "ALX_TRACE_LEVEL_VRB": LEVEL_VRB}

# What each variant's compiled configuration comes to, per gate. (module trace on, global level)
#   args  = how many of the probe's three arguments (WRN, DBG, VRB) are evaluated
#   sites = how many of alxLin.c's own WRN, DBG and VRB sites fire when each is driven once
EXPECTED = {
    "off":          ((0, 0, 0), (0, 0, 0)),
    "default":      ((1, 0, 0), (1, 0, 0)),
    "assert_trace": ((1, 0, 0), (1, 0, 0)),
    "debug":        ((1, 1, 1), (1, 1, 1)),
}

# alxLin.c's own sites, one per level, each reached by one ordinary call with nothing injected.
SITE_WRN_LINE = 426     # AlxLin_Master_Subscribe - a response nobody sent
SITE_DBG_LINE = 836     # AlxLin_RxBuff_Handle - a protected id whose parity is wrong
SITE_VRB_LINE = 875     # AlxLin_RxBuff_Handle - right parity, an identifier nothing claims

BAD_PARITY_PID = 0x00   # id 0 protects to 0x80, so 0x00 can never be a valid protected id
UNCLAIMED_ID = 0x10     # not MASTER_REQ (0x3C), not SLAVE_REQ (0x3D), and no slave frame configured


# Every driver gives its port straight back. The serial fake has ALX_SERIAL_PORT_FAKE_MAX_PORTS (2)
# slots and exhausting them is an exit(1) that takes the whole runner down rather than failing one
# test - so a driver that leaks is not a slow leak, it is a silent one.
def _drive_wrn(lib) -> None:
    """alxLin.c:426 - a master subscribes and the slave's response never arrives."""
    handle = lib.new()
    try:
        lib.c.AlxLinTest_Subscribe(handle, UNCLAIMED_ID, 1)
    finally:
        lib.free(handle)


def _drive_rx(lib, protected_id: int) -> None:
    """alxLin.c:836 or :875 - a SLAVE (never Master_Init'd, so isMaster stays false) is fed the
    two bytes that reach the protected-id check: anything, then the protected id itself."""
    handle = lib.new(init=False)
    try:
        lib.c.AlxLinTest_RxFlush(handle)
        lib.c.AlxLinTest_RxByte(handle, 0x55)
        lib.c.AlxLinTest_RxByte(handle, protected_id)
    finally:
        lib.free(handle)


# ------------------------------------------------------------------------- the contract ----
def test_ALX1553_P532_the_lin_contract_is_the_same_in_every_variant(lin_variant):
    """Configuration decides what a module REPORTS. It must not decide what it puts on the wire.

    A published frame is the strongest form of this claim available here, because the protected
    identifier and the checksum exist nowhere except in those bytes.
    """
    _name, lib = lin_variant
    handle = lib.new()
    assert lib.publish(handle, UNCLAIMED_ID, b"\x01\x02") == 0
    assert lib.wire(handle) == b"\x55\x50\x01\x02\xfc"   # SYNC, protected id, data, classic checksum


def test_ALX1553_P533_the_binary_reports_the_level_its_variant_asked_for(lin_variant):
    """A variant is only worth anything if the image can say which one it is.

    ALX_TRACE_LEVEL is a compile-time constant, so the DLL reading it back is the only way a test
    can tell that it is holding the binary it asked the harness to build rather than a stale one.
    """
    name, lib = lin_variant
    _form, _module_trace, level_name = VARIANTS[name]
    assert lib.c.AlxTracePc_LevelConfigured() == LEVEL_OF_NAME[level_name]


# ------------------------------------------------------------------------ the two gates ----
def test_ALX1553_P534_the_modules_own_switch_discards_the_argument(lin_variant):
    """Gate A, alxLin.h:74 - and the reason it has to be measured by its ARGUMENT.

    In `off` no ALX_LIN_TRACE_ENABLE is defined, so every ALX_LIN_TRACE_* becomes an empty
    do{} while(false) and the expression inside it is deleted rather than skipped. The probe's
    argument is a function call that increments a counter, so a counter that never moves is an
    expression that was never compiled.
    """
    name, lib = lin_variant
    lib.trace_reset()
    lib.c.AlxLinTest_TraceProbe()
    wrn, _dbg, _vrb = lib.arg_evals()

    if VARIANTS[name][1] is False:
        assert wrn == 0, f"variant {name!r} compiles no trace macros, yet an argument was evaluated"
    else:
        assert wrn == 1, f"variant {name!r} enables this module's traces, yet the WRN argument did not run"


def test_ALX1553_P535_the_global_threshold_is_a_compile_time_gate_not_a_filter(lin_variant):
    """Gate B, alxTrace.h:93-127 - the half a run-time log level would get wrong.

    `default` and `assert_trace` sit at INF(4). DBG is 5 and VRB is 6, so both are BELOW the
    threshold and their call sites do not exist in those images - while the WRN site in the very
    same function does. One binary, three counters, and the WRN one moving is what makes the two
    zeros mean something.
    """
    name, lib = lin_variant
    lib.trace_reset()
    lib.c.AlxLinTest_TraceProbe()
    assert lib.arg_evals() == EXPECTED[name][0]


def test_ALX1553_P536_a_counter_that_does_not_move_is_a_counter_that_works(lin_variant):
    """The control, and without it P534 and P535 prove nothing.

    A probe that was never linked in, or a counter that is broken, reads exactly like a trace that
    was elided - all three give zero. These three calls reach the same counters from OUTSIDE any
    trace macro, so they must move in every variant including `off`.
    """
    _name, lib = lin_variant
    lib.trace_reset()
    lib.c.AlxLinTest_BumpWrnDirect()
    lib.c.AlxLinTest_BumpDbgDirect()
    lib.c.AlxLinTest_BumpVrbDirect()
    assert lib.arg_evals() == (1, 1, 1)
    assert lib.traces() == 0, "a direct call to the probe's own counter reached the trace sink"


# --------------------------------------------------- the library's own call sites ----
def test_ALX1553_P537_the_librarys_own_trace_sites_follow_the_same_two_gates(lin_variant):
    """Not the probe - alxLin.c's own code, one site per level, one ordinary call each.

    The probe proves the mechanism; this proves the mechanism is what the library is actually
    subject to. In `default` - the configuration products ship - two of these three reports do not
    exist, and the module's source gives no sign of it.
    """
    name, lib = lin_variant
    expected_wrn, expected_dbg, expected_vrb = EXPECTED[name][1]
    pid_good = lib.protected_id(UNCLAIMED_ID)

    lib.trace_reset()
    _drive_wrn(lib)
    assert lib.traces() == expected_wrn, f"{name}: alxLin.c:{SITE_WRN_LINE}"
    assert lib.traces_at(LEVEL_WRN) == expected_wrn

    lib.trace_reset()
    _drive_rx(lib, BAD_PARITY_PID)
    assert lib.traces() == expected_dbg, f"{name}: alxLin.c:{SITE_DBG_LINE}"
    assert lib.traces_at(LEVEL_DBG) == expected_dbg

    lib.trace_reset()
    _drive_rx(lib, pid_good)
    assert lib.traces() == expected_vrb, f"{name}: alxLin.c:{SITE_VRB_LINE}"
    assert lib.traces_at(LEVEL_VRB) == expected_vrb


def test_ALX1553_P538_a_recorded_trace_names_its_level_and_the_header_it_came_from(lin_variant):
    """CHARACTERIZATION - P513, on the trace path this time.

    ALX_LIN_FILE is "alxLin.h" while __LINE__ comes from alxLin.c, so a trace reports a file and a
    line that never occur together, exactly as an assertion does. Sealed for assertions in
    test_alxAssert.py; recorded here because it is the same macro-level defect reached by a
    different route, and because it is what makes the level and the function worth checking apart
    from it - those two ARE right.
    """
    name, lib = lin_variant
    if name != "debug":
        pytest.skip("only `debug` compiles a VRB site at all - that is P535")

    lib.trace_reset()
    _drive_rx(lib, lib.protected_id(UNCLAIMED_ID))
    level, file, line, fun = lib.last_trace()

    assert level == LEVEL_VRB
    assert fun == "AlxLin_RxBuff_Handle"
    assert line == SITE_VRB_LINE
    assert file == "alxLin.h", "the file stopped being the header - P513 may have been fixed"


# ------------------------------------------------------------------- the harness itself ----
def test_ALX1553_P539_every_variant_configures_this_group_completely():
    """The generated -D flags, checked for what is PRESENT and not only for what is absent.

    _variant_defines derives a module's macro prefix from the ALX_<M>_ASSERT_* block in its header,
    so a trace-bearing module that happened to declare no assertions would silently receive no
    ALX_<M>_TRACE_ENABLE - and every test above would pass by proving nothing, because a missing
    gate and a closed gate give the same zero. Asserting the macro is there is what closes that.
    """
    for name, (form, module_trace, level) in VARIANTS.items():
        defines = {d.removeprefix("-D") for d in _variant_defines(name, LIN_SOURCES)}

        assert [d for d in defines if d.startswith("ALX_TRACE_LEVEL=")] == [f"ALX_TRACE_LEVEL={level}"]

        if module_trace:
            assert "ALX_LIN_TRACE_ENABLE" in defines, f"{name}: the module's trace gate is missing"
        else:
            assert "ALX_LIN_TRACE_ENABLE" not in defines, f"{name}: `off` is not off"

        forms = sorted(d for d in defines if d.startswith("ALX_LIN_ASSERT_"))
        assert forms == ([] if form is None else [f"ALX_LIN_ASSERT_{form}_ENABLE"]), f"{name}: {forms}"


def test_ALX1553_P540_every_declared_dependency_of_every_group_exists():
    """A dependency that is not there is not a dependency - it is an unconditional rebuild.

    host_build.needs_build treats a path it cannot stat as "rebuild and let the compiler say so",
    which is the right default and makes a WRONG path invisible: the group still builds, the tests
    still pass, and the only symptom is time. Found 11.09 because two fixtures came to own one DLL
    for the first time - LIN_DEPS named alxSerialPort.h at the library root and MUX_DEPS named
    alxIoPin.h there, when both live under Mcu/, so those two groups had been rebuilt on every
    pytest run since they were written and the headers they were meant to track were tracked by
    nobody.

    On Windows that stopped being free: rebuilding a DLL another fixture in the same session has
    already loaded fails with 'permission denied', which is how it finally surfaced.
    """
    missing = []
    for name in dir(conftest):
        if not name.endswith(("_DEPS", "_SOURCES", "_SOURCES_STRICT", "_SOURCES_CLOSURE")):
            continue
        value = getattr(conftest, name)
        if not isinstance(value, (list, tuple)):
            continue
        missing += [f"{name}: {path}" for path in value
                    if isinstance(path, Path) and not path.exists()]

    assert not missing, "declared but absent, so the group rebuilds every run: " + "; ".join(missing)
