"""Auralix C Library - AlxNtcg103jf103ft1s on the host.

One free function and a 191-entry table: a thermistor's resistance in, a temperature in whole
degrees out, from -40 to +150. The part is a 10 kOhm NTC - the 103 in its name - so 10000 ohms is
25 degC by definition, and everything else is the table.

The table is read out of the source here and compared against the function, so the test does not
restate 191 numbers it would then have to maintain. What it checks is the SHAPE: that the table is
strictly falling, that every entry maps to its own temperature, that the ends are the ends, and
that a resistance outside the table is bounded rather than refused.

Proofs (ALX-1553): P291-P297.
"""

import re
from itertools import pairwise
from pathlib import Path

import pytest

TEMP_MIN_DEGC = -40
TEMP_MAX_DEGC = 150


def _table() -> list[int]:
    """The module's own lookup table, read from the source it is compiled from."""
    source = Path(__file__).parent.parent / "Ext" / "alxNtcg103jf103ft1s.c"
    text = source.read_text(encoding="utf-8", errors="replace")
    block = text[text.index("static const uint32_t lut"):]
    block = block[:block.index("};")]
    values = [int(v) for v in re.findall(r"^\s*(\d+),?", block, re.MULTILINE)]
    assert len(values) == TEMP_MAX_DEGC - TEMP_MIN_DEGC + 1, "the table is not one entry per degree"
    return values


LUT = _table()


def test_ALX1553_P291_ten_kilohms_is_twenty_five_degrees(ntc_lib):
    """The part's defining point: a 10 kOhm NTC reads 10000 ohms at 25 degC.

    If this one number is wrong the whole table is the wrong part, and nothing else in the module
    would say so - every other reading would still be self-consistent.
    """
    assert ntc_lib.temp(10000) == 25


def test_ALX1553_P292_the_ends_of_the_table_are_the_ends_of_the_range(ntc_lib):
    """The first entry is -40 degC and the last is +150 degC."""
    assert ntc_lib.temp(LUT[0]) == TEMP_MIN_DEGC
    assert ntc_lib.temp(LUT[-1]) == TEMP_MAX_DEGC


def test_ALX1553_P293_every_table_entry_reads_back_as_its_own_temperature(ntc_lib):
    """All 191 entries, each mapping to the degree it was tabulated for.

    Exhaustive because the table IS the module: an entry transposed with its neighbour, or a row
    dropped in an edit, shifts everything after it by a degree and looks perfectly plausible.
    """
    wrong = [(res, ntc_lib.temp(res), TEMP_MIN_DEGC + i)
             for i, res in enumerate(LUT)
             if ntc_lib.temp(res) != TEMP_MIN_DEGC + i]

    assert wrong == []


def test_ALX1553_P294_the_table_falls_as_the_temperature_rises(ntc_lib):
    """A negative temperature coefficient, strictly: every entry lower than the one before it.

    An NTC's resistance falls as it warms, and the lookup depends on that being strict - it walks
    the table until it finds the first entry at or below the reading, which only means anything if
    the table is ordered.
    """
    assert all(a > b for a, b in pairwise(LUT))
    assert ntc_lib.temp(LUT[0]) < ntc_lib.temp(LUT[-1])


@pytest.mark.parametrize("res_ohm", [10_000_000, 1_000_000, 188_501])
def test_ALX1553_P295_a_resistance_colder_than_the_table_reads_the_coldest(ntc_lib, res_ohm):
    """More resistance than the table knows is reported as -40 degC, not refused.

    An open circuit is infinite resistance, so this is the reading a disconnected thermistor gives.
    It is bounded rather than flagged, which means a caller cannot tell -40 degC from a broken
    wire - worth knowing before that distinction matters.
    """
    assert ntc_lib.temp(res_ohm) == TEMP_MIN_DEGC


@pytest.mark.parametrize("res_ohm", [0, 1, 312])
def test_ALX1553_P296_a_resistance_hotter_than_the_table_reads_the_hottest(ntc_lib, res_ohm):
    """Less resistance than the table knows is reported as +150 degC - including a dead short.

    A shorted thermistor is zero ohms, and it reads as the top of the range rather than asserting.
    The same caveat as the cold end: a caller cannot tell +150 degC from a short.
    """
    assert ntc_lib.temp(res_ohm) == TEMP_MAX_DEGC


def test_ALX1553_P297_between_two_entries_the_reading_rounds_toward_hot(ntc_lib):
    """CHARACTERIZATION: the lookup does not interpolate - it steps, and it steps WARM.

    The table holds one entry per degree and the function returns the first entry at or BELOW the
    reading, walking down from the coldest. A resistance strictly between two entries is therefore
    reported as the warmer of the two, and the resolution is one degree.

    Which way it rounds is not a detail on a thermistor: a reading used for a thermal cut-out that
    rounds warm trips early, and one that rounded cold would trip late. Checked halfway between
    three pairs spread across the range, and one ohm below an entry - the sharpest case, where the
    reading has only just left that degree behind.
    """
    for index in (10, 65, 150):
        midpoint = (LUT[index] + LUT[index + 1]) // 2

        assert ntc_lib.temp(midpoint) == TEMP_MIN_DEGC + index + 1, "a midpoint rounded cold"

    assert ntc_lib.temp(LUT[65]) == 25, "an exact entry is its own degree"
    assert ntc_lib.temp(LUT[65] - 1) == 26, "one ohm below an entry is already the warmer degree"
    assert ntc_lib.temp(LUT[65] + 1) == 25, "one ohm above an entry is still that degree"
