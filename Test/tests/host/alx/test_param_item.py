"""ALX-1553 - alxParamItem string format conversion, PC unit tests (Tier 1: pure, no device).

AlxParamItem_SetVal_StrFormat is where a CLI `set-param` lands, and AlxParamItem_GetVal_StrFormat is
where a `get-param` reads back. A parameter declares valMin, valMax and a
AlxParamItem_ValOutOfRangeHandle, so a value outside the range should be refused or ignored
according to that policy - never accepted as a DIFFERENT value.

A bench measurement on a device found otherwise for integers, so these tests bring it down to the
host, where the fix can be proven without hardware. What they establish:

  - a value inside the range round-trips, for every scalar type
  - text with no number in it at all is refused, for every scalar type
  - a FLOAT outside its range is refused - so the bound check itself works
  - an INTEGER outside its RANGE, but inside its TYPE, is refused - so the check works there too
  - an INTEGER outside its TYPE is accepted and stored as a wrapped value      <- defect, P72
  - a value with trailing rubbish is accepted and the rubbish discarded        <- defect, P73

The bound check is present and correct; both defects are in the conversion in front of it. It parses
a numeric prefix in the C library manner and tells the caller nothing about what it ignored - not
that the number did not fit the type, and not that the string did not end where the number did.

Test group P70-P74 = ALX-1553 param-item string format proofs.
"""

import pytest

pytestmark = pytest.mark.unit


@pytest.fixture
def kinds(memsafe_lib):
    """The scalar types this module converts, with a range that spans the type."""
    m = memsafe_lib
    return {
        "uint8": (m.UINT8, 0, 255),
        "uint16": (m.UINT16, 0, 65535),
        "uint32": (m.UINT32, 0, 4294967295),
        "int8": (m.INT8, -128, 127),
        "int16": (m.INT16, -32768, 32767),
        "int32": (m.INT32, -2147483648, 2147483647),
    }


# =====================================================================
# P70 - what works
# =====================================================================


def test_ALX1553_P70_a_value_inside_the_range_round_trips(memsafe_lib, make_item, kinds):
    for name, (item_type, low, high) in kinds.items():
        item = make_item(item_type, 0, low, high)
        for value in (low, 0, 1, high):
            assert memsafe_lib.item_set_str(item, str(value)) == memsafe_lib.OK, f"{name} {value}"
            assert memsafe_lib.item_get_num(item) == value, f"{name} did not keep {value}"
            status, text = memsafe_lib.item_get_str(item)
            assert status == memsafe_lib.OK
            assert text.strip() == str(value), f"{name} read back {text!r}, expected {value}"


def test_ALX1553_P70_a_float_round_trips_including_negatives(memsafe_lib, make_item):
    item = make_item(memsafe_lib.FLOAT, 0.0, -100.0, 100.0)
    for value in (0.0, 1.5, -1.5, -8.7752, 99.5):
        assert memsafe_lib.item_set_str(item, str(value)) == memsafe_lib.OK
        assert memsafe_lib.item_get_num(item) == pytest.approx(value, rel=1e-6)


def test_ALX1553_P70_a_bool_round_trips(memsafe_lib, make_item):
    item = make_item(memsafe_lib.BOOL, 0)
    for text, expected in (("true", 1.0), ("false", 0.0)):
        assert memsafe_lib.item_set_str(item, text) == memsafe_lib.OK
        assert memsafe_lib.item_get_num(item) == expected


def test_ALX1553_P70_text_with_no_number_in_it_is_refused(memsafe_lib, make_item, kinds):
    """Text that begins with no digit is refused and leaves the value alone.

    Note what is NOT in this list: "12abc" and " 12" are ACCEPTED, which is P73. What the conversion
    detects is "there is no number here at all", not "this string is not a number".
    """
    for name, (item_type, low, high) in kinds.items():
        item = make_item(item_type, 7, low, high)
        for text in ("abc", "", " ", "--3"):
            before = memsafe_lib.item_get_num(item)
            assert memsafe_lib.item_set_str(item, text) != memsafe_lib.OK, f"{name} accepted {text!r}"
            assert memsafe_lib.item_get_num(item) == before, f"{name} changed on refused {text!r}"


# =====================================================================
# P71 - the bound check works, on both floats and integers
# =====================================================================


def test_ALX1553_P71_a_float_outside_its_range_is_refused(memsafe_lib, make_item):
    """The evidence that the bound check itself is present and correct."""
    item = make_item(memsafe_lib.FLOAT, 50.0, 0.0, 100.0)
    for value in ("-0.5", "-30", "100.5", "1000"):
        assert memsafe_lib.item_set_str(item, value) != memsafe_lib.OK, f"accepted {value}"
        assert memsafe_lib.item_get_num(item) == pytest.approx(50.0)


def test_ALX1553_P71_an_integer_outside_its_range_but_inside_its_type_is_refused(
    memsafe_lib, make_item
):
    """Same check, integers: a narrow range on a wide type is enforced.

    uint8 with the range 10..20 refuses 9 and 21 - both fit a uint8, so the conversion hands the
    real number to the bound check and the check does its job. Contrast with P72, where the number
    does not fit the type and never reaches the check as itself.
    """
    item = make_item(memsafe_lib.UINT8, 15, 10, 20)
    for value in ("9", "21", "0", "255"):
        assert memsafe_lib.item_set_str(item, value) != memsafe_lib.OK, f"accepted {value}"
        assert memsafe_lib.item_get_num(item) == 15


# =====================================================================
# P72 - the defect: a number that does not fit the type
# =====================================================================


@pytest.mark.xfail(
    strict=True,
    reason="the string to integer conversion truncates into the target type before the bound "
    "check runs, so a value that does not fit is stored as a different in-range value",
)
@pytest.mark.parametrize(
    ("kind", "low", "high", "sent", "wrapped_to"),
    [
        ("uint8", 0, 255, "999", 231),          # 999 mod 256
        ("uint8", 0, 255, "256", 0),
        ("uint8", 0, 255, "-1", 255),
        ("uint16", 0, 65535, "65536", 0),
        ("uint16", 0, 65535, "-1", 65535),
        ("int8", -128, 127, "128", -128),
        ("int8", -128, 127, "-129", 127),
    ],
)
def test_ALX1553_P72_a_value_that_does_not_fit_the_type_is_refused(
    memsafe_lib, make_item, kinds, kind, low, high, sent, wrapped_to
):
    """SEALED DEFECT - out-of-type values are accepted and stored wrapped.

    Found on a device through the CLI (set-param on a 0..255 parameter answered success for 999 and
    stored 231) and reproduced here on the host, which is where the fix belongs.

    The parametrisation states the measured wrap for each case, and the test asserts BOTH: that the
    value really is wrapped as measured, and that the call should have refused it. So the day the
    conversion learns to notice that the number does not fit, this XPASSes and the marker comes off
    in that commit - and until then it documents exactly what a caller gets.

    P71 is the other half of the argument: with a range the value CAN reach, the check refuses it
    correctly. Nothing is wrong with the bound check; the conversion never lets it see the value.
    """
    item_type = kinds[kind][0]
    item = make_item(item_type, 0, low, high)
    status = memsafe_lib.item_set_str(item, sent)
    assert memsafe_lib.item_get_num(item) == wrapped_to, (
        f"{kind} stored {memsafe_lib.item_get_num(item)} for {sent}, expected the measured wrap "
        f"to {wrapped_to}"
    )
    assert status != memsafe_lib.OK, f"{kind} accepted {sent}, which does not fit the type"


# =====================================================================
# P73 - the second defect: a numeric prefix is enough
# =====================================================================


@pytest.mark.xfail(
    strict=True,
    reason="the conversion parses a numeric prefix and discards the rest of the string instead of "
    "refusing it",
)
@pytest.mark.parametrize(("sent", "kept"), [("12abc", 12), (" 12", 12), ("12 ", 12)])
def test_ALX1553_P73_a_value_with_trailing_rubbish_is_refused(memsafe_lib, make_item, sent, kept):
    """SEALED DEFECT - "12abc" sets 12 and reports success.

    Same root as P72 and the same fix site: the conversion reads as far as it can and never tells
    the caller that the string did not end where the number did. Over a CLI that means
    `set-param --key X --val 12abc` silently configures 12, and the operator is told it worked.

    Leading and trailing spaces are in here too. Those are arguably harmless - a human typing into a
    terminal will produce them - so if the fix keeps accepting surrounding whitespace while refusing
    embedded rubbish, this test says so by turning green on the first case and needing an update on
    the other two. That is the right conversation to have at the fix, not now.
    """
    item = make_item(memsafe_lib.UINT8, 7, 0, 255)
    status = memsafe_lib.item_set_str(item, sent)
    assert memsafe_lib.item_get_num(item) == kept, f"{sent!r} stored something other than {kept}"
    assert status != memsafe_lib.OK, f"{sent!r} was accepted"


# =====================================================================
# P74 - integers and floats disagree about what a number looks like
# =====================================================================


def test_ALX1553_P74_the_integer_and_float_paths_read_different_notations(memsafe_lib, make_item):
    """CHARACTERIZATION: the two conversions do not accept the same language.

    Measured, same string into each:

        "0x10"   integer -> 0    (the parse stops at 'x')      float -> 16.0  (hex float understood)
        "1e2"    integer -> 1    (the parse stops at 'e')      float -> 100.0 (exponent understood)
        "7.9"    integer -> 7    (truncated at the point)      float -> 7.9

    Every one of these is reported as success. Nobody specified any of it, and it is defensible as
    "the integer path is base ten" - but a caller cannot tell from the answer which language was
    used, and a CLI user typing 0x10 gets 0 on one parameter and 16 on another. Pinned so a change
    is noticed, and so the fix for P72 and P73 has the full picture in front of it.
    """
    cases = (("0x10", 0, 16.0), ("1e2", 1, 100.0), ("7.9", 7, 7.9))
    for text, as_int, as_float in cases:
        integer = make_item(memsafe_lib.UINT8, 7, 0, 255)
        assert memsafe_lib.item_set_str(integer, text) == memsafe_lib.OK, f"integer refused {text!r}"
        assert memsafe_lib.item_get_num(integer) == as_int, f"integer read {text!r} differently"

        number = make_item(memsafe_lib.FLOAT, 7.0, -1000.0, 1000.0)
        assert memsafe_lib.item_set_str(number, text) == memsafe_lib.OK, f"float refused {text!r}"
        assert memsafe_lib.item_get_num(number) == pytest.approx(as_float, rel=1e-6), (
            f"float read {text!r} differently"
        )
