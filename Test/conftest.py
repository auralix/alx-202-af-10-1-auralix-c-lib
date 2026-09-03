"""Auralix C Library - PC unit test harness (ALX-1514 pilot).

Builds a per-module DLL from the REAL c-lib sources (clang, -std=gnu99 = target dialect)
and exposes it to pytest via ctypes. No struct mirroring: tests use opaque
handles from alxFifoTestHelpers.c and the public alxFifo.h API only.
"""

import ctypes
import os
import re
import subprocess
from pathlib import Path

import pytest

TEST_DIR = Path(__file__).parent
CLIB_DIR = TEST_DIR.parent
BUILD_DIR = TEST_DIR / "build"

FIFO_SOURCES = [
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    TEST_DIR / "alxFifoTestHelpers.c",
    TEST_DIR / "alxBoundTestHelpers.c",
]
FIFO_DEPS = FIFO_SOURCES + [
    CLIB_DIR / "alxFifo.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxBound.h",
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxFifoTest.def",
    Path(__file__),  # flags live here - flag edits must trigger a rebuild
]
FIFO_DLL = BUILD_DIR / "alxFifoTest.dll"

# ----------------------------------------------------------- CLI module ------
# Tier-2 target: REAL alxCli + real param stack over the faked serial port and
# KV store. GATED sources compile with the full strict set; CLOSURE sources
# (linked real, own suites pending per the ALX-1495 rollout) compile with -w.
CLI_SOURCES_STRICT = [
    CLIB_DIR / "alxCli.c",
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    TEST_DIR / "alxSerialPortFake.c",
    TEST_DIR / "alxParamKvStoreFake.c",
    TEST_DIR / "alxIdFake.c",
    TEST_DIR / "alxAssertPc.c",
    TEST_DIR / "alxCliTestHelpers.c",
]
# asserts ON = test the code AS SHIPPED (product enables RST mode). Scoped to
# the CLI DLL for now; the fifo/bound DLL parity is its own follow-up.
CLI_ASSERT_DEFINES = [
    "-DALX_CLI_ASSERT_RST_ENABLE", "-DALX_SERIAL_PORT_ASSERT_RST_ENABLE",
    "-DALX_FIFO_ASSERT_RST_ENABLE", "-DALX_BOUND_ASSERT_RST_ENABLE",
    "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE", "-DALX_PARAM_MGMT_ASSERT_RST_ENABLE",
    "-DALX_FTOA_ASSERT_RST_ENABLE", "-DALX_RANGE_ASSERT_RST_ENABLE",
    "-DALX_ID_ASSERT_RST_ENABLE",
]
CLI_SOURCES_CLOSURE = [
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxParamMgmt.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
]
CLI_DEPS = CLI_SOURCES_STRICT + CLI_SOURCES_CLOSURE + [
    CLIB_DIR / "alxCli.h",
    CLIB_DIR / "Mcu" / "alxSerialPort.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    CLIB_DIR / "alxParamItem.h",
    CLIB_DIR / "alxParamMgmt.h",
    CLIB_DIR / "alxParamKvStore.h",
    CLIB_DIR / "alxFtoa.h",
    CLIB_DIR / "alxRange.h",
    CLIB_DIR / "alxId.h",
    CLIB_DIR / "alxFifo.h",
    CLIB_DIR / "alxBound.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxCliTest.def",
    Path(__file__),
]
CLI_DLL = BUILD_DIR / "alxCliTest.dll"

# ------------------------------------------------------- MemSafe group -------
# Tier-2 target (ALX-1513): REAL alxMemSafe + alxCrc under the strict set, the REAL
# param group/store chain as closure (-w: alxParamGroup.c/alxParamStore.c carry
# -Wswitch-enum warnings, ALX-1495 backlog), all over alxMemRawFake = a 2 kB RAM
# flash with fault injection and a power-loss model.
MEMSAFE_SOURCES_STRICT = [
    CLIB_DIR / "alxMemSafe.c",
    CLIB_DIR / "alxCrc.c",
    CLIB_DIR / "alxBound.c",
    TEST_DIR / "alxMemRawFake.c",
    TEST_DIR / "alxParamKvStoreFake.c",
    TEST_DIR / "alxAssertPc.c",
    TEST_DIR / "alxMemSafeTestHelpers.c",
]
MEMSAFE_SOURCES_CLOSURE = [
    CLIB_DIR / "alxParamGroup.c",
    CLIB_DIR / "alxParamStore.c",
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
]
MEMSAFE_ASSERT_DEFINES = [
    "-DALX_MEM_SAFE_ASSERT_RST_ENABLE", "-DALX_MEM_RAW_ASSERT_RST_ENABLE", "-DALX_CRC_ASSERT_RST_ENABLE",
    "-DALX_PARAM_GROUP_ASSERT_RST_ENABLE", "-DALX_PARAM_STORE_ASSERT_RST_ENABLE",
    "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE", "-DALX_BOUND_ASSERT_RST_ENABLE",
    "-DALX_FTOA_ASSERT_RST_ENABLE", "-DALX_RANGE_ASSERT_RST_ENABLE",
]
MEMSAFE_DEPS = MEMSAFE_SOURCES_STRICT + MEMSAFE_SOURCES_CLOSURE + [
    CLIB_DIR / "alxMemSafe.h", CLIB_DIR / "alxMemRaw.h", CLIB_DIR / "alxCrc.h",
    CLIB_DIR / "alxParamGroup.h", CLIB_DIR / "alxParamStore.h", CLIB_DIR / "alxParamItem.h",
    CLIB_DIR / "alxGlobal.h", CLIB_DIR / "alxAssert.h", TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxMemSafeTest.def", Path(__file__),
]
MEMSAFE_DLL = BUILD_DIR / "alxMemSafeTest.dll"



# ------------------------------------------------------------------ build ----
def _find_vcvars() -> Path:
    vswhere = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / \
        "Microsoft Visual Studio/Installer/vswhere.exe"
    vs_path = subprocess.run(
        [str(vswhere), "-latest", "-property", "installationPath"],
        capture_output=True, text=True, check=True).stdout.strip()
    vcvars = Path(vs_path) / "VC/Auxiliary/Build/vcvars64.bat"
    if not vcvars.exists():
        raise RuntimeError(f"vcvars64.bat not found under {vs_path}")
    return vcvars


def _needs_build(dll: Path, deps) -> bool:
    if not dll.exists():
        return True
    dll_mtime = dll.stat().st_mtime
    try:
        return any(d.stat().st_mtime > dll_mtime for d in deps)
    except FileNotFoundError:
        return True  # missing dependency -> rebuild (and let the compiler complain)


# Host module builds use clang in the TARGET dialect (-std=gnu99) - never test a
# dialect you do not ship. -O0 -g for faithful debugging (clang diagnostics are
# front-end based, near-identical at any -O). Blanket -Werror on the host lane.
CLANG = r"C:/Program Files/LLVM/bin/clang.exe"
HOST_WARN_FLAGS = [
    "-Wall", "-Wextra",
    "-Wshadow", "-Wstrict-prototypes", "-Wold-style-definition",
    "-Wmissing-prototypes", "-Wmissing-declarations", "-Wmissing-variable-declarations",
    "-Wredundant-decls", "-Wnested-externs", "-Wbad-function-cast",
    "-Wcast-qual", "-Wwrite-strings", "-Wundef", "-Wvla", "-Walloca",
    "-Wswitch-enum", "-Wswitch-default", "-Wenum-conversion",
    "-Wformat=2", "-Wfloat-equal", "-Wdouble-promotion", "-Wimplicit-fallthrough",
    "-Wnull-dereference", "-Wunused", "-Wunused-macros", "-Wno-unused-parameter",
]


def _write_compile_db() -> None:
    """compile_commands.json for clang-tidy/clangd - same flags as the real build."""
    import json
    args_common = ["clang", "-std=gnu99", "-O0", *HOST_WARN_FLAGS,
                   "-D_CRT_SECURE_NO_WARNINGS",
                   f"-I{TEST_DIR}", f"-I{CLIB_DIR}", f"-I{CLIB_DIR / 'Mcu'}"]
    db_sources = list(dict.fromkeys(FIFO_SOURCES + CLI_SOURCES_STRICT))
    db = [{"directory": str(BUILD_DIR),
           "arguments": [*args_common, "-c", str(src)],
           "file": str(src)} for src in db_sources]
    (BUILD_DIR / "compile_commands.json").write_text(json.dumps(db, indent=1))


def _build_fifo_dll() -> None:
    BUILD_DIR.mkdir(exist_ok=True)
    _write_compile_db()
    vcvars = _find_vcvars()
    sources = " ".join(f'"{s}"' for s in FIFO_SOURCES)
    flags = " ".join(HOST_WARN_FLAGS)
    cmd = (
        f'"{vcvars}" && "{CLANG}" -std=gnu99 -O0 -g {flags} -Werror '
        f'-D_CRT_SECURE_NO_WARNINGS '
        f'-I"{TEST_DIR}" -I"{CLIB_DIR}" -I"{CLIB_DIR / "Mcu"}" {sources} '
        f'-shared -o "{FIFO_DLL}" -Wl,/DEF:"{TEST_DIR / "alxFifoTest.def"}"'
    )
    result = subprocess.run(f'cmd /s /c "{cmd}"', capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"DLL build failed (rc={result.returncode}):\n{result.stdout}\n{result.stderr}")


def _build_cli_dll() -> None:
    """Two-step build: closure objects with -w, then strict sources + objects
    linked into the CLI test DLL under the full -Werror warning set."""
    BUILD_DIR.mkdir(exist_ok=True)
    obj_dir = BUILD_DIR / "cliClosure"
    obj_dir.mkdir(exist_ok=True)
    _write_compile_db()
    vcvars = _find_vcvars()
    inc = f'-I"{TEST_DIR}" -I"{CLIB_DIR}" -I"{CLIB_DIR / "Mcu"}"'

    closure = " ".join(f'"{s}"' for s in CLI_SOURCES_CLOSURE)
    asserts = " ".join(CLI_ASSERT_DEFINES)
    # asserts ON for the closure too - the product ships them on, and alxParamItem.c has
    # side effects inside ALX_PARAM_ITEM_ASSERT (sprintf of numeric values): with asserts
    # off, get-param printed numbers as EMPTY (found 03.09 by the P15 uint8 test)
    cmd1 = (f'cd /d "{obj_dir}" && "{vcvars}" && "{CLANG}" -std=gnu99 -O0 -g -w '
            f'-D_CRT_SECURE_NO_WARNINGS {asserts} {inc} -c {closure}')
    result = subprocess.run(f'cmd /s /c "{cmd1}"', capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"CLI closure build failed (rc={result.returncode}):\n{result.stdout}\n{result.stderr}")

    strict = " ".join(f'"{s}"' for s in CLI_SOURCES_STRICT)
    objs = " ".join(f'"{o}"' for o in sorted(obj_dir.glob("*.o")))
    flags = " ".join(HOST_WARN_FLAGS)
    asserts = " ".join(CLI_ASSERT_DEFINES)
    cmd2 = (
        f'"{vcvars}" && "{CLANG}" -std=gnu99 -O0 -g {flags} -Werror '
        f'-D_CRT_SECURE_NO_WARNINGS {asserts} {inc} {strict} {objs} '
        f'-shared -o "{CLI_DLL}" -Wl,/DEF:"{TEST_DIR / "alxCliTest.def"}"'
    )
    result = subprocess.run(f'cmd /s /c "{cmd2}"', capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(
            f"CLI DLL build failed (rc={result.returncode}):\n{result.stdout}\n{result.stderr}")


def _build_two_step_dll(strict, closure, assert_defines, dll: Path, def_file: Path, obj_dir_name: str) -> None:
    """Generic two-step build (same recipe as the CLI DLL): closure objects with -w,
    then strict sources + objects linked under the full -Werror warning set."""
    BUILD_DIR.mkdir(exist_ok=True)
    obj_dir = BUILD_DIR / obj_dir_name
    obj_dir.mkdir(exist_ok=True)
    _write_compile_db()
    vcvars = _find_vcvars()
    inc = f'-I"{TEST_DIR}" -I"{CLIB_DIR}" -I"{CLIB_DIR / "Mcu"}"'
    asserts = " ".join(assert_defines)
    closure_src = " ".join(f'"{s}"' for s in closure)
    cmd1 = (f'cd /d "{obj_dir}" && "{vcvars}" && "{CLANG}" -std=gnu99 -O0 -g -w '
            f'-D_CRT_SECURE_NO_WARNINGS {asserts} {inc} -c {closure_src}')
    result = subprocess.run(f'cmd /s /c "{cmd1}"', capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"{dll.name} closure build failed (rc={result.returncode}):\n{result.stdout}\n{result.stderr}")
    strict_src = " ".join(f'"{s}"' for s in strict)
    objs = " ".join(f'"{o}"' for o in sorted(obj_dir.glob("*.o")))
    flags = " ".join(HOST_WARN_FLAGS)
    cmd2 = (f'"{vcvars}" && "{CLANG}" -std=gnu99 -O0 -g {flags} -Werror '
            f'-D_CRT_SECURE_NO_WARNINGS {asserts} {inc} {strict_src} {objs} '
            f'-shared -o "{dll}" -Wl,/DEF:"{def_file}"')
    result = subprocess.run(f'cmd /s /c "{cmd2}"', capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"{dll.name} build failed (rc={result.returncode}):\n{result.stdout}\n{result.stderr}")


def _build_memsafe_dll() -> None:
    _build_two_step_dll(MEMSAFE_SOURCES_STRICT, MEMSAFE_SOURCES_CLOSURE, MEMSAFE_ASSERT_DEFINES,
                        MEMSAFE_DLL, TEST_DIR / "alxMemSafeTest.def", "memSafeClosure")


# ---------------------------------------------------------------- ctypes -----
class Lib:
    """ctypes wrapper around alxFifoTest.dll with contract-shaped helpers."""

    def __init__(self, dll_path: Path):
        self.c = ctypes.CDLL(str(dll_path))
        c = self.c
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
# The proof token in a test's NAME (test_ALX1514_P4_...) is the primary
# traceability link (spec PROOF row <-> test <-> commit, greppable).
# This hook mirrors it into junit XML as <property name="proof" .../> so the
# evidence artifact carries it in structured form too. Additional proofs from
# LATER tasks attach via @pytest.mark.req("ALX-1620-P2") - the test name keeps
# its ORIGINATING token forever (never rename on later modification).
_PROOF_RE = re.compile(r"ALX(\d+)_P(\d+)")


def pytest_collection_modifyitems(items):
    for item in items:
        m = _PROOF_RE.search(item.name)
        if m:
            item.user_properties.append(("proof", f"ALX-{m.group(1)}-P{m.group(2)}"))
        for mark in item.iter_markers(name="req"):
            for rid in mark.args:
                item.user_properties.append(("req", rid))


class BoundLib:
    """ctypes wrapper for the AlxBound functions (same test-group DLL as alxFifo).

    Bound functions act on caller scalars/buffers - no opaque handle. Each
    numeric entry maps to (ctypes scalar type, AlxBound function).
    """

    NUMERIC = {
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

    def bound_str(self, val: bytes, max_len_with_nul: int, buf_len: int = None):
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
    POISON = 0xAA

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
        vp, u32, i32, u8p, b = ctypes.c_void_p, ctypes.c_uint32, ctypes.c_int32, ctypes.POINTER(ctypes.c_uint8), ctypes.c_bool
        c.AlxMemSafeTest_New.restype = vp
        c.AlxMemSafeTest_New.argtypes = [u32, u32, ctypes.c_uint8, ctypes.c_uint8]
        c.AlxMemSafeTest_Delete.argtypes = [vp]
        for name in ("CopyLen", "NumOfItems"):
            getattr(c, f"AlxMemSafeTest_{name}").restype = u32
        for name in ("MemSafeRead", "MemSafeWrite"):
            f = getattr(c, f"AlxMemSafeTest_{name}"); f.restype = i32; f.argtypes = [vp, u8p, u32]
        for name in ("MemSafeIsReadDone", "MemSafeIsReadErr", "MemSafeIsWriteDone", "MemSafeIsWriteErr",
                     "GroupIsValStoredBuffDiff", "StoreIsErr"):
            f = getattr(c, f"AlxMemSafeTest_{name}"); f.restype = b; f.argtypes = [vp]
        for name in ("GroupInit", "StoreInit"):
            f = getattr(c, f"AlxMemSafeTest_{name}"); f.restype = i32; f.argtypes = [vp]
        c.AlxMemSafeTest_StoreHandle.argtypes = [vp]
        c.AlxMemSafeTest_ItemGet.restype = u32; c.AlxMemSafeTest_ItemGet.argtypes = [vp, u32]
        c.AlxMemSafeTest_ItemGetDef.restype = u32; c.AlxMemSafeTest_ItemGetDef.argtypes = [vp, u32]
        c.AlxMemSafeTest_ItemSet.restype = i32; c.AlxMemSafeTest_ItemSet.argtypes = [vp, u32, u32]
        c.AlxMemSafeTest_CrcCalc.restype = u32; c.AlxMemSafeTest_CrcCalc.argtypes = [u32, u8p, u32]
        c.AlxMemSafeTest_CrcIsOk.restype = b; c.AlxMemSafeTest_CrcIsOk.argtypes = [u32, u8p, u32, ctypes.POINTER(u32)]
        c.AlxMemSafeTest_CrcLen.restype = u32; c.AlxMemSafeTest_CrcLen.argtypes = [u32]
        c.AlxMemRawFake_Fill.argtypes = [ctypes.c_uint8]
        c.AlxMemRawFake_Peek.argtypes = [u32, u8p, u32]
        c.AlxMemRawFake_Poke.argtypes = [u32, u8p, u32]
        c.AlxMemRawFake_FailAt.argtypes = [u32, u32]
        c.AlxMemRawFake_Count.restype = u32; c.AlxMemRawFake_Count.argtypes = [u32]
        c.AlxMemRawFake_PowerLossAt.argtypes = [u32, u32]
        c.AlxMemRawFake_IsPowerLost.restype = b
        c.AlxMemRawFake_SetRowEraseModel.argtypes = [b, u32]
        for name in ("Size", "LastNumOfTries", "LastTimeout_ms", "LastWriteAddr", "LastWriteLen"):
            getattr(c, f"AlxMemRawFake_{name}").restype = u32

        def status(name: str) -> int:
            f = getattr(c, f"AlxMemSafeTest_Status_{name}"); f.restype = i32
            return f()

        self.OK = status("Ok")
        self.ERR = status("Err")
        self.ERR_NUM_OF_TRIES = status("ErrNumOfTries")
        self.BOTH_ERR = status("BothCopyErr")
        self.BOTH_OK_SAME_USE_A = status("BothOkSame_UseA")
        self.BOTH_OK_DIFF_USE_A = status("BothOkDiff_UseA")
        self.A_OK_B_ERR_USE_A = status("AOkBErr_UseA")
        self.A_ERR_B_OK_USE_B = status("AErrBOk_UseB")
        self.COPY_LEN = c.AlxMemSafeTest_CopyLen()
        self.NUM_ITEMS = c.AlxMemSafeTest_NumOfItems()

    # -- chain under test -----------------------------------------------------
    def new(self, addr_a: int = 0x000, addr_b: int = 0x100, tries: int = 3, raw_tries: int = 3):
        return self.c.AlxMemSafeTest_New(addr_a, addr_b, tries, raw_tries)

    def delete(self, ctx):
        self.c.AlxMemSafeTest_Delete(ctx)

    @staticmethod
    def _buf(data: bytes):
        return (ctypes.c_uint8 * len(data))(*data)

    def read(self, ctx, n: int = None):
        """MemSafe read into a poison-filled buffer -> (status, bytes)."""
        n = self.COPY_LEN if n is None else n
        buf = (ctypes.c_uint8 * n)(*([self.POISON] * n))
        st = self.c.AlxMemSafeTest_MemSafeRead(ctx, buf, n)
        return st, bytes(buf)

    def write(self, ctx, data: bytes) -> int:
        return self.c.AlxMemSafeTest_MemSafeWrite(ctx, self._buf(data), len(data))

    def flags(self, ctx) -> dict:
        c = self.c
        return {"read_done": c.AlxMemSafeTest_MemSafeIsReadDone(ctx), "read_err": c.AlxMemSafeTest_MemSafeIsReadErr(ctx),
                "write_done": c.AlxMemSafeTest_MemSafeIsWriteDone(ctx), "write_err": c.AlxMemSafeTest_MemSafeIsWriteErr(ctx)}

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

    def last_raw_args(self):
        return self.c.AlxMemRawFake_LastNumOfTries(), self.c.AlxMemRawFake_LastTimeout_ms()


# --------------------------------------------------------------- fixtures ----
@pytest.fixture(scope="session")
def lib() -> Lib:
    # ALX_FIFO_TEST_DLL selects an externally built DLL variant (coverage/sanitizer
    # build) instead of the default clang build - same suite, instrumented binary.
    override = os.environ.get("ALX_FIFO_TEST_DLL")
    if override:
        return Lib(Path(override))
    if _needs_build(FIFO_DLL, FIFO_DEPS):
        _build_fifo_dll()
    return Lib(FIFO_DLL)


@pytest.fixture(scope="session")
def bound(lib) -> BoundLib:
    # same test-group DLL as alxFifo (alxBound.c is linked into it anyway)
    return BoundLib(lib.c)


@pytest.fixture(scope="session")
def cli_lib() -> CliLib:
    # ALX_CLI_TEST_DLL selects an externally built variant (coverage/sanitizer)
    override = os.environ.get("ALX_CLI_TEST_DLL")
    if override:
        return CliLib(Path(override))
    if _needs_build(CLI_DLL, CLI_DEPS):
        _build_cli_dll()
    return CliLib(CLI_DLL)


@pytest.fixture
def make_cli(cli_lib):
    """Factory: make_cli() -> CliUnderTest with a fresh CLI + fake port. Auto-deleted."""
    ctxs = []

    def _make() -> CliUnderTest:
        ctx = cli_lib.c.AlxCliTest_New()
        ctxs.append(ctx)
        return CliUnderTest(cli_lib, ctx)

    yield _make
    for ctx in ctxs:
        cli_lib.c.AlxCliTest_Delete(ctx)


@pytest.fixture
def make_fifo(lib):
    """Factory: make_fifo(buff_len, rotate=n) -> handle. Auto-deleted."""
    handles = []

    def _make(buff_len: int, rotate: int = 0):
        f = lib.new(buff_len)
        handles.append(f)
        if rotate:
            assert rotate < buff_len
            assert lib.write(f, b"x" * rotate) == lib.OK
            status, _ = lib.read(f, rotate)
            assert status == lib.OK
            assert lib.entries(f) == 0
        return f

    yield _make
    for h in handles:
        lib.delete(h)


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


@pytest.fixture(scope="session")
def memsafe_lib() -> MemSafeLib:
    # ALX_MEMSAFE_TEST_DLL selects an externally built variant (coverage/sanitizer)
    override = os.environ.get("ALX_MEMSAFE_TEST_DLL")
    if override:
        return MemSafeLib(Path(override))
    if _needs_build(MEMSAFE_DLL, MEMSAFE_DEPS):
        _build_memsafe_dll()
    return MemSafeLib(MEMSAFE_DLL)


@pytest.fixture
def flash(memsafe_lib) -> MemSafeLib:
    """The fake flash, blank (0xFF) and fault-free at the start of every test."""
    memsafe_lib.fake_reset()
    return memsafe_lib


@pytest.fixture
def make_store(flash):
    """Factory: make_store(addr_a, addr_b, tries, raw_tries) -> ctx over the shared fake flash.
    A second ctx over the same flash = a reboot. Auto-deleted."""
    ctxs = []

    def _make(addr_a: int = 0x000, addr_b: int = 0x100, tries: int = 3, raw_tries: int = 3):
        ctx = flash.new(addr_a, addr_b, tries, raw_tries)
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        flash.delete(ctx)
