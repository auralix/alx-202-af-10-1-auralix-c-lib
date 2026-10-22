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
