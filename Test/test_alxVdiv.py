"""ALX-1553 - alxVdiv PC unit tests (Tier 1: a resistive divider's arithmetic, no state, no device).

Nine context-free functions, and nothing tested them. They are worth testing because a product built
on this library reads its analog inputs through them: every voltage, every current shunt and every
resistance measurement passes through this file, so an error here is an error in a number a client
reads off a screen.

Two families with the same algebra in different units - float volts and kOhm, integer millivolts and
ohm - plus the current a shunt drops. The four float functions are two pairs of inverses, which is
what most of these tests exercise: convert one way, convert back, land where you started.

Test group P79-P83 = ALX-1553 divider proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# a 10:1 divider, the shape a product uses to read a supply rail
HIGH_KOHM, LOW_KOHM = 90.0, 10.0


# =====================================================================
# P79 - the float family
# =====================================================================


def test_ALX1553_P79_the_divider_divides(vdiv_lib):
    """10 V across 90k + 10k puts 1 V across the low leg."""
    assert vdiv_lib.vout_v(10.0, HIGH_KOHM, LOW_KOHM) == pytest.approx(1.0)
    assert vdiv_lib.vout_v(0.0, HIGH_KOHM, LOW_KOHM) == pytest.approx(0.0)
    assert vdiv_lib.vout_v(-10.0, HIGH_KOHM, LOW_KOHM) == pytest.approx(-1.0), "sign is carried"


def test_ALX1553_P79_equal_legs_halve_the_input(vdiv_lib):
    """The case a product uses to read a 3V3 or 5V rail: two identical resistors."""
    for volts in (3.3, 5.0, 24.0):
        assert vdiv_lib.vout_v(volts, 6.8, 6.8) == pytest.approx(volts / 2, rel=1e-6)
        assert vdiv_lib.vin_v(volts / 2, 6.8, 6.8) == pytest.approx(volts, rel=1e-6)


@pytest.mark.parametrize("vin", [0.5, 3.3, 5.0, 12.0, 24.0, 32.0])
def test_ALX1553_P79_vin_and_vout_are_inverses(vdiv_lib, vin):
    vout = vdiv_lib.vout_v(vin, HIGH_KOHM, LOW_KOHM)
    assert vdiv_lib.vin_v(vout, HIGH_KOHM, LOW_KOHM) == pytest.approx(vin, rel=1e-5)


@pytest.mark.parametrize(("high", "low"), [(90.0, 10.0), (18.0, 6.8), (100.0, 10.0), (6.8, 6.8)])
def test_ALX1553_P79_the_resistance_functions_recover_the_divider(vdiv_lib, high, low):
    """Given the two voltages, each function recovers the resistor the other one is not."""
    vin = 5.0
    vout = vdiv_lib.vout_v(vin, high, low)
    assert vdiv_lib.res_high_kohm(vin, vout, low) == pytest.approx(high, rel=1e-4)
    assert vdiv_lib.res_low_kohm(vin, vout, high) == pytest.approx(low, rel=1e-4)


# =====================================================================
# P80 - the scale question behind a resistance reading
# =====================================================================


def test_ALX1553_P80_res_low_is_invariant_to_the_scale_of_both_voltages(vdiv_lib):
    """CHARACTERIZATION, and it settles a diagnosis: scaling BOTH voltages changes nothing.

    AlxVdiv_GetResLow_kOhm is resHigh * vout / (vin - vout). Multiply vin and vout by the same
    number and it cancels top and bottom, so the answer is identical.

    That matters because a product's resistance path passes both voltages divided by 1000, which
    looks like a unit error and has been written down as the likely cause of a resistance-reading
    complaint. It cannot be: the division cancels. Whatever is wrong with that reading is somewhere
    else, and this test exists so nobody spends a day removing a /1000 and expecting the
    measurement to change.

    Checked across four decades, which also shows the float has precision to spare at the small end.
    """
    high_kohm = 3.4                      # a 6.8k leg pair, halved: the shape the product uses
    reference = vdiv_lib.res_low_kohm(5.0, 2.0, high_kohm)
    assert reference == pytest.approx(3.4 * 2.0 / 3.0, rel=1e-5)
    for scale in (1e-1, 1e-2, 1e-3, 1e-4):
        scaled = vdiv_lib.res_low_kohm(5.0 * scale, 2.0 * scale, high_kohm)
        assert scaled == pytest.approx(reference, rel=1e-4), (
            f"scaling both voltages by {scale} changed the resistance"
        )


def test_ALX1553_P80_res_high_is_invariant_the_same_way(vdiv_lib):
    """The other resistance function has the same algebra, so it has the same property."""
    reference = vdiv_lib.res_high_kohm(5.0, 1.0, 10.0)
    for scale in (1e-1, 1e-3):
        assert vdiv_lib.res_high_kohm(5.0 * scale, 1.0 * scale, 10.0) == pytest.approx(
            reference, rel=1e-4
        )


# =====================================================================
# P81 - the integer family
# =====================================================================


def test_ALX1553_P81_the_integer_divider_matches_the_float_one(vdiv_lib):
    """Same algebra, millivolts and ohms."""
    assert vdiv_lib.vout_mv(10000, 90000, 10000) == 1000
    assert vdiv_lib.vin_mv(1000, 90000, 10000) == 10000
    assert vdiv_lib.vout_mv(3300, 6800, 6800) == 1650
    assert vdiv_lib.vin_mv(1650, 6800, 6800) == 3300


def test_ALX1553_P81_the_integer_resistance_functions_recover_the_divider(vdiv_lib):
    assert vdiv_lib.res_high_ohm(10000, 1000, 10000) == 90000
    assert vdiv_lib.res_low_ohm(10000, 1000, 90000) == 10000


def test_ALX1553_P81_a_shunt_current_is_ohms_law(vdiv_lib):
    """120 ohm with 12000 uV across it carries 100 uA."""
    assert vdiv_lib.current_ua(12000, 120) == 100
    assert vdiv_lib.current_ua(0, 120) == 0
    assert vdiv_lib.current_ua(2400000, 120) == 20000, "20 mA, the top of a 4-20 mA loop"


# =====================================================================
# P82 - CHARACTERIZATION: what integer arithmetic costs
# =====================================================================


def test_ALX1553_P82_the_integer_family_truncates_rather_than_rounds(vdiv_lib):
    """Nobody specified rounding, and the integer path truncates. Worth knowing before trusting it.

    3300 mV across two equal 6801 ohm legs is 1650 mV exactly; across 6800 and 6801 it is not, and
    the answer comes back floored rather than rounded to nearest. The float family has no such
    limit, which is a reason to prefer it where a fraction of a millivolt matters.
    """
    exact = vdiv_lib.vout_mv(3300, 6800, 6800)
    assert exact == 1650
    uneven = vdiv_lib.vout_mv(3300, 6800, 6801)
    assert uneven == 1650, "floored, not rounded up from 1650.12"
    assert vdiv_lib.vout_mv(1, 1, 1) == 0, "0.5 mV floors to 0"


# =====================================================================
# P83 - the edges
# =====================================================================


def test_ALX1553_P83_a_zero_input_gives_zero_out(vdiv_lib):
    assert vdiv_lib.vout_v(0.0, HIGH_KOHM, LOW_KOHM) == pytest.approx(0.0)
    assert vdiv_lib.vout_mv(0, 90000, 10000) == 0
    assert vdiv_lib.vin_v(0.0, HIGH_KOHM, LOW_KOHM) == pytest.approx(0.0)
    assert vdiv_lib.vin_mv(0, 90000, 10000) == 0


def test_ALX1553_P83_a_short_across_the_high_leg_passes_the_input_through(vdiv_lib):
    """resHigh 0 means vout == vin, which is the degenerate divider and must not divide by zero."""
    assert vdiv_lib.vout_v(5.0, 0.0, LOW_KOHM) == pytest.approx(5.0)
    assert vdiv_lib.vin_v(5.0, 0.0, LOW_KOHM) == pytest.approx(5.0)
