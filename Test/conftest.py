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
    db = [{"directory": str(BUILD_DIR),
           "arguments": [*args_common, "-c", str(src)],
           "file": str(src)} for src in FIFO_SOURCES]
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
