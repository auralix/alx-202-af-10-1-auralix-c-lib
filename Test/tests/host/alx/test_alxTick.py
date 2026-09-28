"""ALX-1553 - alxTick PC unit tests (Tier 1: one 64 bit counter, and the arithmetic around it).

The clock everything else on a device is measured against. It is a single 64 bit nanosecond counter
that an interrupt advances, plus eighteen one-line functions around it: six getters that divide it
into units, six that step it by one of a unit, and six that step it by many.

Eighteen one-liners is exactly the shape of code nobody tests and nobody reads carefully, and a
wrong constant in any of them is invisible - a device whose minutes are 60 seconds long except
when they are read through Get_min, say, which still counts up and still looks like a clock. So
this is checked exhaustively, unit by unit, against the nanosecond count that is the only thing
actually stored.

The clock is not faked: alxTick is real and the test advances it the way the interrupt would. The
interrupt LOCK is faked, because the real one is CMSIS intrinsics that do not exist on a PC, and it
counts, so P88 can assert the guard rather than assume it.

Under ALX_ZEPHYR this module is a thin shell over the OS clock and every stepping function is
compiled away to nothing - not reachable from this build, and not covered here.

These tests were written inside test_alxTimSw.py, the timer being the only thing that used the
clock. P88 moved here unchanged; P425-P429 are what covering the module itself added.

Proofs (ALX-1553): P88 and P425-P430.
"""

from itertools import pairwise

import pytest

pytestmark = pytest.mark.unit

# The module's own ladder, as it writes it: one of each unit, in nanoseconds.
NS_PER = {
    "ns": 1,
    "us": 1_000,
    "ms": 1_000_000,
    "sec": 1_000_000_000,
    "min": 60_000_000_000,
    "hr": 3_600_000_000_000,
}
UNITS = tuple(NS_PER)


def test_ALX1553_P88_reading_the_tick_takes_the_interrupt_lock(clock):
    """A 64 bit counter cannot be read atomically on a 32 bit core, so the read must be guarded.

    Asserted rather than assumed: the fake counts, and every lock is matched by an unlock, so a
    path that returned early while holding it would show up as a depth that never comes back to
    zero.
    """
    tim = clock.timer()
    before = clock.lock_count()
    clock.start(tim)
    clock.advance_ms(10)
    clock.get(tim, "ms")
    clock.is_timeout(tim, "ms", 5)
    assert clock.lock_count() > before, "the tick was read without taking the lock"
    assert clock.lock_count() == clock.unlock_count(), "a lock was taken and not released"
    assert clock.lock_depth_max() == 1, "the lock is not taken recursively"


def test_ALX1553_P425_a_constructed_clock_reads_zero_in_every_unit(clock):
    """The counter starts at zero, and every unit agrees that it does.

    Which is the one moment all six getters have to give the same answer, so it is the cheapest
    check that none of them has picked up an offset.
    """
    clock.tick_reset()

    assert [clock.now(unit) for unit in UNITS] == [0] * len(UNITS)


@pytest.mark.parametrize("unit", UNITS)
def test_ALX1553_P426_one_step_of_a_unit_is_that_many_nanoseconds(clock, unit):
    """Each Inc_<unit> advances the counter by exactly one of its own unit, measured in nanoseconds.

    The nanosecond count is the only thing stored, so it is the only honest place to check from: a
    Get_min that divided by the wrong constant would agree with an Inc_min that multiplied by the
    same wrong constant, and the two errors would hide each other completely.
    """
    clock.tick_reset()

    clock.tick_inc(unit)

    assert clock.now_ns() == NS_PER[unit]


@pytest.mark.parametrize("unit", UNITS)
@pytest.mark.parametrize("count", [0, 1, 7, 1000])
def test_ALX1553_P427_many_steps_are_that_many_of_the_unit(clock, unit, count):
    """Each IncRange_<unit> advances by count units - including a count of none.

    A count of zero is in the list because a loop that was not away at all still calls this, and an
    implementation that added one step regardless would drift a device's whole sense of time.
    """
    clock.tick_reset()

    clock.tick_inc_range(unit, count)

    assert clock.now_ns() == count * NS_PER[unit]


@pytest.mark.parametrize("unit", UNITS[1:])   # nothing is below a nanosecond to truncate
def test_ALX1553_P428_a_unit_reads_back_what_was_stepped_and_truncates_below_it(clock, unit):
    """CHARACTERIZATION: every getter is an integer division, so it floors - one unit short is zero.

    Three moments per unit: exactly one unit, one nanosecond short of it, and one nanosecond past
    it. The short case is the one that matters, because it says a clock read in minutes is up to
    fifty-nine seconds behind the truth - which is fine for a cadence and wrong for a deadline, and
    nothing in the module's name says which.
    """
    ns = NS_PER[unit]

    clock.tick_reset()
    clock.tick_inc_range("ns", ns - 1)
    assert clock.now(unit) == 0, "one nanosecond short of a unit already read as a whole one"

    clock.tick_inc_range("ns", 1)
    assert clock.now(unit) == 1

    clock.tick_inc_range("ns", 1)
    assert clock.now(unit) == 1, "one nanosecond past a unit read as two"


def test_ALX1553_P429_the_units_nest_exactly_all_the_way_up(clock):
    """A thousand microseconds are a millisecond, sixty minutes an hour - checked through the clock.

    Walked as a ladder rather than a table of constants: each unit is stepped by however many of
    the one below it make one, and the one above has to read exactly 1. That catches a constant
    that is wrong by a factor of ten in either direction, which a per-unit test against the same
    table would not - it would be comparing the module's constant against a copy of itself.
    """
    for smaller, larger in pairwise(UNITS):
        ratio = NS_PER[larger] // NS_PER[smaller]
        clock.tick_reset()

        clock.tick_inc_range(smaller, ratio)

        assert clock.now(larger) == 1, f"{ratio} of {smaller} was not one {larger}"
        assert clock.now(smaller) == ratio
        assert clock.now_ns() == NS_PER[larger]


def test_ALX1553_P430_stepping_the_clock_also_takes_the_interrupt_lock(clock):
    """The write side of the same 64 bit counter, guarded the same way.

    P88 covers the read. This is the other half, and it is the half an interrupt actually runs: a
    64 bit increment is not atomic on a 32 bit core either, and a read that interleaved with the
    low word's carry would see a time that never existed. Every stepping function routes through
    IncRange_ns, so one lock covers all eighteen - which is worth measuring once rather than
    trusting.
    """
    clock.tick_reset()
    before = clock.lock_count()

    for unit in UNITS:
        clock.tick_inc(unit)
        clock.tick_inc_range(unit, 3)

    assert clock.lock_count() >= before + 2 * len(UNITS), "a step wrote the tick unguarded"
    assert clock.lock_count() == clock.unlock_count(), "a lock was taken and not released"
    assert clock.lock_depth_max() == 1, "the lock is not taken recursively"
