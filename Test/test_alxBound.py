"""ALX-1514 - alxBound PC unit tests (L5).

Contract under test (characterization of the shipped module, doc blocks fixed
in the same change):

  AlxBound_<T>(val*, valMin, valMax), T in {Uint8..64, Int8..64, Float, Double}:
    - valMin <= *val <= valMax  -> Alx_Ok, *val unchanged (boundaries INCLUSIVE)
    - *val < valMin             -> *val = valMin, AlxBound_ErrMin
    - *val > valMax             -> *val = valMax, AlxBound_ErrMax
    - precondition valMin <= valMax (asserted; not exercised here)

  AlxBound_Str(valBounded, val, valMaxLenWithNullTerm):
    - fits (strlen+1 <= max)    -> exact copy incl. NUL, Alx_Ok
    - too long                  -> first max-1 chars + NUL, AlxBound_ErrLen
    - precondition max >= 2 (asserted; not exercised here)

P12 = alxBound proof group. Float/Double NaN behavior is characterized in its
own test (documented current behavior, flagged as a design question).
"""

import random

import pytest

pytestmark = pytest.mark.unit

TYPE_RANGES = {
    "Uint8": (0, 2**8 - 1),
    "Uint16": (0, 2**16 - 1),
    "Uint32": (0, 2**32 - 1),
    "Uint64": (0, 2**64 - 1),
    "Int8": (-2**7, 2**7 - 1),
    "Int16": (-2**15, 2**15 - 1),
    "Int32": (-2**31, 2**31 - 1),
    "Int64": (-2**63, 2**63 - 1),
}
INT_TYPES = sorted(TYPE_RANGES)
FLOAT_TYPES = ["Float", "Double"]


# =====================================================================
# P12 - numeric contract, every integer type
# =====================================================================

@pytest.mark.parametrize("t", INT_TYPES)
def test_ALX1514_P12_int_in_range_unchanged(bound, t):
    lo, hi = TYPE_RANGES[t]
    for val, mn, mx in [(5, 1, 9), (lo + 1, lo, hi), (hi - 1, lo, hi), (0, lo, hi)]:
        status, out = bound.bound(t, val, mn, mx)
        assert status == bound.OK and out == val


@pytest.mark.parametrize("t", INT_TYPES)
def test_ALX1514_P12_int_boundaries_inclusive(bound, t):
    lo, hi = TYPE_RANGES[t]
    for val, mn, mx in [(1, 1, 9), (9, 1, 9), (lo, lo, hi), (hi, lo, hi)]:
        status, out = bound.bound(t, val, mn, mx)
        assert status == bound.OK and out == val


@pytest.mark.parametrize("t", INT_TYPES)
def test_ALX1514_P12_int_below_min_clamps(bound, t):
    lo, hi = TYPE_RANGES[t]
    status, out = bound.bound(t, lo, lo + 5, hi)
    assert status == bound.ERR_MIN and out == lo + 5


@pytest.mark.parametrize("t", INT_TYPES)
def test_ALX1514_P12_int_above_max_clamps(bound, t):
    lo, hi = TYPE_RANGES[t]
    status, out = bound.bound(t, hi, lo, hi - 5)
    assert status == bound.ERR_MAX and out == hi - 5


@pytest.mark.parametrize("t", INT_TYPES)
def test_ALX1514_P12_int_degenerate_range(bound, t):
    lo, hi = TYPE_RANGES[t]
    mid = (lo + hi) // 2
    assert bound.bound(t, mid, mid, mid) == (bound.OK, mid)
    assert bound.bound(t, mid + 1, mid, mid) == (bound.ERR_MAX, mid)
    assert bound.bound(t, mid - 1, mid, mid) == (bound.ERR_MIN, mid)


@pytest.mark.parametrize("t", INT_TYPES)
def test_ALX1514_P12_int_property_vs_model(bound, t):
    """Back-to-back comparison against the Python reference model."""
    lo, hi = TYPE_RANGES[t]
    rnd = random.Random(1514)
    pool = [lo, lo + 1, hi - 1, hi, 0, 1] + [rnd.randint(lo, hi) for _ in range(200)]
    for _ in range(300):
        val = rnd.choice(pool)
        a, b = rnd.choice(pool), rnd.choice(pool)
        mn, mx = min(a, b), max(a, b)
        exp_out = min(max(val, mn), mx)
        exp_status = bound.OK if mn <= val <= mx else (bound.ERR_MIN if val < mn else bound.ERR_MAX)
        assert bound.bound(t, val, mn, mx) == (exp_status, exp_out)


# =====================================================================
# P12 - float/double
# =====================================================================

@pytest.mark.parametrize("t", FLOAT_TYPES)
def test_ALX1514_P12_real_contract(bound, t):
    assert bound.bound(t, 0.5, 0.0, 1.0) == (bound.OK, 0.5)
    assert bound.bound(t, 0.0, 0.0, 1.0) == (bound.OK, 0.0)
    assert bound.bound(t, 1.0, 0.0, 1.0) == (bound.OK, 1.0)
    assert bound.bound(t, -0.25, 0.0, 1.0) == (bound.ERR_MIN, 0.0)
    assert bound.bound(t, 1.25, 0.0, 1.0) == (bound.ERR_MAX, 1.0)
    inf = float("inf")
    assert bound.bound(t, inf, 0.0, 1.0) == (bound.ERR_MAX, 1.0)
    assert bound.bound(t, -inf, 0.0, 1.0) == (bound.ERR_MIN, 0.0)


@pytest.mark.parametrize("t", FLOAT_TYPES)
def test_ALX1514_P12_real_nan_passes_through_as_ok(bound, t):
    """CHARACTERIZATION, design question flagged in review notes: NaN compares
    false to both bounds, so the current contract returns Alx_Ok and leaves
    NaN in place - NaN is never clamped."""
    status, out = bound.bound(t, float("nan"), 0.0, 1.0)
    assert status == bound.OK
    assert out != out  # NaN


# =====================================================================
# P12 - Str (bounded copy with truncation status)
# =====================================================================

def test_ALX1514_P12_str_fits_exact_copy(bound):
    status, content, raw = bound.bound_str(b"abc", 8)
    assert status == bound.OK and content == b"abc"
    assert raw[3] == 0                                   # NUL written
    assert all(b == bound.POISON for b in raw[4:])       # beyond NUL untouched


def test_ALX1514_P12_str_exact_fit_boundary(bound):
    status, content, raw = bound.bound_str(b"abcdefg", 8)   # strlen+1 == max
    assert status == bound.OK and content == b"abcdefg" and raw[7] == 0


def test_ALX1514_P12_str_too_long_truncates_terminated(bound):
    status, content, raw = bound.bound_str(b"abcdefgh", 8, buf_len=12)  # strlen+1 == 9 > 8
    assert status == bound.ERR_LEN
    assert content == b"abcdefg"                         # first max-1 chars
    assert raw[7] == 0                                   # NUL at max-1
    assert all(b == bound.POISON for b in raw[8:])       # capacity boundary respected


def test_ALX1514_P12_str_empty_source(bound):
    status, content, raw = bound.bound_str(b"", 4)
    assert status == bound.OK and content == b"" and raw[0] == 0
    assert all(b == bound.POISON for b in raw[1:])


def test_ALX1514_P12_str_min_capacity_two(bound):
    status, content, raw = bound.bound_str(b"xyz", 2, buf_len=6)
    assert status == bound.ERR_LEN and content == b"x" and raw[1] == 0
    assert all(b == bound.POISON for b in raw[2:])


def test_ALX1514_P12_str_property_vs_model(bound):
    rnd = random.Random(1514)
    for _ in range(200):
        n = rnd.randint(0, 12)
        val = bytes(rnd.choice(b"abcdefgh") for _ in range(n))
        cap = rnd.randint(2, 10)
        status, content, raw = bound.bound_str(val, cap, buf_len=16)
        if len(val) + 1 <= cap:
            assert status == bound.OK and content == val and raw[len(val)] == 0
        else:
            assert status == bound.ERR_LEN and content == val[:cap - 1] and raw[cap - 1] == 0
        assert all(b == bound.POISON for b in raw[cap:])
