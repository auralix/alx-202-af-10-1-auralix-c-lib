"""ALX-1514 - alxFifo PC unit tests (L5).

Contract under test (locked 2026-08-31, implementation follows sealed-proof):

  Alx_Status AlxFifo_ReadStrUntil   (me, char* str, const char* delim,    uint32_t len, uint32_t* lenActual);
  Alx_Status AlxFifo_ReadStrUntilAny(me, char* str, const char* delimSet, uint32_t len, uint32_t* lenActual);

  - ReadStrUntil:    line ends at the FIRST occurrence of the FULL delim sequence.
  - ReadStrUntilAny: line ends at the FIRST char that is a member of delimSet
                     (byte 0x00 is never a member).
  - Terminator INCLUDED in str. str ALWAYS null-terminated. len includes the NUL
    (max content = len-1 chars). lenActual = content chars excl. NUL; NULL allowed.
  - Alx_Ok: line delivered & consumed. Empty line = terminator only
    (lenActual == 1 resp. == strlen(delim)).
  - AlxFifo_ErrEmpty: FIFO empty. str="", lenActual=0.
  - AlxFifo_ErrNoDelim: bytes present, no terminator, FIFO NOT full.
    FIFO left untouched. str="", lenActual=0.
  - AlxFifo_ErrTooLong (two triggers, both discard, str="", lenActual=0):
      A) complete line found but line incl. terminator > len-1 chars
         -> ONLY that line (incl. terminator) is consumed & discarded;
      B) FIFO full with no terminator (line can never complete)
         -> entire FIFO content consumed & discarded.

Test groups P1..P10 map to the PROOF rows of the ALX-1514 TaskSpec.
"""

import random

import pytest

from conftest import check


# =====================================================================
# P1 - ReadStrUntilAny: basic contract (delimiter set semantics)
# =====================================================================

def test_ALX1514_P1_any_cr_terminates(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\r")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\r", 16)
    assert lib.entries(f) == 0


def test_ALX1514_P1_any_lf_terminates(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\n")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\n", 16)


def test_ALX1514_P1_any_crlf_yields_line_then_empty_line(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\r\n")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\r", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"\n", 16)  # empty line
    assert lib.entries(f) == 0


def test_ALX1514_P1_any_lfcr_symmetric(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\n\r")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\n", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"\r", 16)


def test_ALX1514_P1_any_cr_cr_two_empty_lines(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"\r\r")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"\r", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"\r", 16)


def test_ALX1514_P1_any_paste_trap_cr_crlf(lib, make_fifo):
    # PuTTY paste trap: "get" pasted with stray CR, then Enter+LF -> get\r\r\n
    f = make_fifo(32)
    lib.write(f, b"get\r\r\n")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\r", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"\r", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"\n", 16)


def test_ALX1514_P1_any_multiple_lines_sequential(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"a\rbb\nccc\r")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"a\r", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"bb\n", 16)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"ccc\r", 16)


def test_ALX1514_P1_any_generic_set_semicolon_comma(lib, make_fifo):
    # delimSet is generic - not newline-specific
    f = make_fifo(32)
    lib.write(f, b"ab;cd,e;")
    check(lib, lib.ru_any(f, b";,", 16), lib.OK, b"ab;", 16)
    check(lib, lib.ru_any(f, b";,", 16), lib.OK, b"cd,", 16)
    check(lib, lib.ru_any(f, b";,", 16), lib.OK, b"e;", 16)


def test_ALX1514_P1_any_nul_byte_is_not_a_delimiter(lib, make_fifo):
    # strchr(delimSet, 0x00) matches the set's NUL terminator - the
    # implementation MUST NOT treat a 0x00 data byte as a set member.
    f = make_fifo(32)
    lib.write(f, b"a\x00b\r")
    status, content, la, raw = lib.ru_any(f, b"\r\n", 16)
    assert status == lib.OK
    assert la == 4
    assert raw[:4] == b"a\x00b\r"


# =====================================================================
# P2 - ReadStrUntilAny: error paths (empty / no delimiter)
# =====================================================================

def test_ALX1514_P2_any_err_empty(lib, make_fifo):
    f = make_fifo(32)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.ERR_EMPTY, b"", 16)


def test_ALX1514_P2_any_err_nodelim_fifo_untouched(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"ge")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.ERR_NO_DELIM, b"", 16)
    assert lib.entries(f) == 2  # nothing consumed
    lib.write(f, b"t\r")        # line completes later (split RX packets)
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\r", 16)


def test_ALX1514_P2_any_lenactual_null_accepted(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\r")
    status, _, _, raw = lib.ru_any(f, b"\r\n", 16, len_actual_null=True)
    assert status == lib.OK
    assert raw[:4] == b"get\r" and raw[4] == 0


# =====================================================================
# P3 - ReadStrUntilAny: ErrTooLong trigger A (line does not fit len)
# =====================================================================

@pytest.mark.parametrize("payload,fits", [
    (b"abcde", True),    # line = 6 incl. CR, len-1 = 7 -> fits
    (b"abcdef", True),   # line = 7 incl. CR = len-1    -> exactly fits
    (b"abcdefg", False), # line = 8 incl. CR > len-1    -> too long
    (b"abcdefgh", False),
])
def test_ALX1514_P3_any_len_boundary(lib, make_fifo, payload, fits):
    f = make_fifo(32)
    lib.write(f, payload + b"\r")
    if fits:
        check(lib, lib.ru_any(f, b"\r\n", 8), lib.OK, payload + b"\r", 8)
    else:
        check(lib, lib.ru_any(f, b"\r\n", 8), lib.ERR_TOO_LONG, b"", 8)
        assert lib.entries(f) == 0  # whole line incl. terminator discarded


def test_ALX1514_P3_any_toolong_discards_only_line_next_survives(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"waytoolongline\r" + b"ok\r")
    check(lib, lib.ru_any(f, b"\r\n", 8), lib.ERR_TOO_LONG, b"", 8)
    check(lib, lib.ru_any(f, b"\r\n", 8), lib.OK, b"ok\r", 8)  # survivor intact
    assert lib.entries(f) == 0


# =====================================================================
# P4 - ReadStrUntilAny: ErrTooLong trigger B (flood: full, no terminator)
# =====================================================================

def test_ALX1514_P4_any_flood_full_no_delim_discards_all_and_unwedges(lib, make_fifo):
    f = make_fifo(8)
    assert lib.write(f, b"AAAAAAAA") == lib.OK   # exactly full, no terminator
    assert lib.write(f, b"B") == lib.ERR_FULL    # producer drops (ISR situation)
    check(lib, lib.ru_any(f, b"\r\n", 32), lib.ERR_TOO_LONG, b"", 32)
    assert lib.entries(f) == 0                   # discarded -> liveness restored
    lib.write(f, b"get\r")
    check(lib, lib.ru_any(f, b"\r\n", 32), lib.OK, b"get\r", 32)


def test_ALX1514_P4_any_full_with_terminator_is_not_flood(lib, make_fifo):
    f = make_fifo(8)
    lib.write(f, b"abc\rdef\n")                  # full, but terminators present
    check(lib, lib.ru_any(f, b"\r\n", 32), lib.OK, b"abc\r", 32)
    check(lib, lib.ru_any(f, b"\r\n", 32), lib.OK, b"def\n", 32)


def test_ALX1514_P4_any_exactly_full_line_with_terminator_ok(lib, make_fifo):
    # The D1 shape: line exactly fills the FIFO, terminator is the last byte.
    f = make_fifo(8)
    lib.write(f, b"abcdefg\r")
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"abcdefg\r", 16)


def test_ALX1514_P4_any_not_full_no_delim_is_nodelim_not_toolong(lib, make_fifo):
    f = make_fifo(8)
    lib.write(f, b"AAAAAAA")                     # 7 of 8 - not full yet
    check(lib, lib.ru_any(f, b"\r\n", 32), lib.ERR_NO_DELIM, b"", 32)
    assert lib.entries(f) == 7                   # line may still complete


# =====================================================================
# P5 - ReadStrUntil (sequence): basic contract
# =====================================================================

def test_ALX1514_P5_seq_crlf_basic_terminator_included(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\r\n")
    check(lib, lib.ru(f, b"\r\n", 16), lib.OK, b"get\r\n", 16)
    assert lib.entries(f) == 0


def test_ALX1514_P5_seq_false_start_cr_then_crlf(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"a\rb\r\n")                    # first CR is NOT the sequence
    check(lib, lib.ru(f, b"\r\n", 16), lib.OK, b"a\rb\r\n", 16)


def test_ALX1514_P5_seq_partial_delim_waits_fifo_untouched(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"get\r")                       # CR at end, LF still in flight
    check(lib, lib.ru(f, b"\r\n", 16), lib.ERR_NO_DELIM, b"", 16)
    assert lib.entries(f) == 4
    lib.write(f, b"\n")
    check(lib, lib.ru(f, b"\r\n", 16), lib.OK, b"get\r\n", 16)


def test_ALX1514_P5_seq_repeated_char_delim(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"aaa")                         # delim "aa": leftmost match at 0
    check(lib, lib.ru(f, b"aa", 16), lib.OK, b"aa", 16)  # empty line, terminator only
    check(lib, lib.ru(f, b"aa", 16), lib.ERR_NO_DELIM, b"", 16)
    assert lib.entries(f) == 1                   # lone "a" stays


def test_ALX1514_P5_seq_empty_line_is_terminator_only(lib, make_fifo):
    f = make_fifo(32)
    lib.write(f, b"\r\n")
    check(lib, lib.ru(f, b"\r\n", 16), lib.OK, b"\r\n", 16)  # lenActual == 2


# =====================================================================
# P6 - ReadStrUntil (sequence): ErrTooLong A + flood B
# =====================================================================

@pytest.mark.parametrize("payload,fits", [
    (b"abc", True),      # line = 5 incl. CRLF = len-1 -> exactly fits
    (b"abcd", False),    # line = 6 incl. CRLF > len-1 -> too long
])
def test_ALX1514_P6_seq_len_boundary(lib, make_fifo, payload, fits):
    f = make_fifo(32)
    lib.write(f, payload + b"\r\n")
    if fits:
        check(lib, lib.ru(f, b"\r\n", 6), lib.OK, payload + b"\r\n", 6)
    else:
        check(lib, lib.ru(f, b"\r\n", 6), lib.ERR_TOO_LONG, b"", 6)
        assert lib.entries(f) == 0


def test_ALX1514_P6_seq_flood_full_ending_with_partial_delim(lib, make_fifo):
    f = make_fifo(8)
    lib.write(f, b"AAAAAAA\r")                   # full; "\r" alone can never become "\r\n"
    check(lib, lib.ru(f, b"\r\n", 32), lib.ERR_TOO_LONG, b"", 32)
    assert lib.entries(f) == 0
    lib.write(f, b"ok\r\n")
    check(lib, lib.ru(f, b"\r\n", 32), lib.OK, b"ok\r\n", 32)


# =====================================================================
# P7 - wrap-around: core scenarios at rotated head/tail positions
# =====================================================================

WRAP_ROTATIONS = [1, 3, 7]  # tail positions in a buffLen=8 FIFO


@pytest.mark.parametrize("rot", WRAP_ROTATIONS)
def test_ALX1514_P7_wrap_any_basic(lib, make_fifo, rot):
    f = make_fifo(8, rotate=rot)
    lib.write(f, b"get\r")                       # line spans the wrap for rot >= 5
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.OK, b"get\r", 16)


@pytest.mark.parametrize("rot", WRAP_ROTATIONS)
def test_ALX1514_P7_wrap_any_toolong(lib, make_fifo, rot):
    f = make_fifo(8, rotate=rot)
    lib.write(f, b"abcdef\r")                    # 7 incl. CR > len-1 = 3
    check(lib, lib.ru_any(f, b"\r\n", 4), lib.ERR_TOO_LONG, b"", 4)
    assert lib.entries(f) == 0


@pytest.mark.parametrize("rot", WRAP_ROTATIONS)
def test_ALX1514_P7_wrap_any_flood(lib, make_fifo, rot):
    f = make_fifo(8, rotate=rot)
    assert lib.write(f, b"XXXXXXXX") == lib.OK
    check(lib, lib.ru_any(f, b"\r\n", 16), lib.ERR_TOO_LONG, b"", 16)
    assert lib.entries(f) == 0


@pytest.mark.parametrize("rot", WRAP_ROTATIONS)
def test_ALX1514_P7_wrap_seq_delim_split_across_wrap(lib, make_fifo, rot):
    # For rot=7 the "\r\n" sequence itself straddles buff[7] -> buff[0].
    f = make_fifo(8, rotate=rot)
    lib.write(f, b"ab\r\n")
    check(lib, lib.ru(f, b"\r\n", 16), lib.OK, b"ab\r\n", 16)


@pytest.mark.parametrize("rot", WRAP_ROTATIONS)
def test_ALX1514_P7_wrap_seq_partial_delim_waits(lib, make_fifo, rot):
    f = make_fifo(8, rotate=rot)
    lib.write(f, b"ab\r")
    check(lib, lib.ru(f, b"\r\n", 16), lib.ERR_NO_DELIM, b"", 16)
    lib.write(f, b"\n")
    check(lib, lib.ru(f, b"\r\n", 16), lib.OK, b"ab\r\n", 16)


# =====================================================================
# P8 - regression: untouched functions keep their current contract
# =====================================================================

def test_ALX1514_P8_write_read_roundtrip(lib, make_fifo):
    f = make_fifo(8)
    assert lib.write(f, b"abc") == lib.OK
    assert lib.entries(f) == 3
    status, data = lib.read(f, 3)
    assert status == lib.OK and data == b"abc"
    assert lib.entries(f) == 0


def test_ALX1514_P8_flush(lib, make_fifo):
    f = make_fifo(8)
    lib.write(f, b"abc")
    lib.c.AlxFifo_Flush(f)
    assert lib.entries(f) == 0


def test_ALX1514_P8_partial_read_current_contract_pinned(lib, make_fifo):
    # D4 accepted as-is: short read consumes what exists, returns ErrEmpty.
    f = make_fifo(8)
    lib.write(f, b"abc")
    status, _ = lib.read(f, 5)
    assert status == lib.ERR_EMPTY
    assert lib.entries(f) == 0


def test_ALX1514_P8_partial_write_current_contract_pinned(lib, make_fifo):
    # D4/D5 accepted as-is: write commits until full, returns ErrFull.
    f = make_fifo(4)
    assert lib.write(f, b"abcdef") == lib.ERR_FULL
    assert lib.entries(f) == 4
    status, data = lib.read(f, 4)
    assert data == b"abcd"


def test_ALX1514_P8_rewind_audit_trace(lib, make_fifo):
    # The verified Rewind wrap trace from the ALX-1514 module audit.
    f = make_fifo(4)
    lib.write(f, b"ABCD")
    lib.read(f, 2)                               # consume A, B
    lib.write(f, b"EF")                          # head wraps
    lib.read(f, 2)                               # consume C, D
    assert lib.c.AlxFifo_Rewind(f, 4) == 2       # clamped to unused space
    status, data = lib.read(f, 4)
    assert status == lib.OK and data == b"CDEF"


# =====================================================================
# P9 - property/model test: random ops vs Python reference model
# =====================================================================

class ModelFifo:
    """Reference model of the locked contract (ReadStrUntilAny path)."""

    def __init__(self, capacity: int):
        self.cap = capacity
        self.buf = bytearray()

    def write(self, data: bytes) -> str:
        for i, b in enumerate(data):
            if len(self.buf) >= self.cap:
                return "ERR_FULL"                # partial commit, rest dropped
            self.buf.append(b)
        return "OK"

    def read_until_any(self, delim_set: bytes, ln: int):
        if not self.buf:
            return "ERR_EMPTY", b""
        idx = next((i for i, b in enumerate(self.buf)
                    if b != 0 and b in delim_set), None)
        if idx is None:
            if len(self.buf) == self.cap:
                self.buf.clear()                 # flood: undeliverable line
                return "ERR_TOO_LONG", b""
            return "ERR_NO_DELIM", b""
        line_len = idx + 1
        if line_len > ln - 1:
            del self.buf[:line_len]              # oversized line discarded
            return "ERR_TOO_LONG", b""
        line = bytes(self.buf[:line_len])
        del self.buf[:line_len]
        return "OK", line


@pytest.mark.parametrize("seed", [1, 2, 3])
def test_ALX1514_P9_property_model_random_ops(lib, make_fifo, seed):
    rng = random.Random(seed)
    cap = 16
    f = make_fifo(cap)
    model = ModelFifo(cap)
    status_name = {lib.OK: "OK", lib.ERR_FULL: "ERR_FULL",
                   lib.ERR_EMPTY: "ERR_EMPTY", lib.ERR_NO_DELIM: "ERR_NO_DELIM",
                   lib.ERR_TOO_LONG: "ERR_TOO_LONG"}
    alphabet = b"ab\r\n\x00;"
    for step in range(2000):
        op = rng.random()
        if op < 0.5:
            chunk = bytes(rng.choice(alphabet) for _ in range(rng.randint(1, 6)))
            got = status_name[lib.write(f, chunk)]
            exp = model.write(chunk)
            assert got == exp, f"seed {seed} step {step} write({chunk!r}): {got} != {exp}"
        else:
            ln = rng.choice([4, 8, 32])
            status, content, la, raw = lib.ru_any(f, b"\r\n", ln)
            exp_status, exp_line = model.read_until_any(b"\r\n", ln)
            got_status = status_name[status]
            assert got_status == exp_status, \
                f"seed {seed} step {step} ru_any(len={ln}): {got_status} != {exp_status}"
            exp_la = len(exp_line)
            assert la == exp_la, f"seed {seed} step {step}: lenActual {la} != {exp_la}"
            assert raw[:la] == exp_line and raw[la] == 0
        assert lib.entries(f) == len(model.buf), \
            f"seed {seed} step {step}: entries {lib.entries(f)} != model {len(model.buf)}"


# =====================================================================
# P10 - str hygiene on every path
# =====================================================================

def test_ALX1514_P10_str_empty_and_terminated_on_all_error_paths(lib, make_fifo):
    f = make_fifo(8)
    # ErrEmpty
    check(lib, lib.ru_any(f, b"\r\n", 8), lib.ERR_EMPTY, b"", 8)
    # ErrNoDelim
    lib.write(f, b"abc")
    check(lib, lib.ru_any(f, b"\r\n", 8), lib.ERR_NO_DELIM, b"", 8)
    # ErrTooLong A (line = "abcdefg\r" = 8 incl. CR > len-1 = 7)
    lib.write(f, b"defg\r")
    check(lib, lib.ru_any(f, b"\r\n", 8), lib.ERR_TOO_LONG, b"", 8)
    assert lib.entries(f) == 0
    # ErrTooLong B
    lib.write(f, b"XXXXXXXX")
    check(lib, lib.ru_any(f, b"\r\n", 8), lib.ERR_TOO_LONG, b"", 8)


def test_ALX1514_P10_min_len_one_only_nul_fits(lib, make_fifo):
    f = make_fifo(8)
    lib.write(f, b"\r")                          # even the empty line (1 char) can't fit
    check(lib, lib.ru_any(f, b"\r\n", 1), lib.ERR_TOO_LONG, b"", 1)
    assert lib.entries(f) == 0
