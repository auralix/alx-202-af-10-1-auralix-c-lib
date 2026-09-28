"""Auralix C Library - AlxAudio on the host.

The sample conversions: linear PCM in four widths to a float between -1 and 1, and back again. No
objects, no clock, no hardware - seven functions, and the place where an audio path either keeps
its full scale or quietly loses a count.

The asymmetry that runs through all of it: TO float divides by the NEGATIVE full scale (128, 32768)
and FROM float multiplies by the POSITIVE one (127, 32767). That is the usual convention and it is
not symmetric. Put together with the truncation on the way back, the round trip is the identity for
silence and for nothing else: P274 measures that exhaustively rather than leaving it to be found in
a product.

Proofs (ALX-1553): P268-P277.
"""

from itertools import pairwise

import pytest


# ------------------------------------------------------------ PCM to float ------
@pytest.mark.parametrize(("sample", "expected"), [
    (-128, -1.0),          # negative full scale, exactly
    (-64, -0.5),
    (0, 0.0),
    (64, 0.5),
    (127, 127 / 128),      # positive full scale is one count SHORT of 1.0
])
def test_ALX1553_P268_a_signed_eight_bit_sample_becomes_a_float(audio_lib, sample, expected):
    """An 8-bit sample divided by 128: -128 is exactly -1, and +127 never reaches +1.

    The gap at the top is the convention, not a defect - there is one more negative code than
    positive one - but a test that only checked 0 and -128 would never show it.
    """
    assert audio_lib.int8_to_float(sample) == pytest.approx(expected, abs=1e-7)


@pytest.mark.parametrize(("sample", "expected"), [
    (0, -1.0),             # unsigned zero is the bottom of the range
    (64, -0.5),
    (128, 0.0),            # mid-scale is silence
    (192, 0.5),
    (255, 127 / 128),
])
def test_ALX1553_P269_an_unsigned_eight_bit_sample_is_the_same_curve_offset(audio_lib, sample,
                                                                            expected):
    """Unsigned PCM is the signed curve with its zero at mid-scale.

    128 is silence, not 0 - which is the difference between a working converter and a very loud
    click, and the module does it by subtracting 128 and reinterpreting the bits.
    """
    assert audio_lib.uint8_to_float(sample) == pytest.approx(expected, abs=1e-7)


@pytest.mark.parametrize(("sample", "expected"), [
    (-32768, -1.0),
    (-16384, -0.5),
    (0, 0.0),
    (16384, 0.5),
    (32767, 32767 / 32768),
])
def test_ALX1553_P270_a_signed_sixteen_bit_sample_becomes_a_float(audio_lib, sample, expected):
    """The same rule at sixteen bits: divide by 32768, and +32767 is one count short of 1.0."""
    assert audio_lib.int16_to_float(sample) == pytest.approx(expected, abs=1e-9)


@pytest.mark.parametrize(("sample", "expected"), [
    (0, -1.0),
    (32768, 0.0),
    (65535, 32767 / 32768),
])
def test_ALX1553_P271_an_unsigned_sixteen_bit_sample_is_the_same_curve_offset(audio_lib, sample,
                                                                              expected):
    """And unsigned at sixteen bits: 32768 is silence."""
    assert audio_lib.uint16_to_float(sample) == pytest.approx(expected, abs=1e-9)


# ------------------------------------------------------------ float to PCM ------
@pytest.mark.parametrize(("sample", "expected"), [
    (1.0, 127),            # positive full scale
    (-1.0, -127),          # NOT -128: the same 127 is used both ways
    (0.0, 0),
    (0.5, 63),             # 63.5 truncated toward zero
    (-0.5, -63),           # and -63.5 truncated toward zero as well
])
def test_ALX1553_P272_a_float_becomes_a_signed_eight_bit_sample(audio_lib, sample, expected):
    """Multiplied by 127 and truncated toward zero - not rounded, and not scaled by 128.

    Truncation is worth stating: 0.5 is exactly halfway to 64 counts and comes out as 63, so a
    signal converted this way is very slightly quieter than the one that went in.
    """
    assert audio_lib.float_to_int8(sample) == expected


@pytest.mark.parametrize(("sample", "expected"), [
    (1.0, 32767),
    (-1.0, -32767),
    (0.0, 0),
    (0.5, 16383),          # 16383.5 truncated
])
def test_ALX1553_P273_a_float_becomes_a_signed_sixteen_bit_sample(audio_lib, sample, expected):
    """The same at sixteen bits: times 32767, truncated toward zero."""
    assert audio_lib.float_to_int16(sample) == expected


# ------------------------------------------------------------- the round trip ---
def test_ALX1553_P274_a_sample_through_a_float_and_back_loses_one_count(audio_lib):
    """CHARACTERIZATION: the round trip is the identity for silence and for NOTHING else.

    Measured exhaustively over all 256 8-bit codes: one survives, and it is zero. Every other code
    comes back one count closer to zero - 127 of them up, 128 of them down.

    The arithmetic is not subtle once both halves are written down. TO float divides by 128; FROM
    float multiplies by 127 and truncates toward zero. So a code x becomes trunc(x * 127 / 128),
    which is x - 1 in magnitude for every |x| >= 1. The two scales are the usual convention on
    their own; it is the truncation that turns the mismatch into a guaranteed loss rather than an
    occasional one.

    What it means in a product: a path that decodes to float, does something, and encodes again
    attenuates by one count each time it does so, whatever the something was. Two conversions cost
    two counts. At 8 bits that is 1/128 of full scale per pass.
    """
    losses = {}
    for code in range(-128, 128):
        back = audio_lib.float_to_int8(audio_lib.int8_to_float(code))
        if back != code:
            losses[code] = back - code

    assert losses.get(0) is None, "silence survives"
    assert len(losses) == 255, "every code but zero moved"
    assert set(losses.values()) == {-1, 1}, "and each moved by exactly one count"
    assert all(shift == 1 for code, shift in losses.items() if code < 0)
    assert all(shift == -1 for code, shift in losses.items() if code > 0)


@pytest.mark.parametrize(("code", "expected"), [
    (-32768, -32767),
    (-32767, -32766),
    (-1, 0),          # the smallest non-zero sample becomes silence
    (0, 0),
    (1, 0),
    (32767, 32766),
])
def test_ALX1553_P275_the_sixteen_bit_round_trip_loses_a_count_the_same_way(audio_lib, code,
                                                                            expected):
    """The same one-count loss at sixteen bits, including at plus and minus one.

    A sample of +/-1 is the quietest thing a 16-bit stream can carry and it comes back as silence,
    which is the clearest statement of what the round trip does: it does not scale, it subtracts.
    """
    assert audio_lib.float_to_int16(audio_lib.int16_to_float(code)) == expected


# ----------------------------------------------------------------- mixing -------
@pytest.mark.parametrize(("left", "right", "expected"), [
    (1.0, 1.0, 1.0),        # the same signal on both channels comes out unchanged
    (-1.0, -1.0, -1.0),
    (1.0, -1.0, 0.0),       # and opposite signals cancel exactly
    (0.5, 0.0, 0.25),
    (0.0, 0.0, 0.0),
])
def test_ALX1553_P276_stereo_becomes_mono_by_averaging(audio_lib, left, right, expected):
    """Mono is the average of the two channels, not their sum.

    The sum would overflow full scale the moment both channels were loud, which is the classic way
    a mono downmix clips; the average cannot. That two identical channels come out unchanged is the
    other half of the property, and the reason it has to be the average and not half of one.
    """
    assert audio_lib.stereo_to_mono(left, right) == pytest.approx(expected, abs=1e-7)


def test_ALX1553_P277_every_eight_bit_code_converts_monotonically(audio_lib):
    """CHARACTERIZATION: the whole 8-bit range, in order, with no step out of place.

    Exhaustive because it costs nothing at this width: 256 codes, each one strictly greater than
    the one below it, and each 1/128 apart. A conversion table with a sign error or a wrapped
    subtraction shows here as a jump, and nowhere else until a speaker makes the noise.
    """
    signed = [audio_lib.int8_to_float(code) for code in range(-128, 128)]
    unsigned = [audio_lib.uint8_to_float(code) for code in range(256)]

    assert signed == sorted(signed)
    assert unsigned == sorted(unsigned)
    assert signed == pytest.approx(unsigned, abs=1e-7), "signed and unsigned are the same curve"
    assert all(b - a == pytest.approx(1 / 128, abs=1e-7) for a, b in pairwise(signed))
