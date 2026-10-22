"""Auralix C Library - AlxVnh7040 on the host.

A full H-bridge motor driver: two direction inputs (INA, INB), a PWM speed input, and one
multisense output that the two select pins (SEL0, SEL1) switch between the two high-side currents,
the chip temperature and the supply voltage. A duty in per mille is a direction by its sign and a
speed by its size; zero is a brake, of the type the product configured. Once a millisecond the
driver reads the multisense for the selected quantity and selects the next: high side A, high
side B, temperature, supply, and round again. A multisense pin above 5 V is the part's fault flag.

The helper's divider is 1 kOhm over 1 kOhm: the pin is twice the ADC voltage, and the sense
current in uA is the ADC's millivolts; the part's current ratio is 4450.

Writing these found one defect, sealed as P659: the first high-side A reading is made with the
select pins as Init left them, which select high side B.

Proofs (ALX-1564, A92): P652-P660.
"""

import pytest

RATIO = 4450


@pytest.fixture
def bridge(vnh7040_lib):
    """One bridge braking to ground by default, initialised."""
    return vnh7040_lib, vnh7040_lib.new("gnd")


def test_ALX1564_P652_init_claims_the_pins_enables_the_multisense_and_waits_for_the_part(vnh7040_lib):
    """Five pins initialised, SENS_EN high, then the 2 ms the part needs to leave standby."""
    obj = vnh7040_lib.new()

    assert vnh7040_lib.pin_counts(obj) == dict.fromkeys(vnh7040_lib.PINS, (1, 0))
    assert vnh7040_lib.level(obj, "sens_en") is True
    assert vnh7040_lib.delays_ms() == 1

    vnh7040_lib.deinit(obj)
    assert vnh7040_lib.pin_counts(obj) == dict.fromkeys(vnh7040_lib.PINS, (1, 1))


@pytest.mark.parametrize(("permil", "direction", "pct"), [
    (1000, (True, False), 100.0),
    (250, (True, False), 25.0),
    (-1, (False, True), 0.1),
    (-750, (False, True), 75.0),
])
def test_ALX1564_P653_the_sign_is_the_direction_and_the_size_is_the_speed(bridge, permil, direction, pct):
    """Positive is INA high and INB low (clockwise), negative the other way; the PWM is the size."""
    lib, obj = bridge
    lib.sense(obj, 0.0)
    lib.duty(obj, permil)

    lib.handle(obj)

    assert lib.direction(obj) == direction
    assert lib.pwm_pct(obj) == pytest.approx(pct)


@pytest.mark.parametrize(("break_type", "direction", "pct"), [
    ("gnd", (False, False), 100.0),     # both low sides on: the motor shorted to ground
    ("vcc", (True, True), 0.0),         # both high sides on: shorted to the supply
    ("open", (False, False), 0.0),      # everything off: the motor coasts
])
def test_ALX1564_P654_a_zero_duty_brakes_the_configured_way(vnh7040_lib, break_type, direction, pct):
    """A brake is a direction pattern and a PWM, and the current reads zero while braking."""
    obj = vnh7040_lib.new(break_type)
    vnh7040_lib.sense(obj, 0.5)
    vnh7040_lib.duty(obj, 0)

    vnh7040_lib.handle(obj)

    assert vnh7040_lib.direction(obj) == direction
    assert vnh7040_lib.pwm_pct(obj) == pytest.approx(pct)
    assert vnh7040_lib.read(obj, "Current_mA") == 0


def test_ALX1564_P655_a_brake_type_configured_later_is_the_next_brake(bridge):
    """Config_BreakType changes what the next zero duty does, not the duty already set."""
    lib, obj = bridge
    lib.sense(obj, 0.0)
    lib.config_break(obj, "vcc")

    lib.duty(obj, 0)
    lib.handle(obj)

    assert lib.direction(obj) == (True, True)


@pytest.mark.parametrize(("permil", "reading"), [(500, "HsA"), (-500, "HsB")])
def test_ALX1564_P656_rotating_the_current_is_the_active_high_sides_sense_times_4450(bridge, permil, reading):
    """Clockwise drives through high side A, counter-clockwise through B: that side is the current.

    0.5 V and 0.25 V at the ADC are 500 uA and 250 uA of sense current: 2225 mA and 1112 mA.
    """
    lib, obj = bridge
    lib.sense(obj, [0.5, 0.25])
    lib.duty(obj, permil)

    lib.handle(obj, periods=2)

    currents = (lib.read(obj, "CurrentHsA_mA"), lib.read(obj, "CurrentHsB_mA"))
    assert currents == (500 * RATIO // 1000, 250 * RATIO // 1000)
    expected = 500 * RATIO // 1000 if reading == "HsA" else 250 * RATIO // 1000
    assert lib.read(obj, "Current_mA") == expected


def test_ALX1564_P657_a_multisense_above_5_v_is_the_parts_fault_flag(bridge):
    """3 V at the ADC is 6 V on the pin: the side's current reads zero and the bridge is in error."""
    lib, obj = bridge
    lib.sense(obj, [3.0, 0.25])
    lib.duty(obj, 500)

    lib.handle(obj)

    assert lib.read(obj, "CurrentHsA_mA") == 0
    assert lib.read(obj, "IsError") == 1


def test_ALX1564_P658_the_multisense_cycles_through_both_currents_temperature_and_supply(bridge):
    """One reading a millisecond, each for the quantity selected before it, then the next selected.

    1 V at the ADC on the temperature step is 2 V on the pin, 38 degC by the part's -5.5 mV/K
    from 2214 mV; 3 V on the supply step is 6 V on the pin, a 24 V supply by the part's ratio of 4.
    """
    lib, obj = bridge
    lib.sense(obj, [0.5, 0.25, 1.0, 3.0])
    lib.duty(obj, 500)

    lib.handle(obj, periods=4)

    assert lib.sense_reads(obj) == 4
    assert lib.read(obj, "CurrentHsA_mA") == 2225
    assert lib.read(obj, "CurrentHsB_mA") == 1112
    assert lib.read(obj, "Temp_degC") == 38
    assert lib.read(obj, "SupplyVoltage_mV") == 24000
    assert (lib.level(obj, "sel0"), lib.level(obj, "sel1")) == (True, False), "the cycle did not come back to A"


def test_ALX1564_P659_init_selects_high_side_a_before_the_first_reading(bridge):
    """SEL0 high and SEL1 low is high side A, the quantity the first reading is filed under."""
    lib, obj = bridge

    assert (lib.level(obj, "sel0"), lib.level(obj, "sel1")) == (True, False)


@pytest.mark.expect_assert("AlxVnh7040_Handle asserts on the status Init leaves until a duty is set")
def test_ALX1564_P660_handle_before_any_duty_asserts(bridge):
    """CHARACTERIZATION: Init leaves the status at Init, which Handle's status chain does not accept.

    So a product must set a duty - a brake, at least - before its first Handle; a Handle first
    trips the module's assertion, and with the asserts off it runs on with no current reported.
    """
    lib, obj = bridge
    lib.sense(obj, 0.0)

    lib.handle(obj)

    assert lib.read(obj, "Current_mA") == 0


# =====================================================================
# P760-P763 - the PWM that fails, high side B's fault flag, no time passed, a brake outside the enumeration (A97)
# =====================================================================


@pytest.mark.expect_assert("P760: a PWM that will not initialise asserts in Init, and one that will not release in DeInit")
def test_ALX1564_P760_a_pwm_that_fails_ends_init_before_the_multisense_and_deinit_before_the_pins(vnh7040_lib):
    """CHARACTERIZATION: Init returns after the PWM's error - the multisense is not enabled and the part is not
    waited for; DeInit returns before the five pins are released."""
    lib = vnh7040_lib
    lib.pwm_answers(1)
    obj = lib.new("gnd")

    assert lib.level(obj, "sens_en") is False
    assert lib.delays_ms() == 0

    lib.pwm_answers(0)
    lib.init(obj)
    assert lib.level(obj, "sens_en") is True
    lib.pwm_answers(1)
    lib.deinit(obj)
    assert lib.pin_counts(obj) == dict.fromkeys(lib.PINS, (2, 0))


def test_ALX1564_P761_a_multisense_above_5_v_on_high_side_b_is_its_fault_flag(bridge):
    """The second step of the cycle: B's current reads zero and the bridge is in error for B alone."""
    lib, obj = bridge
    lib.sense(obj, [0.25, 3.0])
    lib.duty(obj, 500)

    lib.handle(obj)
    assert lib.read(obj, "IsError") == 0, "A is operational"
    lib.handle(obj)

    assert lib.read(obj, "CurrentHsA_mA") == 250 * RATIO // 1000
    assert lib.read(obj, "CurrentHsB_mA") == 0
    assert lib.read(obj, "IsError") == 1


def test_ALX1564_P762_a_handle_before_the_monitoring_period_is_over_reads_nothing(bridge):
    """One reading a millisecond: a Handle with no time passed makes no reading and selects nothing."""
    lib, obj = bridge
    lib.sense(obj, 0.25)
    lib.duty(obj, 500)
    lib.handle(obj)
    assert lib.sense_reads(obj) == 1

    selected = (lib.level(obj, "sel0"), lib.level(obj, "sel1"))
    lib.handle_now(obj)

    assert lib.sense_reads(obj) == 1
    assert (lib.level(obj, "sel0"), lib.level(obj, "sel1")) == selected, "the selection moved on without a reading"


@pytest.mark.expect_assert("P763: a break type outside the enumeration reaches the default of the brake switch")
def test_ALX1564_P763_a_brake_of_a_type_the_enumeration_does_not_have_leaves_the_pins_as_they_were(bridge):
    """CHARACTERIZATION: the switch's default asserts and sets nothing - the next Handle drives the last duty."""
    lib, obj = bridge
    lib.sense(obj, 0.25)
    lib.duty(obj, 500)
    lib.handle(obj)
    assert lib.direction(obj) == (True, False)
    lib.config_break_raw(obj, 99)

    lib.duty(obj, 0)
    lib.handle(obj)

    assert lib.direction(obj) == (True, False)
