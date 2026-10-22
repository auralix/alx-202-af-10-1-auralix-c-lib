"""Auralix C Library - AlxTmp1075 on the host.

A temperature sensor whose reading is 12 bits of two's complement, left-justified in a 16-bit
register and sent most significant byte first: one count is 0.0625 degC, and the four low bits
are always zero. At Init the driver reads the device ID, then writes the configuration and the
two alarm limits with the part's reset values, which a product may change through the weak
RegStruct_SetVal.

Writing these found one defect, sealed as P599: every temperature below zero reads as one between
128 and 256 degC.

Proofs (ALX-1564, A92): P597-P602.
"""

import pytest

REG_TEMP, REG_CONFIG, REG_LIMIT_LOW, REG_LIMIT_HIGH, REG_DEVICE_ID = 0x00, 0x01, 0x02, 0x03, 0x0F


@pytest.fixture
def sensor(tmp1075_lib):
    """One sensor, initialised over a ready bus."""
    obj = tmp1075_lib.new()
    assert tmp1075_lib.init(obj) == tmp1075_lib.OK
    return tmp1075_lib, obj


def test_ALX1564_P597_init_reads_the_id_and_writes_the_configuration_and_both_limits(tmp1075_lib):
    """Each register once, with the part's own reset values: the limits 75 and 80 degC.

    The values are compared as wire bytes because the driver writes the registers' raw words,
    most significant byte first, as the part expects them.
    """
    obj = tmp1075_lib.new()

    assert tmp1075_lib.init(obj) == tmp1075_lib.OK

    assert tmp1075_lib.is_init(obj) is True
    assert tmp1075_lib.read_count(REG_DEVICE_ID) == 1
    assert [tmp1075_lib.write_count(r) for r in (REG_CONFIG, REG_LIMIT_LOW, REG_LIMIT_HIGH)] == [1, 1, 1]
    assert tmp1075_lib.last_write(REG_CONFIG, 2) == bytes([0x00, 0xFF])
    assert tmp1075_lib.last_write(REG_LIMIT_LOW, 2) == bytes([0x4B, 0x00]), "75 degC"
    assert tmp1075_lib.last_write(REG_LIMIT_HIGH, 2) == bytes([0x50, 0x00]), "80 degC"
    assert tmp1075_lib.write_count(REG_TEMP) == 0, "the read-only temperature register was written"


@pytest.mark.parametrize(("wire", "deg_c"), [
    (bytes([0x00, 0x00]), 0.0),
    (bytes([0x00, 0x10]), 0.0625),       # one count
    (bytes([0x19, 0x00]), 25.0),
    (bytes([0x4B, 0x00]), 75.0),
    (bytes([0x7F, 0xF0]), 127.9375),     # the largest positive reading
])
def test_ALX1564_P598_a_positive_reading_is_counts_of_a_sixteenth_of_a_degree(sensor, wire, deg_c):
    """The register's upper 12 bits times 0.0625 degC, from one count to the top of the range."""
    lib, obj = sensor
    lib.set_register(REG_TEMP, wire)

    assert lib.temp_degc(obj) == pytest.approx(deg_c)


@pytest.mark.parametrize(("wire", "deg_c"), [
    (bytes([0xFF, 0xF0]), -0.0625),
    (bytes([0xE7, 0x00]), -25.0),
    (bytes([0xC9, 0x00]), -55.0),        # the bottom of the part's range
])
def test_ALX1564_P599_a_negative_reading_is_a_temperature_below_zero(sensor, wire, deg_c):
    """Below zero the register is two's complement, and the reading is negative."""
    lib, obj = sensor
    lib.set_register(REG_TEMP, wire)

    assert lib.temp_degc(obj) == pytest.approx(deg_c)


def test_ALX1564_P600_init_fails_when_the_part_does_not_answer(tmp1075_lib):
    """No acknowledge on the bus: Init reports the error and the sensor stays uninitialised.

    Nothing is written to a part that did not answer, and its ID is not read.
    """
    obj = tmp1075_lib.new()
    tmp1075_lib.slave_ready(False)

    assert tmp1075_lib.init(obj) == tmp1075_lib.ERR

    assert tmp1075_lib.is_init(obj) is False
    assert tmp1075_lib.read_count(REG_DEVICE_ID) == 0
    assert tmp1075_lib.write_count(REG_CONFIG) == 0


def test_ALX1564_P601_init_fails_when_the_bus_fails(tmp1075_lib):
    """A bus that cannot be initialised stops Init at its first step, with the bus's error."""
    obj = tmp1075_lib.new()
    tmp1075_lib.force_status(tmp1075_lib.ERR)

    assert tmp1075_lib.init(obj) == tmp1075_lib.ERR

    assert tmp1075_lib.is_init(obj) is False


def test_ALX1564_P602_deinit_returns_the_sensor_to_uninitialised(sensor):
    """DeInit releases the bus and leaves the sensor as a fresh one, ready for another Init."""
    lib, obj = sensor

    assert lib.deinit(obj) == lib.OK
    assert lib.is_init(obj) is False

    assert lib.init(obj) == lib.OK
    assert lib.is_init(obj) is True
