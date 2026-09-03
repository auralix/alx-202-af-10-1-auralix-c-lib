"""ALX-1513 - alxCrc PC unit tests (L5, Tier 1 - the CRC that guards the stored parameters).

Contract under test:
  - Crc32 = standard CRC-32 (poly 0x04C11DB7 reflected, init/xorout 0xFFFFFFFF) - the same
    function as zlib.crc32; Ccitt = CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF, MSB first);
    Crc16 = CRC-16/ARC: poly 0x8005, init 0, input and output reflected (the LSB-first 0xA001 form)
  - GetLen: 2 bytes for the 16-bit configs, 4 for Crc32
  - IsOk(dataWithCrc): the trailing GetLen() bytes are the little-endian CRC of the bytes before
    them; on success the validated CRC is returned, on failure the out-parameter is untouched

Expected values come from zlib and from bit-serial reference models written here, never from
the implementation under test. Test group P9 = ALX-1513 CRC proofs.
"""

import random
import zlib

import pytest

pytestmark = pytest.mark.unit

CHECK = b"123456789"     # the classic CRC check vector


def crc_msb_first(data: bytes, width: int, poly: int, init: int, xorout: int) -> int:
    """Bit-serial reference model, MSB first, no reflection (the textbook shift register)."""
    mask = (1 << width) - 1
    top = 1 << (width - 1)
    rem = init
    for byte in data:
        rem ^= byte << (width - 8)
        for _ in range(8):
            if rem & top:
                rem = ((rem << 1) ^ poly) & mask
            else:
                rem = (rem << 1) & mask
    return rem ^ xorout


def crc_lsb_first(data: bytes, poly_reflected: int, init: int) -> int:
    """Bit-serial reference model for reflected CRC-16s (input and output reflected)."""
    rem = init
    for byte in data:
        rem ^= byte
        for _ in range(8):
            if rem & 1:
                rem = (rem >> 1) ^ poly_reflected
            else:
                rem >>= 1
    return rem & 0xFFFF


def ref(lib, cfg: int, data: bytes) -> int:
    if cfg == lib.CRC32:
        return zlib.crc32(data) & 0xFFFFFFFF
    if cfg == lib.CCITT:
        return crc_msb_first(data, 16, 0x1021, 0xFFFF, 0x0000)
    return crc_lsb_first(data, 0xA001, 0x0000)


def le(value: int, n: int) -> bytes:
    return value.to_bytes(n, "little")


# =====================================================================
# P9 - GetLen
# =====================================================================

def test_ALX1513_P9_crc_len_per_config(memsafe_lib):
    lib = memsafe_lib
    assert lib.crc_len(lib.CCITT) == 2
    assert lib.crc_len(lib.CRC16) == 2
    assert lib.crc_len(lib.CRC32) == 4


# =====================================================================
# P9 - Calc against independent references
# =====================================================================

def test_ALX1513_P9_crc32_check_vector_matches_the_standard(memsafe_lib):
    """CRC-32 of '123456789' is 0xCBF43926 in every reference table - and that is what the
    store puts behind the five parameter bytes."""
    lib = memsafe_lib
    assert lib.crc_calc(lib.CRC32, CHECK) == 0xCBF43926 == zlib.crc32(CHECK)


def test_ALX1513_P9_ccitt_check_vector_is_ccitt_false(memsafe_lib):
    lib = memsafe_lib
    assert lib.crc_calc(lib.CCITT, CHECK) == 0x29B1 == ref(lib, lib.CCITT, CHECK)


def test_ALX1513_P9_crc16_check_vector_is_crc16_arc(memsafe_lib):
    lib = memsafe_lib
    assert lib.crc_calc(lib.CRC16, CHECK) == 0xBB3D == ref(lib, lib.CRC16, CHECK)


@pytest.mark.parametrize("cfg", ["CRC32", "CCITT", "CRC16"])
@pytest.mark.parametrize("data", [b"", b"\x00", b"\xff", b"a", CHECK, bytes(range(256)), b"\x00" * 5, b"\xff" * 9],
                         ids=["empty", "zero", "ff", "a", "check", "0..255", "5xzero", "9xff"])
def test_ALX1513_P9_calc_equals_reference_model(memsafe_lib, cfg, data):
    lib = memsafe_lib
    c = getattr(lib, cfg)
    assert lib.crc_calc(c, data) == ref(lib, c, data)


@pytest.mark.parametrize("cfg", ["CRC32", "CCITT", "CRC16"])
def test_ALX1513_P9_calc_equals_reference_model_random_lengths(memsafe_lib, cfg):
    lib = memsafe_lib
    c = getattr(lib, cfg)
    rnd = random.Random(1513)
    for n in range(0, 70):
        data = bytes(rnd.getrandbits(8) for _ in range(n))
        assert lib.crc_calc(c, data) == ref(lib, c, data), f"len {n}"


# =====================================================================
# P9 - IsOk: the check the store relies on
# =====================================================================

@pytest.mark.parametrize("cfg", ["CRC32", "CCITT", "CRC16"])
def test_ALX1513_P9_is_ok_accepts_data_with_little_endian_crc(memsafe_lib, cfg):
    lib = memsafe_lib
    c = getattr(lib, cfg)
    data = bytes([0, 0, 50, 100, 70])                    # the product's factory defaults
    blob = data + le(ref(lib, c, data), lib.crc_len(c))
    ok, validated = lib.crc_is_ok(c, blob)
    assert ok is True
    assert validated == ref(lib, c, data)


def test_ALX1513_P9_is_ok_rejects_every_single_bit_flip(memsafe_lib):
    """One flipped bit anywhere in data or CRC must be caught (CRC-32 detects all 1-bit errors)."""
    lib = memsafe_lib
    data = bytes([0, 0, 50, 100, 70])
    blob = data + le(zlib.crc32(data), 4)
    for byte_idx in range(len(blob)):
        for bit in range(8):
            flipped = bytearray(blob)
            flipped[byte_idx] ^= 1 << bit
            ok, validated = lib.crc_is_ok(lib.CRC32, bytes(flipped))
            assert ok is False, f"flip byte {byte_idx} bit {bit} accepted"
            assert validated == 0xDEADBEEF, "out-parameter must stay untouched on failure"


def test_ALX1513_P9_is_ok_on_blank_flash_is_false(memsafe_lib):
    """A blank RWWEE row reads 0xFF everywhere - never a valid record."""
    lib = memsafe_lib
    ok, _ = lib.crc_is_ok(lib.CRC32, b"\xff" * 9)
    assert ok is False


def test_ALX1513_P9_is_ok_on_all_zero_is_false(memsafe_lib):
    """All-zero bytes are not a valid record either (CRC-32 of five zeros is not zero)."""
    lib = memsafe_lib
    ok, _ = lib.crc_is_ok(lib.CRC32, b"\x00" * 9)
    assert ok is False


def test_ALX1513_P9_is_ok_with_empty_payload(memsafe_lib):
    lib = memsafe_lib
    ok, validated = lib.crc_is_ok(lib.CRC32, le(zlib.crc32(b""), 4))
    assert ok is True and validated == zlib.crc32(b"")
