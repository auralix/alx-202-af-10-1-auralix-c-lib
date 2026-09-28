"""Auralix C Library - AlxBts724g on the host.

A protected high side switch with one output pin and one status pin. The part reports TWO different
faults down that single status pin - an open load and an over-temperature - and the driver tells
them apart by what the output was asked to do: a low status with the output off means nothing is
connected, and a low status with the output on means the switch shut itself down.

Both go through glitch filters, and the two filters are deliberately different: an over-temperature
is reported after 5 ms and an open load after 100 ms, while both take 100 ms of a healthy status to
clear. Fast to trip on the dangerous one, slow on the merely informative one, slow to clear on
both.

The filter times are read back from the driver rather than restated here: they are the part's
specification, and a test that copied them would be testing its own copy.

Two things a reader should know before the tests: both filters are constructed with their initial
value TRUE, so a switch reports both faults at once for its first 100 ms whatever the pin says
(P408, sealed), and the "was detected" getters CLEAR the flag they return, so each fault can only
be read once (P405).

Proofs (ALX-1553): P400-P408.
"""

import pytest

HEALTHY = True
FAULT = False


@pytest.fixture
def switch(bts_lib):
    """One switch, initialised, healthy, and past the start-up window P408 seals.

    The driver reports both faults for its first 100 ms however healthy the part is, so a test
    about anything else runs the clock past that first and clears the latched flags.
    """
    obj = bts_lib.new()
    bts_lib.set_status(obj, HEALTHY)
    bts_lib.handle(obj, for_ms=bts_lib.clear_time_ms(obj) + 20.0)
    bts_lib.was_open_load(obj)          # the getters clear what they return
    bts_lib.was_over_temp(obj)
    return bts_lib, obj


def test_ALX1553_P400_a_healthy_switch_settles_with_no_fault(switch):
    """Once the start-up window has passed, a healthy switch reports nothing and remembers nothing."""
    lib, obj = switch

    assert lib.open_load(obj) is False
    assert lib.over_temp(obj) is False
    assert lib.was_open_load(obj) is False
    assert lib.was_over_temp(obj) is False


def test_ALX1553_P401_a_low_status_with_the_output_off_is_an_open_load(switch):
    """Nothing connected: the switch is off and its status pin is low.

    The output is off, so the part cannot be complaining about its own temperature - what it is
    saying is that it sees no load. Held for the filter's time, because a single low reading is
    not a fault.
    """
    lib, obj = switch
    lib.reset_out(obj)
    lib.set_status(obj, FAULT)

    lib.handle(obj, for_ms=lib.open_load_time_ms(obj) + 5.0)

    assert lib.open_load(obj) is True
    assert lib.over_temp(obj) is False, "an open load was also reported as an over-temperature"


def test_ALX1553_P402_a_low_status_with_the_output_on_is_an_over_temperature(switch):
    """The same pin, the other meaning: the switch was asked to conduct and shut itself down.

    Telling these apart is the whole job of the driver - one is a wiring fault a technician fixes
    and the other is the part protecting itself from a short, and they look identical on the pin.
    """
    lib, obj = switch
    lib.set_out(obj)
    lib.set_status(obj, FAULT)

    lib.handle(obj, for_ms=lib.over_temp_time_ms(obj) + 5.0)

    assert lib.over_temp(obj) is True
    assert lib.open_load(obj) is False, "an over-temperature was also reported as an open load"


def test_ALX1553_P403_the_over_temperature_is_reported_sooner_than_the_open_load(switch):
    """CHARACTERIZATION: 5 ms for an over-temperature, 100 ms for an open load.

    The asymmetry is the design: a part shutting itself down is urgent and a missing load is not,
    so the filter that guards the urgent one is twenty times shorter. Measured by holding each
    fault for the OTHER one's time and seeing which has been reported.
    """
    lib, obj = switch
    assert lib.over_temp_time_ms(obj) < lib.open_load_time_ms(obj)

    lib.set_out(obj)
    lib.set_status(obj, FAULT)
    lib.handle(obj, for_ms=lib.over_temp_time_ms(obj) + 1.0)
    assert lib.over_temp(obj) is True, "the over-temperature took longer than its own filter time"

    other = lib.new()
    lib.set_status(other, HEALTHY)
    lib.handle(other, for_ms=lib.clear_time_ms(other) + 20.0)
    lib.reset_out(other)
    lib.set_status(other, FAULT)
    lib.handle(other, for_ms=lib.over_temp_time_ms(other) + 1.0)
    assert lib.open_load(other) is False, "an open load was reported at the over-temperature's time"


def test_ALX1553_P404_a_fault_clears_after_the_status_pin_has_been_healthy(switch):
    """A fault goes away when the part stops reporting it - after the filter's clearing time.

    Both faults take the same 100 ms to clear, whichever they were: a switch that flickered back to
    healthy for one reading should not be believed any more than one that flickered into a fault.
    """
    lib, obj = switch
    lib.set_out(obj)
    lib.set_status(obj, FAULT)
    lib.handle(obj, for_ms=lib.over_temp_time_ms(obj) + 5.0)
    assert lib.over_temp(obj) is True

    lib.set_status(obj, HEALTHY)
    lib.handle(obj, for_ms=lib.clear_time_ms(obj) / 2)
    assert lib.over_temp(obj) is True, "the fault cleared before its clearing time"

    lib.handle(obj, for_ms=lib.clear_time_ms(obj))
    assert lib.over_temp(obj) is False


def test_ALX1553_P405_a_fault_that_has_happened_is_remembered_once(switch):
    """The "was detected" flags latch so a fault between two reads is not lost - and READING CLEARS.

    A caller polling once a second would otherwise miss a fault that came and went, which is
    precisely the fault worth knowing about on a switch protecting itself. The getter resets the
    flag as it returns it, so the answer is "since you last asked" and not "ever" - which means two
    callers cannot both be told, and the second one hears that nothing happened.
    """
    lib, obj = switch
    lib.set_out(obj)
    lib.set_status(obj, FAULT)
    lib.handle(obj, for_ms=lib.over_temp_time_ms(obj) + 5.0)

    lib.set_status(obj, HEALTHY)
    lib.handle(obj, for_ms=lib.clear_time_ms(obj) * 2)

    assert lib.over_temp(obj) is False, "the fault itself did not clear"
    assert lib.was_over_temp(obj) is True, "and the fact that it happened was forgotten"
    assert lib.was_over_temp(obj) is False, "reading the flag did not clear it"


def test_ALX1553_P406_the_output_pin_follows_what_the_driver_was_asked_for(switch):
    """Set, Reset and Write all reach the pin, and Write is the other two in one call."""
    lib, obj = switch

    lib.set_out(obj)
    assert lib.out_level(obj) is True

    lib.reset_out(obj)
    assert lib.out_level(obj) is False

    lib.write_out(obj, True)
    assert lib.out_level(obj) is True

    lib.write_out(obj, False)
    assert lib.out_level(obj) is False


@pytest.mark.parametrize("output_on", [True, False])
def test_ALX1553_P407_a_healthy_status_pin_reports_nothing_either_way(switch, output_on):
    """With the status pin high there is no fault, whatever the output is doing.

    The two faults are distinguished by the output state, so both states have to be checked -
    otherwise a driver that reported a fault whenever its output was on would pass half of them.
    """
    lib, obj = switch
    lib.write_out(obj, output_on)
    lib.set_status(obj, HEALTHY)

    lib.handle(obj, for_ms=lib.open_load_time_ms(obj) * 2)

    assert lib.open_load(obj) is False
    assert lib.over_temp(obj) is False
    assert lib.was_open_load(obj) is False
    assert lib.was_over_temp(obj) is False


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxBts724g_Ctor constructs both glitch filters with the initial value TRUE, and both "
    "take stableFalseTime (100 ms) of a healthy reading to fall. So from its very first Handle a "
    "switch reports an open load AND an over-temperature at the same time, for 100 ms, however "
    "healthy the part is - and the two are mutually exclusive by construction, one needing the "
    "output off and the other needing it on. Every board therefore logs both faults on every boot, "
    "and a caller reading WasOpenLoadDetected once at start-up is told about a fault that never "
    "happened. The fix is to construct the filters false, or to prime them at Init from the pin"))
def test_ALX1553_P408_a_healthy_switch_reports_nothing_from_the_first_reading(bts_lib):
    """A switch that was never faulty should not report a fault, not even for a moment.

    Measured from the first Handle with the status pin healthy throughout: both faults are reported
    together until 100 ms have passed.
    """
    obj = bts_lib.new()
    bts_lib.set_status(obj, HEALTHY)
    bts_lib.write_out(obj, False)

    bts_lib.handle(obj)

    assert bts_lib.open_load(obj) is False
    assert bts_lib.over_temp(obj) is False


def test_ALX1553_P467_init_claims_both_pins_and_deinit_releases_them(bts_lib):
    """The switch owns two pins - the output and the status - and gives both back.

    A driver that released only the one it writes would leave the status pin configured after the
    part had been shut down, which on a shared port is the next driver's problem rather than this
    one's. Counted rather than read as levels, because a de-initialised pin still answers a read.
    """
    obj = bts_lib.new()

    assert bts_lib.pin_counts(obj) == {"out": (1, 0), "status": (1, 0)}

    bts_lib.deinit(obj)

    assert bts_lib.pin_counts(obj) == {"out": (1, 1), "status": (1, 1)}
