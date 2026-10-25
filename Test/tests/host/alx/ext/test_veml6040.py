"""Auralix C Library - AlxVeml6040 on the host.

A colour sensor with four 16-bit channels - red, green, blue and white - each in its own register
and sent least significant byte first, and one configuration register. Init writes the
configuration the application chose through the weak RegStruct_SetVal (the helper is that
application here: the sensor enabled), and DeInit writes the part's default back, which shuts
the sensor down.

Proofs (ALX-1564, A92): P603-P607.
"""

import pytest

REG_CONF = 0x00
REG_DATA = {"R": 0x08, "G": 0x09, "B": 0x0A, "W": 0x0B}


@pytest.fixture
def sensor(veml6040_lib):
    """One sensor, initialised over a ready bus."""
    obj = veml6040_lib.new()
    assert veml6040_lib.init(obj) == veml6040_lib.OK
    return veml6040_lib, obj


def test_ALX1564_P603_init_writes_the_configuration_the_application_chose(veml6040_lib):
    """Once, after the part answered: here all zero bits, the sensor enabled in auto mode."""
    obj = veml6040_lib.new()

    assert veml6040_lib.init(obj) == veml6040_lib.OK

    assert veml6040_lib.write_count(REG_CONF) == 1
    assert veml6040_lib.last_write(REG_CONF, 2) == bytes([0x00, 0x00])


def test_ALX1564_P604_deinit_shuts_the_sensor_down(sensor):
    """DeInit writes the part's default configuration, whose bit 0 (SD) shuts the sensor down.

    A sensor that is no longer used should not keep integrating light and drawing current, and the
    default is exactly that state.
    """
    lib, obj = sensor

    assert lib.deinit(obj) == lib.OK

    assert lib.write_count(REG_CONF) == 2
    assert lib.last_write(REG_CONF, 2) == bytes([0x01, 0x00])


@pytest.mark.parametrize("ch", ["R", "G", "B", "W"])
def test_ALX1564_P605_each_channel_is_its_own_register_least_significant_byte_first(sensor, ch):
    """0x34 then 0x12 on the wire is 0x1234, and only the channel asked for is read.

    The other three hold different values, so a driver that read the wrong register would answer
    with one of them.
    """
    lib, obj = sensor
    for other, addr in REG_DATA.items():
        lib.set_register(addr, bytes([0x00, 0x00]) if other != ch else bytes([0x34, 0x12]))

    assert lib.channel(obj, ch) == (lib.OK, 0x1234)
    assert lib.read_count(REG_DATA[ch]) == 1
    assert sum(lib.read_count(a) for c, a in REG_DATA.items() if c != ch) == 0


def test_ALX1564_P606_a_failed_read_returns_the_error_and_leaves_the_value_alone(sensor):
    """On a bus error the caller gets the error, and its variable keeps what it held."""
    lib, obj = sensor
    lib.set_register(REG_DATA["G"], bytes([0xFF, 0xFF]))
    lib.force_status(lib.ERR)

    assert lib.channel(obj, "G", preset=0xBEEF) == (lib.ERR, 0xBEEF)


def test_ALX1564_P607_init_fails_when_the_part_does_not_answer(veml6040_lib):
    """No acknowledge: Init reports the error and writes nothing."""
    obj = veml6040_lib.new()
    veml6040_lib.slave_ready(False)

    assert veml6040_lib.init(obj) == veml6040_lib.ERR

    assert veml6040_lib.write_count(REG_CONF) == 0


# =====================================================================
# P754-P756 - the bus fails at one chosen step (A97)
# =====================================================================


@pytest.mark.parametrize("nth", [1, 2, 3])
def test_ALX1564_P754_init_stops_at_the_bus_step_that_fails(veml6040_lib, nth):
    """The bus's Init, the part's acknowledge, the configuration write: whichever fails ends Init."""
    obj = veml6040_lib.new()
    veml6040_lib.fail_at(nth)

    assert veml6040_lib.init(obj) == veml6040_lib.ERR
    assert veml6040_lib.write_count(REG_CONF) == (1 if nth == 3 else 0)


@pytest.mark.parametrize("nth", [1, 2])
def test_ALX1564_P755_deinit_stops_at_the_bus_step_that_fails(sensor, nth):
    """The default written back, then the bus released: whichever fails ends DeInit with the error."""
    lib, obj = sensor
    lib.fail_at(nth)

    assert lib.deinit(obj) == lib.ERR


@pytest.mark.parametrize("ch", ["R", "B", "W"])
def test_ALX1564_P756_a_failed_read_of_each_other_channel_leaves_the_value_alone(sensor, ch):
    """P606 is the green channel; the red, blue and white reads are their own functions."""
    lib, obj = sensor
    lib.fail_at(1)

    assert lib.channel(obj, ch, preset=0xBEEF) == (lib.ERR, 0xBEEF)
