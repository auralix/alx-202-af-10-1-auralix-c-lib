"""Auralix C Library - AlxInterpLin on the host (Tier 1: x to y, pure, no device).

A table of measured points and a straight line between each neighbouring pair - how a calibration
curve is applied: a sensor's raw reading in, the real quantity out, with the curve held as however
many points somebody measured on a bench.

These tests were written with `alxLinFun`'s, in one file, because the two modules are the library's
two ways of doing the same job and share a DLL. They are split out here so the module has the
mirror file the README's convention asks for and the mutation lane resolves - which is why the
proof tokens are not contiguous: P183-P187 are that earlier work, moved unchanged, and P409-P411
are what a second pass over the module added.

One thing a reader should know first: asking for the LAST point of the table answers NaN and
reports Alx_Ok while doing it (P185, sealed). Tests that are not about that stay off the far end.

Proofs (ALX-1553): P183-P187 and P409-P411.
"""

import math

import pytest

pytestmark = pytest.mark.unit

# A table whose second segment is steeper than its first, so interpolation cannot be faked by a
# single line: 10 per unit below x = 10, 30 per unit above it.
TABLE_X = [0.0, 10.0, 20.0]
TABLE_Y = [0.0, 100.0, 400.0]

# The line P187 compares the smallest table against: through (0, 0) and (100, 1000).
X1, Y1, X2, Y2 = 0.0, 0.0, 100.0, 1000.0
WIDE = (-1.0e6, 1.0e6)              # limits far enough away that nothing clips


# =====================================================================
# P183-P184 - the lookup table
# =====================================================================


@pytest.mark.parametrize(("x", "y"), [(0.0, 0.0), (5.0, 50.0), (10.0, 100.0), (15.0, 250.0)])
def test_ALX1553_P183_a_rising_table_interpolates_inside_each_segment(lin_fun_lib, x, y):
    """Two segments of different slope, checked inside both and at the point that joins them.

    A table whose segments have the same slope is a line, and a test over one proves nothing about
    the segment search. This one bends: 10 per unit below x = 10, 30 per unit above it.
    """
    table = lin_fun_lib.table(TABLE_X, TABLE_Y, rising=True)
    assert lin_fun_lib.table_y(table, x) == pytest.approx(y)


def test_ALX1553_P184_a_table_clips_at_both_ends_and_reports_which(lin_fun_lib):
    """Outside the table the nearest end point is returned, with ErrMin or ErrMax, both directions.

    The falling table is not a mirror of the rising one in the source - it is a second branch with
    its own comparisons - so both are checked rather than one.
    """
    rising = lin_fun_lib.table(TABLE_X, TABLE_Y, rising=True)
    assert lin_fun_lib.table_y_status(rising, -1.0) == (lin_fun_lib.ERR_MIN, pytest.approx(0.0))
    assert lin_fun_lib.table_y_status(rising, 21.0) == (lin_fun_lib.ERR_MAX, pytest.approx(400.0))

    falling = lin_fun_lib.table(TABLE_X[::-1], TABLE_Y[::-1], rising=False)
    assert lin_fun_lib.table_y_status(falling, -1.0) == (lin_fun_lib.ERR_MIN, pytest.approx(0.0))
    assert lin_fun_lib.table_y_status(falling, 21.0) == (lin_fun_lib.ERR_MAX, pytest.approx(400.0))
    assert lin_fun_lib.table_y_status(falling, 15.0) == (lin_fun_lib.OK, pytest.approx(250.0))


# =====================================================================
# P185-P186 - the defect, and the asymmetry that hides it
# =====================================================================


@pytest.mark.xfail(
    strict=True,
    reason="AlxInterpLin: an x exactly equal to the table's LAST point matches neither the "
           "out-of-range guard (>) nor the segment search (<), so both indices stay 0 and the "
           "interpolation divides by x0 - x0; the caller gets NaN and a status of Alx_Ok",
)
@pytest.mark.parametrize("rising", [True, False])
def test_ALX1553_P185_the_last_point_of_a_table_returns_its_own_y(lin_fun_lib, rising):
    """SEALED DEFECT - x exactly at the end of the table gives NaN, and says Alx_Ok.

    The rising branch guards the top with ``x > xPointArr[last]`` and searches with
    ``x < xPointArr[i]``. Both are strict, so an x exactly equal to the last point falls through
    both: the guard does not clip it and the search never breaks. ``index0`` and ``index1`` keep
    their initial value of 0, and the last line divides by ``x1 - x0``, which is zero.

    Measured on a table (0, 10, 20) -> (0, 100, 400):

        x = 20.0   ->   nan,  status Alx_Ok        (400.0 expected)

    The falling branch has the same shape and the same hole at ITS last point, x = 0.0. Both are
    covered here.

    Two things make this worse than an edge case. The status is Alx_Ok, so a caller that checks it
    is told the value is good. And the value that triggers it is the END OF THE CALIBRATION - the
    number a table is most likely to be handed exactly, because it is usually a round one.

    A one-character fix in each branch (``>=`` in the guard, or ``<=`` in the search) closes it, but
    the two choices give different answers at every OTHER point too, so it is a decision rather than
    a typo to correct blind. Strict xfail; see the task TODO.
    """
    points_x = TABLE_X if rising else TABLE_X[::-1]
    points_y = TABLE_Y if rising else TABLE_Y[::-1]
    table = lin_fun_lib.table(points_x, points_y, rising=rising)

    status, value = lin_fun_lib.table_y_status(table, points_x[-1])
    assert not math.isnan(value), f"the last point returned NaN with status {status}"
    assert value == pytest.approx(points_y[-1])


@pytest.mark.parametrize("rising", [True, False])
def test_ALX1553_P186_the_first_point_of_a_table_is_fine(lin_fun_lib, rising):
    """CHARACTERIZATION: the same exact-match at the OTHER end works, which is why P185 survived.

    At the first point the search finds the SECOND point as its upper bound - x is not less than
    x[0] but is less than x[1] - so the two indices differ and the arithmetic is ordinary. Only the
    far end has nothing beyond it to be less than. An asymmetry like that is exactly what a
    hand-written check of one end misses, and it is worth a test of its own so the difference is on
    the record rather than implied by P185's absence.
    """
    points_x = TABLE_X if rising else TABLE_X[::-1]
    points_y = TABLE_Y if rising else TABLE_Y[::-1]
    table = lin_fun_lib.table(points_x, points_y, rising=rising)

    status, value = lin_fun_lib.table_y_status(table, points_x[0])
    assert status == lin_fun_lib.OK
    assert value == pytest.approx(points_y[0])


# =====================================================================
# P187 - the smallest table
# =====================================================================


def test_ALX1553_P187_a_two_point_table_is_a_line_between_them(lin_fun_lib):
    """The smallest table there is, checked against the line module over the same two points.

    Two implementations of the same idea, agreeing - which is worth one test, because the day they
    stop agreeing one of them has changed and neither file mentions the other.
    """
    table = lin_fun_lib.table([X1, X2], [Y1, Y2], rising=True)
    line = lin_fun_lib.line(X1, Y1, X2, Y2, *WIDE)
    for x in (0.0, 1.0, 33.0, 99.0):
        assert lin_fun_lib.table_y(table, x) == pytest.approx(lin_fun_lib.y(line, x))


# =====================================================================
# P409-P411 - what the second pass added
# =====================================================================


def test_ALX1553_P409_a_reading_is_placed_along_its_segment_and_not_between_its_ends(lin_fun_lib):
    """A quarter of the way into a segment is a quarter of the way up it, not half.

    P183 checks one point per segment and both of them happen to be midpoints - where the correct
    answer and the MEAN OF THE SEGMENT'S TWO ENDS are the same number. A module that ignored x
    inside the segment and averaged its ends would pass P183 twice over. Three fractions of one
    segment separate them, and the steeper segment is used because its two ends are furthest apart.
    """
    table = lin_fun_lib.table(TABLE_X, TABLE_Y, rising=True)
    x0, x1, y0, y1 = TABLE_X[1], TABLE_X[2], TABLE_Y[1], TABLE_Y[2]

    for fraction in (0.25, 0.5, 0.75):
        x = x0 + fraction * (x1 - x0)

        assert lin_fun_lib.table_y(table, x) == pytest.approx(y0 + fraction * (y1 - y0)), (
            f"x={x} is off the segment's line"
        )

    mean_of_ends = (y0 + y1) / 2
    assert lin_fun_lib.table_y(table, x0 + 0.25 * (x1 - x0)) != pytest.approx(mean_of_ends)


def test_ALX1553_P410_the_short_call_answers_the_same_number_and_cannot_say_it_clipped(
    lin_fun_lib,
):
    """CHARACTERIZATION: AlxInterpLin_GetY returns only y, so a clip is indistinguishable.

    Both entry points answer the same number everywhere, including off the ends - the short one
    just cannot say WHICH it did. Two readings far apart, one a little past the top of the table
    and one a long way past it, come back as the same y through this call with nothing to
    distinguish either from a genuine reading AT the top of the table. Fine for a curve a reading
    can never leave; quietly wrong for one it can, which is most of them.
    """
    table = lin_fun_lib.table(TABLE_X, TABLE_Y, rising=True)

    for x in (-1.0e6, -1.0, 0.0, 5.0, 15.0, 21.0, 1.0e6):
        _, with_status = lin_fun_lib.table_y_status(table, x)
        assert lin_fun_lib.table_y(table, x) == pytest.approx(with_status), (
            f"the two entry points disagreed at x={x}"
        )

    assert lin_fun_lib.table_y(table, 21.0) == lin_fun_lib.table_y(table, 1.0e6)
    assert lin_fun_lib.table_y(table, -1.0) == lin_fun_lib.table_y(table, -1.0e6)


def test_ALX1553_P411_the_y_column_may_go_wherever_the_curve_goes(lin_fun_lib):
    """Only X has to be monotonic - Y may fall, sit flat, rise and turn around inside one table.

    Which is the point of a lookup rather than a line. A temperature compensation curve that sags
    in the middle and recovers is exactly what somebody measures on a bench, and the module must
    not assume the output is ordered just because the input is. It does not - the segment search
    reads X only - and that is worth holding, because a future search that used Y to bisect the
    table would be faster and silently wrong on a curve like this one.
    """
    x_points = [0.0, 1.0, 2.0, 3.0, 4.0]
    y_points = [10.0, -5.0, -5.0, 20.0, 0.0]
    table = lin_fun_lib.table(x_points, y_points, rising=True)

    assert lin_fun_lib.table_y(table, 0.5) == pytest.approx(2.5), "a falling segment"
    assert lin_fun_lib.table_y(table, 1.5) == pytest.approx(-5.0), "a flat segment"
    assert lin_fun_lib.table_y(table, 2.5) == pytest.approx(7.5), "a rising segment"
    assert lin_fun_lib.table_y(table, 3.5) == pytest.approx(10.0), "and falling again"

    assert lin_fun_lib.table_y_status(table, -1.0) == (lin_fun_lib.ERR_MIN, pytest.approx(10.0)), (
        "the low end of X, whose Y is neither the lowest nor the highest"
    )
