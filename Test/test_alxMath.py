"""ALX-1553 - alxHys1, alxHys2, alxAvg and alxMath PC unit tests (Tier 1: pure, no clock, no device).

The library's signal conditioning: two hysteresis state machines, a moving average and a running
count/sum/mean/min/max. None of them had a test. They are worth having because a product does not
read a sensor, it reads a DECISION about a sensor - is the input high, has the mode changed, what
was the peak - and all four of these turn a stream of numbers into one.

They are also the cheapest things in the library to test: no clock, no peripheral, no state outside
their own structure. A sequence of samples goes in and an answer comes out, so a test is a list.

Three behaviours are recorded rather than asserted as design, and all three are start-up behaviours -
which is where a module with memory is least likely to have been thought about:
  P171 the two hysteresis modules choose DIFFERENT initial states, and one of them reports "high"
       before it has seen a single sample;
  P175 the moving average never clears the caller's buffer, so its first window is diluted;
  P178 the running statistics report a minimum of 4294967295 until the first sample arrives.

Test group P169-P178 = ALX-1553 signal conditioning proofs.
"""

import pytest

pytestmark = pytest.mark.unit

HIGH, LOW = 10.0, 5.0                       # one hysteresis band, with a 5.0 wide dead zone
TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW = 10.0, 8.0, 3.0, 1.0


# =====================================================================
# P169-P171 - one threshold pair
# =====================================================================


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


def test_ALX1553_P171_the_two_hysteresis_modules_start_in_different_states(math_lib):
    """CHARACTERIZATION: before any sample, one reports HIGH and the other reports MIDDLE.

    ``AlxHys1_Ctor`` sets ``st = AlxHys1_StHigh``; ``AlxHys2_Ctor`` sets ``st = AlxHys2_StMid``. Both
    are the natural choice seen from inside their own file - the first enum value, and the neutral
    one - and together they are a trap: two modules of the same family disagree about what "not yet
    measured" means, and neither header says.

    It matters most for AlxHys1. A sample inside the dead band cannot move it, so a product whose
    input starts in the band reads HIGH until the input leaves the band in either direction. If that
    module is behind an over-range or over-temperature decision, its first answer is the alarming one
    and it did not measure anything to get there.

    Not a defect - nothing documents an initial state, so nothing is violated. Recorded because the
    day someone wants a defined start-up value, this is the test that says what today's is.
    """
    one = math_lib.hys1(HIGH, LOW)
    two = math_lib.hys2(TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW)
    middle_of_the_band = (HIGH + LOW) / 2

    assert math_lib.hys1_process(one, middle_of_the_band) == math_lib.HYS1_HIGH
    assert math_lib.hys2_process(two, middle_of_the_band) == math_lib.HYS2_MID


# =====================================================================
# P172-P173 - two threshold pairs
# =====================================================================


def test_ALX1553_P172_hys2_never_moves_between_top_and_bottom_without_passing_through_the_middle(
    math_lib
):
    """One sample can move the state by one step, never by two, however far the input jumps.

    A consumer switching on the state can therefore trust that it will see the middle - which is the
    property a three-state machine is chosen FOR, and the one a flat comparison would not have.
    """
    hys = math_lib.hys2(TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW)
    assert math_lib.hys2_process(hys, 20.0) == math_lib.HYS2_TOP

    assert math_lib.hys2_process(hys, -50.0) == math_lib.HYS2_MID, "jumped straight past the middle"
    assert math_lib.hys2_process(hys, -50.0) == math_lib.HYS2_BOT

    assert math_lib.hys2_process(hys, 50.0) == math_lib.HYS2_MID, "jumped straight past the middle"
    assert math_lib.hys2_process(hys, 50.0) == math_lib.HYS2_TOP


@pytest.mark.parametrize(("start_at", "value", "expected"), [
    (20.0, TOP_LOW, "MID"),        # topLow leaves the top, inclusive
    (20.0, TOP_LOW + 0.1, "TOP"),  # and only just above it does not
    (None, TOP_HIGH, "TOP"),       # topHigh enters the top from the middle, inclusive
    (None, TOP_HIGH - 0.1, "MID"),
    (None, BOT_LOW, "BOT"),        # botLow enters the bottom from the middle, inclusive
    (None, BOT_LOW + 0.1, "MID"),
    (-50.0, BOT_HIGH, "MID"),      # botHigh leaves the bottom, inclusive
    (-50.0, BOT_HIGH - 0.1, "BOT"),
])
def test_ALX1553_P173_each_hys2_threshold_drives_exactly_one_transition(
    math_lib, start_at, value, expected
):
    """All four thresholds, each checked at its own boundary and one tenth to the wrong side of it.

    Four thresholds is where a copy-and-paste error lives, and every one of them is inclusive.
    """
    hys = math_lib.hys2(TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW)
    if start_at is not None:
        math_lib.hys2_process(hys, start_at)          # into the top or the bottom, twice if needed
        math_lib.hys2_process(hys, start_at)
    state = math_lib.hys2_process(hys, value)
    assert state == getattr(math_lib, f"HYS2_{expected}")


# =====================================================================
# P174-P176 - the moving average
# =====================================================================


def test_ALX1553_P174_a_full_window_averages_exactly_what_is_in_it(math_lib):
    """Once every slot has been written the answer is the plain mean of the last buffLen samples."""
    avg = math_lib.avg(4, 1)
    for value in (1.0, 2.0, 3.0, 4.0):
        result = math_lib.avg_process(avg, value)
    assert result == pytest.approx(2.5), "the mean of 1, 2, 3, 4"
    assert math_lib.avg_process(avg, 8.0) == pytest.approx(4.25), "8 replaced the oldest sample, 1"
    assert math_lib.avg_process(avg, 8.0) == pytest.approx(5.75), "and then 2"


def test_ALX1553_P175_the_first_window_is_diluted_by_whatever_the_buffer_held(math_lib):
    """CHARACTERIZATION: AlxAvg_Ctor never touches the caller's buffer, and the average includes it.

    The module averages the WHOLE buffer from the first sample onward, not the samples it has
    actually received, so for the first buffLen calls the answer is a blend of real data and
    whatever was in the array beforehand. Over a zeroed buffer that reads as a ramp - 1.0, 2.0, 3.0,
    4.0 for a constant input of 4.0 into four slots - which looks like a filter settling and is not.

    The buffer belongs to the caller, so this is the caller's job. The hazard is that nothing says
    so: a product that declares the array without an initialiser gets undefined numbers out of its
    first window, and they will look like plausible measurements rather than like garbage. Measured
    both ways here, over a zeroed buffer and over one filled with 100.0.
    """
    over_zeroes = math_lib.avg(4, 1, fill=0.0)
    ramp = [math_lib.avg_process(over_zeroes, 4.0) for _ in range(4)]
    assert ramp == pytest.approx([1.0, 2.0, 3.0, 4.0]), "not a settling filter, an unfilled buffer"

    over_hundreds = math_lib.avg(4, 1, fill=100.0)
    decay = [math_lib.avg_process(over_hundreds, 4.0) for _ in range(4)]
    assert decay == pytest.approx([76.0, 52.0, 28.0, 4.0]), "the same effect, from a filled buffer"


@pytest.mark.parametrize("threshold", [1, 2, 4])
def test_ALX1553_P176_the_shift_threshold_says_how_often_the_average_is_recomputed(
    math_lib, threshold
):
    """Between recomputations the previous answer is repeated, unchanged, however new the samples.

    Eight samples of a constant into a four-slot window: the answer may only move on the calls where
    the shift counter reaches the threshold, and on every other call it has to be exactly what it
    was. Checked against the schedule rather than against a list of numbers, so the same test says
    the same thing for every threshold.
    """
    avg = math_lib.avg(4, threshold)
    answers = [math_lib.avg_process(avg, 4.0) for _ in range(8)]
    previous = 0.0
    for index, answer in enumerate(answers):
        if (index + 1) % threshold == 0:
            previous = answer
        else:
            assert answer == previous, f"call {index + 1} recomputed off schedule"
    assert answers[-1] == pytest.approx(4.0), "eight samples of 4.0 into four slots is 4.0"


def test_ALX1553_P176_a_shift_threshold_of_zero_never_recomputes_at_all(math_lib):
    """CHARACTERIZATION: threshold 0 means the average is never calculated, and nothing says so.

    The counter is incremented and THEN compared, so it is 1 on the first call and never 0 again.
    The module keeps accepting samples, keeps filling the buffer and keeps returning 0.0 forever.
    Neither the constructor nor the header rejects or mentions a threshold of zero, and zero is the
    value a partially initialised configuration structure has.
    """
    avg = math_lib.avg(4, 0)
    assert [math_lib.avg_process(avg, 7.0) for _ in range(20)] == [0.0] * 20


# =====================================================================
# P177-P178 - the running statistics
# =====================================================================


def test_ALX1553_P177_count_sum_min_and_max_are_exact_and_the_mean_truncates(math_lib):
    """The four exact numbers are exact; the mean is integer division, so it rounds toward zero."""
    math_obj = math_lib.math()
    for value in (5, 1, 9):
        data = math_lib.math_process(math_obj, value)
    assert (data.count, data.sum, data.min, data.max) == (3, 15, 1, 9)
    assert data.avg == 5

    math_obj = math_lib.math()
    for value in (1, 2):
        data = math_lib.math_process(math_obj, value)
    assert data.sum == 3
    assert data.avg == 1, "3 / 2 is 1 in a uint32, not 1.5 and not 2"


def test_ALX1553_P178_before_the_first_sample_the_minimum_is_the_largest_uint32(math_lib):
    """CHARACTERIZATION: the module reports min 4294967295 and max 0 until it has seen something.

    The constructor has to seed the minimum with something a real sample beats, and the largest
    uint32 is the correct choice - the alternative, seeding it with zero, would pin the minimum at
    zero forever. The consequence is that a reader that polls before the first sample gets a pair of
    numbers that look like a measurement and are not, with no count to warn it except ``count == 0``.

    Pinned so that a future change to the seed is a deliberate one, and so the field's meaning is
    written down somewhere: ``count == 0`` is the only thing that says "nothing here yet".
    """
    math_obj = math_lib.math()
    first = math_lib.math_process(math_obj, 42)
    assert (first.count, first.min, first.max) == (1, 42, 42), "one sample is its own min and max"

    fresh = math_lib.math()
    after = math_lib.math_process(fresh, 4294967295)
    assert after.min == 4294967295, "the seed and a real maximum-valued sample are indistinguishable"
    assert after.count == 1, "count is the only field that separates them"
