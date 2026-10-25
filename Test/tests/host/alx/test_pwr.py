"""Auralix C Library - AlxPwr on the host.

A power supervisor answers one question - is the supply where it should be - and gets there in
three steps: undo the divider in front of the converter, decide whether the reconstructed voltage
is inside a window with hysteresis on both edges, and hold that decision through a glitch filter
before reporting it.

Each step is a module this suite already tests on its own. What is tested here is the composition:
that the divider is undone BEFORE the comparison, that "in range" means between both edges and not
merely above one, that the hysteresis works on both edges, and that the filter delays the answer
without changing it.

The window below is a 24 V supply: good between 20 V and 28 V, with 0.5 V of hysteresis on each
edge, behind a 10k/2k divider that puts 24 V at 4 V on the converter pin.

Proofs (ALX-1553): P259-P267.
"""

import pytest

RES_HIGH_KOHM = 10.0
RES_LOW_KOHM = 2.0
DIVIDER_GAIN = (RES_HIGH_KOHM + RES_LOW_KOHM) / RES_LOW_KOHM     # 6.0

TOP_HIGH_V = 28.0      # above this, too high
TOP_LOW_V = 27.5       # coming back down, good again below this
BOT_HIGH_V = 20.5      # coming back up, good again above this
BOT_LOW_V = 20.0       # below this, too low


def _adc_for(supply_V: float) -> float:  # noqa: N803 - the unit belongs in the name
    """The converter voltage a supply produces through the divider."""
    return supply_V / DIVIDER_GAIN


def _settled(lib, obj, supply_V: float) -> bool:  # noqa: N803 - the unit belongs in the name
    """The supervisor's answer once the glitch filter has had a moment to agree with the reading.

    Even a filter configured for zero milliseconds needs time to pass and a second call (P267), so
    a test about the WINDOW rather than about the filter takes the settled answer.
    """
    lib.process(obj, _adc_for(supply_V))
    lib.advance_ms(1.0)
    return lib.process(obj, _adc_for(supply_V))


@pytest.fixture
def supervisor(pwr_lib):
    """A 24 V supervisor with no glitch filtering, so each reading is its own answer."""
    return pwr_lib, pwr_lib.new(res_high=RES_HIGH_KOHM, res_low=RES_LOW_KOHM,
                                top_high=TOP_HIGH_V, top_low=TOP_LOW_V,
                                bot_high=BOT_HIGH_V, bot_low=BOT_LOW_V,
                                initial=False)


@pytest.mark.parametrize("supply_V", [0.0, 12.0, 24.0, 28.0, 48.0])
def test_ALX1553_P259_the_divider_is_undone_before_anything_is_decided(
    supervisor, supply_V,  # noqa: N803 - the unit belongs in the name
):
    """The supervisor judges the SUPPLY, not the voltage on the converter pin.

    24 V through a 10k/2k divider is 4 V at the pin, and a window written in supply volts would
    reject every reading if the divider were forgotten. The reconstructed voltage is read back
    directly so the failure says which step was wrong.
    """
    lib, obj = supervisor

    lib.process(obj, _adc_for(supply_V))

    assert lib.val_V(obj) == pytest.approx(supply_V, rel=1e-4, abs=1e-4)


@pytest.mark.parametrize(("supply_V", "expected"), [
    (24.0, True),      # the middle of the window
    (20.6, True),      # just inside the lower edge
    (27.4, True),      # just inside the upper edge
    (19.0, False),     # too low
    (30.0, False),     # too high
    (0.0, False),      # no supply at all
])
def test_ALX1553_P260_in_range_means_between_both_edges(
    supervisor, supply_V, expected,  # noqa: N803 - the unit belongs in the name
):
    """Too high is as much out of range as too low.

    A supervisor written as a single comparison would call 30 V on a 24 V rail perfectly healthy,
    and an over-voltage is the one that damages what is downstream.
    """
    lib, obj = supervisor

    assert _settled(lib, obj, supply_V) is expected


def test_ALX1553_P261_the_upper_edge_releases_lower_than_it_trips(supervisor):
    """Rising past 28 V trips; coming back down it is not good again until below 27.5 V.

    That gap is the whole point of a supervisor with hysteresis: a supply sitting exactly on an
    edge would otherwise chatter, and everything downstream would chatter with it.
    """
    lib, obj = supervisor
    assert _settled(lib, obj, 24.0) is True

    assert _settled(lib, obj, 28.5) is False, "did not trip above the upper edge"
    assert _settled(lib, obj, 27.8) is False, "released before it came back under 27.5 V"
    assert _settled(lib, obj, 27.2) is True


def test_ALX1553_P262_the_lower_edge_releases_higher_than_it_trips(supervisor):
    """Falling below 20 V trips; coming back up it is not good again until above 20.5 V."""
    lib, obj = supervisor
    assert _settled(lib, obj, 24.0) is True

    assert _settled(lib, obj, 19.5) is False, "did not trip below the lower edge"
    assert _settled(lib, obj, 20.2) is False, "released before it came back over 20.5 V"
    assert _settled(lib, obj, 20.8) is True


def test_ALX1553_P263_the_supervisor_knows_which_way_it_went_out_and_reports_a_bool(supervisor):
    """CHARACTERIZATION: the state is Top, Mid or Bot inside; the answer outside is one bit.

    A caller that has to tell a brown-out from an over-voltage - to stop a motor rather than to
    wait for the rail to come back - cannot, and the information exists one struct field away.

    The second half is the state machine's own shape: it moves ONE step per call, so a supply that
    jumps from over-voltage to under-voltage passes through Mid on the way. A caller sampling once
    per cycle therefore sees one cycle of "in range" between the two faults, which is not a glitch
    in the supply and is indistinguishable from one.
    """
    lib, obj = supervisor

    lib.process(obj, _adc_for(24.0))
    assert lib.hys_state(obj) == lib.MID

    lib.process(obj, _adc_for(30.0))
    assert lib.hys_state(obj) == lib.TOP

    lib.process(obj, _adc_for(10.0))
    assert lib.hys_state(obj) == lib.MID, "Top to Bot in one call - the state machine changed"
    assert lib.in_range_raw(obj) is True, "and that intermediate state reads as IN RANGE"

    lib.process(obj, _adc_for(10.0))
    assert lib.hys_state(obj) == lib.BOT


def test_ALX1553_P264_the_filter_holds_the_answer_until_the_reading_is_stable(pwr_lib):
    """With a filter time, a supply that has just come good is not reported good until it stays.

    The raw decision and the filtered one are read together, so the test can tell "out of range"
    from "in range but not yet for long enough" - which is the distinction the filter exists for.
    """
    lib = pwr_lib
    obj = lib.new(res_high=RES_HIGH_KOHM, res_low=RES_LOW_KOHM,
                  top_high=TOP_HIGH_V, top_low=TOP_LOW_V,
                  bot_high=BOT_HIGH_V, bot_low=BOT_LOW_V,
                  initial=False, stable_true_ms=100.0, stable_false_ms=100.0)

    assert lib.process(obj, _adc_for(24.0)) is False, "reported good immediately"
    assert lib.in_range_raw(obj) is True, "the reading itself is in range"

    lib.advance_ms(99.0)
    assert lib.process(obj, _adc_for(24.0)) is False

    lib.advance_ms(2.0)
    assert lib.process(obj, _adc_for(24.0)) is True


def test_ALX1553_P265_a_glitch_shorter_than_the_filter_time_is_not_reported(pwr_lib):
    """A supply that dips for less than the filter time never shows as bad.

    This is what the filter is for: a motor starting pulls the rail down for a few milliseconds,
    and a supervisor that reported it would reset a device that was never in trouble.
    """
    lib = pwr_lib
    obj = lib.new(res_high=RES_HIGH_KOHM, res_low=RES_LOW_KOHM,
                  top_high=TOP_HIGH_V, top_low=TOP_LOW_V,
                  bot_high=BOT_HIGH_V, bot_low=BOT_LOW_V,
                  initial=True, stable_true_ms=50.0, stable_false_ms=50.0)

    lib.advance_ms(100.0)
    assert lib.process(obj, _adc_for(24.0)) is True

    # a 10 ms dip, sampled every 2 ms
    for _ in range(5):
        lib.advance_ms(2.0)
        assert lib.process(obj, _adc_for(15.0)) is True, "reported a dip shorter than the filter"

    lib.advance_ms(2.0)
    assert lib.process(obj, _adc_for(24.0)) is True


def test_ALX1553_P266_the_initial_answer_is_the_one_the_constructor_was_given(pwr_lib):
    """Before any reading, the supervisor answers what it was built with - either way round.

    A supervisor is asked what it thinks before it has seen anything, and which default it gives is
    a safety decision: starting at "good" lets a device run until the first reading says otherwise.
    """
    lib = pwr_lib

    started_bad = lib.new(res_high=RES_HIGH_KOHM, res_low=RES_LOW_KOHM,
                          top_high=TOP_HIGH_V, top_low=TOP_LOW_V,
                          bot_high=BOT_HIGH_V, bot_low=BOT_LOW_V,
                          initial=False, stable_true_ms=1000.0, stable_false_ms=1000.0)
    started_good = lib.new(res_high=RES_HIGH_KOHM, res_low=RES_LOW_KOHM,
                           top_high=TOP_HIGH_V, top_low=TOP_LOW_V,
                           bot_high=BOT_HIGH_V, bot_low=BOT_LOW_V,
                           initial=True, stable_true_ms=1000.0, stable_false_ms=1000.0)

    assert lib.process(started_bad, _adc_for(24.0)) is False
    assert lib.process(started_good, _adc_for(0.0)) is True


def test_ALX1553_P267_a_zero_millisecond_filter_still_costs_a_call_and_a_moment(supervisor):
    """CHARACTERIZATION: a filter configured for no delay at all still reports one call late.

    The glitch filter starts its stability timer when the reading changes and can only conclude on
    a LATER call with time behind it, so the first Process after a change answers the old value
    however short the filter time is - and so does a second call at the same instant. Measured here
    with the raw decision alongside the filtered one, so the gap is visible rather than inferred:

        call 1, t = 0        raw in range, answer FALSE
        call 2, t = 0        raw in range, answer FALSE
        call 3, t = 1 ms     raw in range, answer TRUE

    It matters to anyone reading a supervisor once per cycle: the answer is one cycle old, and at
    the cadence this library's products run that is ten milliseconds, not nothing.
    """
    lib, obj = supervisor

    assert lib.process(obj, _adc_for(24.0)) is False
    assert lib.in_range_raw(obj) is True, "the reading itself is in range from the first call"

    assert lib.process(obj, _adc_for(24.0)) is False, "no time has passed, so nothing is stable yet"

    lib.advance_ms(1.0)
    assert lib.process(obj, _adc_for(24.0)) is True
