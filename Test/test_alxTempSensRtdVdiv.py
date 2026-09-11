"""Auralix C Library - AlxTempSensRtdVdiv on the host.

An RTD is a resistor whose value is its temperature. The module reads two converter channels - the
voltage across the divider and the voltage at its middle - works the RTD's resistance out of them,
and looks the temperature up in a table. Three steps, all arithmetic, and a bench can vary none of
them without a calibrated source and a box of resistors.

The table below is a Pt1000 in kilohms, thinned to the points a test needs: 1.000 kOhm at 0 degC,
and roughly 3.9 ohms per degree above it.

Proofs (ALX-1553): P251-P258.
"""

import pytest

# A Pt1000, thinned. Rising in resistance, which is what the table is told.
PT1000_KOHM = [0.803, 0.882, 0.961, 1.000, 1.039, 1.117, 1.194, 1.271, 1.347, 1.423]
PT1000_DEGC = [-50.0, -30.0, -10.0, 0.0, 10.0, 30.0, 50.0, 70.0, 90.0, 110.0]

CH_VIN = 0
CH_VOUT = 1
RES_OTHER_KOHM = 1.0    # the fixed half of the divider


@pytest.fixture
def sensor(temp_sens_lib):
    """An RTD on the LOW side of a 1 kOhm divider, reading channels 0 and 1."""
    lib = temp_sens_lib
    return lib, lib.new(PT1000_KOHM, PT1000_DEGC, ch_vin=CH_VIN, ch_vout=CH_VOUT,
                        rtd_low=True, res_other_kOhm=RES_OTHER_KOHM)


def _vout_for_low_side(
    vin_V: float,  # noqa: N803 - the unit belongs in the name
    rtd_kOhm: float,  # noqa: N803
    other_kOhm: float = RES_OTHER_KOHM,  # noqa: N803
) -> float:
    """The middle voltage a divider produces with the RTD in its low half."""
    return vin_V * rtd_kOhm / (other_kOhm + rtd_kOhm)


@pytest.mark.parametrize(("rtd_kOhm", "expected_degC"), [
    (1.000, 0.0),
    (1.039, 10.0),
    (0.961, -10.0),
    (1.347, 90.0),
])
def test_ALX1553_P251_a_resistance_becomes_the_temperature_the_table_gives_it(
    sensor,
    rtd_kOhm,  # noqa: N803 - the unit belongs in the name
    expected_degC,  # noqa: N803
):
    """A divider voltage that means a known resistance reads as that resistance's temperature.

    The voltage is computed here from the divider the sensor was built with, so the test states the
    physics and the module does the arithmetic - not the other way round.

    The table's own FIRST and LAST points are left out on purpose. A resistance rebuilt from a
    voltage lands a fraction either side of the value that produced it, and at the ends of the
    table that fraction is the difference between a reading and an out-of-range status - which is
    P185's subject, not this one.
    """
    lib, obj = sensor
    lib.set_voltage_V(obj, CH_VIN, 3.3)
    lib.set_voltage_V(obj, CH_VOUT, _vout_for_low_side(3.3, rtd_kOhm))

    status, temp = lib.temp(obj)

    assert status == lib.OK
    assert temp == pytest.approx(expected_degC, abs=0.1)


def test_ALX1553_P252_a_resistance_between_two_points_is_interpolated(sensor):
    """Between table points the temperature is interpolated, not rounded to a point.

    1.0195 kOhm is halfway between the 0 degC and the 10 degC entries, so 5 degC is the answer a
    straight line gives - and a table that returned its nearest point would answer 0 or 10.
    """
    lib, obj = sensor
    lib.set_voltage_V(obj, CH_VIN, 3.3)
    lib.set_voltage_V(obj, CH_VOUT, _vout_for_low_side(3.3, 1.0195))

    status, temp = lib.temp(obj)

    assert status == lib.OK
    assert temp == pytest.approx(5.0, abs=0.1)


def test_ALX1553_P253_the_supply_voltage_cancels_out(sensor):
    """The same RTD reads the same temperature whatever the divider is driven with.

    A divider is a ratio, so the supply cancels; a module that used the middle voltage alone would
    report a temperature that moved with the rail. Three supplies, one answer.
    """
    lib, obj = sensor
    readings = []
    for vin in (1.8, 3.3, 5.0):
        lib.set_voltage_V(obj, CH_VIN, vin)
        lib.set_voltage_V(obj, CH_VOUT, _vout_for_low_side(vin, 1.117))
        readings.append(lib.temp(obj))

    assert all(status == lib.OK for status, _ in readings)
    assert [pytest.approx(30.0, abs=0.1)] * 3 == [temp for _, temp in readings]


def test_ALX1553_P254_both_channels_are_read_every_time(sensor):
    """Each reading takes one sample from each channel - the top of the divider and its middle.

    A module that read one channel twice would still produce a plausible number, and would be wrong
    by exactly the ratio it was supposed to compute.
    """
    lib, obj = sensor
    lib.set_voltage_V(obj, CH_VIN, 3.3)
    lib.set_voltage_V(obj, CH_VOUT, 1.65)

    lib.temp(obj)
    assert (lib.read_count(obj, CH_VIN), lib.read_count(obj, CH_VOUT)) == (1, 1)

    lib.temp(obj)
    assert (lib.read_count(obj, CH_VIN), lib.read_count(obj, CH_VOUT)) == (2, 2)


@pytest.mark.parametrize(("rtd_kOhm", "expected_status"), [
    (0.700, "ERR_MIN"),     # colder than the table's first point
    (1.600, "ERR_MAX"),     # hotter than its last
])
def test_ALX1553_P255_a_resistance_off_the_end_of_the_table_says_so(
    sensor,
    rtd_kOhm,  # noqa: N803 - the unit belongs in the name
    expected_status,
):
    """Past either end of the table the status says which end, and the value is the end's.

    The module returns the table's status untouched, which is the only way a caller can tell a
    reading of 110 degC from a sensor that is hotter than the table can describe - or from a broken
    connection, which reads as one end or the other.
    """
    lib, obj = sensor
    lib.set_voltage_V(obj, CH_VIN, 3.3)
    lib.set_voltage_V(obj, CH_VOUT, _vout_for_low_side(3.3, rtd_kOhm))

    status, temp = lib.temp(obj)

    assert status == getattr(lib, expected_status)
    assert temp == (PT1000_DEGC[0] if expected_status == "ERR_MIN" else PT1000_DEGC[-1])


def test_ALX1553_P256_the_rtd_can_be_the_high_half_of_the_divider(temp_sens_lib):
    """The same sensor with the RTD on the other side of the divider reads the same temperature.

    Which half the RTD is in is a wiring choice and a constructor flag, and getting it backwards
    gives a reading that moves the right way and is wrong everywhere - the worst kind.
    """
    lib = temp_sens_lib
    obj = lib.new(PT1000_KOHM, PT1000_DEGC, ch_vin=CH_VIN, ch_vout=CH_VOUT,
                  rtd_low=False, res_other_kOhm=RES_OTHER_KOHM)

    # with the RTD on top, the middle voltage is the FIXED resistor's share
    rtd_kOhm = 1.117  # noqa: N806 - the unit belongs in the name; 1.117 kOhm is 30 degC
    vin = 3.3
    lib.set_voltage_V(obj, CH_VIN, vin)
    lib.set_voltage_V(obj, CH_VOUT, vin * RES_OTHER_KOHM / (RES_OTHER_KOHM + rtd_kOhm))

    status, temp = lib.temp(obj)

    assert status == lib.OK
    assert temp == pytest.approx(30.0, abs=0.1)


def test_ALX1553_P257_the_fixed_resistor_scales_the_measurement(temp_sens_lib):
    """The other half of the divider is a parameter, and the reading follows it.

    A sensor built with a 10 kOhm partner needs ten times the middle voltage ratio to see the same
    RTD, so a module that ignored the parameter would read the same number for both and be right
    only for the one it was written against.
    """
    lib = temp_sens_lib
    obj = lib.new(PT1000_KOHM, PT1000_DEGC, rtd_low=True, res_other_kOhm=10.0)

    lib.set_voltage_V(obj, CH_VIN, 3.3)
    lib.set_voltage_V(obj, CH_VOUT, _vout_for_low_side(3.3, 1.039, other_kOhm=10.0))

    status, temp = lib.temp(obj)

    assert status == lib.OK
    assert temp == pytest.approx(10.0, abs=0.1)


def test_ALX1553_P258_a_middle_voltage_equal_to_the_top_is_not_a_temperature(sensor):
    """CHARACTERIZATION: an open circuit divides by zero, and the reading is whatever that gives.

    With the RTD in the low half, an open connection to the fixed resistor puts the middle voltage
    at the top one, and the resistance formula divides by (vin - vout) = 0. C says nothing useful
    about that for floats; on this build it produces an infinity, the table clips it, and the caller
    is told ERR_MAX - which is at least a status a caller can act on, but it is arithmetic reaching
    that conclusion rather than the module detecting an open circuit.

    Recorded, not sealed: nothing in the module claims to detect a broken sensor, and a caller that
    treats ERR_MAX as "too hot" would treat a broken wire as too hot as well.
    """
    lib, obj = sensor
    lib.set_voltage_V(obj, CH_VIN, 3.3)
    lib.set_voltage_V(obj, CH_VOUT, 3.3)

    status, temp = lib.temp(obj)

    assert status == lib.ERR_MAX
    assert temp == PT1000_DEGC[-1]
