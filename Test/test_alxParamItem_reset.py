"""ALX-1553 - AlxParamItem_SetValToDef, and the assertion that does the work.

The last line of AlxParamItem_SetValToDef (alxParamItem.c:1372) is:

    ALX_PARAM_ITEM_ASSERT(AlxParamItem_SetVal(me, valDefPtr) == Alx_Ok);

That call is the ONLY place the function writes anything, and it sits inside the assert macro. A
module compiled with no ALX_PARAM_ITEM_ASSERT_*_ENABLE expands that macro to `do {} while(false)`
taking no parameters, so the preprocessor deletes the argument - the call included. The function
still links, still returns, and does nothing at all. For every data type, not just one.

Measured across the four build variants, on a uint8 item whose default is 42, set to 7 first:

    default        ASSERT_RST      -> 42    restored
    debug          ASSERT_BKPT     -> 42    restored
    assert_trace   ASSERT_TRACE    -> 42    restored
    off            (no form)       ->  7    NO-OP

This is the same shape as the defect behind NotesClaude 3g, where 257 of the product's assertions
were compiled out of a host build and a bench reset defect came back. It matters more than most
because of what the function is for: a reset to defaults is the operation a factory reset and a
CLI `set-param-to-def` are built on, and D1/A1 is an open plan to add exactly that. In a release
build with this module's assertions off, it would report success and change nothing.

The fix is to take the call out of the assertion and check its status separately, which is a
library source change and so not made here.

A SECOND defect lives in the same function and is NOT executed by these tests, because it cannot be:

    else if (me->dataType == AlxParamItem_Arr)
    {
        ALX_PARAM_ITEM_ASSERT(false);   // We should never get here
    }

valDefPtr is left NULL and the line above then passes NULL to AlxParamItem_SetVal, which memcpys
from it. With assertions ON that is a null dereference - it took the test process down when I first
reached it, and no xfail can catch a process that dies. With assertions OFF the whole line is gone,
so it is merely the no-op this file seals. Either way calling SetValToDef on an Arr item is never
correct, so it is written down rather than run. test_alxParamItem_buff.py covers everything else
those two types can do.

Proofs (ALX-1553): P560-P561.
"""

import pytest

from conftest import VARIANTS

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


@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxParamItem_SetValToDef does its only write inside an assertion. "
    "alxParamItem.c:1372 reads ALX_PARAM_ITEM_ASSERT(AlxParamItem_SetVal(me, valDefPtr) == Alx_Ok), "
    "and a module compiled with no ALX_PARAM_ITEM_ASSERT_*_ENABLE expands that macro to "
    "do {} while(false) taking no parameters - so the preprocessor deletes the argument and the "
    "call with it. The function links, returns, and changes nothing, for every data type. "
    "Measured: default/debug/assert_trace restore a uint8 default of 42; `off` leaves the 7 that "
    "was set. This is the operation a factory reset and a CLI reset-to-default are built on, and "
    "D1/A1 is an open plan to add one - it would report success and do nothing in a release build "
    "with this module's assertions off. The fix is to call AlxParamItem_SetVal on its own line and "
    "assert on the status it returns, which is a library source change"))
def test_ALX1553_P561_a_build_without_assertions_also_resets_a_value_to_its_default(
        memsafe_variant_lib):
    """The `off` variant, where the write has been deleted along with the assertion.

    Only the configuration with no assert form is asked; the other three are P560 and skip here, so
    a fix makes this XPASS and fails the suite exactly once, in the one place that measures it.
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
