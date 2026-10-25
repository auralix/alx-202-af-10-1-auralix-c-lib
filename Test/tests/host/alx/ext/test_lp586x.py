"""Auralix C Library - AlxLp586x on the host.

An LED matrix driver for 108 dots, one 8-bit brightness register each, paged behind the I2C
address. The driver keeps the wanted state of every dot; Handle sends only the dots that
changed, as full on (0xFF) or off (0x00), and then pulses SYNC so the part shows every change at
once. Init enables the part, writes its configuration and the brightness of the dots, and pulses
SYNC too.

The I2C fake does not record the I2C address, so a brightness register shares its slot with the
core register of the same low byte. These tests read only the dots above 0x29, where no core
register is written.

Writing these found one defect, sealed as P627: Init writes the brightness of 107 of the 108
dots.

Proofs (ALX-1564, A92): P622-P627.
"""

import pytest

DOT = 60                  # a dot whose register slot no core register shares


@pytest.fixture
def leds(lp586x_lib):
    """One driver for all 108 dots, initialised."""
    obj = lp586x_lib.new()
    assert lp586x_lib.call(obj, "Init") == lp586x_lib.OK
    return lp586x_lib, obj


def test_ALX1564_P622_init_enables_the_part_writes_the_dots_and_pulses_sync(lp586x_lib):
    """EN high, every brightness register written once, and one SYNC pulse that ends low."""
    obj = lp586x_lib.new()

    assert lp586x_lib.call(obj, "Init") == lp586x_lib.OK

    assert lp586x_lib.level(obj, "En") is True
    assert lp586x_lib.level(obj, "Sync") is False
    assert lp586x_lib.pin_writes(obj, "Sync") == 3, "SYNC: reset before the part answers, then one pulse"
    assert lp586x_lib.delays_us() == 1
    assert [lp586x_lib.write_count(d) for d in range(0x2A, 0x6B)] == [1] * (0x6B - 0x2A)


def test_ALX1564_P623_a_dot_is_sent_only_when_it_changed(leds):
    """On is 0xFF, off is 0x00, and a Handle with nothing changed sends no dot at all.

    Sending only the changes is the point of the shadow: the part has 108 registers, and a bus
    that carried all of them on every Handle would be busy for nothing.
    """
    lib, obj = leds
    lib.led(obj, DOT, True)

    lib.call(obj, "Handle")
    assert (lib.write_count(DOT), lib.last_write(DOT, 1)) == (2, b"\xff")

    lib.call(obj, "Handle")
    assert lib.write_count(DOT) == 2, "an unchanged dot was sent again"

    lib.led(obj, DOT, False)
    lib.call(obj, "Handle")
    assert (lib.write_count(DOT), lib.last_write(DOT, 1)) == (3, b"\x00")


def test_ALX1564_P624_every_handle_ends_with_one_sync_pulse(leds):
    """SYNC high, 10 us, low - after the writes, whether or not anything changed."""
    lib, obj = leds
    writes, delays = lib.pin_writes(obj, "Sync"), lib.delays_us()

    lib.call(obj, "Handle")
    lib.call(obj, "Handle")

    assert lib.pin_writes(obj, "Sync") - writes == 4
    assert lib.delays_us() - delays == 2
    assert lib.level(obj, "Sync") is False


def test_ALX1564_P625_deinit_drives_both_pins_low_and_releases_them(leds):
    """The part disabled before its pins are given back: EN low, SYNC low."""
    lib, obj = leds

    assert lib.call(obj, "DeInit") == lib.OK

    assert (lib.level(obj, "En"), lib.level(obj, "Sync")) == (False, False)
    assert (lib.pin_counts(obj, "En"), lib.pin_counts(obj, "Sync")) == ((1, 1), (1, 1))


def test_ALX1564_P626_init_fails_when_the_part_does_not_answer_and_leaves_it_enabled(lp586x_lib):
    """CHARACTERIZATION: no acknowledge, Init reports the error - with EN already driven high.

    The driver enables the part before it asks whether it is there, and does not undo that on the
    way out, so a missing or broken part is left powered.
    """
    obj = lp586x_lib.new()
    lp586x_lib.slave_ready(False)

    assert lp586x_lib.call(obj, "Init") == lp586x_lib.ERR

    assert lp586x_lib.level(obj, "En") is True
    assert sum(lp586x_lib.write_count(d) for d in range(0x2A, 0x6C)) == 0


def test_ALX1564_P627_init_writes_the_brightness_of_all_108_dots(lp586x_lib):
    """Dot 107 is a dot like the others, and Init writes it like the others."""
    obj = lp586x_lib.new()

    assert lp586x_lib.call(obj, "Init") == lp586x_lib.OK

    assert lp586x_lib.write_count(0x6B) == 1


# =====================================================================
# P764-P766 - the bus failing at any step of Init, the bus's own Init and DeInit (A97)
# =====================================================================

# the nth bus call of Init: the acknowledge, then the configuration registers in the order the driver
# writes them - the six core registers, the first of each group, of each current, of each dot's group,
# of each dot's brightness
INIT_STEPS = (2, 3, 4, 5, 6, 7, 8, 11, 14, 44)


@pytest.mark.parametrize("nth", INIT_STEPS)
def test_ALX1564_P764_init_stops_at_the_configuration_write_that_fails(lp586x_lib, nth):
    obj = lp586x_lib.new()
    lp586x_lib.fail_at(nth)

    assert lp586x_lib.call(obj, "Init") == lp586x_lib.ERR


def test_ALX1564_P765_the_bus_that_will_not_initialise_is_reported(lp586x_lib):
    obj = lp586x_lib.new(periph=False)
    lp586x_lib.fail_at(1)

    assert lp586x_lib.call(obj, "InitPeriph") == lp586x_lib.ERR
    assert lp586x_lib.call(obj, "InitPeriph") == lp586x_lib.OK, "and the next attempt works"


def test_ALX1564_P766_deinit_periph_releases_the_bus_and_reports_one_that_will_not(lp586x_lib):
    obj = lp586x_lib.new()
    lp586x_lib.fail_at(1)

    assert lp586x_lib.call(obj, "DeInitPeriph") == lp586x_lib.ERR
    assert lp586x_lib.call(obj, "DeInitPeriph") == lp586x_lib.OK
