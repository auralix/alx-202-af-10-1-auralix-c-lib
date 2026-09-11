"""Auralix C Library - AlxAudioVol on the host.

A volume control is two mappings and a multiply: a percentage becomes decibels on a straight line,
decibels become an amplitude factor through 10^(dB/20), and a sample is multiplied by that factor
and clipped to the range a sample may take. Everything here is the real module over the real
AlxLinFun; nothing is faked, because there is no hardware in it.

The module is small enough to read in a minute and still has four things worth pinning: it starts
MUTED whatever range it was given, zero percent is a special case that does not go through the
decibel map, the factor is the amplitude ratio and not the power ratio, and the output clips rather
than wrapping.

Proofs (ALX-1553): P235-P243.
"""

import math

import pytest

# The range this suite builds most of its objects over: the usual shape of an audio volume control,
# silence at one end and unity at the other.
MIN_DB = -60.0
MAX_DB = 0.0


@pytest.fixture
def vol(audiovol_lib):
    """One volume control over the standard range, muted as the constructor leaves it."""
    return audiovol_lib, audiovol_lib.new(MIN_DB, MAX_DB)


@pytest.mark.expect_assert(
    "P451: a fixed volume, volMin_dB == volMax_dB, gives a line with no slope - AlxLinFun_Ctor asserts and pctMax "
    "becomes NaN"
)
def test_ALX1553_P235_a_new_volume_control_is_muted(audiovol_lib):
    """Whatever range it is given, a volume control starts at zero output.

    It is the only safe default for something that drives a speaker: the alternative is a device
    that comes up at whatever the range's maximum happens to be. The constructor sets the factor to
    zero by hand rather than by mapping a percentage, so this is its own behaviour and not a
    consequence of the range.
    """
    for lo, hi in [(MIN_DB, MAX_DB), (-90.0, 10.0), (-6.0, -6.0)]:
        obj = audiovol_lib.new(lo, hi)

        assert audiovol_lib.process(obj, 1.0) == 0.0
        assert audiovol_lib.process(obj, -1.0) == 0.0


def test_ALX1553_P236_zero_decibels_passes_a_sample_through_unchanged(vol):
    """0 dB is unity gain, so Process is the identity on every sample.

    10^(0/20) is 1, which is worth asserting rather than assuming: a decibel formula written with
    the power ratio, 10^(dB/10), gives 1 here too and differs everywhere else (P237).
    """
    lib, obj = vol
    lib.set_dB(obj, 0.0)

    for sample in (-1.0, -0.5, -0.125, 0.0, 0.125, 0.5, 1.0):
        assert lib.process(obj, sample) == pytest.approx(sample, abs=1e-6)


@pytest.mark.parametrize(("vol_dB", "factor"), [
    (0.0, 1.0),
    (-6.0, 0.501187),      # about a half: the familiar "6 dB is half the amplitude"
    (-20.0, 0.1),          # a tenth, exactly, which is what makes 20 dB the decade
    (-40.0, 0.01),
    (-60.0, 0.001),
])
def test_ALX1553_P237_the_factor_is_the_amplitude_ratio(vol, vol_dB, factor):  # noqa: N803 - the unit belongs in the name
    """The gain is 10^(dB/20) - the AMPLITUDE ratio, not the power ratio.

    The difference is not academic: at -20 dB the amplitude formula gives 0.1 and the power formula
    gives 0.01, so a sample would come out ten times too quiet. Both formulas agree at 0 dB, which
    is exactly why P236 alone would not catch it.
    """
    lib, obj = vol
    lib.set_dB(obj, vol_dB)

    assert lib.process(obj, 1.0) == pytest.approx(factor, rel=1e-4)
    assert lib.process(obj, 1.0) == pytest.approx(10.0 ** (vol_dB / 20.0), rel=1e-6)


def test_ALX1553_P238_the_sign_of_a_sample_survives_the_volume(vol):
    """A negative sample stays negative: this is a gain, not a rectifier."""
    lib, obj = vol
    lib.set_dB(obj, -20.0)

    assert lib.process(obj, -0.5) == pytest.approx(-0.05, rel=1e-4)
    assert lib.process(obj, 0.5) == pytest.approx(0.05, rel=1e-4)


def test_ALX1553_P239_the_output_clips_instead_of_running_past_full_scale(audiovol_lib):
    """A gain above unity is allowed, and the output is held at full scale rather than exceeding it.

    A sample is a fraction of full scale; letting it past 1.0 is what makes a converter wrap and a
    speaker click. The clip is symmetric, so the negative end is asserted too.
    """
    obj = audiovol_lib.new(-60.0, 20.0)     # a range that CAN amplify
    audiovol_lib.set_dB(obj, 20.0)          # ten times

    assert audiovol_lib.process(obj, 1.0) == 1.0
    assert audiovol_lib.process(obj, -1.0) == -1.0
    assert audiovol_lib.process(obj, 0.05) == pytest.approx(0.5, rel=1e-4)


@pytest.mark.expect_assert(
    "P450: pctMax is the inverse of the line at its own maximum and comes out 99.99999237 on an ordinary range, so "
    "setting 100 percent asserts"
)
def test_ALX1553_P240_the_percentage_maps_linearly_onto_the_decibel_range(vol):
    """Percent is a straight line onto the decibel range, so 50 % is the midpoint IN DECIBELS.

    That is a decision, not a law: a volume control that is linear in percent is not linear to the
    ear, and halfway on the dial is -30 dB here, which is about a thirtieth of full amplitude. It is
    asserted so that a later change to a logarithmic taper has to be deliberate.
    """
    lib, obj = vol

    lib.set_pct(obj, 100.0)
    assert lib.process(obj, 1.0) == pytest.approx(1.0, rel=1e-5)          # MAX_DB = 0 dB

    lib.set_pct(obj, 50.0)
    assert lib.process(obj, 1.0) == pytest.approx(10.0 ** (-30.0 / 20.0), rel=1e-4)

    lib.set_pct(obj, 25.0)
    assert lib.process(obj, 1.0) == pytest.approx(10.0 ** (-45.0 / 20.0), rel=1e-4)


def test_ALX1553_P241_zero_percent_is_silence_and_not_the_lowest_decibel(vol):
    """0 % mutes exactly, and that needs its own branch in the module.

    Through the decibel map 0 % is MIN_DB, and 10^(-60/20) is 0.001 - quiet, but not silent, and it
    never becomes silent however low the range goes. The module special-cases zero, so "off" is off.
    """
    lib, obj = vol

    lib.set_pct(obj, 0.0)
    assert lib.process(obj, 1.0) == 0.0

    # the smallest percentage that is NOT zero is still the quiet-but-audible value
    lib.set_pct(obj, 0.001)
    assert lib.process(obj, 1.0) > 0.0
    assert lib.process(obj, 1.0) == pytest.approx(10.0 ** (MIN_DB / 20.0), rel=1e-2)


def test_ALX1553_P242_the_accepted_percentage_range_comes_from_the_decibel_range(audiovol_lib):
    """CHARACTERIZATION: what the constructor decides a percentage may be.

    Set_pct asserts against pctMax, which the constructor computes by asking the line where the
    maximum decibel value lives. For a range whose top is the line's own endpoint that is 100;
    recorded here because a test that guessed 100 would be testing its guess, and because the
    number moves with the range.
    """
    assert audiovol_lib.pct_max(audiovol_lib.new(MIN_DB, MAX_DB)) == pytest.approx(100.0)
    assert audiovol_lib.pct_max(audiovol_lib.new(-90.0, 10.0)) == pytest.approx(100.0)


@pytest.mark.expect_assert(
    "P450: pctMax is the inverse of the line at its own maximum and comes out 99.99999237 on an ordinary range, so "
    "setting 100 percent asserts"
)
def test_ALX1553_P243_setting_the_volume_twice_does_not_accumulate(vol):
    """The volume is set, not adjusted: the second call replaces the first.

    A gain that multiplied instead of replacing would pass every single-call test above and be
    wrong the first time a user turned the dial twice.
    """
    lib, obj = vol

    lib.set_dB(obj, -20.0)
    lib.set_dB(obj, -40.0)
    assert lib.process(obj, 1.0) == pytest.approx(0.01, rel=1e-4)

    lib.set_pct(obj, 100.0)
    lib.set_pct(obj, 50.0)
    assert lib.process(obj, 1.0) == pytest.approx(math.pow(10.0, -30.0 / 20.0), rel=1e-4)


# =====================================================================
# P450, P451 - two ranges the module cannot express
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the volume cannot be set to 100 percent on an ordinary range. pctMax is computed in "
    "the constructor as AlxLinFun_GetX(line, volMax_dB) - the inverse of the line at its own "
    "maximum - and float arithmetic does not round-trip, so for the common -60 dB to 0 dB range it "
    "comes out 99.99999237 rather than 100. AlxAudioVol_Set_pct then asserts on "
    "(vol_pct <= me->pctMax), which means a caller asking for full volume - the single most "
    "ordinary thing to ask a volume control - trips an assertion, and on a product that enables "
    "them that RESETS THE MCU. The value is the constructor's own idea of its maximum, so nothing "
    "a caller can read tells it to ask for 99.99999 instead. The fix is to keep the percentage "
    "range as the 0..100 it was built from rather than rediscovering it through the inverse"))
def test_ALX1553_P450_full_volume_can_be_asked_for_by_name(audiovol_lib):
    """100 percent, on the range the rest of this file uses."""
    lib = audiovol_lib
    obj = lib.new(MIN_DB, MAX_DB)
    lib.c.AlxAssertPc_Reset()

    lib.set_pct(obj, 100.0)

    assert lib.c.AlxAssertPc_Count() == 0, (
        f"asking for full volume asserted; pctMax is {lib.pct_max(obj)!r}"
    )


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: a FIXED volume cannot be constructed. Passing the same value for volMin_dB and "
    "volMax_dB - a control that does not vary, which is a reasonable thing for a product to ask "
    "for - builds a line whose two points share a y, and AlxLinFun_Ctor asserts on it. pctMax then "
    "comes out NaN, so every later Set_pct asserts too, because no percentage compares <= NaN. "
    "Nothing in AlxAudioVol's own interface says the two decibel values have to differ, and the "
    "failure is an assertion rather than a status. The fix is either to reject the range in "
    "AlxAudioVol_Ctor with something a caller can see, or to special-case a flat range as one "
    "whose percentage maps to a constant"))
def test_ALX1553_P451_a_fixed_volume_range_is_a_range(audiovol_lib):
    """The same decibel value for both ends - a control that does not vary."""
    lib = audiovol_lib
    lib.c.AlxAssertPc_Reset()

    obj = lib.new(-6.0, -6.0)

    assert lib.c.AlxAssertPc_Count() == 0, "constructing a fixed volume asserted"
    assert lib.pct_max(obj) == lib.pct_max(obj), "pctMax is NaN"
