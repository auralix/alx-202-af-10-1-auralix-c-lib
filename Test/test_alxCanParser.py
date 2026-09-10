"""ALX-1553 - alxCanParser PC unit tests (Tier 1: the CAN payload codec, no state, no peripheral).

Twenty-four functions that read and write typed values inside eight bytes, and nothing tested them.
They are worth testing because every CAN frame a product sends or receives is built and taken apart
here: a byte in the wrong place is a number a client reads off a bus, and the bus has no way to
notice. The module needs no CAN peripheral to exercise - AlxCan_Msg is a plain structure - so all of
it is host code the moment it is compiled on a PC.

What these tests pin, in order of how easily it could break unnoticed:
  the byte ORDER of every multi-byte type, in both endians, which is the whole job of the module;
  the BIT numbering, which is LSB-first and stated in one comment in the header;
  the exact BYTES a write is allowed to touch, at every legal offset including the last one.

Two behaviours are recorded as characterization rather than asserted as design: the signed getters
are a reinterpretation of the same bytes the unsigned ones read, and a bit field wider than the byte
it starts in is silently truncated instead of spilling into the next byte.

The DLL is built with the module's asserts DISABLED, which is the default and what a product ships
unless it opts in. The asserts guard byteOffset only, and a failed assert calls abort() on the PC,
which would end the suite with no report; P156 proves the same property by measurement instead.

Test group P150-P158 = ALX-1553 CAN codec proofs.
"""

import ctypes
import struct

import pytest

from conftest import CanMsg

pytestmark = pytest.mark.unit

BIG, LITTLE = 0, 1                  # AlxCanParser_Endian, and Big means MSB at the lowest address
ENDIANS = (BIG, LITTLE)
PAYLOAD = 8

SIZES = {
    "Uint8": 1, "Int8": 1,
    "Uint16": 2, "Int16": 2,
    "Uint32": 4, "Int32": 4,
    "Uint64": 8, "Int64": 8,
    "Float": 4, "Double": 8,
}

# every value is exactly representable in the type it is written to, so a round trip is equality and
# not approximation - the floats are chosen for that reason, not for realism
VALUES = {
    "Uint8": (0, 1, 0x7F, 0xFF),
    "Int8": (-128, -1, 0, 127),
    "Uint16": (0, 0x1234, 0xFFFF),
    "Int16": (-32768, -1, 0x1234, 32767),
    "Uint32": (0, 0x12345678, 0xFFFFFFFF),
    "Int32": (-2147483648, -1, 0x12345678, 2147483647),
    "Uint64": (0, 0x123456789ABCDEF0, 0xFFFFFFFFFFFFFFFF),
    "Int64": (-(2**63), -1, 0x123456789ABCDEF0, 2**63 - 1),
    "Float": (0.0, 1.0, -1.0, 3.5, 65536.0, -0.15625),
    "Double": (0.0, 1.0, -1.0, 3.5, 65536.0, -0.15625),
}


def offsets(name):
    """Every byteOffset at which a value of this type still fits inside the eight bytes."""
    return range(PAYLOAD - SIZES[name] + 1)


def call_args(name, endian, offset):
    """The leading arguments of a Set/Get call: the one-byte types take no endian."""
    return (offset,) if SIZES[name] == 1 else (endian, offset)


def round_trip_cases():
    for name, size in SIZES.items():
        for endian in (ENDIANS if size > 1 else (BIG,)):
            for offset in offsets(name):
                for value in VALUES[name]:
                    side = "big" if endian == BIG else "little"
                    yield pytest.param(name, endian, offset, value,
                                       id=f"{name}-{side}-at{offset}-{value}")


# =====================================================================
# P150 - the round trip, exhaustively
# =====================================================================


@pytest.mark.parametrize(("name", "endian", "offset", "value"), list(round_trip_cases()))
def test_ALX1553_P150_every_type_round_trips_at_every_legal_offset(
    canparser_lib, name, endian, offset, value
):
    """Write it, read it back, get the same value - for every type, endian and offset that fits.

    Exhaustive rather than sampled: the offsets are 0 to 7 and the types are ten, so the whole space
    is a few hundred cases and there is no reason to guess which of them matters. It also means an
    off-by-one at the LAST legal offset of any type - the place such a bug actually lives - is
    covered by construction rather than by someone remembering to add it.
    """
    frame = canparser_lib.msg()
    args = call_args(name, endian, offset)
    canparser_lib.set(name, frame, *args, value)
    assert canparser_lib.get(name, frame, *args) == value


# =====================================================================
# P151 - byte order, which is the module's whole job
# =====================================================================


@pytest.mark.parametrize(("name", "value", "big_bytes"), [
    ("Uint16", 0x1234, "1234"),
    ("Uint32", 0x12345678, "12345678"),
    ("Uint64", 0x123456789ABCDEF0, "123456789abcdef0"),
])
def test_ALX1553_P151_big_puts_the_most_significant_byte_at_the_lowest_address(
    canparser_lib, name, value, big_bytes
):
    """The wire image, written out. Big is the most significant byte first; Little is its reverse.

    This is the one property a round-trip test cannot prove: Set and Get could agree with each other
    on a byte order that is wrong for everybody else on the bus, and every round trip would still
    pass. So the expected bytes are spelled out here as literals rather than computed.
    """
    big = canparser_lib.msg()
    canparser_lib.set(name, big, BIG, 0, value)
    little = canparser_lib.msg()
    canparser_lib.set(name, little, LITTLE, 0, value)

    size = SIZES[name]
    assert canparser_lib.payload(big)[:size].hex() == big_bytes
    assert canparser_lib.payload(little)[:size] == canparser_lib.payload(big)[:size][::-1]


# =====================================================================
# P152 - bit numbering
# =====================================================================


@pytest.mark.parametrize("bit", range(8))
def test_ALX1553_P152_bit_offset_0_is_the_least_significant_bit(canparser_lib, bit):
    """LSB first: bitOffset 0 addresses value 0x01, bitOffset 7 addresses 0x80.

    The header says so in a single comment line, which is exactly the kind of statement that gets
    read the other way round by whoever writes the next database file.
    """
    frame = canparser_lib.msg()
    canparser_lib.set("Bit", frame, 0, bit, True)
    assert canparser_lib.payload(frame)[0] == 1 << bit
    assert canparser_lib.get("Bit", frame, 0, bit) is True


def test_ALX1553_P152_the_bits_of_a_byte_are_independent(canparser_lib):
    """Setting one bit and clearing another leaves the rest of the byte alone, from either start."""
    ones = b"\xff" * PAYLOAD
    frame = canparser_lib.msg(ones)
    canparser_lib.set("Bit", frame, 3, 5, False)
    assert canparser_lib.payload(frame) == b"\xff\xff\xff\xdf\xff\xff\xff\xff"
    canparser_lib.set("Bit", frame, 3, 5, True)
    assert canparser_lib.payload(frame) == ones


# =====================================================================
# P153, P154 - what the signed and floating types put on the wire
# =====================================================================


@pytest.mark.parametrize(("name", "value", "big_bytes"), [
    ("Int8", -1, "ff"),
    ("Int8", -128, "80"),
    ("Int16", -1, "ffff"),
    ("Int16", -32768, "8000"),
    ("Int32", -1, "ffffffff"),
    ("Int32", -2147483648, "80000000"),
    ("Int64", -1, "ffffffffffffffff"),
])
def test_ALX1553_P153_the_signed_types_are_twos_complement_on_the_wire(
    canparser_lib, name, value, big_bytes
):
    """A negative value goes out as its two's complement image, not as a sign-magnitude one."""
    frame = canparser_lib.msg()
    canparser_lib.set(name, frame, *call_args(name, BIG, 0), value)
    assert canparser_lib.payload(frame)[:SIZES[name]].hex() == big_bytes


@pytest.mark.parametrize(("name", "value", "fmt"), [
    ("Float", 1.0, ">f"),
    ("Float", -1.0, ">f"),
    ("Float", 3.5, ">f"),
    ("Float", -0.15625, ">f"),
    ("Double", 1.0, ">d"),
    ("Double", -0.15625, ">d"),
])
def test_ALX1553_P154_float_and_double_are_ieee_754_on_the_wire(canparser_lib, name, value, fmt):
    """The bytes are the IEEE-754 image, checked against struct rather than against the module.

    The module builds them through a union over the HOST's byte order, so this is the test that
    would notice the day it is compiled somewhere that order is not little-endian.
    """
    frame = canparser_lib.msg()
    canparser_lib.set(name, frame, BIG, 0, value)
    assert canparser_lib.payload(frame)[:SIZES[name]] == struct.pack(fmt, value)


# =====================================================================
# P155 - enums, the only bit-field API
# =====================================================================


@pytest.mark.parametrize("bit_offset", range(6))
def test_ALX1553_P155_an_enum_writes_exactly_the_bits_it_claims(canparser_lib, bit_offset):
    """Three bits at a chosen offset, over a byte that starts all ones and then all zeros.

    Both directions matter: a field that only ORs its bits in looks correct over a zero byte and
    silently keeps stale ones over a byte that was already written.
    """
    mask = 0b111 << bit_offset
    for start, value in ((b"\x00" * PAYLOAD, 0b101), (b"\xff" * PAYLOAD, 0b010)):
        frame = canparser_lib.msg(start)
        canparser_lib.set("Enum", frame, 2, bit_offset, 3, value)
        byte = canparser_lib.payload(frame)[2]
        assert byte & mask == value << bit_offset, "the field did not take the value"
        assert byte & ~mask & 0xFF == start[2] & ~mask & 0xFF, "a bit outside the field moved"
        assert canparser_lib.get("Enum", frame, 2, bit_offset, 3) == value
        assert canparser_lib.payload(frame)[:2] == start[:2], "a neighbouring byte moved"
        assert canparser_lib.payload(frame)[3:] == start[3:], "a neighbouring byte moved"


# =====================================================================
# P156 - the bytes a write is allowed to touch
# =====================================================================


class _Guarded(ctypes.Structure):
    """A frame with sixteen witness bytes behind it, to catch a write that runs off the end."""

    _fields_ = (("frame", CanMsg), ("guard", ctypes.c_uint8 * 16))


@pytest.mark.parametrize("name", list(SIZES))
def test_ALX1553_P156_a_write_at_the_last_legal_offset_stays_inside_the_payload(
    canparser_lib, name
):
    """The largest value of each type, at the highest offset that still fits, against a witness.

    This is the property the module's asserts exist to protect, and it is the one that cannot be
    tested through them here: a failed assert aborts the process. Measuring it directly is better
    evidence anyway - it shows what the code DOES at the boundary rather than that a guard was
    written somewhere near it.
    """
    guarded = _Guarded()
    for i in range(16):
        guarded.guard[i] = 0xA5
    offset = PAYLOAD - SIZES[name]
    value = VALUES[name][-1]
    canparser_lib.set(name, guarded.frame, *call_args(name, BIG, offset), value)

    assert list(guarded.guard) == [0xA5] * 16, "the write ran past the eight data bytes"
    assert canparser_lib.get(name, guarded.frame, *call_args(name, BIG, offset)) == value
    assert canparser_lib.payload(guarded.frame)[:offset] == b"\x00" * offset, (
        "the write reached below its own offset"
    )


# =====================================================================
# P157, P158 - two behaviours recorded, not designed
# =====================================================================


def test_ALX1553_P157_a_bit_field_wider_than_its_byte_is_truncated_not_carried(canparser_lib):
    """CHARACTERIZATION: an enum that would cross a byte boundary loses its top bits, silently.

    ``SetEnum`` asserts ``bitOffset + numOfBits <= 8``, so this is a precondition violation and not
    a defect - but the asserts are off in every default build, so what actually happens is what a
    caller gets. Measured: four bits written at bit 6 store only the two that fit, the neighbouring
    byte is NOT touched, and ``GetEnum`` reads back the two.

    Recorded because it is also a statement about what the module can express: a CAN signal that
    spans a byte boundary - which is ordinary in a real database - has no representation here at
    all, and nothing at runtime says so.
    """
    frame = canparser_lib.msg()
    canparser_lib.set("Enum", frame, 0, 6, 4, 0b1111)
    assert canparser_lib.payload(frame) == b"\xc0" + b"\x00" * 7, "bits were dropped, not carried"
    assert canparser_lib.get("Enum", frame, 0, 6, 4) == 0b11


@pytest.mark.parametrize(("unsigned", "signed", "raw", "as_unsigned", "as_signed"), [
    ("Uint8", "Int8", 0xFF, 0xFF, -1),
    ("Uint16", "Int16", 0xFFFF, 0xFFFF, -1),
    ("Uint32", "Int32", 0x80000000, 0x80000000, -2147483648),
])
def test_ALX1553_P158_the_signed_and_unsigned_getters_read_the_same_bytes(
    canparser_lib, unsigned, signed, raw, as_unsigned, as_signed
):
    """CHARACTERIZATION: nothing on the wire says which of the two a field is.

    The same bytes answer to both getters, so the sign of a CAN signal lives entirely in the caller.
    Worth pinning because the implementation reaches it by assigning a uint8_t to an int8_t, whose
    result is only fixed by the compiler being two's complement - true everywhere the library runs,
    and still worth having a test that would notice.
    """
    frame = canparser_lib.msg()
    canparser_lib.set(unsigned, frame, *call_args(unsigned, BIG, 0), raw)
    assert canparser_lib.get(unsigned, frame, *call_args(unsigned, BIG, 0)) == as_unsigned
    assert canparser_lib.get(signed, frame, *call_args(signed, BIG, 0)) == as_signed
