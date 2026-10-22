"""ALX-1553 - the two AlxParamItem types that carry a buffer.

Arr and Str are shaped unlike the ten scalar types: the caller supplies the storage. Arr is built
with three buffers - the live value, the default, and the length they share - and Str with one
buffer plus a length that counts the null terminator. They were the last eight functions of the
module's 105 that no test had called.

Who owns that memory is the part worth being careful about, so the shim's context owns it. A test
that lent the item a Python buffer could prove something about memory that had already been freed.

Until 2026-10-01 AlxParamItem_SetValToDef left valDefPtr NULL for an Arr item and then passed it
to AlxParamItem_SetVal, which memcpys from it - with assertions on that was a null dereference,
and it took the test process down when I first reached it. A dead process cannot be xfailed. The
Arr branch returns after its assertion since f7fcf2a, so the call can be made now, and P664 makes
it: an Arr item has no default for SetValToDef to set, and its value is left as it was.

Proofs (ALX-1553): P562-P565; (ALX-1564, A87): P664.
"""

import pytest

pytestmark = pytest.mark.unit

ARR_DEF = bytes([1, 2, 3, 4])
ARR_NEW = bytes([9, 8, 7, 6])
STR_DEF = "hello"
STR_BUFF = 16


# =====================================================================
# P562 - an Arr item is built from the caller's three buffers
# =====================================================================


def test_ALX1553_P562_an_arr_item_reports_the_length_it_was_lent(memsafe_lib, make_buff_item):
    """An Arr item's length is valLen, and buffLen is NOT set for this type.

    Measured, and worth an assertion rather than a guess: AlxParamItem_CtorArr assigns
    me->valLen = valBuffLen and never touches me->buffLen, so GetBuffLen answers 0 on an Arr
    item while GetValLen answers the caller's length. A caller that sized a memcpy by
    GetBuffLen would copy nothing - which is exactly what this file's own shim did first.
    """
    m = memsafe_lib
    ctx = make_buff_item("arr", ARR_DEF, "CAL_TABLE", "raw")

    assert m.buff(ctx, "GetDataType") == m.D_ARR
    assert m.buff(ctx, "GetValLen") == len(ARR_DEF)
    assert m.buff(ctx, "GetBuffLen") == 0, "CtorArr does not set buffLen"
    assert m.buff(ctx, "GetKey") == "CAL_TABLE"
    assert m.buff(ctx, "GetValUnit") == "raw"


def test_ALX1553_P562_an_arr_item_starts_at_its_default(memsafe_lib, make_buff_item):
    """The constructor copies the default into the live buffer, so both read the same bytes."""
    m = memsafe_lib
    ctx = make_buff_item("arr", ARR_DEF)

    assert m.buff_get_arr(ctx, len(ARR_DEF)) == ARR_DEF
    assert m.buff_get_def_arr(ctx, len(ARR_DEF)) == ARR_DEF


def test_ALX1553_P562_an_arr_value_round_trips_and_the_default_does_not_move(
        memsafe_lib, make_buff_item):
    """The two buffers are separate: writing the value must not write the default.

    They are separate arguments to the constructor and a product passes two different arrays, so a
    single memcpy to the wrong one would be invisible until somebody asked for the default back.
    """
    m = memsafe_lib
    ctx = make_buff_item("arr", ARR_DEF)

    m.buff_set_arr(ctx, ARR_NEW)
    assert m.buff_get_arr(ctx, len(ARR_NEW)) == ARR_NEW
    assert m.buff_get_def_arr(ctx, len(ARR_DEF)) == ARR_DEF, "the set overwrote the default"


def test_ALX1553_P562_an_arr_item_holds_bytes_that_are_not_text(memsafe_lib, make_buff_item):
    """Zeros and high bytes: an Arr is not a string and must not stop at a null."""
    m = memsafe_lib
    raw = bytes([0x00, 0xFF, 0x00, 0x80, 0x00])
    ctx = make_buff_item("arr", raw)

    assert m.buff_get_arr(ctx, len(raw)) == raw

    other = bytes([0xFF, 0x00, 0xFF, 0x00, 0xFF])
    m.buff_set_arr(ctx, other)
    assert m.buff_get_arr(ctx, len(other)) == other


# =====================================================================
# P563 - a Str item and the buffer it was lent
# =====================================================================


def test_ALX1553_P563_a_str_item_reports_the_buffer_it_was_lent(memsafe_lib, make_buff_item):
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF, "DEVICE_NAME", "")

    assert m.buff(ctx, "GetDataType") == m.D_STR
    assert m.buff(ctx, "GetBuffLen") == STR_BUFF
    assert m.buff(ctx, "GetKey") == "DEVICE_NAME"
    assert m.buff(ctx, "GetValDefStr") == STR_DEF


def test_ALX1553_P563_a_str_item_starts_at_its_default(memsafe_lib, make_buff_item):
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)

    status, value = m.buff_get_str(ctx)
    assert status == m.OK
    assert value == STR_DEF


def test_ALX1553_P563_a_str_value_round_trips(memsafe_lib, make_buff_item):
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)

    assert m.buff_set_str(ctx, "world") == m.OK
    status, value = m.buff_get_str(ctx)
    assert status == m.OK
    assert value == "world"
    assert m.buff(ctx, "GetValDefStr") == STR_DEF, "the set overwrote the default"


def test_ALX1553_P563_the_empty_string_is_a_value(memsafe_lib, make_buff_item):
    """A zero-length string is legal and is not the same as never having been set."""
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)

    assert m.buff_set_str(ctx, "") == m.OK
    status, value = m.buff_get_str(ctx)
    assert status == m.OK
    assert value == ""


# =====================================================================
# P564 - the buffer's length is a limit, at both ends
# =====================================================================


def test_ALX1553_P564_a_string_that_exactly_fills_the_buffer_is_accepted(
        memsafe_lib, make_buff_item):
    """The length counts the null terminator, so buffLen-1 characters fit - the same contract
    AlxRange_CheckStr is held to in P77."""
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)
    longest = "x" * (STR_BUFF - 1)

    assert m.buff_set_str(ctx, longest) == m.OK
    status, value = m.buff_get_str(ctx)
    assert status == m.OK
    assert value == longest


def test_ALX1553_P564_a_string_one_character_too_long_is_refused(memsafe_lib, make_buff_item):
    """And the item keeps what it had - a refused set must not leave a half-written buffer."""
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)

    assert m.buff_set_str(ctx, "fits") == m.OK
    assert m.buff_set_str(ctx, "x" * STR_BUFF) != m.OK

    status, value = m.buff_get_str(ctx)
    assert status == m.OK
    assert value == "fits", "the refused set changed the value anyway"


def test_ALX1553_P564_reading_into_a_buffer_too_small_is_refused(memsafe_lib, make_buff_item):
    """The read side of the same contract: the caller's buffer has to hold the value and its null."""
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)
    assert m.buff_set_str(ctx, "abcdef") == m.OK

    assert m.buff_get_str(ctx, len("abcdef"))[0] != m.OK, "six characters need seven bytes"
    assert m.buff_get_str(ctx, len("abcdef") + 1) == (m.OK, "abcdef")


# =====================================================================
# P565 - a Str item's default survives being set to
# =====================================================================


def test_ALX1553_P565_the_default_string_is_readable_after_every_set(memsafe_lib, make_buff_item):
    """GetValDefStr answers from the constructor's string, not from the live buffer.

    Worth its own proof: both are char buffers on the same item, and a getter reading the wrong one
    would agree with the right one until the first set - which is to say, in every test that does
    not set anything first.
    """
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)

    for value in ("one", "two", "three"):
        assert m.buff_set_str(ctx, value) == m.OK
        assert m.buff(ctx, "GetValDefStr") == STR_DEF, f"the default moved after setting {value!r}"


# =====================================================================
# P664 - SetValToDef has nothing to set on an Arr item
# =====================================================================


@pytest.mark.expect_assert("SetValToDef on an Arr item: the Arr branch asserts, as it says, and returns")
def test_ALX1564_P664_set_val_to_def_on_an_arr_item_asserts_and_leaves_the_value(memsafe_lib, make_buff_item):
    """The Arr branch of SetValToDef is an assertion that it is never reached, and then a return.

    Before f7fcf2a the return was missing and SetVal was handed a NULL default; the value is the
    proof that nothing is copied now - neither the default over it nor anything else.
    """
    m = memsafe_lib
    ctx = make_buff_item("arr", ARR_DEF)
    m.buff_set_arr(ctx, ARR_NEW)

    m.buff_set_val_to_def(ctx)

    assert m.buff_get_arr(ctx, len(ARR_NEW)) == ARR_NEW, "SetValToDef changed an Arr value"
    assert m.buff_get_def_arr(ctx, len(ARR_DEF)) == ARR_DEF


# =====================================================================
# P741, P742, P745, P748 - a Str back to its default, a form too long, an Arr's string form, a Str bounded (A97)
# =====================================================================


def test_ALX1564_P741_a_str_goes_back_to_its_default(memsafe_lib, make_buff_item):
    """SetValToDef takes the Str branch: the default pointer is the string itself, not its address."""
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)
    assert m.buff_set_str(ctx, "other") == m.OK

    m.buff_set_val_to_def(ctx)

    assert m.buff_get_str(ctx) == (m.OK, STR_DEF)


def test_ALX1564_P742_the_string_form_of_a_str_is_refused_when_the_caller_s_buffer_is_too_small(
        memsafe_lib, make_buff_item):
    """GetVal_StrFormat's Str path checks the value against the caller's length before it copies."""
    m = memsafe_lib
    ctx = make_buff_item("str", STR_DEF, STR_BUFF)

    status, text = m.buff_get_str_format(ctx, size=3)

    assert status != m.OK
    assert text == ""


@pytest.mark.expect_assert("P745: an Arr item has no string form - both conversions assert")
def test_ALX1564_P745_an_arr_item_has_no_string_form(memsafe_lib, make_buff_item):
    """CHARACTERIZATION: reading gives the empty string and Alx_Ok, setting gives Alx_Err and changes nothing."""
    m = memsafe_lib
    ctx = make_buff_item("arr", ARR_DEF)

    assert m.buff_get_str_format(ctx) == (m.OK, "")
    assert m.buff_set_str_format(ctx, "1") == m.ERR
    assert m.buff_get_arr(ctx, len(ARR_DEF)) == ARR_DEF


def test_ALX1564_P748_under_the_bound_handle_a_string_too_long_is_cut_to_the_buffer(memsafe_lib, make_buff_item):
    """AlxBound_Str keeps what fits and says the length was wrong; the cut value counts as a change."""
    m = memsafe_lib
    ctx = make_buff_item("str_with", "abc", 8, m.BOUND)

    assert m.buff_set_str(ctx, "0123456789") == m.BOUND_ERR_LEN
    assert m.buff_get_str(ctx) == (m.OK, "0123456")
