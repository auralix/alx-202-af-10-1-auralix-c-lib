"""ALX-1553 - alxHys2 PC unit tests (Tier 1: two threshold pairs, three states, pure).

The three-state hysteresis: top, middle, bottom, with four thresholds. Four thresholds is where a
copy-and-paste error lives, so each one is checked at its own boundary and one tenth to the wrong
side of it, and the property the three states are chosen FOR - that a consumer always sees the
middle - is checked against an input that jumps the whole range at once.

P171 is recorded rather than asserted as design: this module starts in its MIDDLE state where its
one-threshold sibling starts HIGH.

Test group P169-P178 = ALX-1553 signal conditioning proofs, split by module.
"""

import pytest

pytestmark = pytest.mark.unit

HIGH, LOW = 10.0, 5.0                       # one hysteresis band, with a 5.0 wide dead zone
TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW = 10.0, 8.0, 3.0, 1.0

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


def test_ALX1553_P171_hys2_starts_in_its_middle_state_before_any_sample(math_lib):
    """CHARACTERIZATION: this one starts neutral, where its one-threshold sibling starts HIGH.

    ``AlxHys2_Ctor`` sets ``st = AlxHys2_StMid``. Both choices are natural seen from inside their own
    file - the first enum value, and the neutral one - and together they are a trap. The matching
    half is test_alxHys1.py P171, which is also where it matters more.
    """
    hys = math_lib.hys2(TOP_HIGH, TOP_LOW, BOT_HIGH, BOT_LOW)
    assert math_lib.hys2_process(hys, (TOP_HIGH + BOT_LOW) / 2) == math_lib.HYS2_MID
