"""ALX-1553 - alxRtc_Global PC unit tests (Tier 1: the calendar, pure, no clock, no device).

Sixteen free functions converting between Unix time and a broken-down date in four resolutions, and
splitting a second into milliseconds, microseconds and nanoseconds. Nothing tested any of them.

This module is unusual in the library: its correct answer can be computed INDEPENDENTLY. Every other
module is checked against what it is supposed to do; this one is checked against Python's own
calendar, which is a second implementation written by other people. So the sweep can be exhaustive
rather than representative - every day from 2000 to 2135, plus twenty thousand seeded random
instants - and it is, because leap years, century years and the 2038 rollover are exactly the cases
a hand-written list of examples forgets.

The calendar came through it clean. The sub-second half did not:

  P206  the last 29 nanoseconds of every second convert to a fraction of exactly 1.0 and back to
        ZERO - the end of a second reads as its beginning - and the function's own assert cannot
        catch it, because the bound it compares against is not representable in a 32-bit float.

Test group P201-P210 = ALX-1553 calendar and sub-second proofs.
"""

import datetime as dt
import random

import pytest

pytestmark = pytest.mark.unit

EPOCH = dt.datetime(1970, 1, 1, tzinfo=dt.UTC)
FIRST = dt.datetime(2000, 1, 1, tzinfo=dt.UTC)     # the module's own lower bound
LAST = dt.datetime(2135, 12, 31, tzinfo=dt.UTC)    # a uint8 year counted from 2000
SEED = 20260910

# a real RTC sub-second tick: a 32768 Hz crystal divided to 128 Hz, one tick every 244141 ns
TICK_NS = 244_141


def unix(moment: dt.datetime) -> int:
    return int(moment.timestamp())


# =====================================================================
# P201-P203 - the calendar, against a second implementation
# =====================================================================


@pytest.mark.expect_assert(
    "P449: the module's two directions disagree about the supported range - UnixTimeNsToDateTime produces years "
    "past 2099 and DateTimeToUnixTimeNs asserts on them"
)
def test_ALX1553_P201_every_day_from_2000_to_2135_converts_the_way_a_calendar_says(rtc_lib):
    """Both directions and the weekday, for 49673 consecutive days, against Python's datetime.

    Exhaustive because the interesting inputs are not the ones anybody would list: 2000 is a leap
    year AND a century year, 2100 is a century year that is NOT a leap year, and the module's own
    loop subtracts a year at a time from the epoch, so an error in one year shifts every year after
    it. A sweep finds that on its first bad day; a list of examples finds it if someone thought of
    the right one.
    """
    moment = FIRST
    checked = 0
    while moment <= LAST:
        stamp = unix(moment)
        date = rtc_lib.to_date(stamp, "Sec")
        assert date.as_tuple() == (moment.year, moment.month, moment.day, 0, 0, 0), str(moment)
        assert date.weekDay == moment.isoweekday(), f"weekday of {moment}"
        assert rtc_lib.from_date(rtc_lib.date(moment.year, moment.month, moment.day)) == stamp
        moment += dt.timedelta(days=1)
        checked += 1
    assert checked == 49673, "the sweep did not cover what it claims to"


@pytest.mark.expect_assert(
    "P449: the module's two directions disagree about the supported range - UnixTimeNsToDateTime produces years "
    "past 2099 and DateTimeToUnixTimeNs asserts on them"
)
def test_ALX1553_P202_twenty_thousand_random_instants_convert_both_ways(rtc_lib):
    """Times of day as well as dates, from a seeded generator so a failure is reproducible.

    The daily sweep only ever asks about midnight. This asks about the hours, minutes and seconds,
    which is where a modulo in the wrong order would show and nowhere else.
    """
    generator = random.Random(SEED)
    for _ in range(20_000):
        stamp = generator.randrange(unix(FIRST), unix(LAST))
        moment = EPOCH + dt.timedelta(seconds=stamp)
        date = rtc_lib.to_date(stamp, "Sec")
        expected = (moment.year, moment.month, moment.day,
                    moment.hour, moment.minute, moment.second)
        assert date.as_tuple() == expected, f"unix {stamp}"
        assert date.weekDay == moment.isoweekday(), f"unix {stamp}"
        assert rtc_lib.from_date(rtc_lib.date(*expected)) == stamp, f"unix {stamp}"


@pytest.mark.parametrize("iso", [
    "2000-01-01T00:00:00",      # the module's lower bound, and a century leap year
    "2000-02-29T12:00:00",      # 2000 IS a leap year - divisible by 400
    "2001-02-28T23:59:59",      # the day before a non-leap March
    "2020-02-29T23:59:59",
    "2038-01-19T03:14:07",      # the last second a signed 32-bit time can hold
    "2038-01-19T03:14:08",      # and the first one it cannot
    "2099-12-31T23:59:59",
    "2100-02-28T23:59:59",      # 2100 is NOT a leap year - divisible by 100, not by 400
    "2100-03-01T00:00:00",
    "2135-12-31T23:59:59",      # the last date a uint8 year counted from 2000 can express
])
@pytest.mark.expect_assert(
    "P449: the module's two directions disagree about the supported range - UnixTimeNsToDateTime produces years "
    "past 2099 and DateTimeToUnixTimeNs asserts on them"
)
def test_ALX1553_P203_the_dates_that_break_calendars(rtc_lib, iso):
    """The specific instants worth naming, kept as their own test so a failure says which one."""
    moment = dt.datetime.fromisoformat(iso).replace(tzinfo=dt.UTC)
    expected = (moment.year, moment.month, moment.day, moment.hour, moment.minute, moment.second)
    assert rtc_lib.to_date(unix(moment), "Sec").as_tuple() == expected
    assert rtc_lib.from_date(rtc_lib.date(*expected)) == unix(moment)


# =====================================================================
# P204-P205 - the four resolutions
# =====================================================================


@pytest.mark.parametrize(("unit", "per_second"), [("Sec", 1), ("Ms", 1_000),
                                                  ("Us", 1_000_000), ("Ns", 1_000_000_000)])
def test_ALX1553_P204_all_four_resolutions_describe_the_same_instant(rtc_lib, unit, per_second):
    """Seconds, milliseconds, microseconds and nanoseconds of the same moment agree on the date."""
    moment = dt.datetime(2026, 9, 10, 21, 30, 45, tzinfo=dt.UTC)
    expected = (2026, 9, 10, 21, 30, 45)
    assert rtc_lib.to_date(unix(moment) * per_second, unit).as_tuple() == expected
    assert rtc_lib.from_date(rtc_lib.date(*expected), unit) == unix(moment) * per_second


def test_ALX1553_P205_the_sub_second_fields_survive_the_finest_resolution(rtc_lib):
    """A nanosecond timestamp carries ms, us and ns through, and comes back the same number."""
    moment = dt.datetime(2026, 9, 10, 21, 30, 45, tzinfo=dt.UTC)
    stamp_ns = unix(moment) * 1_000_000_000 + 123_456_789
    date = rtc_lib.to_date(stamp_ns, "Ns")
    assert (date.ms, date.us, date.ns) == (123, 456, 789)
    assert rtc_lib.from_date(date, "Ns") == stamp_ns


# =====================================================================
# P206-P207 - the second fraction
# =====================================================================


@pytest.mark.xfail(
    strict=True,
    reason="AlxRtc_MsUsNsToSecFract returns exactly 1.0f for the last 29 ns of a second, and "
           "AlxRtc_SecFractToMsUsNs turns that back into 0 - the end of a second reads as its "
           "beginning; the function's own assert cannot catch it because 0.999999999f IS 1.0f",
)
@pytest.mark.parametrize(("ms", "us", "ns"), [(999, 999, 999), (999, 999, 971)])
def test_ALX1553_P206_the_end_of_a_second_is_not_the_start_of_one(rtc_lib, ms, us, ns):
    """SEALED DEFECT - the last 29 nanoseconds of every second wrap to zero.

    ``AlxRtc_MsUsNsToSecFract`` builds the total nanoseconds as a uint64 and then scales it into a
    32-bit float. Near 1.0 a float32 step is about 60 ns, so everything from 999 999 971 ns upward
    rounds to exactly 1.0f. Binary-searched to the nanosecond:

        999 999 970 ns  ->  0.99999994  ->  back to (999, 999, 936)
        999 999 971 ns  ->  1.0         ->  back to (0, 0, 0)          <-- the wrap
        999 999 999 ns  ->  1.0         ->  back to (0, 0, 0)

    The function asserts its own postcondition, ``secFract <= 0.999999999f`` - and that assert
    cannot fire, because 0.999999999 is not representable in a float32 either: it IS 1.0f, so the
    check reads 1.0f <= 1.0f and passes. A guard that is written correctly and cannot work is worth
    more attention than the 29 ns, which is why this is sealed rather than noted.

    29 ns of every second is 3 parts in 100 million, and the wrap is to ZERO rather than to a nearby
    value - so a consumer that timestamps with it sees an instant jump backwards by a whole second's
    worth of fraction, not a rounding error. Whether that matters depends on what reads it.

    Strict xfail: XPASSes the day the fraction is computed or compared in a way that keeps the top
    of the range distinguishable from 1.0.
    """
    fraction = rtc_lib.sec_fract(ms, us, ns)
    assert fraction < 1.0, f"a fraction of a second came out as {fraction}"
    assert rtc_lib.sec_fract_to_ms_us_ns(fraction) != (0, 0, 0)


@pytest.mark.parametrize("total_ns", [0, 1, 1_000, 1_000_000, 123_456_789, 500_000_000,
                                      999_000_000, 999_999_000, 999_999_970])
def test_ALX1553_P207_the_fraction_round_trip_is_good_to_60_nanoseconds_not_to_one(
    rtc_lib, total_ns, record_property
):
    """CHARACTERIZATION: the fraction is a float32, so a nanosecond does not survive it.

    The API offers nanosecond fields on both sides and a 24-bit mantissa in between. Below the wrap
    point of P206 the error is bounded: swept across the second, the worst observed round trip is
    60 ns, which is one float32 step near 1.0 and much less lower down.

    Recorded rather than sealed. Nothing claims the fraction is exact, and a float32 second fraction
    is a reasonable thing to have - but the signature promises nanoseconds, so the number is worth
    having written down next to it.
    """
    ms, rest = divmod(total_ns, 1_000_000)
    us, ns = divmod(rest, 1_000)
    fraction = rtc_lib.sec_fract(ms, us, ns)
    back = rtc_lib.sec_fract_to_ms_us_ns(fraction)
    recovered = back[0] * 1_000_000 + back[1] * 1_000 + back[2]
    record_property(f"sec_fract_error_ns_at_{total_ns}", recovered - total_ns)
    assert abs(recovered - total_ns) <= 60, f"{total_ns} ns came back as {recovered} ns"


# =====================================================================
# P208-P210 - the tick conversions, whose names say the opposite of what they do
# =====================================================================


def test_ALX1553_P208_the_ns_functions_convert_ticks_not_nanoseconds(rtc_lib):
    """CHARACTERIZATION: ``rtcTick_ns`` is the DURATION of a tick, and the values are tick COUNTS.

    ``AlxRtc_MsUsNsToNs(rtcTick_ns, ms, us, ns)`` divides the total nanoseconds BY ``rtcTick_ns``,
    so what it returns is a number of RTC sub-second ticks, not nanoseconds. Its partner
    ``AlxRtc_NsToMsUsNs(rtcTick_ns, in_ns, ...)`` multiplies ``in_ns`` by the same duration, so its
    ``in_ns`` argument is a tick count too. Both names say nanoseconds; neither is.

    Measured with a real RTC divider - a 32768 Hz crystal divided to 128 Hz, 244141 ns per tick:

        (500, 0, 0)  ->  2047 ticks  ->  (499, 756, 627)

    which is half a second to the tick and back, quantised to the tick. The names are the only thing
    wrong here, and they are wrong in the direction that makes a caller pass the wrong argument.
    """
    ticks = rtc_lib.ms_us_ns_to_ns(TICK_NS, 500, 0, 0)
    assert ticks == 2047, "half a second is 2047 ticks of 244141 ns, not a count of nanoseconds"
    assert rtc_lib.ns_to_ms_us_ns(TICK_NS, ticks) == (499, 756, 627)


@pytest.mark.parametrize(("ms", "us", "ns"), [(0, 0, 0), (1, 0, 0), (123, 456, 789),
                                              (999, 999, 999)])
def test_ALX1553_P209_a_one_nanosecond_tick_makes_the_round_trip_exact(rtc_lib, ms, us, ns):
    """With a tick of one nanosecond the two functions are exact inverses, as their names imply.

    That is the case the names describe, and it works - which is what makes P208 a naming problem
    rather than an arithmetic one.
    """
    ticks = rtc_lib.ms_us_ns_to_ns(1, ms, us, ns)
    assert ticks == ms * 1_000_000 + us * 1_000 + ns
    assert rtc_lib.ns_to_ms_us_ns(1, ticks) == (ms, us, ns)


def test_ALX1553_P210_the_shorter_conversions_drop_the_fields_they_do_not_take(rtc_lib):
    """``MsUsToNs`` and ``MsToNs`` are the full conversion with the missing fields passed as zero."""
    assert rtc_lib.c.AlxRtc_MsUsToNs(1, 123, 456) == rtc_lib.ms_us_ns_to_ns(1, 123, 456, 0)
    assert rtc_lib.c.AlxRtc_MsToNs(1, 123) == rtc_lib.ms_us_ns_to_ns(1, 123, 0, 0)
    assert rtc_lib.c.AlxRtc_MsToNs(1, 123) == 123_000_000


# =====================================================================
# P449 - the two directions disagree about the range
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: the module's two conversions disagree about the years they support. "
    "AlxRtc_UnixTimeNsToDateTime happily produces years past 2099 - P201 walks every day to 2135 "
    "and P203 checks 2100 and 2135 by name - but AlxRtc_DateTimeToUnixTimeNs opens with "
    "ALX_RTC_GLOBAL_ASSERT(dateTime.yr <= 99), because the year field is two digits. So a date the "
    "module itself just produced cannot be given back to it: a round trip through any instant "
    "after 2099 trips an assertion, and on a product that enables them - which is how they ship - "
    "that RESETS THE MCU. It is not a far-future problem in the usual sense: the assertion is "
    "reachable from a corrupted RTC, an unset clock reading a wild value, or a device left running "
    "past 2099, and the failure mode is a reboot loop rather than a wrong date. The fix is to "
    "decide the range in one place: either the assertion admits the years the reverse direction "
    "produces, or the reverse direction stops producing them"))
def test_ALX1553_P449_a_date_the_module_produced_can_be_given_back_to_it(rtc_lib):
    """Round trip through a date after 2099 - the years the other direction hands out.

    Measured through the library's own assertion counter rather than by comparing numbers: the
    conversion still returns something, and what is wrong is that it asserted on the way.
    """
    rtc_lib.c.AlxAssertPc_Reset()

    rtc_lib.from_date(rtc_lib.date(2100, 3, 1))

    assert rtc_lib.c.AlxAssertPc_Count() == 0, (
        f"a date after 2099 asserted: {(rtc_lib.c.AlxAssertPc_First() or b'').decode()}"
    )
