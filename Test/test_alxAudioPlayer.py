"""Auralix C Library - AlxAudioPlayer on the host.

A player is a track in memory and a cursor into it. Everything it does is arithmetic on that
cursor: how far to step for the encoding it was given, where the right channel is relative to the
left, what happens at the end of the track, and what the transport controls do to the position.

The tracks below are chosen so that every byte is a different number, which makes a cursor that
lands one byte out say so instead of reading a plausible sample.

Proofs (ALX-1553): P278-P290.
"""

import pytest

# Eight bytes, all different, readable as 8-bit mono (8 samples) or 8-bit stereo (4 L/R pairs).
TRACK = [10, 20, 30, 40, 50, 60, 70, 80]


def _f(code: int) -> float:
    """The float an 8-bit signed sample becomes - the module's own conversion, restated."""
    return code / 128.0


@pytest.fixture
def mono(audio_player_lib):
    """An 8-bit mono player over TRACK, PLAYING: one byte per sample, eight samples.

    Started, because a player that is not playing answers silence whatever the track holds (P290),
    and every test below is about the track rather than about the transport.
    """
    obj = audio_player_lib.new(TRACK, mono=True)
    audio_player_lib.play(obj)
    return audio_player_lib, obj


@pytest.fixture
def stereo(audio_player_lib):
    """An 8-bit stereo player over TRACK, playing: two bytes per sample, four samples."""
    obj = audio_player_lib.new(TRACK, mono=False)
    audio_player_lib.play(obj)
    return audio_player_lib, obj


def test_ALX1553_P278_a_new_player_is_not_playing(audio_player_lib):
    """A player comes up stopped, whatever track it was given.

    It is the same argument as a muted volume control: something that drives a speaker should not
    start making noise because it was constructed.
    """
    obj = audio_player_lib.new(TRACK)

    assert audio_player_lib.is_playing(obj) is False


@pytest.mark.parametrize(("encoding", "is_mono", "step"), [
    ("INT8", True, 1),
    ("INT8", False, 2),
    ("UINT8", True, 1),
    ("UINT8", False, 2),
    ("INT16", True, 2),
    ("INT16", False, 4),
    ("UINT16", True, 2),
    ("UINT16", False, 4),
    ("INT16_FLASH", True, 2),
    ("INT16_FLASH", False, 4),
])
def test_ALX1553_P279_the_step_is_the_width_times_the_channels(audio_player_lib, encoding,
                                                                is_mono, step):
    """One sample is as many bytes as its width times its channel count, and that is the step.

    Ten combinations because the module writes them out as two nested conditions and the flash
    variant of 16-bit is easy to leave out of the second one - which would make a track play at
    half speed and nothing else.
    """
    obj = audio_player_lib.new(TRACK, encoding=getattr(audio_player_lib, encoding), mono=is_mono)

    assert audio_player_lib.step(obj) == step


def test_ALX1553_P280_the_right_channel_is_the_byte_after_the_left(stereo):
    """In a stereo track the two channels are interleaved, left first.

    Reading them the other way round gives a track that plays with its channels swapped - audible
    only if somebody is listening in stereo, and invisible to every other test.
    """
    lib, obj = stereo

    assert lib.left(obj) == pytest.approx(_f(10))
    assert lib.right(obj) == pytest.approx(_f(20))

    lib.advance(obj)
    assert lib.left(obj) == pytest.approx(_f(30))
    assert lib.right(obj) == pytest.approx(_f(40))


def test_ALX1553_P281_a_mono_track_answers_the_same_sample_on_both_channels(mono):
    """A mono track has one channel, and asking for the right one gives it too.

    The module says so in a comment and it matters: a caller that feeds a stereo codec from a mono
    track gets the same sample on both sides rather than the NEXT sample on one of them.
    """
    lib, obj = mono

    assert lib.left(obj) == pytest.approx(_f(10))
    assert lib.right(obj) == pytest.approx(_f(10))
    assert lib.mono(obj) == pytest.approx(_f(10))

    lib.advance(obj)
    assert lib.left(obj) == pytest.approx(_f(20))
    assert lib.right(obj) == pytest.approx(_f(20))


def test_ALX1553_P282_the_mono_sample_of_a_stereo_track_is_the_average(stereo):
    """Mixing down happens here too, and it is the average of the two channels."""
    lib, obj = stereo

    assert lib.mono(obj) == pytest.approx((_f(10) + _f(20)) / 2)

    lib.advance(obj)
    assert lib.mono(obj) == pytest.approx((_f(30) + _f(40)) / 2)


def test_ALX1553_P283_the_cursor_walks_the_track_one_sample_at_a_time(mono):
    """Eight samples, eight positions, in order.

    Asserted through the samples rather than the offset, because what a caller hears is the sample.
    """
    lib, obj = mono
    heard = [lib.left(obj)]
    for _ in range(7):
        lib.advance(obj)
        heard.append(lib.left(obj))

    assert heard == pytest.approx([_f(code) for code in TRACK])


def test_ALX1553_P284_without_looping_the_end_of_the_track_stops_and_rewinds(mono):
    """At the last sample the player stops - and stopping rewinds, so it is ready to play again.

    Two behaviours in one call, and the second is easy to miss: a player that stopped WITHOUT
    rewinding would replay nothing but its last sample forever.
    """
    lib, obj = mono
    lib.loop(obj, on=False)

    lib.advance(obj, times=len(TRACK) - 1)      # now on the last sample
    assert lib.left(obj) == pytest.approx(_f(TRACK[-1]))
    assert lib.is_playing(obj) is True

    lib.advance(obj)

    assert lib.is_playing(obj) is False
    assert lib.offset(obj) == 0, "stopping did not rewind"
    lib.play(obj)
    assert lib.left(obj) == pytest.approx(_f(TRACK[0]))


def test_ALX1553_P285_with_looping_the_track_wraps_and_keeps_playing(mono):
    """With the loop on, the sample after the last is the first, and the player never stops."""
    lib, obj = mono
    lib.loop(obj, on=True)

    lib.advance(obj, times=len(TRACK))

    assert lib.is_playing(obj) is True
    assert lib.left(obj) == pytest.approx(_f(TRACK[0]))

    lib.advance(obj)
    assert lib.left(obj) == pytest.approx(_f(TRACK[1])), "the loop did not continue past the wrap"


def test_ALX1553_P286_pause_keeps_the_position_and_stop_does_not(mono):
    """Pause and Stop differ in exactly one thing, and it is the one a listener notices.

    Both stop the player; only Stop rewinds. A Pause that rewound would make a paused track restart
    from the beginning, which is the classic complaint about a media player.
    """
    lib, obj = mono
    lib.advance(obj, times=3)
    assert lib.left(obj) == pytest.approx(_f(TRACK[3]))

    lib.pause(obj)
    assert lib.is_playing(obj) is False

    lib.play(obj)
    assert lib.left(obj) == pytest.approx(_f(TRACK[3])), "pause moved the cursor"

    lib.stop(obj)
    lib.play(obj)
    assert lib.left(obj) == pytest.approx(_f(TRACK[0])), "stop did not rewind"


def test_ALX1553_P287_replay_starts_the_track_again_from_the_beginning(mono):
    """Replay is Stop and then Play: back to the first sample, and playing."""
    lib, obj = mono
    lib.advance(obj, times=5)

    lib.replay(obj)

    assert lib.is_playing(obj) is True
    assert lib.left(obj) == pytest.approx(_f(TRACK[0]))


def test_ALX1553_P288_loading_a_track_takes_its_encoding_and_its_start_offset(audio_player_lib):
    """A new track brings its own length, encoding, channel count and starting sample.

    The start offset is counted in SAMPLES and turned into bytes with the NEW track's step, then
    wrapped modulo the new length - so a player that kept the old step would land between samples.
    """
    lib = audio_player_lib
    obj = lib.new(TRACK, mono=True)
    lib.play(obj)
    lib.advance(obj, times=5)

    other = [1, 2, 3, 4, 5, 6]
    lib.load(obj, other, mono=False, start_sample=2)     # stereo: step 2, so byte offset 4

    assert lib.step(obj) == 2
    assert lib.offset(obj) == 4
    assert lib.left(obj) == pytest.approx(_f(other[4]))
    assert lib.right(obj) == pytest.approx(_f(other[5]))


def test_ALX1553_P289_a_start_offset_past_the_end_wraps_instead_of_reading_past_it(
    audio_player_lib,
):
    """CHARACTERIZATION: an out-of-range start offset is taken modulo the track, not refused.

    Six bytes of stereo is three samples; asking to start at sample 5 is ten bytes in, which the
    module reduces to four. Nothing rejects it and nothing says it happened - which is safe, in
    that it cannot read past the track, and silent, in that a caller with an off-by-one in its own
    track table hears the wrong part of the track and is told nothing.
    """
    lib = audio_player_lib
    other = [1, 2, 3, 4, 5, 6]

    obj = lib.new(other, mono=False, start_sample=5)      # 5 * 2 = 10 bytes, modulo 6 = 4
    lib.play(obj)

    assert lib.offset(obj) == 4
    assert lib.left(obj) == pytest.approx(_f(other[4]))


def test_ALX1553_P290_a_player_that_is_not_playing_answers_silence(audio_player_lib):
    """Stopped or paused, every channel reads zero - and the cursor stays where it was.

    It is the property that makes a player safe to poll from an audio interrupt: the caller asks
    for a sample every period whether or not anything is playing, and gets silence rather than the
    last one repeated or whatever the track happens to hold.
    """
    lib = audio_player_lib
    obj = lib.new(TRACK, mono=True)

    assert lib.left(obj) == 0.0
    assert lib.right(obj) == 0.0
    assert lib.mono(obj) == 0.0

    lib.play(obj)
    lib.advance(obj, times=3)
    assert lib.left(obj) == pytest.approx(_f(TRACK[3]))

    lib.pause(obj)
    assert lib.left(obj) == 0.0, "a paused player still answered a sample"
    assert lib.offset(obj) == 3, "and pausing moved the cursor"
