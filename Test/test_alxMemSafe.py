"""ALX-1513 - alxMemSafe PC unit tests (L5, Tier 2 over alxMemRawFake).

AlxMemSafe keeps two copies of a record (copy A at copyAddrA, copy B at copyAddrB), each
followed by its CRC. Contract under test:

  Read
  - both copies valid and equal          -> use A, status BothOkSame_UseA, nothing written
  - both valid but different             -> use A, B rewritten from A, status BothOkDiff_UseA
  - A invalid, B valid                   -> use B, A rewritten from B, status AErrBOk_UseB
  - A valid, B invalid                   -> use A, B rewritten from A, status AOkBErr_UseA
  - both invalid (blank or corrupt)      -> status BothCopyErr, out buffer and flash untouched
  - a failing raw Init/Read/DeInit is retried memSafeReadWriteNumOfTries times
  Write
  - A is written first, then B, each as data + CRC; a failing raw op restarts the pair
  - numOfTries/timeout of the raw layer are passed through unchanged
  Power loss (the reason the two copies exist, and the reason they sit in different RWWEE rows)
  - a cut during A leaves the OLD record readable from B, and A gets repaired on the next read
  - a cut during B leaves the NEW record readable from A
  - any interleaving of writes and cuts: a reboot always reads the last committed record or
    the one before it, never a third value, never nothing

Layout on the fake flash mirrors the product: A at 0x000 (RWWEE row 0), B at 0x100 (row 1),
copyLen 5, CRC32. Test group P8 = ALX-1513 safe-store proofs.
"""

import random
import zlib

import pytest

pytestmark = pytest.mark.unit

A, B = 0x000, 0x100
REC = 5 + 4              # copyLen + CRC32


def blob(data: bytes) -> bytes:
    """What a valid copy looks like in flash: payload + little-endian CRC-32 (reference: zlib)."""
    return data + (zlib.crc32(data) & 0xFFFFFFFF).to_bytes(4, "little")


def flip(lib, addr: int, byte_idx: int = 1, bit: int = 0):
    b = bytearray(lib.peek(addr, REC))
    b[byte_idx] ^= 1 << bit
    lib.poke(addr, bytes(b))


V1 = bytes([0, 0, 50, 100, 70])
V2 = bytes([7, 9, 51, 18, 69])
V3 = bytes([100, 100, 0, 0, 1])


# =====================================================================
# P8 - write then read, the happy path and its flash image
# =====================================================================

def test_ALX1513_P8_write_puts_payload_plus_crc32_in_both_copies(flash, make_store):
    ctx = make_store()
    assert flash.write(ctx, V1) == flash.OK
    assert flash.peek(A, REC) == blob(V1)
    assert flash.peek(B, REC) == blob(V1)
    assert flash.peek(REC, 0x100 - REC) == b"\xff" * (0x100 - REC), "nothing else touched between the copies"
    assert flash.flags(ctx)["write_done"] is True and flash.flags(ctx)["write_err"] is False


def test_ALX1513_P8_read_after_write_uses_copy_a_and_writes_nothing(flash, make_store):
    ctx = make_store()
    assert flash.write(ctx, V2) == flash.OK
    writes = flash.count(flash.WRITE)
    status, data = flash.read(ctx)
    assert status == flash.BOTH_OK_SAME_USE_A
    assert data == V2
    assert flash.count(flash.WRITE) == writes, "a clean read must not write"
    assert flash.flags(ctx)["read_done"] is True and flash.flags(ctx)["read_err"] is False


def test_ALX1513_P8_write_order_is_copy_a_then_copy_b(flash, make_store):
    ctx = make_store()
    assert flash.write(ctx, V1) == flash.OK
    assert flash.count(flash.WRITE) == 2
    assert flash.last_write() == (B, REC), "B is the second and last write"


def test_ALX1513_P8_raw_layer_gets_the_configured_tries_and_timeout(flash, make_store):
    """Both copy writes and every read carry the SAME (numOfTries, timeout_ms) and nothing else - the fake
    compares every call with the first one (an argument swap in one of the three Write calls survived the
    05.09 mutation run because only the last call was inspected)."""
    ctx = make_store(raw_tries=7)
    assert flash.write(ctx, V1) == flash.OK
    assert flash.last_raw_args() == (7, 100)
    assert flash.read(ctx)[0] == flash.BOTH_OK_SAME_USE_A
    assert flash.args_mismatch() == 0, "a raw call got other tries/timeout than the configured ones"
    assert flash.not_init_calls() == 0, "a raw Read/Write was issued without a successful Init"


@pytest.mark.parametrize("case", ["diff", "b-bad", "a-bad"])
def test_ALX1513_P8_repair_write_carries_the_configured_tries_and_timeout(flash, make_store, case):
    ctx = make_store(raw_tries=7)
    _repair_case(flash, ctx, case)
    status, data = flash.read(ctx)
    assert status == getattr(flash, REPAIR_EXPECT[case][0]) and data == V1
    assert flash.args_mismatch() == 0, "the repair write got other tries/timeout than the configured ones"
    assert flash.not_init_calls() == 0


def test_ALX1513_P8_reboot_reads_what_was_written(flash, make_store):
    boot1 = make_store()
    assert flash.write(boot1, V3) == flash.OK
    boot2 = make_store()                      # new object over the same flash = reboot
    status, data = flash.read(boot2)
    assert status == flash.BOTH_OK_SAME_USE_A and data == V3


def test_ALX1513_P8_all_flags_are_false_right_after_ctor(flash, make_store):
    """The helper poisons the struct with 0xFF before the Ctor: every flag must be cleared by the Ctor itself
    (a removed initialisation survived the 05.09 mutation run because the flags were only read after an op)."""
    ctx = make_store()
    assert flash.flags(ctx) == {"read_done": False, "read_err": False, "write_done": False, "write_err": False}


# =====================================================================
# P8 - the decision table of Read
# =====================================================================

def test_ALX1513_P8_blank_flash_reads_both_copies_invalid_and_touches_nothing(flash, make_store):
    ctx = make_store()
    status, data = flash.read(ctx)
    assert status == flash.BOTH_ERR
    assert data == bytes([flash.POISON] * 5), "out buffer must stay untouched"
    assert flash.count(flash.WRITE) == 0
    assert flash.peek(A, REC) == b"\xff" * REC and flash.peek(B, REC) == b"\xff" * REC


def test_ALX1513_P8_characterization_read_flags_after_both_invalid(flash, make_store):
    """CHARACTERIZATION (not a requirement): after BothCopyErr the module still reports
    isReadDone=true / isReadErr=false - the return status is the only truthful signal.
    Documented so a change is noticed; callers must check the status, not the flags."""
    ctx = make_store()
    assert flash.read(ctx)[0] == flash.BOTH_ERR
    assert flash.flags(ctx) == {"read_done": True, "read_err": False, "write_done": False, "write_err": False}


@pytest.mark.parametrize("byte_idx", [0, 4, 5, 8], ids=["data-first", "data-last", "crc-first", "crc-last"])
def test_ALX1513_P8_corrupt_copy_a_is_repaired_from_b(flash, make_store, byte_idx):
    ctx = make_store()
    assert flash.write(ctx, V1) == flash.OK
    flip(flash, A, byte_idx)
    status, data = flash.read(ctx)
    assert status == flash.A_ERR_B_OK_USE_B
    assert data == V1
    assert flash.peek(A, REC) == blob(V1), "A repaired from B"
    assert flash.peek(B, REC) == blob(V1)


@pytest.mark.parametrize("byte_idx", [0, 4, 5, 8], ids=["data-first", "data-last", "crc-first", "crc-last"])
def test_ALX1513_P8_corrupt_copy_b_is_repaired_from_a(flash, make_store, byte_idx):
    ctx = make_store()
    assert flash.write(ctx, V1) == flash.OK
    flip(flash, B, byte_idx)
    status, data = flash.read(ctx)
    assert status == flash.A_OK_B_ERR_USE_A
    assert data == V1
    assert flash.peek(B, REC) == blob(V1), "B repaired from A"


def test_ALX1513_P8_two_valid_but_different_copies_a_wins_and_b_is_rewritten(flash, make_store):
    """A is always written first, so after a cut between the two writes A holds the newer record."""
    ctx = make_store()
    assert flash.write(ctx, V1) == flash.OK
    flash.poke(B, blob(V2))                  # a valid but older/different record in B
    status, data = flash.read(ctx)
    assert status == flash.BOTH_OK_DIFF_USE_A
    assert data == V1
    assert flash.peek(B, REC) == blob(V1)


def test_ALX1513_P8_both_copies_corrupt_reads_error_and_repairs_nothing(flash, make_store):
    ctx = make_store()
    assert flash.write(ctx, V1) == flash.OK
    flip(flash, A, 2)
    flip(flash, B, 7)
    a, b = flash.peek(A, REC), flash.peek(B, REC)
    status, data = flash.read(ctx)
    assert status == flash.BOTH_ERR
    assert data == bytes([flash.POISON] * 5)
    assert (flash.peek(A, REC), flash.peek(B, REC)) == (a, b), "nothing rewritten"


def test_ALX1513_P8_all_zero_copies_are_invalid(flash, make_store):
    """Zeroed flash (e.g. after a bad programmer) is not a record: CRC-32 of zeros is not zero."""
    ctx = make_store()
    flash.fake_fill(0x00)
    assert flash.read(ctx)[0] == flash.BOTH_ERR


# =====================================================================
# P8 - retries of the raw layer
# =====================================================================

@pytest.mark.parametrize("kind", ["INIT", "READ", "DEINIT"])
def test_ALX1513_P8_one_failing_raw_call_during_read_is_retried(flash, make_store, kind):
    ctx = make_store(tries=3)
    assert flash.write(ctx, V1) == flash.OK
    k = getattr(flash, kind)
    flash.fail_at(k, flash.count(k) + 1)      # the very next call of that kind fails once
    status, data = flash.read(ctx)
    assert status == flash.BOTH_OK_SAME_USE_A and data == V1


@pytest.mark.parametrize("kind", ["INIT", "WRITE", "DEINIT"])
def test_ALX1513_P8_one_failing_raw_call_during_write_is_retried(flash, make_store, kind):
    ctx = make_store(tries=3)
    k = getattr(flash, kind)
    flash.fail_at(k, 1)
    assert flash.write(ctx, V2) == flash.OK
    assert flash.peek(A, REC) == blob(V2) and flash.peek(B, REC) == blob(V2)


def test_ALX1513_P8_raw_init_failing_always_gives_error_after_the_configured_tries(flash, make_store):
    ctx = make_store(tries=3)
    flash.fail_at(flash.INIT, flash.ALWAYS)
    assert flash.write(ctx, V1) == flash.ERR
    assert flash.flags(ctx)["write_err"] is True and flash.flags(ctx)["write_done"] is False
    assert flash.count(flash.INIT) == 3, "one Init attempt per memSafeReadWriteNumOfTries"
    assert flash.count(flash.WRITE) == 0
    flash.fake_reset()
    flash.fail_at(flash.INIT, flash.ALWAYS)
    assert flash.read(ctx)[0] == flash.ERR
    assert flash.count(flash.INIT) == 3


def test_ALX1513_P8_raw_write_failing_always_leaves_old_record_intact(flash, make_store):
    ctx = make_store(tries=2)
    assert flash.write(ctx, V1) == flash.OK
    flash.fail_at(flash.WRITE, flash.ALWAYS)
    assert flash.write(ctx, V2) == flash.ERR
    assert flash.flags(ctx)["write_err"] is True
    assert flash.peek(A, REC) == blob(V1) and flash.peek(B, REC) == blob(V1)


# =====================================================================
# P8 - power loss: the property the two copies exist for
# =====================================================================

@pytest.mark.parametrize("landed", list(range(0, REC)))
def test_ALX1513_P8_power_loss_during_copy_a_keeps_the_old_record(flash, make_store, landed):
    boot1 = make_store()
    assert flash.write(boot1, V1) == flash.OK
    flash.power_loss_at(1, landed)           # the next write (copy A of V2) is cut after `landed` bytes
    assert flash.write(boot1, V2) == flash.ERR
    assert flash.power_lost() is True
    flash.power_on()
    boot2 = make_store()
    status, data = flash.read(boot2)
    assert data == V1, f"landed={landed}: old record must survive"
    assert status in (flash.BOTH_OK_SAME_USE_A, flash.A_ERR_B_OK_USE_B)
    assert flash.peek(A, REC) == blob(V1), "A repaired (or never damaged)"


@pytest.mark.parametrize("landed", list(range(0, REC)))
def test_ALX1513_P8_power_loss_during_copy_b_keeps_the_new_record(flash, make_store, landed):
    boot1 = make_store()
    assert flash.write(boot1, V1) == flash.OK
    flash.power_loss_at(2, landed)           # copy A of V2 lands, copy B is cut
    assert flash.write(boot1, V2) == flash.ERR
    flash.power_on()
    boot2 = make_store()
    status, data = flash.read(boot2)
    assert data == V2, f"landed={landed}: A holds the new record"
    assert status in (flash.BOTH_OK_DIFF_USE_A, flash.A_OK_B_ERR_USE_A)
    assert flash.peek(B, REC) == blob(V2), "B repaired"


def test_ALX1513_P8_property_reboot_reads_last_or_previous_record_never_a_third(flash, make_store):
    """Random writes, each optionally cut at a random point of copy A or copy B, each followed
    by a reboot: the record read is always the last committed one or the one before it, and a
    reboot never finds both copies invalid once a first record exists."""
    rnd = random.Random(20260903)
    committed = None
    ctx = make_store()
    for step in range(120):
        val = bytes(rnd.randrange(0, 101) for _ in range(5))
        cut = rnd.choice([None, None, "A", "B"])
        if cut is not None:
            flash.power_loss_at(1 if cut == "A" else 2, rnd.randrange(0, REC + 1))
        st = flash.write(ctx, val)
        if cut is None:
            assert st == flash.OK, f"step {step}: clean write must succeed"
        flash.power_on()
        ctx = make_store()                   # reboot
        status, data = flash.read(ctx)
        allowed = {val} if committed is None else {val, committed}
        if committed is None and cut == "A":
            allowed.add(bytes([flash.POISON] * 5))  # nothing was ever committed and the first A was cut
        assert data in allowed, f"step {step}: read {data!r}, allowed {allowed}, cut={cut}"
        if data == val:
            committed = val
        if committed is not None:
            assert status != flash.BOTH_ERR, f"step {step}: a committed record must always be readable"


# =====================================================================
# P8 - why the copies live in different RWWEE rows (row-erase model)
# =====================================================================

def test_ALX1513_P8_row_erase_model_product_layout_a_and_b_in_own_rows_is_safe(flash, make_store):
    flash.row_erase_model(True, 256)
    ctx = make_store(0x000, 0x100)
    assert flash.write(ctx, V1) == flash.OK
    assert flash.read(ctx)[0] == flash.BOTH_OK_SAME_USE_A
    flash.power_loss_at(2, 3)                # cut during B
    assert flash.write(ctx, V2) == flash.ERR
    flash.power_on()
    status, data = flash.read(make_store(0x000, 0x100))
    assert data == V2 and status in (flash.BOTH_OK_DIFF_USE_A, flash.A_OK_B_ERR_USE_A)


def test_ALX1513_P8_row_erase_model_copies_in_the_same_row_destroy_each_other(flash, make_store):
    """The mmx-ec2 layout (A=0, B=16) is fine on an I2C EEPROM but NOT on RWWEE: writing B
    erases the row that holds A. A read then never finds both copies valid, every read
    triggers a repair write, and a cut during B loses the record completely. This is why the
    product uses 0x000 and 0x100."""
    flash.row_erase_model(True, 256)
    ctx = make_store(0, 16)
    assert flash.write(ctx, V1) == flash.OK
    status, data = flash.read(ctx)
    assert status != flash.BOTH_OK_SAME_USE_A, "both copies can never be valid at the same time"
    assert data == V1                        # still readable today - via the repair ping-pong
    flash.power_loss_at(2, 3)                # write V2: A lands (erasing B), B is cut
    flash.write(ctx, V2)
    flash.power_on()
    assert flash.read(make_store(0, 16))[0] == flash.BOTH_ERR, "record lost - the failure the row layout prevents"


# =====================================================================
# P8 - coverage-driven hardening: every raw call site of Read/Write is retried, including the
#      repair writes (llvm-cov of the first suite showed these branches unexecuted)
# =====================================================================

@pytest.mark.parametrize("kind,nth", [("INIT", 2), ("READ", 2), ("DEINIT", 1), ("DEINIT", 2)],
                         ids=["init-before-B", "read-B", "deinit-after-A", "deinit-after-B"])
def test_ALX1513_P8_read_retries_a_failure_at_every_raw_call_site(flash, make_store, kind, nth):
    ctx = make_store(tries=3)
    assert flash.write(ctx, V1) == flash.OK
    k = getattr(flash, kind)
    inits = flash.count(flash.INIT)
    flash.fail_at(k, flash.count(k) + nth)
    status, data = flash.read(ctx)
    assert status == flash.BOTH_OK_SAME_USE_A and data == V1
    assert flash.count(flash.INIT) >= inits + 3, "a failed raw call must restart the read (2 Inits per attempt)"
    assert flash.not_init_calls() == 0, "no Read/Write may be issued after a failed Init"


@pytest.mark.parametrize("kind,nth", [("INIT", 2), ("WRITE", 2), ("DEINIT", 1), ("DEINIT", 2)],
                         ids=["init-before-B", "write-B", "deinit-after-A", "deinit-after-B"])
def test_ALX1513_P8_write_retries_a_failure_at_every_raw_call_site(flash, make_store, kind, nth):
    ctx = make_store(tries=3)
    k = getattr(flash, kind)
    flash.fail_at(k, nth)
    assert flash.write(ctx, V2) == flash.OK
    assert flash.peek(A, REC) == blob(V2) and flash.peek(B, REC) == blob(V2)
    assert flash.count(flash.INIT) >= 3, "a failed raw call must restart the pair (2 Inits per attempt)"
    assert flash.not_init_calls() == 0, "no Read/Write may be issued after a failed Init"


def _repair_case(flash, ctx, case: str):
    """Prepare the three repair situations of Read: both valid but different / A ok B bad / A bad B ok."""
    assert flash.write(ctx, V1) == flash.OK
    if case == "diff":
        flash.poke(B, blob(V2))
    elif case == "b-bad":
        flip(flash, B, 2)
    else:
        flip(flash, A, 2)


REPAIR_EXPECT = {
    "diff": ("BOTH_OK_DIFF_USE_A", V1, B),
    "b-bad": ("A_OK_B_ERR_USE_A", V1, B),
    "a-bad": ("A_ERR_B_OK_USE_B", V1, A),
}


@pytest.mark.parametrize("case", ["diff", "b-bad", "a-bad"])
@pytest.mark.parametrize("kind,nth", [("INIT", 3), ("WRITE", 1), ("DEINIT", 3)],
                         ids=["init-before-repair", "repair-write", "deinit-after-repair"])
def test_ALX1513_P8_repair_write_path_retries_a_failure_and_still_repairs(flash, make_store, case, kind, nth):
    """The repair (rewriting the bad copy from the good one) is the third Init/DeInit and the first
    Write inside one Read; a failure there restarts the whole read and the repair lands on the retry."""
    ctx = make_store(tries=3)
    _repair_case(flash, ctx, case)
    k = getattr(flash, kind)
    flash.fail_at(k, flash.count(k) + nth)
    status, data = flash.read(ctx)
    exp_status, exp_data, repaired = REPAIR_EXPECT[case]
    if kind == "DEINIT":
        exp_status = "BOTH_OK_SAME_USE_A"     # the repair had already landed: the retry finds two equal copies
    assert status == getattr(flash, exp_status)
    assert data == exp_data
    assert flash.peek(repaired, REC) == blob(V1), f"{case}: repaired copy must hold the good record"


@pytest.mark.parametrize("case", ["diff", "b-bad", "a-bad"])
def test_ALX1513_P8_repair_write_failing_always_gives_error_and_leaves_flash_as_it_was(flash, make_store, case):
    ctx = make_store(tries=2)
    _repair_case(flash, ctx, case)
    before = (flash.peek(A, REC), flash.peek(B, REC))
    flash.fail_at(flash.WRITE, flash.ALWAYS)
    status, data = flash.read(ctx)
    assert status == flash.ERR
    assert data == bytes([flash.POISON] * 5), "no record delivered when the repair cannot be completed"
    assert (flash.peek(A, REC), flash.peek(B, REC)) == before
