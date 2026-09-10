"""ALX-1553 - alxBool PC unit tests (Tier 2: a boolean with memory, over a clock the test owns).

One input and twenty-one functions about it: is it true, has it been true for a short time, for a
long time, was it ever, and six flags a caller clears by hand. It is the module a product uses to
tell a tap from a hold, a blip from a fault - and it had no test.

It is built on the glitch filter and two software timers, all of them real here; only the interrupt
lock is faked. Time moves only when the test moves it, so every one of the four thresholds is
approached to the microsecond instead of to a sampling interval.

Three behaviours are recorded rather than asserted as design:
  P193 a glitch time of ZERO does not mean "no filtering" - a change is still never believed within
       the same instant, because the filter compares elapsed time with ``>``;
  P194 the "was true for a short time" latch is only set if the module is UPDATED while the input
       is inside the short-to-long band, so a caller polling more slowly than that band never sees
       it, however long the input was actually true;
  P197 the constructor latches ``WasTrue`` from the initial value, so the module answers "yes, it
       has been true" before it has observed anything at all.

Test group P188-P197 = ALX-1553 boolean-with-memory proofs.
"""

import pytest

pytestmark = pytest.mark.unit

SHORT_MS, LONG_MS = 100.0, 1000.0
TICK = 0.001            # one microsecond in milliseconds: the smallest step that changes anything


def settled_true(bool_lib, obj):
    """Drive the input true and let the glitch filter believe it, without consuming any threshold."""
    bool_lib.update(obj, False)
    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, True)


# =====================================================================
# P188-P190 - the state machine
# =====================================================================


def test_ALX1553_P188_the_true_side_walks_from_short_to_long_and_the_thresholds_are_inclusive(
    bool_lib
):
    """Under short, at short, under long, at long - the four states of a held input, exactly.

    The boundaries are the point of the test. ``IsTrueUpToShortTime`` holds while the elapsed time
    is strictly BELOW the short threshold and gives way exactly ON it; the same at the long one. One
    microsecond either side of each is checked, which a bench sampling every few milliseconds could
    never distinguish.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, obj)

    bool_lib.advance_ms(SHORT_MS - TICK)
    bool_lib.update(obj, True)
    assert bool_lib.true_flags(obj) == {"IsTrue", "IsTrueUpToShortTime", "IsTrueUpToLongTime",
                                        "WasTrue"}

    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, True)
    assert bool_lib.true_flags(obj) == {"IsTrue", "IsTrueUpToLongTime", "WasTrue",
                                        "WasTrueForShortTime"}, "exactly the short time"

    bool_lib.advance_ms(LONG_MS - SHORT_MS - TICK)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrueUpToLongTime") is True, "one microsecond under the long time"
    assert bool_lib.query(obj, "IsTrueForLongTime") is False

    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, True)
    assert bool_lib.true_flags(obj) == {"IsTrue", "IsTrueForLongTime", "WasTrue",
                                        "WasTrueForShortTime", "WasTrueForLongTime"}


def test_ALX1553_P189_the_false_side_is_the_mirror_of_the_true_side(bool_lib):
    """The same walk with the input inverted gives the same answers with the names swapped.

    Two branches of the same shape written out twice is where a copy-and-paste slip lives, so this
    compares them by construction rather than by eye.
    """
    obj = bool_lib.make(True, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    bool_lib.update(obj, True)
    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, False)

    bool_lib.advance_ms(SHORT_MS)
    bool_lib.update(obj, False)
    assert bool_lib.false_flags(obj) == {"IsFalse", "IsFalseUpToLongTime", "WasFalse",
                                         "WasFalseForShortTime"}

    bool_lib.advance_ms(LONG_MS - SHORT_MS)
    bool_lib.update(obj, False)
    assert bool_lib.false_flags(obj) == {"IsFalse", "IsFalseForLongTime", "WasFalse",
                                         "WasFalseForShortTime", "WasFalseForLongTime"}


@pytest.mark.parametrize("elapsed_ms", [0.0, 1.0, SHORT_MS, 500.0, LONG_MS, 5000.0])
def test_ALX1553_P190_true_and_false_are_never_both_current(bool_lib, elapsed_ms):
    """``IsTrue`` and ``IsFalse`` are answers about the same instant and cannot both be yes.

    Checked at six points along the walk, because the two sides keep separate flags and separate
    timers - nothing in the module's structure makes them exclusive except the code that clears one
    set whenever it writes the other.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, obj)
    bool_lib.advance_ms(elapsed_ms)
    bool_lib.update(obj, True)

    assert bool_lib.query(obj, "IsTrue") is True
    assert bool_lib.query(obj, "IsFalse") is False
    for name in ("IsFalseUpToShortTime", "IsFalseUpToLongTime", "IsFalseForLongTime"):
        assert bool_lib.query(obj, name) is False, f"{name} while the input is true"


# =====================================================================
# P191-P192 - the latches
# =====================================================================


def test_ALX1553_P191_the_was_flags_outlive_the_input_that_set_them(bool_lib):
    """A held input sets three true-side latches, and they stay set after it goes away.

    That is what a latch is for: a caller that polls slowly still learns what happened between two
    of its own reads. Both long-lived flags are checked well after the input has gone false and the
    false side has taken over.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, obj)
    bool_lib.advance_ms(SHORT_MS)
    bool_lib.update(obj, True)
    bool_lib.advance_ms(LONG_MS)
    bool_lib.update(obj, True)

    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, False)
    bool_lib.advance_ms(LONG_MS * 2)
    bool_lib.update(obj, False)

    assert bool_lib.query(obj, "IsTrue") is False, "the input is long gone"
    assert bool_lib.query(obj, "WasTrue") is True
    assert bool_lib.query(obj, "WasTrueForShortTime") is True
    assert bool_lib.query(obj, "WasTrueForLongTime") is True


@pytest.mark.parametrize(("clear", "cleared"), [
    ("ClearWasTrueFlag", "WasTrue"),
    ("ClearWasTrueForShortTimeFlag", "WasTrueForShortTime"),
    ("ClearWasTrueForLongTimeFlag", "WasTrueForLongTime"),
])
def test_ALX1553_P191_each_clear_function_clears_exactly_one_latch(bool_lib, clear, cleared):
    """Three latches, three clear functions, and no clear touches a flag that is not its own."""
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, obj)
    bool_lib.advance_ms(SHORT_MS)
    bool_lib.update(obj, True)
    bool_lib.advance_ms(LONG_MS)
    bool_lib.update(obj, True)
    before = bool_lib.state(obj)

    bool_lib.clear(obj, clear)
    after = bool_lib.state(obj)
    assert after[cleared] is False, f"{clear} did not clear {cleared}"
    assert {k: v for k, v in after.items() if k != cleared} == \
           {k: v for k, v in before.items() if k != cleared}, f"{clear} moved something else"


def test_ALX1553_P192_a_cleared_latch_is_set_again_by_the_next_qualifying_input(bool_lib):
    """Clearing is not permanent: the flag comes back when the condition happens again.

    A latch that could only be set once would be a real trap for a caller that clears it every
    cycle, so this is worth an assertion rather than an assumption.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, obj)
    bool_lib.advance_ms(LONG_MS)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "WasTrueForLongTime") is True

    bool_lib.clear(obj, "ClearWasTrueForLongTimeFlag")
    assert bool_lib.query(obj, "WasTrueForLongTime") is False

    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "WasTrueForLongTime") is True, "the latch did not arm again"


# =====================================================================
# P193-P194 - two things measurement showed and reading would not
# =====================================================================


def test_ALX1553_P193_a_glitch_time_of_zero_still_needs_time_to_pass(bool_lib):
    """CHARACTERIZATION: configured with 0 ms of filtering, a change is still not believed at once.

    The glitch filter compares ``unstableTime_us > stableTime_us`` with ``>``, so at the instant the
    input changes the elapsed time is 0 and 0 is not greater than 0. The change is believed on the
    next update that happens at a LATER time, however small the step - one microsecond is enough.

    So "no glitch filtering" is not what a zero configures. It configures "not within the same
    instant", which for a product whose loop reads an input and updates in the same pass means the
    change is always seen one iteration late. Measured here at the smallest step the clock has.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS, 0.0, 0.0)
    bool_lib.update(obj, False)

    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrue") is False, "believed within the same instant"

    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrue") is True, "not believed even a microsecond later"


def test_ALX1553_P194_the_short_time_latch_is_missed_by_a_caller_that_polls_too_slowly(bool_lib):
    """CHARACTERIZATION: ``WasTrueForShortTime`` records being SAMPLED in the band, not being in it.

    The module sets the flag only while the elapsed true time is inside ``[short, long)``. It is set
    from a sample, so a caller whose loop is slower than that band steps straight over it: the input
    was genuinely true for longer than the short time, and the flag never comes on.

    Measured, with a band of 100 ms to 1000 ms:

        updated at 60 ms and then at 1060 ms   ->  WasTrueForShortTime FALSE, ForLongTime true
        updated once inside the band           ->  both true

    A consumer can infer it - ``WasTrueForLongTime`` implies the short time passed - but the flag
    itself does not answer the question its name asks. Recorded rather than sealed because nothing
    documents which of the two it means, and the cheap fix (set it whenever the long band is
    entered) is a decision about the API.
    """
    stepped_over = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, stepped_over)
    bool_lib.advance_ms(60.0)
    bool_lib.update(stepped_over, True)
    bool_lib.advance_ms(1000.0)
    bool_lib.update(stepped_over, True)
    assert bool_lib.query(stepped_over, "IsTrueForLongTime") is True, "it was true far past long"
    assert bool_lib.query(stepped_over, "WasTrueForShortTime") is False, "the flag was missed"

    bool_lib.tick_reset()
    sampled = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, sampled)
    for step in (60.0, 200.0, 1000.0):
        bool_lib.advance_ms(step)
        bool_lib.update(sampled, True)
    assert bool_lib.query(sampled, "WasTrueForShortTime") is True, "the same input, sampled inside"
    assert bool_lib.query(sampled, "WasTrueForLongTime") is True


# =====================================================================
# P195-P197 - the filter in front, the timer behind, and the start
# =====================================================================


def test_ALX1553_P195_a_glitch_shorter_than_the_filter_never_reaches_the_state_machine(bool_lib):
    """With 50 ms of filtering configured, a 40 ms pulse leaves every true-side flag untouched.

    The filter is a real AlxFiltGlitchBool here, not a stub, so this also proves the two modules are
    wired together in the direction the header implies.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS, 50.0, 50.0)
    bool_lib.update(obj, False)

    bool_lib.advance_ms(40.0)
    bool_lib.update(obj, True)
    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, False)
    assert bool_lib.true_flags(obj) == set(), "a 40 ms pulse past a 50 ms filter"

    bool_lib.advance_ms(50.0 + TICK)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrue") is True, "a pulse longer than the filter must get through"


def test_ALX1553_P196_the_true_timer_starts_again_from_zero_after_an_interruption(bool_lib):
    """900 ms of true, a false, then true again: the long threshold is a fresh 1000 ms away.

    Total time is not what the module measures - CONSECUTIVE time is - and the two only differ after
    an interruption, which is exactly the case a caller cares about.
    """
    obj = bool_lib.make(False, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    settled_true(bool_lib, obj)
    bool_lib.advance_ms(900.0)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrueForLongTime") is False

    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, False)
    bool_lib.advance_ms(TICK)
    bool_lib.update(obj, True)

    bool_lib.advance_ms(900.0)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrueForLongTime") is False, "the first 900 ms were counted again"
    bool_lib.advance_ms(100.0)
    bool_lib.update(obj, True)
    assert bool_lib.query(obj, "IsTrueForLongTime") is True


@pytest.mark.parametrize("initial", [True, False])
def test_ALX1553_P197_the_was_flag_is_latched_by_the_constructor_before_anything_is_observed(
    bool_lib, initial
):
    """CHARACTERIZATION: constructed true, the module already answers "yes, it has been true".

    ``AlxBool_Ctor`` writes ``wasTrue = valInitial`` and ``wasFalse = !valInitial`` directly, so the
    latch reports a state the module was TOLD about rather than one it measured. The two duration
    latches are not seeded - they stay false until a real interval passes - so only the plain
    ``WasTrue`` / ``WasFalse`` pair carries the initial value.

    Deliberate-looking: it is an explicit assignment, not an oversight, and an initial value has to
    go somewhere. Pinned because a caller that clears the latches at start-up and a caller that does
    not will disagree about the first cycle, and nothing says which is intended.
    """
    obj = bool_lib.make(initial, SHORT_MS, LONG_MS, SHORT_MS, LONG_MS)
    state = bool_lib.state(obj)

    assert state["WasTrue"] is initial, "before any Update at all"
    assert state["WasFalse"] is (not initial)
    assert state["IsTrue"] is initial
    assert state["WasTrueForShortTime"] is False, "a duration latch is not seeded"
    assert state["WasTrueForLongTime"] is False
    assert state["WasFalseForShortTime"] is False
    assert state["WasFalseForLongTime"] is False
