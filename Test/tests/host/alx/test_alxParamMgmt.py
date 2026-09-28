"""ALX-1553 - alxParamMgmt PC unit tests (Tier 1 over a real item table, no store, no device).

The view a product's CLI reaches its parameters through: how many there are, what each one is called,
which group it belongs to, and how to read or write it by index, by key or by id. This device's
firmware uses it - `get-param`, `set-param` and the factory-defaults path all end up here - and it
had no test.

The items under it are REAL `AlxParamItem`, six of them, declared in `alxMathTestHelpers`' sibling
`alxParamMgmtTestHelpers.c` and documented at the top of that file: two groups, five data types, and
a Var among the Params. So these tests exercise the whole lookup and conversion chain rather than a
stand-in for it, which is why the string formatting that P70-P74 seal on the item itself shows up
here as the values a dump would carry.

Asserts are ON in this group, as the product ships them - `alxParamItem.c` has side effects inside
its own asserts, and a closure built without them formats numbers as empty.

Test group P211-P218 = ALX-1553 parameter management proofs.
"""

import pytest

pytestmark = pytest.mark.unit

# the helper's table, as the tests expect to find it
TABLE = [
    {"key": "U8_A", "id": 10, "group_key": "GRP_A", "group_id": 100, "type": "Uint8", "len": 1},
    {"key": "U16_B", "id": 11, "group_key": "GRP_A", "group_id": 100, "type": "Uint16", "len": 2},
    {"key": "I32_C", "id": 12, "group_key": "GRP_B", "group_id": 200, "type": "Int32", "len": 4},
    {"key": "F_D", "id": 13, "group_key": "GRP_B", "group_id": 200, "type": "Float", "len": 4},
    {"key": "BOOL_E", "id": 14, "group_key": "GRP_A", "group_id": 100, "type": "Bool", "len": 1},
    {"key": "VAR_F", "id": 15, "group_key": "GRP_B", "group_id": 200, "type": "Uint32", "len": 4},
]
DEFAULTS = ["5", "1000", "-7", "1.500000", "true", "42"]
GRP_A, GRP_B = 100, 200


# =====================================================================
# P211-P213 - what the module knows about its table
# =====================================================================


def test_ALX1553_P211_the_counts_separate_parameters_from_variables(param_mgmt_lib):
    """Six items, five of them parameters and one a variable, and the two counts agree with the six.

    A product's CLI publishes these two groups separately - `get-param` and `get-var` - so a count
    that mixed them would be visible on the wire as a wrong-sized dump and nowhere else.
    """
    mgmt = param_mgmt_lib.table()
    assert param_mgmt_lib.count(mgmt) == len(TABLE)
    parameters = param_mgmt_lib.count_of_type(mgmt, param_mgmt_lib.PARAM)
    variables = param_mgmt_lib.count_of_type(mgmt, param_mgmt_lib.VAR)
    assert (parameters, variables) == (5, 1)
    assert parameters + variables == param_mgmt_lib.count(mgmt), "an item belongs to neither"


@pytest.mark.parametrize("index", range(len(TABLE)))
def test_ALX1553_P212_every_item_reports_its_own_identity(param_mgmt_lib, index):
    """Key, id, group key, group id, data type and value length, item by item.

    Seven accessors that each walk the same array with the same index, which is exactly the shape
    where one of them gets an off-by-one and the others do not.
    """
    mgmt = param_mgmt_lib.table()
    expected = TABLE[index]
    actual = param_mgmt_lib.by_index(mgmt, index)

    assert actual["key"] == expected["key"]
    assert actual["id"] == expected["id"]
    assert actual["group_key"] == expected["group_key"]
    assert actual["group_id"] == expected["group_id"]
    assert actual["data_type"] == param_mgmt_lib.TYPES[expected["type"]]
    assert actual["val_len"] == expected["len"], "the value length is the width of the C type"
    assert actual["param_type"] == (param_mgmt_lib.VAR if expected["key"] == "VAR_F"
                                    else param_mgmt_lib.PARAM)


def test_ALX1553_P213_the_dump_of_every_value_is_the_table_of_defaults(param_mgmt_lib):
    """A freshly constructed table reads back as its declared defaults, in index order.

    This is the whole of a `get-param` response in one assertion, and it is the test that would
    notice a formatting change - the float carries six decimals, the bool is the word `true`.
    """
    mgmt = param_mgmt_lib.table()
    assert param_mgmt_lib.values(mgmt) == DEFAULTS


# =====================================================================
# P214-P215 - the three ways in
# =====================================================================


@pytest.mark.parametrize(("key", "written", "index", "read_back"), [
    ("U8_A", "200", 0, "200"),
    ("U16_B", "7", 1, "7"),
    ("I32_C", "55", 2, "55"),
    ("F_D", "-3.25", 3, "-3.250000"),
    ("BOOL_E", "false", 4, "false"),
    ("VAR_F", "999", 5, "999"),
])
def test_ALX1553_P214_a_write_by_key_is_visible_by_index(param_mgmt_lib, key, written, index,
                                                         read_back):
    """Set by name, read by position: the two lookups reach the same item, for every data type.

    A Var is written the same way a Param is - nothing in this module refuses it - which is worth
    knowing for a product that expects its variables to be read-only from the CLI.
    """
    mgmt = param_mgmt_lib.table()
    assert param_mgmt_lib.set_by_key(mgmt, key, written) == param_mgmt_lib.OK
    assert param_mgmt_lib.value(mgmt, index) == read_back

    untouched = [i for i in range(len(TABLE)) if i != index]
    assert [param_mgmt_lib.value(mgmt, i) for i in untouched] == \
           [DEFAULTS[i] for i in untouched], "writing one item moved another"


def test_ALX1553_P215_an_unknown_key_or_id_is_refused_and_changes_nothing(param_mgmt_lib):
    """A name or number that is not in the table returns Err, and the table is untouched.

    The failure mode worth ruling out is the one where a miss falls through to index 0: a CLI is
    reachable by anyone, and a typo that silently writes the first parameter is worse than an error.
    """
    mgmt = param_mgmt_lib.table()
    assert param_mgmt_lib.set_by_key(mgmt, "NOPE", "1") == param_mgmt_lib.ERR
    assert param_mgmt_lib.set_by_key(mgmt, "u8_a", "1") == param_mgmt_lib.ERR, "keys are case sensitive"
    assert param_mgmt_lib.len_by_id(mgmt, 99)[0] == param_mgmt_lib.ERR
    assert param_mgmt_lib.values(mgmt) == DEFAULTS, "a refused write still changed something"


def test_ALX1553_P216_by_id_reads_and_writes_the_raw_value(param_mgmt_lib):
    """The binary path a CAN or API layer uses: the value's own bytes, not its text.

    Length first, then the bytes - and a write through it is visible to the text path, which is what
    makes the two views of one item the same item.
    """
    mgmt = param_mgmt_lib.table()
    status, length = param_mgmt_lib.len_by_id(mgmt, 11)          # U16_B, a uint16
    assert (status, length) == (param_mgmt_lib.OK, 2)

    status, raw = param_mgmt_lib.get_by_id(mgmt, 11, length)
    assert status == param_mgmt_lib.OK
    assert int.from_bytes(raw, "little") == 1000, "the default, as bytes"

    assert param_mgmt_lib.set_by_id(mgmt, 11, (4242).to_bytes(2, "little")) == param_mgmt_lib.OK
    assert param_mgmt_lib.value(mgmt, 1) == "4242", "the binary write is invisible to the text path"


# =====================================================================
# P217-P218 - back to the defaults
# =====================================================================


def test_ALX1553_P217_a_group_reset_restores_that_group_and_leaves_the_other_alone(param_mgmt_lib):
    """Change all six, restore one group, and exactly that group's three items come back.

    This is the module's half of the factory-defaults path, and the property that makes it usable:
    a product with a configuration group and a calibration group can reset one without the other.
    """
    mgmt = param_mgmt_lib.table()
    for entry, value in zip(TABLE, ["200", "7", "55", "-3.25", "false", "999"], strict=True):
        param_mgmt_lib.set_by_key(mgmt, entry["key"], value)
    changed = param_mgmt_lib.values(mgmt)
    assert changed != DEFAULTS, "nothing was changed, so the reset proves nothing"

    param_mgmt_lib.defaults_group(mgmt, GRP_A)
    after = param_mgmt_lib.values(mgmt)
    for index, entry in enumerate(TABLE):
        if entry["group_id"] == GRP_A:
            assert after[index] == DEFAULTS[index], f"{entry['key']} was not restored"
        else:
            assert after[index] == changed[index], f"{entry['key']} was restored and should not be"


def test_ALX1553_P218_resetting_everything_includes_the_variable_and_an_unknown_group_is_a_no_op(
    param_mgmt_lib
):
    """CHARACTERIZATION: `SetValToDef_All` restores Vars too, and an unknown group id does nothing.

    Two behaviours worth pinning because neither is stated. A Var is a runtime reading rather than a
    stored setting, so whether it HAS a factory default is a design question - here it does, and the
    reset writes it. And an unknown group id is silently ignored rather than refused or asserted, so
    a caller that passes the wrong number gets no reset and no complaint; there is no return value
    for it to check either.
    """
    mgmt = param_mgmt_lib.table()
    for entry, value in zip(TABLE, ["200", "7", "55", "-3.25", "false", "999"], strict=True):
        param_mgmt_lib.set_by_key(mgmt, entry["key"], value)
    changed = param_mgmt_lib.values(mgmt)

    param_mgmt_lib.defaults_group(mgmt, 999)
    assert param_mgmt_lib.values(mgmt) == changed, "an unknown group id reset something"

    param_mgmt_lib.defaults_all(mgmt)
    assert param_mgmt_lib.values(mgmt) == DEFAULTS, "the Var was not restored with the rest"
