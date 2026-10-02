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
