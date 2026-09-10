"""ALX-1553 - alxLinFun and alxInterpLin PC unit tests (Tier 1: x to y, pure, no device).

The library's two ways of turning a raw number into an engineering one: a straight line through two
points, and a lookup table interpolated between its points. Neither had a test, and both are what
stands between an ADC count and the number a client reads off a screen.

Two defects came out of writing these, and both are SEALED as strict xfail:

  P182  AlxLinFunInt_GetY computes a different line entirely. The slope term is written
        ``(dy + x) / dx`` where it must be ``(dy * x) / dx`` - an addition where a multiplication
        belongs - so the integer version of the module does not implement a line at all.
  P185  AlxInterpLin_GetY returns NaN when x is exactly the LAST point of the table, in both the
        rising and the falling direction, and reports Alx_Ok while doing it.

Neither is reachable from this product: nothing in the firmware calls either function, and the
integer variant's other half is commented out in the library with a TODO next to it. They are in the
public library all the same, and the first product to use them gets wrong numbers with no warning -
which is the whole reason a test suite is worth writing before someone needs the module.

Test group P179-P187 = ALX-1553 mapping proofs.
"""

import math

import pytest

pytestmark = pytest.mark.unit

# a line through (0, 0) and (100, 1000): ten units of y per unit of x
X1, Y1, X2, Y2 = 0.0, 0.0, 100.0, 1000.0
WIDE = (-1.0e6, 1.0e6)              # limits far enough away that nothing clips

# a table whose second segment is steeper than its first, so interpolation cannot be faked by a line
TABLE_X = [0.0, 10.0, 20.0]
TABLE_Y = [0.0, 100.0, 400.0]


# =====================================================================
# P179-P181 - the float line
# =====================================================================


@pytest.mark.parametrize(("x", "y"), [(0.0, 0.0), (1.0, 10.0), (50.0, 500.0), (100.0, 1000.0),
                                      (-10.0, -100.0), (250.0, 2500.0)])
def test_ALX1553_P179_the_line_passes_through_its_two_points_and_continues_past_them(
    lin_fun_lib, x, y
):
    """Both defining points, three interpolations and two extrapolations, on one line.

    The extrapolations matter: the module is a LINE, not a segment, and with the limits set wide it
    keeps going in both directions. A product that relies on that - a sensor calibrated over part of
    its range and used over all of it - is relying on behaviour nothing else states.
    """
    line = lin_fun_lib.line(X1, Y1, X2, Y2, *WIDE)
    assert lin_fun_lib.y(line, x) == pytest.approx(y)


@pytest.mark.parametrize("y", [0.0, 250.0, 500.0, 1000.0, -100.0])
def test_ALX1553_P180_getx_is_the_inverse_of_gety(lin_fun_lib, y):
    """The line read backwards: GetX of GetY is the x you started with, and the other way round."""
    line = lin_fun_lib.line(X1, Y1, X2, Y2, *WIDE)
    x = lin_fun_lib.x(line, y)
    assert lin_fun_lib.y(line, x) == pytest.approx(y)


def test_ALX1553_P181_the_two_limit_modes_clip_different_things_and_say_so(lin_fun_lib):
    """``isLimitOnX`` decides whether min and max bound the INPUT or the OUTPUT, and they differ.

    Measured on the same line with the same numbers:

        limits 100..800 on Y   x = 100  ->  800, ErrMax     the result is clipped
        limits  10..90  on X   x = 100  ->  900, ErrMax     the INPUT is clipped, then evaluated

    Both report the same status, and the values are not the same number - so a caller that reads the
    status without knowing which mode it configured learns nothing useful. Pinned in both modes at
    both ends, because one flag changing the meaning of two others is worth having written down.
    """
    on_y = lin_fun_lib.line(X1, Y1, X2, Y2, 100.0, 800.0, limit_on_x=False)
    assert lin_fun_lib.y_status(on_y, 0.0) == (lin_fun_lib.ERR_MIN, pytest.approx(100.0))
    assert lin_fun_lib.y_status(on_y, 50.0) == (lin_fun_lib.OK, pytest.approx(500.0))
    assert lin_fun_lib.y_status(on_y, 100.0) == (lin_fun_lib.ERR_MAX, pytest.approx(800.0))

    on_x = lin_fun_lib.line(X1, Y1, X2, Y2, 10.0, 90.0, limit_on_x=True)
    assert lin_fun_lib.y_status(on_x, 0.0) == (lin_fun_lib.ERR_MIN, pytest.approx(100.0))
    assert lin_fun_lib.y_status(on_x, 50.0) == (lin_fun_lib.OK, pytest.approx(500.0))
    assert lin_fun_lib.y_status(on_x, 100.0) == (lin_fun_lib.ERR_MAX, pytest.approx(900.0))


# =====================================================================
# P182 - the first defect
# =====================================================================


@pytest.mark.xfail(
    strict=True,
    reason="AlxLinFunInt: the slope term is written (dy + x) / dx instead of (dy * x) / dx, so the "
           "integer line is not a line - a line through (0,0) and (100,1000) returns 10 at x = 0",
)
@pytest.mark.parametrize(("x", "y"), [(0, 0), (50, 500), (100, 1000)])
def test_ALX1553_P182_the_integer_line_passes_through_its_own_two_points(lin_fun_lib, x, y):
    """SEALED DEFECT - AlxLinFunInt_GetY computes a different line entirely.

    ``Get_Y_Int`` reads:

        int32_t kx = (dy + x) / dx;
        int32_t y  = kx + n;

    The slope term of a line through two points is ``(dy / dx) * x``, which in integers has to be
    written ``(dy * x) / dx``. What is there is an ADDITION. The result does not depend on x in any
    meaningful way: measured on a line through (0, 0) and (100, 1000), which should be y = 10x,

        x =   0  ->   10      (0 expected)
        x =  50  ->   10      (500 expected)
        x = 100  ->   11      (1000 expected)

    The float version of the same module is correct, and this is a separate implementation rather
    than a wrapper around it, which is how the two came to differ.

    Nothing in this product calls it - and the module's own ``Get_X_Int`` is commented out with a
    TODO beside it, so the integer half looks unfinished rather than broken-in-service. It is in the
    public library all the same. Strict xfail: this XPASSes the day the expression is corrected, and
    the marker comes off in that commit.
    """
    line = lin_fun_lib.line_int(0, 0, 100, 1000, -1_000_000, 1_000_000)
    assert lin_fun_lib.y_int(line, x) == y


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
# P185-P186 - the second defect, and the asymmetry that hides it
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


def test_ALX1553_P187_a_two_point_table_is_a_line_between_them(lin_fun_lib):
    """The smallest table there is, checked against the line module over the same two points.

    Two implementations of the same idea, agreeing - which is worth one test, because the day they
    stop agreeing one of them has changed and neither file mentions the other.
    """
    table = lin_fun_lib.table([X1, X2], [Y1, Y2], rising=True)
    line = lin_fun_lib.line(X1, Y1, X2, Y2, *WIDE)
    for x in (0.0, 1.0, 33.0, 99.0):
        assert lin_fun_lib.table_y(table, x) == pytest.approx(lin_fun_lib.y(line, x))
