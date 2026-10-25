"""ALX-1553 - alxMath PC unit tests (Tier 1: a running count, sum, mean, minimum and maximum).

Five numbers accumulated over uint32 samples. Small enough to read in a minute and worth pinning
anyway: the mean is integer division and truncates, and before the first sample the module reports a
minimum of 4294967295 and a maximum of 0 - a pair that looks like a measurement and is not, with
``count == 0`` the only thing that says otherwise.

Test group P169-P178 = ALX-1553 signal conditioning proofs, split by module.
"""

import pytest

pytestmark = pytest.mark.unit

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
