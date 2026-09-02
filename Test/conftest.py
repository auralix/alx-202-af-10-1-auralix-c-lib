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
    cmd1 = (f'cd /d "{obj_dir}" && "{vcvars}" && "{CLANG}" -std=gnu99 -O0 -g -w '
            f'-D_CRT_SECURE_NO_WARNINGS {inc} -c {closure}')
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
        c.AlxCliTest_GetCmdLenMax.restype = ctypes.c_uint32
        c.AlxCliTest_GetLongKey.restype = ctypes.c_void_p     # raw pointer; string_at() reads it
        c.AlxCliTest_WasResetRequested.restype = ctypes.c_bool
        c.AlxCliTest_Status_Ok.restype = ctypes.c_int32
        c.AlxSerialPortFake_InjectRx.restype = ctypes.c_int32
        c.AlxSerialPortFake_InjectRx.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
        c.AlxSerialPortFake_TxRead.restype = ctypes.c_uint32
        c.AlxSerialPortFake_TxRead.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_uint32]
        self.OK = c.AlxCliTest_Status_Ok()

    def buff_len(self) -> int:
        return self.c.AlxCliTest_GetBuffLen()

    def cmd_len_max(self) -> int:
        """ALX_CLI_CMD_LEN_MAX as compiled into the CLI (spec: 300)."""
        return self.c.AlxCliTest_GetCmdLenMax()

    def long_key(self) -> bytes:
        """The helper's 128-char param key (spec maximum)."""
        return ctypes.string_at(self.c.AlxCliTest_GetLongKey())

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
