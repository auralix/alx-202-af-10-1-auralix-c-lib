"""ALX-1513 - alxParamStore PC unit tests (L5, Tier 2: real store over group, MemSafe and the fake).

The store is the auto-save state machine the product runs once per main-loop pass:
Checking (compare the items with what is stored, start a write on a difference) ->
Writing (wait for the MemSafe write) -> Checking. Contract:

  - Init = the group's Init (defaults on a blank store, stored values otherwise)
  - a changed item is written within three Handle passes - the pass that sees the change only
    refreshes the working buffer, the next one writes A and B (blocking), the third completes -
    i.e. one loop period after the set-param on the device; a reboot then reads it back
  - unchanged items cause no flash writes (endurance) - including right after a blank boot
    (the stale working-buffer finding of 03.09 was fixed in alxParamGroup.c Init: valBuff is
    refreshed from the final stored record after the read/reset-to-default switch)
  - a failing write puts the store into its error state (IsErr) and leaves flash as it was

Test group P11 = ALX-1513 store state-machine proofs.
"""

import random
import zlib

import pytest

pytestmark = pytest.mark.unit

A, B = 0x000, 0x100
REC = 9
DEFAULTS = [0, 0, 50, 100, 70]


def blob(vals) -> bytes:
    data = bytes(vals)
    return data + (zlib.crc32(data) & 0xFFFFFFFF).to_bytes(4, "little")


def booted(flash, make_store):
    ctx = make_store()
    assert flash.store_init(ctx) == flash.OK
    flash.store_handle(ctx, 6)               # settle: Init -> Checking, a few steady passes
    return ctx


# =====================================================================
# P11 - Init
# =====================================================================

def test_ALX1513_P11_init_on_blank_flash_is_ok_with_defaults(flash, make_store):
    ctx = make_store()
    assert flash.store_init(ctx) == flash.OK
    assert flash.store_err(ctx) is False
    assert flash.items(ctx) == DEFAULTS
    assert flash.peek(A, REC) == blob(DEFAULTS)


def test_ALX1513_P11_init_with_raw_layer_failing_always_is_error(flash, make_store):
    flash.fail_at(flash.INIT, flash.ALWAYS)
    ctx = make_store()
    assert flash.store_init(ctx) == flash.ERR


# =====================================================================
# P11 - the endurance rule: no change, no write
# =====================================================================

def test_ALX1513_P11_no_change_means_no_flash_write_after_blank_boot(flash, make_store):
    ctx = make_store()
    assert flash.store_init(ctx) == flash.OK
    writes = flash.count(flash.WRITE)        # the defaults written by Init
    flash.store_handle(ctx, 3)
    assert flash.count(flash.WRITE) == writes, "no item changed - the store must not write"
    assert flash.peek(A, REC) == blob(DEFAULTS)


def test_ALX1513_P11_flash_never_holds_an_all_zero_record_after_blank_boot(flash, make_store):
    """The power-loss window of the finding: after Init and the first Checking pass the record in
    flash must still be the defaults, not zeros."""
    ctx = make_store()
    assert flash.store_init(ctx) == flash.OK
    flash.store_handle(ctx, 2)               # pass 1: Init -> Checking; pass 2: the first Checking pass
    assert flash.peek(A, REC) == blob(DEFAULTS)


def test_ALX1513_P11_no_change_means_no_flash_write_in_steady_state(flash, make_store):
    ctx = booted(flash, make_store)
    writes = flash.count(flash.WRITE)
    flash.store_handle(ctx, 20)
    assert flash.count(flash.WRITE) == writes


# =====================================================================
# P11 - a change is stored within three passes and survives a reboot
# =====================================================================

@pytest.mark.parametrize("index", range(5))
def test_ALX1513_P11_changed_item_is_stored_within_three_passes(flash, make_store, index):
    ctx = booted(flash, make_store)
    expected = list(DEFAULTS)
    expected[index] = 33
    assert flash.item_set(ctx, index, 33) == flash.OK
    writes = flash.count(flash.WRITE)
    flash.store_handle(ctx, 1)               # pass 1: no diff yet (working buffer stale), refresh it
    assert flash.count(flash.WRITE) == writes
    flash.store_handle(ctx, 1)               # pass 2: diff -> write A and B (blocking MemSafe)
    assert flash.count(flash.WRITE) == writes + 2
    assert flash.peek(A, REC) == blob(expected) and flash.peek(B, REC) == blob(expected)
    flash.store_handle(ctx, 1)               # pass 3: Writing -> done -> Checking
    assert flash.store_err(ctx) is False
    boot2 = make_store()
    assert flash.store_init(boot2) == flash.OK
    assert flash.items(boot2) == expected


def test_ALX1513_P11_two_items_changed_in_one_pass_are_stored_as_one_record(flash, make_store):
    ctx = booted(flash, make_store)
    assert flash.item_set(ctx, 0, 11) == flash.OK
    assert flash.item_set(ctx, 4, 44) == flash.OK
    writes = flash.count(flash.WRITE)
    flash.store_handle(ctx, 3)
    assert flash.count(flash.WRITE) == writes + 2, "one record = one A + one B write"
    assert flash.peek(A, REC) == blob([11, 0, 50, 100, 44])


def test_ALX1513_P11_setting_the_same_value_again_writes_nothing(flash, make_store):
    ctx = booted(flash, make_store)
    writes = flash.count(flash.WRITE)
    assert flash.item_set(ctx, 2, 50) == flash.OK        # 50 is already the value
    flash.store_handle(ctx, 3)
    assert flash.count(flash.WRITE) == writes


def test_ALX1513_P11_out_of_range_set_is_ignored_and_nothing_is_written(flash, make_store):
    ctx = booted(flash, make_store)
    writes = flash.count(flash.WRITE)
    assert flash.item_set(ctx, 1, 101) != flash.OK
    assert flash.items(ctx)[1] == 0
    flash.store_handle(ctx, 3)
    assert flash.count(flash.WRITE) == writes


def test_ALX1513_P11_write_failure_puts_the_store_in_error_and_keeps_old_flash(flash, make_store):
    ctx = booted(flash, make_store)
    flash.fail_at(flash.WRITE, flash.ALWAYS)
    assert flash.item_set(ctx, 3, 18) == flash.OK
    flash.store_handle(ctx, 3)
    assert flash.store_err(ctx) is True
    assert flash.peek(A, REC) == blob(DEFAULTS) and flash.peek(B, REC) == blob(DEFAULTS)
    assert flash.items(ctx)[3] == 18, "the RAM value stays what the user set"
    flash.store_handle(ctx, 5)
    assert flash.store_err(ctx) is True, "error state is sticky"


def test_ALX1513_P11_property_random_sets_survive_reboots(flash, make_store):
    rnd = random.Random(1513)
    ctx = booted(flash, make_store)
    expected = list(DEFAULTS)
    for step in range(40):
        idx = rnd.randrange(5)
        val = rnd.randrange(0, 101)
        assert flash.item_set(ctx, idx, val) == flash.OK
        expected[idx] = val
        flash.store_handle(ctx, 3)
        if rnd.random() < 0.3:
            ctx = make_store()               # reboot
            assert flash.store_init(ctx) == flash.OK
            flash.store_handle(ctx, 3)
        assert flash.items(ctx) == expected, f"step {step}"
    assert flash.peek(A, REC) == blob(expected) and flash.peek(B, REC) == blob(expected)
