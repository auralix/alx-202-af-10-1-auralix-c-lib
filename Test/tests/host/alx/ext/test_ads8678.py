"""Auralix C Library - AlxAds8678 on the host.

An eight-channel 14-bit ADC on SPI. A register write is one three-byte frame - the address shifted
left with the write bit set, the value, a dummy - and the part echoes the value in the third byte,
which the driver checks. A conversion run is one AUTO_RST frame and then one NO_OP frame per
channel the auto-scan register enables, in channel order; the result is 14 bits left-justified in
16, and in the part's +-2.5 x Vref range one count is 1.25 mV from -10.24 V. The output mode
(SDO) decides how long a NO_OP frame is and what it carries besides the data: the channel
address, the device address, the range.

The helper is the application: each test chooses the raw configuration bytes its Init writes.

Writing these found one defect, sealed as P639: the channel's range is used for the conversion only
in the output mode that reports it; in every other mode every channel converts as +-2.5 x Vref.

Proofs (ALX-1564, A92): P634-P640.
"""

import pytest

REG_ORDER = (0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C)
AUTO_RST = bytes([0xA0, 0x00, 0x00, 0x00])


def _values(auto_scan=0xFF, feature_select=0x00, ranges=bytes(8)) -> list[int]:
    """What Init writes, register by register: the defaults with the application's bytes over them."""
    return [auto_scan, 0x00, feature_select, *ranges]


def _echoes(values) -> bytes:
    """The part's answer to each write frame: the value back in the third byte."""
    return b"".join(bytes([0x00, 0x00, v]) for v in values)


def _frame(data: int, extra: bytes = b"") -> bytes:
    """A NO_OP answer: two bytes during the command, then the 16-bit result and whatever SDO adds."""
    return bytes([0x00, 0x00, data >> 8, data & 0xFF]) + extra


@pytest.fixture
def adc(ads8678_lib):
    """One converter with the default configuration, initialised, with its traffic cleared."""
    obj = ads8678_lib.new()
    ads8678_lib.queue_miso(_echoes(_values()))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.spi_reset()
    return ads8678_lib, obj


def test_ALX1564_P634_init_wakes_the_part_and_writes_every_register_once(ads8678_lib):
    """nRST high, the 100 ms wake-up, then eleven write frames in register order.

    Each frame is the address with the write bit, the value and a dummy byte; every one is its own
    chip-select frame, so a part that lost one frame does not take the next byte as an address.
    """
    obj = ads8678_lib.new()
    values = _values(auto_scan=0x0F, feature_select=0x01, ranges=bytes([0x05] * 8))
    ads8678_lib.configure(auto_scan=0x0F, feature_select=0x01, ranges=bytes([0x05] * 8))
    ads8678_lib.queue_miso(_echoes(values))

    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK

    assert ads8678_lib.nrst_level(obj) is True
    assert ads8678_lib.delays_ms() == 1
    frames = [bytes([(a << 1) | 0x01, v, 0x00]) for a, v in zip(REG_ORDER, values, strict=True)]
    assert ads8678_lib.mosi() == b"".join(frames)
    assert ads8678_lib.cs_frames() == len(REG_ORDER)


def test_ALX1564_P635_a_register_the_part_does_not_echo_fails_init(ads8678_lib):
    """The echo is the driver's proof that a write arrived; a wrong one stops Init with an error."""
    obj = ads8678_lib.new()
    echoes = bytearray(_echoes(_values()))
    echoes[3 * 2 + 2] ^= 0xFF                 # the third register's echo is wrong
    ads8678_lib.queue_miso(bytes(echoes))

    assert ads8678_lib.call(obj, "Init") == ads8678_lib.ERR

    assert ads8678_lib.cs_frames() == 3, "the writes went on after the failed echo"


@pytest.mark.parametrize(("data", "volts"), [(0x0000, -10.24), (0x8000, 0.0), (0xFFFC, 10.23875), (0x4000, -5.12)])
def test_ALX1564_P636_a_reading_is_14_bits_of_1_25_mv_from_minus_10_24_v(adc, data, volts):
    """The default range, +-2.5 x Vref with the 4.096 V reference: every channel the same way."""
    lib, obj = adc
    lib.queue_miso(AUTO_RST + _frame(data) * 8)

    status, v, enabled = lib.voltages(obj)

    assert status == lib.OK
    assert enabled == [True] * 8
    assert v == pytest.approx([volts] * 8, abs=1e-4)


def test_ALX1564_P637_only_the_enabled_channels_are_read(ads8678_lib):
    """Channels 0 and 2 enabled: one AUTO_RST and two NO_OP frames; the others read zero, disabled."""
    obj = ads8678_lib.new()
    ads8678_lib.configure(auto_scan=0b0000_0101)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0101)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.spi_reset()
    ads8678_lib.queue_miso(AUTO_RST + _frame(0xFFFC) + _frame(0x0000))

    status, v, enabled = ads8678_lib.voltages(obj)

    assert status == ads8678_lib.OK
    assert enabled == [True, False, True, False, False, False, False, False]
    assert v == pytest.approx([10.23875, 0.0, -10.24, 0.0, 0.0, 0.0, 0.0, 0.0], abs=1e-4)
    assert ads8678_lib.mosi() == AUTO_RST + bytes(8)
    assert ads8678_lib.cs_frames() == 3


def test_ALX1564_P638_with_the_channel_address_on_a_frame_from_another_channel_is_an_error(ads8678_lib):
    """SDO with the channel address: the fifth byte's high nibble must be the channel being read.

    A frame out of order means the scan and the driver disagree about which channel is which, and
    every value after it would be filed under the wrong channel.
    """
    obj = ads8678_lib.new()
    ads8678_lib.configure(auto_scan=0b0000_0011, feature_select=0x01)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0011, feature_select=0x01)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK

    ads8678_lib.queue_miso(AUTO_RST + _frame(0x8000, bytes([0 << 4])) + _frame(0x8000, bytes([1 << 4])))
    assert ads8678_lib.voltages(obj)[0] == ads8678_lib.OK, "two frames in order were refused"

    ads8678_lib.queue_miso(AUTO_RST + _frame(0x8000, bytes([1 << 4])))
    assert ads8678_lib.voltages(obj)[0] == ads8678_lib.ERR


def test_ALX1564_P639_a_channel_converts_in_the_range_init_gave_it(ads8678_lib):
    """Channel 0 at 0..2.5 x Vref (0 to 10.24 V, 625 uV a count): mid-scale is 5.12 V."""
    obj = ads8678_lib.new()
    ranges = bytes([0x05] + [0x00] * 7)
    ads8678_lib.configure(auto_scan=0b0000_0001, ranges=ranges)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, ranges=ranges)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.queue_miso(AUTO_RST + _frame(0x8000))

    status, v, _ = ads8678_lib.voltages(obj)

    assert status == ads8678_lib.OK
    assert v[0] == pytest.approx(5.12, abs=1e-3)


def test_ALX1564_P640_deinit_writes_the_defaults_back_and_holds_the_part_in_reset(adc):
    """The part's own defaults, eleven frames, then nRST low: power down, every register reset."""
    lib, obj = adc
    lib.queue_miso(_echoes(_values()))

    assert lib.call(obj, "DeInit") == lib.OK

    assert lib.mosi() == b"".join(bytes([(a << 1) | 0x01, v, 0x00]) for a, v in zip(REG_ORDER, _values(), strict=True))
    assert lib.nrst_level(obj) is False


# =====================================================================
# P787-P793 - the bus failing at any step, the other output modes, the other ranges, the chain's ends (A97)
# =====================================================================

MID = 0x8000                      # mid-scale, left-justified: 8192 counts


def _range_frame(data: int, range_code: int, dev: int = 0, ch: int = 0) -> bytes:
    """A NO_OP answer in the in-range mode: the channel, the device, the range's low three bits."""
    lsb3 = range_code & 0x07
    return _frame(data, bytes([(ch << 4) | (dev << 2) | (lsb3 >> 1), (lsb3 & 1) << 7]))


def test_ALX1564_P787_the_bus_that_will_not_initialise_or_release_is_reported(ads8678_lib):
    obj = ads8678_lib.new(periph=False)
    ads8678_lib.spi_fail_at(1)
    assert ads8678_lib.call(obj, "InitPeriph") == ads8678_lib.ERR
    assert ads8678_lib.call(obj, "InitPeriph") == ads8678_lib.OK

    ads8678_lib.spi_fail_at(1)
    assert ads8678_lib.call(obj, "DeInitPeriph") == ads8678_lib.ERR
    assert ads8678_lib.call(obj, "DeInitPeriph") == ads8678_lib.OK
    assert ads8678_lib.spi_inits() == (2, 2)


@pytest.mark.parametrize("nth", range(1, 12))
def test_ALX1564_P788_init_stops_at_the_register_write_that_fails(ads8678_lib, nth):
    obj = ads8678_lib.new()
    ads8678_lib.queue_miso(_echoes(_values()))
    ads8678_lib.spi_fail_at(nth)

    assert ads8678_lib.call(obj, "Init") == ads8678_lib.ERR
    assert ads8678_lib.cs_frames() == nth


def test_ALX1564_P789_deinit_reports_a_bus_that_fails(adc):
    lib, obj = adc
    lib.queue_miso(_echoes(_values()))
    lib.spi_fail_at(1)

    assert lib.call(obj, "DeInit") == lib.ERR


@pytest.mark.parametrize("nth", [1, 2])
def test_ALX1564_P790_a_reading_stops_at_the_frame_that_fails(adc, nth):
    """The AUTO_RST frame, then the first NO_OP frame: either reports the bus and ends the scan."""
    lib, obj = adc
    lib.queue_miso(AUTO_RST + _frame(MID) * 8)
    lib.spi_fail_at(nth)

    assert lib.voltages(obj)[0] == lib.ERR
    assert lib.cs_frames() == nth


def test_ALX1564_P791_with_the_device_address_on_a_frame_from_another_device_is_an_error(ads8678_lib):
    """SDO with the channel and the device address: five-byte frames, the device in bits 2 and 3 of the fifth."""
    obj = ads8678_lib.new()
    ads8678_lib.configure(auto_scan=0b0000_0001, feature_select=0x02)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, feature_select=0x02)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.spi_reset()

    ads8678_lib.queue_miso(AUTO_RST + _frame(MID, bytes([0x00])))
    assert ads8678_lib.voltages(obj)[0] == ads8678_lib.OK
    assert ads8678_lib.mosi() == AUTO_RST + bytes(5)

    ads8678_lib.queue_miso(AUTO_RST + _frame(MID, bytes([1 << 2])))
    assert ads8678_lib.voltages(obj)[0] == ads8678_lib.ERR


@pytest.mark.parametrize(("range_code", "mid_volts"), [
    (0b0001, 0.0), (0b0010, 0.0), (0b0011, 0.0),      # the bipolar ranges, mid-scale is zero
    (0b0110, 2.56), (0b0111, 1.28),                   # the unipolar ones, mid-scale is half
])
def test_ALX1564_P792_in_the_in_range_mode_each_range_converts_by_its_own_step_and_offset(ads8678_lib, range_code, mid_volts):
    """Six-byte frames carrying the range's low three bits, checked against the channel's register, then converted.

    The two ranges with the top bit set, 0b1011 and 0b1111, are P792's other test: the host cannot reach them.
    """
    obj = ads8678_lib.new()
    ranges = bytes([range_code] + [0x00] * 7)
    ads8678_lib.configure(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.spi_reset()
    ads8678_lib.queue_miso(AUTO_RST + _range_frame(MID, range_code))

    status, v, _ = ads8678_lib.voltages(obj)

    assert status == ads8678_lib.OK
    assert v[0] == pytest.approx(mid_volts, abs=1e-3)
    assert ads8678_lib.mosi() == AUTO_RST + bytes(6)


@pytest.mark.parametrize("range_code", [0b1011, 0b1111])
@pytest.mark.expect_assert("P792: on this host the two range codes with the top bit set read back negative and reach the else")
def test_ALX1564_P792_the_two_ranges_with_the_top_bit_set_cannot_be_tested_on_this_host(ads8678_lib, range_code, record_property):
    """CHARACTERIZATION: on the target +-0.15625 x Vref and 0..0.3125 x Vref; here the four-bit enumeration field
    reads 0b1011 as -5 and 0b1111 as -1 (A93, as P225 describes for the INA228), so neither branch of the range
    parameters is reached, the conversion asserts and the channel reads 0 V. The read succeeds; recorded."""
    obj = ads8678_lib.new()
    ranges = bytes([range_code] + [0x00] * 7)
    ads8678_lib.configure(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.queue_miso(AUTO_RST + _range_frame(MID, range_code))

    status, v, _ = ads8678_lib.voltages(obj)

    record_property("host_mid_scale_volts", v[0])
    assert status == ads8678_lib.OK


def test_ALX1564_P792_a_frame_carrying_another_range_than_the_channels_is_an_error(ads8678_lib):
    obj = ads8678_lib.new()
    ranges = bytes([0b0001] + [0x00] * 7)
    ads8678_lib.configure(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.queue_miso(AUTO_RST + _range_frame(MID, 0b0010))

    assert ads8678_lib.voltages(obj)[0] == ads8678_lib.ERR


@pytest.mark.expect_assert("P793: a range code outside the enumeration reaches the else of the range parameters")
def test_ALX1564_P793_a_range_the_enumeration_does_not_have_converts_to_zero(ads8678_lib):
    """CHARACTERIZATION: the step and the offset stay zero, so the channel reads 0 V whatever the count."""
    obj = ads8678_lib.new()
    ranges = bytes([0b0100] + [0x00] * 7)
    ads8678_lib.configure(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, feature_select=0x03, ranges=ranges)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.queue_miso(AUTO_RST + _range_frame(MID, 0b0100))

    status, v, _ = ads8678_lib.voltages(obj)

    assert status == ads8678_lib.OK
    assert v[0] == 0.0


@pytest.mark.expect_assert("P793: an output mode outside the enumeration reaches the else of the frame length")
def test_ALX1564_P793_an_output_mode_the_enumeration_does_not_have_sends_an_empty_frame(ads8678_lib):
    """CHARACTERIZATION: the NO_OP frame has no length, nothing is checked, and the channel reads the bottom of
    its range from the zeros nothing filled."""
    obj = ads8678_lib.new()
    ads8678_lib.configure(auto_scan=0b0000_0001, feature_select=0x04)
    ads8678_lib.queue_miso(_echoes(_values(auto_scan=0b0000_0001, feature_select=0x04)))
    assert ads8678_lib.call(obj, "Init") == ads8678_lib.OK
    ads8678_lib.spi_reset()
    ads8678_lib.queue_miso(AUTO_RST)

    status, v, _ = ads8678_lib.voltages(obj)

    assert status == ads8678_lib.OK
    assert ads8678_lib.mosi() == AUTO_RST
    assert v[0] == pytest.approx(-10.24, abs=1e-4)
