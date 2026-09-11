"""ALX-1553 - alxTimSw PC unit tests (Tier 2: the real timer over a clock the test owns).

The software timer is how every periodic thing on a device happens - a product's main loop uses it
a dozen times over, for LED flashing, diagnostic cadence and every timeout - and nothing tested it.

The clock is not faked. alxTick is a plain counter that an interrupt advances on hardware, so the
test advances it instead: the timer runs its real code and the test owns time. That makes every
timeout exact rather than approximate, which is the whole reason to test a timer on a host at all -
the bench can measure that a 500 ms cadence is 500.0 ms, but only this can say that the boundary
itself is at 500 and not at 499 or 501.

The interrupt lock IS faked, because the real one is CMSIS intrinsics that do not exist on a PC.
The fake counts, so a test can assert that the timer takes the lock around the shared 64 bit tick
rather than assuming it - that proof is P88, and it moved to `test_alxTick.py` with the rest of the
clock's own tests, so each module has the mirror file the README asks for.

Test group P84-P87 = ALX-1553 software timer proofs.
"""

import pytest

pytestmark = pytest.mark.unit

MS = 1_000_000          # nanoseconds in a millisecond
SEC = 1_000 * MS


# =====================================================================
# P84 - start, stop, running
# =====================================================================


def test_ALX1553_P84_a_fresh_timer_is_not_running(clock):
    tim = clock.timer()
    assert clock.is_running(tim) is False
    assert clock.get(tim, "ns") == 0


def test_ALX1553_P84_start_and_stop(clock):
    tim = clock.timer()
    clock.start(tim)
    assert clock.is_running(tim) is True
    clock.stop(tim)
    assert clock.is_running(tim) is False
    assert clock.get(tim, "ns") == 0, "a stopped timer reads zero"


def test_ALX1553_P84_restarting_rezeroes_the_elapsed_time(clock):
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ms(300)
    assert clock.get(tim, "ms") == 300
    clock.start(tim)
    assert clock.get(tim, "ms") == 0, "start on a running timer restarts it"
    clock.advance_ms(50)
    assert clock.get(tim, "ms") == 50


# =====================================================================
# P85 - elapsed time in every unit
# =====================================================================


def test_ALX1553_P85_elapsed_time_reads_the_same_moment_in_every_unit(clock):
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ns(2 * 60 * 60 * SEC + 3 * 60 * SEC + 4 * SEC + 5 * MS)
    assert clock.get(tim, "hr") == 2
    assert clock.get(tim, "min") == 2 * 60 + 3
    assert clock.get(tim, "sec") == (2 * 60 + 3) * 60 + 4
    assert clock.get(tim, "ms") == ((2 * 60 + 3) * 60 + 4) * 1000 + 5
    assert clock.get(tim, "us") == clock.get(tim, "ms") * 1000
    assert clock.get(tim, "ns") == clock.get(tim, "ms") * MS


def test_ALX1553_P85_the_coarser_units_truncate(clock):
    """999 ms is zero seconds, not one. A timeout that rounded up would fire early."""
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ms(999)
    assert clock.get(tim, "ms") == 999
    assert clock.get(tim, "sec") == 0
    clock.advance_ms(1)
    assert clock.get(tim, "sec") == 1


# =====================================================================
# P86 - the timeout boundary
# =====================================================================


def test_ALX1553_P86_a_timeout_fires_at_its_boundary_and_not_before(clock):
    """The exact question a bench measurement cannot answer: where is the edge?"""
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ms(499)
    assert clock.is_timeout(tim, "ms", 500) is False, "499 ms is not 500"
    clock.advance_ns(999_999)
    assert clock.is_timeout(tim, "ms", 500) is False, "one nanosecond short is still short"
    clock.advance_ns(1)
    assert clock.is_timeout(tim, "ms", 500) is True, "exactly 500 ms has elapsed"


def test_ALX1553_P86_a_timeout_stays_true_once_it_has_passed(clock):
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ms(500)
    assert clock.is_timeout(tim, "ms", 500) is True
    clock.advance_ms(10_000)
    assert clock.is_timeout(tim, "ms", 500) is True, "it does not wrap back to false"


def test_ALX1553_P86_a_stopped_timer_never_times_out(clock):
    tim = clock.timer()
    clock.advance_ms(5000)
    assert clock.is_timeout(tim, "ms", 1) is False, "a timer that was never started"
    clock.start(tim)
    clock.advance_ms(5000)
    clock.stop(tim)
    assert clock.is_timeout(tim, "ms", 1) is False, "and one that was stopped"


@pytest.mark.parametrize(
    ("unit", "timeout", "just_under_ns", "exactly_ns"),
    [
        ("ns", 1000, 999, 1000),
        ("us", 500, 499_999, 500_000),
        ("ms", 500, 500 * MS - 1, 500 * MS),
        ("sec", 2, 2 * SEC - 1, 2 * SEC),
        ("min", 1, 60 * SEC - 1, 60 * SEC),
        ("hr", 1, 3600 * SEC - 1, 3600 * SEC),
    ],
)
def test_ALX1553_P86_every_unit_has_the_same_boundary_rule(
    clock, unit, timeout, just_under_ns, exactly_ns
):
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ns(just_under_ns)
    assert clock.is_timeout(tim, unit, timeout) is False, f"{unit}: fired early"
    clock.advance_ns(exactly_ns - just_under_ns)
    assert clock.is_timeout(tim, unit, timeout) is True, f"{unit}: did not fire on the boundary"


# =====================================================================
# P87 - the periodic pattern a main loop actually uses
# =====================================================================


def test_ALX1553_P87_restarting_on_timeout_gives_an_exact_period(clock):
    """The loop shape behind every cadence on a device: if timed out, act and restart.

    Twenty periods of 500 ms land on exactly 10 seconds with nothing accumulated - which is the
    host-side counterpart of the 500.00 ms measured on the bench.
    """
    tim = clock.timer()
    clock.start(tim)
    fired = 0
    for _ in range(20 * 500):                 # one millisecond of loop at a time, 10 seconds of it
        clock.advance_ms(1)
        if clock.is_timeout(tim, "ms", 500):
            fired += 1
            clock.start(tim)
    assert fired == 20, f"{fired} periods in 10 s, expected 20"
    assert clock.now_ns() == 20 * 500 * MS


def test_ALX1553_P87_a_late_loop_does_not_lose_the_period(clock):
    """A loop that misses its slot by a lot still fires once, not zero times and not twice."""
    tim = clock.timer()
    clock.start(tim)
    clock.advance_ms(1500)                    # three periods late
    assert clock.is_timeout(tim, "ms", 500) is True
    clock.start(tim)
    assert clock.is_timeout(tim, "ms", 500) is False, "the restart clears it"
