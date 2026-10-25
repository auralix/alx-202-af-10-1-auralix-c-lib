"""ALX-1553 - alxAvg PC unit tests (Tier 1: a moving average over a caller-owned buffer, pure).

An average of the last N samples, where the buffer belongs to the CALLER and the module never clears
it. That is the whole of P175, and it is the reason this file exists: over a zeroed array a constant
input reads as a ramp, which looks like a filter settling and is not, and over an uninitialised one
the first window is undefined values that will look like plausible measurements.

P176 records the other trap: a shift threshold of zero means the average is never calculated at all,
and zero is what a partially initialised configuration structure holds.

Test group P169-P178 = ALX-1553 signal conditioning proofs, split by module.
"""

import pytest

pytestmark = pytest.mark.unit

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
