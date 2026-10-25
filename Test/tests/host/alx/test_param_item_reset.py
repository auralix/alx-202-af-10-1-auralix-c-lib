"""ALX-1553 - AlxParamItem_SetValToDef, and the assertion that did the work.

Until 91d88c4 (ALX-1564, 2026-10-01) the last line of AlxParamItem_SetValToDef was:

    ALX_PARAM_ITEM_ASSERT(AlxParamItem_SetVal(me, valDefPtr) == Alx_Ok);

That call was the ONLY place the function wrote anything, and it sat inside the assert macro. A
module compiled with no ALX_PARAM_ITEM_ASSERT_*_ENABLE expands that macro to `do {} while(false)`
taking no parameters, so the preprocessor deleted the argument - the call included. The function
still linked, still returned, and did nothing at all. For every data type, not just one.

Measured across the four build variants before the fix, on a uint8 item whose default is 42, set
to 7 first:

    default        ASSERT_RST      -> 42    restored
    debug          ASSERT_BKPT     -> 42    restored
    assert_trace   ASSERT_TRACE    -> 42    restored
    off            (no form)       ->  7    NO-OP

This is the same shape as the defect behind NotesClaude 3g, where 257 of the product's assertions
were compiled out of a host build and a bench reset defect came back. It matters more than most
because of what the function is for: a reset to defaults is the operation a factory reset and a
CLI `set-param-to-def` are built on, and D1/A1 is an open plan to add exactly that. In a release
build with this module's assertions off, it would report success and change nothing.

91d88c4 calls AlxParamItem_SetVal on its own line and asserts on the status it returns: all four
variants restore the default now, and P561's strict xfail came off with the fix.

A SECOND defect lived in the same function, and these tests did not execute it, because they could
not:

    else if (me->dataType == AlxParamItem_Arr)
    {
        ALX_PARAM_ITEM_ASSERT(false);   // We should never get here
    }

valDefPtr was left NULL and the set then passed NULL to AlxParamItem_SetVal, which memcpys from
it. With assertions ON that is a null dereference - it took the test process down when I first
reached it, and no xfail can catch a process that dies. With assertions OFF the whole line was
gone, so it was merely the no-op this file sealed, until 91d88c4 moved the call out of the
assertion. Since then the Arr branch, like the branch for an unknown type, returns after its
assertion, so SetVal never receives NULL. Calling SetValToDef on an Arr item is still never
correct. test_param_item_buff.py covers everything else those two types can do.

Proofs (ALX-1553): P560-P561.
"""

import pytest

from harness.host.build import (
    VARIANTS,
)

pytestmark = pytest.mark.variants

DEFAULT, CHANGED = 42, 7


def test_ALX1553_P560_a_build_with_assertions_resets_a_value_to_its_default(memsafe_variant_lib):
    """The three configurations that compile an assert form all do the work."""
    name, m = memsafe_variant_lib
    form, _trace, _level = VARIANTS[name]
    if form is None:
        pytest.skip("the variant with no assert form is P561, the sealed half")

    ctx = m.meta_new(m.D_UINT8, val_def=DEFAULT, val_min=0, val_max=255)
    try:
        assert m.meta_set_val(ctx, m.D_UINT8, CHANGED) == m.OK
        assert m.meta_get_val(ctx, m.D_UINT8) == CHANGED, "the set did not take"

        m.meta_set_val_to_def(ctx)
        assert m.meta_get_val(ctx, m.D_UINT8) == DEFAULT
    finally:
        m.meta_delete(ctx)


def test_ALX1553_P561_a_build_without_assertions_also_resets_a_value_to_its_default(
        memsafe_variant_lib):
    """The `off` variant, where the write was deleted along with the assertion until 91d88c4.

    Only the configuration with no assert form is asked; the other three are P560 and skip here. It
    was a strict xfail on the defect, so the fix made it XPASS once, here, and the marker came off.
    """
    name, m = memsafe_variant_lib
    form, _trace, _level = VARIANTS[name]
    if form is not None:
        pytest.skip("a variant that compiles assertions is P560")

    ctx = m.meta_new(m.D_UINT8, val_def=DEFAULT, val_min=0, val_max=255)
    try:
        m.meta_set_val(ctx, m.D_UINT8, CHANGED)
        assert m.meta_get_val(ctx, m.D_UINT8) == CHANGED, "the set did not take"

        m.meta_set_val_to_def(ctx)
        assert m.meta_get_val(ctx, m.D_UINT8) == DEFAULT
    finally:
        m.meta_delete(ctx)
