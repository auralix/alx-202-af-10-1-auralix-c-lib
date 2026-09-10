"""Auralix C Library - PC unit test harness (ALX-1514 pilot).

Builds a per-module DLL from the REAL c-lib sources (clang, -std=gnu99 = target dialect)
and exposes it to pytest via ctypes. No struct mirroring: tests use opaque
handles from alxFifoTestHelpers.c and the public alxFifo.h API only.
"""

import ctypes
import os
from pathlib import Path
from typing import ClassVar

import pytest
from alx.c_lib import host_build

# proof token / req marker -> junit <property>, run_dir, git_head: the Auralix Python lib's evidence plugin
pytest_plugins = ("alx.verify.evidence",)

TEST_DIR = Path(__file__).parent
CLIB_DIR = TEST_DIR.parent
BUILD_DIR = TEST_DIR / "build"

FIFO_SOURCES = [
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    TEST_DIR / "alxFifoTestHelpers.c",
    TEST_DIR / "alxBoundTestHelpers.c",
]
FIFO_DEPS = [
    *FIFO_SOURCES,
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


# -------------------------------------------------------- Vdiv module -------
# Tier-1 target: a resistive divider's arithmetic, nine context-free functions and
# no state. Its own group because it shares nothing with the store chain: the
# smallest DLL here, and the shape any other pure module should follow.
VDIV_SOURCES = [
    CLIB_DIR / "alxVdiv.c",
    TEST_DIR / "alxAssertPc.c",
]
VDIV_DEPS = [
    *VDIV_SOURCES,
    CLIB_DIR / "alxVdiv.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxVdivTest.def",
    Path(__file__),
]
VDIV_DLL = BUILD_DIR / "alxVdivTest.dll"


# ------------------------------------------------------ TimSw module -------
# Tier-2 target: the REAL software timer over the REAL tick counter, with the
# interrupt lock faked because it is CMSIS intrinsics that do not exist on a PC.
# The tick is not faked - it is a plain counter the test advances itself, so the
# timer sees real code and the test owns the clock.
TIMSW_SOURCES = [
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    TEST_DIR / "alxIrqFake.c",
    TEST_DIR / "alxAssertPc.c",
]
TIMSW_DEPS = [
    *TIMSW_SOURCES,
    CLIB_DIR / "alxTimSw.h",
    CLIB_DIR / "alxTick.h",
    CLIB_DIR / "alxIrq.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxTimSwTest.def",
    Path(__file__),
]
TIMSW_DLL = BUILD_DIR / "alxTimSwTest.dll"


# ------------------------------------------------- CanParser module -------
# Tier-1 target: the CAN payload codec, 24 context-free functions over an
# 8-byte buffer and nothing else. It is the module every product's CAN frame
# passes through, and it needs no CAN peripheral to test: AlxCan_Msg is a plain
# structure, so the whole codec is host code the moment it is compiled here.
CANPARSER_SOURCES = [
    CLIB_DIR / "alxCanParser.c",
    TEST_DIR / "alxAssertPc.c",
]
CANPARSER_DEPS = [
    *CANPARSER_SOURCES,
    CLIB_DIR / "alxCanParser.h",
    CLIB_DIR / "Mcu" / "alxCan.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxCanParserTest.def",
    Path(__file__),
]
CANPARSER_DLL = BUILD_DIR / "alxCanParserTest.dll"


# ------------------------------------------------ FiltGlitch modules -------
# Tier-2 target: both glitch filters over the REAL software timer and the REAL
# tick counter, with only the interrupt lock faked. The clock is the test's, so
# every debounce boundary is exact instead of approximate - which is the whole
# reason these are worth testing at all.
FILTGLITCH_SOURCES = [
    CLIB_DIR / "alxFiltGlitchBool.c",
    CLIB_DIR / "alxFiltGlitchUint32.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    TEST_DIR / "alxFiltGlitchTestHelpers.c",
    TEST_DIR / "alxIrqFake.c",
    TEST_DIR / "alxAssertPc.c",
]
FILTGLITCH_DEPS = [
    *FILTGLITCH_SOURCES,
    CLIB_DIR / "alxFiltGlitchBool.h",
    CLIB_DIR / "alxFiltGlitchUint32.h",
    CLIB_DIR / "alxTimSw.h",
    CLIB_DIR / "alxTick.h",
    CLIB_DIR / "alxIrq.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxFiltGlitchTest.def",
    Path(__file__),
]
FILTGLITCH_DLL = BUILD_DIR / "alxFiltGlitchTest.dll"


# ---------------------------------------------------- Math modules -------
# Tier-1 target: the library's pure signal conditioning - two hysteresis state
# machines, a moving average and a running min/max/mean - plus the number
# formatting in alxGlobal, which has no home of its own. In one group because
# they share everything that matters: no state outside their own arguments, no
# clock, no peripheral, and a product reads every analog decision through one.
MATH_SOURCES = [
    CLIB_DIR / "alxHys1.c",
    CLIB_DIR / "alxHys2.c",
    CLIB_DIR / "alxAvg.c",
    CLIB_DIR / "alxMath.c",
    CLIB_DIR / "alxGlobal.c",
    TEST_DIR / "alxMathTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
MATH_DEPS = [
    *MATH_SOURCES,
    CLIB_DIR / "alxHys1.h",
    CLIB_DIR / "alxHys2.h",
    CLIB_DIR / "alxAvg.h",
    CLIB_DIR / "alxMath.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxGlobal.c",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxMathTest.def",
    Path(__file__),
]
MATH_DLL = BUILD_DIR / "alxMathTest.dll"


# -------------------------------------------------- Mapping modules -------
# Tier-1 target: the two ways the library turns an x into a y - a straight line
# through two points, and a lookup table interpolated between its points. Both
# are pure, both are how a raw reading becomes an engineering unit, and neither
# had a test.
LINFUN_SOURCES = [
    CLIB_DIR / "alxLinFun.c",
    CLIB_DIR / "alxInterpLin.c",
    TEST_DIR / "alxLinFunTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
LINFUN_DEPS = [
    *LINFUN_SOURCES,
    CLIB_DIR / "alxLinFun.h",
    CLIB_DIR / "alxInterpLin.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxLinFunTest.def",
    Path(__file__),
]
LINFUN_DLL = BUILD_DIR / "alxLinFunTest.dll"


# ------------------------------------------------------ Bool module -------
# Tier-2 target: the library's boolean-with-memory, over the REAL glitch filter,
# software timer and tick, with only the interrupt lock faked. Twenty-one query
# and clear functions on one input - the module a product uses to tell a short
# press from a long one - and the clock is the test's, so every one of its
# thresholds is exact.
BOOL_SOURCES = [
    CLIB_DIR / "alxBool.c",
    CLIB_DIR / "alxFiltGlitchBool.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    TEST_DIR / "alxBoolTestHelpers.c",
    TEST_DIR / "alxIrqFake.c",
    TEST_DIR / "alxAssertPc.c",
]
BOOL_DEPS = [
    *BOOL_SOURCES,
    CLIB_DIR / "alxBool.h",
    CLIB_DIR / "alxFiltGlitchBool.h",
    CLIB_DIR / "alxTimSw.h",
    CLIB_DIR / "alxTick.h",
    CLIB_DIR / "alxIrq.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxBoolTest.def",
    Path(__file__),
]
BOOL_DLL = BUILD_DIR / "alxBoolTest.dll"


# ------------------------------------------------------- RTC module -------
# Tier-1 target: the calendar. Sixteen free functions converting between Unix
# time and a broken-down date in four resolutions, and splitting a second into
# its fractions. No object, no clock, no peripheral - and the one module in the
# library whose correct answers can be computed independently, which is what
# these tests do.
RTC_SOURCES = [
    CLIB_DIR / "alxRtc_Global.c",
    TEST_DIR / "alxAssertPc.c",
]
RTC_DEPS = [
    *RTC_SOURCES,
    CLIB_DIR / "alxRtc_Global.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxRtcTest.def",
    Path(__file__),
]
RTC_DLL = BUILD_DIR / "alxRtcTest.dll"


# ------------------------------------------------ ParamMgmt module -------
# Tier-1 target over a real item table: the view a product's CLI reaches its
# parameters through - by index, by key, by id - and the per-group reset to
# factory defaults. The items are real AlxParamItem built by the helper, so the
# whole lookup and conversion chain is under test, not a stand-in for it.
# Asserts ON, as the product ships them: alxParamItem.c has side effects inside
# ALX_PARAM_ITEM_ASSERT (an sprintf of numeric values), so a closure built
# without them formats numbers as EMPTY - and alxRange.c has one function whose
# whole body is an assert, which falls off the end of a non-void function when
# they are off. Same reasoning, same defines as the MemSafe group.
PARAMMGMT_SOURCES_STRICT = [
    CLIB_DIR / "alxParamMgmt.c",
    TEST_DIR / "alxParamKvStoreFake.c",   # the items are constructed WITHOUT a store; this is here
    TEST_DIR / "alxParamMgmtTestHelpers.c",   # only because alxParamItem.c references the symbols
    TEST_DIR / "alxAssertPc.c",
]
PARAMMGMT_SOURCES_CLOSURE = [
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
    CLIB_DIR / "alxBound.c",
]
PARAMMGMT_ASSERT_DEFINES = [
    "-DALX_PARAM_MGMT_ASSERT_RST_ENABLE", "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE",
    "-DALX_BOUND_ASSERT_RST_ENABLE", "-DALX_FTOA_ASSERT_RST_ENABLE",
    "-DALX_RANGE_ASSERT_RST_ENABLE",
]
PARAMMGMT_DEPS = [
    *PARAMMGMT_SOURCES_STRICT,
    *PARAMMGMT_SOURCES_CLOSURE,
    CLIB_DIR / "alxParamMgmt.h",
    CLIB_DIR / "alxParamItem.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxParamMgmtTest.def",
    Path(__file__),
]
PARAMMGMT_DLL = BUILD_DIR / "alxParamMgmtTest.dll"



# ------------------------------------------------------------------ build ----
# The mechanics live in the Python lib (alx.c_lib.host_build): where the tools are, the MSVC build
# environment, the rebuild-if-stale check, the compile database and the two DLL recipes. What stays
# here is what is this repository's - the source lists, the defines and the .def files above.
#
# Driver GNU = clang with GNU-style flags, which is what the dev build has always used; the sanitizer
# and coverage lanes use clang-cl on the same recipe (Test/noxfile.py). Dialect gnu99 = what the
# target ships: never test a dialect you do not ship. -O0 -g for faithful debugging; clang's
# diagnostics are front-end based, so the warning set is the same at any -O.
TOOLCHAIN = host_build.Toolchain()
INCLUDES = [TEST_DIR, CLIB_DIR, CLIB_DIR / "Mcu"]
DEBUG_FLAGS = ["-O0", "-g"]
STRICT_WARNINGS = [*host_build.WARNINGS, "-Werror"]   # blanket -Werror on the host lane
DB_ARGUMENTS = ["clang", "-std=gnu99", "-O0", *host_build.WARNINGS, host_build.CRT_DEFINE,
                *[f"-I{d}" for d in INCLUDES]]


def _needs_build(dll: Path, deps) -> bool:
    return host_build.needs_build(dll, deps)


def write_compile_db() -> None:
    """compile_commands.json for clang-tidy/clangd - same flags as the real build."""
    host_build.write_compile_db(
        BUILD_DIR / "compile_commands.json",
        list(dict.fromkeys(FIFO_SOURCES + CLI_SOURCES_STRICT)),
        DB_ARGUMENTS,
        directory=BUILD_DIR,
    )


def _build_dll(strict, closure, defines, dll: Path, def_file: Path, obj_dir_name: str | None) -> None:
    """One group's DLL: the library's recipe, this repository's lists.

    With a closure it is the two-step build - the closure compiled with warnings off, then the
    gated sources and those objects linked under the full set with -Werror. The defines reach both
    steps: asserts are ON as the product ships them, and alxParamItem.c has side effects inside
    ALX_PARAM_ITEM_ASSERT (sprintf of numeric values), so a closure built without them printed
    numbers as EMPTY (found 03.09 by the P15 uint8 test).
    """
    BUILD_DIR.mkdir(exist_ok=True)
    write_compile_db()
    host_build.build_dll(
        TOOLCHAIN,
        out=dll,
        strict=strict,
        closure=closure,
        includes=INCLUDES,
        defines=defines,
        def_file=def_file,
        flags=DEBUG_FLAGS,
        warnings=STRICT_WARNINGS,
        obj_dir=None if obj_dir_name is None else BUILD_DIR / obj_dir_name,
        driver=host_build.GNU,
    )


def _build_fifo_dll() -> None:
    _build_dll(FIFO_SOURCES, (), (), FIFO_DLL, TEST_DIR / "alxFifoTest.def", None)


def _build_cli_dll() -> None:
    _build_dll(CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE, CLI_ASSERT_DEFINES,
               CLI_DLL, TEST_DIR / "alxCliTest.def", "cliClosure")


def _build_memsafe_dll() -> None:
    _build_dll(MEMSAFE_SOURCES_STRICT, MEMSAFE_SOURCES_CLOSURE, MEMSAFE_ASSERT_DEFINES,
               MEMSAFE_DLL, TEST_DIR / "alxMemSafeTest.def", "memSafeClosure")


def _build_vdiv_dll() -> None:
    _build_dll(VDIV_SOURCES, (), (), VDIV_DLL, TEST_DIR / "alxVdivTest.def", None)


def _build_canparser_dll() -> None:
    _build_dll(CANPARSER_SOURCES, (), (), CANPARSER_DLL, TEST_DIR / "alxCanParserTest.def", None)


def _build_filtglitch_dll() -> None:
    _build_dll(FILTGLITCH_SOURCES, (), (), FILTGLITCH_DLL, TEST_DIR / "alxFiltGlitchTest.def", None)


def _build_math_dll() -> None:
    _build_dll(MATH_SOURCES, (), (), MATH_DLL, TEST_DIR / "alxMathTest.def", None)


def _build_linfun_dll() -> None:
    _build_dll(LINFUN_SOURCES, (), (), LINFUN_DLL, TEST_DIR / "alxLinFunTest.def", None)


def _build_bool_dll() -> None:
    _build_dll(BOOL_SOURCES, (), (), BOOL_DLL, TEST_DIR / "alxBoolTest.def", None)


def _build_rtc_dll() -> None:
    _build_dll(RTC_SOURCES, (), (), RTC_DLL, TEST_DIR / "alxRtcTest.def", None)


def _build_parammgmt_dll() -> None:
    _build_dll(PARAMMGMT_SOURCES_STRICT, PARAMMGMT_SOURCES_CLOSURE, PARAMMGMT_ASSERT_DEFINES,
               PARAMMGMT_DLL, TEST_DIR / "alxParamMgmtTest.def", "paramMgmtClosure")


def _build_timsw_dll() -> None:
    _build_dll(TIMSW_SOURCES, (), (), TIMSW_DLL, TEST_DIR / "alxTimSwTest.def", None)


# The groups as DATA, for anything that must rebuild them without running the suite: the MUTATE
# lane names this list on the command line (alx.c_lib.mutation_hooks rebuild --groups
# conftest:DLL_GROUPS), so the lane needs no script of its own in this repository.
DLL_GROUPS = [
    (FIFO_DLL, FIFO_DEPS, _build_fifo_dll),
    (CLI_DLL, CLI_DEPS, _build_cli_dll),
    (MEMSAFE_DLL, MEMSAFE_DEPS, _build_memsafe_dll),
    (VDIV_DLL, VDIV_DEPS, _build_vdiv_dll),
    (TIMSW_DLL, TIMSW_DEPS, _build_timsw_dll),
    (CANPARSER_DLL, CANPARSER_DEPS, _build_canparser_dll),
    (FILTGLITCH_DLL, FILTGLITCH_DEPS, _build_filtglitch_dll),
    (MATH_DLL, MATH_DEPS, _build_math_dll),
    (LINFUN_DLL, LINFUN_DEPS, _build_linfun_dll),
    (BOOL_DLL, BOOL_DEPS, _build_bool_dll),
    (RTC_DLL, RTC_DEPS, _build_rtc_dll),
    (PARAMMGMT_DLL, PARAMMGMT_DEPS, _build_parammgmt_dll),
]


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


class VdivLib:
    """ctypes wrapper around alxVdivTest.dll: a resistive divider's arithmetic.

    Two families with the same algebra in different units - float volts and kOhm, and integer
    millivolts and ohm - plus the current a shunt drops. Every function is context free.
    """

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
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

    def vout_v(self, vin_v, res_high_kohm, res_low_kohm) -> float:
        return self.c.AlxVdiv_GetVout_V(vin_v, res_high_kohm, res_low_kohm)

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

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
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
        c.AlxTick_Get_ns.restype = u64
        c.AlxTick_Get_ns.argtypes = [vp]
        c.AlxTick_IncRange_ns.argtypes = [vp, u64]
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


@pytest.fixture(scope="session")
def timsw_lib() -> TimSwLib:
    override = os.environ.get("ALX_TIMSW_TEST_DLL")
    if override:
        return TimSwLib(Path(override))
    if _needs_build(TIMSW_DLL, TIMSW_DEPS):
        _build_timsw_dll()
    return TimSwLib(TIMSW_DLL)


@pytest.fixture
def clock(timsw_lib) -> TimSwLib:
    """The timer library with the clock back at zero and the fake's counters cleared."""
    timsw_lib.tick_reset()
    return timsw_lib


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
    which is what test_alxRtc.py does with Python's own datetime.
    """

    RESOLUTIONS: ClassVar[dict[str, int]] = {
        "Sec": 1, "Ms": 1_000, "Us": 1_000_000, "Ns": 1_000_000_000,
    }

    def __init__(self, dll_path: Path):
        c = ctypes.CDLL(str(dll_path))
        self.c = c
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


@pytest.fixture(scope="session")
def param_mgmt_lib_session() -> ParamMgmtLib:
    override = os.environ.get("ALX_PARAMMGMT_TEST_DLL")
    if override:
        return ParamMgmtLib(Path(override))
    if _needs_build(PARAMMGMT_DLL, PARAMMGMT_DEPS):
        _build_parammgmt_dll()
    return ParamMgmtLib(PARAMMGMT_DLL)


@pytest.fixture
def param_mgmt_lib(param_mgmt_lib_session) -> ParamMgmtLib:
    """The manager library, with every table the previous test built already released."""
    yield param_mgmt_lib_session
    param_mgmt_lib_session.free_all()


@pytest.fixture(scope="session")
def rtc_lib() -> RtcLib:
    override = os.environ.get("ALX_RTC_TEST_DLL")
    if override:
        return RtcLib(Path(override))
    if _needs_build(RTC_DLL, RTC_DEPS):
        _build_rtc_dll()
    return RtcLib(RTC_DLL)


@pytest.fixture(scope="session")
def bool_lib_session() -> BoolLib:
    override = os.environ.get("ALX_BOOL_TEST_DLL")
    if override:
        return BoolLib(Path(override))
    if _needs_build(BOOL_DLL, BOOL_DEPS):
        _build_bool_dll()
    return BoolLib(BOOL_DLL)


@pytest.fixture
def bool_lib(bool_lib_session) -> BoolLib:
    """The boolean library with the clock back at zero and nothing left from the last test."""
    bool_lib_session.tick_reset()
    yield bool_lib_session
    bool_lib_session.free_all()


@pytest.fixture(scope="session")
def lin_fun_lib_session() -> LinFunLib:
    override = os.environ.get("ALX_LINFUN_TEST_DLL")
    if override:
        return LinFunLib(Path(override))
    if _needs_build(LINFUN_DLL, LINFUN_DEPS):
        _build_linfun_dll()
    return LinFunLib(LINFUN_DLL)


@pytest.fixture
def lin_fun_lib(lin_fun_lib_session) -> LinFunLib:
    """The mapping library, with everything the previous test allocated already released."""
    yield lin_fun_lib_session
    lin_fun_lib_session.free_all()


@pytest.fixture(scope="session")
def math_lib_session() -> MathLib:
    override = os.environ.get("ALX_MATH_TEST_DLL")
    if override:
        return MathLib(Path(override))
    if _needs_build(MATH_DLL, MATH_DEPS):
        _build_math_dll()
    return MathLib(MATH_DLL)


@pytest.fixture
def math_lib(math_lib_session) -> MathLib:
    """The arithmetic library, with everything the previous test allocated already released."""
    yield math_lib_session
    math_lib_session.free_all()


@pytest.fixture(scope="session")
def filt_glitch_lib() -> FiltGlitchLib:
    override = os.environ.get("ALX_FILTGLITCH_TEST_DLL")
    if override:
        return FiltGlitchLib(Path(override))
    if _needs_build(FILTGLITCH_DLL, FILTGLITCH_DEPS):
        _build_filtglitch_dll()
    return FiltGlitchLib(FILTGLITCH_DLL)


@pytest.fixture
def filt(filt_glitch_lib) -> FiltGlitchLib:
    """The filter library with the clock back at zero and nothing left over from the last test."""
    filt_glitch_lib.tick_reset()
    yield filt_glitch_lib
    filt_glitch_lib.free_all()


@pytest.fixture(scope="session")
def canparser_lib() -> CanParserLib:
    override = os.environ.get("ALX_CANPARSER_TEST_DLL")
    if override:
        return CanParserLib(Path(override))
    if _needs_build(CANPARSER_DLL, CANPARSER_DEPS):
        _build_canparser_dll()
    return CanParserLib(CANPARSER_DLL)


@pytest.fixture(scope="session")
def vdiv_lib() -> VdivLib:
    override = os.environ.get("ALX_VDIV_TEST_DLL")
    if override:
        return VdivLib(Path(override))
    if _needs_build(VDIV_DLL, VDIV_DEPS):
        _build_vdiv_dll()
    return VdivLib(VDIV_DLL)


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
def make_item(memsafe_lib):
    """Factory: make_item(type, val_def, val_min, val_max) -> a standalone AlxParamItem.

    Outside the store chain on purpose: what these tests drive is the string format conversion
    that a CLI set-param reaches, not persistence. Auto-deleted.
    """
    ctxs = []

    def _make(item_type: int, val_def: float = 0.0, val_min: float = 0.0, val_max: float = 0.0):
        ctx = memsafe_lib.item_new(item_type, val_def, val_min, val_max)
        assert ctx, "the item could not be constructed"
        ctxs.append(ctx)
        return ctx

    yield _make
    for ctx in ctxs:
        memsafe_lib.item_delete(ctx)


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
