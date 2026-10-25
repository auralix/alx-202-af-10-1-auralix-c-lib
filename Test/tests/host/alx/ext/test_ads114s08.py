"""Auralix C Library - AlxAds114s08 on the host.

A 16-bit delta-sigma ADC on SPI with three pins: nRST, START and nDRDY. A conversion is START high,
a wait for nDRDY low, one RDATA frame (0x12 and two dummies) whose last two bytes are the result,
and START low. The result is two's complement.

Not proven here until A93 - recorded rather than skipped silently:

- Init, the device ID and the readings that configure the part first. Each writes a register and
  reads it back with a frame sized by the register's union, which the host compiler makes up to
  five bytes where the target makes one: the read-back frame would be written past its three-byte
  buffer. The helper leaves each converter as Init leaves it.
- The conversion of counts to millivolts. It asserts that the reference fields hold
  REFSEL_Internal2_5Vref and REFCON_InternalRefAlwaysOn, both 0b10 in 2-bit enumeration fields,
  which this host stores signed and reads back as -2; and its gain switch reads PGA_GAIN_128,
  0b111 in a 3-bit field, as -1.

Proofs (ALX-1564, A92): P641.
"""

import pytest


@pytest.fixture
def adc(ads114s08_lib):
    """One converter, its pins and bus initialised, data ready."""
    return ads114s08_lib, ads114s08_lib.new()


@pytest.mark.parametrize(("wire", "raw"), [
    (bytes([0x00, 0x12, 0x34]), 0x1234),
    (bytes([0x00, 0x7F, 0xFF]), 32767),
    (bytes([0x00, 0xFF, 0xFE]), -2),
    (bytes([0x00, 0x80, 0x00]), -32768),
])
def test_ALX1564_P641_a_conversion_is_start_rdata_and_a_twos_complement_result(adc, wire, raw):
    """START high, one RDATA frame in one chip select, the result from its last two bytes, START low."""
    lib, obj = adc
    lib.queue_miso(wire)
    start_writes = lib.pin_writes(obj, "Start")

    assert lib.perform(obj) == (lib.OK, raw)

    assert lib.mosi() == bytes([0x12, 0x00, 0x00])
    assert lib.cs_frames() == 1
    assert lib.pin_writes(obj, "Start") - start_writes == 2
    assert lib.level(obj, "Start") is False


# =====================================================================
# P801-P803 - the bus that will not initialise or release, a conversion that times out, a read that fails (A97)
# =====================================================================


def test_ALX1564_P801_the_bus_that_will_not_initialise_or_release_is_reported(ads114s08_lib):
    obj = ads114s08_lib.new(periph=False)
    ads114s08_lib.spi_fail_at(1)
    assert ads114s08_lib.call(obj, "InitPeriph") == ads114s08_lib.ERR
    assert ads114s08_lib.call(obj, "InitPeriph") == ads114s08_lib.OK

    ads114s08_lib.spi_fail_at(1)
    assert ads114s08_lib.call(obj, "DeInitPeriph") == ads114s08_lib.ERR
    assert ads114s08_lib.call(obj, "DeInitPeriph") == ads114s08_lib.OK
    assert ads114s08_lib.spi_inits() == (2, 2)


def test_ALX1564_P802_a_conversion_that_never_becomes_ready_times_out_with_start_low_and_no_frame(adc):
    """nDRDY stays high: 10 ms waits until the 750 ms timeout, then the error, START released, nothing sent."""
    lib, obj = adc
    lib.set_ndrdy(obj, True)
    lib.time_passes(True)

    status, _ = lib.perform(obj)

    assert status == lib.ERR
    assert lib.level(obj, "Start") is False
    assert lib.mosi() == b""
    assert lib.cs_asserted() is False
    assert 70 <= lib.delays_ms() <= 80


def test_ALX1564_P803_a_read_frame_that_fails_is_reported(adc):
    lib, obj = adc
    lib.queue_miso(bytes([0x00, 0x12, 0x34]))
    lib.spi_fail_at(1)

    assert lib.perform(obj)[0] == lib.ERR
