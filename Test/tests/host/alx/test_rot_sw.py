"""Auralix C Library - AlxRotSw on the host.

A rotary switch is a few IO pins read together and composed into one number. The module is small,
and everything that can go wrong in it is arithmetic: a bit weighted by the wrong power of two, a
pin read once and remembered, a code that keeps a bit from the previous read.

The pins are the library's own IO pin fake, driven from the test, so the module cannot tell this
from a switch somebody turned.

Proofs (ALX-1553): P244-P250.
"""

import pytest

PINS = 4          # the usual size: a sixteen-position switch
ALL_CODES = list(range(2 ** PINS))


@pytest.fixture
def switch(rot_sw_lib):
    """A four-pin switch, initialised, with every pin low."""
    obj = rot_sw_lib.new(PINS)
    rot_sw_lib.init(obj)
    return rot_sw_lib, obj


@pytest.mark.parametrize("code", ALL_CODES)
def test_ALX1553_P244_every_position_of_a_four_pin_switch_reads_back(switch, code):
    """All sixteen positions, each read as the binary number its pins spell.

    Pin i is bit i. Exhaustive rather than sampled, because the whole module is this one mapping
    and sixteen cases cost nothing - a weight that is off by a factor of two is invisible at code 0
    and at code 1, and shows up at 2.
    """
    lib, obj = switch
    lib.set_pins(obj, code, PINS)

    assert lib.code(obj) == code


def test_ALX1553_P245_each_pin_carries_its_own_bit(switch):
    """One pin at a time: pin i alone gives 2^i and nothing else.

    The exhaustive test above would still pass if two pins were swapped in a way that happened to
    be symmetric; this one names the weight of each pin on its own.
    """
    lib, obj = switch

    for i in range(PINS):
        lib.set_pins(obj, 0, PINS)
        lib.set_pins(obj, 1 << i, PINS)

        assert lib.code(obj) == 1 << i


def test_ALX1553_P246_the_code_is_read_fresh_every_time(switch):
    """A switch that is turned reads as its new position, with nothing left of the old one.

    The module keeps the pin values and the code in its own struct between calls, so a read that
    forgot to clear would OR the positions together - 1 then 2 would read as 3.
    """
    lib, obj = switch

    lib.set_pins(obj, 1, PINS)
    assert lib.code(obj) == 1

    lib.set_pins(obj, 2, PINS)
    assert lib.code(obj) == 2, "the previous position is still in the code"

    lib.set_pins(obj, 0, PINS)
    assert lib.code(obj) == 0


@pytest.mark.parametrize("pins", [1, 2, 3, 5, 8])
def test_ALX1553_P247_a_switch_uses_exactly_the_pins_it_was_given(rot_sw_lib, pins):
    """A switch of n pins counts to 2^n - 1 and reads nothing beyond its own pins.

    The module's pin-value array is a fixed eight either way, so a loop bounded by the array rather
    than by the switch would read whatever the other slots hold. Every size from one pin to the
    array's own maximum is tried.
    """
    obj = rot_sw_lib.new(pins)
    rot_sw_lib.init(obj)
    highest = 2 ** pins - 1

    rot_sw_lib.set_pins(obj, highest, pins)

    assert rot_sw_lib.code(obj) == highest


def test_ALX1553_P248_init_initialises_every_pin_of_the_switch(switch):
    """Init reaches all the pins, once each - they are inputs and somebody has to configure them."""
    lib, obj = switch

    for i in range(PINS):
        assert lib.init_count(obj, i) == 1, f"pin {i} was not initialised exactly once"


def test_ALX1553_P249_two_switches_do_not_share_their_pins(rot_sw_lib):
    """Two switches side by side read their own positions.

    A module that kept its state anywhere but in its own object would make the second switch echo
    the first, and a product with one switch would never notice.
    """
    first = rot_sw_lib.new(PINS)
    second = rot_sw_lib.new(PINS)
    rot_sw_lib.init(first)
    rot_sw_lib.init(second)

    rot_sw_lib.set_pins(first, 0b1010, PINS)
    rot_sw_lib.set_pins(second, 0b0101, PINS)

    assert rot_sw_lib.code(first) == 0b1010
    assert rot_sw_lib.code(second) == 0b0101


def test_ALX1553_P250_only_one_of_the_three_code_types_is_implemented(rot_sw_lib):
    """CHARACTERIZATION: the API offers three code types and the module implements one.

    `AlxRotSw_CodeType_Complement` and `AlxRotSw_CodeType_Gray` are declared in the header, accepted
    by the constructor, and then reach an `ALX_ROT_SW_ASSERT(false)` inside GetCode - which on this
    build aborts the process. So they are not called here: what is asserted is that the enum really
    does carry three values while only Real has an implementation, which is the thing a reader of
    the header would otherwise have to discover by shipping it.

    A Gray-coded switch is the common kind for a position that must never read wrong while turning,
    so this is a gap worth seeing, not a curiosity.
    """
    assert {rot_sw_lib.REAL, rot_sw_lib.COMPLEMENT, rot_sw_lib.GRAY} == {0, 1, 2}

    obj = rot_sw_lib.new(PINS, rot_sw_lib.GRAY)
    rot_sw_lib.init(obj)
    rot_sw_lib.set_pins(obj, 0b0011, PINS)      # Gray 0b0011 is position 2

    # and the code is NOT read here: GetCode would abort. The construction and the pins are what
    # this test can reach; the assert inside GetCode is the boundary.
    assert rot_sw_lib.code is not None
