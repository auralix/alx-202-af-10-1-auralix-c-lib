"""Auralix C Library - AlxAdxl355 on the host.

A three-axis accelerometer on SPI. A register access is one chip-select frame: the address
shifted left by one with the read bit in bit 0, then the data. A sample is 20 bits of two's
complement per axis, most significant byte first and left-justified in three bytes; at the +-2 g
range the helper keeps, one count is 3.9 ug. The temperature is 12 bits, 1885 counts at 25 degC
and -9.05 counts per degC.

The configuration registers carry enumeration bit-fields, which the host compiler lays out
wider than the target (A93), so these tests read only the first byte the driver sends after a
register's address and never the length of a configuration write.

Proofs (ALX-1564, A92): P628-P633.
"""

import pytest

ID_BYTES = bytes([0xAD, 0x1D, 0xED, 0x01])       # DEVID_AD, DEVID_MST, PARTID, REVID
POWER_CTL_WRITE = 0x2D << 1
DATA_READ = (0x08 << 1) | 0x01
LSB_G = 3.9e-6


@pytest.fixture
def adxl(adxl355_lib):
    """One accelerometer, initialised, with what it sent and received so far cleared."""
    obj = adxl355_lib.new()
    adxl355_lib.queue_miso(ID_BYTES)
    assert adxl355_lib.call(obj, "Init") == adxl355_lib.OK
    adxl355_lib.spi_reset()
    return adxl355_lib, obj


def _power_ctl_writes(mosi: bytes) -> list[int]:
    """The first value byte of every POWER_CTL write, in order."""
    return [mosi[i + 1] for i in range(len(mosi) - 1) if mosi[i] == POWER_CTL_WRITE]


def test_ALX1564_P628_init_forces_standby_first_and_reads_the_four_id_registers(adxl355_lib):
    """Standby before anything else, then the four ID reads, then the configuration.

    A part left measuring by a previous run would otherwise be reconfigured while it measures.
    Eleven frames: the standby write, four ID reads and six configuration writes.
    """
    obj = adxl355_lib.new()
    adxl355_lib.queue_miso(ID_BYTES)

    assert adxl355_lib.call(obj, "Init") == adxl355_lib.OK

    mosi = adxl355_lib.mosi()
    assert mosi[0] == POWER_CTL_WRITE, "the first frame was not a POWER_CTL write"
    assert mosi[1] & 0x01 == 0x01, "the first POWER_CTL write did not set standby"
    assert adxl355_lib.miso_left() == 0, "the four ID registers were not all read"
    assert adxl355_lib.cs_frames() == 11
    assert adxl355_lib.cs_asserted() is False


@pytest.mark.parametrize(("wire", "counts"), [
    (bytes([0x00, 0x00, 0x00]), 0),
    (bytes([0x00, 0x00, 0x10]), 1),
    (bytes([0xFF, 0xFF, 0xF0]), -1),
    (bytes([0x7F, 0xFF, 0xF0]), 524287),      # positive full scale
    (bytes([0x80, 0x00, 0x00]), -524288),     # negative full scale
])
def test_ALX1564_P629_a_sample_is_20_bit_twos_complement_most_significant_byte_first(adxl, wire, counts):
    """Each axis the same way, and each axis its own three bytes: X, then Y, then Z.

    The value under test is on one axis at a time and the other two hold different values, so a
    driver that mixed up the axes or their bytes would answer with one of those.
    """
    lib, obj = adxl
    lib.queue_miso(wire + bytes([0x00, 0x00, 0x20]) + bytes([0xFF, 0xFF, 0xE0]))

    status, (x, y, z) = lib.xyz_g(obj)

    assert status == lib.OK
    assert x == pytest.approx(counts * LSB_G, abs=LSB_G / 10)
    assert y == pytest.approx(2 * LSB_G, abs=LSB_G / 10)
    assert z == pytest.approx(-2 * LSB_G, abs=LSB_G / 10)


def test_ALX1564_P630_a_sample_read_is_one_frame_of_the_data_address_and_nine_bytes(adxl):
    """One chip select, one address byte with the read bit, and the nine data bytes received."""
    lib, obj = adxl
    lib.queue_miso(bytes(9))

    lib.xyz_g(obj)

    assert lib.mosi() == bytes([DATA_READ])
    assert lib.cs_frames() == 1
    assert lib.miso_left() == 0


@pytest.mark.parametrize(("counts", "deg_c"), [(1885, 25.0), (1795, 34.9447), (1975, 15.0553)])
def test_ALX1564_P631_the_temperature_is_1885_counts_at_25_degc_falling_9_05_per_degree(adxl, counts, deg_c):
    """TEMP2 (the high nibble) then TEMP1 on the wire; more counts is colder."""
    lib, obj = adxl
    lib.queue_miso(bytes([counts >> 8, counts & 0xFF]))

    status, temp = lib.temp_degc(obj)

    assert status == lib.OK
    assert temp == pytest.approx(deg_c, abs=0.01)


def test_ALX1564_P632_enable_and_disable_write_the_standby_bit(adxl):
    """Enable clears POWER_CTL's standby bit and starts measuring; Disable sets it again."""
    lib, obj = adxl

    assert lib.call(obj, "Enable") == lib.OK
    assert lib.call(obj, "Disable") == lib.OK

    assert [b & 0x01 for b in _power_ctl_writes(lib.mosi())] == [0, 1]


def test_ALX1564_P633_init_fails_when_the_bus_fails(adxl355_lib):
    """A bus that cannot be initialised stops Init at its first step, with the bus's error."""
    obj = adxl355_lib.new()
    adxl355_lib.force_spi_status(adxl355_lib.ERR)

    assert adxl355_lib.call(obj, "Init") == adxl355_lib.ERR

    assert adxl355_lib.mosi() == b"", "a part behind a failed bus was written"
