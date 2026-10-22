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


# =====================================================================
# P794-P800 - the bus failing at any step, DeInit, the FIFO, the status register, the other ranges (A97)
# =====================================================================

# the nth SPI call of Init: the bus's Init; the standby write (address, value); the four ID reads (address,
# data each); the six configuration writes (address, value each)
INIT_STEPS = tuple(range(1, 24))


def _fifo_sample(x: int, y: int, z: int, *, x_marker: bool = True, y_marker: bool = False, z_marker: bool = False,
                 empty: bool = False) -> bytes:
    """One FIFO sample on the wire: each axis most significant byte first, the markers in the low nibble."""
    def axis(counts: int, marker: bool) -> bytes:
        word = (counts & 0xFFFFF) << 4
        return bytes([word >> 16, (word >> 8) & 0xFF, (word & 0xF0) | int(marker) | (int(empty) << 1)])
    return axis(x, x_marker) + axis(y, y_marker) + axis(z, z_marker)


@pytest.mark.parametrize("nth", INIT_STEPS)
def test_ALX1564_P794_init_stops_at_the_bus_call_that_fails(adxl355_lib, nth):
    obj = adxl355_lib.new()
    adxl355_lib.queue_miso(ID_BYTES)
    adxl355_lib.spi_fail_at(nth)

    assert adxl355_lib.call(obj, "Init") == adxl355_lib.ERR


@pytest.mark.parametrize("nth", [1, 2, 3])
def test_ALX1564_P795_deinit_forces_standby_and_releases_the_bus_and_reports_either_failing(adxl, nth):
    """The standby write's two calls, then the bus's DeInit: whichever fails ends DeInit with the error."""
    lib, obj = adxl
    lib.spi_fail_at(nth)
    assert lib.call(obj, "DeInit") == lib.ERR

    assert lib.call(obj, "DeInit") == lib.OK
    assert lib.spi_inits()[1] == (2 if nth == 3 else 1), "the bus's DeInit, counted when it failed too"
    assert _power_ctl_writes(lib.mosi())[-1] & 0x01 == 1, "the part was not put in standby"


@pytest.mark.parametrize("what", ["Enable", "Disable"])
def test_ALX1564_P796_enable_and_disable_report_a_bus_that_fails(adxl, what):
    lib, obj = adxl
    lib.spi_fail_at(1)

    assert lib.call(obj, what) == lib.ERR


def test_ALX1564_P796_the_readings_report_a_bus_that_fails(adxl):
    """A sample, a FIFO read, the temperature and the status register: each reports its bus."""
    lib, obj = adxl
    for read in (lambda: lib.xyz_g(obj)[0], lambda: lib.fifo_xyz_g(obj, 1)[0], lambda: lib.temp_degc(obj)[0],
                 lambda: lib.status_reg(obj)[0]):
        lib.spi_fail_at(1)
        assert read() == lib.ERR


def test_ALX1564_P797_a_negative_y_sample_is_sign_extended(adxl):
    """P629 proves the sign on X; the Y axis has its own padding."""
    lib, obj = adxl
    lib.queue_miso(bytes([0x00, 0x00, 0x00]) + bytes([0xFF, 0xFF, 0xF0]) + bytes([0x00, 0x00, 0x00]))

    status, (x, y, z) = lib.xyz_g(obj)

    assert status == lib.OK
    assert (x, y, z) == pytest.approx((0.0, -LSB_G, 0.0), abs=LSB_G / 10)


@pytest.mark.parametrize("range_code", [2, 3])
@pytest.mark.expect_assert("P798: on this host the 4 g and 8 g codes read back as -2 and -1 and reach the switch's "
                           "default")
def test_ALX1564_P798_the_4_g_and_8_g_ranges_cannot_be_tested_on_this_host(adxl, range_code, record_property):
    """CHARACTERIZATION: on the target 7.8 ug and 15.6 ug a count; here the two-bit enumeration field reads
    0b10 as -2 and 0b11 as -1 (A93, as P225 describes for the INA228), so neither case of the range switch is
    reached and the conversion asserts and answers 0 g. The read succeeds; the number is recorded."""
    lib, obj = adxl
    lib.set_range(obj, range_code)
    lib.queue_miso(bytes([0x00, 0x01, 0x00]) + bytes(6))

    status, (x, _, _) = lib.xyz_g(obj)

    record_property("host_x_g_for_16_counts", x)
    assert status == lib.OK


@pytest.mark.expect_assert("P798: the reserved range code reaches the default of the range switch")
def test_ALX1564_P798_the_reserved_range_asserts_and_reads_zero(adxl):
    """CHARACTERIZATION: the factor is left at zero, so every axis reads 0 g."""
    lib, obj = adxl
    lib.set_range(obj, 0)
    lib.queue_miso(bytes([0x00, 0x01, 0x00]) * 3)

    assert lib.xyz_g(obj) == (lib.OK, (0.0, 0.0, 0.0))


def test_ALX1564_P799_a_fifo_read_is_one_frame_of_nine_bytes_a_sample_with_the_x_marker_checked(adxl):
    """Two samples: the X marker on the first byte of each X, and none on Y or Z, else the read is refused."""
    lib, obj = adxl
    lib.queue_miso(_fifo_sample(16, 32, -16) + _fifo_sample(256, 0, 16))

    status, samples = lib.fifo_xyz_g(obj, 2)

    assert status == lib.OK
    assert lib.mosi() == bytes([(0x11 << 1) | 0x01])
    assert lib.miso_left() == 0
    assert samples[0] == pytest.approx((16 * LSB_G, 32 * LSB_G, -16 * LSB_G), abs=LSB_G / 10)
    assert samples[1] == pytest.approx((256 * LSB_G, 0.0, 16 * LSB_G), abs=LSB_G / 10)


@pytest.mark.parametrize("wrong", [
    {"x_marker": False}, {"y_marker": True}, {"z_marker": True}, {"empty": True},
])
def test_ALX1564_P799_a_fifo_sample_with_the_markers_wrong_is_refused(adxl, wrong):
    lib, obj = adxl
    lib.queue_miso(_fifo_sample(16, 32, 48, **wrong))

    assert lib.fifo_xyz_g(obj, 1)[0] == lib.ERR


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxAdxl355_GetFifoXyz_g loses the low nibble of every FIFO sample. The FIFO overlay splits each "
    "axis's third byte into the two marker bits and a four-bit XDATA1 field, and the conversion puts that "
    "four-bit field where AlxAdxl355_GetXyz_g puts the whole byte (alxAdxl355.c, the Prepare block of "
    "GetFifoXyz_g), then shifts right by four as the plain read does - so the nibble that held the sample's low "
    "four bits is shifted out. Measured: a FIFO sample of 1 count reads 0 g, of 17 counts 16 counts. The fix is "
    "to shift the nibble left by four before the right shift, or to skip the shift for FIFO samples"))
def test_ALX1564_P799_a_fifo_sample_keeps_its_low_four_bits(adxl):
    lib, obj = adxl
    lib.queue_miso(_fifo_sample(1, 17, -1))

    status, samples = lib.fifo_xyz_g(obj, 1)

    assert status == lib.OK
    assert samples[0] == pytest.approx((LSB_G, 17 * LSB_G, -LSB_G), abs=LSB_G / 10)


def test_ALX1564_P800_the_status_register_and_the_fifo_length_are_read_as_the_part_sends_them(adxl):
    """One byte for the status; the FIFO length is the FIFO_SAMPLES watermark the driver configured, in samples of
    three."""
    lib, obj = adxl
    lib.queue_miso(bytes([0b0001_0011]))

    assert lib.status_reg(obj) == (lib.OK, 0b0001_0011)
    assert lib.mosi() == bytes([(0x04 << 1) | 0x01])
    assert lib.fifo_len(obj) == 32, "the part's default watermark of 96 bytes"
