"""ALX-1553 - an AlxParamItem that declares the LIST of values it will accept.

An item can be built with an enum list, and a set is then checked against that list instead of
against the minimum and the maximum. It is a second and separate gate, and it is where both of this
module's static functions live:

    AlxParamItem_AreEnumArrValFromLowToHigh   the constructor asks whether the list is ordered
    AlxParamItem_IsEnumOnList                 every set asks whether the value is a member

Which of the two a given build actually runs is worth knowing, because they are reached very
differently. The ordering check exists ONLY inside ALX_PARAM_ITEM_ASSERT (alxParamItem.c:2378), so a
module compiled with no assert form discards the call along with the expression and an out-of-order
list is accepted in silence. The membership check is not - it sits in the set path itself
(alxParamItem.c:2715) and runs in every build, asserts or none. This group compiles asserts ON, so
both are live here, and P557 says the first one out loud rather than leaving a reader to assume a
list is validated everywhere.

The refusal is also worth stating precisely: a value off the list is refused with
AlxParamItem_ErrEnum and the item keeps what it had - the Bound handle does NOT pull the value to
the nearest list member. That is a real distinction for a caller, and the status is the only way it
can be seen, which is why the shim now returns it.

Test group P555-P559 = ALX-1553 param-item enum proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# the types an enum list can be declared for - Bool, Arr and Str take no list
INT_TYPES = ("D_UINT8", "D_UINT16", "D_UINT32", "D_UINT64",
             "D_INT8", "D_INT16", "D_INT32", "D_INT64")
FLOAT_TYPES = ("D_FLOAT", "D_DOUBLE")

# ordered, with gaps, so "on the list" is narrower than "inside the range"
VALUES = (2, 5, 9)
FLOAT_VALUES = (1.5, 2.5, 4.0)


def _t(lib, name: str) -> int:
    return getattr(lib, name)


# =====================================================================
# P555 - the list comes back as it was declared
# =====================================================================


@pytest.mark.parametrize("type_name", INT_TYPES)
def test_ALX1553_P555_an_integer_enum_list_reads_back_in_order(
        memsafe_lib, make_enum_item, type_name):
    """Length and every element, through the getter for that type."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, VALUES, val_def=VALUES[0],
                         val_min=VALUES[0], val_max=VALUES[-1])

    assert m.enum_is_enum(ctx) is True
    assert m.enum_len(ctx) == len(VALUES)
    assert [m.enum_at(ctx, data_type, i) for i in range(len(VALUES))] == list(VALUES)


@pytest.mark.parametrize("type_name", FLOAT_TYPES)
def test_ALX1553_P555_a_float_enum_list_reads_back_in_order(
        memsafe_lib, make_enum_item, type_name):
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, FLOAT_VALUES, val_def=FLOAT_VALUES[0],
                         val_min=FLOAT_VALUES[0], val_max=FLOAT_VALUES[-1])

    assert m.enum_is_enum(ctx) is True
    assert m.enum_len(ctx) == len(FLOAT_VALUES)
    assert [m.enum_at(ctx, data_type, i) for i in range(len(FLOAT_VALUES))] == list(FLOAT_VALUES)


# =====================================================================
# P556 - a value on the list is accepted
# =====================================================================


@pytest.mark.parametrize("type_name", INT_TYPES)
def test_ALX1553_P556_every_value_on_the_list_is_accepted(
        memsafe_lib, make_enum_item, type_name):
    """Each member in turn, and the status says so."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, VALUES, val_def=VALUES[0],
                         val_min=VALUES[0], val_max=VALUES[-1])

    for value in VALUES:
        assert m.enum_set_val(ctx, data_type, value) == m.OK, f"{type_name}: set {value}"
        assert m.enum_get_val(ctx, data_type) == value


@pytest.mark.parametrize("type_name", FLOAT_TYPES)
def test_ALX1553_P556_every_float_value_on_the_list_is_accepted(
        memsafe_lib, make_enum_item, type_name):
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, FLOAT_VALUES, val_def=FLOAT_VALUES[0],
                         val_min=FLOAT_VALUES[0], val_max=FLOAT_VALUES[-1])

    for value in FLOAT_VALUES:
        assert m.enum_set_val(ctx, data_type, value) == m.OK, f"{type_name}: set {value}"
        assert m.enum_get_val(ctx, data_type) == value


# =====================================================================
# P557 - a value off the list is refused, and the old one stands
# =====================================================================


@pytest.mark.parametrize("type_name", INT_TYPES)
def test_ALX1553_P557_a_value_between_two_list_entries_is_refused(
        memsafe_lib, make_enum_item, type_name):
    """6 is inside the range 2..9 and is not on the list - membership is the narrower test."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, VALUES, val_def=VALUES[0],
                         val_min=VALUES[0], val_max=VALUES[-1])

    assert m.enum_set_val(ctx, data_type, 5) == m.OK
    assert m.enum_set_val(ctx, data_type, 6) == m.ERR_ENUM, "a value off the list was accepted"
    assert m.enum_get_val(ctx, data_type) == 5, "the refused set changed the value anyway"


@pytest.mark.parametrize("type_name", INT_TYPES)
def test_ALX1553_P557_bound_does_not_pull_a_value_onto_the_list(
        memsafe_lib, make_enum_item, type_name):
    """The Bound handle clamps to min/max - it does NOT snap to the nearest list member.

    Worth an assertion of its own: a reader who knows Bound clamps out-of-range values could
    reasonably expect it to do something similar here, and it does not. The set is refused.
    """
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, VALUES, val_def=VALUES[0], val_min=VALUES[0],
                         val_max=VALUES[-1], out_of_range=m.BOUND)

    assert m.enum_set_val(ctx, data_type, 6) == m.ERR_ENUM
    assert m.enum_get_val(ctx, data_type) == VALUES[0], "the value moved despite the refusal"


@pytest.mark.parametrize("type_name", FLOAT_TYPES)
def test_ALX1553_P557_a_float_value_off_the_list_is_refused(
        memsafe_lib, make_enum_item, type_name):
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_enum_item(data_type, FLOAT_VALUES, val_def=FLOAT_VALUES[0],
                         val_min=FLOAT_VALUES[0], val_max=FLOAT_VALUES[-1])

    assert m.enum_set_val(ctx, data_type, 2.5) == m.OK
    assert m.enum_set_val(ctx, data_type, 3.0) == m.ERR_ENUM
    assert m.enum_get_val(ctx, data_type) == 2.5


@pytest.mark.expect_assert("an out-of-order enum list is what the constructor's assertion is for")
def test_ALX1553_P557_an_out_of_order_list_is_caught_only_by_an_assertion(
        memsafe_lib, make_enum_item):
    """CHARACTERIZATION of where the ordering check lives, not a defect.

    AlxParamItem_AreEnumArrValFromLowToHigh is called from inside ALX_PARAM_ITEM_ASSERT and nowhere
    else. With asserts compiled - as this group compiles them - a descending list trips it. With a
    module built carrying no ALX_PARAM_ITEM_ASSERT_*_ENABLE the whole expression is discarded, the
    call never happens, and the same list is accepted in silence.

    The membership check is not like this: it runs in the set path in every build, which is why
    P556 and P557 hold whatever the assert form. This test marks the difference so nobody reads
    the enum list as validated everywhere.
    """
    m = memsafe_lib
    ctx = make_enum_item(m.D_UINT8, (9, 5, 2), val_def=9, val_min=2, val_max=9)

    assert ctx, "the item is still constructed - the assertion reports, it does not refuse"


# =====================================================================
# P558 - the status a set returns on an item with no list
# =====================================================================


@pytest.mark.parametrize("type_name", INT_TYPES)
def test_ALX1553_P558_a_set_inside_the_range_reports_ok(
        memsafe_lib, make_meta_item, type_name):
    """The plain path, for contrast with the two below - and the status was being thrown away."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_meta_item(data_type, val_def=10, val_min=10, val_max=100)

    assert m.meta_set_val(ctx, data_type, 50) == m.OK


@pytest.mark.parametrize("type_name", INT_TYPES)
def test_ALX1553_P558_ignore_and_bound_both_refuse_an_out_of_range_set_in_the_status(
        memsafe_lib, make_meta_item, type_name):
    """Both handles report failure; they differ in what they leave behind, which is P552.

    Ignore keeps the old value and Bound clamps to the edge, but neither answers Ok - so a caller
    that only reads the status cannot tell a clamped set from a discarded one, and a caller that
    only reads the value cannot tell a clamp from a success. Both have to be read.
    """
    m = memsafe_lib
    data_type = _t(m, type_name)

    ignoring = make_meta_item(data_type, val_def=10, val_min=10, val_max=100,
                              out_of_range=m.IGNORE)
    assert m.meta_set_val(ignoring, data_type, 101) != m.OK
    assert m.meta_get_val(ignoring, data_type) == 10, "Ignore kept the out-of-range value"

    bounding = make_meta_item(data_type, val_def=10, val_min=10, val_max=100,
                              out_of_range=m.BOUND)
    assert m.meta_set_val(bounding, data_type, 101) != m.OK
    assert m.meta_get_val(bounding, data_type) == 100, "Bound did not clamp to the maximum"


# =====================================================================
# P559 - the bool setter, which the integer table does not reach
# =====================================================================


def test_ALX1553_P559_a_bool_value_round_trips(memsafe_lib, make_meta_item):
    """AlxParamItem_SetValBool has no min and no max to check, so it is the simplest set there is
    - and it was the one function of the scalar setters that no test called."""
    m = memsafe_lib
    ctx = make_meta_item(m.D_BOOL, val_def=0)

    assert m.meta_get_val(ctx, m.D_BOOL) == 0
    assert m.meta_set_val(ctx, m.D_BOOL, 1) == m.OK
    assert m.meta_get_val(ctx, m.D_BOOL) == 1
    assert m.meta_set_val(ctx, m.D_BOOL, 0) == m.OK
    assert m.meta_get_val(ctx, m.D_BOOL) == 0


def test_ALX1553_P559_setting_a_bool_to_default_undoes_a_set(memsafe_lib, make_meta_item):
    m = memsafe_lib
    ctx = make_meta_item(m.D_BOOL, val_def=1)

    assert m.meta_set_val(ctx, m.D_BOOL, 0) == m.OK
    assert m.meta_get_val(ctx, m.D_BOOL) == 0

    m.meta_set_val_to_def(ctx)
    assert m.meta_get_val(ctx, m.D_BOOL) == 1
