"""ALX-1553 - a parameter store with more than one group.

AlxParamStore_Handle dispatches on the number of groups and has three arms - 1, 2 and 4 - with an
asserting `default:` for anything else. Only the first arm had ever run: the existing shim builds a
store with `groupArr[1]` and there was no way to ask for another, so _Handle_2xGroup and
_Handle_4xGroup were the last two functions in alxParamStore.c that no test entered.

Measured, on a store whose groups each hold two uint8 items in their own window of the fake flash:

    groups   Init   dispatches to       a changed item reaches flash   assertion
    1        Alx_Ok  _Handle_1xGroup     YES                            none
    2        Alx_Ok  _Handle_2xGroup     YES                            none
    3        Alx_Ok  default:            no                             every Handle call
    4        Alx_Ok  _Handle_4xGroup     no                             every Handle call

The two that fail are different faults with the same consequence.

FOUR is the worse one, because four is a count the dispatch explicitly accepts. The arm it leads to
is not a state machine at all:

    static void AlxParamStore_Handle_4xGroup(AlxParamStore* me)
    {
        // TV: TODO
        (void)me;
        ALX_PARAM_STORE_ASSERT(false);
    }

THREE is a gap of a different kind: AlxParamStore_Ctor does not validate numOfParamGroups at all,
and AlxParamStore_Init only asserts that the count MATCHES what the constructor was given - not that
it is one the store can handle. So the count is accepted twice and rejected only on the first
Handle, by an assertion.

In both cases Init answers Alx_Ok and every parameter the product changes stays in RAM for ever.
With this module's assertions compiled out - and 6u/13 is a list of 49 places where that switch
removes behaviour rather than checks - it happens in complete silence: no trace, no status, nothing
to see but parameters that do not survive a reboot.

This product uses one group (numOfParamGroups = 1), so nothing here is exposed today. That is why
it was never noticed, and it is the reason to write it down rather than to treat it as urgent.

Proofs (ALX-1553): P568-P570.
"""

import pytest

pytestmark = pytest.mark.unit

CHANGED = 99


def _change_and_run(m, ctx, group: int, handles: int = 20) -> bytes:
    """Set the group's first item, let the store run, and answer with what reached flash."""
    assert m.store_item_set(ctx, group, 0, CHANGED) == m.OK
    m.store_group_handle(ctx, handles)
    return m.peek(m.store_group_addr(group), m.store_items_per_group())


# =====================================================================
# P568 - the two arms that are implemented
# =====================================================================


@pytest.mark.parametrize("groups", [1, 2])
def test_ALX1553_P568_a_store_of_one_or_two_groups_initialises(
        memsafe_lib, make_multi_store, groups):
    m = memsafe_lib
    ctx = make_multi_store(groups)

    assert m.store_group_init(ctx) == m.OK
    assert m.store_group_err(ctx) is False


@pytest.mark.parametrize("groups", [1, 2])
def test_ALX1553_P568_every_group_persists_a_change(memsafe_lib, make_multi_store, groups):
    """Each group in turn, because the 2x machine walks them one at a time.

    A change to group 1 that only ever reached group 0's window would pass a test that looked at
    one group, so both are set and both are read back from their own addresses.
    """
    m = memsafe_lib
    ctx = make_multi_store(groups)
    assert m.store_group_init(ctx) == m.OK

    for group in range(groups):
        landed = _change_and_run(m, ctx, group)
        assert landed[0] == CHANGED, (
            f"{groups}-group store: the change to group {group} never reached flash"
        )


def test_ALX1553_P568_a_change_to_one_group_leaves_the_other_alone(
        memsafe_lib, make_multi_store):
    """Two groups, two windows: the store must not write a record into the wrong one."""
    m = memsafe_lib
    ctx = make_multi_store(2)
    assert m.store_group_init(ctx) == m.OK
    n = m.store_items_per_group()

    untouched_before = m.peek(m.store_group_addr(1), n)
    landed = _change_and_run(m, ctx, 0)

    assert landed[0] == CHANGED
    assert m.peek(m.store_group_addr(1), n) == untouched_before, "the other group's record moved"


# =====================================================================
# P569 - four groups, where the handler is a stub
# =====================================================================


@pytest.mark.expect_assert(
    "AlxParamStore_Handle_4xGroup asserts false on every call - that IS the defect")
@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: AlxParamStore_Handle_4xGroup is an unimplemented stub - `// TV: TODO`, (void)me, "
    "ALX_PARAM_STORE_ASSERT(false) and nothing else (alxParamStore.c:439-444). Four is one of the "
    "three counts AlxParamStore_Handle's dispatch explicitly accepts, so a store built with four "
    "groups constructs, returns Alx_Ok from Init, and then stores NOTHING for ever: measured over "
    "40 Handle calls, a changed item stays at its default in flash while reading back correctly "
    "from RAM. With this module's assertions compiled out there is no trace, no status and no "
    "assertion either - just parameters that do not survive a reboot. The product uses one group, "
    "so nothing is exposed today. Either implement the arm or take 4 out of the dispatch so the "
    "count is refused where it is given"))
def test_ALX1553_P569_a_store_of_four_groups_persists_a_change(memsafe_lib, make_multi_store):
    m = memsafe_lib
    ctx = make_multi_store(4)
    assert m.store_group_init(ctx) == m.OK, "Init reports success whatever Handle will do"

    landed = _change_and_run(m, ctx, 0, handles=40)

    assert landed[0] == CHANGED


# =====================================================================
# P570 - three groups, where only the dispatch says no
# =====================================================================


@pytest.mark.expect_assert(
    "the dispatch's default: arm asserts on every Handle - that IS the defect")
@pytest.mark.xfail(strict=True, reason=(
    "DEFECT: nothing refuses an unsupported group count where it is given. AlxParamStore_Ctor does "
    "not validate numOfParamGroups at all, and AlxParamStore_Init only asserts that the count "
    "MATCHES what the constructor was given - not that the store can handle it. A three-group "
    "store therefore constructs, returns Alx_Ok from Init, and is rejected only on the first "
    "Handle, by the dispatch's asserting default. Measured: 20 Handle calls, 20 assertions, and a "
    "changed item still at its default in flash. With assertions compiled out the store is simply "
    "inert and silent. The fix belongs in the constructor, which is the one place a caller can act "
    "on it"))
def test_ALX1553_P570_a_store_of_three_groups_persists_a_change(memsafe_lib, make_multi_store):
    m = memsafe_lib
    ctx = make_multi_store(3)
    assert m.store_group_init(ctx) == m.OK, "Init reports success on a count Handle cannot dispatch"

    landed = _change_and_run(m, ctx, 0)

    assert landed[0] == CHANGED
