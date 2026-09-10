"""ALX-1553 - alxFiltGlitchBool and alxFiltGlitchUint32 PC unit tests (Tier 2: over a clock we own).

Two debounce filters, and nothing tested either. They matter because they sit in front of decisions:
a digital input that chatters, a mode that flickers, a state a product acts on. A filter that is one
sample too eager passes the glitch it exists to remove, and a filter that is one sample too slow
delays every real change - and neither shows up in a build, in a review or on a bus.

They are worth testing HERE rather than on a bench for the same reason: on hardware the boundary can
only be approached to within a sampling interval, so an off-by-one in a comparison is invisible.
Here the tick counter is the test's - time moves only when the test moves it - so each boundary can
be measured to the microsecond from both sides. The timer and the tick are the real modules; only
the interrupt lock is faked, because it is CMSIS intrinsics that do not exist on a PC.

The two filters do not agree with each other about their own boundary. That is P163, and it is the
finding this file exists for.

Test group P159-P168 = ALX-1553 glitch filter proofs.
"""

import pytest

pytestmark = pytest.mark.unit

RISE_MS = 10.0
FALL_MS = 50.0
US = 0.001          # one microsecond, expressed in the milliseconds the helpers take


# =====================================================================
# P159-P162 - the boolean filter
# =====================================================================


def test_ALX1553_P159_the_boolean_filter_holds_until_the_input_has_been_stable(filt):
    """A change is only believed after the input has held it for the configured time.

    Stepped through the whole approach rather than jumped: at construction, immediately after the
    input changes, and at every point up to the boundary, the OLD value is what the filter reports.
    """
    flt = filt.boolean(False, RISE_MS, RISE_MS)
    assert filt.process(flt, False) is False, "the initial value, before anything happens"
    assert filt.process(flt, True) is False, "the change is seen but not believed"
    for elapsed in (1.0, 5.0, 9.0, RISE_MS):
        filt.tick_reset()
        flt = filt.boolean(False, RISE_MS, RISE_MS)
        filt.process(flt, True)
        filt.advance_ms(elapsed)
        assert filt.process(flt, True) is False, f"believed after only {elapsed} ms"
    filt.advance_ms(US)
    assert filt.process(flt, True) is True, "not believed even after the full time"


def test_ALX1553_P160_the_rise_and_fall_times_are_independent(filt):
    """A filter configured 10 ms up and 50 ms down uses each time for its own direction.

    One time used for both directions is the mistake this catches, and it is invisible whenever a
    test configures the two the same - which is why every other test here does configure them the
    same, and this one does not.
    """
    flt = filt.boolean(False, RISE_MS, FALL_MS)
    filt.process(flt, True)
    filt.advance_ms(RISE_MS + US)
    assert filt.process(flt, True) is True, "the rise took longer than the rise time"

    filt.process(flt, False)
    filt.advance_ms(RISE_MS + US)
    assert filt.process(flt, False) is True, "the fall used the RISE time"
    filt.advance_ms(FALL_MS - RISE_MS)
    assert filt.process(flt, False) is False, "the fall never happened"


def test_ALX1553_P161_a_glitch_shorter_than_the_time_is_rejected_and_the_window_restarts(filt):
    """Nine milliseconds of true, one sample of false, nine more of true: still false.

    This is the filter's whole purpose, and it is the one property that a naive implementation -
    counting total time rather than CONSECUTIVE time - gets wrong. The single matching sample in the
    middle has to throw the accumulated time away.
    """
    flt = filt.boolean(False, RISE_MS, RISE_MS)
    filt.process(flt, True)
    filt.advance_ms(9.0)
    assert filt.process(flt, True) is False

    assert filt.process(flt, False) is False, "one sample agreeing with the output"
    filt.advance_ms(9.0)
    assert filt.process(flt, True) is False, "the nine milliseconds before the glitch still counted"
    filt.advance_ms(1.0 + US)
    assert filt.process(flt, True) is True, "a full window after the glitch was not enough"


def test_ALX1553_P162_reset_returns_the_initial_value_and_restarts_the_window(filt):
    """Whatever it has settled to, Reset puts back the constructed value AND restarts the clock.

    The second half is what makes Reset usable. Time is left running between calls (P168), so a
    filter reset after a long idle period would otherwise accept the very next disagreeing sample:
    the hundred milliseconds here are more than the window, and it is Reset that makes them not
    count.
    """
    flt = filt.boolean(False, RISE_MS, RISE_MS)
    filt.process(flt, True)
    filt.advance_ms(RISE_MS + US)
    assert filt.process(flt, True) is True, "the filter did not settle in the first place"

    filt.advance_ms(100.0)                  # a long gap with nobody calling Process
    filt.reset(flt)
    assert filt.process(flt, True) is False, "Reset returned neither the initial value nor a window"
    filt.advance_ms(RISE_MS)
    assert filt.process(flt, True) is False, "the window did not start at the Reset"
    filt.advance_ms(US)
    assert filt.process(flt, True) is True


# =====================================================================
# P163 - the finding
# =====================================================================


def test_ALX1553_P163_the_two_filters_disagree_about_their_own_boundary(filt):
    """CHARACTERIZATION: at EXACTLY the configured time, one filter has changed and the other has not.

    Measured to the microsecond, on a clock the test owns:

        AlxFiltGlitchBool     at 10.000 ms: unchanged     at 10.001 ms: changed
        AlxFiltGlitchUint32   at  5.000 ms: CHANGED

    The boolean filter compares ``unstableTime_us > stableTime_us``; the uint32 filter compares
    ``stableTime_ms >= me->stableTime_ms``. Same library, same purpose, opposite convention at the
    boundary, and neither header says which it is.

    A microsecond either way is not a defect and this test does not call it one. It is recorded
    because it is exactly the kind of difference that is decided once, silently, by whoever wrote
    each file, and then relied on by someone reading the other one - and because a test that pins
    both makes the next person's choice deliberate.
    """
    boolean = filt.boolean(False, RISE_MS, RISE_MS)
    filt.process(boolean, True)
    filt.advance_ms(RISE_MS)
    assert filt.process(boolean, True) is False, "the boolean filter changed AT its time"

    filt.tick_reset()
    integer = filt.uint32(0, 5.0)
    filt.process(integer, 7)
    filt.advance_ms(5.0)
    assert filt.process(integer, 7) == 7, "the uint32 filter did NOT change at its time"


# =====================================================================
# P164-P165 - the uint32 filter
# =====================================================================


def test_ALX1553_P164_the_uint32_filter_passes_a_value_only_once_it_has_been_stable(filt):
    """The old value stays on the output until the new one has been held for the whole time."""
    flt = filt.uint32(0, RISE_MS)
    assert filt.process(flt, 0) == 0
    assert filt.process(flt, 7) == 0, "the new value appeared immediately"
    filt.advance_ms(RISE_MS - US)
    assert filt.process(flt, 7) == 0, "the new value appeared before its time"
    filt.advance_ms(US)
    assert filt.process(flt, 7) == 7


def test_ALX1553_P165_a_value_that_keeps_changing_never_reaches_the_output(filt):
    """Every change restarts the window, so a value that alternates is held off indefinitely.

    Twenty alternations spread over twenty times the stable time: without the restart the output
    would have followed long before the end.
    """
    flt = filt.uint32(0, RISE_MS)
    for i in range(20):
        filt.advance_ms(RISE_MS)
        assert filt.process(flt, 1 + (i % 2)) == 0, f"the output followed after {i} alternations"
    filt.advance_ms(RISE_MS + US)
    assert filt.process(flt, 1 + (19 % 2)) == 2, "a value held steady never got through either"


# =====================================================================
# P166-P167 - what the two constructors and the float conversion can express
# =====================================================================


@pytest.mark.parametrize("ms", [1.0, 10.0, 250.0, 1000.0])
def test_ALX1553_P166_the_millisecond_and_microsecond_constructors_agree(filt, ms):
    """The same duration, configured both ways, changes state on the same microsecond."""
    for elapsed, expected in ((ms, False), (ms + US, True)):
        filt.tick_reset()
        from_ms = filt.boolean(False, ms, ms)
        from_us = filt.boolean_us(False, int(ms * 1000), int(ms * 1000))
        filt.process(from_ms, True)
        filt.process(from_us, True)
        filt.advance_ms(elapsed)
        assert filt.process(from_ms, True) is expected
        assert filt.process(from_us, True) is expected


@pytest.mark.parametrize(("nominal_ms", "measured_us", "error_us"), [
    (10.0, 10_000, 0),
    (1000.0, 1_000_000, 0),
    (100_000.0, 99_999_996, -4),
    (10_000_000.0, 9_999_999_489, -511),
])
def test_ALX1553_P167_the_uint32_filter_loses_resolution_as_its_window_grows(
    filt, nominal_ms, measured_us, error_us, record_property
):
    """METRIC: the uint32 filter converts elapsed microseconds to a 32-bit float, and it costs.

    ``AlxTimSw_Get_us(&me->tim) / 1000.f`` puts a uint64 count of microseconds through a float, whose
    24-bit mantissa runs out at about 16.8 million. Past that the comparison is made on a rounded
    number, so the filter changes state slightly EARLY. Binary-searched to the microsecond:

        10 ms         exact
        1 s           exact
        100 s         4 us early
        10 000 s      511 us early

    Half a millisecond after nearly three hours is nothing, and the boolean filter does not have the
    problem at all because it keeps microseconds as uint64 throughout. This is here because it is a
    number rather than an impression: if a product ever configures a window of hours and cares about
    its edge, this says what it will get.
    """
    filt.tick_reset()
    flt = filt.uint32(0, nominal_ms)
    filt.process(flt, 7)
    filt.advance_ms((measured_us - 1) / 1000)
    assert filt.process(flt, 7) == 0, "changed one microsecond before the measured boundary"
    filt.advance_ms(US)
    assert filt.process(flt, 7) == 7, "did not change at the measured boundary"
    record_property(f"uint32_window_{nominal_ms:.0f}ms_error_us", error_us)


def test_ALX1553_P168_the_window_is_elapsed_time_not_observed_samples(filt):
    """CHARACTERIZATION: the filter measures TIME since the last agreeing sample, not what it saw.

    Both filters ask a software timer how long it has been since the input last agreed with the
    output. Nothing counts samples, and nothing notices that the caller went away. So a caller whose
    loop stalls - a blocking write, a long flash erase, a lower-priority task starved - comes back
    to a filter that accepts the FIRST disagreeing sample it is given, because the whole stall
    counted as stable time it never actually observed.

    Measured: settle to true, stop calling Process for a hundred milliseconds, then feed ONE false
    sample to a filter configured for ten. It changes on that one sample.

    Not a defect: a debounce filter that trusts wall time is a normal design and the cheaper one.
    Recorded because it is a real precondition on the caller - Process must be called at a rate
    faster than the window - and nothing in the header says so. It is also the reason P162 has to
    prove that Reset restarts the clock, and the reason a product that can stall should Reset after
    it does.
    """
    flt = filt.boolean(False, RISE_MS, RISE_MS)
    filt.process(flt, True)
    filt.advance_ms(RISE_MS + US)
    assert filt.process(flt, True) is True, "the filter did not settle in the first place"

    filt.advance_ms(100.0)                  # the stall: ten windows, and Process is never called
    assert filt.process(flt, False) is False, "one sample after the stall did not flip the output"
