"""Repository host build declarations and recipes; no pytest fixtures or ctypes access."""

import functools
import re
from pathlib import Path
from typing import NamedTuple

from alx.c_lib import host_build as build_tools

TEST_DIR = Path(__file__).parent
CLIB_DIR = TEST_DIR.parent
BUILD_DIR = TEST_DIR / "build"

FIFO_SOURCES = [
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    TEST_DIR / "alxFifoTestHelpers.c",
    TEST_DIR / "alxBoundTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
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
    # alxTracePc.c SUPPLIES AlxTrace_WriteLevel and the alxTrace instance - no group compiles
    # Mcu/alxTrace.c. Needed the moment a variant compiles a trace form; KEEP IN SYNC with
    # noxfile.CLI_STRICT.
    TEST_DIR / "alxTracePc.c",
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
    TEST_DIR / "alxTracePc.c",   # this group has traces once a variant enables them
    TEST_DIR / "alxMemSafeTestHelpers.c",
]
MEMSAFE_SOURCES_CLOSURE = [
    CLIB_DIR / "alxParamGroup.c",
    CLIB_DIR / "alxParamStore.c",
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
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
    CLIB_DIR / "alxAudioVol.c",
    TEST_DIR / "alxLinFunTestHelpers.c",
    TEST_DIR / "alxAudioVolTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
LINFUN_DEPS = [
    *LINFUN_SOURCES,
    CLIB_DIR / "alxLinFun.h",
    CLIB_DIR / "alxInterpLin.h",
    CLIB_DIR / "alxAudioVol.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxLinFunTest.def",
    Path(__file__),
]
LINFUN_DLL = BUILD_DIR / "alxLinFunTest.dll"


# ------------------------------------------------- Rotary switch module ---
# Tier-2 target: the REAL rotary switch over faked IO pins. The module is a
# handful of pins read together and composed into a number, which is the kind
# of thing that is either right or off by a power of two.
ROTSW_SOURCES = [
    CLIB_DIR / "alxRotSw.c",
    TEST_DIR / "alxIoPinFake.c",
    TEST_DIR / "alxRotSwTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
ROTSW_DEPS = [
    *ROTSW_SOURCES,
    CLIB_DIR / "alxRotSw.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "Mcu" / "alxIoPin.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxRotSwTest.def",
    Path(__file__),
]
ROTSW_DLL = BUILD_DIR / "alxRotSwTest.dll"


# ------------------------------------------ RTD temperature sensor module ---
# Tier-2 target: the REAL sensor over a faked converter. Two channels in, a
# divider, a lookup table out - the chain that turns a voltage into a
# temperature, and every step of it is arithmetic a bench cannot vary.
TEMPSENS_SOURCES = [
    CLIB_DIR / "alxTempSensRtdVdiv.c",
    CLIB_DIR / "alxVdiv.c",
    CLIB_DIR / "alxInterpLin.c",
    TEST_DIR / "alxAdcFake.c",
    TEST_DIR / "alxTempSensTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
TEMPSENS_DEPS = [
    *TEMPSENS_SOURCES,
    CLIB_DIR / "alxTempSensRtdVdiv.h",
    CLIB_DIR / "alxVdiv.h",
    CLIB_DIR / "alxInterpLin.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "Mcu" / "alxAdc.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxTempSensTest.def",
    Path(__file__),
]
TEMPSENS_DLL = BUILD_DIR / "alxTempSensTest.dll"


# ------------------------------------------------ Power supervisor module ---
# Tier-2 target: the REAL supervisor over the REAL divider, hysteresis, glitch
# filter and software timer, with only the interrupt lock faked. One converter
# voltage in, one yes-or-no out, and three decisions between them.
PWR_SOURCES = [
    CLIB_DIR / "alxPwr.c",
    CLIB_DIR / "alxVdiv.c",
    CLIB_DIR / "alxHys2.c",
    CLIB_DIR / "alxFiltGlitchBool.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    TEST_DIR / "alxPwrTestHelpers.c",
    TEST_DIR / "alxIrqFake.c",
    TEST_DIR / "alxAssertPc.c",
]
PWR_DEPS = [
    *PWR_SOURCES,
    CLIB_DIR / "alxPwr.h",
    CLIB_DIR / "alxVdiv.h",
    CLIB_DIR / "alxHys2.h",
    CLIB_DIR / "alxFiltGlitchBool.h",
    CLIB_DIR / "alxTimSw.h",
    CLIB_DIR / "alxTick.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxPwrTest.def",
    Path(__file__),
]
PWR_DLL = BUILD_DIR / "alxPwrTest.dll"


# ------------------------------------------------------- Audio module -----
# Tier-1 target: the sample conversions. No objects, no hardware, no clock -
# seven free functions that turn a PCM sample into a float and back, which is
# where an audio path either keeps its full scale or quietly loses a count.
AUDIO_SOURCES = [
    CLIB_DIR / "alxAudio.c",
    TEST_DIR / "alxAssertPc.c",
]
AUDIO_DEPS = [
    *AUDIO_SOURCES,
    CLIB_DIR / "alxAudio.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxAudioTest.def",
    Path(__file__),
]
AUDIO_DLL = BUILD_DIR / "alxAudioTest.dll"


# ------------------------------------------------ Audio player module -----
# Tier-2 target: the REAL player over the REAL sample conversions. A track is
# an array of bytes the test owns, so "what does it play" is a question with
# an exact answer.
AUDIOPLAYER_SOURCES_STRICT = [
    CLIB_DIR / "alxAudio.c",
    TEST_DIR / "alxAudioPlayerTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
# alxAudioPlayer.c is CLOSURE, not because its tests are pending - they are in
# test_alxAudioPlayer.py - but because it does not survive -Wcast-qual: it reads
# the track through `*((volatile int16_t*)(me->trackPtr + offset))`, and trackPtr
# is a const pointer, so every 16-bit read casts the const away. Nothing is
# written through it, so nothing is undefined; cleaning it up is a LIBRARY change
# and belongs to whoever owns the module (TODO A18).
AUDIOPLAYER_SOURCES_CLOSURE = [
    CLIB_DIR / "alxAudioPlayer.c",
]
AUDIOPLAYER_SOURCES = [*AUDIOPLAYER_SOURCES_STRICT, *AUDIOPLAYER_SOURCES_CLOSURE]
AUDIOPLAYER_DEPS = [
    *AUDIOPLAYER_SOURCES,
    CLIB_DIR / "alxAudioPlayer.h",
    CLIB_DIR / "alxAudio.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxAudioPlayerTest.def",
    Path(__file__),
]
AUDIOPLAYER_DLL = BUILD_DIR / "alxAudioPlayerTest.dll"


# --------------------------------------------------------- NTC module -----
# Tier-1 target: one free function and a 191-entry lookup table. A thermistor
# is the cheapest temperature sensor on a board and the table is the whole
# part - so the test is mostly the table, read back through the function.
# The module is CLOSURE, for one line: it defines LUT_TEMP_MAX_degC and never
# uses it, which -Wunused-macros makes an error. Deleting a dead macro is a
# library change and belongs to whoever owns the module (TODO A18).
NTC_SOURCES_STRICT = [
    CLIB_DIR / "alxBound.c",
    TEST_DIR / "alxAssertPc.c",
]
NTC_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxNtcg103jf103ft1s.c",
]
NTC_SOURCES = [*NTC_SOURCES_STRICT, *NTC_SOURCES_CLOSURE]
NTC_DEPS = [
    *NTC_SOURCES,
    CLIB_DIR / "Ext" / "alxNtcg103jf103ft1s.h",
    CLIB_DIR / "alxBound.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxNtcTest.def",
    Path(__file__),
]
NTC_DLL = BUILD_DIR / "alxNtcTest.dll"


# ------------------------------------------------ Safe file storage -----
# Tier-2 target: two copies of every file, each with a CRC, and the logic that
# decides which to believe and which to repair. It is what makes a stored
# setting survive losing power halfway through writing it, and it had no test.
# Over the same file system fake as the key-value store, so a copy can be
# corrupted on the "flash" before the module ever runs.
# alxFsSafe.c is CLOSURE, and for a real reason: AlxFsSafe_PathToPathWithSuffix
# declares its OUTPUT parameter `const char*` and then sprintf()s into it, which
# clang rejects under -Werror as discarding qualifiers. It is a library change
# in somebody else's module, so it is recorded as TODO A18 rather than fixed
# here - and it means nothing in that file is warning-checked (see README).
FSSAFE_SOURCES_STRICT = [
    CLIB_DIR / "alxCrc.c",
    TEST_DIR / "alxFsSafeTestHelpers.c",
    TEST_DIR / "alxFsFake.c",
    TEST_DIR / "alxAssertPc.c",
]
FSSAFE_SOURCES_CLOSURE = [
    CLIB_DIR / "alxFsSafe.c",
]
FSSAFE_SOURCES = [*FSSAFE_SOURCES_STRICT, *FSSAFE_SOURCES_CLOSURE]
FSSAFE_DEFINES = ["-D_CRT_SECURE_NO_WARNINGS", "-DALX_FS_SAFE_ASSERT_RST_ENABLE"]
FSSAFE_DEPS = [
    *FSSAFE_SOURCES,
    CLIB_DIR / "alxFsSafe.h",
    CLIB_DIR / "alxCrc.h",
    CLIB_DIR / "alxFs.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxFsSafeTest.def",
    Path(__file__),
]
FSSAFE_DLL = BUILD_DIR / "alxFsSafeTest.dll"


# --------------------------------------------------------- LIN bus master -----
# Tier-2 target: the master half of a LIN transaction, over the serial port
# fake. Only the PUBLISH path is covered - it is the one that puts a whole frame
# on the wire in one call, so the protected identifier's parity bits and both
# checksums are readable as BYTES, which is the only place they exist. The
# subscribe half waits on the clock and belongs with a self-advancing tick.
LIN_SOURCES = [
    CLIB_DIR / "alxLin.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    CLIB_DIR / "alxDelay.c",
    TEST_DIR / "alxLinTestHelpers.c",
    TEST_DIR / "alxSerialPortFake.c",
    TEST_DIR / "alxIoPinFake.c",
    TEST_DIR / "alxIrqFake.c",
    TEST_DIR / "alxOsDelayFake.c",
    TEST_DIR / "alxAssertPc.c",
    # The trace sink. Every enabled ALX_TRACE_<LEVEL> macro calls AlxTrace_WriteLevel(&alxTrace, ...)
    # and BOTH of those symbols live in Mcu/alxTrace.c, which no group compiles - measured, turning
    # this group's traces on without it fails to link with exactly those two undefined and no others.
    TEST_DIR / "alxTracePc.c",
]
LIN_DEPS = [
    *LIN_SOURCES,
    CLIB_DIR / "alxLin.h",
    CLIB_DIR / "Mcu" / "alxSerialPort.h",
    CLIB_DIR / "alxGlobal.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxLinTest.def",
    Path(__file__),
]
LIN_DLL = BUILD_DIR / "alxLinTest.dll"


# ------------------------------------------------------- Busy-wait delay -----
# Tier-1 target: six one-line busy waits on the global tick. They cannot be
# tested over the real clock the way the timer is, because a busy wait never
# returns to the test that would have to advance it - so alxTick is FAKED here,
# with a clock that advances by itself one step per read. That is what an
# interrupt does on a target while the loop spins, and it is the only group in
# this suite where the clock is not real.
DELAY_SOURCES = [
    CLIB_DIR / "alxDelay.c",
    TEST_DIR / "alxTickFake.c",
    TEST_DIR / "alxAssertPc.c",
]
DELAY_DEPS = [
    *DELAY_SOURCES,
    CLIB_DIR / "alxDelay.h",
    CLIB_DIR / "alxTick.h",
    CLIB_DIR / "alxGlobal.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxDelayTest.def",
    Path(__file__),
]
DELAY_DLL = BUILD_DIR / "alxDelayTest.dll"


# ------------------------------------------- Parameter key-value store -----
# Tier-2 target: the store every stored parameter on a device passes through,
# over a NEW link-time fake of the file system. The module is a thin shell over
# AlxFs, which makes it look uninteresting until you ask what it does when the
# file system says no - so the fake can be told to fail any operation, once or
# always, and it counts opens against closes.
PARAMKV_SOURCES = [
    CLIB_DIR / "alxParamKvStore.c",
    TEST_DIR / "alxParamKvStoreTestHelpers.c",
    TEST_DIR / "alxFsFake.c",
    TEST_DIR / "alxAssertPc.c",
]
PARAMKV_DEPS = [
    *PARAMKV_SOURCES,
    CLIB_DIR / "alxParamKvStore.h",
    CLIB_DIR / "alxFs.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxParamKvStoreTest.def",
    Path(__file__),
]
PARAMKV_DLL = BUILD_DIR / "alxParamKvStoreTest.dll"


# ------------------------------------------------- Analog multiplexer -----
# Tier-1 target: an enable pin and up to eight select pins, over the library's
# own IO pin fake. Small, and load-bearing - a board that measures more signals
# than it has converter channels reads all of them through one of these, so a
# select code written wrong reports the wrong signal under the right name.
MUX_SOURCES = [
    CLIB_DIR / "alxMux.c",
    TEST_DIR / "alxMuxTestHelpers.c",
    TEST_DIR / "alxIoPinFake.c",
    TEST_DIR / "alxAssertPc.c",
]
MUX_DEPS = [
    *MUX_SOURCES,
    CLIB_DIR / "alxMux.h",
    CLIB_DIR / "Mcu" / "alxIoPin.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxMuxTest.def",
    Path(__file__),
]
MUX_DLL = BUILD_DIR / "alxMuxTest.dll"


# ------------------------------------------- BTS724G high side switch -----
# Tier-2 target: the REAL driver over faked pins, with the REAL glitch filters
# and software timer under it. The part reports two different faults down ONE
# status pin, told apart by whether the output was asked to be on.
BTS_SOURCES = [
    CLIB_DIR / "Ext" / "alxBts724g.c",
    CLIB_DIR / "alxFiltGlitchBool.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    TEST_DIR / "alxIoPinFake.c",
    TEST_DIR / "alxIrqFake.c",
    TEST_DIR / "alxBts724gTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
BTS_DEPS = [
    *BTS_SOURCES,
    CLIB_DIR / "Ext" / "alxBts724g.h",
    CLIB_DIR / "alxFiltGlitchBool.h",
    CLIB_DIR / "alxTimSw.h",
    CLIB_DIR / "alxTick.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "Mcu" / "alxIoPin.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxBts724gTest.def",
    Path(__file__),
]
BTS_DLL = BUILD_DIR / "alxBts724gTest.dll"

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


# ---------------------------------------------------- Ina228 module -------
# Tier-2 target over a faked I2C bus: the current-sense driver this product's
# main power measurement runs through, configured exactly as the board
# configures it. The point is the CONVERSION CHAIN - what a register value
# becomes in amps - which is the arithmetic ALX-1480 is about and which no
# bench measurement can separate from the analog front end.
#
# ALX_INA238 is what the product defines (Usr/alxConfig.h): the part on the
# board is an INA238, and the same driver serves both. The 238 divides its
# maximum expected current by 2^15 where the 228 divides by 2^19, so building
# with the wrong one would silently change every number here by a factor of 16.
#
# The driver and the helper are in the CLOSURE, built with warnings off: the
# library's register bit-fields are declared with ENUM types, whose signedness
# is implementation defined, so clang reports every assignment of a full-width
# enum value as a truncation. The bits that reach the wire are correct - it is
# the header's style, not this suite's, and not something a test should waive
# globally.
INA228_SOURCES_STRICT = [
    TEST_DIR / "alxI2cFake.c",
    TEST_DIR / "alxAssertPc.c",
]
INA228_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxIna228.c",
    TEST_DIR / "alxIna228TestHelpers.c",
]
# Asserts OFF here, unlike the other groups that mirror the product: the
# library's weak AlxIna228_RegStruct_SetVal default is an assert, this suite
# cannot override a weak symbol on a COFF host (see alxIna228TestHelpers.c), and
# with asserts on that default would abort the suite inside Init.
INA228_DEFINES = ["-DALX_INA238"]        # the part the product selects
INA228_DEPS = [
    *INA228_SOURCES_STRICT,
    *INA228_SOURCES_CLOSURE,
    CLIB_DIR / "Ext" / "alxIna228.h",
    CLIB_DIR / "Mcu" / "alxI2c.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxIna228Test.def",
    Path(__file__),
]
INA228_DLL = BUILD_DIR / "alxIna228Test.dll"


# ---------------------------------------------------- Pi4ioe module -------
# Tier-2 target over the same faked I2C bus plus a faked pin: the 40-channel IO
# expander this product drives more of its board through than any other single
# device driver. What it owns is a pin-to-bit mapping and the shape of two bulk
# transfers, which is exactly the kind of thing that is right or wrong and never
# nearly right.
PI4IOE_SOURCES_STRICT = [
    TEST_DIR / "alxI2cFake.c",
    TEST_DIR / "alxIoPinFake.c",
    TEST_DIR / "alxAssertPc.c",
]
PI4IOE_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxPi4ioe5v6534q.c",
    TEST_DIR / "alxPi4ioeTestHelpers.c",
]
PI4IOE_DEFINES: list[str] = []            # asserts off: the weak RegStruct_SetVal default is one
PI4IOE_DEPS = [
    *PI4IOE_SOURCES_STRICT,
    *PI4IOE_SOURCES_CLOSURE,
    CLIB_DIR / "Ext" / "alxPi4ioe5v6534q.h",
    CLIB_DIR / "Mcu" / "alxI2c.h",
    CLIB_DIR / "Mcu" / "alxIoPin.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxPi4ioe5v6534qTest.def",
    Path(__file__),
]
PI4IOE_DLL = BUILD_DIR / "alxPi4ioe5v6534qTest.dll"


# ----------------------------------------------------------- Id module -------
# Tier-2 target (ALX-1553): the identity block every Auralix product prints at boot - firmware
# name, version, build date and hash, the bootloader's own copy of all of that read out of flash,
# and the board identification straps. It had no test of its own until now; the only mention of
# AlxId under Test/ was alxIdFake.c, which is what the CLI group links INSTEAD of this module.
#
# It matters more than its size suggests. The Python library parses this block out of a boot trace
# and the device suite uses it to say WHICH IMAGE a bench run actually ran against, so an identity
# block that is wrong does not break a board - it makes every result describe the wrong binary.
#
# alxId.c is CLOSURE, and it is not close: under the strict set it produces seven errors. Six are
# -Wformat, every "%lu" in the file given a uint32_t (alxId.c:123, 124, 171, 172, 478, 491), and one
# is -Wint-to-void-pointer-cast at alxId.c:141, where the boot block's uint32_t address is cast
# straight to void*. Worth knowing while reading those six: on Windows and on the ARM target
# unsigned long is 32 bits, so the strings come out RIGHT in both places - the format is only
# genuinely wrong on an LP64 host, which is why it has survived. The .c is read-only to this suite
# (TODO A18, the module's owner), so it is compiled with -w and nothing in it is warning-checked.
#
# alxIdTestDateComp.c is closure too, for a different and smaller reason - see its file header: a
# translation unit that mentions ALX_BUILD_DATE_COMP cannot compile under -Werror at all, so the
# three accessors that need the macro are split out and the rest of the helpers stay gated.
ID_SOURCES_STRICT = [
    TEST_DIR / "alxIdTestHelpers.c",
    TEST_DIR / "alxIoPinFake.c",
    TEST_DIR / "alxAssertPc.c",
]
ID_SOURCES_CLOSURE = [
    CLIB_DIR / "alxId.c",
    TEST_DIR / "alxIdTestDateComp.c",
]
ID_SOURCES = [*ID_SOURCES_STRICT, *ID_SOURCES_CLOSURE]
# No alxDelay.c and no alxTrace.c: alxId.h includes alxDelay.h but alxId.c calls nothing from it,
# and Test/alxConfig.h sets ALX_TRACE_LEVEL_OFF, which compiles ALX_TRACE_INF itself away - so the
# module's whole trace paragraph reduces to its two assertions. Assertions come from
# _assert_defines, which reads ALX_ID_ASSERT_BKPT_ENABLE out of alxId.h and reaches closure sources
# too: every getter here asserts isInit, so a test that reads one before Init needs
# @pytest.mark.expect_assert.
ID_DEPS = [
    *ID_SOURCES,
    CLIB_DIR / "alxId.h",
    CLIB_DIR / "alxBuild.h",
    CLIB_DIR / "alxBuild_GENERATED.h",
    CLIB_DIR / "Mcu" / "alxIoPin.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    CLIB_DIR / "alxDelay.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxIdTest.def",
    Path(__file__),
]
ID_DLL = BUILD_DIR / "alxIdTest.dll"


# ------------------------------------------------------- Assert module -------
# Tier-1 target (ALX-1553), and the one module every other group already depends on without ever
# compiling it: alxAssert.c is the funnel all 2293 ALX_*_ASSERT call sites in the library reach.
# Until now it appeared in NO source list here - only alxAssert.h did, in 26 dependency lists -
# because Test/alxAssertPc.c is in every group and supplies strong definitions for all three
# handlers. So the library's own three bodies have run nowhere in this suite.
#
# TWO DLLs, one group, and the second one is not a convenience: all three handlers are ALX_WEAK, a
# strong definition displaces a weak one for the whole image, and both sides of that cannot be
# reachable at once. So the group is built twice from almost the same list.
#
#   ASSERT_WEAK  alxAssert.c WITHOUT alxAssertPc.c - the library's own bodies run. AlxAssert_Rst's
#                default is EMPTY and returns (a shipped board resets only because a product
#                supplies the override), and AlxAssert_Trace's calls AlxTrace_WriteLevel directly
#                rather than through ALX_TRACE_FTL, so ALX_TRACE_LEVEL_OFF does not silence it.
#                Both are observable only here. It exports no AlxAssertPc_* and its wrapper must
#                NOT be registered with _register_lib - safe, because alxAssert.c contains no
#                ALX_*_ASSERT call site of its own for the autouse fixture to watch.
#   ASSERT       alxAssert.c WITH alxAssertPc.c - the same three weak symbols, displaced. It is
#                what proves displacement works at all (nothing else in the suite links a weak
#                default and its override together), and it is the only image where the BKPT form
#                is safe to drive, which is why alxAssertBkptCaller.c is linked here alone.
#
# No assert defines for either: alxAssert.h declares no ALX_<MODULE>_ASSERT_*_ENABLE of its own, so
# _assert_defines returns nothing for it, which is correct - this module is the mechanism, not one
# of its clients. alxAssert.c compiles clean under the full strict set, so neither DLL needs a
# closure.
ASSERT_WEAK_SOURCES = [
    CLIB_DIR / "alxAssert.c",
    TEST_DIR / "alxAssertTestHelpers.c",
]
ASSERT_SOURCES = [
    CLIB_DIR / "alxAssert.c",
    TEST_DIR / "alxAssertTestHelpers.c",
    TEST_DIR / "alxAssertBkptCaller.c",
    TEST_DIR / "alxAssertPc.c",
]
ASSERT_WEAK_DEPS = [
    *ASSERT_WEAK_SOURCES,
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxAssertWeakTest.def",
    Path(__file__),
]
ASSERT_DEPS = [
    *ASSERT_SOURCES,
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxAssertTest.def",
    Path(__file__),
]
ASSERT_WEAK_DLL = BUILD_DIR / "alxAssertWeakTest.dll"
ASSERT_DLL = BUILD_DIR / "alxAssertTest.dll"


# ------------------------------------------------------- MemRaw module -------
# Tier-1 target (ALX-1553): the library's raw-memory contract, and the module a product's persistent
# parameters pass through on their way off the MCU. Its whole public API - constructor, Init, DeInit,
# Read and Write - is ALX_WEAK and implemented nowhere in the repository, which makes it the only
# module here whose complete API is weak rather than a working module with a callback hook bolted on.
#
# It has never been compiled by any lane. The only Test/ file bearing the name is alxMemRawFake.c, a
# complete strong replacement the MemSafe group links INSTEAD of the module, so MEMSAFE_ASSERT_
# DEFINES has carried -DALX_MEM_RAW_ASSERT_RST_ENABLE for a file that group does not compile.
#
# TWO DLLs for the same reason the Assert group has two - a symbol cannot be weak and displaced at
# once:
#
#   MEMRAW      the five weak defaults intact, with alxAssertPc.c so that the four asserting stubs
#               are observable. Each of Init/DeInit/Read/Write is (void)me, ALX_MEM_RAW_ASSERT
#               (false), return 0 - and 0 is Alx_Ok, so a call that cannot work reports success.
#               The constructor is the odd one out: it does not assert, and it sets neither of the
#               two fields the header declares.
#   MEMRAW_OVR  the same, plus alxMemRawTestOverride.c, which is what a product supplies. It
#               overrides FOUR of the five and leaves AlxMemRaw_DeInit weak on purpose, so one
#               image shows displacement is per symbol.
#
# Assertions come from _assert_defines, which reads ALX_MEM_RAW_ASSERT_BKPT_ENABLE out of
# alxMemRaw.h and derives the RST form - do NOT hand-write it. Note what that define is doing here:
# without it the module's own macro is do{} while (false) and DISCARDS the expression, so a product
# that forgets its override gets Alx_Ok, an untouched buffer and no diagnostic at all. alxMemRaw.c
# compiles clean under the full strict set, so neither DLL needs a closure.
MEMRAW_SOURCES = [
    CLIB_DIR / "alxMemRaw.c",
    TEST_DIR / "alxMemRawTestHelpers.c",
    TEST_DIR / "alxAssertPc.c",
]
MEMRAW_OVR_SOURCES = [
    *MEMRAW_SOURCES,
    TEST_DIR / "alxMemRawTestOverride.c",
]
MEMRAW_DEPS = [
    *MEMRAW_SOURCES,
    CLIB_DIR / "alxMemRaw.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxMemRawTest.def",
    Path(__file__),
]
MEMRAW_OVR_DEPS = [
    *MEMRAW_OVR_SOURCES,
    CLIB_DIR / "alxMemRaw.h",
    CLIB_DIR / "alxGlobal.h",
    CLIB_DIR / "alxAssert.h",
    CLIB_DIR / "Mcu" / "alxTrace.h",
    TEST_DIR / "alxConfig.h",
    TEST_DIR / "alxMemRawOvrTest.def",
    Path(__file__),
]
MEMRAW_DLL = BUILD_DIR / "alxMemRawTest.dll"
MEMRAW_OVR_DLL = BUILD_DIR / "alxMemRawOvrTest.dll"


# ------------------------------------------------------------------ build ----
# The mechanics live in the Python lib (alx.c_lib.host_build): where the tools are, the MSVC build
# environment, the rebuild-if-stale check, the compile database and the two DLL recipes. What stays
# here is what is this repository's - the source lists, the defines and the .def files above.
#
# Driver GNU = clang with GNU-style flags, which is what the dev build has always used; the sanitizer
# and coverage lanes use clang-cl on the same recipe (Test/noxfile.py). Dialect gnu99 = what the
# target ships: never test a dialect you do not ship. -O0 -g for faithful debugging; clang's
# diagnostics are front-end based, so the warning set is the same at any -O.
TOOLCHAIN = build_tools.Toolchain()
INCLUDES = [TEST_DIR, CLIB_DIR, CLIB_DIR / "Mcu", CLIB_DIR / "Ext"]
DEBUG_FLAGS = ["-O0", "-g"]
STRICT_WARNINGS = [*build_tools.WARNINGS, "-Werror"]   # blanket -Werror on the host lane
DB_ARGUMENTS = ["clang", "-std=gnu99", "-O0", *build_tools.WARNINGS, build_tools.CRT_DEFINE,
                *[f"-I{d}" for d in INCLUDES]]


def _needs_build(dll: Path, deps) -> bool:
    return build_tools.needs_build(dll, deps)


def write_compile_db() -> None:
    """compile_commands.json for clang-tidy/clangd - same flags as the real build."""
    build_tools.write_compile_db(
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
    build_tools.build_dll(
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
        driver=build_tools.GNU,
    )


class VariantGroup(NamedTuple):
    """A test group that can be built in every named configuration.

    Only BUILD data lives here. The ctypes wrapper deliberately does not: the table has to be
    defined beside the source lists, while the wrapper classes live in host_harness.py.
    The fixture is where the two meet.
    """

    dll: Path
    strict: tuple
    closure: tuple
    deps: list
    def_file: Path
    obj_dir: str | None


# Which groups the variant matrix covers. Adding a row builds four more DLLs and costs nothing else.
#
#   fifo   the assert axis. 12 assertion sites, and ZERO trace sites - so it says nothing at all
#          about the trace half, which is what lin is here for.
#   lin    the trace axis. 23 sites at three levels (16 WRN, 6 DBG, 1 VRB), which straddles the
#          INF/DBG boundary the four variants actually cross, and it is the only module in the
#          library that does so AND already has a group.
VARIANT_GROUPS: dict[str, VariantGroup] = {
    "fifo": VariantGroup(FIFO_DLL, FIFO_SOURCES, (), FIFO_DEPS, TEST_DIR / "alxFifoTest.def", None),
    "lin": VariantGroup(LIN_DLL, LIN_SOURCES, (), LIN_DEPS, TEST_DIR / "alxLinTest.def", None),
    "memsafe": VariantGroup(MEMSAFE_DLL, MEMSAFE_SOURCES_STRICT, MEMSAFE_SOURCES_CLOSURE,
                            MEMSAFE_DEPS, TEST_DIR / "alxMemSafeTest.def", "memSafeClosure"),
    # cli   the ELISION question at its sharpest. alxCli.c does 21 of its serial writes
    #       INSIDE ALX_CLI_ASSERT, so `off` compiles a CLI that answers nothing at all.
    #       The group already had sources, a closure and a .def; only this line was missing.
    "cli": VariantGroup(CLI_DLL, CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE,
                        CLI_DEPS, TEST_DIR / "alxCliTest.def", "cliClosure"),
}


def _variant_dll(group: str, variant: str) -> Path:
    """One DLL per (group, configuration).

    `default` keeps the group's unsuffixed name, so the sanitizer and coverage lanes - which select
    a DLL by path through ALX_<GROUP>_TEST_DLL - carry on without having to learn about variants.
    The suffix is derived with with_name() rather than spelled out, so a group never acquires a
    second spelling of its own DLL name.
    """
    dll = VARIANT_GROUPS[group].dll
    return dll if variant == DEFAULT_VARIANT else dll.with_name(f"{dll.stem}_{variant}.dll")


def _build_variant_dll(group: str, variant: str) -> None:
    g = VARIANT_GROUPS[group]
    _build_dll(g.strict, g.closure, _variant_defines(variant, g.strict, g.closure),
               _variant_dll(group, variant), g.def_file, g.obj_dir)




def _fifo_variant_dll(variant: str) -> Path:
    # Kept as a name of its own because test_alxFifo_variants.py imports it - P529 is a claim about
    # exactly this function.
    return _variant_dll("fifo", variant)


def _build_fifo_dll() -> None:
    # The default group IS the `default` variant now. It used to be _assert_defines() alone, which
    # is assert-RST with no traces and ALX_TRACE_LEVEL_OFF - a combination no product ships.
    _build_variant_dll("fifo", DEFAULT_VARIANT)


def _build_cli_dll() -> None:
    _build_dll(CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE,
               [*CLI_ASSERT_DEFINES, *_assert_defines(CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE)],
               CLI_DLL, TEST_DIR / "alxCliTest.def", "cliClosure")


def _build_memsafe_dll() -> None:
    # The hand-written MEMSAFE_ASSERT_DEFINES went 12.09. Eight of its nine entries were exactly
    # what _assert_defines() derives from the modules' own headers, and the ninth -
    # ALX_MEM_RAW_ASSERT_RST_ENABLE - named a module this group does not compile: it links
    # alxMemRawFake.c, not alxMemRaw.c, so that define had never done anything. That is A31's third
    # gate case, an enable macro for a module the build does not contain, found by hand instead.
    #
    # It also hard-coded RST, which is what would have stopped this group ever joining the variant
    # matrix: `off` would have built as assert-RST and quietly lost the elision configuration.
    _build_variant_dll("memsafe", DEFAULT_VARIANT)


def _build_vdiv_dll() -> None:
    _build_dll(VDIV_SOURCES, (), _assert_defines(VDIV_SOURCES), VDIV_DLL, TEST_DIR / "alxVdivTest.def", None)


def _build_canparser_dll() -> None:
    _build_dll(CANPARSER_SOURCES, (),
               _assert_defines(CANPARSER_SOURCES),
               CANPARSER_DLL, TEST_DIR / "alxCanParserTest.def", None)


def _build_filtglitch_dll() -> None:
    _build_dll(FILTGLITCH_SOURCES, (),
               _assert_defines(FILTGLITCH_SOURCES),
               FILTGLITCH_DLL, TEST_DIR / "alxFiltGlitchTest.def", None)


def _build_math_dll() -> None:
    _build_dll(MATH_SOURCES, (), _assert_defines(MATH_SOURCES), MATH_DLL, TEST_DIR / "alxMathTest.def", None)


def _build_fssafe_dll() -> None:
    _build_dll(FSSAFE_SOURCES_STRICT, FSSAFE_SOURCES_CLOSURE,
               [*FSSAFE_DEFINES, *_assert_defines(FSSAFE_SOURCES_STRICT, FSSAFE_SOURCES_CLOSURE)],
               FSSAFE_DLL,
               TEST_DIR / "alxFsSafeTest.def", "fsSafeClosure")


def _build_lin_dll() -> None:
    # As with FIFO: the plain group is now the `default` variant, which is the only combination a
    # product actually ships. _assert_defines() alone left ALX_TRACE_LEVEL_OFF, so all 23 of
    # alxLin.c's trace call sites were compiled away in every build this suite had ever made.
    _build_variant_dll("lin", DEFAULT_VARIANT)


def _build_delay_dll() -> None:
    _build_dll(DELAY_SOURCES, (), _assert_defines(DELAY_SOURCES), DELAY_DLL,
               TEST_DIR / "alxDelayTest.def", None)


def _build_paramkv_dll() -> None:
    _build_dll(PARAMKV_SOURCES, (), _assert_defines(PARAMKV_SOURCES), PARAMKV_DLL,
               TEST_DIR / "alxParamKvStoreTest.def", None)


def _build_mux_dll() -> None:
    _build_dll(MUX_SOURCES, (), _assert_defines(MUX_SOURCES), MUX_DLL, TEST_DIR / "alxMuxTest.def", None)


def _build_bts_dll() -> None:
    _build_dll(BTS_SOURCES, (), _assert_defines(BTS_SOURCES), BTS_DLL, TEST_DIR / "alxBts724gTest.def", None)


def _build_ntc_dll() -> None:
    _build_dll(NTC_SOURCES_STRICT, NTC_SOURCES_CLOSURE,
               _assert_defines(NTC_SOURCES), NTC_DLL,
               TEST_DIR / "alxNtcTest.def", "ntcClosure")


def _build_audioplayer_dll() -> None:
    _build_dll(AUDIOPLAYER_SOURCES_STRICT, AUDIOPLAYER_SOURCES_CLOSURE,
               _assert_defines(AUDIOPLAYER_SOURCES), AUDIOPLAYER_DLL,
               TEST_DIR / "alxAudioPlayerTest.def", "audioPlayerClosure")


def _build_audio_dll() -> None:
    _build_dll(AUDIO_SOURCES, (), _assert_defines(AUDIO_SOURCES), AUDIO_DLL, TEST_DIR / "alxAudioTest.def", None)


def _build_pwr_dll() -> None:
    _build_dll(PWR_SOURCES, (), _assert_defines(PWR_SOURCES), PWR_DLL, TEST_DIR / "alxPwrTest.def", None)


def _build_tempsens_dll() -> None:
    _build_dll(TEMPSENS_SOURCES, (),
               _assert_defines(TEMPSENS_SOURCES),
               TEMPSENS_DLL, TEST_DIR / "alxTempSensTest.def", None)


def _build_rotsw_dll() -> None:
    _build_dll(ROTSW_SOURCES, (), _assert_defines(ROTSW_SOURCES), ROTSW_DLL, TEST_DIR / "alxRotSwTest.def", None)


def _build_linfun_dll() -> None:
    _build_dll(LINFUN_SOURCES, (), _assert_defines(LINFUN_SOURCES), LINFUN_DLL, TEST_DIR / "alxLinFunTest.def", None)


def _build_bool_dll() -> None:
    _build_dll(BOOL_SOURCES, (), _assert_defines(BOOL_SOURCES), BOOL_DLL, TEST_DIR / "alxBoolTest.def", None)


def _build_rtc_dll() -> None:
    _build_dll(RTC_SOURCES, (), _assert_defines(RTC_SOURCES), RTC_DLL, TEST_DIR / "alxRtcTest.def", None)


def _build_parammgmt_dll() -> None:
    _build_dll(PARAMMGMT_SOURCES_STRICT, PARAMMGMT_SOURCES_CLOSURE,
               [*PARAMMGMT_ASSERT_DEFINES, *_assert_defines(PARAMMGMT_SOURCES_STRICT, PARAMMGMT_SOURCES_CLOSURE)],
               PARAMMGMT_DLL, TEST_DIR / "alxParamMgmtTest.def", "paramMgmtClosure")


def _build_ina228_dll() -> None:
    _build_dll(INA228_SOURCES_STRICT, INA228_SOURCES_CLOSURE,
               [*INA228_DEFINES, *_assert_defines(INA228_SOURCES_STRICT, INA228_SOURCES_CLOSURE)],
               INA228_DLL, TEST_DIR / "alxIna228Test.def", "ina228Closure")


def _build_pi4ioe_dll() -> None:
    _build_dll(PI4IOE_SOURCES_STRICT, PI4IOE_SOURCES_CLOSURE,
               [*PI4IOE_DEFINES, *_assert_defines(PI4IOE_SOURCES_STRICT, PI4IOE_SOURCES_CLOSURE)],
               PI4IOE_DLL, TEST_DIR / "alxPi4ioe5v6534qTest.def", "pi4ioeClosure")


def _build_id_dll() -> None:
    _build_dll(ID_SOURCES_STRICT, ID_SOURCES_CLOSURE,
               _assert_defines(ID_SOURCES_STRICT, ID_SOURCES_CLOSURE),
               ID_DLL, TEST_DIR / "alxIdTest.def", "idClosure")


def _build_timsw_dll() -> None:
    _build_dll(TIMSW_SOURCES, (), _assert_defines(TIMSW_SOURCES), TIMSW_DLL, TEST_DIR / "alxTimSwTest.def", None)


def _build_assert_weak_dll() -> None:
    _build_dll(ASSERT_WEAK_SOURCES, (), _assert_defines(ASSERT_WEAK_SOURCES), ASSERT_WEAK_DLL,
               TEST_DIR / "alxAssertWeakTest.def", None)


def _build_assert_dll() -> None:
    _build_dll(ASSERT_SOURCES, (), _assert_defines(ASSERT_SOURCES), ASSERT_DLL,
               TEST_DIR / "alxAssertTest.def", None)


def _build_memraw_dll() -> None:
    _build_dll(MEMRAW_SOURCES, (), _assert_defines(MEMRAW_SOURCES), MEMRAW_DLL,
               TEST_DIR / "alxMemRawTest.def", None)


def _build_memraw_ovr_dll() -> None:
    _build_dll(MEMRAW_OVR_SOURCES, (), _assert_defines(MEMRAW_OVR_SOURCES), MEMRAW_OVR_DLL,
               TEST_DIR / "alxMemRawOvrTest.def", None)


# The groups as DATA, for anything that must rebuild them without running the suite: the MUTATE
# lane names this list on the command line (alx.c_lib.mutation_hooks rebuild --groups
# host_build:DLL_GROUPS), so the lane needs no script of its own in this repository.
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
    (INA228_DLL, INA228_DEPS, _build_ina228_dll),
    (PI4IOE_DLL, PI4IOE_DEPS, _build_pi4ioe_dll),
    (ROTSW_DLL, ROTSW_DEPS, _build_rotsw_dll),
    (TEMPSENS_DLL, TEMPSENS_DEPS, _build_tempsens_dll),
    (PWR_DLL, PWR_DEPS, _build_pwr_dll),
    (AUDIO_DLL, AUDIO_DEPS, _build_audio_dll),
    (AUDIOPLAYER_DLL, AUDIOPLAYER_DEPS, _build_audioplayer_dll),
    (NTC_DLL, NTC_DEPS, _build_ntc_dll),
    (BTS_DLL, BTS_DEPS, _build_bts_dll),
    (MUX_DLL, MUX_DEPS, _build_mux_dll),
    (PARAMKV_DLL, PARAMKV_DEPS, _build_paramkv_dll),
    (DELAY_DLL, DELAY_DEPS, _build_delay_dll),
    (LIN_DLL, LIN_DEPS, _build_lin_dll),
    (FSSAFE_DLL, FSSAFE_DEPS, _build_fssafe_dll),
    (ID_DLL, ID_DEPS, _build_id_dll),
    (ASSERT_WEAK_DLL, ASSERT_WEAK_DEPS, _build_assert_weak_dll),
    (ASSERT_DLL, ASSERT_DEPS, _build_assert_dll),
    (MEMRAW_DLL, MEMRAW_DEPS, _build_memraw_dll),
    (MEMRAW_OVR_DLL, MEMRAW_OVR_DEPS, _build_memraw_ovr_dll),
]


# ------------------------------------- the library's own assertions, per group -----
# A module built with no ALX_<MODULE>_ASSERT_*_ENABLE macro compiles its assertions down to
# do{} while(false) and DISCARDS the expression - the invariant is not weakened, it is absent.
# Seventeen groups here were doing that until 11.09, when the device repository's mutation lane
# found the same thing: mutants that inverted an assertion survived, because there was nothing
# left of it to change.
#
# The macro name is not derivable from the file name (ALX_FILT_GLITCH_BOOL against
# alxFiltGlitchBool), so it is read out of each module's own header, which is the one place it is
# declared. That also means a group that gains a source gains its assertions with it, and a module
# renamed in the library does not leave a stale define behind.
_ASSERT_MACRO_RE = re.compile(r"#if defined\((ALX_[A-Z0-9_]+)_ASSERT_BKPT_ENABLE\)")


@functools.cache
def _assert_macro(header: Path) -> str | None:
    """The module's own ASSERT enable macro, or None for a module that declares none."""
    try:
        found = _ASSERT_MACRO_RE.search(header.read_text(encoding="utf-8", errors="replace"))
    except OSError:
        return None
    return f"{found.group(1)}_ASSERT_RST_ENABLE" if found else None


def _assert_defines(*source_lists) -> list[str]:
    """-D flags turning on the assertions of every LIBRARY module a group compiles.

    Test helpers and fakes are skipped - they have no assertions of their own, and alxAssertPc.c is
    what the enabled ones land in.
    """
    macros = set()
    for sources in source_lists:
        for source in sources:
            if TEST_DIR in source.parents:
                continue
            macro = _assert_macro(source.with_suffix(".h"))
            if macro:
                macros.add(macro)
    return [f"-D{m}" for m in sorted(macros)]

# ------------------------------------------------------ build variants ---------------------------
# A module's behaviour is not one thing. alxFifo alone has three assert forms plus none, times its
# own trace on or off, times the global ALX_TRACE_LEVEL - eight compiled behaviours from one source
# file, across 12 assert sites. Until 11.09 this suite built exactly ONE of them, did not say which,
# and it was not a combination any product ships: assert-RST like the shipped configuration, but
# ALX_TRACE_LEVEL_OFF, which compiles all 549 of the library's trace call sites away AND DISCARDS
# their arguments.
#
# These four are the standard set. Each earns its place by reaching something none of the others do:
#
#   default       what customers get - alxConfig_TEMPLATE.h's own combination. Must always be green.
#   off           the elision configuration. No assert macro and no traces, so both collapse to
#                 do{} while(false) and discard their expressions - the shape of the biggest defect
#                 ALX-1553 found (NotesClaude 3g), pinned here so it cannot come back unnoticed.
#   debug         the only variant that COMPILES the DBG and VRB call sites, so a wrong format
#                 string or argument count in one of them is visible at all.
#   assert_trace  the only form where a failed assertion CONTINUES into the code after the check.
#                 RST resets and BKPT halts; this one carries on with the precondition violated.
#
# BKPT is safe in a DLL because alxAssertPc.c defines all three handlers strongly - it records where
# the target would break. Without that, `debug` would take the test runner down with it.
VARIANTS: dict[str, tuple[str | None, bool, str]] = {
    #                assert form   module trace   ALX_TRACE_LEVEL
    "default":      ("RST",        True,          "ALX_TRACE_LEVEL_INF"),
    "off":          (None,         False,         "ALX_TRACE_LEVEL_OFF"),
    "debug":        ("BKPT",       True,          "ALX_TRACE_LEVEL_VRB"),
    "assert_trace": ("TRACE",      True,          "ALX_TRACE_LEVEL_INF"),
}
DEFAULT_VARIANT = "default"


def _module_macro_prefix(header: Path) -> str | None:
    """A module's own ALX_<MODULE> macro prefix, read out of its header, or None if it declares no
    assertions. The header is the only place that knows it - deriving it from the file name would
    be a second spelling to keep in step."""
    try:
        found = _ASSERT_MACRO_RE.search(header.read_text(encoding="utf-8", errors="replace"))
    except OSError:
        return None
    return found.group(1) if found else None


def _variant_defines(variant: str, *source_lists) -> list[str]:
    """-D flags putting every LIBRARY module a group compiles into one named configuration.

    Test helpers and fakes are skipped, as in _assert_defines: they have no assertions or traces of
    their own, and alxAssertPc.c is where the enabled ones land.
    """
    form, module_trace, level = VARIANTS[variant]
    macros = {f"ALX_TRACE_LEVEL={level}"}
    for sources in source_lists:
        for source in sources:
            if TEST_DIR in source.parents:
                continue
            prefix = _module_macro_prefix(source.with_suffix(".h"))
            if prefix is None:
                continue
            if form is not None:
                macros.add(f"{prefix}_ASSERT_{form}_ENABLE")
            if module_trace:
                macros.add(f"{prefix}_TRACE_ENABLE")
    return [f"-D{m}" for m in sorted(macros)]
