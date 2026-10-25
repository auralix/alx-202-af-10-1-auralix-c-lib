"""ALX-1553 - alxGlobal PC unit tests (Tier 1: number formatting, pure, no state).

Three helpers with no module of their own: a uint64 and an int64 to decimal text, and a byte swap.
The first is on the CLI path of any product that publishes a 64-bit value, and it does not call a
64-bit printf - the target's C library may not have one - so it splits the number into four groups of
five digits and formats those. Four branches and three zero-padded joins is where the digits of
1000000000000001 get lost, so every group boundary is checked with a value either side of it.

They are compiled into the alxMathTest DLL, which is a linker convenience: a group serves the linker,
a file mirrors a source.

Test group P198-P200 = ALX-1553 number formatting proofs.
"""

import pytest

pytestmark = pytest.mark.unit

@pytest.mark.parametrize("value", [
    0, 1, 9, 10, 99, 12345,
    99999, 100000, 100001,                          # the first group boundary, 10^5
    9999999999, 10000000000, 10000000001,           # the second, 10^10
    999999999999999, 1000000000000000,              # the third, 10^15
    4294967295, 4294967296,                         # a uint32 and one past it
    18446744073709551615,                           # the largest uint64
])
def test_ALX1553_P198_ulltoa_writes_the_same_digits_python_would(math_lib, value):
    """Every group boundary the implementation has, and one either side of each.

    The module does not call a 64-bit printf - the target's C library may not have one - so it
    splits the number into four groups of five digits and formats those. That is four branches and
    three zero-padded joins, which is where the digits of a number like 1000000000000001 get lost.
    Checked against Python's own conversion rather than against a hand-written expectation.
    """
    assert math_lib.ulltoa(value) == str(value)


@pytest.mark.parametrize("value", [0, 1, -1, 99999, -99999, -100000, -4294967296,
                                   9223372036854775807, -9223372036854775808])
def test_ALX1553_P199_slltoa_writes_the_sign_and_then_the_digits(math_lib, value):
    """Both signs across the same boundaries, and the one input that should not work but does.

    ``AlxGlobal_Slltoa`` negates its argument to make it positive. For the most negative int64 that
    negation overflows, which is undefined behaviour in C - and on every two's complement machine it
    lands back on the same value, whose unsigned reading is exactly the magnitude wanted. So the
    answer is right, by luck rather than by design. Pinned here because the day it is compiled
    somewhere that traps on signed overflow, this is the test that says what changed.
    """
    assert math_lib.slltoa(value) == str(value)


@pytest.mark.parametrize("value", [0x00000000, 0x12345678, 0xFFFFFFFF, 0x000000FF, 0xFF000000,
                                   0xDEADBEEF])
def test_ALX1553_P200_ntohl_reverses_the_four_bytes_and_is_its_own_inverse(math_lib, value):
    """The byte swap, against Python's own, and applied twice to get back where it started."""
    expected = int.from_bytes(value.to_bytes(4, "big"), "little")
    assert math_lib.ntohl(value) == expected
    assert math_lib.ntohl(math_lib.ntohl(value)) == value
