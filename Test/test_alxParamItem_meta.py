"""ALX-1553 - what an AlxParamItem's CONSTRUCTOR recorded, read back through its getters.

test_alxParamItem.py drives one corner of this module: the string conversion a CLI set-param
reaches. That corner is 25 of the module's 105 functions, and the coverage lane had been reporting
even those as never executed because the MemSafe group's test list did not name the file. With the
list corrected the module measures 25 of 105, and the eighty that remain are this file's subject.

They are not obscure. An item is built from seventeen arguments - key, id, group, default, minimum,
maximum, unit, the out-of-range handle, the after-reset flag - and every one of them has a getter
that a CLI, a CAN parameter service and a JSON dump all read. Nothing asserted that what goes into
the constructor comes back out of it.

That is worth saying plainly because of the shape of the defect it admits. A wrong default or a
wrong unit does not crash and does not fail a build; it is a value that ships and is read by
somebody downstream as though it were meant. A product's mutation run makes the same point from the
other end - changing a constructor's valDef from 0 to 1, or moving its unit string to a different
argument, leaves every test passing.

Two notes on how values cross the boundary. They travel as int64_t or double, so each type's own
edges can be named exactly: int64_t is exact for every integer type here and double is exact for
float. The one value the channel cannot carry is a uint64 above INT64_MAX, so this file does not
ask for one. And the items stand alone - no kv store, no group object - because what is under test
is what the constructor recorded, not what a store would later do with it.

Test group P548-P554 = ALX-1553 param-item metadata proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# data type, the C name in the getters, the type's own minimum and maximum
INT_TYPES = (
    ("D_UINT8", "Uint8", 0, 255, 1),
    ("D_UINT16", "Uint16", 0, 65535, 2),
    ("D_UINT32", "Uint32", 0, 4294967295, 4),
    # the shim's channel is int64_t, so the top of uint64 is out of reach and is not asked for
    ("D_UINT64", "Uint64", 0, 9223372036854775807, 8),
    ("D_INT8", "Int8", -128, 127, 1),
    ("D_INT16", "Int16", -32768, 32767, 2),
    ("D_INT32", "Int32", -2147483648, 2147483647, 4),
    ("D_INT64", "Int64", -9223372036854775808, 9223372036854775807, 8),
)

# float and double, with values that are exact in both
FLOAT_TYPES = (("D_FLOAT", "Float", 4), ("D_DOUBLE", "Double", 8))


def _t(lib, name: str) -> int:
    return getattr(lib, name)


# =====================================================================
# P548 - the seventeen arguments come back out
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P548_an_integer_item_remembers_how_it_was_built(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """Every constructor argument with a getter, checked against what was passed.

    Each is given a value that could not have arrived by accident - an id that is not zero, a group
    id different from the id, a unit that is not the empty string - so a getter reading the wrong
    field is visible rather than coincidentally right.
    """
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), key="THE_KEY", item_id=17, group_key="THE_GROUP",
                         group_id=4, val_def=0, val_min=type_min, val_max=type_max,
                         out_of_range=m.BOUND, unit="mA", after_reset=True)

    assert m.meta(ctx, "GetDataType") == _t(m, type_name)
    assert m.meta(ctx, "GetParamType") == m.PARAM
    assert m.meta(ctx, "GetKey") == "THE_KEY"
    assert m.meta(ctx, "GetId") == 17
    assert m.meta(ctx, "GetGroupKey") == "THE_GROUP"
    assert m.meta(ctx, "GetGroupId") == 4
    assert m.meta(ctx, "GetValOutOfRangeHandle") == m.BOUND
    assert m.meta(ctx, "GetValUnit") == "mA"
    assert m.meta(ctx, "GetValChangeTakesEffectAfterReset") is True


def test_ALX1553_P548_a_float_item_remembers_how_it_was_built(memsafe_lib, make_meta_item):
    """The same nine fields on the float path, which reaches a different constructor."""
    m = memsafe_lib
    ctx = make_meta_item(m.D_FLOAT, key="VOLTAGE", item_id=3, group_key="PWR", group_id=9,
                         val_def=1.5, val_min=-2.5, val_max=9.25, out_of_range=m.IGNORE,
                         unit="V", after_reset=False)

    assert m.meta(ctx, "GetDataType") == m.D_FLOAT
    assert m.meta(ctx, "GetKey") == "VOLTAGE"
    assert m.meta(ctx, "GetId") == 3
    assert m.meta(ctx, "GetGroupKey") == "PWR"
    assert m.meta(ctx, "GetGroupId") == 9
    assert m.meta(ctx, "GetValOutOfRangeHandle") == m.IGNORE
    assert m.meta(ctx, "GetValUnit") == "V"
    assert m.meta(ctx, "GetValChangeTakesEffectAfterReset") is False


def test_ALX1553_P548_a_bool_item_remembers_how_it_was_built(memsafe_lib, make_meta_item):
    """Bool takes a shorter constructor - no range and no enum - so it is checked on its own."""
    m = memsafe_lib
    ctx = make_meta_item(m.D_BOOL, key="ENABLED", item_id=5, group_key="CFG", group_id=1,
                         val_def=1, unit="", after_reset=True)

    assert m.meta(ctx, "GetDataType") == m.D_BOOL
    assert m.meta(ctx, "GetKey") == "ENABLED"
    assert m.meta(ctx, "GetId") == 5
    assert m.meta(ctx, "GetGroupKey") == "CFG"
    assert m.meta(ctx, "GetGroupId") == 1
    assert m.meta(ctx, "GetValUnit") == "", "a unit-less parameter reads as the empty string"
    assert m.meta(ctx, "GetValChangeTakesEffectAfterReset") is True
    assert m.meta(ctx, "GetValDefI") == 1


# =====================================================================
# P549 - the default, the minimum and the maximum, at each type's edges
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P549_an_integer_range_survives_the_constructor_at_the_types_edges(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """A range spanning the whole type, which is where a narrowing cast or a sign error shows."""
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), val_def=type_min, val_min=type_min, val_max=type_max)

    assert m.meta(ctx, "GetValDefI") == type_min
    assert m.meta(ctx, "GetValMinI") == type_min
    assert m.meta(ctx, "GetValMaxI") == type_max


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P549_a_default_away_from_the_edges_is_kept_exactly(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """A default that is neither the minimum nor the maximum, so the three cannot be confused."""
    m = memsafe_lib
    low, high = (10, 200) if type_min == 0 else (-100, 100)
    ctx = make_meta_item(_t(m, type_name), val_def=high - 1, val_min=low, val_max=high)

    assert m.meta(ctx, "GetValDefI") == high - 1
    assert m.meta(ctx, "GetValMinI") == low
    assert m.meta(ctx, "GetValMaxI") == high


@pytest.mark.parametrize(("type_name", "kind", "width"), FLOAT_TYPES)
def test_ALX1553_P549_a_float_range_survives_the_constructor(
        memsafe_lib, make_meta_item, type_name, kind, width):
    """Values exact in float and double alike, so a float item can be compared without a tolerance."""
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), val_def=1.5, val_min=-2.5, val_max=9.25)

    assert m.meta(ctx, "GetValDefF") == 1.5
    assert m.meta(ctx, "GetValMinF") == -2.5
    assert m.meta(ctx, "GetValMaxF") == 9.25


# =====================================================================
# P550 - a new item already holds its default
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P550_a_new_integer_item_reads_as_its_default(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """Nothing has been stored yet, so the value a getter answers with is the declared default."""
    m = memsafe_lib
    low, high = (10, 200) if type_min == 0 else (-100, 100)
    ctx = make_meta_item(_t(m, type_name), val_def=high - 1, val_min=low, val_max=high)

    assert m.meta_get_val(ctx, _t(m, type_name)) == high - 1


@pytest.mark.parametrize(("type_name", "kind", "width"), FLOAT_TYPES)
def test_ALX1553_P550_a_new_float_item_reads_as_its_default(
        memsafe_lib, make_meta_item, type_name, kind, width):
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), val_def=1.5, val_min=-2.5, val_max=9.25)

    assert m.meta_get_val(ctx, _t(m, type_name)) == 1.5


# =====================================================================
# P551 - a value can be set, read back, and put back to the default
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P551_an_integer_value_round_trips(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """Set then get, at both ends of the declared range and one value between them."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    low, high = (10, 200) if type_min == 0 else (-100, 100)
    ctx = make_meta_item(data_type, val_def=low, val_min=low, val_max=high)

    for value in (low, low + 1, high - 1, high):
        m.meta_set_val(ctx, data_type, value)
        assert m.meta_get_val(ctx, data_type) == value, f"{kind}: set {value}"


@pytest.mark.parametrize(("type_name", "kind", "width"), FLOAT_TYPES)
def test_ALX1553_P551_a_float_value_round_trips(
        memsafe_lib, make_meta_item, type_name, kind, width):
    m = memsafe_lib
    data_type = _t(m, type_name)
    ctx = make_meta_item(data_type, val_def=0.0, val_min=-2.5, val_max=9.25)

    for value in (-2.5, -0.25, 0.0, 1.5, 9.25):
        m.meta_set_val(ctx, data_type, value)
        assert m.meta_get_val(ctx, data_type) == value, f"{kind}: set {value}"


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P551_setting_to_default_undoes_a_set(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """The reset-to-default path, which a CLI and a factory-reset both reach."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    low, high = (10, 200) if type_min == 0 else (-100, 100)
    ctx = make_meta_item(data_type, val_def=high - 1, val_min=low, val_max=high)

    m.meta_set_val(ctx, data_type, low + 1)
    assert m.meta_get_val(ctx, data_type) == low + 1, "the set did not take"

    m.meta_set_val_to_def(ctx)
    assert m.meta_get_val(ctx, data_type) == high - 1


# =====================================================================
# P552 - what the out-of-range handle actually does
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P552_ignore_discards_a_value_outside_the_range(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """AlxParamItem_Ignore: the item keeps what it had. Measured, not assumed."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    low, high = (10, 200) if type_min == 0 else (-100, 100)
    ctx = make_meta_item(data_type, val_def=low, val_min=low, val_max=high, out_of_range=m.IGNORE)

    m.meta_set_val(ctx, data_type, low + 5)
    m.meta_set_val(ctx, data_type, high + 1)
    assert m.meta_get_val(ctx, data_type) == low + 5, "a value above the maximum was kept"

    m.meta_set_val(ctx, data_type, low - 1)
    assert m.meta_get_val(ctx, data_type) == low + 5, "a value below the minimum was kept"


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P552_bound_clamps_a_value_outside_the_range(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """AlxParamItem_Bound: the value is pulled to the edge it passed, not discarded."""
    m = memsafe_lib
    data_type = _t(m, type_name)
    low, high = (10, 200) if type_min == 0 else (-100, 100)
    ctx = make_meta_item(data_type, val_def=low, val_min=low, val_max=high, out_of_range=m.BOUND)

    m.meta_set_val(ctx, data_type, high + 1)
    assert m.meta_get_val(ctx, data_type) == high

    m.meta_set_val(ctx, data_type, low - 1)
    assert m.meta_get_val(ctx, data_type) == low


# =====================================================================
# P553 - the length a stored value occupies
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P553_an_integer_items_value_is_the_width_of_its_type(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """GetValLen is what the store writes and what a CAN service reads - it must be the C width."""
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), val_def=type_min, val_min=type_min, val_max=type_max)

    assert m.meta(ctx, "GetValLen") == width


@pytest.mark.parametrize(("type_name", "kind", "width"), FLOAT_TYPES)
def test_ALX1553_P553_a_float_items_value_is_the_width_of_its_type(
        memsafe_lib, make_meta_item, type_name, kind, width):
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), val_def=1.5, val_min=-2.5, val_max=9.25)

    assert m.meta(ctx, "GetValLen") == width


def test_ALX1553_P553_a_bool_items_value_is_one_byte(memsafe_lib, make_meta_item):
    m = memsafe_lib
    ctx = make_meta_item(m.D_BOOL, val_def=1)

    assert m.meta(ctx, "GetValLen") == 1


# =====================================================================
# P554 - an item built without an enum array is not an enum
# =====================================================================


@pytest.mark.parametrize(("type_name", "kind", "type_min", "type_max", "width"), INT_TYPES)
def test_ALX1553_P554_an_item_built_without_an_enum_list_is_not_an_enum(
        memsafe_lib, make_meta_item, type_name, kind, type_min, type_max, width):
    """isEnum false and a null list: the flag and the list are separate constructor arguments, and
    a parameter service asks this before it offers a caller a choice of values."""
    m = memsafe_lib
    ctx = make_meta_item(_t(m, type_name), val_def=type_min, val_min=type_min, val_max=type_max)

    assert m.meta(ctx, "GetIsEnum") is False
    assert m.meta(ctx, "GetBuffLen") == 0, "a scalar carries no buffer"
