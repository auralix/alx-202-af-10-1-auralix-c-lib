"""ALX-1513 - alxParamGroup PC unit tests (L5, Tier 2: real group over real MemSafe over the fake).

The group packs its parameter items (here the product's five uint8 items with factory
defaults 0, 0, 50, 100, 70) into one record and moves it through AlxMemSafe. Contract:

  Init
  - a valid record in flash loads the items (values taken as stored)
  - a damaged copy is tolerated (the other one is used, the damaged one repaired)
  - nothing valid in flash -> the items keep their factory defaults and the defaults are
    written to flash, so the next boot finds a valid record
  - a raw layer that keeps failing -> Init returns an error (initNumOfTries exhausted)
  After Init the group's working buffer must equal what it just stored: otherwise the store
  state machine believes a change is pending and writes a stale buffer to flash (finding of
  03.09, fixed the same day: Init refreshes valBuff from the final stored record).

Test group P10 = ALX-1513 param-group proofs.
"""

import zlib

import pytest

pytestmark = pytest.mark.unit

A, B = 0x000, 0x100
REC = 9
DEFAULTS = [0, 0, 50, 100, 70]
def blob(vals) -> bytes:
    data = bytes(vals)
    return data + (zlib.crc32(data) & 0xFFFFFFFF).to_bytes(4, "little")


# =====================================================================
# P10 - Init on a blank store
# =====================================================================

def test_ALX1513_P10_blank_flash_init_keeps_factory_defaults_and_writes_them(flash, make_store):
    ctx = make_store()
    assert flash.defaults(ctx) == DEFAULTS
    assert flash.group_init(ctx) == flash.OK
    assert flash.items(ctx) == DEFAULTS
    assert flash.peek(A, REC) == blob(DEFAULTS) and flash.peek(B, REC) == blob(DEFAULTS)


def test_ALX1513_P10_second_boot_after_blank_init_finds_a_valid_record(flash, make_store):
    assert flash.group_init(make_store()) == flash.OK
    writes = flash.count(flash.WRITE)
    boot2 = make_store()
    assert flash.group_init(boot2) == flash.OK
    assert flash.items(boot2) == DEFAULTS
    assert flash.count(flash.WRITE) == writes, "a clean second boot writes nothing"


def test_ALX1513_P10_after_blank_init_no_change_is_pending(flash, make_store):
    """After Init the working buffer equals the stored buffer: the store must not see a diff."""
    ctx = make_store()
    assert flash.group_init(ctx) == flash.OK
    assert flash.group_diff(ctx) is False


# =====================================================================
# P10 - Init with stored values
# =====================================================================

def test_ALX1513_P10_stored_record_loads_into_the_items(flash, make_store):
    flash.poke(A, blob([1, 2, 3, 4, 5]))
    flash.poke(B, blob([1, 2, 3, 4, 5]))
    ctx = make_store()
    assert flash.group_init(ctx) == flash.OK
    assert flash.items(ctx) == [1, 2, 3, 4, 5]
    assert flash.count(flash.WRITE) == 0


def test_ALX1513_P10_stored_record_with_damaged_copy_a_loads_from_b_and_repairs(flash, make_store):
    flash.poke(A, blob([1, 2, 3, 4, 5]))
    flash.poke(B, blob([1, 2, 3, 4, 5]))
    damaged = bytearray(flash.peek(A, REC))
    damaged[3] ^= 0x80
    flash.poke(A, bytes(damaged))
    ctx = make_store()
    assert flash.group_init(ctx) == flash.OK
    assert flash.items(ctx) == [1, 2, 3, 4, 5]
    assert flash.peek(A, REC) == blob([1, 2, 3, 4, 5])


def test_ALX1513_P10_characterization_stored_out_of_range_value_is_loaded_unchecked(flash, make_store):
    """CHARACTERIZATION: the group copies stored bytes straight into the items, bypassing the
    item's min/max. A CRC-valid record with 200 loads as 200 (uint8 0..100 item). Only a range
    change between firmware versions can produce this; the app may clamp after Init (D6)."""
    flash.poke(A, blob([200, 0, 50, 100, 70]))
    flash.poke(B, blob([200, 0, 50, 100, 70]))
    ctx = make_store()
    assert flash.group_init(ctx) == flash.OK
    assert flash.items(ctx)[0] == 200


def test_ALX1513_P10_init_with_raw_layer_failing_always_returns_error(flash, make_store):
    flash.fail_at(flash.INIT, flash.ALWAYS)
    ctx = make_store()
    assert flash.group_init(ctx) == flash.ERR
    assert flash.items(ctx) == DEFAULTS, "items untouched"


def test_ALX1513_P10_init_survives_one_failing_raw_call(flash, make_store):
    flash.poke(A, blob([9, 8, 7, 6, 5]))
    flash.poke(B, blob([9, 8, 7, 6, 5]))
    flash.fail_at(flash.READ, 1)
    ctx = make_store()
    assert flash.group_init(ctx) == flash.OK
    assert flash.items(ctx) == [9, 8, 7, 6, 5]
