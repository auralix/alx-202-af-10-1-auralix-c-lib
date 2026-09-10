"""ALX-1553 - alxFiltGlitchBool PC unit tests (Tier 2: a debounce filter over a clock we own).

A boolean glitch filter with separate rise and fall times, and nothing tested it. It sits in front of
decisions - a digital input that chatters, a mode that flickers - and one sample too eager passes the
glitch it exists to remove, while one too slow delays every real change. Neither shows up in a build,
in a review or on a bus.

It is worth testing HERE rather than on a bench because on hardware a boundary can only be approached
to within a sampling interval, so an off-by-one in a comparison is invisible there. The tick counter
is the test's: time moves only when the test moves it, so each boundary is measured to the
microsecond from both sides. The timer and the tick are the real modules; only the interrupt lock is
faked.

Two behaviours are recorded rather than asserted as design:
  P163 the boundary is STRICTLY after the configured time - its uint32 sibling changes AT it, and
       neither header says which convention it follows;
  P168 the window is elapsed TIME since the last agreeing sample, not the samples observed, so a
       caller whose loop stalls accepts the first disagreeing sample it is handed afterwards.

Test group P159-P168 = ALX-1553 glitch filter proofs, split by module with alxFiltGlitchUint32.
"""

import pytest

pytestmark = pytest.mark.unit

SHORT_MS, LONG_MS = 100.0, 1000.0
RISE_MS = 10.0
FALL_MS = 50.0
US = 0.001          # one microsecond, expressed in the milliseconds the helpers take


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


def test_ALX1553_P163_the_boolean_filter_changes_strictly_after_its_time(filt):
    """CHARACTERIZATION: at EXACTLY the configured time this filter has NOT changed yet.

    It compares ``unstableTime_us > stableTime_us`` with a strict greater-than. Its uint32 sibling
    compares ``stableTime_ms >= me->stableTime_ms`` and therefore changes AT its time - same library,
    same purpose, opposite convention at the boundary, and neither header says which.

    A microsecond either way is not a defect and this does not call it one. It is recorded because it
    is exactly the kind of difference that is decided once, silently, by whoever wrote each file, and
    then relied on by someone reading the other one. The matching half is
    test_alxFiltGlitchUint32.py P163.
    """
    flt = filt.boolean(False, RISE_MS, RISE_MS)
    filt.process(flt, True)
    filt.advance_ms(RISE_MS)
    assert filt.process(flt, True) is False, "changed AT its configured time"
    filt.advance_ms(US)
    assert filt.process(flt, True) is True, "did not change one microsecond after it"
