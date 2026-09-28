"""ALX-1553 - alxFiltGlitchUint32 PC unit tests (Tier 2: the value filter over a clock we own).

The other glitch filter: any change of a uint32 has to hold for one stable time before it reaches the
output. Nothing tested it. Same argument for testing it on a host as for its boolean sibling - the
tick is the test's, so the boundary is exact rather than approximate.

Two behaviours are recorded rather than asserted as design:
  P163 the boundary is AT the configured time - its boolean sibling changes strictly after it, and
       neither header says which convention it follows;
  P167 the elapsed time goes through a 32-bit float, so the filter fires slightly early once the
       window is long.

Test group P159-P168 = ALX-1553 glitch filter proofs, split by module with alxFiltGlitchBool.
"""

import pytest

pytestmark = pytest.mark.unit

SHORT_MS, LONG_MS = 100.0, 1000.0
RISE_MS = 10.0
FALL_MS = 50.0
US = 0.001          # one microsecond, expressed in the milliseconds the helpers take


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


def test_ALX1553_P163_the_uint32_filter_changes_at_its_time(filt):
    """CHARACTERIZATION: at EXACTLY the configured time this filter HAS already changed.

    It compares ``stableTime_ms >= me->stableTime_ms``. Its boolean sibling uses a strict
    greater-than and changes one tick later - same library, same purpose, opposite convention, and
    neither header says which. The matching half is test_alxFiltGlitchBool.py P163.
    """
    flt = filt.uint32(0, 5.0)
    filt.process(flt, 7)
    filt.advance_ms(5.0 - US)
    assert filt.process(flt, 7) == 0, "changed before its configured time"
    filt.advance_ms(US)
    assert filt.process(flt, 7) == 7, "did not change AT its configured time"
