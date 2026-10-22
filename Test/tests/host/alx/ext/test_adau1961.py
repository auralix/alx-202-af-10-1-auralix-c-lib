"""Auralix C Library - AlxAdau1961 on the host.

An audio codec configured over I2C (16-bit register addresses from 0x4000) and streaming over I2S.
Init starts the interface, writes the clock control first and then the 41 other registers the
driver keeps; the application chooses their values in two weak hooks (the helper keeps every
default). The gains are register codes computed from decibels on three scales:

- differential input: -12 dB to 55.5 dB in 0.75 dB steps, a 6-bit volume, with a +20 dB boost
  stage switched in above 35.25 dB - the volume then carries the rest less 20.25 dB;
- auxiliary input: -12 dB to 6 dB in 3 dB steps, code 1 to 7, code 0 is mute;
- line output: -57 dB to 6 dB in 1 dB steps, a 6-bit volume and a mute bit.

The fields are read back by name, the value the driver computed, never as wire bytes (A93).

Proofs (ALX-1564, A92): P645-P651.
"""

import pytest

CLOCK = 0x00
NON_CLOCK = [0x08, *range(0x0A, 0x2E), *range(0x2F, 0x32), 0x36]
R5, R7, R8, R9, R31, R32 = 0x0B, 0x0D, 0x0E, 0x0F, 0x25, 0x26
BOOST_0DB, BOOST_20DB = 0b01, 0b10


@pytest.fixture
def codec(adau1961_lib):
    """One codec, initialised over a ready bus."""
    obj = adau1961_lib.new()
    assert adau1961_lib.call(obj, "Init") == adau1961_lib.OK
    return adau1961_lib, obj


def test_ALX1564_P645_init_starts_the_interface_and_writes_each_register_once(adau1961_lib):
    """The I2S interface, then the clock control, then the 41 other registers, each once."""
    obj = adau1961_lib.new()

    assert adau1961_lib.call(obj, "Init") == adau1961_lib.OK

    assert adau1961_lib.i2s_counts()[0] == 1
    assert adau1961_lib.write_count(CLOCK) == 1
    assert len(NON_CLOCK) == 41
    assert [adau1961_lib.write_count(r) for r in NON_CLOCK] == [1] * 41


@pytest.mark.parametrize("right", [False, True], ids=["left", "right"])
@pytest.mark.parametrize(("gain_db", "vol", "boost"), [
    (-12.0, 0, BOOST_0DB),
    (0.0, 16, BOOST_0DB),
    (35.25, 63, BOOST_0DB),         # the top of the volume alone
    (36.0, 37, BOOST_20DB),         # the boost stage, the volume carrying 36 - 20.25
    (55.5, 63, BOOST_20DB),         # the top of both
])
def test_ALX1564_P646_a_differential_input_gain_is_a_volume_and_a_boost(codec, right, gain_db, vol, boost):
    """0.75 dB a code from -12 dB; above 35.25 dB the +20 dB boost and the volume share the gain.

    Two writes each time: the boost register first and the volume register after, so the gain
    never passes through a step louder than the one asked for.
    """
    lib, obj = codec
    boost_reg, vol_reg = (R7, R9) if right else (R5, R8)
    writes = (lib.write_count(boost_reg), lib.write_count(vol_reg))

    assert lib.set_gain(obj, "InDiffR" if right else "InDiffL", gain_db) == lib.OK

    assert lib.in_diff(obj, right=right) == (vol, 1, boost)
    assert (lib.write_count(boost_reg) - writes[0], lib.write_count(vol_reg) - writes[1]) == (1, 1)


def test_ALX1564_P647_a_differential_input_is_muted_by_its_mute_bit_alone(codec):
    """-120 dB is mute: the mute bit cleared, one write of the volume register, the boost untouched."""
    lib, obj = codec
    lib.set_gain(obj, "InDiffL", 0.0)
    writes = (lib.write_count(R5), lib.write_count(R8))

    assert lib.set_gain(obj, "InDiffL", -120.0) == lib.OK

    assert lib.in_diff(obj) == (16, 0, BOOST_0DB), "mute changed more than the mute bit"
    assert (lib.write_count(R5) - writes[0], lib.write_count(R8) - writes[1]) == (0, 1)


@pytest.mark.parametrize("right", [False, True], ids=["left", "right"])
@pytest.mark.parametrize(("gain_db", "code"), [(-12, 1), (-9, 2), (0, 5), (6, 7), (-120, 0)])
def test_ALX1564_P648_an_aux_input_gain_is_a_3_db_step_code_above_mute(codec, right, gain_db, code):
    """Code 1 is -12 dB and each code 3 dB more, up to 6 dB at code 7; code 0 is mute."""
    lib, obj = codec

    assert lib.set_gain(obj, "InAuxR" if right else "InAuxL", gain_db) == lib.OK

    assert lib.in_aux(obj, right=right) == code


@pytest.mark.parametrize("right", [False, True], ids=["left", "right"])
@pytest.mark.parametrize(("gain_db", "vol"), [(-57, 0), (-1, 56), (0, 57), (6, 63)])
def test_ALX1564_P649_a_line_output_gain_is_a_1_db_step_volume(codec, right, gain_db, vol):
    """1 dB a code from -57 dB, the output unmuted, one write of its volume register."""
    lib, obj = codec
    reg = R32 if right else R31
    writes = lib.write_count(reg)

    assert lib.set_gain(obj, "OutLineR" if right else "OutLineL", gain_db) == lib.OK

    assert lib.out_line(obj, right=right) == (vol, 1)
    assert lib.write_count(reg) - writes == 1


def test_ALX1564_P650_a_muted_line_output_keeps_its_volume(codec):
    """-120 dB clears the mute bit and leaves the volume, so unmuting comes back at the same level."""
    lib, obj = codec
    lib.set_gain(obj, "OutLineL", -10)

    assert lib.set_gain(obj, "OutLineL", -120) == lib.OK

    assert lib.out_line(obj) == (47, 0)


@pytest.mark.expect_assert("AlxAdau1961_DeInit is a TODO in the library: it asserts and returns Alx_Err")
def test_ALX1564_P651_deinit_is_not_implemented(codec):
    """CHARACTERIZATION: DeInit asserts and returns the error, and the interface is never stopped.

    Recorded so that implementing it is a visible change: this test then fails and says what DeInit
    has to do - stop I2S, release the bus.
    """
    lib, obj = codec

    assert lib.call(obj, "DeInit") == lib.ERR

    assert lib.i2s_counts()[1] == 0


# =====================================================================
# P804-P807 - the interface and the bus failing at any step, the gains' failing writes, the right-side mutes,
# the foreground handle (A97)
# =====================================================================

# the nth bus call of Init: the bus's Init, the acknowledge, the clock control, then the 41 other registers
INIT_STEPS = tuple(range(1, 45))


def test_ALX1564_P804_init_stops_when_the_interface_will_not_start(adau1961_lib):
    obj = adau1961_lib.new()
    adau1961_lib.i2s_answers(1)

    assert adau1961_lib.call(obj, "Init") == adau1961_lib.ERR
    assert adau1961_lib.bus_calls() == 0, "the bus was touched after the interface failed"


@pytest.mark.parametrize("nth", INIT_STEPS)
def test_ALX1564_P804_init_stops_at_the_bus_call_that_fails(adau1961_lib, nth):
    obj = adau1961_lib.new()
    adau1961_lib.fail_at(nth)

    assert adau1961_lib.call(obj, "Init") == adau1961_lib.ERR
    assert adau1961_lib.bus_calls() == nth


@pytest.mark.parametrize(("what", "gain_db", "nth"), [
    ("InDiffL", -120.0, 1), ("InDiffL", 20.25, 1), ("InDiffL", 20.25, 2), ("InDiffL", 40.5, 1), ("InDiffL", 40.5, 2),
    ("InDiffR", -120.0, 1), ("InDiffR", 20.25, 1), ("InDiffR", 20.25, 2), ("InDiffR", 40.5, 1), ("InDiffR", 40.5, 2),
    ("InAuxL", 0, 1), ("InAuxR", 0, 1), ("OutLineL", 0, 1), ("OutLineR", 0, 1),
])
def test_ALX1564_P805_a_gain_whose_register_write_fails_reports_the_bus(codec, what, gain_db, nth):
    """Every write of every gain path: the mute, the boosted and the unboosted differential, the aux, the line."""
    lib, obj = codec
    lib.fail_at(nth)

    assert lib.set_gain(obj, what, gain_db) == lib.ERR


def test_ALX1564_P806_the_right_differential_input_and_the_right_line_output_mute_like_the_left(codec):
    """P647 and P650 for the right side: the mute bit alone, the volume kept."""
    lib, obj = codec
    assert lib.set_gain(obj, "InDiffR", 20.25) == lib.OK
    assert lib.set_gain(obj, "OutLineR", -6) == lib.OK
    vol_in, _, boost = lib.in_diff(obj, right=True)
    vol_out, _ = lib.out_line(obj, right=True)

    assert lib.set_gain(obj, "InDiffR", -120.0) == lib.OK
    assert lib.set_gain(obj, "OutLineR", -120) == lib.OK

    assert lib.in_diff(obj, right=True) == (vol_in, 0, boost)
    assert lib.out_line(obj, right=True) == (vol_out, 0)


def test_ALX1564_P807_the_foreground_handle_is_the_interfaces(codec):
    lib, obj = codec

    lib.handle(obj)

    assert lib.i2s_counts()[2] == 1
