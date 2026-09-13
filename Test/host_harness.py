"""Host access harness: ctypes interfaces and test-instance access; builds live in host_build.py."""
import ctypes
from pathlib import Path
from typing import ClassVar

from host_build import (
    VARIANT_GROUPS,
    _build_variant_dll,
    _needs_build,
    _variant_dll,
)


def _variant_lib(group: str, variant: str, wrapper):
    """The group built in one configuration, wrapped. Used by the parametrized variant fixtures."""
    dll = _variant_dll(group, variant)
    if _needs_build(dll, VARIANT_GROUPS[group].deps):
        _build_variant_dll(group, variant)
    return wrapper(dll)


_LOADED_DLLS: list = []


def _register_lib(c) -> None:
    """Every wrapper's ctypes handle, so one fixture can watch all of them."""
    if c not in _LOADED_DLLS:
        c.AlxAssertPc_Count.restype = ctypes.c_uint32
        c.AlxAssertPc_First.restype = ctypes.c_char_p
        _LOADED_DLLS.append(c)


# ---------------------------------------------------------------- ctypes -----
class Lib:
    """ctypes wrapper around alxFifoTest.dll with contract-shaped helpers."""

    def __init__(self, dll_path: Path):
        self.c = ctypes.CDLL(str(dll_path))
        c = self.c
        _register_lib(c)
        c.AlxFifoTest_New.restype = ctypes.c_void_p
        c.AlxFifoTest_New.argtypes = [ctypes.c_uint32]
        c.AlxFifoTest_Delete.argtypes = [ctypes.c_void_p]
        c.AlxFifo_Flush.argtypes = [ctypes.c_void_p]
        c.AlxFifo_Read.restype = ctypes.c_int32
        c.AlxFifo_Read.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
        c.AlxFifo_Write.restype = ctypes.c_int32
        c.AlxFifo_Write.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
        c.AlxFifo_WriteStr.restype = ctypes.c_int32
        c.AlxFifo_WriteStr.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        c.AlxFifo_GetNumOfEntries.restype = ctypes.c_uint32
        c.AlxFifo_GetNumOfEntries.argtypes = [ctypes.c_void_p]
        c.AlxFifo_Rewind.restype = ctypes.c_uint32
        c.AlxFifo_Rewind.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
        for fn in ("AlxFifo_ReadStrUntil", "AlxFifo_ReadStrUntilAny"):
            f = getattr(c, fn)
            f.restype = ctypes.c_int32
            f.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p,
                          ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32)]

        def status(name: str) -> int:
            f = getattr(c, f"AlxFifoTest_Status_{name}")
            f.restype = ctypes.c_int32
            return f()

        self.OK = status("Ok")
        self.ERR = status("Err")
        self.ERR_FULL = status("ErrFull")
        self.ERR_EMPTY = status("ErrEmpty")
        self.ERR_NO_DELIM = status("ErrNoDelim")
        self.ERR_TOO_LONG = status("ErrTooLong")

    # -- helpers used by every test --------------------------------------
    def new(self, buff_len: int):
        return self.c.AlxFifoTest_New(buff_len)

    def delete(self, f):
        self.c.AlxFifoTest_Delete(f)

    def write(self, f, data: bytes) -> int:
        return self.c.AlxFifo_Write(f, data, len(data))

    def read(self, f, n: int):
        buf = (ctypes.c_char * n)()
        status = self.c.AlxFifo_Read(f, buf, n)
        return status, bytes(buf)

    def entries(self, f) -> int:
        return self.c.AlxFifo_GetNumOfEntries(f)

    POISON = 0xAA

    def _read_str(self, fn_name, f, delim: bytes, ln: int, len_actual_null=False):
        """Call ReadStrUntil/ReadStrUntilAny with a poison-filled buffer.

        Returns (status, content_bytes, len_actual, raw_buffer_bytes).
        """
        buf = (ctypes.c_ubyte * ln)(*([self.POISON] * ln))
        buf_p = ctypes.cast(buf, ctypes.c_char_p)
        la = ctypes.c_uint32(0xDEADBEEF)
        la_arg = None if len_actual_null else ctypes.byref(la)
        status = getattr(self.c, fn_name)(f, buf_p, delim, ln, la_arg)
        raw = bytes(buf)
        n = la.value if not len_actual_null else None
        content = raw[:n] if n is not None and n <= ln else None
        return status, content, n, raw

    def ru(self, f, delim: bytes, ln: int, **kw):
        return self._read_str("AlxFifo_ReadStrUntil", f, delim, ln, **kw)

    def ru_any(self, f, delim_set: bytes, ln: int, **kw):
        return self._read_str("AlxFifo_ReadStrUntilAny", f, delim_set, ln, **kw)


# ------------------------------------------------------- traceability ----
# The proof token in a test's NAME (test_ALX1514_P4_...) is the primary traceability link (spec PROOF
# row <-> test <-> commit, greppable). alx.verify.evidence (pytest_plugins above) mirrors it into the
# junit XML as <property name="proof"/> and every @pytest.mark.req("ALX-<key>-P<n>") as <property
# name="req"/>; the test name keeps its ORIGINATING token forever (never rename on later modification).


class BoundLib:
    """ctypes wrapper for the AlxBound functions (same test-group DLL as alxFifo).

    Bound functions act on caller scalars/buffers - no opaque handle. Each
    numeric entry maps to (ctypes scalar type, AlxBound function).
    """

    NUMERIC: ClassVar[dict[str, type]] = {
        "Uint8": ctypes.c_uint8, "Uint16": ctypes.c_uint16,
        "Uint32": ctypes.c_uint32, "Uint64": ctypes.c_uint64,
        "Int8": ctypes.c_int8, "Int16": ctypes.c_int16,
        "Int32": ctypes.c_int32, "Int64": ctypes.c_int64,
        "Float": ctypes.c_float, "Double": ctypes.c_double,
    }

    def __init__(self, cdll):
        self.c = cdll
        for name, ct in self.NUMERIC.items():
            f = getattr(cdll, f"AlxBound_{name}")
            f.restype = ctypes.c_int32
            f.argtypes = [ctypes.POINTER(ct), ct, ct]
        cdll.AlxBound_Str.restype = ctypes.c_int32
        cdll.AlxBound_Str.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_uint32]

        def status(name: str) -> int:
            f = getattr(cdll, f"AlxBoundTest_Status_{name}")
            f.restype = ctypes.c_int32
            return f()

        self.OK = status("Ok")
        self.ERR_MIN = status("ErrMin")
        self.ERR_MAX = status("ErrMax")
        self.ERR_LEN = status("ErrLen")

    def bound(self, name: str, val, val_min, val_max):
        """Call AlxBound_<name>; returns (status, bounded_value)."""
        ct = self.NUMERIC[name]
        v = ct(val)
        status = getattr(self.c, f"AlxBound_{name}")(ctypes.byref(v), val_min, val_max)
        return status, v.value

    POISON = 0xAA

    def bound_str(self, val: bytes, max_len_with_nul: int, buf_len: int | None = None):
        """Call AlxBound_Str with a poison-filled destination.

        Returns (status, content_up_to_nul, raw_buffer_bytes)."""
        n = buf_len if buf_len is not None else max_len_with_nul
        buf = (ctypes.c_ubyte * n)(*([self.POISON] * n))
        status = self.c.AlxBound_Str(ctypes.cast(buf, ctypes.c_char_p), val, max_len_with_nul)
        raw = bytes(buf)
        content = raw.split(b"\x00", 1)[0]
        return status, content, raw


class CliLib:
    """ctypes wrapper around alxCliTest.dll (Tier-2: real CLI, faked serial)."""

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        c.AlxCliTest_New.restype = ctypes.c_void_p
        c.AlxCliTest_Delete.argtypes = [ctypes.c_void_p]
        c.AlxCliTest_Handle.argtypes = [ctypes.c_void_p]
        c.AlxCliTest_Port.restype = ctypes.c_void_p
        c.AlxCliTest_Port.argtypes = [ctypes.c_void_p]
        c.AlxCliTest_GetBuffLen.restype = ctypes.c_uint32
        c.AlxCliTest_GetStrValBuffLen.restype = ctypes.c_uint32
        c.AlxCliTest_WasResetRequested.restype = ctypes.c_bool
        c.AlxCliTest_Status_Ok.restype = ctypes.c_int32
        c.AlxSerialPortFake_InjectRx.restype = ctypes.c_int32
        c.AlxSerialPortFake_InjectRx.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
        c.AlxSerialPortFake_TxRead.restype = ctypes.c_uint32
        c.AlxSerialPortFake_TxRead.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
        self.OK = c.AlxCliTest_Status_Ok()

    def buff_len(self) -> int:
        return self.c.AlxCliTest_GetBuffLen()

    def str_val_buff_len(self) -> int:
        """Value buffer of the helper's STR_TEST param (value <= len-1 chars)."""
        return self.c.AlxCliTest_GetStrValBuffLen()

    def was_reset(self) -> bool:
        return self.c.AlxCliTest_WasResetRequested()

    def clear_reset(self):
        self.c.AlxCliTest_ClearResetRequested()


class CliUnderTest:
    """One CLI instance: inject RX bytes, poll Handle, drain TX responses."""

    def __init__(self, cli_lib: CliLib, ctx):
        self._l = cli_lib
        self.ctx = ctx
        self.port = cli_lib.c.AlxCliTest_Port(ctx)

    def inject(self, data: bytes):
        assert self._l.c.AlxSerialPortFake_InjectRx(self.port, data, len(data)) == self._l.OK

    def handle(self):
        self._l.c.AlxCliTest_Handle(self.ctx)

    def tx(self) -> bytes:
        out = b""
        while True:
            buf = ctypes.create_string_buffer(4096)
            n = self._l.c.AlxSerialPortFake_TxRead(self.port, buf, 4096)
            if n == 0:
                return out
            out += buf.raw[:n]


class MemSafeLib:
    """ctypes wrapper around alxMemSafeTest.dll: the safe-store chain (AlxCrc -> AlxMemRaw fake
    -> AlxMemSafe -> AlxParamGroup -> AlxParamStore) plus the fake's test controls.
    Fake operation kinds for fail_at()/count(): INIT=0, DEINIT=1, READ=2, WRITE=3.
    CRC configs: CCITT=0, CRC16=1, CRC32=2 (AlxCrc_Config enum order)."""

    INIT, DEINIT, READ, WRITE = 0, 1, 2, 3
    CCITT, CRC16, CRC32 = 0, 1, 2
    ALWAYS = 0xFFFFFFFF

    # -- the trace axis, as on LinLib. This group is the first in the variant matrix built in TWO
    # -- steps (strict sources plus a warnings-off closure), which is the part of the harness the
    # -- one-step fifo and lin groups cannot exercise.
    def trace_reset(self) -> None:
        self.c.AlxTracePc_Reset()

    def traces(self) -> int:
        self.c.AlxTracePc_Count.restype = ctypes.c_uint32
        return self.c.AlxTracePc_Count()

    def traces_at(self, level: int) -> int:
        self.c.AlxTracePc_CountAtLevel.restype = ctypes.c_uint32
        self.c.AlxTracePc_CountAtLevel.argtypes = [ctypes.c_uint8]
        return self.c.AlxTracePc_CountAtLevel(level)

    def trace_level_configured(self) -> int:
        self.c.AlxTracePc_LevelConfigured.restype = ctypes.c_uint8
        return self.c.AlxTracePc_LevelConfigured()
    POISON = 0xAA

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u32, i32 = ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int32
        u8p, b = ctypes.POINTER(ctypes.c_uint8), ctypes.c_bool
        c.AlxMemSafeTest_New.restype = vp
        c.AlxMemSafeTest_New.argtypes = [u32, u32, ctypes.c_uint8, ctypes.c_uint8]
        c.AlxMemSafeTest_Delete.argtypes = [vp]
        for name in ("CopyLen", "NumOfItems"):
            getattr(c, f"AlxMemSafeTest_{name}").restype = u32
        for name in ("MemSafeRead", "MemSafeWrite"):
            f = getattr(c, f"AlxMemSafeTest_{name}")
            f.restype = i32
            f.argtypes = [vp, u8p, u32]
        for name in ("MemSafeIsReadDone", "MemSafeIsReadErr", "MemSafeIsWriteDone", "MemSafeIsWriteErr",
                     "GroupIsValStoredBuffDiff", "StoreIsErr"):
            f = getattr(c, f"AlxMemSafeTest_{name}")
            f.restype = b
            f.argtypes = [vp]
        for name in ("GroupInit", "StoreInit"):
            f = getattr(c, f"AlxMemSafeTest_{name}")
            f.restype = i32
            f.argtypes = [vp]
        c.AlxMemSafeTest_StoreHandle.argtypes = [vp]
        c.AlxMemSafeTest_ItemGet.restype = u32
        c.AlxMemSafeTest_ItemGet.argtypes = [vp, u32]
        c.AlxMemSafeTest_ItemGetDef.restype = u32
        c.AlxMemSafeTest_ItemGetDef.argtypes = [vp, u32]
        c.AlxMemSafeTest_ItemSet.restype = i32
        c.AlxMemSafeTest_ItemSet.argtypes = [vp, u32, u32]
        c.AlxMemSafeTest_CrcCalc.restype = u32
        c.AlxMemSafeTest_CrcCalc.argtypes = [u32, u8p, u32]
        c.AlxMemSafeTest_CrcIsOk.restype = b
        c.AlxMemSafeTest_CrcIsOk.argtypes = [u32, u8p, u32, ctypes.POINTER(u32)]
        c.AlxMemSafeTest_CrcLen.restype = u32
        c.AlxMemSafeTest_CrcLen.argtypes = [u32]
        c.AlxMemRawFake_Fill.argtypes = [ctypes.c_uint8]
        c.AlxMemRawFake_Peek.argtypes = [u32, u8p, u32]
        c.AlxMemRawFake_Poke.argtypes = [u32, u8p, u32]
        c.AlxMemRawFake_FailAt.argtypes = [u32, u32]
        c.AlxMemRawFake_Count.restype = u32
        c.AlxMemRawFake_Count.argtypes = [u32]
        c.AlxMemRawFake_PowerLossAt.argtypes = [u32, u32]
        c.AlxMemRawFake_IsPowerLost.restype = b
        c.AlxMemRawFake_SetRowEraseModel.argtypes = [b, u32]
        for name in ("Size", "LastNumOfTries", "LastTimeout_ms", "LastWriteAddr", "LastWriteLen"):
            getattr(c, f"AlxMemRawFake_{name}").restype = u32
        c.AlxParamItemStrTest_New.restype = vp
        c.AlxParamItemStrTest_New.argtypes = [u32, ctypes.c_double, ctypes.c_double, ctypes.c_double]
        c.AlxParamItemStrTest_Delete.argtypes = [vp]
        c.AlxParamItemStrTest_SetStr.restype = i32
        c.AlxParamItemStrTest_SetStr.argtypes = [vp, ctypes.c_char_p]
        c.AlxParamItemStrTest_GetStr.restype = i32
        c.AlxParamItemStrTest_GetStr.argtypes = [vp, ctypes.c_char_p, u32]
        c.AlxParamItemStrTest_GetNum.restype = ctypes.c_double
        c.AlxParamItemStrTest_GetNum.argtypes = [vp]
        # the metadata shim: one context per data type, values widened to int64/double
        cp, i64, f64 = ctypes.c_char_p, ctypes.c_int64, ctypes.c_double
        c.AlxParamItemMetaTest_New.restype = vp
        c.AlxParamItemMetaTest_New.argtypes = [u32, cp, u32, cp, u32, i64, i64, i64,
                                               f64, f64, f64, u32, cp, ctypes.c_bool]
        c.AlxParamItemMetaTest_Delete.argtypes = [vp]
        for name in ("GetDataType", "GetParamType", "GetId", "GetGroupId",
                     "GetValOutOfRangeHandle", "GetBuffLen", "GetValLen"):
            fn = getattr(c, f"AlxParamItemMetaTest_{name}")
            fn.restype, fn.argtypes = u32, [vp]
        for name in ("GetKey", "GetGroupKey", "GetValUnit"):
            fn = getattr(c, f"AlxParamItemMetaTest_{name}")
            fn.restype, fn.argtypes = cp, [vp]
        for name in ("GetValChangeTakesEffectAfterReset", "GetIsEnum"):
            fn = getattr(c, f"AlxParamItemMetaTest_{name}")
            fn.restype, fn.argtypes = ctypes.c_bool, [vp]
        for name in ("GetValDefI", "GetValMinI", "GetValMaxI", "GetValI"):
            fn = getattr(c, f"AlxParamItemMetaTest_{name}")
            fn.restype, fn.argtypes = i64, [vp]
        for name in ("GetValDefF", "GetValMinF", "GetValMaxF", "GetValF"):
            fn = getattr(c, f"AlxParamItemMetaTest_{name}")
            fn.restype, fn.argtypes = f64, [vp]
        c.AlxParamItemMetaTest_SetValI.restype = i32
        c.AlxParamItemMetaTest_SetValI.argtypes = [vp, i64]
        c.AlxParamItemMetaTest_SetValF.restype = i32
        c.AlxParamItemMetaTest_SetValF.argtypes = [vp, f64]
        # the enum-list shim: the item carries a copy of the list it was built from
        c.AlxParamItemEnumTest_New.restype = vp
        c.AlxParamItemEnumTest_New.argtypes = [u32, ctypes.POINTER(i64), ctypes.POINTER(f64),
                                               ctypes.c_uint8, i64, i64, i64,
                                               f64, f64, f64, u32]
        c.AlxParamItemEnumTest_Delete.argtypes = [vp]
        c.AlxParamItemEnumTest_GetIsEnum.restype = ctypes.c_bool
        c.AlxParamItemEnumTest_GetIsEnum.argtypes = [vp]
        c.AlxParamItemEnumTest_GetEnumArrLen.restype = ctypes.c_uint8
        c.AlxParamItemEnumTest_GetEnumArrLen.argtypes = [vp]
        c.AlxParamItemEnumTest_GetEnumArrAtI.restype = i64
        c.AlxParamItemEnumTest_GetEnumArrAtI.argtypes = [vp, ctypes.c_uint8]
        c.AlxParamItemEnumTest_GetEnumArrAtF.restype = f64
        c.AlxParamItemEnumTest_GetEnumArrAtF.argtypes = [vp, ctypes.c_uint8]
        c.AlxParamItemEnumTest_SetValI.restype = i32
        c.AlxParamItemEnumTest_SetValI.argtypes = [vp, i64]
        c.AlxParamItemEnumTest_SetValF.restype = i32
        c.AlxParamItemEnumTest_SetValF.argtypes = [vp, f64]
        c.AlxParamItemEnumTest_GetValI.restype = i64
        c.AlxParamItemEnumTest_GetValI.argtypes = [vp]
        c.AlxParamItemEnumTest_GetValF.restype = f64
        c.AlxParamItemEnumTest_GetValF.argtypes = [vp]
        # the buffer shim: Arr and Str lend the item their storage, so the context owns it
        c.AlxParamItemBuffTest_NewArr.restype = vp
        c.AlxParamItemBuffTest_NewArr.argtypes = [cp, u32, cp, cp]
        c.AlxParamItemBuffTest_NewStr.restype = vp
        c.AlxParamItemBuffTest_NewStr.argtypes = [cp, u32, cp, cp]
        c.AlxParamItemBuffTest_Delete.argtypes = [vp]
        for name in ("GetBuffLen", "GetValLen", "GetDataType"):
            fn = getattr(c, f"AlxParamItemBuffTest_{name}")
            fn.restype, fn.argtypes = u32, [vp]
        for name in ("GetKey", "GetValUnit", "GetValDefStr"):
            fn = getattr(c, f"AlxParamItemBuffTest_{name}")
            fn.restype, fn.argtypes = cp, [vp]
        c.AlxParamItemBuffTest_GetValArr.argtypes = [vp, cp]
        c.AlxParamItemBuffTest_SetValArr.argtypes = [vp, cp]
        c.AlxParamItemBuffTest_GetValDefArr.argtypes = [vp, cp]
        c.AlxParamItemBuffTest_GetValStr.restype = i32
        c.AlxParamItemBuffTest_GetValStr.argtypes = [vp, cp, u32]
        c.AlxParamItemBuffTest_SetValStr.restype = i32
        c.AlxParamItemBuffTest_SetValStr.argtypes = [vp, cp]
        c.AlxParamItemBuffTest_SetValToDef.argtypes = [vp]
        # a store with MORE THAN ONE group - the 2x and 4x dispatch arms
        c.AlxStoreTest_New.restype = vp
        c.AlxStoreTest_New.argtypes = [u32]
        c.AlxStoreTest_Delete.argtypes = [vp]
        for name in ("MaxGroups", "ItemsPerGroup"):
            fn = getattr(c, f"AlxStoreTest_{name}")
            fn.restype, fn.argtypes = u32, []
        c.AlxStoreTest_GroupAddrA.restype = u32
        c.AlxStoreTest_GroupAddrA.argtypes = [u32]
        c.AlxStoreTest_Init.restype = i32
        c.AlxStoreTest_Init.argtypes = [vp]
        c.AlxStoreTest_Handle.argtypes = [vp, u32]
        c.AlxStoreTest_IsErr.restype = ctypes.c_bool
        c.AlxStoreTest_IsErr.argtypes = [vp]
        c.AlxStoreTest_ItemGet.restype = u32
        c.AlxStoreTest_ItemGet.argtypes = [vp, u32, u32]
        c.AlxStoreTest_ItemSet.restype = i32
        c.AlxStoreTest_ItemSet.argtypes = [vp, u32, u32, u32]
        # LoadVal / StoreVal, which need a store pointer - and the fake that backs it
        c.AlxParamItemKvTest_New.restype = vp
        c.AlxParamItemKvTest_New.argtypes = [u32, cp, i64]
        c.AlxParamItemKvTest_Delete.argtypes = [vp]
        for name in ("LoadVal", "StoreVal"):
            fn = getattr(c, f"AlxParamItemKvTest_{name}")
            fn.restype, fn.argtypes = i32, [vp]
        c.AlxParamItemKvTest_SetVal.restype = i32
        c.AlxParamItemKvTest_SetVal.argtypes = [vp, i64]
        c.AlxParamItemKvTest_GetVal.restype = i64
        c.AlxParamItemKvTest_GetVal.argtypes = [vp]
        c.AlxParamItemKvTest_GetKey.restype = cp
        c.AlxParamItemKvTest_GetKey.argtypes = [vp]
        c.AlxParamKvStoreFake_Reset.argtypes = []
        for name in ("Enable", "FailGet", "FailSet"):
            getattr(c, f"AlxParamKvStoreFake_{name}").argtypes = [ctypes.c_bool]
        for name in ("GetCount", "SetCount", "NumOfKeys"):
            fn = getattr(c, f"AlxParamKvStoreFake_{name}")
            fn.restype, fn.argtypes = u32, []
        c.AlxParamKvStoreFake_Peek.restype = ctypes.c_bool
        c.AlxParamKvStoreFake_Peek.argtypes = [cp, cp, ctypes.POINTER(u32)]
        c.AlxParamKvStoreFake_Poke.argtypes = [cp, cp, u32]
        c.AlxParamItemMetaTest_SetValToDef.argtypes = [vp]
        # alxRange and alxFtoa: pure functions, no context, called directly
        for name, ct in (("Uint8", ctypes.c_uint8), ("Uint16", ctypes.c_uint16),
                         ("Uint32", u32), ("Uint64", ctypes.c_uint64),
                         ("Int8", ctypes.c_int8), ("Int16", ctypes.c_int16),
                         ("Int32", i32), ("Int64", ctypes.c_int64),
                         ("Float", ctypes.c_float), ("Double", ctypes.c_double)):
            f = getattr(c, f"AlxRange_Check{name}")
            f.restype = i32
            f.argtypes = [ct, ct, ct]
        c.AlxRange_CheckStr.restype = i32
        c.AlxRange_CheckStr.argtypes = [ctypes.c_char_p, u32]
        c.AlxFtoa.restype = ctypes.c_char_p
        c.AlxFtoa.argtypes = [ctypes.c_double, ctypes.c_char_p, ctypes.c_int]

        def status(name: str) -> int:
            f = getattr(c, f"AlxMemSafeTest_Status_{name}")
            f.restype = i32
            return f()

        self.OK = status("Ok")
        self.ERR = status("Err")
        self.ERR_NUM_OF_TRIES = status("ErrNumOfTries")
        self.BOTH_ERR = status("BothCopyErr")
        self.BOTH_OK_SAME_USE_A = status("BothOkSame_UseA")
        self.BOTH_OK_DIFF_USE_A = status("BothOkDiff_UseA")
        self.A_OK_B_ERR_USE_A = status("AOkBErr_UseA")
        self.A_ERR_B_OK_USE_B = status("AErrBOk_UseB")
        self.ERR_ENUM = status("ErrEnum")
        self.ERR_CONV = status("ErrConv")
        self.COPY_LEN = c.AlxMemSafeTest_CopyLen()
        self.NUM_ITEMS = c.AlxMemSafeTest_NumOfItems()

    # -- pure helpers the parameter path leans on ----------------------------
    def range_check(self, kind: str, val, low, high) -> int:
        """AlxRange_Check<kind>(val, valMin, valMax) - the bound check itself."""
        return getattr(self.c, f"AlxRange_Check{kind}")(val, low, high)

    def range_check_str(self, text: str, max_len_with_null: int) -> int:
        return self.c.AlxRange_CheckStr(text.encode("ascii"), max_len_with_null)

    def ftoa(self, value: float, precision: int, size: int = 64) -> str:
        """AlxFtoa(f, buf, precision) - how a float reaches the CLI's JSON."""
        buf = ctypes.create_string_buffer(size)
        self.c.AlxFtoa(value, buf, precision)
        return buf.value.decode("ascii", "replace")

    # -- a standalone parameter item, for the string format conversion --------
    UINT8, UINT16, UINT32, INT8, INT16, INT32, FLOAT, BOOL = range(8)

    def item_new(self, item_type: int, val_def: float = 0.0, val_min: float = 0.0,
                 val_max: float = 0.0):
        return self.c.AlxParamItemStrTest_New(item_type, val_def, val_min, val_max)

    def item_delete(self, ctx) -> None:
        self.c.AlxParamItemStrTest_Delete(ctx)

    # -- the same item, seen through the metadata its constructor was given ----
    # AlxParamItem_DataType, in the library's own order (alxParamItem.h)
    (D_UINT8, D_UINT16, D_UINT32, D_UINT64, D_INT8, D_INT16, D_INT32, D_INT64,
     D_FLOAT, D_DOUBLE, D_BOOL, D_ARR, D_STR) = range(13)
    INTEGER_TYPES = (D_UINT8, D_UINT16, D_UINT32, D_UINT64, D_INT8, D_INT16, D_INT32, D_INT64)
    FLOAT_TYPES = (D_FLOAT, D_DOUBLE)
    # AlxParamItem_ValOutOfRangeHandle
    ASSERT, IGNORE, BOUND = 0, 1, 2
    # AlxParamItem_ParamType - the shim always builds a Param
    PARAM = 0

    def meta_new(self, data_type: int, key: str = "KEY", item_id: int = 0, group_key: str = "GRP",
                 group_id: int = 0, val_def: float = 0, val_min: float = 0, val_max: float = 0,
                 out_of_range: int = 1, unit: str = "", after_reset: bool = False):
        """Build one item of `data_type`. val_* travel down whichever channel that type reads."""
        is_float = data_type in self.FLOAT_TYPES
        ints = (0, 0, 0) if is_float else (int(val_def), int(val_min), int(val_max))
        flts = (float(val_def), float(val_min), float(val_max)) if is_float else (0.0, 0.0, 0.0)
        return self.c.AlxParamItemMetaTest_New(
            data_type, key.encode("ascii"), item_id, group_key.encode("ascii"), group_id,
            *ints, *flts, out_of_range, unit.encode("ascii"), after_reset)

    def meta_delete(self, ctx) -> None:
        self.c.AlxParamItemMetaTest_Delete(ctx)

    def meta(self, ctx, name: str):
        """Read one getter by its C name suffix, decoding the ones that answer with a string."""
        value = getattr(self.c, f"AlxParamItemMetaTest_{name}")(ctx)
        return value.decode("ascii") if isinstance(value, bytes) else value

    def meta_set_val(self, ctx, data_type: int, val: float) -> int:
        if data_type in self.FLOAT_TYPES:
            return self.c.AlxParamItemMetaTest_SetValF(ctx, float(val))
        return self.c.AlxParamItemMetaTest_SetValI(ctx, int(val))

    def meta_get_val(self, ctx, data_type: int):
        if data_type in self.FLOAT_TYPES:
            return self.c.AlxParamItemMetaTest_GetValF(ctx)
        return self.c.AlxParamItemMetaTest_GetValI(ctx)

    def meta_set_val_to_def(self, ctx) -> None:
        self.c.AlxParamItemMetaTest_SetValToDef(ctx)

    # -- an item that declares the LIST of values it will accept ---------------

    def enum_new(self, data_type: int, values, val_def: float = 0, val_min: float = 0,
                 val_max: float = 0, out_of_range: int = 1):
        """Build an item carrying `values` as its enum list, in the type's own representation."""
        is_float = data_type in self.FLOAT_TYPES
        n = len(values)
        list_i = (ctypes.c_int64 * max(n, 1))(*([0] * n if is_float else [int(v) for v in values]))
        list_f = (ctypes.c_double * max(n, 1))(*([float(v) for v in values] if is_float else [0.0] * n))
        ints = (0, 0, 0) if is_float else (int(val_def), int(val_min), int(val_max))
        flts = (float(val_def), float(val_min), float(val_max)) if is_float else (0.0, 0.0, 0.0)
        return self.c.AlxParamItemEnumTest_New(data_type, list_i, list_f, n, *ints, *flts,
                                               out_of_range)

    def enum_delete(self, ctx) -> None:
        self.c.AlxParamItemEnumTest_Delete(ctx)

    def enum_len(self, ctx) -> int:
        return self.c.AlxParamItemEnumTest_GetEnumArrLen(ctx)

    def enum_at(self, ctx, data_type: int, index: int):
        if data_type in self.FLOAT_TYPES:
            return self.c.AlxParamItemEnumTest_GetEnumArrAtF(ctx, index)
        return self.c.AlxParamItemEnumTest_GetEnumArrAtI(ctx, index)

    def enum_is_enum(self, ctx) -> bool:
        return self.c.AlxParamItemEnumTest_GetIsEnum(ctx)

    def enum_set_val(self, ctx, data_type: int, val: float) -> int:
        if data_type in self.FLOAT_TYPES:
            return self.c.AlxParamItemEnumTest_SetValF(ctx, float(val))
        return self.c.AlxParamItemEnumTest_SetValI(ctx, int(val))

    def enum_get_val(self, ctx, data_type: int):
        if data_type in self.FLOAT_TYPES:
            return self.c.AlxParamItemEnumTest_GetValF(ctx)
        return self.c.AlxParamItemEnumTest_GetValI(ctx)

    # -- the two types that carry a buffer -------------------------------------

    def buff_new_arr(self, val_def: bytes, key: str = "ARR", unit: str = ""):
        return self.c.AlxParamItemBuffTest_NewArr(val_def, len(val_def), key.encode("ascii"),
                                                  unit.encode("ascii"))

    def buff_new_str(self, val_def: str, buff_len: int, key: str = "STR", unit: str = ""):
        return self.c.AlxParamItemBuffTest_NewStr(val_def.encode("ascii"), buff_len,
                                                  key.encode("ascii"), unit.encode("ascii"))

    def buff_delete(self, ctx) -> None:
        self.c.AlxParamItemBuffTest_Delete(ctx)

    def buff(self, ctx, name: str):
        value = getattr(self.c, f"AlxParamItemBuffTest_{name}")(ctx)
        return value.decode("ascii") if isinstance(value, bytes) else value

    def buff_get_arr(self, ctx, length: int) -> bytes:
        out = ctypes.create_string_buffer(length)
        self.c.AlxParamItemBuffTest_GetValArr(ctx, out)
        return out.raw[:length]

    def buff_set_arr(self, ctx, val: bytes) -> None:
        self.c.AlxParamItemBuffTest_SetValArr(ctx, val)

    def buff_get_def_arr(self, ctx, length: int) -> bytes:
        out = ctypes.create_string_buffer(length)
        self.c.AlxParamItemBuffTest_GetValDefArr(ctx, out)
        return out.raw[:length]

    def buff_get_str(self, ctx, size: int = 64) -> tuple[int, str]:
        out = ctypes.create_string_buffer(size)
        status = self.c.AlxParamItemBuffTest_GetValStr(ctx, out, size)
        return status, out.value.decode("ascii", "replace")

    def buff_set_str(self, ctx, val: str) -> int:
        return self.c.AlxParamItemBuffTest_SetValStr(ctx, val.encode("ascii"))

    def buff_set_val_to_def(self, ctx) -> None:
        self.c.AlxParamItemBuffTest_SetValToDef(ctx)

    # -- a parameter store with more than one group ----------------------------

    def store_new(self, groups: int):
        return self.c.AlxStoreTest_New(groups)

    def store_delete(self, ctx) -> None:
        self.c.AlxStoreTest_Delete(ctx)

    def store_max_groups(self) -> int:
        return self.c.AlxStoreTest_MaxGroups()

    def store_items_per_group(self) -> int:
        return self.c.AlxStoreTest_ItemsPerGroup()

    def store_group_addr(self, group: int) -> int:
        return self.c.AlxStoreTest_GroupAddrA(group)

    def store_group_init(self, ctx) -> int:
        return self.c.AlxStoreTest_Init(ctx)

    def store_group_handle(self, ctx, times: int = 1) -> None:
        self.c.AlxStoreTest_Handle(ctx, times)

    def store_group_err(self, ctx) -> bool:
        return self.c.AlxStoreTest_IsErr(ctx)

    def store_item(self, ctx, group: int, index: int) -> int:
        return self.c.AlxStoreTest_ItemGet(ctx, group, index)

    def store_item_set(self, ctx, group: int, index: int, val: int) -> int:
        return self.c.AlxStoreTest_ItemSet(ctx, group, index, val)

    # -- the KV store an item loads from and stores to --------------------------

    def kv_reset(self) -> None:
        """Clear the fake AND disarm it - the disarmed fake is what every other group expects."""
        self.c.AlxParamKvStoreFake_Reset()

    def kv_enable(self, enable: bool = True) -> None:
        self.c.AlxParamKvStoreFake_Enable(enable)

    def kv_fail_get(self, fail: bool = True) -> None:
        self.c.AlxParamKvStoreFake_FailGet(fail)

    def kv_fail_set(self, fail: bool = True) -> None:
        self.c.AlxParamKvStoreFake_FailSet(fail)

    def kv_get_count(self) -> int:
        return self.c.AlxParamKvStoreFake_GetCount()

    def kv_set_count(self) -> int:
        return self.c.AlxParamKvStoreFake_SetCount()

    def kv_num_of_keys(self) -> int:
        return self.c.AlxParamKvStoreFake_NumOfKeys()

    def kv_peek(self, key: str, size: int = 64):
        """The bytes stored under `key`, or None if the key is not there."""
        out = ctypes.create_string_buffer(size)
        length = ctypes.c_uint32(0)
        found = self.c.AlxParamKvStoreFake_Peek(key.encode("ascii"), out, ctypes.byref(length))
        return out.raw[:length.value] if found else None

    def kv_poke(self, key: str, data: bytes) -> None:
        self.c.AlxParamKvStoreFake_Poke(key.encode("ascii"), data, len(data))

    def kv_item_new(self, data_type: int, key: str = "KV_KEY", val_def: int = 0):
        return self.c.AlxParamItemKvTest_New(data_type, key.encode("ascii"), val_def)

    def kv_item_delete(self, ctx) -> None:
        self.c.AlxParamItemKvTest_Delete(ctx)

    def kv_load(self, ctx) -> int:
        return self.c.AlxParamItemKvTest_LoadVal(ctx)

    def kv_store(self, ctx) -> int:
        return self.c.AlxParamItemKvTest_StoreVal(ctx)

    def kv_set(self, ctx, val: int) -> int:
        return self.c.AlxParamItemKvTest_SetVal(ctx, val)

    def kv_get(self, ctx) -> int:
        return self.c.AlxParamItemKvTest_GetVal(ctx)

    def item_set_str(self, ctx, val: str) -> int:
        return self.c.AlxParamItemStrTest_SetStr(ctx, val.encode("ascii"))

    def item_get_str(self, ctx, size: int = 64) -> tuple[int, str]:
        buf = ctypes.create_string_buffer(size)
        status = self.c.AlxParamItemStrTest_GetStr(ctx, buf, size)
        return status, buf.value.decode("ascii", "replace")

    def item_get_num(self, ctx) -> float:
        return self.c.AlxParamItemStrTest_GetNum(ctx)

    # -- chain under test -----------------------------------------------------
    def new(self, addr_a: int = 0x000, addr_b: int = 0x100, tries: int = 3, raw_tries: int = 3):
        return self.c.AlxMemSafeTest_New(addr_a, addr_b, tries, raw_tries)

    def delete(self, ctx):
        self.c.AlxMemSafeTest_Delete(ctx)

    @staticmethod
    def _buf(data: bytes):
        return (ctypes.c_uint8 * len(data))(*data)

    def read(self, ctx, n: int | None = None):
        """MemSafe read into a poison-filled buffer -> (status, bytes)."""
        n = self.COPY_LEN if n is None else n
        buf = (ctypes.c_uint8 * n)(*([self.POISON] * n))
        st = self.c.AlxMemSafeTest_MemSafeRead(ctx, buf, n)
        return st, bytes(buf)

    def write(self, ctx, data: bytes) -> int:
        return self.c.AlxMemSafeTest_MemSafeWrite(ctx, self._buf(data), len(data))

    def flags(self, ctx) -> dict:
        c = self.c
        return {
            "read_done": c.AlxMemSafeTest_MemSafeIsReadDone(ctx),
            "read_err": c.AlxMemSafeTest_MemSafeIsReadErr(ctx),
            "write_done": c.AlxMemSafeTest_MemSafeIsWriteDone(ctx),
            "write_err": c.AlxMemSafeTest_MemSafeIsWriteErr(ctx),
        }

    def group_init(self, ctx) -> int:
        return self.c.AlxMemSafeTest_GroupInit(ctx)

    def group_diff(self, ctx) -> bool:
        return self.c.AlxMemSafeTest_GroupIsValStoredBuffDiff(ctx)

    def store_init(self, ctx) -> int:
        return self.c.AlxMemSafeTest_StoreInit(ctx)

    def store_handle(self, ctx, passes: int = 1):
        for _ in range(passes):
            self.c.AlxMemSafeTest_StoreHandle(ctx)

    def store_err(self, ctx) -> bool:
        return self.c.AlxMemSafeTest_StoreIsErr(ctx)

    def items(self, ctx) -> list:
        return [self.c.AlxMemSafeTest_ItemGet(ctx, i) for i in range(self.NUM_ITEMS)]

    def defaults(self, ctx) -> list:
        return [self.c.AlxMemSafeTest_ItemGetDef(ctx, i) for i in range(self.NUM_ITEMS)]

    def item_set(self, ctx, index: int, val: int) -> int:
        return self.c.AlxMemSafeTest_ItemSet(ctx, index, val)

    # -- CRC ------------------------------------------------------------------
    def crc_calc(self, cfg: int, data: bytes) -> int:
        return self.c.AlxMemSafeTest_CrcCalc(cfg, self._buf(data), len(data))

    def crc_is_ok(self, cfg: int, blob: bytes):
        v = ctypes.c_uint32(0xDEADBEEF)
        ok = self.c.AlxMemSafeTest_CrcIsOk(cfg, self._buf(blob), len(blob), ctypes.byref(v))
        return ok, v.value

    def crc_len(self, cfg: int) -> int:
        return self.c.AlxMemSafeTest_CrcLen(cfg)

    # -- fake flash -------------------------------------------------------------
    def fake_reset(self):
        self.c.AlxMemRawFake_Reset()

    def fake_fill(self, val: int):
        self.c.AlxMemRawFake_Fill(val)

    def peek(self, addr: int, n: int) -> bytes:
        buf = (ctypes.c_uint8 * n)()
        self.c.AlxMemRawFake_Peek(addr, buf, n)
        return bytes(buf)

    def poke(self, addr: int, data: bytes):
        self.c.AlxMemRawFake_Poke(addr, self._buf(data), len(data))

    def fail_at(self, kind: int, nth: int):
        self.c.AlxMemRawFake_FailAt(kind, nth)

    def count(self, kind: int) -> int:
        return self.c.AlxMemRawFake_Count(kind)

    def power_loss_at(self, nth_write: int, bytes_that_land: int):
        self.c.AlxMemRawFake_PowerLossAt(nth_write, bytes_that_land)

    def power_on(self):
        self.c.AlxMemRawFake_PowerOn()

    def power_lost(self) -> bool:
        return self.c.AlxMemRawFake_IsPowerLost()

    def row_erase_model(self, enable: bool, row_size: int = 256):
        self.c.AlxMemRawFake_SetRowEraseModel(enable, row_size)

    def last_write(self):
        return self.c.AlxMemRawFake_LastWriteAddr(), self.c.AlxMemRawFake_LastWriteLen()

    def args_mismatch(self) -> int:
        return self.c.AlxMemRawFake_ArgsMismatchCount()

    def not_init_calls(self) -> int:
        return self.c.AlxMemRawFake_NotInitCallCount()

    def last_raw_args(self):
        return self.c.AlxMemRawFake_LastNumOfTries(), self.c.AlxMemRawFake_LastTimeout_ms()


# --------------------------------------------------------------- fixtures ----


def check(lib, result, exp_status, exp_content: bytes, ln: int):
    """Assert the full contract of one ReadStrUntil* call:
    status, content, lenActual, null-termination, poison beyond NUL intact."""
    status, content, la, raw = result
    assert status == exp_status, f"status {status} != {exp_status} (raw={raw!r})"
    assert la == len(exp_content), f"lenActual {la} != {len(exp_content)}"
    assert content == exp_content, f"content {content!r} != {exp_content!r}"
    nul_pos = len(exp_content)
    assert raw[nul_pos] == 0, f"str not null-terminated at {nul_pos} (raw={raw!r})"
    poison = raw[nul_pos + 1:]
    assert all(b == lib.POISON for b in poison), f"bytes beyond NUL modified: {raw!r}"


class VdivLib:
    """ctypes wrapper around alxVdivTest.dll: a resistive divider's arithmetic.

    Two families with the same algebra in different units - float volts and kOhm, and integer
    millivolts and ohm - plus the current a shunt drops. Every function is context free.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        f, u32 = ctypes.c_float, ctypes.c_uint32
        for name in ("GetVout_V", "GetVin_V", "GetResHigh_kOhm", "GetResLow_kOhm"):
            fn = getattr(c, f"AlxVdiv_{name}")
            fn.restype = f
            fn.argtypes = [f, f, f]
        for name in ("GetVout_mV", "GetVin_mV", "GetResHigh_ohm", "GetResLow_ohm"):
            fn = getattr(c, f"AlxVdiv_{name}")
            fn.restype = u32
            fn.argtypes = [u32, u32, u32]
        c.AlxVdiv_GetCurrent_uA.restype = u32
        c.AlxVdiv_GetCurrent_uA.argtypes = [u32, u32]
        c.AlxVdiv_GetCurrent_mA.restype = f
        c.AlxVdiv_GetCurrent_mA.argtypes = [f, f]

    def vout_v(self, vin_v, res_high_kohm, res_low_kohm) -> float:
        return self.c.AlxVdiv_GetVout_V(vin_v, res_high_kohm, res_low_kohm)

    def current_ma(self, vout_v, res_low_kohm) -> float:
        return self.c.AlxVdiv_GetCurrent_mA(vout_v, res_low_kohm)

    def vin_v(self, vout_v, res_high_kohm, res_low_kohm) -> float:
        return self.c.AlxVdiv_GetVin_V(vout_v, res_high_kohm, res_low_kohm)

    def res_high_kohm(self, vin_v, vout_v, res_low_kohm) -> float:
        return self.c.AlxVdiv_GetResHigh_kOhm(vin_v, vout_v, res_low_kohm)

    def res_low_kohm(self, vin_v, vout_v, res_high_kohm) -> float:
        return self.c.AlxVdiv_GetResLow_kOhm(vin_v, vout_v, res_high_kohm)

    def vout_mv(self, vin_mv, res_high_ohm, res_low_ohm) -> int:
        return self.c.AlxVdiv_GetVout_mV(vin_mv, res_high_ohm, res_low_ohm)

    def vin_mv(self, vout_mv, res_high_ohm, res_low_ohm) -> int:
        return self.c.AlxVdiv_GetVin_mV(vout_mv, res_high_ohm, res_low_ohm)

    def res_high_ohm(self, vin_mv, vout_mv, res_low_ohm) -> int:
        return self.c.AlxVdiv_GetResHigh_ohm(vin_mv, vout_mv, res_low_ohm)

    def res_low_ohm(self, vin_mv, vout_mv, res_high_ohm) -> int:
        return self.c.AlxVdiv_GetResLow_ohm(vin_mv, vout_mv, res_high_ohm)

    def current_ua(self, vout_uv, res_low_ohm) -> int:
        return self.c.AlxVdiv_GetCurrent_uA(vout_uv, res_low_ohm)


class TimSwLib:
    """ctypes wrapper around alxTimSwTest.dll: the software timer over a clock the test owns.

    Time only moves when advance_ns is called, so every timeout is exact and nothing is flaky.
    """

    TIMSW_SIZE = 32          # sizeof(AlxTimSw): uint64 + two bools, generously padded

    TICK_UNITS = ("ns", "us", "ms", "sec", "min", "hr")

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u64, b, u32, i32 = (ctypes.c_void_p, ctypes.c_uint64, ctypes.c_bool,
                                ctypes.c_uint32, ctypes.c_int32)
        for name in ("Ctor", "Start", "Stop"):
            getattr(c, f"AlxTimSw_{name}").argtypes = [vp]
        c.AlxTimSw_IsRunning.restype = b
        c.AlxTimSw_IsRunning.argtypes = [vp]
        for unit in ("ns", "us", "ms", "sec", "min", "hr"):
            get = getattr(c, f"AlxTimSw_Get_{unit}")
            get.restype = u64
            get.argtypes = [vp]
            timeout = getattr(c, f"AlxTimSw_IsTimeout_{unit}")
            timeout.restype = b
            timeout.argtypes = [vp, u64]
        c.AlxTick_Ctor.argtypes = [vp]
        for unit in self.TICK_UNITS:
            getter = getattr(c, f"AlxTick_Get_{unit}")
            getter.restype = u64
            getter.argtypes = [vp]
            getattr(c, f"AlxTick_Inc_{unit}").argtypes = [vp]
            getattr(c, f"AlxTick_IncRange_{unit}").argtypes = [vp, u64]
        # the library's own global tick instance, reached as data rather than rebuilt here
        self.tick = ctypes.addressof(ctypes.c_uint8.in_dll(c, "alxTick"))
        for name in ("LockCount", "UnlockCount"):
            getattr(c, f"AlxIrqFake_{name}").restype = u32
        for name in ("Depth", "DepthMax"):
            getattr(c, f"AlxIrqFake_{name}").restype = i32

    # -- the clock the test owns ----------------------------------------------
    def tick_reset(self) -> None:
        self.c.AlxTick_Ctor(self.tick)
        self.c.AlxIrqFake_Reset()

    def advance_ns(self, ns: int) -> None:
        self.c.AlxTick_IncRange_ns(self.tick, ns)

    def advance_ms(self, ms: float) -> None:
        # round, not truncate: 1.001 ms is 1000999.9999 ns in binary floating point, and a filter
        # boundary approached one nanosecond short of the intended time is a test that lies
        self.advance_ns(round(ms * 1_000_000))

    def now_ns(self) -> int:
        return self.c.AlxTick_Get_ns(self.tick)

    def now(self, unit: str) -> int:
        """The clock in whichever of the six units the module offers."""
        return getattr(self.c, f"AlxTick_Get_{unit}")(self.tick)

    def tick_inc(self, unit: str) -> None:
        """One step of one unit - what an interrupt does on the target."""
        getattr(self.c, f"AlxTick_Inc_{unit}")(self.tick)

    def tick_inc_range(self, unit: str, count: int) -> None:
        """Many steps at once, for a loop that was away longer than one."""
        getattr(self.c, f"AlxTick_IncRange_{unit}")(self.tick, count)

    # -- one timer ------------------------------------------------------------
    def timer(self):
        buf = ctypes.create_string_buffer(self.TIMSW_SIZE)
        self.c.AlxTimSw_Ctor(buf)
        return buf

    def start(self, tim) -> None:
        self.c.AlxTimSw_Start(tim)

    def stop(self, tim) -> None:
        self.c.AlxTimSw_Stop(tim)

    def is_running(self, tim) -> bool:
        return self.c.AlxTimSw_IsRunning(tim)

    def get(self, tim, unit: str) -> int:
        return getattr(self.c, f"AlxTimSw_Get_{unit}")(tim)

    def is_timeout(self, tim, unit: str, value: int) -> bool:
        return getattr(self.c, f"AlxTimSw_IsTimeout_{unit}")(tim, value)

    # -- what the fake saw ----------------------------------------------------
    def lock_count(self) -> int:
        return self.c.AlxIrqFake_LockCount()

    def unlock_count(self) -> int:
        return self.c.AlxIrqFake_UnlockCount()

    def lock_depth_max(self) -> int:
        return self.c.AlxIrqFake_DepthMax()


class CanMsg(ctypes.Structure):
    """AlxCan_Msg: identifier, two flags, a length and the eight data bytes the codec works on."""

    _fields_ = (
        ("id", ctypes.c_uint32),
        ("isExtendedId", ctypes.c_bool),
        ("isDataFrame", ctypes.c_bool),
        ("dataLen", ctypes.c_uint8),
        ("data", ctypes.c_uint8 * 8),
    )


class CanParserLib:
    """ctypes wrapper around alxCanParserTest.dll: the CAN payload codec.

    Twelve setters and twelve getters over one 8-byte buffer, no state and no peripheral. The
    endian argument is the module's own enum, Big = 0 and Little = 1, and it names where the MOST
    significant byte goes, not the host's byte order.

    Built with the module's asserts DISABLED, which is the default configuration and the one every
    product ships unless it opts in. The asserts guard byteOffset only, and on the PC a failed
    assert calls abort(), which would take the whole suite down with no report; the same guarantee
    is proved here instead by writing at every LEGAL offset and checking that nothing outside the
    payload moved (see test_alxCanParser.py P156).
    """

    BIG = 0
    LITTLE = 1

    # name -> (ctypes type, does the function take an endian argument)
    SCALARS: ClassVar[dict[str, tuple[type, bool]]] = {
        "Uint8": (ctypes.c_uint8, False),
        "Int8": (ctypes.c_int8, False),
        "Uint16": (ctypes.c_uint16, True),
        "Int16": (ctypes.c_int16, True),
        "Uint32": (ctypes.c_uint32, True),
        "Int32": (ctypes.c_int32, True),
        "Uint64": (ctypes.c_uint64, True),
        "Int64": (ctypes.c_int64, True),
        "Float": (ctypes.c_float, True),
        "Double": (ctypes.c_double, True),
    }

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        msg_p, u8, endian_t = ctypes.POINTER(CanMsg), ctypes.c_uint8, ctypes.c_int
        for name, (ctype, has_endian) in self.SCALARS.items():
            head = [msg_p, endian_t, u8] if has_endian else [msg_p, u8]
            setter = getattr(c, f"AlxCanParser_Set{name}")
            setter.restype = None
            setter.argtypes = [*head, ctype]
            getter = getattr(c, f"AlxCanParser_Get{name}")
            getter.restype = ctype
            getter.argtypes = head
        c.AlxCanParser_SetBit.restype = None
        c.AlxCanParser_SetBit.argtypes = [msg_p, u8, u8, ctypes.c_bool]
        c.AlxCanParser_GetBit.restype = ctypes.c_bool
        c.AlxCanParser_GetBit.argtypes = [msg_p, u8, u8]
        c.AlxCanParser_SetEnum.restype = None
        c.AlxCanParser_SetEnum.argtypes = [msg_p, u8, u8, u8, u8]
        c.AlxCanParser_GetEnum.restype = u8
        c.AlxCanParser_GetEnum.argtypes = [msg_p, u8, u8, u8]

    @staticmethod
    def msg(payload: bytes = b"") -> CanMsg:
        """A frame whose data bytes start as ``payload``, zero filled to eight."""
        frame = CanMsg()
        for i, byte in enumerate(payload):
            frame.data[i] = byte
        return frame

    @staticmethod
    def payload(frame: CanMsg) -> bytes:
        """The eight data bytes - what would go on the wire."""
        return bytes(frame.data)

    def set(self, name: str, frame: CanMsg, *args) -> None:
        """Call ``AlxCanParser_Set<name>`` on ``frame``."""
        getattr(self.c, f"AlxCanParser_Set{name}")(ctypes.byref(frame), *args)

    def get(self, name: str, frame: CanMsg, *args):
        """Call ``AlxCanParser_Get<name>`` on ``frame`` and return what it read."""
        return getattr(self.c, f"AlxCanParser_Get{name}")(ctypes.byref(frame), *args)


class FiltGlitchLib:
    """ctypes wrapper around alxFiltGlitchTest.dll: both glitch filters over a clock the test owns.

    The filters ask the software timer how long the input has been unstable, and the timer asks the
    tick counter. The tick moves only when ``advance_ms`` is called, so a debounce boundary can be
    approached from both sides exactly - which is the only way to tell ``>`` from ``>=``, and the two
    filters do not agree on that (see test_alxFiltGlitch.py P163).
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u32, u64, b, f = (ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint64,
                              ctypes.c_bool, ctypes.c_float)
        c.AlxFiltGlitchBoolTest_New.restype = vp
        c.AlxFiltGlitchBoolTest_New.argtypes = [b, f, f]
        c.AlxFiltGlitchBoolTest_New_us.restype = vp
        c.AlxFiltGlitchBoolTest_New_us.argtypes = [b, u64, u64]
        c.AlxFiltGlitchBoolTest_Delete.argtypes = [vp]
        c.AlxFiltGlitchBool_Process.restype = b
        c.AlxFiltGlitchBool_Process.argtypes = [vp, b]
        c.AlxFiltGlitchBool_Reset.argtypes = [vp]
        c.AlxFiltGlitchUint32Test_New.restype = vp
        c.AlxFiltGlitchUint32Test_New.argtypes = [u32, f]
        c.AlxFiltGlitchUint32Test_Delete.argtypes = [vp]
        c.AlxFiltGlitchUint32_Process.restype = u32
        c.AlxFiltGlitchUint32_Process.argtypes = [vp, u32]
        c.AlxTick_Ctor.argtypes = [vp]
        c.AlxTick_Get_ns.restype = u64
        c.AlxTick_Get_ns.argtypes = [vp]
        c.AlxTick_IncRange_ns.argtypes = [vp, u64]
        self.tick = ctypes.addressof(ctypes.c_uint8.in_dll(c, "alxTick"))
        self._handles: list = []

    # -- the clock the test owns ----------------------------------------------
    def tick_reset(self) -> None:
        self.c.AlxTick_Ctor(self.tick)

    def advance_ms(self, ms: float) -> None:
        self.c.AlxTick_IncRange_ns(self.tick, round(ms * 1_000_000))

    def now_ns(self) -> int:
        return self.c.AlxTick_Get_ns(self.tick)

    # -- the filters ----------------------------------------------------------
    def boolean(self, initial: bool, true_ms: float, false_ms: float):
        """A boolean filter with separate rise and fall times, in milliseconds."""
        return self._keep(self.c.AlxFiltGlitchBoolTest_New(initial, true_ms, false_ms), "Bool")

    def boolean_us(self, initial: bool, true_us: int, false_us: int):
        """The same filter through its microsecond constructor."""
        return self._keep(self.c.AlxFiltGlitchBoolTest_New_us(initial, true_us, false_us), "Bool")

    def uint32(self, initial: int, stable_ms: float):
        """A uint32 filter: one stable time for any change of value."""
        return self._keep(self.c.AlxFiltGlitchUint32Test_New(initial, stable_ms), "Uint32")

    def process(self, filt, value):
        """Feed one sample. Booleans go to the boolean filter, integers to the uint32 one."""
        if isinstance(value, bool):
            return self.c.AlxFiltGlitchBool_Process(filt, value)
        return self.c.AlxFiltGlitchUint32_Process(filt, value)

    def reset(self, filt) -> None:
        self.c.AlxFiltGlitchBool_Reset(filt)

    def _keep(self, handle, kind: str):
        assert handle, "the test helper could not allocate a filter"
        self._handles.append((handle, kind))
        return handle

    def free_all(self) -> None:
        """Release every filter this test was handed, through the destructor of its own type."""
        for handle, kind in self._handles:
            getattr(self.c, f"AlxFiltGlitch{kind}Test_Delete")(handle)
        self._handles.clear()


class AlxMathData(ctypes.Structure):
    """AlxMath_Data: what the running statistics module reports, returned by value."""

    _fields_ = (
        ("count", ctypes.c_uint32),
        ("sum", ctypes.c_uint64),
        ("avg", ctypes.c_uint32),
        ("min", ctypes.c_uint32),
        ("max", ctypes.c_uint32),
    )


class MathLib:
    """ctypes wrapper around alxMathTest.dll: the library's pure signal conditioning.

    Two hysteresis state machines, a moving average over a caller-owned buffer, and a running
    min/max/mean. Nothing here has a clock or a peripheral, so every test is a sequence of samples
    and an expected answer.
    """

    HYS1_HIGH, HYS1_LOW = 0, 1              # AlxHys1_St
    HYS2_TOP, HYS2_MID, HYS2_BOT = 0, 1, 2  # AlxHys2_St

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u32, f, i32 = ctypes.c_void_p, ctypes.c_uint32, ctypes.c_float, ctypes.c_int32
        c.AlxHys1Test_New.restype = vp
        c.AlxHys1Test_New.argtypes = [f, f]
        c.AlxHys2Test_New.restype = vp
        c.AlxHys2Test_New.argtypes = [f, f, f, f]
        c.AlxAvgTest_New.restype = vp
        c.AlxAvgTest_New.argtypes = [u32, u32, f]
        c.AlxMathTest_New.restype = vp
        for name in ("AlxHys1Test_Delete", "AlxHys2Test_Delete", "AlxAvgTest_Delete",
                     "AlxMathTest_Delete"):
            getattr(c, name).argtypes = [vp]
        c.AlxHys1_Process.restype = i32
        c.AlxHys1_Process.argtypes = [vp, f]
        c.AlxHys2_Process.restype = i32
        c.AlxHys2_Process.argtypes = [vp, f]
        c.AlxAvg_Process.restype = f
        c.AlxAvg_Process.argtypes = [vp, f]
        c.AlxMath_Process.restype = AlxMathData
        c.AlxMath_Process.argtypes = [vp, u32]
        c.AlxGlobal_Ulltoa.argtypes = [ctypes.c_uint64, ctypes.c_char_p]
        c.AlxGlobal_Slltoa.argtypes = [ctypes.c_int64, ctypes.c_char_p]
        c.AlxGlobal_Ntohl.restype = u32
        c.AlxGlobal_Ntohl.argtypes = [u32]
        self._handles: list = []

    # -- the four modules -----------------------------------------------------
    def hys1(self, high: float, low: float):
        """One threshold pair: high to enter the high state, low to leave it."""
        return self._keep(self.c.AlxHys1Test_New(high, low), "AlxHys1Test_Delete")

    def hys2(self, top_high: float, top_low: float, bot_high: float, bot_low: float):
        """Two threshold pairs and three states, top / middle / bottom."""
        return self._keep(self.c.AlxHys2Test_New(top_high, top_low, bot_high, bot_low),
                          "AlxHys2Test_Delete")

    def avg(self, buff_len: int, shift_threshold: int, fill: float = 0.0):
        """A moving average over ``buff_len`` samples, recomputed every ``shift_threshold`` of them.

        ``fill`` is what the caller's buffer holds before the first sample. The library's own
        constructor never touches that buffer, which is the point of being able to choose it here.
        """
        return self._keep(self.c.AlxAvgTest_New(buff_len, shift_threshold, fill),
                          "AlxAvgTest_Delete")

    def math(self):
        """A running count, sum, mean, minimum and maximum over uint32 samples."""
        return self._keep(self.c.AlxMathTest_New(), "AlxMathTest_Delete")

    def hys1_process(self, obj, value: float) -> int:
        return self.c.AlxHys1_Process(obj, value)

    def hys2_process(self, obj, value: float) -> int:
        return self.c.AlxHys2_Process(obj, value)

    def avg_process(self, obj, value: float) -> float:
        return self.c.AlxAvg_Process(obj, value)

    def math_process(self, obj, value: int) -> AlxMathData:
        return self.c.AlxMath_Process(obj, value)

    # -- the number formatting in alxGlobal -----------------------------------
    def ulltoa(self, value: int) -> str:
        """A uint64 as decimal text. The buffer is 64 bytes; the module writes at most 20 digits."""
        buff = ctypes.create_string_buffer(64)
        self.c.AlxGlobal_Ulltoa(value, buff)
        return buff.value.decode("ascii")

    def slltoa(self, value: int) -> str:
        """An int64 as decimal text, sign included."""
        buff = ctypes.create_string_buffer(64)
        self.c.AlxGlobal_Slltoa(value, buff)
        return buff.value.decode("ascii")

    def ntohl(self, value: int) -> int:
        """A uint32 with its four bytes reversed."""
        return self.c.AlxGlobal_Ntohl(value)

    def _keep(self, handle, deleter: str):
        assert handle, "the test helper could not allocate"
        self._handles.append((handle, deleter))
        return handle

    def free_all(self) -> None:
        """Release everything this test was handed, through the destructor of its own type."""
        for handle, deleter in self._handles:
            getattr(self.c, deleter)(handle)
        self._handles.clear()


class LinFunLib:
    """ctypes wrapper around alxLinFunTest.dll: the library's two x-to-y mappings.

    A straight line through two points, in float and in integer, and a lookup table interpolated
    between its points. Both clip, and both report which end they clipped at through Alx_Status.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, i32, u32, b = (ctypes.c_void_p, ctypes.c_float, ctypes.c_int32,
                              ctypes.c_uint32, ctypes.c_bool)
        c.AlxLinFunTest_New.restype = vp
        c.AlxLinFunTest_New.argtypes = [f, f, f, f, f, f, b]
        c.AlxLinFunIntTest_New.restype = vp
        c.AlxLinFunIntTest_New.argtypes = [i32, i32, i32, i32, i32, i32, b]
        c.AlxInterpLinTest_New.restype = vp
        c.AlxInterpLinTest_New.argtypes = [ctypes.POINTER(f), ctypes.POINTER(f), u32, b]
        for name in ("AlxLinFunTest_Delete", "AlxLinFunIntTest_Delete", "AlxInterpLinTest_Delete"):
            getattr(c, name).argtypes = [vp]
        for name in ("AlxLinFun_GetY", "AlxLinFun_GetX", "AlxInterpLin_GetY"):
            fn = getattr(c, name)
            fn.restype = f
            fn.argtypes = [vp, f]
        c.AlxLinFunInt_GetY.restype = i32
        c.AlxLinFunInt_GetY.argtypes = [vp, i32]
        for name in ("AlxLinFun_GetY_WithStatus", "AlxLinFun_GetX_WithStatus",
                     "AlxInterpLin_GetY_WithStatus"):
            fn = getattr(c, name)
            fn.restype = i32
            fn.argtypes = [vp, f, ctypes.POINTER(f)]
        c.AlxLinFunInt_GetY_WithStatus.restype = i32
        c.AlxLinFunInt_GetY_WithStatus.argtypes = [vp, i32, ctypes.POINTER(i32)]
        for name in ("Ok", "ErrMin", "ErrMax"):
            fn = getattr(c, f"AlxLinFunTest_Status_{name}")
            fn.restype = i32
        self.OK = c.AlxLinFunTest_Status_Ok()
        self.ERR_MIN = c.AlxLinFunTest_Status_ErrMin()
        self.ERR_MAX = c.AlxLinFunTest_Status_ErrMax()
        self._handles: list = []

    # -- the three mappings ---------------------------------------------------
    def line(self, x1, y1, x2, y2, minimum, maximum, limit_on_x=False):
        """A float line through (x1, y1) and (x2, y2), clipped on x or on y."""
        return self._keep(self.c.AlxLinFunTest_New(x1, y1, x2, y2, minimum, maximum, limit_on_x),
                          "AlxLinFunTest_Delete")

    def line_int(self, x1, y1, x2, y2, minimum, maximum, limit_on_x=False):
        """The same line in integers, which is a separate implementation, not a wrapper."""
        return self._keep(self.c.AlxLinFunIntTest_New(x1, y1, x2, y2, minimum, maximum, limit_on_x),
                          "AlxLinFunIntTest_Delete")

    def table(self, x_points, y_points, rising=True):
        """A lookup table; the helper copies the points, so the caller keeps nothing alive."""
        count = len(x_points)
        assert count == len(y_points), "a table needs one y for every x"
        arr = ctypes.c_float * count
        return self._keep(
            self.c.AlxInterpLinTest_New(arr(*x_points), arr(*y_points), count, rising),
            "AlxInterpLinTest_Delete")

    # -- evaluation -----------------------------------------------------------
    def y(self, obj, x: float) -> float:
        return self.c.AlxLinFun_GetY(obj, x)

    def x(self, obj, y: float) -> float:
        return self.c.AlxLinFun_GetX(obj, y)

    def y_int(self, obj, x: int) -> int:
        return self.c.AlxLinFunInt_GetY(obj, x)

    def table_y(self, obj, x: float) -> float:
        return self.c.AlxInterpLin_GetY(obj, x)

    def y_status(self, obj, x: float) -> tuple[int, float]:
        out = ctypes.c_float()
        return self.c.AlxLinFun_GetY_WithStatus(obj, x, ctypes.byref(out)), out.value

    def x_status(self, obj, y: float) -> tuple[int, float]:
        out = ctypes.c_float()
        return self.c.AlxLinFun_GetX_WithStatus(obj, y, ctypes.byref(out)), out.value

    def y_int_status(self, obj, x: int) -> tuple[int, int]:
        out = ctypes.c_int32()
        return self.c.AlxLinFunInt_GetY_WithStatus(obj, x, ctypes.byref(out)), out.value

    def table_y_status(self, obj, x: float) -> tuple[int, float]:
        out = ctypes.c_float()
        return self.c.AlxInterpLin_GetY_WithStatus(obj, x, ctypes.byref(out)), out.value

    def _keep(self, handle, deleter: str):
        assert handle, "the test helper could not allocate"
        self._handles.append((handle, deleter))
        return handle

    def free_all(self) -> None:
        """Release everything this test was handed, through the destructor of its own type."""
        for handle, deleter in self._handles:
            getattr(self.c, deleter)(handle)
        self._handles.clear()


class BoolLib:
    """ctypes wrapper around alxBoolTest.dll: one boolean input and everything the library says about it.

    ``update(value)`` feeds a sample; ``state()`` returns every query the module answers, as a dict,
    so a test can assert the whole picture at once rather than call fourteen functions by hand. The
    clock is the test's: ``advance_ms`` is the only thing that moves time.
    """

    QUERIES = ("IsTrue", "IsTrueUpToShortTime", "IsTrueUpToLongTime", "IsTrueForLongTime",
               "WasTrue", "WasTrueForShortTime", "WasTrueForLongTime",
               "IsFalse", "IsFalseUpToShortTime", "IsFalseUpToLongTime", "IsFalseForLongTime",
               "WasFalse", "WasFalseForShortTime", "WasFalseForLongTime")
    CLEARS = ("ClearWasTrueFlag", "ClearWasTrueForShortTimeFlag", "ClearWasTrueForLongTimeFlag",
              "ClearWasFalseFlag", "ClearWasFalseForShortTimeFlag", "ClearWasFalseForLongTimeFlag")

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, b, u64 = ctypes.c_void_p, ctypes.c_float, ctypes.c_bool, ctypes.c_uint64
        c.AlxBoolTest_New.restype = vp
        c.AlxBoolTest_New.argtypes = [b, f, f, f, f, f, f]
        c.AlxBoolTest_Delete.argtypes = [vp]
        c.AlxBool_Update.argtypes = [vp, b]
        for name in self.QUERIES:
            fn = getattr(c, f"AlxBool_{name}")
            fn.restype = b
            fn.argtypes = [vp]
        for name in self.CLEARS:
            getattr(c, f"AlxBool_{name}").argtypes = [vp]
        c.AlxTick_Ctor.argtypes = [vp]
        c.AlxTick_Get_ns.restype = u64
        c.AlxTick_Get_ns.argtypes = [vp]
        c.AlxTick_IncRange_ns.argtypes = [vp, u64]
        self.tick = ctypes.addressof(ctypes.c_uint8.in_dll(c, "alxTick"))
        self._handles: list = []

    # -- the clock the test owns ----------------------------------------------
    def tick_reset(self) -> None:
        self.c.AlxTick_Ctor(self.tick)

    def advance_ms(self, ms: float) -> None:
        self.c.AlxTick_IncRange_ns(self.tick, round(ms * 1_000_000))

    # -- one boolean ----------------------------------------------------------
    def make(self, initial: bool, true_short_ms: float, true_long_ms: float,
             false_short_ms: float, false_long_ms: float,
             stable_true_ms: float = 0.0, stable_false_ms: float = 0.0):
        """A boolean with a short and a long threshold on each side, over a glitch filter."""
        handle = self.c.AlxBoolTest_New(initial, true_short_ms, true_long_ms,
                                        false_short_ms, false_long_ms,
                                        stable_true_ms, stable_false_ms)
        assert handle, "the test helper could not allocate"
        self._handles.append(handle)
        return handle

    def update(self, obj, value: bool) -> None:
        self.c.AlxBool_Update(obj, value)

    def query(self, obj, name: str) -> bool:
        return getattr(self.c, f"AlxBool_{name}")(obj)

    def clear(self, obj, name: str) -> None:
        getattr(self.c, f"AlxBool_{name}")(obj)

    def state(self, obj) -> dict:
        """Every query the module answers, in one dict - what a test compares against."""
        return {name: self.query(obj, name) for name in self.QUERIES}

    def true_flags(self, obj) -> set:
        """The names of the true-side queries that answer yes; the readable half of state()."""
        return {name for name, value in self.state(obj).items() if value and "True" in name}

    def false_flags(self, obj) -> set:
        return {name for name, value in self.state(obj).items() if value and "False" in name}

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxBoolTest_Delete(handle)
        self._handles.clear()


class AlxDateTime(ctypes.Structure):
    """AlxRtc_DateTime: a broken-down date, with the year counted from 2000 and the day of week 1-7."""

    _fields_ = (
        ("yr", ctypes.c_uint8),
        ("mo", ctypes.c_uint8),
        ("day", ctypes.c_uint8),
        ("weekDay", ctypes.c_uint8),
        ("hr", ctypes.c_uint8),
        ("min", ctypes.c_uint8),
        ("sec", ctypes.c_uint8),
        ("ms", ctypes.c_uint16),
        ("us", ctypes.c_uint16),
        ("ns", ctypes.c_uint16),
    )

    def as_tuple(self) -> tuple:
        """Year (full), month, day, hour, minute, second - what a calendar library would give."""
        return (2000 + self.yr, self.mo, self.day, self.hr, self.min, self.sec)


class RtcLib:
    """ctypes wrapper around alxRtcTest.dll: Unix time to a calendar date and back.

    Every function is free - no object, no clock - so the wrapper is a thin naming layer. The
    conversions are the one thing in the library whose right answer can be computed independently,
    which is what test_alxRtc_Global.py does with Python's own datetime.
    """

    RESOLUTIONS: ClassVar[dict[str, int]] = {
        "Sec": 1, "Ms": 1_000, "Us": 1_000_000, "Ns": 1_000_000_000,
    }

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        u16, u64, f = ctypes.c_uint16, ctypes.c_uint64, ctypes.c_float
        p16 = ctypes.POINTER(u16)
        for unit in self.RESOLUTIONS:
            to_date = getattr(c, f"AlxRtc_UnixTime{unit}ToDateTime")
            to_date.restype = AlxDateTime
            to_date.argtypes = [u64]
            from_date = getattr(c, f"AlxRtc_DateTimeToUnixTime{unit}")
            from_date.restype = u64
            from_date.argtypes = [AlxDateTime]
        c.AlxRtc_MsUsNsToNs.restype = u64
        c.AlxRtc_MsUsNsToNs.argtypes = [u64, u16, u16, u16]
        c.AlxRtc_MsUsToNs.restype = u64
        c.AlxRtc_MsUsToNs.argtypes = [u64, u16, u16]
        c.AlxRtc_MsToNs.restype = u64
        c.AlxRtc_MsToNs.argtypes = [u64, u16]
        c.AlxRtc_MsUsNsToSecFract.restype = f
        c.AlxRtc_MsUsNsToSecFract.argtypes = [u16, u16, u16]
        c.AlxRtc_NsToMsUsNs.argtypes = [u64, u64, p16, p16, p16]
        c.AlxRtc_NsToMsUs.argtypes = [u64, u64, p16, p16]
        c.AlxRtc_NsToMs.argtypes = [u64, u64, p16]
        c.AlxRtc_SecFractToMsUsNs.argtypes = [f, p16, p16, p16]

    # -- the calendar ---------------------------------------------------------
    def to_date(self, unix_time: int, unit: str = "Sec") -> AlxDateTime:
        """Unix time in the given unit to a broken-down date."""
        return getattr(self.c, f"AlxRtc_UnixTime{unit}ToDateTime")(unix_time)

    def from_date(self, date: AlxDateTime, unit: str = "Sec") -> int:
        """A broken-down date back to Unix time in the given unit."""
        return getattr(self.c, f"AlxRtc_DateTimeToUnixTime{unit}")(date)

    @staticmethod
    def date(year, month, day, hour=0, minute=0, second=0, ms=0, us=0, ns=0) -> AlxDateTime:
        """Build the structure the module takes; ``year`` is the full year, not the offset."""
        return AlxDateTime(year - 2000, month, day, 0, hour, minute, second, ms, us, ns)

    # -- the sub-second helpers -----------------------------------------------
    def ms_us_ns_to_ns(self, tick_ns: int, ms: int, us: int, ns: int) -> int:
        return self.c.AlxRtc_MsUsNsToNs(tick_ns, ms, us, ns)

    def ns_to_ms_us_ns(self, tick_ns: int, value_ns: int) -> tuple[int, int, int]:
        out = [ctypes.c_uint16() for _ in range(3)]
        self.c.AlxRtc_NsToMsUsNs(tick_ns, value_ns, *[ctypes.byref(o) for o in out])
        return tuple(o.value for o in out)

    def sec_fract(self, ms: int, us: int, ns: int) -> float:
        return self.c.AlxRtc_MsUsNsToSecFract(ms, us, ns)

    def sec_fract_to_ms_us_ns(self, fraction: float) -> tuple[int, int, int]:
        out = [ctypes.c_uint16() for _ in range(3)]
        self.c.AlxRtc_SecFractToMsUsNs(fraction, *[ctypes.byref(o) for o in out])
        return tuple(o.value for o in out)


class ParamMgmtLib:
    """ctypes wrapper around alxParamMgmtTest.dll: the view a CLI reaches its parameters through.

    The manager is built over a fixed table of six REAL AlxParamItem - two groups, five data types,
    a Var among the Params - declared in alxParamMgmtTestHelpers.c and documented at the top of it.
    Everything here goes through the module's public API; nothing reads the items directly.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u32, i32, cp = ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int32, ctypes.c_char_p
        c.AlxParamMgmtTest_New.restype = vp
        c.AlxParamMgmtTest_Delete.argtypes = [vp]
        for name in ("NumOfItems",):
            getattr(c, f"AlxParamMgmtTest_{name}").restype = u32
        for name in ("ParamType_Param", "ParamType_Var", "DataType_Uint8", "DataType_Uint16",
                     "DataType_Uint32", "DataType_Int32", "DataType_Float", "DataType_Bool",
                     "Status_Ok", "Status_Err"):
            getattr(c, f"AlxParamMgmtTest_{name}").restype = i32
        c.AlxParamMgmt_GetNumOfParamItems.restype = u32
        c.AlxParamMgmt_GetNumOfParamItems.argtypes = [vp]
        c.AlxParamMgmt_GetNumOfParamTypeItems.restype = u32
        c.AlxParamMgmt_GetNumOfParamTypeItems.argtypes = [vp, i32]
        for name in ("GetDataType", "GetParamType"):
            fn = getattr(c, f"AlxParamMgmt_ByIndex_{name}")
            fn.restype = i32
            fn.argtypes = [vp, u32]
        for name in ("GetKey", "GetGroupKey"):
            fn = getattr(c, f"AlxParamMgmt_ByIndex_{name}")
            fn.restype = cp
            fn.argtypes = [vp, u32]
        for name in ("GetId", "GetGroupId", "GetValLen"):
            fn = getattr(c, f"AlxParamMgmt_ByIndex_{name}")
            fn.restype = u32
            fn.argtypes = [vp, u32]
        c.AlxParamMgmt_ByIndex_GetVal_StrFormat.restype = i32
        c.AlxParamMgmt_ByIndex_GetVal_StrFormat.argtypes = [vp, u32, cp, u32]
        c.AlxParamMgmt_ByKey_SetVal_StrFormat.restype = i32
        c.AlxParamMgmt_ByKey_SetVal_StrFormat.argtypes = [vp, cp, cp]
        c.AlxParamMgmt_ById_GetValLen.restype = i32
        c.AlxParamMgmt_ById_GetValLen.argtypes = [vp, u32, ctypes.POINTER(u32)]
        for name in ("Get", "Set"):
            fn = getattr(c, f"AlxParamMgmt_ById_{name}")
            fn.restype = i32
            fn.argtypes = [vp, u32, ctypes.c_void_p, u32]
        c.AlxParamMgmt_SetValToDef_Group.argtypes = [vp, u32]
        c.AlxParamMgmt_SetValToDef_All.argtypes = [vp]

        self.OK = c.AlxParamMgmtTest_Status_Ok()
        self.ERR = c.AlxParamMgmtTest_Status_Err()
        self.PARAM = c.AlxParamMgmtTest_ParamType_Param()
        self.VAR = c.AlxParamMgmtTest_ParamType_Var()
        self.TYPES = {name: getattr(c, f"AlxParamMgmtTest_DataType_{name}")()
                      for name in ("Uint8", "Uint16", "Uint32", "Int32", "Float", "Bool")}
        self._handles: list = []

    def table(self):
        """The manager over the helper's fixed six-item table."""
        handle = self.c.AlxParamMgmtTest_New()
        assert handle, "the test helper could not allocate"
        self._handles.append(handle)
        return handle

    # -- the module's own questions -------------------------------------------
    def count(self, mgmt) -> int:
        return self.c.AlxParamMgmt_GetNumOfParamItems(mgmt)

    def count_of_type(self, mgmt, param_type: int) -> int:
        return self.c.AlxParamMgmt_GetNumOfParamTypeItems(mgmt, param_type)

    def by_index(self, mgmt, index: int) -> dict:
        """Everything the module will say about one item, in one dict."""
        c = self.c
        return {
            "key": c.AlxParamMgmt_ByIndex_GetKey(mgmt, index).decode("ascii"),
            "id": c.AlxParamMgmt_ByIndex_GetId(mgmt, index),
            "group_key": c.AlxParamMgmt_ByIndex_GetGroupKey(mgmt, index).decode("ascii"),
            "group_id": c.AlxParamMgmt_ByIndex_GetGroupId(mgmt, index),
            "data_type": c.AlxParamMgmt_ByIndex_GetDataType(mgmt, index),
            "param_type": c.AlxParamMgmt_ByIndex_GetParamType(mgmt, index),
            "val_len": c.AlxParamMgmt_ByIndex_GetValLen(mgmt, index),
        }

    def value(self, mgmt, index: int, size: int = 64) -> str:
        buff = ctypes.create_string_buffer(size)
        status = self.c.AlxParamMgmt_ByIndex_GetVal_StrFormat(mgmt, index, buff, size)
        assert status == self.OK, f"reading index {index} returned {status}"
        return buff.value.decode("ascii")

    def values(self, mgmt) -> list:
        """Every item's value as text, in index order - what a get-param dump is made of."""
        return [self.value(mgmt, i) for i in range(self.count(mgmt))]

    def set_by_key(self, mgmt, key: str, value: str) -> int:
        return self.c.AlxParamMgmt_ByKey_SetVal_StrFormat(mgmt, key.encode(), value.encode())

    def len_by_id(self, mgmt, identifier: int) -> tuple[int, int]:
        out = ctypes.c_uint32()
        return self.c.AlxParamMgmt_ById_GetValLen(mgmt, identifier, ctypes.byref(out)), out.value

    def get_by_id(self, mgmt, identifier: int, length: int) -> tuple[int, bytes]:
        buff = ctypes.create_string_buffer(length)
        status = self.c.AlxParamMgmt_ById_Get(mgmt, identifier, buff, length)
        return status, buff.raw

    def set_by_id(self, mgmt, identifier: int, raw: bytes) -> int:
        buff = ctypes.create_string_buffer(raw, len(raw))
        return self.c.AlxParamMgmt_ById_Set(mgmt, identifier, buff, len(raw))

    def defaults_group(self, mgmt, group_id: int) -> None:
        self.c.AlxParamMgmt_SetValToDef_Group(mgmt, group_id)

    def defaults_all(self, mgmt) -> None:
        self.c.AlxParamMgmt_SetValToDef_All(mgmt)

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxParamMgmtTest_Delete(handle)
        self._handles.clear()


class Ina228Lib:
    """ctypes wrapper around alxIna228Test.dll: the current sensor driver over a faked I2C bus.

    The test writes a register value into the fake the way the part would present it - most
    significant byte first, as it travels on the wire - and asks the driver what that means in volts,
    amps or degrees. The two conversion factors the driver derives from its constructor arguments are
    readable directly, because they are what a board's accuracy is decided by.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, i32, u8, u32 = (ctypes.c_void_p, ctypes.c_float, ctypes.c_int32,
                               ctypes.c_uint8, ctypes.c_uint32)
        c.AlxIna228Test_New.restype = vp
        c.AlxIna228Test_New.argtypes = [i32, f, f]
        c.AlxIna228Test_Delete.argtypes = [vp]
        c.AlxIna228Test_CurrentLsb_A.restype = f
        c.AlxIna228Test_CurrentLsb_A.argtypes = [vp]
        c.AlxIna228Test_ShuntCal.restype = ctypes.c_uint16
        c.AlxIna228Test_ShuntCal.argtypes = [vp]
        for name in ("AdcRange_163_84_mV", "AdcRange_40_96_mV", "Status_Ok", "Status_Err"):
            getattr(c, f"AlxIna228Test_{name}").restype = i32
        for name in ("GetShuntVoltage_V", "GetBusVoltage_V", "GetTemp_degC", "GetCurrent_A",
                     "GetPower_W"):
            fn = getattr(c, f"AlxIna228_{name}")
            fn.restype = i32
            fn.argtypes = [vp, ctypes.POINTER(f)]
        c.AlxI2cFake_SetReg.argtypes = [u8, ctypes.POINTER(u8), u8]
        c.AlxI2cFake_GetLastWrite.argtypes = [u8, ctypes.POINTER(u8), u8]
        for name in ("WriteCount", "ReadCount"):
            fn = getattr(c, f"AlxI2cFake_{name}")
            fn.restype = u32
            fn.argtypes = [u8]
        c.AlxI2cFake_SetSlaveReady.argtypes = [ctypes.c_bool]
        c.AlxI2cFake_SetForcedStatus.argtypes = [i32]

        self.OK = c.AlxIna228Test_Status_Ok()
        self.ERR = c.AlxIna228Test_Status_Err()
        self.RANGE_163_84_MV = c.AlxIna228Test_AdcRange_163_84_mV()
        self.RANGE_40_96_MV = c.AlxIna228Test_AdcRange_40_96_mV()
        self._handles: list = []

    # -- the part -------------------------------------------------------------
    def sensor(self, adc_range: int, shunt_ohm: float, shunt_ppm: float = 200.0):
        """A configured sensor. The one this suite is written around is (40.96 mV, 100 uOhm, 200 ppm)."""
        self.c.AlxI2cFake_Reset()
        handle = self.c.AlxIna228Test_New(adc_range, shunt_ohm, shunt_ppm)
        assert handle, "the driver refused to initialise over the fake bus"
        self._handles.append(handle)
        return handle

    def current_lsb(self, sensor) -> float:
        """Amps per count of the CURRENT register - what the reading's resolution is."""
        return self.c.AlxIna228Test_CurrentLsb_A(sensor)

    def shunt_cal(self, sensor) -> int:
        """The value the driver computes for the part's SHUNT_CAL register."""
        return self.c.AlxIna228Test_ShuntCal(sensor)

    # -- the fake bus ---------------------------------------------------------
    def set_register(self, addr: int, value: int, length: int) -> None:
        """Put a register value on the fake bus, most significant byte first, as the part sends it."""
        raw = value.to_bytes(length, "big", signed=value < 0)
        buff = (ctypes.c_uint8 * length)(*raw)
        self.c.AlxI2cFake_SetReg(addr, buff, length)

    def last_write(self, addr: int, length: int) -> bytes:
        buff = (ctypes.c_uint8 * length)()
        self.c.AlxI2cFake_GetLastWrite(addr, buff, length)
        return bytes(buff)

    def write_count(self, addr: int) -> int:
        return self.c.AlxI2cFake_WriteCount(addr)

    def read_count(self, addr: int) -> int:
        return self.c.AlxI2cFake_ReadCount(addr)

    # -- the readings ---------------------------------------------------------
    def read(self, sensor, quantity: str) -> tuple[int, float]:
        """One reading: ``current_A``, ``bus_voltage_V``, ``shunt_voltage_V``, ``temp_degC``, ``power_W``."""
        names = {"current_A": "GetCurrent_A", "bus_voltage_V": "GetBusVoltage_V",
                 "shunt_voltage_V": "GetShuntVoltage_V", "temp_degC": "GetTemp_degC",
                 "power_W": "GetPower_W"}
        out = ctypes.c_float()
        status = getattr(self.c, f"AlxIna228_{names[quantity]}")(sensor, ctypes.byref(out))
        return status, out.value

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxIna228Test_Delete(handle)
        self._handles.clear()


class Pi4ioeLib:
    """ctypes wrapper around alxPi4ioe5v6534qTest.dll: the 40-channel IO expander, over two fakes.

    The driver keeps a shadow of the output register and pushes it to the part in one transfer, so a
    test writes pins, calls ``handle``, and reads what actually went on the bus. Inputs go the other
    way: the test puts bytes in the fake, calls ``handle``, and asks the driver about a pin.
    """

    PORTS, PINS = 5, 8

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u8, i32, b, u32 = (ctypes.c_void_p, ctypes.c_uint8, ctypes.c_int32,
                               ctypes.c_bool, ctypes.c_uint32)
        c.AlxPi4ioeTest_New.restype = vp
        c.AlxPi4ioeTest_New.argtypes = [u8]
        c.AlxPi4ioeTest_Delete.argtypes = [vp]
        c.AlxPi4ioeTest_ResetPin.restype = vp
        c.AlxPi4ioeTest_ResetPin.argtypes = [vp]
        for name in ("InputPortAddr", "OutputPortAddr"):
            fn = getattr(c, f"AlxPi4ioeTest_{name}")
            fn.restype = u8
            fn.argtypes = [vp]
        for name in ("Status_Ok", "Status_Err"):
            getattr(c, f"AlxPi4ioeTest_{name}").restype = i32
        c.AlxPi4ioe5v6534q_Handle.restype = i32
        c.AlxPi4ioe5v6534q_Handle.argtypes = [vp, u8, u8]
        c.AlxPi4ioe5v6534q_IoPin_Read.restype = b
        c.AlxPi4ioe5v6534q_IoPin_Read.argtypes = [vp, u8, u8]
        c.AlxPi4ioe5v6534q_IoPin_Write.argtypes = [vp, u8, u8, b]
        for name in ("Set", "Reset", "Toggle"):
            getattr(c, f"AlxPi4ioe5v6534q_IoPin_{name}").argtypes = [vp, u8, u8]
        c.AlxI2cFake_SetReg.argtypes = [u8, ctypes.POINTER(u8), u8]
        c.AlxI2cFake_GetLastWrite.argtypes = [u8, ctypes.POINTER(u8), u8]
        for name in ("WriteCount", "ReadCount"):
            fn = getattr(c, f"AlxI2cFake_{name}")
            fn.restype = u32
            fn.argtypes = [u8]
        c.AlxI2cFake_SetForcedStatus.argtypes = [i32]
        c.AlxIoPinFake_Level.restype = b
        c.AlxIoPinFake_Level.argtypes = [vp]
        for name in ("InitCount", "DeInitCount", "WriteCount"):
            fn = getattr(c, f"AlxIoPinFake_{name}")
            fn.restype = u32
            fn.argtypes = [vp]

        self.OK = c.AlxPi4ioeTest_Status_Ok()
        self.ERR = c.AlxPi4ioeTest_Status_Err()
        self._handles: list = []

    def expander(self, i2c_addr: int = 0x40):
        """A constructed and initialised expander, with both fakes cleared first."""
        self.c.AlxI2cFake_Reset()
        self.c.AlxIoPinFake_Reset()
        handle = self.c.AlxPi4ioeTest_New(i2c_addr)
        assert handle, "the driver refused to initialise over the fakes"
        self._handles.append(handle)
        return handle

    # -- the driver -----------------------------------------------------------
    def addresses(self, dev) -> tuple[int, int]:
        """The input and output register addresses the driver was built with."""
        return (self.c.AlxPi4ioeTest_InputPortAddr(dev),
                self.c.AlxPi4ioeTest_OutputPortAddr(dev))

    def write(self, dev, port: int, pin: int, value: bool) -> None:
        self.c.AlxPi4ioe5v6534q_IoPin_Write(dev, port, pin, value)

    def act(self, dev, action: str, port: int, pin: int) -> None:
        """``Set``, ``Reset`` or ``Toggle`` one pin."""
        getattr(self.c, f"AlxPi4ioe5v6534q_IoPin_{action}")(dev, port, pin)

    def read(self, dev, port: int, pin: int) -> bool:
        return self.c.AlxPi4ioe5v6534q_IoPin_Read(dev, port, pin)

    def handle(self, dev, in_ports: int = PORTS, out_ports: int = PORTS) -> int:
        """One service pass: read ``in_ports`` input bytes, write ``out_ports`` output bytes."""
        return self.c.AlxPi4ioe5v6534q_Handle(dev, in_ports, out_ports)

    def clear_all(self, dev) -> None:
        """Drive every one of the forty channels low - the output register starts at all ones."""
        for port in range(self.PORTS):
            for pin in range(self.PINS):
                self.write(dev, port, pin, False)

    # -- the fakes ------------------------------------------------------------
    def set_inputs(self, addr: int, values: bytes) -> None:
        buff = (ctypes.c_uint8 * len(values))(*values)
        self.c.AlxI2cFake_SetReg(addr, buff, len(values))

    def outputs_on_the_bus(self, addr: int, length: int = PORTS) -> bytes:
        buff = (ctypes.c_uint8 * length)()
        self.c.AlxI2cFake_GetLastWrite(addr, buff, length)
        return bytes(buff)

    def bus_writes(self, addr: int) -> int:
        return self.c.AlxI2cFake_WriteCount(addr)

    def bus_reads(self, addr: int) -> int:
        return self.c.AlxI2cFake_ReadCount(addr)

    def reset_pin(self, dev) -> tuple[bool, int]:
        """The reset pin's level and how many times it was driven."""
        pin = self.c.AlxPi4ioeTest_ResetPin(dev)
        return self.c.AlxIoPinFake_Level(pin), self.c.AlxIoPinFake_WriteCount(pin)

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxPi4ioeTest_Delete(handle)
        self._handles.clear()


class FsSafeLib:
    """ctypes wrapper around alxFsSafeTest.dll: two copies of a file and the logic that picks one.

    A test writes onto the "flash" directly to set up a state the module then has to recover from -
    a missing copy, a corrupt one, two copies that disagree - so the CRC the module uses is
    exposed here rather than restated.
    """

    OPS = ("mount", "unmount", "format", "open", "close", "read", "write", "remove")

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, b, u16, u32, i32 = (ctypes.c_void_p, ctypes.c_bool, ctypes.c_uint16,
                                ctypes.c_uint32, ctypes.c_int32)
        u8p = ctypes.POINTER(ctypes.c_uint8)
        cp = ctypes.c_char_p
        c.AlxFsSafeTest_New.restype = vp
        c.AlxFsSafeTest_New.argtypes = [b]
        c.AlxFsSafeTest_Delete.argtypes = [vp]
        c.AlxFsSafeTest_FsSafe.restype = vp
        c.AlxFsSafeTest_FsSafe.argtypes = [vp]
        c.AlxFsSafeTest_CrcLen.restype = u32
        c.AlxFsSafeTest_CrcLen.argtypes = [vp]
        c.AlxFsSafeTest_Crc.restype = u16
        c.AlxFsSafeTest_Crc.argtypes = [vp, u8p, u32]
        for name in ("Read", "Write"):
            fn = getattr(c, f"AlxFsSafe_File_{name}")
            fn.restype = i32
            fn.argtypes = [vp, cp, vp, u32]
        c.AlxFsFake_FailNext.argtypes = [u32, i32]
        c.AlxFsFake_FailSkip.argtypes = [u32, u32]
        c.AlxFsFake_CallCount.restype = u32
        c.AlxFsFake_CallCount.argtypes = [u32]
        for name in ("OpenCount", "CloseCount", "FormatCount", "FilesHeld"):
            getattr(c, f"AlxFsFake_{name}").restype = u32
        c.AlxFsFake_Put.argtypes = [cp, u8p, u32]
        c.AlxFsFake_Get.restype = u32
        c.AlxFsFake_Get.argtypes = [cp, u8p, u32]
        c.AlxFsFake_Has.restype = b
        c.AlxFsFake_Has.argtypes = [cp]
        self._handles: list = []

    def new(self, *, use_orig: bool = False) -> int:
        handle = self.c.AlxFsSafeTest_New(use_orig)
        assert handle, "the safe storage helper could not allocate"
        self._handles.append(handle)
        return handle

    # -- the module ------------------------------------------------------------
    def _safe(self, handle: int) -> int:
        return self.c.AlxFsSafeTest_FsSafe(handle)

    def read(self, handle: int, path: str, length: int) -> tuple:
        buff = ctypes.create_string_buffer(length)
        status = self.c.AlxFsSafe_File_Read(self._safe(handle), path.encode("ascii"),
                                            buff, length)
        return status, buff.raw[:length]

    def write(self, handle: int, path: str, data: bytes) -> int:
        buff = ctypes.create_string_buffer(data, len(data))
        return self.c.AlxFsSafe_File_Write(self._safe(handle), path.encode("ascii"),
                                           buff, len(data))

    # -- the module's own CRC, so a test can build a copy by hand ---------------
    def crc_len(self, handle: int) -> int:
        return self.c.AlxFsSafeTest_CrcLen(handle)

    def crc(self, handle: int, data: bytes) -> int:
        arr = (ctypes.c_uint8 * len(data))(*data)
        return self.c.AlxFsSafeTest_Crc(handle, arr, len(data))

    def copy_bytes(self, handle: int, data: bytes) -> bytes:
        """What one good copy of `data` looks like on the flash: the data, then its CRC."""
        crc = self.crc(handle, data)
        return data + crc.to_bytes(self.crc_len(handle), "little")

    # -- the flash under it ----------------------------------------------------
    def put_copy(self, handle: int, path: str, which: str, data: bytes) -> None:
        """Put a VALID copy on the flash - "a" keeps the plain name, "b" takes the B suffix."""
        self.put_raw(self._suffixed(path, which), self.copy_bytes(handle, data))

    def put_raw(self, path: str, blob: bytes) -> None:
        arr = (ctypes.c_uint8 * len(blob))(*blob)
        self.c.AlxFsFake_Put(path.encode("ascii"), arr, len(blob))

    def raw(self, path: str, len_max: int = 256) -> bytes:
        arr = (ctypes.c_uint8 * len_max)()
        n = self.c.AlxFsFake_Get(path.encode("ascii"), arr, len_max)
        return bytes(arr[:n])

    def copy(self, handle: int, path: str, which: str, len_max: int = 256) -> bytes:
        return self.raw(self._suffixed(path, which), len_max)

    def has(self, path: str) -> bool:
        return bool(self.c.AlxFsFake_Has(path.encode("ascii")))

    @staticmethod
    def _suffixed(path: str, which: str) -> str:
        """The module's own naming: copy A keeps the plain name, copy B gets a B before the dot."""
        stem, dot, ext = path.rpartition(".")
        assert dot, "the module asserts that a path has a dot"
        return f"{stem}.{ext}" if which == "a" else f"{stem}B.{ext}"

    # -- failures --------------------------------------------------------------
    def fail(self, op: str, times: int = 1, *, after: int = 0) -> None:
        """Fail an operation - `after` calls let through first, then `times` failures."""
        self.c.AlxFsFake_FailSkip(self.OPS.index(op), after)
        self.c.AlxFsFake_FailNext(self.OPS.index(op), times)

    def opens(self) -> int:
        return self.c.AlxFsFake_OpenCount()

    def closes(self) -> int:
        return self.c.AlxFsFake_CloseCount()

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxFsSafeTest_Delete(handle)
        self._handles.clear()


class LinLib:
    """ctypes wrapper around alxLinTest.dll: a LIN master and the bytes it puts on the wire."""

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, b, u8, u32, i32 = (ctypes.c_void_p, ctypes.c_bool, ctypes.c_uint8,
                               ctypes.c_uint32, ctypes.c_int32)
        u8p = ctypes.POINTER(ctypes.c_uint8)
        c.AlxLinTest_New.restype = vp
        c.AlxLinTest_Delete.argtypes = [vp]
        c.AlxLinTest_Lin.restype = vp
        c.AlxLinTest_Lin.argtypes = [vp]
        c.AlxLinTest_Port.restype = vp
        c.AlxLinTest_Port.argtypes = [vp]
        c.AlxLinTest_Publish.restype = i32
        c.AlxLinTest_Publish.argtypes = [vp, u8, u8p, u8, b]
        for name in ("Init", "DeInit"):
            fn = getattr(c, f"AlxLin_Master_{name}")
            fn.restype = i32
            fn.argtypes = [vp]
        c.AlxLin_Master_IsInit.restype = b
        c.AlxLin_Master_IsInit.argtypes = [vp]
        c.AlxLin_SetNad.argtypes = [vp, u8]
        c.AlxLin_GetNad.restype = u8
        c.AlxLin_GetNad.argtypes = [vp]
        c.AlxSerialPortFake_TxRead.restype = u32
        c.AlxSerialPortFake_TxRead.argtypes = [vp, u8p, u32]
        c.AlxSerialPortFake_TxNumOfEntries.restype = u32
        c.AlxSerialPortFake_TxNumOfEntries.argtypes = [vp]

        # The trace axis: alxLin.c's own sites, the three-level probe, and the recorder they land in
        c.AlxLinTest_Subscribe.restype = i32
        c.AlxLinTest_Subscribe.argtypes = [vp, u8, u8]
        c.AlxLinTest_RxFlush.argtypes = [vp]
        c.AlxLinTest_RxByte.argtypes = [vp, u8]
        for name in ("TraceArgEvalsWrn", "TraceArgEvalsDbg", "TraceArgEvalsVrb"):
            getattr(c, f"AlxLinTest_{name}").restype = u32
        c.AlxTracePc_Count.restype = u32
        c.AlxTracePc_CountAtLevel.restype = u32
        c.AlxTracePc_CountAtLevel.argtypes = [u8]
        c.AlxTracePc_LastLevel.restype = u8
        c.AlxTracePc_LastLine.restype = u32
        c.AlxTracePc_LastFile.restype = ctypes.c_char_p
        c.AlxTracePc_LastFun.restype = ctypes.c_char_p
        c.AlxTracePc_LevelConfigured.restype = u8
        self._handles: list = []

    # -- the trace axis -----------------------------------------------------
    def trace_reset(self) -> None:
        """Both recorders: the sink's counters and the probe's argument counters."""
        self.c.AlxTracePc_Reset()
        self.c.AlxLinTest_TraceReset()

    def traces(self) -> int:
        return self.c.AlxTracePc_Count()

    def traces_at(self, level: int) -> int:
        return self.c.AlxTracePc_CountAtLevel(level)

    def arg_evals(self) -> tuple[int, int, int]:
        """How many times the WRN, DBG and VRB probe ARGUMENTS were evaluated."""
        return (self.c.AlxLinTest_TraceArgEvalsWrn(),
                self.c.AlxLinTest_TraceArgEvalsDbg(),
                self.c.AlxLinTest_TraceArgEvalsVrb())

    def last_trace(self) -> tuple[int, str, int, str]:
        return (self.c.AlxTracePc_LastLevel(),
                (self.c.AlxTracePc_LastFile() or b"").decode("ascii", "replace"),
                self.c.AlxTracePc_LastLine(),
                (self.c.AlxTracePc_LastFun() or b"").decode("ascii", "replace"))

    def protected_id(self, id_: int) -> int:
        """The protected identifier for `id_`, taken OFF THE WIRE rather than recomputed.

        AlxLin_CalcProtectedId is static, so a second spelling of the LIN parity rule in Python
        would be exactly the kind of duplicated constant this suite exists to catch. A master's
        frame header is SYNC then the protected id, so byte 1 of what Subscribe transmits is it.
        """
        handle = self.new()
        try:
            self.c.AlxLinTest_Subscribe(handle, id_, 1)
            buff = (ctypes.c_uint8 * 2)()
            got = self.c.AlxSerialPortFake_TxRead(self.c.AlxLinTest_Port(handle), buff, 2)
            assert got == 2, f"the master put {got} header bytes on the wire, expected 2"
            return int(buff[1])
        finally:
            # The fake has ALX_SERIAL_PORT_FAKE_MAX_PORTS (2) slots and calls exit(1) when they run
            # out - which takes the runner down rather than failing a test. Give this one straight back.
            self.free(handle)

    def new(self, *, init: bool = True) -> int:
        handle = self.c.AlxLinTest_New()
        assert handle, "the LIN helper could not allocate"
        self._handles.append(handle)
        if init:
            assert self.init(handle) == 0, "the master would not initialise"
        return handle

    # -- the module ------------------------------------------------------------
    def _lin(self, handle: int) -> int:
        return self.c.AlxLinTest_Lin(handle)

    def init(self, handle: int) -> int:
        return self.c.AlxLin_Master_Init(self._lin(handle))

    def deinit(self, handle: int) -> int:
        return self.c.AlxLin_Master_DeInit(self._lin(handle))

    def is_init(self, handle: int) -> bool:
        return bool(self.c.AlxLin_Master_IsInit(self._lin(handle)))

    def set_nad(self, handle: int, nad: int) -> None:
        self.c.AlxLin_SetNad(self._lin(handle), nad)

    def nad(self, handle: int) -> int:
        return self.c.AlxLin_GetNad(self._lin(handle))

    def publish(self, handle: int, frame_id: int, data: bytes, *, enhanced: bool = False) -> int:
        arr = (ctypes.c_uint8 * max(len(data), 1))(*data)
        return self.c.AlxLinTest_Publish(handle, frame_id, arr, len(data), enhanced)

    # -- the wire --------------------------------------------------------------
    def wire(self, handle: int) -> bytes:
        """Every byte the master has put on the bus since it was last read."""
        port = self.c.AlxLinTest_Port(handle)
        n = self.c.AlxSerialPortFake_TxNumOfEntries(port)
        if n == 0:
            return b""
        buff = (ctypes.c_uint8 * n)()
        got = self.c.AlxSerialPortFake_TxRead(port, buff, n)
        return bytes(buff[:got])

    def free(self, handle: int) -> None:
        self.c.AlxLinTest_Delete(handle)
        self._handles.remove(handle)

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxLinTest_Delete(handle)
        self._handles.clear()


class DelayLib:
    """ctypes wrapper around alxDelayTest.dll: six busy waits over a clock that moves by itself."""

    UNITS = ("ns", "us", "ms", "sec", "min", "hr")

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        u64, u32 = ctypes.c_uint64, ctypes.c_uint32
        for unit in self.UNITS:
            getattr(c, f"AlxDelay_{unit}").argtypes = [u64]
        c.AlxTickFake_Reset.argtypes = [u64]
        c.AlxTickFake_SetNow_ns.argtypes = [u64]
        c.AlxTickFake_Now_ns.restype = u64
        c.AlxTickFake_Reads.restype = u32

    def clock(self, *, step_ns: int = 1, start_ns: int = 0) -> None:
        """Start the clock: how far it moves per read, and where it starts."""
        self.c.AlxTickFake_Reset(step_ns)
        if start_ns:
            self.c.AlxTickFake_SetNow_ns(start_ns)

    def delay(self, unit: str, count: int) -> None:
        getattr(self.c, f"AlxDelay_{unit}")(count)

    def now_ns(self) -> int:
        return self.c.AlxTickFake_Now_ns()

    def reads(self) -> int:
        return self.c.AlxTickFake_Reads()


class ParamKvStoreLib:
    """ctypes wrapper around alxParamKvStoreTest.dll: the store, over a file system that can fail.

    The fake's operations are named rather than numbered on this side, because a test that said
    `fail("open")` reads and a test that said `fail(3)` does not.
    """

    OPS = ("mount", "unmount", "format", "open", "close", "read", "write", "remove")

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, b, u32, i32 = ctypes.c_void_p, ctypes.c_bool, ctypes.c_uint32, ctypes.c_int32
        u8p = ctypes.POINTER(ctypes.c_uint8)
        cp = ctypes.c_char_p
        c.AlxParamKvStoreTest_New.restype = vp
        c.AlxParamKvStoreTest_Delete.argtypes = [vp]
        c.AlxParamKvStoreTest_Store.restype = vp
        c.AlxParamKvStoreTest_Store.argtypes = [vp]
        c.AlxParamKvStoreTest_IsInit.restype = b
        c.AlxParamKvStoreTest_IsInit.argtypes = [vp]
        for name in ("Init", "DeInit"):
            fn = getattr(c, f"AlxParamKvStore_{name}")
            fn.restype = i32
            fn.argtypes = [vp]
        c.AlxParamKvStore_Get.restype = i32
        c.AlxParamKvStore_Get.argtypes = [vp, cp, vp, u32, ctypes.POINTER(u32)]
        c.AlxParamKvStore_Set.restype = i32
        c.AlxParamKvStore_Set.argtypes = [vp, cp, vp, u32]
        c.AlxParamKvStore_Remove.restype = i32
        c.AlxParamKvStore_Remove.argtypes = [vp, cp]
        c.AlxFsFake_FailNext.argtypes = [u32, i32]
        c.AlxFsFake_FailSkip.argtypes = [u32, u32]
        c.AlxFsFake_CallCount.restype = u32
        c.AlxFsFake_CallCount.argtypes = [u32]
        for name in ("OpenCount", "CloseCount", "FormatCount", "FilesHeld"):
            getattr(c, f"AlxFsFake_{name}").restype = u32
        c.AlxFsFake_IsMounted.restype = b
        c.AlxFsFake_Put.argtypes = [cp, u8p, u32]
        c.AlxFsFake_Get.restype = u32
        c.AlxFsFake_Get.argtypes = [cp, u8p, u32]
        c.AlxFsFake_Has.restype = b
        c.AlxFsFake_Has.argtypes = [cp]
        c.AlxFsFake_LastOpenMode.restype = cp
        self._handles: list = []

    def new(self, *, init: bool = True) -> int:
        handle = self.c.AlxParamKvStoreTest_New()
        assert handle, "the key-value store helper could not allocate"
        self._handles.append(handle)
        if init:
            assert self.init(handle) == 0, "the store would not initialise over a healthy fake"
        return handle

    # -- the module ------------------------------------------------------------
    def _store(self, handle: int) -> int:
        return self.c.AlxParamKvStoreTest_Store(handle)

    def init(self, handle: int) -> int:
        return self.c.AlxParamKvStore_Init(self._store(handle))

    def deinit(self, handle: int) -> int:
        return self.c.AlxParamKvStore_DeInit(self._store(handle))

    def is_init(self, handle: int) -> bool:
        return bool(self.c.AlxParamKvStoreTest_IsInit(handle))

    def get(self, handle: int, key: str, len_max: int = 64) -> tuple:
        buff = ctypes.create_string_buffer(len_max)
        actual = ctypes.c_uint32(0)
        status = self.c.AlxParamKvStore_Get(self._store(handle), key.encode("ascii"),
                                            buff, len_max, ctypes.byref(actual))
        return status, buff.raw[:actual.value]

    def set(self, handle: int, key: str, data: bytes) -> int:
        buff = ctypes.create_string_buffer(data, len(data))
        return self.c.AlxParamKvStore_Set(self._store(handle), key.encode("ascii"),
                                          buff, len(data))

    def remove(self, handle: int, key: str) -> int:
        return self.c.AlxParamKvStore_Remove(self._store(handle), key.encode("ascii"))

    # -- the file system under it ----------------------------------------------
    def fail(self, op: str, times: int = 1) -> None:
        """Make one file system operation fail: `times` more calls, or every call if negative."""
        self.c.AlxFsFake_FailNext(self.OPS.index(op), times)

    def calls(self, op: str) -> int:
        return self.c.AlxFsFake_CallCount(self.OPS.index(op))

    def opens(self) -> int:
        return self.c.AlxFsFake_OpenCount()

    def closes(self) -> int:
        return self.c.AlxFsFake_CloseCount()

    def formats(self) -> int:
        return self.c.AlxFsFake_FormatCount()

    def files_held(self) -> int:
        return self.c.AlxFsFake_FilesHeld()

    def mounted(self) -> bool:
        return bool(self.c.AlxFsFake_IsMounted())

    def put_on_flash(self, key: str, data: bytes) -> None:
        """What is already stored before the module ever runs."""
        arr = (ctypes.c_uint8 * len(data))(*data)
        self.c.AlxFsFake_Put(key.encode("ascii"), arr, len(data))

    def on_flash(self, key: str, len_max: int = 64) -> bytes:
        arr = (ctypes.c_uint8 * len_max)()
        n = self.c.AlxFsFake_Get(key.encode("ascii"), arr, len_max)
        return bytes(arr[:n])

    def has_on_flash(self, key: str) -> bool:
        return bool(self.c.AlxFsFake_Has(key.encode("ascii")))

    def last_open_mode(self) -> str:
        """The mode string the module last asked a file to be opened with."""
        return (self.c.AlxFsFake_LastOpenMode() or b"").decode("ascii")

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxParamKvStoreTest_Delete(handle)
        self._handles.clear()


class MuxLib:
    """ctypes wrapper around alxMuxTest.dll: an enable pin, some select pins, and a channel code.

    Everything a test asks about is a pin level or a pin's init count, through the library's own IO
    pin fake, so the multiplexer is checked by what it DRIVES rather than by what it stores.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, b, u8, i32 = ctypes.c_void_p, ctypes.c_bool, ctypes.c_uint8, ctypes.c_int32
        c.AlxMuxTest_New.restype = vp
        c.AlxMuxTest_New.argtypes = [u8]
        c.AlxMuxTest_Delete.argtypes = [vp]
        c.AlxMuxTest_Mux.restype = vp
        c.AlxMuxTest_Mux.argtypes = [vp]
        c.AlxMuxTest_EnPin.restype = vp
        c.AlxMuxTest_EnPin.argtypes = [vp]
        c.AlxMuxTest_SelPin.restype = vp
        c.AlxMuxTest_SelPin.argtypes = [vp, u8]
        for name in ("Init", "DeInit", "DeInit_Select"):
            fn = getattr(c, f"AlxMux_{name}")
            fn.restype = i32
            fn.argtypes = [vp]
        c.AlxMux_Enable.argtypes = [vp, b]
        c.AlxMux_Select.argtypes = [vp, i32]
        c.AlxIoPinFake_Level.restype = b
        c.AlxIoPinFake_Level.argtypes = [vp]
        c.AlxIoPinFake_InitCount.restype = ctypes.c_uint32
        c.AlxIoPinFake_InitCount.argtypes = [vp]
        c.AlxIoPinFake_DeInitCount.restype = ctypes.c_uint32
        c.AlxIoPinFake_DeInitCount.argtypes = [vp]
        # P419 reads the overflow flag from the test itself rather than only at teardown, so this
        # group declares it here as well as in _assert_pins_fitted. It has to be declared BEFORE
        # the first call: a C bool comes back in AL and ctypes' default int restype reads the whole
        # register, so an undeclared call answers with whatever the upper bits happened to hold -
        # found 11.09 as a test that passed or failed depending on where the random order put it.
        c.AlxIoPinFake_DidOverflow.restype = b
        self._handles: list = []

    def new(self, num_of_sel_pins: int = 4, *, init: bool = True) -> int:
        """A multiplexer with that many select pins, initialised unless a test wants it raw."""
        handle = self.c.AlxMuxTest_New(num_of_sel_pins)
        assert handle, "the multiplexer helper could not allocate"
        self._handles.append(handle)
        if init:
            self.init(handle)
        return handle

    # -- the module ------------------------------------------------------------
    def _mux(self, handle: int) -> int:
        return self.c.AlxMuxTest_Mux(handle)

    def init(self, handle: int) -> int:
        return self.c.AlxMux_Init(self._mux(handle))

    def deinit(self, handle: int) -> int:
        return self.c.AlxMux_DeInit(self._mux(handle))

    def deinit_select(self, handle: int) -> int:
        return self.c.AlxMux_DeInit_Select(self._mux(handle))

    def enable(self, handle: int, val: bool) -> None:
        self.c.AlxMux_Enable(self._mux(handle), val)

    def select(self, handle: int, ch: int) -> None:
        self.c.AlxMux_Select(self._mux(handle), ch)

    # -- the pins --------------------------------------------------------------
    def en_pin(self, handle: int) -> int:
        return self.c.AlxMuxTest_EnPin(handle)

    def sel_pin(self, handle: int, index: int) -> int:
        return self.c.AlxMuxTest_SelPin(handle, index)

    def en_level(self, handle: int) -> bool:
        return bool(self.c.AlxIoPinFake_Level(self.en_pin(handle)))

    def code(self, handle: int, num_of_sel_pins: int) -> int:
        """The select pins read back as the binary number they spell, pin 0 the least significant."""
        return sum(
            bool(self.c.AlxIoPinFake_Level(self.sel_pin(handle, i))) << i
            for i in range(num_of_sel_pins)
        )

    def init_count(self, pin: int) -> int:
        return self.c.AlxIoPinFake_InitCount(pin)

    def deinit_count(self, pin: int) -> int:
        return self.c.AlxIoPinFake_DeInitCount(pin)

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxMuxTest_Delete(handle)
        self._handles.clear()


class BtsLib:
    """ctypes wrapper around alxBts724gTest.dll: a high side switch and its one status pin.

    The clock belongs to the test; the part's own filter times are read back from the driver rather
    than restated here.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, u64, b = ctypes.c_void_p, ctypes.c_float, ctypes.c_uint64, ctypes.c_bool
        c.AlxBts724gTest_New.restype = vp
        c.AlxBts724gTest_Delete.argtypes = [vp]
        for name in ("Bts", "OutPin", "StatusPin"):
            fn = getattr(c, f"AlxBts724gTest_{name}")
            fn.restype = vp
            fn.argtypes = [vp]
        for name in ("OpenLoadTrue_ms", "OverTempTrue_ms", "ClearTime_ms"):
            fn = getattr(c, f"AlxBts724gTest_{name}")
            fn.restype = f
            fn.argtypes = [vp]
        for name in ("Init", "DeInit", "Handle", "SetOut", "ResetOut"):
            getattr(c, f"AlxBts724g_{name}").argtypes = [vp]
        c.AlxBts724g_WriteOut.argtypes = [vp, b]
        for name in ("IsOpenLoadDetected", "IsOverTempDetected",
                     "WasOpenLoadDetected", "WasOverTempDetected"):
            fn = getattr(c, f"AlxBts724g_{name}")
            fn.restype = b
            fn.argtypes = [vp]
        c.AlxIoPinFake_InitCount.restype = ctypes.c_uint32
        c.AlxIoPinFake_InitCount.argtypes = [vp]
        c.AlxIoPinFake_DeInitCount.restype = ctypes.c_uint32
        c.AlxIoPinFake_DeInitCount.argtypes = [vp]
        c.AlxIoPinFake_SetLevel.argtypes = [vp, b]
        c.AlxIoPinFake_Level.restype = b
        c.AlxIoPinFake_Level.argtypes = [vp]
        c.AlxTick_Ctor.argtypes = [vp]
        c.AlxTick_IncRange_ns.argtypes = [vp, u64]
        self.tick = ctypes.addressof(ctypes.c_uint8.in_dll(c, "alxTick"))
        self._handles: list = []

    # -- the clock the test owns ----------------------------------------------
    def tick_reset(self) -> None:
        self.c.AlxTick_Ctor(self.tick)
        self.c.AlxIrqFake_Reset()

    def advance_ms(self, ms: float) -> None:
        self.c.AlxTick_IncRange_ns(self.tick, round(ms * 1_000_000))

    # -- one switch -----------------------------------------------------------
    def new(self):
        """A switch, initialised, with its status pin low - which is what a fault looks like."""
        handle = self.c.AlxBts724gTest_New()
        assert handle, "AlxBts724gTest_New returned NULL"
        self._handles.append(handle)
        self.c.AlxBts724g_Init(self.c.AlxBts724gTest_Bts(handle))
        return handle

    def _b(self, obj):
        return self.c.AlxBts724gTest_Bts(obj)

    def set_status(self, obj, healthy: bool) -> None:
        """The part pulls its status pin LOW to report a fault; high is healthy."""
        self.c.AlxIoPinFake_SetLevel(self.c.AlxBts724gTest_StatusPin(obj), healthy)

    def out_level(self, obj) -> bool:
        return self.c.AlxIoPinFake_Level(self.c.AlxBts724gTest_OutPin(obj))

    def set_out(self, obj) -> None:
        self.c.AlxBts724g_SetOut(self._b(obj))

    def reset_out(self, obj) -> None:
        self.c.AlxBts724g_ResetOut(self._b(obj))

    def write_out(self, obj, state: bool) -> None:
        self.c.AlxBts724g_WriteOut(self._b(obj), state)

    def handle(self, obj, *, for_ms: float = 0.0, step_ms: float = 1.0) -> None:
        """Run the driver, advancing the clock in steps, the way a super-loop would."""
        self.c.AlxBts724g_Handle(self._b(obj))
        elapsed = 0.0
        while elapsed < for_ms:
            self.advance_ms(step_ms)
            self.c.AlxBts724g_Handle(self._b(obj))
            elapsed += step_ms

    def open_load(self, obj) -> bool:
        return self.c.AlxBts724g_IsOpenLoadDetected(self._b(obj))

    def over_temp(self, obj) -> bool:
        return self.c.AlxBts724g_IsOverTempDetected(self._b(obj))

    def was_open_load(self, obj) -> bool:
        return self.c.AlxBts724g_WasOpenLoadDetected(self._b(obj))

    def was_over_temp(self, obj) -> bool:
        return self.c.AlxBts724g_WasOverTempDetected(self._b(obj))

    def open_load_time_ms(self, obj) -> float:
        return self.c.AlxBts724gTest_OpenLoadTrue_ms(obj)

    def over_temp_time_ms(self, obj) -> float:
        return self.c.AlxBts724gTest_OverTempTrue_ms(obj)

    def clear_time_ms(self, obj) -> float:
        return self.c.AlxBts724gTest_ClearTime_ms(obj)

    def deinit(self, obj) -> None:
        self.c.AlxBts724g_DeInit(self.c.AlxBts724gTest_Bts(obj))

    def pin_counts(self, obj) -> dict:
        """How often each of the switch's two pins was initialised and de-initialised."""
        return {
            name: (self.c.AlxIoPinFake_InitCount(pin), self.c.AlxIoPinFake_DeInitCount(pin))
            for name, pin in (("out", self.c.AlxBts724gTest_OutPin(obj)),
                              ("status", self.c.AlxBts724gTest_StatusPin(obj)))
        }

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxBts724gTest_Delete(handle)
        self._handles.clear()


class NtcLib:
    """ctypes wrapper around alxNtcTest.dll: a thermistor resistance to a temperature."""

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        c.AlxNtcg103jf103ft1s_ResToTemp_degC.restype = ctypes.c_int16
        c.AlxNtcg103jf103ft1s_ResToTemp_degC.argtypes = [ctypes.c_uint32]

    def temp(self, res_ohm: int) -> int:
        return self.c.AlxNtcg103jf103ft1s_ResToTemp_degC(res_ohm)


class AudioPlayerLib:
    """ctypes wrapper around alxAudioPlayerTest.dll: a track in memory and a cursor into it.

    The helper owns the track bytes, so a test passes a list of ints and gets a player back. The
    byte offset and the step are readable because "where in the track am I" has no public getter
    and is the thing every transport test is about.
    """

    INT8 = 0
    UINT8 = 1
    INT16 = 2
    UINT16 = 3
    INT16_FLASH = 4

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, u32, i32, u8, b = (ctypes.c_void_p, ctypes.c_float, ctypes.c_uint32,
                                  ctypes.c_int32, ctypes.c_uint8, ctypes.c_bool)
        c.AlxAudioPlayerTest_New.restype = vp
        c.AlxAudioPlayerTest_New.argtypes = [ctypes.POINTER(u8), u32, u32, i32, b]
        c.AlxAudioPlayerTest_Delete.argtypes = [vp]
        c.AlxAudioPlayerTest_Player.restype = vp
        c.AlxAudioPlayerTest_Player.argtypes = [vp]
        c.AlxAudioPlayerTest_LoadOther.argtypes = [vp, ctypes.POINTER(u8), u32, u32, i32, b]
        for name in ("Offset_Byte", "Step_Byte"):
            fn = getattr(c, f"AlxAudioPlayerTest_{name}")
            fn.restype = u32
            fn.argtypes = [vp]
        for name in ("GetSampleL", "GetSampleR", "GetSampleMono"):
            fn = getattr(c, f"AlxAudioPlayer_{name}")
            fn.restype = f
            fn.argtypes = [vp]
        for name in ("IncSampleOffset", "Play", "Stop", "Pause", "Replay", "LoopOn", "LoopOff"):
            getattr(c, f"AlxAudioPlayer_{name}").argtypes = [vp]
        c.AlxAudioPlayer_LoopConfig.argtypes = [vp, b]
        c.AlxAudioPlayer_IsPlaying.restype = b
        c.AlxAudioPlayer_IsPlaying.argtypes = [vp]
        self._handles: list = []

    def new(self, track, *, encoding=INT8, mono=True, start_sample=0):
        """A player over a copy of `track`; released when the test ends."""
        arr = (ctypes.c_uint8 * len(track))(*track)
        handle = self.c.AlxAudioPlayerTest_New(arr, len(track), start_sample, encoding, mono)
        assert handle, "AlxAudioPlayerTest_New returned NULL"
        self._handles.append(handle)
        return handle

    def load(self, obj, track, *, encoding=INT8, mono=True, start_sample=0):
        arr = (ctypes.c_uint8 * len(track))(*track)
        self.c.AlxAudioPlayerTest_LoadOther(obj, arr, len(track), start_sample, encoding, mono)

    def _p(self, obj):
        return self.c.AlxAudioPlayerTest_Player(obj)

    def offset(self, obj) -> int:
        return self.c.AlxAudioPlayerTest_Offset_Byte(obj)

    def step(self, obj) -> int:
        return self.c.AlxAudioPlayerTest_Step_Byte(obj)

    def left(self, obj) -> float:
        return self.c.AlxAudioPlayer_GetSampleL(self._p(obj))

    def right(self, obj) -> float:
        return self.c.AlxAudioPlayer_GetSampleR(self._p(obj))

    def mono(self, obj) -> float:
        return self.c.AlxAudioPlayer_GetSampleMono(self._p(obj))

    def advance(self, obj, times: int = 1) -> None:
        for _ in range(times):
            self.c.AlxAudioPlayer_IncSampleOffset(self._p(obj))

    def play(self, obj) -> None:
        self.c.AlxAudioPlayer_Play(self._p(obj))

    def stop(self, obj) -> None:
        self.c.AlxAudioPlayer_Stop(self._p(obj))

    def pause(self, obj) -> None:
        self.c.AlxAudioPlayer_Pause(self._p(obj))

    def replay(self, obj) -> None:
        self.c.AlxAudioPlayer_Replay(self._p(obj))

    def loop(self, obj, *, on: bool) -> None:
        self.c.AlxAudioPlayer_LoopConfig(self._p(obj), on)

    def is_playing(self, obj) -> bool:
        return self.c.AlxAudioPlayer_IsPlaying(self._p(obj))

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxAudioPlayerTest_Delete(handle)
        self._handles.clear()


class AudioLib:
    """ctypes wrapper around alxAudioTest.dll: PCM samples to floats and back.

    Seven free functions and no object, so this is the whole module.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        f, i8, u8, i16, u16 = (ctypes.c_float, ctypes.c_int8, ctypes.c_uint8,
                               ctypes.c_int16, ctypes.c_uint16)
        for name, arg in (("Int8", i8), ("Uint8", u8), ("Int16", i16), ("Uint16", u16)):
            fn = getattr(c, f"AlxAudio_LinerPcm{name}ToFloat")
            fn.restype = f
            fn.argtypes = [arg]
        c.AlxAudio_FloatToLinerPcmInt8.restype = i8
        c.AlxAudio_FloatToLinerPcmInt8.argtypes = [f]
        c.AlxAudio_FloatToLinerPcmInt16.restype = i16
        c.AlxAudio_FloatToLinerPcmInt16.argtypes = [f]
        c.AlxAudio_StereoToMono.restype = f
        c.AlxAudio_StereoToMono.argtypes = [f, f]

    def int8_to_float(self, sample: int) -> float:
        return self.c.AlxAudio_LinerPcmInt8ToFloat(sample)

    def uint8_to_float(self, sample: int) -> float:
        return self.c.AlxAudio_LinerPcmUint8ToFloat(sample)

    def int16_to_float(self, sample: int) -> float:
        return self.c.AlxAudio_LinerPcmInt16ToFloat(sample)

    def uint16_to_float(self, sample: int) -> float:
        return self.c.AlxAudio_LinerPcmUint16ToFloat(sample)

    def float_to_int8(self, sample: float) -> int:
        return self.c.AlxAudio_FloatToLinerPcmInt8(sample)

    def float_to_int16(self, sample: float) -> int:
        return self.c.AlxAudio_FloatToLinerPcmInt16(sample)

    def stereo_to_mono(self, left: float, right: float) -> float:
        return self.c.AlxAudio_StereoToMono(left, right)


class PwrLib:
    """ctypes wrapper around alxPwrTest.dll: one converter voltage in, one yes-or-no out.

    The clock belongs to the test, as it does for every group with a timer in it: nothing here
    moves until `advance_ms` says so.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, i32, u64, b = (ctypes.c_void_p, ctypes.c_float, ctypes.c_int32,
                              ctypes.c_uint64, ctypes.c_bool)
        c.AlxPwrTest_New.restype = vp
        c.AlxPwrTest_New.argtypes = [f, f, f, f, f, f, b, f, f]
        c.AlxPwrTest_Delete.argtypes = [vp]
        c.AlxPwr_Process.restype = b
        c.AlxPwr_Process.argtypes = [vp, f]
        c.AlxPwrTest_Val_V.restype = f
        c.AlxPwrTest_Val_V.argtypes = [vp]
        c.AlxPwrTest_HysSt.restype = i32
        c.AlxPwrTest_HysSt.argtypes = [vp]
        c.AlxPwrTest_IsInRangeRaw.restype = b
        c.AlxPwrTest_IsInRangeRaw.argtypes = [vp]
        for name in ("Top", "Mid", "Bot"):
            getattr(c, f"AlxPwrTest_HysSt_{name}").restype = i32
        c.AlxTick_Ctor.argtypes = [vp]
        c.AlxTick_IncRange_ns.argtypes = [vp, u64]
        self.TOP = c.AlxPwrTest_HysSt_Top()
        self.MID = c.AlxPwrTest_HysSt_Mid()
        self.BOT = c.AlxPwrTest_HysSt_Bot()
        self.tick = ctypes.addressof(ctypes.c_uint8.in_dll(c, "alxTick"))
        self._handles: list = []

    # -- the clock the test owns ----------------------------------------------
    def tick_reset(self) -> None:
        self.c.AlxTick_Ctor(self.tick)
        self.c.AlxIrqFake_Reset()

    def advance_ms(self, ms: float) -> None:
        self.c.AlxTick_IncRange_ns(self.tick, round(ms * 1_000_000))

    # -- one supervisor -------------------------------------------------------
    def new(self, *, res_high=0.0, res_low=1.0, top_high=0.0, top_low=0.0,
            bot_high=0.0, bot_low=0.0, initial=False, stable_true_ms=0.0,
            stable_false_ms=0.0):
        """A supervisor: a divider, a window with hysteresis on both edges, and a glitch filter."""
        handle = self.c.AlxPwrTest_New(res_high, res_low, top_high, top_low, bot_high, bot_low,
                                       initial, stable_true_ms, stable_false_ms)
        assert handle, "AlxPwrTest_New returned NULL"
        self._handles.append(handle)
        return handle

    def process(self, obj, adc_V: float) -> bool:  # noqa: N803 - the unit belongs in the name
        return self.c.AlxPwr_Process(obj, adc_V)

    def val_V(self, obj) -> float:
        """The supply voltage the supervisor reconstructed from the converter reading."""
        return self.c.AlxPwrTest_Val_V(obj)

    def hys_state(self, obj) -> int:
        return self.c.AlxPwrTest_HysSt(obj)

    def in_range_raw(self, obj) -> bool:
        """In range BEFORE the glitch filter - what tells "out of range" from "not yet stable"."""
        return self.c.AlxPwrTest_IsInRangeRaw(obj)

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxPwrTest_Delete(handle)
        self._handles.clear()


class TempSensLib:
    """ctypes wrapper around alxTempSensTest.dll: a voltage divider with an RTD in it.

    The helper owns the converter, the interpolation table and the points it interpolates, so a
    test builds a sensor out of a table and two channel numbers and then drives the converter.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f, u32, i32, b = (ctypes.c_void_p, ctypes.c_float, ctypes.c_uint32,
                              ctypes.c_int32, ctypes.c_bool)
        c.AlxTempSensTest_New.restype = vp
        c.AlxTempSensTest_New.argtypes = [ctypes.POINTER(f), ctypes.POINTER(f), u32, b,
                                          u32, u32, b, f]
        c.AlxTempSensTest_Delete.argtypes = [vp]
        c.AlxTempSensTest_Sens.restype = vp
        c.AlxTempSensTest_Sens.argtypes = [vp]
        c.AlxTempSensTest_Adc.restype = vp
        c.AlxTempSensTest_Adc.argtypes = [vp]
        c.AlxTempSensRtdVdiv_GetTemp_degC.restype = i32
        c.AlxTempSensRtdVdiv_GetTemp_degC.argtypes = [vp, ctypes.POINTER(f)]
        for name in ("Init", "DeInit"):
            fn = getattr(c, f"AlxTempSensRtdVdiv_{name}")
            fn.restype = i32
            fn.argtypes = [vp]
        c.AlxAdcFake_SetVoltage_V.argtypes = [vp, u32, f]
        c.AlxAdcFake_ReadCount.restype = u32
        c.AlxAdcFake_ReadCount.argtypes = [vp, u32]
        for name in ("Ok", "ErrMin", "ErrMax"):
            getattr(c, f"AlxTempSensTest_Status_{name}").restype = i32
        self.OK = c.AlxTempSensTest_Status_Ok()
        self.ERR_MIN = c.AlxTempSensTest_Status_ErrMin()
        self.ERR_MAX = c.AlxTempSensTest_Status_ErrMax()
        self._handles: list = []

    def new(
        self,
        res_points_kOhm,  # noqa: N803 - the unit belongs in the name
        temp_points_degC,  # noqa: N803
        *,
        rising=True,
        ch_vin=0,
        ch_vout=1,
        rtd_low=True,
        res_other_kOhm=1.0,  # noqa: N803
    ):
        """A sensor over a table of (resistance, temperature) points; released when the test ends."""
        count = len(res_points_kOhm)
        assert count == len(temp_points_degC), "a table needs one temperature per resistance"
        arr = ctypes.c_float * count
        handle = self.c.AlxTempSensTest_New(arr(*res_points_kOhm), arr(*temp_points_degC), count,
                                            rising, ch_vin, ch_vout, rtd_low, res_other_kOhm)
        assert handle, "AlxTempSensTest_New returned NULL"
        self._handles.append(handle)
        return handle

    def set_voltage_V(self, obj, ch: int, voltage_V: float) -> None:  # noqa: N803
        self.c.AlxAdcFake_SetVoltage_V(self.c.AlxTempSensTest_Adc(obj), ch, voltage_V)

    def read_count(self, obj, ch: int) -> int:
        return self.c.AlxAdcFake_ReadCount(self.c.AlxTempSensTest_Adc(obj), ch)

    def lifecycle(self, obj, what: str) -> int:
        """Init or DeInit the sensor - the two calls a product makes around everything else."""
        return getattr(self.c, f"AlxTempSensRtdVdiv_{what}")(self.c.AlxTempSensTest_Sens(obj))

    def asserts(self) -> int:
        """How many of the library's own assertions have failed since this test began."""
        return self.c.AlxAssertPc_Count()

    def temp(self, obj) -> tuple[int, float]:
        """(status, temperature) - the status is the table's, passed straight through."""
        out = ctypes.c_float()
        status = self.c.AlxTempSensRtdVdiv_GetTemp_degC(self.c.AlxTempSensTest_Sens(obj),
                                                        ctypes.byref(out))
        return status, out.value

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxTempSensTest_Delete(handle)
        self._handles.clear()


class RotSwLib:
    """ctypes wrapper around alxRotSwTest.dll: a rotary switch read through faked IO pins.

    The helper owns the pins and the array the module keeps a pointer to, so a test asks for pin
    `i` and drives it; nothing here mirrors a struct.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u8, u32, i32, b = (ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint32,
                               ctypes.c_int32, ctypes.c_bool)
        c.AlxRotSwTest_New.restype = vp
        c.AlxRotSwTest_New.argtypes = [u8, i32]
        c.AlxRotSwTest_Delete.argtypes = [vp]
        c.AlxRotSwTest_RotSw.restype = vp
        c.AlxRotSwTest_RotSw.argtypes = [vp]
        c.AlxRotSwTest_Pin.restype = vp
        c.AlxRotSwTest_Pin.argtypes = [vp, u8]
        for name in ("Real", "Complement", "Gray"):
            getattr(c, f"AlxRotSwTest_CodeType_{name}").restype = i32
        c.AlxRotSw_Init.argtypes = [vp]
        c.AlxRotSw_DeInit.argtypes = [vp]
        c.AlxRotSw_GetCode.restype = u32
        c.AlxRotSw_GetCode.argtypes = [vp]
        c.AlxIoPinFake_SetLevel.argtypes = [vp, b]
        c.AlxIoPinFake_InitCount.restype = u32
        c.AlxIoPinFake_InitCount.argtypes = [vp]
        self.REAL = c.AlxRotSwTest_CodeType_Real()
        self.COMPLEMENT = c.AlxRotSwTest_CodeType_Complement()
        self.GRAY = c.AlxRotSwTest_CodeType_Gray()
        self._handles: list = []

    def new(self, num_of_pins: int, code_type: int | None = None):
        """A switch with `num_of_pins` pins; released when the test ends."""
        handle = self.c.AlxRotSwTest_New(num_of_pins,
                                         self.REAL if code_type is None else code_type)
        assert handle, "AlxRotSwTest_New returned NULL"
        self._handles.append(handle)
        return handle

    def rotsw(self, obj):
        return self.c.AlxRotSwTest_RotSw(obj)

    def pin(self, obj, index: int):
        return self.c.AlxRotSwTest_Pin(obj, index)

    def set_pins(self, obj, code: int, num_of_pins: int) -> None:
        """Drive the pins so that they spell `code` in binary, pin i being bit i."""
        for i in range(num_of_pins):
            self.c.AlxIoPinFake_SetLevel(self.pin(obj, i), bool(code >> i & 1))

    def init(self, obj) -> None:
        self.c.AlxRotSw_Init(self.rotsw(obj))

    def deinit(self, obj) -> None:
        self.c.AlxRotSw_DeInit(self.rotsw(obj))

    def code(self, obj) -> int:
        return self.c.AlxRotSw_GetCode(self.rotsw(obj))

    def init_count(self, obj, index: int) -> int:
        return self.c.AlxIoPinFake_InitCount(self.pin(obj, index))

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxRotSwTest_Delete(handle)
        self._handles.clear()


class AudioVolLib:
    """ctypes wrapper around the mapping DLL's audio volume: percent to decibels to a factor.

    It shares alxLinFunTest.dll with AlxLinFun because that is what the module is built on - the
    percentage-to-decibel line IS an AlxLinFun, constructed inside AlxAudioVol_Ctor.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, f = ctypes.c_void_p, ctypes.c_float
        c.AlxAudioVolTest_New.restype = vp
        c.AlxAudioVolTest_New.argtypes = [f, f]
        c.AlxAudioVolTest_Delete.argtypes = [vp]
        c.AlxAudioVolTest_PctMax.restype = f
        c.AlxAudioVolTest_PctMax.argtypes = [vp]
        c.AlxAudioVol_Process.restype = f
        c.AlxAudioVol_Process.argtypes = [vp, f]
        c.AlxAudioVol_Set_pct.argtypes = [vp, f]
        c.AlxAudioVol_Set_dB.argtypes = [vp, f]
        self._handles: list = []

    def new(self, vol_min_dB: float, vol_max_dB: float):  # noqa: N803 - the unit belongs in the name
        """A volume control over a decibel range; released when the test ends."""
        handle = self.c.AlxAudioVolTest_New(vol_min_dB, vol_max_dB)
        assert handle, "AlxAudioVolTest_New returned NULL"
        self._handles.append(handle)
        return handle

    def process(self, obj, sample: float) -> float:
        return self.c.AlxAudioVol_Process(obj, sample)

    def set_pct(self, obj, vol_pct: float) -> None:
        self.c.AlxAudioVol_Set_pct(obj, vol_pct)

    def set_dB(self, obj, vol_dB: float) -> None:  # noqa: N803 - the unit belongs in the name
        self.c.AlxAudioVol_Set_dB(obj, vol_dB)

    def pct_max(self, obj) -> float:
        """The highest percentage the object will accept, as its constructor computed it."""
        return self.c.AlxAudioVolTest_PctMax(obj)

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxAudioVolTest_Delete(handle)
        self._handles.clear()


def _assert_pins_fitted(lib) -> None:
    """Every pin a test used had a slot of its own in the IO pin fake.

    The fake tells pins apart by address and has a fixed number of slots; past that it folds the
    extras onto the last one, so two pins answer each other's level and a perfectly correct module
    measures wrong. Checked after each test rather than before, because it is the test's own pins
    that overflow it.
    """
    lib.c.AlxIoPinFake_DidOverflow.restype = ctypes.c_bool
    assert not lib.c.AlxIoPinFake_DidOverflow(), (
        "more pins than the IO pin fake has slots - raise ALX_IO_PIN_FAKE_NUM_OF_PINS in "
        "Test/alxIoPinFake.c; everything this test measured about a pin is unreliable"
    )


class IdLib:
    """ctypes wrapper around alxIdTest.dll: the identity block a product prints at boot.

    The object is 1464 bytes of nested structs and one constructor takes a hardware instance BY
    VALUE, so nothing here is built from Python - ``alloc`` hands back an opaque handle and the
    helpers in alxIdTestHelpers.c do the filling. Construction is deliberately two phase: alloc,
    then fill the arrays the constructor will be given, then a ctor. The arrays are INPUTS and the
    module reads them at Init, so a test that wants a board the firmware does not know has to be
    able to say so first.

    The poison byte is passed rather than assumed. 0xFF is the house default and it is what makes a
    field the constructor forgot readable as garbage instead of as a plausible zero; a test that
    wants the counterpart passes 0.

    What the DLL was compiled with is read back through ``build``, never restated here -
    alxBuild_GENERATED.h is rewritten on every firmware build, so a hardcoded hash is a test that
    goes red on somebody else's commit.
    """

    POISON = 0xFF

    STR_GETTERS: ClassVar[tuple[str, ...]] = (
        "FwArtf", "FwName", "FwVerStr", "FwBinStr",
        "FwBootArtf", "FwBootName", "FwBootVerStr", "FwBootBinStr",
        "HwPcbArtf", "HwPcbName", "HwPcbVerStr",
        "HwBomArtf", "HwBomName", "HwBomVerStr", "HwMcuUniqueIdStr",
    )
    U8_GETTERS: ClassVar[tuple[str, ...]] = (
        "FwVerMajor", "FwVerMinor", "FwVerPatch",
        "FwBootVerMajor", "FwBootVerMinor", "FwBootVerPatch",
        "HwPcbVerMajor", "HwPcbVerMinor", "HwPcbVerPatch",
        "HwBomVerMajor", "HwBomVerMinor", "HwBomVerPatch",
        "HwId",
    )
    U32_GETTERS: ClassVar[tuple[str, ...]] = (
        "FwVerDate", "FwHashShort", "FwBootVerDate", "FwBootHashShort",
        "HwPcbVerDate", "HwBomVerDate",
    )
    U64_GETTERS: ClassVar[tuple[str, ...]] = ("FwVer", "FwBootVer", "HwPcbVer", "HwBomVer")
    BOOL_GETTERS: ClassVar[tuple[str, ...]] = ("FwIsBootUsed",)

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        vp, u8, u16, u32, u64, b, cp = (ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16,
                                        ctypes.c_uint32, ctypes.c_uint64, ctypes.c_bool,
                                        ctypes.c_char_p)
        c.AlxIdTest_Alloc.restype = vp
        c.AlxIdTest_Alloc.argtypes = [u8]
        c.AlxIdTest_Delete.argtypes = [vp]
        c.AlxIdTest_Pin.restype = vp
        c.AlxIdTest_Pin.argtypes = [vp, u8]
        c.AlxIdTest_Instance.restype = vp
        c.AlxIdTest_Instance.argtypes = [vp]
        c.AlxIdTest_Known.restype = vp
        c.AlxIdTest_Known.argtypes = [vp, u8]
        c.AlxIdTest_SetSupported.argtypes = [vp, u8, u8]
        c.AlxIdTest_InstanceSetPcb.argtypes = [vp, u8, cp, cp, u8, u8, u8, u32]
        c.AlxIdTest_InstanceSetBom.argtypes = [vp, cp, cp, u8, u8, u8, u32]
        c.AlxIdTest_Ctor.argtypes = [vp, cp, cp, u8, u8, u8, b, u32, b, u32, u8, u8, u8, cp]
        # what the product reports about the toolchain that built it
        c.AlxIdTest_FwCompName.restype = cp
        c.AlxIdTest_FwCompName.argtypes = [vp]
        c.AlxIdTest_FwCompVerMajor.restype = u32
        c.AlxIdTest_FwCompVerMajor.argtypes = [vp]
        c.AlxIdTest_FwLangCVer.restype = u32
        c.AlxIdTest_FwLangCVer.argtypes = [vp]
        c.AlxIdTest_FwLangCLibName.restype = cp
        c.AlxIdTest_FwLangCLibName.argtypes = [vp]
        c.AlxIdTest_CtorNoHwId.argtypes = [vp, cp, cp, u8, u8, u8, b, u32, b, u32, cp]
        for name in ("IsInit", "IsHwIdUsed"):
            fn = getattr(c, f"AlxIdTest_{name}")
            fn.restype = b
            fn.argtypes = [vp]
        for name in ("IdIoPinState", "CalcHwId"):
            fn = getattr(c, f"AlxIdTest_{name}")
            fn.restype = u8
            fn.argtypes = [vp, u8]
        for name in ("HiZ", "Hi", "Lo", "Undefined"):
            getattr(c, f"AlxIdTest_TriState_{name}").restype = u8
        c.AlxIdTest_FlashAlloc.restype = u32
        c.AlxIdTest_FlashFree.argtypes = [u32]
        c.AlxIdTest_FlashFill.argtypes = [u32, u8, u32]
        c.AlxIdTest_BootBlobWrite.argtypes = [u32, u32, u32, cp, cp, u8, u8, u8, b, u32, u32,
                                              cp, cp, cp, u32, u16]
        c.AlxIdTest_BootBlobFillField.restype = b
        c.AlxIdTest_BootBlobFillField.argtypes = [u32, cp, u8, u32]
        for name in ("BootBlobLen", "BootBlobMagicNum", "BootBlobVer",
                     "BuildDate", "BuildNum", "BuildRev", "BuildHashShortUint32", "BuildDateComp"):
            getattr(c, f"AlxIdTest_{name}").restype = u32
        for name in ("BuildName", "BuildHash", "BuildHashShort", "CompDate", "CompTime"):
            getattr(c, f"AlxIdTest_{name}").restype = cp
        c.AlxId_Init.argtypes = [vp]
        c.AlxId_Trace.argtypes = [vp]
        for names, restype in ((self.STR_GETTERS, cp), (self.U8_GETTERS, u8),
                               (self.U32_GETTERS, u32), (self.U64_GETTERS, u64),
                               (self.BOOL_GETTERS, b)):
            for name in names:
                fn = getattr(c, f"AlxId_Get{name}")
                fn.restype = restype
                fn.argtypes = [vp]
        # The two array variants of the MCU unique id are [out] parameters rather than returns, so
        # they are declared by hand instead of through the getter tables above.
        c.AlxId_GetHwMcuUniqueIdUint32.argtypes = [vp, ctypes.POINTER(u32), u8]
        c.AlxId_GetHwMcuUniqueIdUint8.argtypes = [vp, ctypes.POINTER(u8), u8]
        c.AlxIoPinFake_SetTriState.argtypes = [vp, ctypes.c_int]
        for name in ("InitCount", "DeInitCount"):
            fn = getattr(c, f"AlxIoPinFake_{name}")
            fn.restype = u32
            fn.argtypes = [vp]

        self.HI_Z = c.AlxIdTest_TriState_HiZ()
        self.HI = c.AlxIdTest_TriState_Hi()
        self.LO = c.AlxIdTest_TriState_Lo()
        self.UNDEFINED = c.AlxIdTest_TriState_Undefined()
        self.MAGIC_NUM = c.AlxIdTest_BootBlobMagicNum()
        self.BOOT_ID_VER = c.AlxIdTest_BootBlobVer()
        self.BOOT_BLOB_LEN = c.AlxIdTest_BootBlobLen()
        self._handles: list = []
        self._pages: list = []

    # -- the object ------------------------------------------------------------
    def alloc(self, poison: int = POISON) -> int:
        """An AlxId filled with ``poison`` and nothing else done to it."""
        handle = self.c.AlxIdTest_Alloc(poison)
        assert handle, "the identity helper could not allocate"
        self._handles.append(handle)
        return handle

    def instance(self, obj: int) -> int:
        """The hardware instance AlxIdTest_CtorNoHwId will copy - fill it BEFORE the ctor."""
        return self.c.AlxIdTest_Instance(obj)

    def known(self, obj: int, i: int) -> int:
        """One entry of the known instance array - fill it any time before Init."""
        return self.c.AlxIdTest_Known(obj, i)

    def set_supported(self, obj: int, i: int, hw_id: int) -> None:
        self.c.AlxIdTest_SetSupported(obj, i, hw_id)

    def set_pcb(self, inst: int, *, hw_id: int = 0, artf: str = "PcbArtf", name: str = "PcbName",
                ver: tuple = (0, 0, 0), date: int = 0) -> None:
        self.c.AlxIdTest_InstanceSetPcb(inst, hw_id, artf.encode("ascii"), name.encode("ascii"),
                                        ver[0], ver[1], ver[2], date)

    def set_bom(self, inst: int, *, artf: str = "BomArtf", name: str = "BomName",
                ver: tuple = (0, 0, 0), date: int = 0) -> None:
        self.c.AlxIdTest_InstanceSetBom(inst, artf.encode("ascii"), name.encode("ascii"),
                                        ver[0], ver[1], ver[2], date)

    def ctor(self, obj: int, *, artf: str = "FwArtf", name: str = "FwName",
             ver: tuple = (0, 0, 0), is_build_job_used: bool = False, date_comp: int = 0,
             is_boot_used: bool = False, boot_addr: int = 0, known_len: int = 1,
             supported_len: int = 1, pin_len: int = 2, mcu: str = "McuName") -> None:
        """The full constructor: the hardware id is read off the pins this object owns."""
        self.c.AlxIdTest_Ctor(obj, artf.encode("ascii"), name.encode("ascii"),
                              ver[0], ver[1], ver[2], is_build_job_used, date_comp,
                              is_boot_used, boot_addr, known_len, supported_len, pin_len,
                              mcu.encode("ascii"))

    def ctor_no_hw_id(self, obj: int, *, artf: str = "FwArtf", name: str = "FwName",
                      ver: tuple = (0, 0, 0), is_build_job_used: bool = False, date_comp: int = 0,
                      is_boot_used: bool = False, boot_addr: int = 0,
                      mcu: str = "McuName") -> None:
        """The constructor a board with one hardware variant uses: no pins are read at all."""
        self.c.AlxIdTest_CtorNoHwId(obj, artf.encode("ascii"), name.encode("ascii"),
                                    ver[0], ver[1], ver[2], is_build_job_used, date_comp,
                                    is_boot_used, boot_addr, mcu.encode("ascii"))

    def init(self, obj: int) -> None:
        self.c.AlxId_Init(obj)

    def trace(self, obj: int) -> None:
        self.c.AlxId_Trace(obj)

    def is_init(self, obj: int) -> bool:
        return self.c.AlxIdTest_IsInit(obj)

    def is_hw_id_used(self, obj: int) -> bool:
        return self.c.AlxIdTest_IsHwIdUsed(obj)

    # -- the getters, under the name they carry in the library ------------------
    def text(self, obj: int, name: str) -> str:
        """One of the string getters, e.g. ``text(obj, "FwVerStr")``."""
        assert name in self.STR_GETTERS, f"not a string getter: {name}"
        return (getattr(self.c, f"AlxId_Get{name}")(obj) or b"").decode("ascii", "replace")

    def num(self, obj: int, name: str) -> int:
        """One of the numeric or boolean getters, e.g. ``num(obj, "FwVerMajor")``."""
        known = self.U8_GETTERS + self.U32_GETTERS + self.U64_GETTERS + self.BOOL_GETTERS
        assert name in known, f"not a numeric getter: {name}"
        return getattr(self.c, f"AlxId_Get{name}")(obj)

    def mcu_unique_id(self, obj: int, width: int, length: int) -> list:
        """One of the two array getters, over a buffer pre-filled with 0xA5 so a no-op is visible.

        They take the caller's buffer as an [out] parameter and a `len` they hand straight to
        memcpy as a BYTE count, whichever unit their name promises - so the pre-fill is what says
        whether anything was written at all.
        """
        if width == 32:
            buf32 = (ctypes.c_uint32 * length)(*([0xA5A5A5A5] * length))
            self.c.AlxId_GetHwMcuUniqueIdUint32(obj, buf32, length)
            return list(buf32)
        buf8 = (ctypes.c_uint8 * length)(*([0xA5] * length))
        self.c.AlxId_GetHwMcuUniqueIdUint8(obj, buf8, length)
        return list(buf8)

    # -- the identification straps ---------------------------------------------
    def pin(self, obj: int, i: int) -> int:
        return self.c.AlxIdTest_Pin(obj, i)

    def strap(self, obj: int, i: int, state: int) -> None:
        """Tie one identification pin high or low, or leave it floating."""
        self.c.AlxIoPinFake_SetTriState(self.pin(obj, i), state)

    def pin_state(self, obj: int, i: int) -> int:
        """What the module recorded for strap ``i`` while it worked out the hardware id."""
        return self.c.AlxIdTest_IdIoPinState(obj, i)

    def calc_hw_id(self, obj: int, pin_len: int) -> int:
        """AlxId_CalcHwId on its own, without an Init around it."""
        return self.c.AlxIdTest_CalcHwId(obj, pin_len)

    def pin_counts(self, obj: int, i: int) -> tuple:
        """How many times strap ``i`` was initialised and deinitialised."""
        return (self.c.AlxIoPinFake_InitCount(self.pin(obj, i)),
                self.c.AlxIoPinFake_DeInitCount(self.pin(obj, i)))

    # -- the bootloader's block, at an address a uint32_t can hold --------------
    def flash(self) -> int:
        """A page below 4 GB, because the module casts its uint32_t address straight to void*."""
        addr = self.c.AlxIdTest_FlashAlloc()
        assert addr, "the low page the boot id needs is already taken"
        self._pages.append(addr)
        return addr

    def flash_fill(self, addr: int, byte: int, length: int | None = None) -> None:
        """Whole block filled with one byte - 0xFF is a device that was never programmed."""
        self.c.AlxIdTest_FlashFill(addr, byte, self.BOOT_BLOB_LEN if length is None else length)

    def boot_blob(self, addr: int, *, magic_num: int | None = None, ver: int | None = None,
                  artf: str = "BootArtf", name: str = "BootName", fw_ver: tuple = (0, 0, 0),
                  is_build_job_used: bool = False, build_date: int = 0, build_date_comp: int = 0,
                  build_name: str = "BuildName", hash_: str = "0" * 40, hash_short: str = "0" * 7,
                  hash_short_uint32: int = 0, crc: int = 0) -> None:
        """Write a bootloader identity block where the constructor will go looking for one."""
        self.c.AlxIdTest_BootBlobWrite(
            addr,
            self.MAGIC_NUM if magic_num is None else magic_num,
            self.BOOT_ID_VER if ver is None else ver,
            artf.encode("ascii"), name.encode("ascii"),
            fw_ver[0], fw_ver[1], fw_ver[2], is_build_job_used, build_date, build_date_comp,
            build_name.encode("ascii"), hash_.encode("ascii"), hash_short.encode("ascii"),
            hash_short_uint32, crc)

    def boot_blob_fill(self, addr: int, field: str, byte: int, length: int) -> None:
        """Overwrite one named field of the block with a repeated byte and no terminator."""
        written = self.c.AlxIdTest_BootBlobFillField(addr, field.encode("ascii"), byte, length)
        assert written, f"the boot id block has no field called {field}"

    # -- what this DLL was compiled with ---------------------------------------
    def build(self, name: str):
        """One ALX_BUILD_* macro as the compiler saw it, e.g. ``build("HashShort")``."""
        value = getattr(self.c, f"AlxIdTest_Build{name}")()
        return value.decode("ascii") if isinstance(value, bytes) else value

    def comp(self, name: str) -> str:
        """``__DATE__`` or ``__TIME__`` of this DLL - what ALX_BUILD_DATE_COMP is derived from."""
        return getattr(self.c, f"AlxIdTest_Comp{name}")().decode("ascii")

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxIdTest_Delete(handle)
        self._handles.clear()
        for page in self._pages:
            self.c.AlxIdTest_FlashFree(page)
        self._pages.clear()


class AssertWeakLib:
    """ctypes wrapper around alxAssertWeakTest.dll: alxAssert.c's own weak defaults, undisplaced.

    The only image in the suite where the library's three handler bodies run. Two of the three are
    reachable from here - AlxAssert_Rst, whose body is empty and returns, and AlxAssert_Trace, whose
    body writes through AlxTrace_WriteLevel. The third, AlxAssert_Bkpt, runs ALX_BKPT() and does not
    return on this host, so nothing in this DLL can call it and no caller for it is exported.

    Deliberately NOT registered with _register_lib: it links no alxAssertPc.c, so it exports none of
    the AlxAssertPc_* symbols that fixture binds. Nothing is lost by that - alxAssert.c has no
    assertion of its own for the autouse check to find.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        u8, u32, b = ctypes.c_uint8, ctypes.c_uint32, ctypes.c_bool
        c.AlxAssertTest_TraceCount.restype = u32
        c.AlxAssertTest_TraceLevel.restype = u8
        c.AlxAssertTest_TraceText.restype = ctypes.c_char_p
        c.AlxAssertTest_TraceFile.restype = ctypes.c_char_p
        c.AlxAssertTest_TraceFun.restype = ctypes.c_char_p
        c.AlxAssertTest_TraceLine.restype = u32
        c.AlxAssertTest_TraceLevelFtl.restype = u8
        c.AlxAssertTest_TraceLevelConfigured.restype = u8
        c.AlxAssertTest_TraceLevelOff.restype = u8
        c.AlxAssertTest_File.restype = ctypes.c_char_p
        c.AlxAssertTest_SideEffects.restype = u32
        c.AlxAssertTest_SideEffectsOff.restype = u32
        c.AlxAssertTest_LineRst.restype = u32
        c.AlxAssertTest_LineTrace.restype = u32
        c.AlxAssertTest_ReachedAfterRst.restype = b
        c.AlxAssertTest_ReachedAfterTrace.restype = b
        c.AlxAssertTest_CallRstDirect.restype = b
        c.AlxAssertTest_CallTraceDirect.restype = b
        for name in ("DriveRst", "DriveTrace"):
            getattr(c, f"AlxAssertTest_{name}").argtypes = [b]
        self.FTL = c.AlxAssertTest_TraceLevelFtl()
        self.OFF = c.AlxAssertTest_TraceLevelOff()

    # -- the recorder the library's weak AlxAssert_Trace writes to ---------
    def reset(self) -> None:
        self.c.AlxAssertTest_Reset()

    def traces(self) -> int:
        return self.c.AlxAssertTest_TraceCount()

    def trace_level(self) -> int:
        return self.c.AlxAssertTest_TraceLevel()

    def trace_text(self) -> str:
        return (self.c.AlxAssertTest_TraceText() or b"").decode("ascii", "replace")

    def trace_file(self) -> str:
        """The `file` the handler was passed - the CALLER's constant, not alxAssert.c's name."""
        return (self.c.AlxAssertTest_TraceFile() or b"").decode("ascii", "replace")

    def trace_fun(self) -> str:
        """The `fun` the handler was passed - __func__ expanded at the CALL SITE."""
        return (self.c.AlxAssertTest_TraceFun() or b"").decode("ascii", "replace")

    def trace_line(self) -> int:
        """The `line` the handler was passed - __LINE__ expanded at the CALL SITE."""
        return self.c.AlxAssertTest_TraceLine()

    def level_ftl(self) -> int:
        """ALX_TRACE_LEVEL_FTL as the DLL was compiled with it."""
        return self.c.AlxAssertTest_TraceLevelFtl()

    def level_configured(self) -> int:
        """ALX_TRACE_LEVEL this DLL was built at - Test/alxConfig.h sets it OFF."""
        return self.c.AlxAssertTest_TraceLevelConfigured()

    # -- the call sites ----------------------------------------------------
    def file(self) -> str:
        return self.c.AlxAssertTest_File().decode("ascii")

    def drive(self, kind: str, expr: bool) -> None:
        """Drive one ALX_ASSERT_<kind> macro with ``expr`` as its expression."""
        getattr(self.c, f"AlxAssertTest_Drive{kind}")(expr)

    def line(self, kind: str) -> int:
        """The line the ``kind`` macro sits on, as the DLL recorded it - never a copy in the test."""
        return getattr(self.c, f"AlxAssertTest_Line{kind}")()

    def reached_after(self, kind: str) -> bool:
        """Did the statement AFTER the ``kind`` macro run - the whole question of a failed assertion."""
        return bool(getattr(self.c, f"AlxAssertTest_ReachedAfter{kind}")())

    def side_effects(self) -> int:
        """How often the probe inside an ENABLED assert expression ran."""
        return self.c.AlxAssertTest_SideEffects()

    def side_effects_off(self) -> int:
        """How often the probe inside the DISABLED expansion ran - the expression is discarded, so never."""
        return self.c.AlxAssertTest_SideEffectsOff()

    def drive_side_effect(self) -> None:
        self.c.AlxAssertTest_DriveRstSideEffect()

    def drive_side_effect_off(self) -> None:
        self.c.AlxAssertTest_DriveOffSideEffect()

    def bump_off_direct(self) -> None:
        """Run the disabled form's probe by hand, to show the counter it does not move is a live one."""
        self.c.AlxAssertTest_BumpOffDirect()

    def call_direct(self, kind: str) -> bool:
        """Call AlxAssert_<kind> directly; True = execution continued past the call."""
        return bool(getattr(self.c, f"AlxAssertTest_Call{kind}Direct")())


class AssertLib(AssertWeakLib):
    """ctypes wrapper around alxAssertTest.dll: the same weak defaults, displaced by alxAssertPc.c.

    Same surface as AssertWeakLib plus the BKPT call sites, which are safe only here, and the
    AlxAssertPc_* recorder - so a test can say WHICH definition ran, which is the whole comparison.
    """

    def __init__(self, dll_path: Path):
        super().__init__(dll_path)
        c = self.c
        _register_lib(c)
        c.AlxAssertTest_LineBkpt.restype = ctypes.c_uint32
        c.AlxAssertTest_FileBkpt.restype = ctypes.c_char_p
        c.AlxAssertTest_ReachedAfterBkpt.restype = ctypes.c_bool
        c.AlxAssertTest_CallBkptDirect.restype = ctypes.c_bool
        c.AlxAssertTest_DriveBkpt.argtypes = [ctypes.c_bool]

    def file_bkpt(self) -> str:
        return self.c.AlxAssertTest_FileBkpt().decode("ascii")

    def asserts(self) -> int:
        return self.c.AlxAssertPc_Count()

    def first(self) -> str:
        return (self.c.AlxAssertPc_First() or b"").decode("ascii", "replace")

    def assert_reset(self) -> None:
        self.c.AlxAssertPc_Reset()


class MemRawLib:
    """ctypes wrapper around alxMemRawTest.dll: alxMemRaw.c's five weak defaults, undisplaced.

    Construction is poisoned with 0xFF first, so the two fields the header declares read back as
    garbage rather than zeros when the constructor does not write them. The data buffer belongs to
    the helper and is read byte by byte, because what a raw read has to answer is whether it wrote
    anything at all.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        _register_lib(c)
        self._handles: list = []
        u8, u16, u32, i32, b = (ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint32,
                                ctypes.c_int32, ctypes.c_bool)
        vp = ctypes.c_void_p
        c.AlxMemRawTest_New.restype = vp
        c.AlxMemRawTest_NewNoCtor.restype = vp
        c.AlxMemRawTest_Delete.argtypes = [vp]
        for name in ("WasCtorCalled", "IsInit"):
            fn = getattr(c, f"AlxMemRawTest_{name}")
            fn.restype = u8      # the BYTE in the field, never loaded as bool - see the helper
            fn.argtypes = [vp]
        c.AlxMemRawTest_SizeOf.restype = u32
        c.AlxMemRawTest_PoisonByte.restype = u8
        for name in ("Init", "DeInit"):
            fn = getattr(c, f"AlxMemRawTest_{name}")
            fn.restype = i32
            fn.argtypes = [vp]
        c.AlxMemRawTest_Read.restype = i32
        c.AlxMemRawTest_Read.argtypes = [vp, u32, u32, u8, u16]
        c.AlxMemRawTest_Write.restype = i32
        c.AlxMemRawTest_Write.argtypes = [vp, u32, u32, b, u8, u16]
        c.AlxMemRawTest_BuffFill.argtypes = [u8]
        c.AlxMemRawTest_BuffPeek.restype = u8
        c.AlxMemRawTest_BuffPeek.argtypes = [u32]
        c.AlxMemRawTest_BuffPoke.argtypes = [u32, u8]
        c.AlxMemRawTest_BuffLen.restype = u32

        def status(name: str) -> int:
            fn = getattr(c, f"AlxMemRawTest_Status_{name}")
            fn.restype = i32
            return fn()

        self.OK = status("Ok")
        self.ERR = status("Err")
        self.ERR_NUM_OF_TRIES = status("ErrNumOfTries")
        self.POISON = c.AlxMemRawTest_PoisonByte()
        self.SIZEOF = c.AlxMemRawTest_SizeOf()

    # -- construction ------------------------------------------------------
    def new(self):
        """A poisoned AlxMemRaw with the constructor run over it. Auto-deleted by the fixture."""
        handle = self.c.AlxMemRawTest_New()
        self._handles.append(handle)
        return handle

    def new_no_ctor(self):
        """The same poisoned block with NO constructor - what the poison alone looks like."""
        handle = self.c.AlxMemRawTest_NewNoCtor()
        self._handles.append(handle)
        return handle

    def free_all(self) -> None:
        for handle in self._handles:
            self.c.AlxMemRawTest_Delete(handle)
        self._handles.clear()

    def field(self, me, name: str) -> int:
        """The BYTE in wasCtorCalled or isInit, not a bool.

        On a poisoned object the field holds 0xFF, which is not a value a C bool may hold - loading
        it as one is undefined behaviour the SANITIZE lane aborts on. The byte is also the more
        useful answer: it tells a test apart a field the constructor set false from a field the
        constructor never touched, which is the whole of P516.
        """
        return getattr(self.c, f"AlxMemRawTest_{name}")(me)

    # -- the five public calls --------------------------------------------
    def init(self, me) -> int:
        return self.c.AlxMemRawTest_Init(me)

    def deinit(self, me) -> int:
        return self.c.AlxMemRawTest_DeInit(me)

    def read(self, me, addr: int = 0, length: int = 8, tries: int = 3, timeout_ms: int = 1000) -> int:
        return self.c.AlxMemRawTest_Read(me, addr, length, tries, timeout_ms)

    def write(self, me, addr: int = 0, length: int = 8, check: bool = True,
              tries: int = 3, timeout_ms: int = 1000) -> int:
        return self.c.AlxMemRawTest_Write(me, addr, length, check, tries, timeout_ms)

    def lifecycle(self, me, call: str) -> int:
        """Call Init or DeInit by name, for a test parametrized over the asserting stubs."""
        return getattr(self.c, f"AlxMemRawTest_{call}")(me)

    # -- the caller's data buffer -----------------------------------------
    def fill(self, value: int) -> None:
        self.c.AlxMemRawTest_BuffFill(value)

    def buff(self, length: int) -> bytes:
        return bytes(self.c.AlxMemRawTest_BuffPeek(i) for i in range(length))

    def asserts(self) -> int:
        return self.c.AlxAssertPc_Count()

    def first(self) -> str:
        return (self.c.AlxAssertPc_First() or b"").decode("ascii", "replace")

    def assert_reset(self) -> None:
        self.c.AlxAssertPc_Reset()


class MemRawOverrideLib(MemRawLib):
    """ctypes wrapper around alxMemRawOvrTest.dll: four of the five weak symbols displaced.

    Same surface as MemRawLib plus the override's recorders. AlxMemRaw_DeInit is left weak on
    purpose, so ``deinit`` still reaches the library's asserting stub in this very DLL - which is
    how one image shows that displacement is per symbol rather than per translation unit.
    """

    def __init__(self, dll_path: Path):
        super().__init__(dll_path)
        c = self.c
        u8, u32, b = ctypes.c_uint8, ctypes.c_uint32, ctypes.c_bool
        for name in ("CtorCount", "InitCount", "ReadCount", "WriteCount", "LastAddr", "LastLen",
                     "LastNumOfTries", "LastTimeout_ms", "Size"):
            getattr(c, f"AlxMemRawOverride_{name}").restype = u32
        c.AlxMemRawOverride_LastCheckWithReadEnable.restype = b
        c.AlxMemRawOverride_Peek.restype = u8
        c.AlxMemRawOverride_Peek.argtypes = [u32]
        c.AlxMemRawOverride_Poke.argtypes = [u32, u8]
        self.SIZE = c.AlxMemRawOverride_Size()

    def ovr_reset(self) -> None:
        self.c.AlxMemRawOverride_Reset()

    def count(self, kind: str) -> int:
        """How many times the override's ``kind`` ran - Ctor, Init, Read or Write."""
        return getattr(self.c, f"AlxMemRawOverride_{kind}Count")()

    def last(self, field: str):
        """One recorded argument of the last call: Addr, Len, NumOfTries, Timeout_ms, ..."""
        return getattr(self.c, f"AlxMemRawOverride_Last{field}")()

    def mem(self, addr: int, length: int) -> bytes:
        return bytes(self.c.AlxMemRawOverride_Peek(addr + i) for i in range(length))

    def poke(self, addr: int, data: bytes) -> None:
        for i, byte in enumerate(data):
            self.c.AlxMemRawOverride_Poke(addr + i, byte)


