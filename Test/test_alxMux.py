"""Auralix C Library - AlxMux on the host.

An analog multiplexer, as a board uses one: an enable pin and up to eight select pins, driven by a
channel number. It is what lets a board measure more signals than it has converter channels - the
select pins choose one signal, the converter reads it, and the code walks on to the next.

That makes it small and load-bearing at the same time. Nothing about a reading says which channel
it came from, so a select code driven wrong reports one signal under another signal's name, and
every consumer downstream believes it.

Everything here is asserted through what the module DRIVES, over the library's own IO pin fake -
the pin levels, and how often each pin was initialised - rather than through anything it stores.

One thing a reader should know first: `AlxMux_Enable(me, true)` turns the multiplexer OFF (P414,
sealed).

Proofs (ALX-1553): P412-P419.
"""

import pytest

SEL_PINS = 4                                  # a 16-channel part, the common size
CHANNELS = 2 ** SEL_PINS


@pytest.fixture
def mux(mux_lib):
    """A 16-channel multiplexer, constructed and initialised."""
    return mux_lib, mux_lib.new(SEL_PINS)


def test_ALX1553_P412_the_select_pins_spell_the_channel_number_in_binary(mux):
    """All sixteen channels, each driving the select pins to its own bit pattern, pin 0 the lowest.

    Exhaustive because the whole module is this one loop and every code has to be right: a mux is
    used by walking it, so a single transposed pair of pins swaps two signals for the life of the
    board - and both readings stay perfectly plausible, because they ARE readings, just of each
    other's channel.
    """
    lib, handle = mux

    wrong = []
    for ch in range(CHANNELS):
        lib.select(handle, ch)
        if lib.code(handle, SEL_PINS) != ch:
            wrong.append((ch, lib.code(handle, SEL_PINS)))

    assert wrong == [], "channels whose select pins did not spell their own number"


def test_ALX1553_P413_selecting_a_channel_does_not_touch_the_enable_pin(mux):
    """Walking the channels leaves the enable pin exactly as it was, whichever way it was left.

    The two are separate on purpose - a board settles the select lines, then enables, then reads -
    and a Select that also wrote the enable pin would glitch the output through every intermediate
    code on the way to the one that was asked for.
    """
    lib, handle = mux

    for level in (False, True):
        lib.enable(handle, level)
        for ch in range(CHANNELS):
            lib.select(handle, ch)
            assert lib.en_level(handle) is level, f"selecting channel {ch} moved the enable pin"


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxMux_Enable writes its `enable` argument straight to the pin, and the pin the "
    "constructor takes is named do_nEN - an ACTIVE LOW enable. So AlxMux_Enable(me, true) drives "
    "nEN high and turns the multiplexer OFF, and AlxMux_Enable(me, false) turns it on. The "
    "argument means the opposite of its own name, in the one function whose entire job is that "
    "pin. A caller reading the header cannot get it right by reading it, and a caller that does "
    "get it right has code that says the opposite of what it does. The fix is one character - "
    "write !enable - or, if the level is meant to be the caller's business, rename the argument "
    "and the function so the header stops promising something else"))
def test_ALX1553_P414_enabling_the_multiplexer_turns_it_on(mux):
    """Enable(true) should make the part conduct, which on an active-low enable pin means driving low."""
    lib, handle = mux

    lib.enable(handle, True)

    assert lib.en_level(handle) is False, "Enable(true) drove the active-low enable pin HIGH"


def test_ALX1553_P415_the_enable_pin_follows_the_argument_it_was_given(mux):
    """CHARACTERIZATION: whatever it MEANS, the argument reaches the pin unchanged, both ways.

    This is the behaviour to write code against while P414 stands, and the test that has to change
    when it is fixed. Both directions, because a function that always drove the pin low would pass
    a one-way check and leave a multiplexer that can never be switched off.
    """
    lib, handle = mux

    lib.enable(handle, True)
    assert lib.en_level(handle) is True

    lib.enable(handle, False)
    assert lib.en_level(handle) is False


@pytest.mark.parametrize("sel_pins", [1, 2, 3, 4, 8])
def test_ALX1553_P416_a_channel_beyond_the_pins_is_silently_truncated(mux_lib, sel_pins):
    """CHARACTERIZATION: a code too big for the pins wraps, quietly, into a channel that exists.

    The select loop walks the pins it has and reads the bit under each, so bits above the last pin
    are simply never looked at. A two-pin multiplexer asked for channel 6 selects channel 2, and
    says nothing - which matters because the channel number is usually an index into a table of
    signals, and a table that outgrew the hardware reads the WRONG SIGNAL rather than failing.

    Checked at every pin count the module allows, since the truncation point is the pin count.
    """
    lib = mux_lib
    handle = lib.new(sel_pins)
    channels = 2 ** sel_pins

    for ch in range(channels, 3 * channels):
        lib.select(handle, ch)

        assert lib.code(handle, sel_pins) == ch % channels, f"channel {ch} did not wrap"


def test_ALX1553_P417_init_and_deinit_take_the_enable_pin_and_every_select_pin(mux):
    """Init initialises all of them once; DeInit de-initialises all of them once.

    A select pin that was never initialised still answers a write on this fake and would on a real
    port too, right up to the first time the port's clock is gated - which is why the count is
    checked rather than the level.
    """
    lib, handle = mux
    pins = [lib.en_pin(handle), *[lib.sel_pin(handle, i) for i in range(SEL_PINS)]]

    assert [lib.init_count(p) for p in pins] == [1] * len(pins)
    assert [lib.deinit_count(p) for p in pins] == [0] * len(pins)

    lib.deinit(handle)

    assert [lib.deinit_count(p) for p in pins] == [1] * len(pins)


def test_ALX1553_P418_deinit_select_releases_the_select_pins_and_keeps_the_object_usable(mux):
    """CHARACTERIZATION: the half-DeInit for boards where several multiplexers share select pins.

    The module's own comment says what it is for: one set of select pins, a separate enable pin per
    part. Releasing the select pins lets another multiplexer's driver claim them while this one
    keeps its enable.

    What the comment does not say is that it leaves `isInit` TRUE, so the object still accepts
    Select and Enable afterwards and still drives pins it has just released - and that a later full
    DeInit de-initialises those select pins a SECOND time. Neither is harmful on a GPIO, and both
    are surprising enough to be worth writing down: this is a release of the pins, not of the
    object.
    """
    lib, handle = mux
    sel = [lib.sel_pin(handle, i) for i in range(SEL_PINS)]
    en = lib.en_pin(handle)

    lib.deinit_select(handle)

    assert [lib.deinit_count(p) for p in sel] == [1] * SEL_PINS
    assert lib.deinit_count(en) == 0, "the half-DeInit released the enable pin too"

    lib.select(handle, 3)
    assert lib.code(handle, SEL_PINS) == 3, "the object refused to drive released pins"

    lib.deinit(handle)
    assert [lib.deinit_count(p) for p in sel] == [2] * SEL_PINS
    assert lib.deinit_count(en) == 1


def test_ALX1553_P419_the_pin_fake_says_so_when_it_runs_out_of_slots(mux_lib):
    """The suite's own guard: more pins than the fake has slots must be LOUD, not folded silently.

    The fake tells pins apart by address and has a fixed number of slots. Past that it puts the
    extra pins on the last slot, where two pins answer each other's level - which is not a failure
    but a wrong answer, and it cost an hour of reading a correct module looking for the defect that
    made an eight-select-pin multiplexer report channel 128 as channel 192. Every fixture that
    drives pins now asserts the flag after its test; this is the test that the flag still works.

    It deliberately overflows the fake and then resets it, so the fixture's own check at teardown
    sees a clean slate - the only test in the suite that is allowed to.
    """
    lib = mux_lib
    assert not lib.c.AlxIoPinFake_DidOverflow(), "the fake started a test already overflowed"

    made = 0
    while not lib.c.AlxIoPinFake_DidOverflow():
        handle = lib.new(8, init=False)         # 8 select pins and an enable pin each
        lib.select(handle, 0xFF)                # touch every one of them, so each takes a slot
        made += 1
        assert made < 64, "the fake never reported running out of slots"

    assert lib.c.AlxIoPinFake_DidOverflow() is True
    lib.c.AlxIoPinFake_Reset()
    assert not lib.c.AlxIoPinFake_DidOverflow(), "a reset did not clear the overflow flag"
