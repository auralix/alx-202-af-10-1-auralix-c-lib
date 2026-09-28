"""ALX-1553 - alxLinFun PC unit tests (Tier 1: x to y, pure, no device).

A straight line through two given points: what stands between an ADC count and the number a client
reads off a screen, for every reading that is not a table.

One defect came out of writing these, and it is SEALED as a strict xfail:

  P182  AlxLinFunInt_GetY computes a different line entirely. The slope term is written
        ``(dy + x) / dx`` where it must be ``(dy * x) / dx`` - an addition where a multiplication
        belongs - so the integer version of the module does not implement a line at all.

It is not reachable from this product: nothing in the firmware calls the function, and the integer
variant's other half is commented out in the library with a TODO next to it. It is in the public
library all the same, and the first product to use it gets wrong numbers with no warning - which is
the whole reason a test suite is worth writing before someone needs the module.

`alxInterpLin` was tested here too, the two modules being the library's two ways of doing the same
job over one DLL. Those tests now live in `test_alxInterpLin.py`, so each module has the mirror file
the README asks for; they moved unchanged, tokens and all.

Test group P179-P182 = ALX-1553 mapping proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# a line through (0, 0) and (100, 1000): ten units of y per unit of x
X1, Y1, X2, Y2 = 0.0, 0.0, 100.0, 1000.0
WIDE = (-1.0e6, 1.0e6)              # limits far enough away that nothing clips


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
