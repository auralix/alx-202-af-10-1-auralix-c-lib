"""ALX-1553 - AlxParamItem_LoadVal and AlxParamItem_StoreVal, the two that need a KV store.

They were the last two of this module's 105 functions no test had called. Both open with
ALX_PARAM_ITEM_ASSERT(me->paramKvStore != NULL), and every item the harness built passed NULL - the
same as the product's CLI items - so there was no way in. The store pointer is never dereferenced by
the library; it is handed straight back to AlxParamKvStore_Get/_Set, which the link-time fake
supplies, so a stand-in pointer plus an armed fake is enough.

The fake stays DISARMED unless a test arms it. That matters: the CLI and ParamMgmt groups build
their items without a store, and a call arriving at the fake means a test wandered into persistence.
Disarmed it still returns Alx_Err loudly, exactly as it always did.

What the four proofs cover, all measured before they were written:

  P571  first boot - no key in the store - keeps the default AND seeds the store with it
  P572  a stored value is loaded over the default, and loading does not mark the item pending
  P573  StoreVal is gated on pendingStore: no change, no write. A failed write KEEPS pending, so
        the next call retries and succeeds - which is the behaviour a flash that is briefly busy
        needs
  P574  SEALED - a record SHORTER than the item's type is accepted and zero-extended

P574 is the one to read. AlxParamItem_LoadVal asks the store for the value and for its length:

    status = AlxParamKvStore_Get(me->paramKvStore, me->key, buff, buffLen, &actualValLen);

`actualValLen` is declared at alxParamItem.c:2178, filled here at 2211, and NEVER READ AGAIN -
grep the module and those are its only two mentions. AlxParamItem_SetVal then takes the whole
valLen from the buffer whatever the store returned. The buffer is calloc'd, so the missing bytes
read as zero rather than as rubbish, and the result is a value that is silently WRONG instead of a
read that is refused.

Measured: a uint32 item whose stored record is the single byte 0xAB loads as 0x000000AB, and
LoadVal answers Alx_Ok.

The case that makes it matter is a firmware upgrade that widens a parameter - a uint8 current limit
that becomes a uint16. Every record in flash is then one byte short, and every one of them loads as
a small wrong number instead of being detected as stale and reset to its default. The check costs
one comparison against the length the store already returns.

Proofs (ALX-1553): P571-P574.
"""

import pytest

pytestmark = pytest.mark.unit

DEFAULT = 42
CHANGED = 99


# =====================================================================
# P571 - first boot: the key is not there yet
# =====================================================================


def test_ALX1553_P571_a_missing_key_keeps_the_default(memsafe_lib, make_kv_item):
    m = memsafe_lib
    ctx = make_kv_item(m.D_UINT8, "P571_A", DEFAULT)

    assert m.kv_peek("P571_A") is None, "the store starts empty"
    assert m.kv_load(ctx) == m.OK
    assert m.kv_get(ctx) == DEFAULT


def test_ALX1553_P571_a_missing_key_is_created_with_the_default(memsafe_lib, make_kv_item):
    """The first boot writes the default back, so the key exists from then on.

    That is what makes a factory-fresh device's store self-populating, and it is one Set - the
    count matters because a loop over a whole parameter table would otherwise write every key on
    every boot.
    """
    m = memsafe_lib
    ctx = make_kv_item(m.D_UINT8, "P571_B", DEFAULT)

    before = m.kv_set_count()
    assert m.kv_load(ctx) == m.OK

    assert m.kv_peek("P571_B") == bytes([DEFAULT]), "the default was not written to the store"
    assert m.kv_set_count() == before + 1, "the seed must be exactly one write"


def test_ALX1553_P571_a_store_that_cannot_be_read_or_written_reports_failure(
        memsafe_lib, make_kv_item):
    """Get fails, so the seed path runs; the Set fails too, so there is nothing left to try."""
    m = memsafe_lib
    ctx = make_kv_item(m.D_UINT8, "P571_C", DEFAULT)
    m.kv_fail_get(True)
    m.kv_fail_set(True)

    assert m.kv_load(ctx) != m.OK
    assert m.kv_get(ctx) == DEFAULT, "a failed load must leave the item at its default"


# =====================================================================
# P572 - a stored value wins over the default
# =====================================================================


def test_ALX1553_P572_a_stored_value_is_loaded(memsafe_lib, make_kv_item):
    m = memsafe_lib
    m.kv_poke("P572_A", bytes([77]))
    ctx = make_kv_item(m.D_UINT8, "P572_A", DEFAULT)

    assert m.kv_load(ctx) == m.OK
    assert m.kv_get(ctx) == 77


def test_ALX1553_P572_loading_does_not_leave_the_item_pending(memsafe_lib, make_kv_item):
    """A loaded value differs from the default, and that must NOT count as a change to store.

    The module says so in a comment at alxParamItem.c:2243 - "if loaded val diff than default,
    pendingStore was set.. we don't want that" - and this is the assertion behind it: a boot that
    loads a stored value must not write it straight back. StoreVal returning without a write is
    what proves pendingStore is clear.
    """
    m = memsafe_lib
    m.kv_poke("P572_B", bytes([77]))
    ctx = make_kv_item(m.D_UINT8, "P572_B", DEFAULT)
    assert m.kv_load(ctx) == m.OK

    before = m.kv_set_count()
    assert m.kv_store(ctx) == m.OK
    assert m.kv_set_count() == before, "the load marked the item pending and it wrote on boot"


# =====================================================================
# P573 - StoreVal writes only what changed
# =====================================================================


def test_ALX1553_P573_no_change_means_no_write(memsafe_lib, make_kv_item):
    """The endurance rule, the same one the param store holds to (P11)."""
    m = memsafe_lib
    ctx = make_kv_item(m.D_UINT8, "P573_A", DEFAULT)
    assert m.kv_load(ctx) == m.OK

    before = m.kv_set_count()
    assert m.kv_store(ctx) == m.OK
    assert m.kv_store(ctx) == m.OK
    assert m.kv_set_count() == before, "nothing changed and the store was written anyway"


def test_ALX1553_P573_a_changed_value_is_written_once(memsafe_lib, make_kv_item):
    m = memsafe_lib
    ctx = make_kv_item(m.D_UINT8, "P573_B", DEFAULT)
    assert m.kv_load(ctx) == m.OK
    assert m.kv_set(ctx, CHANGED) == m.OK

    before = m.kv_set_count()
    assert m.kv_store(ctx) == m.OK
    assert m.kv_peek("P573_B") == bytes([CHANGED])
    assert m.kv_set_count() == before + 1

    assert m.kv_store(ctx) == m.OK
    assert m.kv_set_count() == before + 1, "the second call wrote again - pending was not cleared"


def test_ALX1553_P573_a_failed_write_stays_pending_and_the_retry_succeeds(
        memsafe_lib, make_kv_item):
    """A flash that is briefly busy must not lose the change.

    If a failed StoreVal cleared pendingStore, the value would live in RAM and never reach the
    store again - it would be lost at the next reset with nothing to say so.
    """
    m = memsafe_lib
    ctx = make_kv_item(m.D_UINT8, "P573_C", DEFAULT)
    assert m.kv_load(ctx) == m.OK
    assert m.kv_set(ctx, CHANGED) == m.OK

    m.kv_fail_set(True)
    assert m.kv_store(ctx) != m.OK, "the write failed and StoreVal must say so"
    m.kv_fail_set(False)

    assert m.kv_store(ctx) == m.OK, "the retry did not happen - pending was cleared on failure"
    assert m.kv_peek("P573_C") == bytes([CHANGED])


# =====================================================================
# P574 - the sealed one: a record shorter than the type
# =====================================================================


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxParamItem_LoadVal never checks how many bytes the store actually returned. It asks "
    "for the value and its length - AlxParamKvStore_Get(..., buff, buffLen, &actualValLen) at "
    "alxParamItem.c:2211 - and `actualValLen` is declared at 2178, filled there, and never read "
    "again; those two lines are its only mentions in the module. AlxParamItem_SetVal then takes the "
    "full valLen out of the buffer whatever the store returned. The buffer is calloc'd, so the "
    "missing bytes read as zero and the result is a value that is silently WRONG rather than a read "
    "that is refused. Measured: a uint32 item whose stored record is the single byte 0xAB loads as "
    "0x000000AB and LoadVal answers Alx_Ok. The case that makes it matter is a firmware upgrade "
    "that widens a parameter - a uint8 that becomes a uint16 - where every record in flash is then "
    "short and every one loads as a small wrong number instead of being detected as stale and reset "
    "to its default. The fix is one comparison against the length the store already hands back"))
def test_ALX1553_P574_a_record_shorter_than_the_type_is_refused(memsafe_lib, make_kv_item):
    m = memsafe_lib
    m.kv_poke("P574_A", bytes([0xAB]))                 # one byte for a four-byte parameter
    ctx = make_kv_item(m.D_UINT32, "P574_A", 0x11223344)

    m.kv_load(ctx)

    assert m.kv_get(ctx) == 0x11223344, (
        f"a one-byte record was accepted for a uint32 and read back as "
        f"0x{m.kv_get(ctx):08X} - the item should have kept its default"
    )
