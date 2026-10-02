"""Auralix C Library - AlxTpa3255 on the host.

A class-D amplifier with one control and two alarms. nRESET low holds the part in reset;
nFAULT low says it has shut itself down (over-current, under-voltage, over-temperature), and
nCLIP_OTW low warns of clipping or an approaching over-temperature. A shut-down part stays down
until it is reset, so the driver recovers it in time: it waits the re-enable time the product
chose, pulses nRESET low, waits for the part to settle, and lets it run - and does it again for as
long as the fault is there.

The pulse and the settle time are read back from the driver rather than restated here.

Proofs (ALX-1564, A92): P589-P596.
"""

import itertools

import pytest

STEP_MS = 0.5
RE_ENABLE_MS = 100.0


@pytest.fixture
def amp(tpa3255_lib):
    """One amplifier, initialised with a 100 ms re-enable wait, healthy and running."""
    obj = tpa3255_lib.new(RE_ENABLE_MS)
    tpa3255_lib.handle(obj, for_ms=5, step_ms=STEP_MS)
    return tpa3255_lib, obj


def _falling_edges(levels: list[bool]) -> int:
    return sum(1 for a, b in itertools.pairwise(levels) if a and not b)


def test_ALX1564_P589_init_claims_the_three_pins_and_deinit_releases_them(tpa3255_lib):
    """The amplifier owns three pins - reset, fault and warning - and gives all three back."""
    obj = tpa3255_lib.new(RE_ENABLE_MS)
    assert tpa3255_lib.pin_counts(obj) == {"nreset": (1, 0), "nfault": (1, 0), "nclip_otw": (1, 0)}

    tpa3255_lib.deinit(obj)

    assert tpa3255_lib.pin_counts(obj) == {"nreset": (1, 1), "nfault": (1, 1), "nclip_otw": (1, 1)}


def test_ALX1564_P590_enable_and_disable_drive_the_reset_pin(amp):
    """Enable releases the part (nRESET high), Disable holds it in reset (nRESET low)."""
    lib, obj = amp

    lib.disable(obj)
    assert lib.reset_level(obj) is False

    lib.enable(obj)
    assert lib.reset_level(obj) is True


@pytest.mark.parametrize(("fault", "warning"), [(False, False), (True, False), (False, True), (True, True)])
def test_ALX1564_P591_both_alarms_are_active_low_and_independent(amp, fault, warning):
    """A low nFAULT is the error and a low nCLIP_OTW the warning, each on its own pin."""
    lib, obj = amp
    lib.set_alarms(obj, fault=fault, warning=warning)

    assert lib.err(obj) is fault
    assert lib.warning(obj) is warning


def test_ALX1564_P592_a_healthy_amplifier_is_released_on_every_handle(amp):
    """CHARACTERIZATION: with the re-enable on, the driver owns nRESET while the part is healthy.

    Every Handle in the running state writes nRESET high, so an application's Disable lasts only
    until the next Handle: muting the amplifier through Disable needs the re-enable off (P596).
    """
    lib, obj = amp
    lib.disable(obj)
    assert lib.reset_level(obj) is False

    lib.handle(obj)

    assert lib.reset_level(obj) is True, "the next Handle did not release the part again"


def test_ALX1564_P593_a_fault_is_answered_with_one_reset_pulse_after_the_re_enable_wait(amp):
    """The part shut itself down: wait the re-enable time, pulse nRESET low, release it again.

    Waiting first is the point - a part that faults on an over-temperature needs time before it is
    asked to run again, and a driver that reset it at once would cycle it into the same fault. The
    reset clears the part's fault, which is what clearing nFAULT at the pulse models.
    """
    lib, obj = amp
    lib.set_alarms(obj, fault=True, warning=False)
    waiting = lib.handle(obj, for_ms=RE_ENABLE_MS - 1, step_ms=STEP_MS)
    assert all(waiting), "the part was reset before the re-enable wait was over"

    lib.set_alarms(obj, fault=False, warning=False)
    after = lib.handle(obj, for_ms=20, step_ms=STEP_MS)

    assert _falling_edges(after) == 1, "not exactly one reset pulse"
    assert after.count(False) * STEP_MS == pytest.approx(lib.low_level_ms(obj), abs=STEP_MS)
    assert after[-1] is True, "the part was not released after the pulse"


def test_ALX1564_P594_a_fault_that_stays_is_answered_once_every_re_enable_wait(amp):
    """While nFAULT stays low, the driver keeps trying: one pulse per wait, pulse and settle time."""
    lib, obj = amp
    lib.set_alarms(obj, fault=True, warning=False)
    period = RE_ENABLE_MS + lib.low_level_ms(obj) + lib.waiting_ms(obj)

    levels = lib.handle(obj, for_ms=2.5 * period, step_ms=STEP_MS)

    assert _falling_edges(levels) == 2


def test_ALX1564_P595_the_was_flags_latch_until_the_application_clears_them(amp):
    """A fault or a warning that came and went is remembered, and only the application forgets it.

    Unlike a getter that clears what it returns, these are read as often as needed and cleared on
    purpose - so two readers can both be told.
    """
    lib, obj = amp
    lib.set_alarms(obj, fault=True, warning=True)
    lib.handle(obj, for_ms=5, step_ms=STEP_MS)
    lib.set_alarms(obj, fault=False, warning=False)
    lib.handle(obj, for_ms=5, step_ms=STEP_MS)

    assert (lib.err(obj), lib.warning(obj)) == (False, False)
    assert (lib.was_err(obj), lib.was_warning(obj)) == (True, True)
    assert (lib.was_err(obj), lib.was_warning(obj)) == (True, True), "reading the flags cleared them"

    lib.clear_was(obj)

    assert (lib.was_err(obj), lib.was_warning(obj)) == (False, False)


def test_ALX1564_P596_a_re_enable_wait_of_zero_turns_the_recovery_off(tpa3255_lib):
    """CHARACTERIZATION: with no re-enable wait, Handle only holds nRESET high.

    A fault is never answered with a reset, and the was-flags are never set: they are kept by the
    same branch of Handle as the recovery, so an application that turns the recovery off loses the
    record of its faults too.
    """
    obj = tpa3255_lib.new(0.0)
    tpa3255_lib.set_alarms(obj, fault=True, warning=True)

    levels = tpa3255_lib.handle(obj, for_ms=300, step_ms=STEP_MS)

    assert all(levels), "the part was reset with the recovery turned off"
    assert (tpa3255_lib.was_err(obj), tpa3255_lib.was_warning(obj)) == (False, False)
