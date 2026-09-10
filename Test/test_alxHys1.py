"""ALX-1553 - alxHys1 PC unit tests (Tier 1: one threshold pair, pure, no clock, no device).

A hysteresis with a high and a low threshold and two states. A product does not read a sensor, it
reads a DECISION about a sensor, and this is where that decision is made - so a threshold that is
inclusive on one side and exclusive on the other, or a state that starts in the wrong place, is a
client-visible answer that no build or review would question.

P171 is recorded rather than asserted as design: the module reports HIGH before it has seen a single
sample, and its two-threshold sibling starts neutral instead.

Test group P169-P178 = ALX-1553 signal conditioning proofs, split by module.
"""

import pytest

pytestmark = pytest.mark.unit

HIGH, LOW = 10.0, 5.0                       # one hysteresis band, with a 5.0 wide dead zone
TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW = 10.0, 8.0, 3.0, 1.0

def test_ALX1553_P169_hys1_changes_only_outside_the_dead_band(math_lib):
    """Below low it is low, above high it is high, and in between it keeps what it had.

    Walked up and back down through the band rather than jumped, because the whole point of a
    hysteresis is that the answer in the middle depends on the direction of travel.
    """
    hys = math_lib.hys1(HIGH, LOW)
    assert math_lib.hys1_process(hys, LOW) == math_lib.HYS1_LOW, "start it from a known state"

    for value in (5.1, 7.0, 9.9):
        assert math_lib.hys1_process(hys, value) == math_lib.HYS1_LOW, f"{value} is inside the band"
    assert math_lib.hys1_process(hys, 10.1) == math_lib.HYS1_HIGH

    for value in (9.9, 7.0, 5.1):
        assert math_lib.hys1_process(hys, value) == math_lib.HYS1_HIGH, "the same values, going down"
    assert math_lib.hys1_process(hys, 4.9) == math_lib.HYS1_LOW


def test_ALX1553_P170_hys1_thresholds_are_inclusive_on_both_sides(math_lib):
    """Exactly ``high`` enters the high state and exactly ``low`` enters the low one.

    ``>=`` and ``<=`` in the source; pinned because a threshold that is inclusive on one side and
    exclusive on the other is a real and common asymmetry, and because it is invisible until the
    input lands exactly on the number - which, with a value that comes from a fixed-point ADC, it
    does far more often than a float suggests.
    """
    hys = math_lib.hys1(HIGH, LOW)
    math_lib.hys1_process(hys, LOW)
    assert math_lib.hys1_process(hys, HIGH) == math_lib.HYS1_HIGH, "exactly high did not rise"
    assert math_lib.hys1_process(hys, LOW) == math_lib.HYS1_LOW, "exactly low did not fall"


def test_ALX1553_P171_hys1_starts_in_its_high_state_before_any_sample(math_lib):
    """CHARACTERIZATION: before it has measured anything, this filter already reports HIGH.

    ``AlxHys1_Ctor`` sets ``st = AlxHys1_StHigh``, the first value of the enum. Its two-threshold
    sibling starts in the MIDDLE instead (test_alxHys2.py P171), so two modules of the same family
    disagree about what "not yet measured" means and neither header says.

    It matters more here. A sample inside the dead band cannot move this filter, so a product whose
    input starts in the band reads HIGH until the input leaves the band in either direction. If the
    module is behind an over-range or over-temperature decision, its first answer is the alarming one
    and it did not measure anything to get there.

    Not a defect - nothing documents an initial state, so nothing is violated. Recorded so that the
    day someone wants a defined start-up value, this is the test that says what today's is.
    """
    hys = math_lib.hys1(HIGH, LOW)
    assert math_lib.hys1_process(hys, (HIGH + LOW) / 2) == math_lib.HYS1_HIGH
