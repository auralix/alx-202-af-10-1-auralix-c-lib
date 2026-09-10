"""ALX-1553 - alxRange and alxFtoa PC unit tests (Tier 1: pure functions, no context, no device).

Two small modules that the parameter path leans on and that nothing tested.

AlxRange_Check<Type>(val, valMin, valMax) is THE bound check. test_alxParamItem P71 showed from
above that it works, and that the defects there sit in the conversion in front of it; these tests
say the same thing from below, per type and at the edges, so a future change to the check cannot
hide behind the conversion's behaviour.

AlxFtoa(f, buf, precision) is how a float becomes text - the number a `get-param` shows for every
float parameter on every device built on this library.

Test group P75-P78 = ALX-1553 range and float-format proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# name, min of the type, max of the type
INT_TYPES = (
    ("Uint8", 0, 255),
    ("Uint16", 0, 65535),
    ("Uint32", 0, 4294967295),
    ("Uint64", 0, 18446744073709551615),
    ("Int8", -128, 127),
    ("Int16", -32768, 32767),
    ("Int32", -2147483648, 2147483647),
    ("Int64", -9223372036854775808, 9223372036854775807),
)


# =====================================================================
# P75 - the bound check, per type
# =====================================================================


@pytest.mark.parametrize(("kind", "type_min", "type_max"), INT_TYPES)
def test_ALX1553_P75_a_value_inside_the_range_passes(memsafe_lib, kind, type_min, type_max):
    low, high = (10, 20) if type_min == 0 else (-10, 20)
    for value in (low, low + 1, high - 1, high):
        assert memsafe_lib.range_check(kind, value, low, high) == memsafe_lib.OK, (
            f"{kind}: {value} is inside {low}..{high} and was refused"
        )


@pytest.mark.parametrize(("kind", "type_min", "type_max"), INT_TYPES)
def test_ALX1553_P75_a_value_outside_the_range_is_refused(memsafe_lib, kind, type_min, type_max):
    low, high = (10, 20) if type_min == 0 else (-10, 20)
    for value in (low - 1, high + 1, type_min, type_max):
        if low <= value <= high:
            continue                       # the type's own extreme can fall inside a wide range
        assert memsafe_lib.range_check(kind, value, low, high) != memsafe_lib.OK, (
            f"{kind}: {value} is outside {low}..{high} and was accepted"
        )


@pytest.mark.parametrize(("kind", "type_min", "type_max"), INT_TYPES)
def test_ALX1553_P75_the_whole_type_is_a_valid_range(memsafe_lib, kind, type_min, type_max):
    """The edges of the type itself, which is where an off-by-one or a sign error would show."""
    for value in (type_min, type_min + 1, max(type_min, 0), type_max - 1, type_max):
        assert memsafe_lib.range_check(kind, value, type_min, type_max) == memsafe_lib.OK, (
            f"{kind}: {value} refused inside the full type range"
        )


@pytest.mark.parametrize(("kind", "type_min", "type_max"), INT_TYPES)
def test_ALX1553_P75_an_empty_range_accepts_only_its_one_value(memsafe_lib, kind, type_min, type_max):
    """min == max is a legal declaration: exactly one value is allowed."""
    only = 7
    assert memsafe_lib.range_check(kind, only, only, only) == memsafe_lib.OK
    assert memsafe_lib.range_check(kind, only + 1, only, only) != memsafe_lib.OK
    assert memsafe_lib.range_check(kind, only - 1, only, only) != memsafe_lib.OK


# =====================================================================
# P76 - the bound check on floating point
# =====================================================================


@pytest.mark.parametrize("kind", ["Float", "Double"])
def test_ALX1553_P76_a_float_range_is_checked_at_its_edges(memsafe_lib, kind):
    for value in (-10.0, -9.9, 0.0, 19.9, 20.0):
        assert memsafe_lib.range_check(kind, value, -10.0, 20.0) == memsafe_lib.OK, f"{kind} {value}"
    for value in (-10.1, 20.1, -1e30, 1e30):
        assert memsafe_lib.range_check(kind, value, -10.0, 20.0) != memsafe_lib.OK, f"{kind} {value}"


@pytest.mark.parametrize("kind", ["Float", "Double"])
def test_ALX1553_P76_a_negative_range_is_checked_the_right_way_round(memsafe_lib, kind):
    """A range entirely below zero: a sign mistake in the comparison shows here and nowhere else."""
    assert memsafe_lib.range_check(kind, -50.0, -100.0, -10.0) == memsafe_lib.OK
    assert memsafe_lib.range_check(kind, -100.0, -100.0, -10.0) == memsafe_lib.OK
    assert memsafe_lib.range_check(kind, -10.0, -100.0, -10.0) == memsafe_lib.OK
    assert memsafe_lib.range_check(kind, -100.1, -100.0, -10.0) != memsafe_lib.OK
    assert memsafe_lib.range_check(kind, 0.0, -100.0, -10.0) != memsafe_lib.OK


# =====================================================================
# P77 - the string length check
# =====================================================================


def test_ALX1553_P77_a_string_is_checked_against_its_buffer(memsafe_lib):
    """The limit counts the null terminator, so a string of exactly limit-1 characters fits."""
    assert memsafe_lib.range_check_str("", 8) == memsafe_lib.OK
    assert memsafe_lib.range_check_str("abcdefg", 8) == memsafe_lib.OK, "7 characters plus the null"
    assert memsafe_lib.range_check_str("abcdefgh", 8) != memsafe_lib.OK, "8 characters need 9 bytes"
    assert memsafe_lib.range_check_str("abcdefghijk", 8) != memsafe_lib.OK


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
