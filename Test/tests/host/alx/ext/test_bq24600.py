"""Auralix C Library - AlxBq24600 on the host.

A battery charger that reports what it is doing on two open-drain pins: STAT and nPG (power good,
active low). Three of the four statuses are levels - STAT low is charging, STAT high with power
good is a full battery, STAT high without it is sleep - and the fourth is a frequency: STAT
blinking at about 1 Hz is a fault. The driver tells that one apart by counting the STAT transitions
whose half period falls inside a window around 1 s, over a 4 s detection window, and it reports a
status only after it has held for a while.

The detection times are read back from the driver rather than restated here: they are the part's
specification, and a test that copied them would be testing its own copy.

Proofs (ALX-1564, A92): P576-P584.
"""

import pytest

POWER_GOOD = False      # nPG is active low: a low pin means the adapter is good
NO_POWER = True


@pytest.fixture
def charger(bq24600_lib):
    """One charger, initialised, every pin low."""
    obj = bq24600_lib.new()
    return bq24600_lib, obj


def _stable_ms(lib, obj) -> int:
    """How long a status must hold before the driver reports it: the blink's longest half period."""
    return lib.half_period_ms(obj)[1]


def test_ALX1564_P576_init_claims_the_three_pins_and_deinit_releases_them(bq24600_lib):
    """The charger owns three pins - enable, power good and status - and gives all three back.

    Counted rather than read as levels, because a de-initialised pin still answers a read.
    """
    obj = bq24600_lib.new()
    assert bq24600_lib.pin_counts(obj) == {"en": (1, 0), "npg": (1, 0), "stat": (1, 0)}

    bq24600_lib.deinit(obj)

    assert bq24600_lib.pin_counts(obj) == {"en": (1, 1), "npg": (1, 1), "stat": (1, 1)}


def test_ALX1564_P577_enable_and_disable_drive_the_enable_pin(charger):
    """Enable and Disable are the whole of the driver's output, and both reach the pin."""
    lib, obj = charger

    lib.enable(obj)
    assert lib.en_level(obj) is True

    lib.disable(obj)
    assert lib.en_level(obj) is False


@pytest.mark.parametrize(("stat", "npg", "expected"), [
    (False, POWER_GOOD, "charging"),
    (False, NO_POWER, "charging"),     # STAT low wins whatever power good says
    (True, POWER_GOOD, "full"),
    (True, NO_POWER, "sleep"),
])
def test_ALX1564_P578_each_pin_level_is_one_status_once_it_has_held(charger, stat, npg, expected):
    """The level statuses, each reported only after it has held for the stable time.

    Before that the driver reports none of them: a status seen for one reading is not believed,
    because a blinking STAT pin passes through both levels on its way to being a fault.
    """
    lib, obj = charger
    lib.set_pins(obj, stat=stat, npg=npg)

    lib.handle(obj, for_ms=_stable_ms(lib, obj) - 50)
    assert lib.status(obj) == "none", "a status was reported before it had held"

    lib.handle(obj, for_ms=100)
    assert lib.status(obj) == expected


def test_ALX1564_P579_a_stat_pin_blinking_at_one_hertz_is_an_error(charger):
    """The fault: STAT toggling every second, counted over one detection window.

    The part blinks STAT on a fault (a timer expiry, a battery that will not charge), and both of
    its levels are ordinary statuses, so only the frequency tells a fault from a battery that is
    charging one second and full the next. Run for two windows: the first counts the
    transitions, the end of it decides, and the error then has to hold like any other status.
    """
    lib, obj = charger
    assert lib.half_period_ms(obj)[0] <= 1000 <= lib.half_period_ms(obj)[1]

    lib.blink(obj, half_period_ms=1000, for_ms=2 * lib.window_ms(obj), npg=POWER_GOOD)

    assert lib.status(obj) == "error"


@pytest.mark.parametrize(("half_period_ms", "counted"), [
    (880, False),
    (920, True),
    (1080, True),
    (1120, False),
])
def test_ALX1564_P580_only_a_blink_near_one_hertz_is_counted(charger, half_period_ms, counted):
    """The half period has to fall inside the part's blink window for a transition to count.

    Both edges of the window, from inside and from outside: a driver that counted every transition
    would call a fast-toggling STAT a fault, and one that counted none would never see the fault.
    """
    lib, obj = charger
    lo, hi = lib.half_period_ms(obj)
    assert (lo <= half_period_ms <= hi) is counted

    lib.blink(obj, half_period_ms=half_period_ms, for_ms=2 * lib.window_ms(obj) + 2000,
              npg=POWER_GOOD)

    assert (lib.status(obj) == "error") is counted


def test_ALX1564_P581_a_stat_pin_toggling_fast_is_never_an_error(charger):
    """CHARACTERIZATION: a STAT pin toggling every 200 ms is not a fault, and it is no status at all.

    None of its transitions has a fault's half period, so none is counted; and no level holds for
    the stable time, so the driver keeps reporting what it reported before - here, nothing.
    """
    lib, obj = charger

    lib.blink(obj, half_period_ms=200, for_ms=3 * lib.window_ms(obj), npg=POWER_GOOD)

    assert lib.status(obj) == "none"


def test_ALX1564_P582_losing_power_good_restarts_the_blink_count(charger):
    """Without power good the count starts again on every reading, so a blink is never a fault.

    The part blinks STAT on a fault only while it has an input; without one, its STAT pin is
    whatever the pull-up makes of it, and counting those edges would report a fault for a charger
    that is merely unplugged.
    """
    lib, obj = charger

    lib.blink(obj, half_period_ms=1000, for_ms=3 * lib.window_ms(obj), npg=NO_POWER)

    assert lib.status(obj) != "error"


def test_ALX1564_P583_the_error_clears_once_the_blinking_stops(charger):
    """A fault that goes away is reported as gone, one detection window and a stable time later."""
    lib, obj = charger
    lib.blink(obj, half_period_ms=1000, for_ms=2 * lib.window_ms(obj), npg=POWER_GOOD)
    assert lib.status(obj) == "error"

    lib.set_pins(obj, stat=False, npg=POWER_GOOD)
    lib.handle(obj, for_ms=lib.window_ms(obj) + _stable_ms(lib, obj) + 100)

    assert lib.status(obj) == "charging"


def test_ALX1564_P584_the_detection_times_are_the_parts_blink_specification(charger):
    """CHARACTERIZATION: a 4 s window, 3 transitions, a half period between 900 ms and 1100 ms.

    Recorded because every other proof here is written against them: the part blinks STAT at
    1 Hz, and three transitions inside four seconds is one and a half periods of that blink.
    """
    lib, obj = charger

    assert lib.window_ms(obj) == 4000
    assert lib.err_transitions(obj) == 3
    assert lib.half_period_ms(obj) == (900, 1100)
