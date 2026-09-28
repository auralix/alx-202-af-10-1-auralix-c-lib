"""ALX-1553 - alxFtoa PC unit tests (Tier 1: a pure function, no context, no device).

`AlxFtoa(f, buf, precision)` is how a float becomes text - the number a `get-param` shows for every
float parameter on every device built on this library, and the bytes that end up in a client's JSON.

These tests were written with `alxRange`'s, in one file, because the two are the small pure things
the parameter path leans on. They are split out here so the module has the mirror file the README's
convention asks for and the mutation lane resolves, which is why the proof tokens are not
contiguous: P78 is that earlier work, moved unchanged, and P420-P424 are what a second pass added.

Two of those are sealed defects, and both are reachable from a client-visible string: a value past
2147483647 prints punctuation instead of digits, and precision 0 is the one precision that
truncates instead of rounding.

Proofs (ALX-1553): P78 and P420-P424.
"""

import pytest

pytestmark = pytest.mark.unit

LONG_MAX = 2147483647           # `long` is 32 bits on this host AND on the target
MAX_PRECISION = 10              # alxFtoa.c: the size of the rounders table


# =====================================================================
# P78 - a float becomes text
# =====================================================================


@pytest.mark.parametrize(
    ("value", "precision", "expected"),
    [
        (0.0, 2, "0.00"),
        (1.5, 2, "1.50"),
        (-1.5, 2, "-1.50"),
        (12.25, 2, "12.25"),
        (-12.25, 2, "-12.25"),
        (100.0, 0, "100"),
        (3.14159, 4, "3.1416"),
        (-3.14159, 4, "-3.1416"),
    ],
)
def test_ALX1553_P78_a_float_is_formatted_at_the_requested_precision(
    memsafe_lib, value, precision, expected
):
    """What a get-param shows for a float parameter, negatives included."""
    assert memsafe_lib.ftoa(value, precision) == expected


def test_ALX1553_P78_the_sign_survives_a_value_that_rounds_to_zero(memsafe_lib):
    """CHARACTERIZATION: a small negative at low precision.

    -0.001 at two decimals is zero to the precision asked for. Whether that prints as "0.00" or
    "-0.00" is not specified anywhere, and both are defensible; what matters is that it is stable,
    because it appears in a client-visible JSON response. Recorded here as whatever it is.
    """
    text = memsafe_lib.ftoa(-0.001, 2)
    assert text in ("0.00", "-0.00"), f"unexpected formatting {text!r}"
    assert memsafe_lib.ftoa(-0.001, 2) == text, "and it is at least stable"


# =====================================================================
# P420-P421 - two sealed defects, both visible to a client
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxFtoa takes the integer part with `long intPart = f;`, and `long` is 32 bits both "
    "on this host and on the target, so any value above 2147483647 overflows it. That conversion "
    "is undefined behaviour, and what it actually produces here is a NEGATIVE intPart, which the "
    "digit loop turns into `'0' + intPart % 10` - characters BELOW '0'. 2147483648.0 formats as "
    "'./,),(-*,('. It is not an error, not a clamp and not a rounding artefact: it is punctuation "
    "where the number should be, in the string that reaches a client's JSON. Any float parameter "
    "that can exceed 2.1 billion hits it - a millisecond timestamp, an energy counter, a "
    "frequency in hertz. The fix is a 64-bit integer part, or a range check that answers something "
    "honest; a saturating clamp would at least be a number"))
@pytest.mark.parametrize("value", [LONG_MAX + 1.0, 1e10, 1e15, -1e10])
def test_ALX1553_P420_a_large_value_formats_as_a_number(memsafe_lib, value):
    """A float bigger than a 32-bit integer still has to come out as digits."""
    text = memsafe_lib.ftoa(value, 0)

    assert all(c.isdigit() or c == "-" for c in text), f"{value} formatted as {text!r}"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: precision 0 is the one precision that TRUNCATES. The rounding is applied as "
    "`if (precision) f += rounders[precision];` - and rounders[0] is 0.5, written for exactly this "
    "case and never reached, because 0 is false. So 0.9 formats as '0' and 1.9 as '1', while 0.09 "
    "at precision 1 correctly becomes '0.1'. A whole-number reading is the one a client is most "
    "likely to see, and it is the one that is up to a full unit low. The fix is `if (precision >= "
    "0)` - or deleting the unreachable first entry of the table, if truncation is what was meant"))
@pytest.mark.parametrize(("value", "expected"), [(0.5, "1"), (0.9, "1"), (1.5, "2"), (1.9, "2")])
def test_ALX1553_P421_precision_zero_rounds_like_every_other_precision(memsafe_lib, value,
                                                                       expected):
    """At every precision but one, half rounds away from zero. This is the one."""
    assert memsafe_lib.ftoa(value, 0) == expected


# =====================================================================
# P422-P424 - what the arguments mean
# =====================================================================


@pytest.mark.parametrize(("value", "expected"), [
    (0.5, "0.500000"),
    (1.0, "1.00000"),
    (50.0, "50.0000"),
    (500.0, "500.000"),
    (5000.0, "5000.00"),
    (50000.0, "50000.0"),
    (500000.0, "500000"),
])
def test_ALX1553_P422_a_negative_precision_asks_the_function_to_choose(memsafe_lib, value,
                                                                       expected):
    """CHARACTERIZATION: a precision below zero means "pick one" - about six significant figures.

    The module says so in a comment and nothing else does. It steps the decimals down by magnitude,
    so the total width stays roughly constant and a column of such numbers lines up. Worth pinning
    because it is a SECOND meaning for the same argument: a caller that computes a precision and
    lets it go negative gets a different number of decimals rather than none.
    """
    assert memsafe_lib.ftoa(value, -1) == expected


def test_ALX1553_P423_a_precision_past_the_table_is_clamped_to_the_table(memsafe_lib):
    """The rounders table has eleven entries and the precision indexes it, so it is clamped first.

    This is the bounds check that keeps the module safe, and it is worth a test of its own: without
    the clamp a caller asking for twelve decimals would read past the end of a static array and add
    whatever followed it to the value. Checked well past the end, not just one over.
    """
    at_max = memsafe_lib.ftoa(1.23456789, MAX_PRECISION)

    for precision in (MAX_PRECISION + 1, 50, 1000):
        assert memsafe_lib.ftoa(1.23456789, precision) == at_max, (
            f"precision {precision} was not clamped to {MAX_PRECISION}"
        )

    assert at_max.count(".") == 1
    assert len(at_max.split(".")[1]) == MAX_PRECISION


def test_ALX1553_P424_the_longest_output_an_in_range_value_can_produce(memsafe_lib):
    """CHARACTERIZATION: there is no buffer length argument, so here is the number to size one by.

    `AlxFtoa(f, buf, precision)` writes as many characters as the number needs and the caller has
    nothing in the signature to tell it how many that is. For values the module handles correctly -
    magnitudes up to a 32-bit integer, precision up to the table's ten - the worst case is a sign,
    ten digits, a point and ten decimals: 22 characters and a terminator, so 23 bytes.

    Stated as a test rather than a comment because it is the only place a caller can learn it, and
    because a change that widened the integer part (which is what P420 asks for) moves this number
    and should have to say so.
    """
    worst = max(
        len(memsafe_lib.ftoa(value, precision))
        for value in (float(LONG_MAX), -float(LONG_MAX), -0.9999999999, 1234567890.123456789)
        for precision in range(MAX_PRECISION + 1)
    )

    assert worst == 22, "the worst case moved - a caller's buffer size is derived from this"
    assert memsafe_lib.ftoa(-float(LONG_MAX), MAX_PRECISION) == "-2147483647.0000000000"
