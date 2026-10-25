"""Auralix C Library - AlxDelay on the host.

Six one-line busy waits on the global tick: read the clock, then spin reading it again until the
difference reaches what was asked for. Every blocking pause in a product that is not an RTOS sleep
goes through one of them.

They cannot be tested over the real clock the way the software timer is, because a busy wait never
returns to the test that would have to advance it. So the tick is FAKED here, with a clock that
advances by itself one step per read - which is what an interrupt does on a target while the loop
spins. It is the only group in this suite where the clock is not real, and the step is the test's
to choose, because how coarse the clock is relative to the delay is the whole question.

Proofs (ALX-1553): P452-P456.
"""

import pytest

NS_PER = {
    "ns": 1,
    "us": 1_000,
    "ms": 1_000_000,
    "sec": 1_000_000_000,
    "min": 60_000_000_000,
    "hr": 3_600_000_000_000,
}
UNITS = tuple(NS_PER)
U64_WRAP = 1 << 64


@pytest.mark.parametrize("unit", UNITS)
@pytest.mark.parametrize("count", [1, 3])
def test_ALX1553_P452_a_delay_waits_at_least_the_time_it_was_asked_for(delay_lib, unit, count):
    """Each unit waits its own duration, measured on the clock the delay itself watched.

    The clock is stepped by one of the unit being tested, so an hour costs three reads rather than
    three and a half trillion - the arithmetic under test is the multiplication from the unit into
    nanoseconds, and that does not care how coarse the steps are.

    "At least" rather than "exactly" is the honest claim: a busy wait can only notice time passing
    when it looks, so it overshoots by up to one step. That is the property a caller can rely on,
    and the one a wrong constant would break.
    """
    step = NS_PER[unit]
    delay_lib.clock(step_ns=step)

    delay_lib.delay(unit, count)

    waited = delay_lib.now_ns() - step          # the start read moved the clock once before the wait
    assert waited >= count * NS_PER[unit], f"{count} {unit} was short"
    assert waited < (count + 1) * NS_PER[unit], f"{count} {unit} overshot by more than one step"


def test_ALX1553_P453_a_zero_delay_does_not_wait_but_does_read_the_clock(delay_lib):
    """CHARACTERIZATION: asking for nothing returns at once, at the cost of two clock reads.

    The loop takes a start reading and then compares, so zero exits on the first comparison - two
    reads, and on a real device two passes through whatever guards the tick. Worth stating because
    a delay of zero is what a configurable pause set to nothing looks like, and it is reassuring
    that it is not a full spin; it is not free either.
    """
    delay_lib.clock(step_ns=1)

    delay_lib.delay("ns", 0)

    assert delay_lib.reads() == 2
    assert delay_lib.now_ns() == 2


def test_ALX1553_P454_a_delay_that_spans_the_counters_wrap_still_ends_on_time(delay_lib):
    """The 64 bit tick wrapping mid-wait does not extend the delay to the age of the universe.

    The loop subtracts the start from the current reading in unsigned arithmetic, which is the one
    way to write it that survives the wrap. Written the obvious wrong way - comparing against a
    computed end value - a delay started five nanoseconds before the wrap would either return
    instantly or never return at all, and no test that starts its clock at zero would ever say so.

    A 64 bit nanosecond counter wraps after about 585 years, so this is not a case a product will
    hit. It is a case a REVIEWER will ask about, and now the answer is measured.
    """
    delay_lib.clock(step_ns=1, start_ns=U64_WRAP - 5)

    delay_lib.delay("ns", 10)

    assert delay_lib.now_ns() == 6, "the clock did not end ten steps past where it started"
    assert delay_lib.reads() == 11


def test_ALX1553_P455_the_wait_costs_one_pass_per_step_of_the_clock(delay_lib):
    """CHARACTERIZATION: it is a BUSY wait, and how busy depends on the clock, not on the delay.

    The same millisecond costs a thousand times more passes through the loop with a one-microsecond
    clock than with a one-millisecond one. Nothing else on the device runs during any of them: this
    is not a yield, and a product calling a millisecond delay from its main loop stops answering
    for a millisecond.

    The other half of the same fact cannot be written as a test, only as a warning: **the loop only
    ends because something else moves the tick.** Called from inside the interrupt that advances it,
    or before that interrupt is running, AlxDelay never returns at all.
    """
    delay_lib.clock(step_ns=1_000)
    delay_lib.delay("ms", 1)
    coarse = delay_lib.reads()

    delay_lib.clock(step_ns=1)
    delay_lib.delay("us", 1)
    fine = delay_lib.reads()

    assert coarse == 1_001, "a millisecond at a microsecond clock is a thousand passes"
    assert fine == 1_001, "and so is a microsecond at a nanosecond clock - the ratio is what counts"


def test_ALX1553_P456_each_delay_is_measured_from_when_it_was_called(delay_lib):
    """Two delays back to back each wait their own time, rather than sharing one deadline.

    The start reading is taken inside the call, so a caller that pauses twice pauses twice. A
    module that had kept its start across calls - a plausible way to write it with a static - would
    make the second delay return instantly, and only a test that called it twice would notice.
    """
    delay_lib.clock(step_ns=1)

    delay_lib.delay("ns", 10)
    after_first = delay_lib.now_ns()

    delay_lib.delay("ns", 10)

    assert after_first == 11
    assert delay_lib.now_ns() - after_first == 11, "the second delay did not wait its own time"
