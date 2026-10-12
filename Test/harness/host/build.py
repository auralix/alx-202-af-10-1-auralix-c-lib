"""Repository host build declarations and recipes; no pytest fixtures or ctypes access."""

import functools
import re
from pathlib import Path
from typing import NamedTuple

from alx.verify import host_build as build_tools

TEST_DIR = Path(__file__).resolve().parents[2]   # Test/, the verification root
CLIB_DIR = TEST_DIR.parent
BUILD_DIR = TEST_DIR / "build"
CONFIG_DIR = TEST_DIR / "config"
HOST_DIR = TEST_DIR / "host"
HELPERS_DIR = HOST_DIR / "helpers"
FAKES_DIR = HOST_DIR / "fakes"
CHECKS_DIR = HOST_DIR / "checks"


FIFO_SOURCES = [
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    HELPERS_DIR / "alxFifoTestHelpers.c",
    HELPERS_DIR / "alxBoundTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
# A group's DEPS are its sources and this file, where its flags live: a source added to a group or
# a flag changed rebuilds it. The headers are the compiler's to list - build_dll records every file
# a build read in <dll>.d and needs_build reads it - so no list here names one. A hand-written list
# that missed Mcu/alxTrace.h let two lane runs pass on a stale LIN DLL (A75).
FIFO_DEPS = [*FIFO_SOURCES, Path(__file__)]
FIFO_DLL = BUILD_DIR / "alxFifoTest.dll"
FIFO_EXPORTS = build_tools.Exports("alxFifoTest", (
    "AlxFifo_Ctor",
    "AlxFifo_Flush",
    "AlxFifo_Read",
    "AlxFifo_ReadStrUntil",
    "AlxFifo_ReadStrUntilAny",
    "AlxFifo_Write",
    "AlxFifo_WriteStr",
    "AlxFifo_GetNumOfEntries",
    "AlxFifo_Rewind",
    "AlxFifoTest_New",
    "AlxFifoTest_Delete",
    "AlxFifoTest_Status_Ok",
    "AlxFifoTest_Status_Err",
    "AlxFifoTest_Status_ErrFull",
    "AlxFifoTest_Status_ErrEmpty",
    "AlxFifoTest_Status_ErrNoDelim",
    "AlxFifoTest_Status_ErrTooLong",
    "AlxBound_Uint8",
    "AlxBound_Uint16",
    "AlxBound_Uint32",
    "AlxBound_Uint64",
    "AlxBound_Int8",
    "AlxBound_Int16",
    "AlxBound_Int32",
    "AlxBound_Int64",
    "AlxBound_Float",
    "AlxBound_Double",
    "AlxBound_Str",
    "AlxBoundTest_Status_Ok",
    "AlxBoundTest_Status_ErrMin",
    "AlxBoundTest_Status_ErrMax",
    "AlxBoundTest_Status_ErrLen",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

# ----------------------------------------------------------- CLI module ------
# Tier-2 target: REAL alxCli + real param stack over the faked serial port and
# KV store, every source under the full strict set (the param stack since goal 7).
CLI_SOURCES_STRICT = [
    CLIB_DIR / "alxCli.c",
    CLIB_DIR / "alxFifo.c",
    CLIB_DIR / "alxBound.c",
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxParamMgmt.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
    FAKES_DIR / "alxSerialPortFake.c",
    FAKES_DIR / "alxParamKvStoreFake.c",
    FAKES_DIR / "alxIdFake.c",
    FAKES_DIR / "alxAssertFake.c",
    # alxTraceFake.c SUPPLIES AlxTrace_WriteLevel and the alxTrace instance - no group compiles
    # Mcu/alxTrace.c. Needed the moment a variant compiles a trace form; KEEP IN SYNC with
    # noxfile.CLI_STRICT.
    FAKES_DIR / "alxTraceFake.c",
    HELPERS_DIR / "alxCliTestHelpers.c",
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
CLI_SOURCES_CLOSURE: list[Path] = []
CLI_DEPS = [*CLI_SOURCES_STRICT, *CLI_SOURCES_CLOSURE, Path(__file__)]
CLI_DLL = BUILD_DIR / "alxCliTest.dll"
CLI_EXPORTS = build_tools.Exports("alxCliTest", (
    "AlxCliTest_New",
    "AlxCliTest_Delete",
    "AlxCliTest_Handle",
    "AlxCliTest_Port",
    "AlxCliTest_GetBuffLen",
    "AlxCliTest_GetStrValBuffLen",
    "AlxCliTest_Status_Ok",
    "AlxCliTest_WasResetRequested",
    "AlxCliTest_ClearResetRequested",
    "AlxSerialPortFake_InjectRx",
    "AlxSerialPortFake_TxRead",
    "AlxSerialPortFake_TxNumOfEntries",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

# ------------------------------------------------------- MemSafe group -------
# Tier-2 target (ALX-1513): REAL alxMemSafe + alxCrc and the REAL param group/store
# chain, all under the strict set (the chain since goal 7), all over alxMemRawFake = a
# 2 kB RAM flash with fault injection and a power-loss model.
MEMSAFE_SOURCES_STRICT = [
    CLIB_DIR / "alxMemSafe.c",
    CLIB_DIR / "alxCrc.c",
    CLIB_DIR / "alxBound.c",
    CLIB_DIR / "alxParamGroup.c",
    CLIB_DIR / "alxParamStore.c",
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
    FAKES_DIR / "alxMemRawFake.c",
    FAKES_DIR / "alxParamKvStoreFake.c",
    FAKES_DIR / "alxAssertFake.c",
    FAKES_DIR / "alxTraceFake.c",   # this group has traces once a variant enables them
    HELPERS_DIR / "alxMemSafeTestHelpers.c",
]
MEMSAFE_SOURCES_CLOSURE: list[Path] = []
MEMSAFE_DEPS = [*MEMSAFE_SOURCES_STRICT, *MEMSAFE_SOURCES_CLOSURE, Path(__file__)]
MEMSAFE_DLL = BUILD_DIR / "alxMemSafeTest.dll"
MEMSAFE_EXPORTS = build_tools.Exports("alxMemSafeTest", (
    "AlxMemSafeTest_New",
    "AlxMemSafeTest_Delete",
    "AlxMemSafeTest_CopyLen",
    "AlxMemSafeTest_NumOfItems",
    "AlxMemSafeTest_MemSafeRead",
    "AlxMemSafeTest_MemSafeWrite",
    "AlxMemSafeTest_MemSafeIsReadDone",
    "AlxMemSafeTest_MemSafeIsReadErr",
    "AlxMemSafeTest_MemSafeIsWriteDone",
    "AlxMemSafeTest_MemSafeIsWriteErr",
    "AlxMemSafeTest_GroupInit",
    "AlxMemSafeTest_GroupIsValStoredBuffDiff",
    "AlxMemSafeTest_StoreInit",
    "AlxMemSafeTest_StoreHandle",
    "AlxMemSafeTest_StoreIsErr",
    "AlxMemSafeTest_ItemGet",
    "AlxMemSafeTest_ItemSet",
    "AlxMemSafeTest_ItemGetDef",
    "AlxMemSafeTest_CrcCalc",
    "AlxMemSafeTest_CrcIsOk",
    "AlxMemSafeTest_CrcLen",
    "AlxMemSafeTest_Status_Ok",
    "AlxMemSafeTest_Status_Err",
    "AlxMemSafeTest_Status_ErrNumOfTries",
    "AlxMemSafeTest_Status_BothCopyErr",
    "AlxMemSafeTest_Status_BothOkSame_UseA",
    "AlxMemSafeTest_Status_BothOkDiff_UseA",
    "AlxMemSafeTest_Status_AOkBErr_UseA",
    "AlxMemSafeTest_Status_AErrBOk_UseB",
    "AlxMemRawFake_Reset",
    "AlxMemRawFake_Fill",
    "AlxMemRawFake_Peek",
    "AlxMemRawFake_Poke",
    "AlxMemRawFake_FailAt",
    "AlxMemRawFake_Count",
    "AlxMemRawFake_PowerLossAt",
    "AlxMemRawFake_PowerOn",
    "AlxMemRawFake_IsPowerLost",
    "AlxMemRawFake_SetRowEraseModel",
    "AlxMemRawFake_Size",
    "AlxMemRawFake_LastNumOfTries",
    "AlxMemRawFake_LastTimeout_ms",
    "AlxMemRawFake_LastWriteAddr",
    "AlxMemRawFake_LastWriteLen",
    "AlxMemRawFake_ArgsMismatchCount",
    "AlxMemRawFake_NotInitCallCount",
    "AlxParamItemStrTest_New",
    "AlxParamItemStrTest_Delete",
    "AlxParamItemStrTest_SetStr",
    "AlxParamItemStrTest_GetStr",
    "AlxParamItemStrTest_GetNum",
    "AlxRange_CheckUint8",
    "AlxRange_CheckUint16",
    "AlxRange_CheckUint32",
    "AlxRange_CheckUint64",
    "AlxRange_CheckInt8",
    "AlxRange_CheckInt16",
    "AlxRange_CheckInt32",
    "AlxRange_CheckInt64",
    "AlxRange_CheckFloat",
    "AlxRange_CheckDouble",
    "AlxRange_CheckStr",
    "AlxFtoa",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
    "AlxTraceFake_Reset",
    "AlxTraceFake_Count",
    "AlxTraceFake_CountAtLevel",
    "AlxTraceFake_LastLevel",
    "AlxTraceFake_LastLine",
    "AlxTraceFake_LastFile",
    "AlxTraceFake_LastFun",
    "AlxTraceFake_LevelConfigured",
    "AlxParamItemMetaTest_New",
    "AlxParamItemMetaTest_Delete",
    "AlxParamItemMetaTest_GetDataType",
    "AlxParamItemMetaTest_GetParamType",
    "AlxParamItemMetaTest_GetKey",
    "AlxParamItemMetaTest_GetId",
    "AlxParamItemMetaTest_GetGroupKey",
    "AlxParamItemMetaTest_GetGroupId",
    "AlxParamItemMetaTest_GetValOutOfRangeHandle",
    "AlxParamItemMetaTest_GetValUnit",
    "AlxParamItemMetaTest_GetValChangeTakesEffectAfterReset",
    "AlxParamItemMetaTest_GetBuffLen",
    "AlxParamItemMetaTest_GetValLen",
    "AlxParamItemMetaTest_GetIsEnum",
    "AlxParamItemMetaTest_GetValDefI",
    "AlxParamItemMetaTest_GetValMinI",
    "AlxParamItemMetaTest_GetValMaxI",
    "AlxParamItemMetaTest_GetValDefF",
    "AlxParamItemMetaTest_GetValMinF",
    "AlxParamItemMetaTest_GetValMaxF",
    "AlxParamItemMetaTest_GetValI",
    "AlxParamItemMetaTest_GetValF",
    "AlxParamItemMetaTest_SetValI",
    "AlxParamItemMetaTest_SetValF",
    "AlxParamItemMetaTest_SetValToDef",
    "AlxParamItemEnumTest_New",
    "AlxParamItemEnumTest_Delete",
    "AlxParamItemEnumTest_GetIsEnum",
    "AlxParamItemEnumTest_GetEnumArrLen",
    "AlxParamItemEnumTest_GetEnumArrAtI",
    "AlxParamItemEnumTest_GetEnumArrAtF",
    "AlxParamItemEnumTest_SetValI",
    "AlxParamItemEnumTest_SetValF",
    "AlxParamItemEnumTest_GetValI",
    "AlxParamItemEnumTest_GetValF",
    "AlxMemSafeTest_Status_ErrEnum",
    "AlxMemSafeTest_Status_ErrConv",
    "AlxParamItemBuffTest_NewArr",
    "AlxParamItemBuffTest_NewStr",
    "AlxParamItemBuffTest_Delete",
    "AlxParamItemBuffTest_GetBuffLen",
    "AlxParamItemBuffTest_GetValLen",
    "AlxParamItemBuffTest_GetKey",
    "AlxParamItemBuffTest_GetValUnit",
    "AlxParamItemBuffTest_GetDataType",
    "AlxParamItemBuffTest_GetValArr",
    "AlxParamItemBuffTest_SetValArr",
    "AlxParamItemBuffTest_GetValDefArr",
    "AlxParamItemBuffTest_GetValStr",
    "AlxParamItemBuffTest_SetValStr",
    "AlxParamItemBuffTest_GetValDefStr",
    "AlxParamItemBuffTest_SetValToDef",
    "AlxStoreTest_New",
    "AlxStoreTest_Delete",
    "AlxStoreTest_MaxGroups",
    "AlxStoreTest_ItemsPerGroup",
    "AlxStoreTest_GroupAddrA",
    "AlxStoreTest_Init",
    "AlxStoreTest_Handle",
    "AlxStoreTest_IsErr",
    "AlxStoreTest_ItemGet",
    "AlxStoreTest_ItemSet",
    "AlxParamItemKvTest_New",
    "AlxParamItemKvTest_Delete",
    "AlxParamItemKvTest_LoadVal",
    "AlxParamItemKvTest_StoreVal",
    "AlxParamItemKvTest_SetVal",
    "AlxParamItemKvTest_GetVal",
    "AlxParamItemKvTest_GetKey",
    "AlxParamKvStoreFake_Reset",
    "AlxParamKvStoreFake_Enable",
    "AlxParamKvStoreFake_FailGet",
    "AlxParamKvStoreFake_FailSet",
    "AlxParamKvStoreFake_GetCount",
    "AlxParamKvStoreFake_SetCount",
    "AlxParamKvStoreFake_NumOfKeys",
    "AlxParamKvStoreFake_Peek",
    "AlxParamKvStoreFake_Poke",
))


# -------------------------------------------------------- Vdiv module -------
# Tier-1 target: a resistive divider's arithmetic, nine context-free functions and
# no state. Its own group because it shares nothing with the store chain: the
# smallest DLL here, and the shape any other pure module should follow.
VDIV_SOURCES = [
    CLIB_DIR / "alxVdiv.c",
    FAKES_DIR / "alxAssertFake.c",
]
VDIV_DEPS = [*VDIV_SOURCES, Path(__file__)]
VDIV_DLL = BUILD_DIR / "alxVdivTest.dll"
VDIV_EXPORTS = build_tools.Exports("alxVdivTest", (
    "AlxVdiv_GetVout_V",
    "AlxVdiv_GetVin_V",
    "AlxVdiv_GetResHigh_kOhm",
    "AlxVdiv_GetResLow_kOhm",
    "AlxVdiv_GetVout_mV",
    "AlxVdiv_GetVin_mV",
    "AlxVdiv_GetResHigh_ohm",
    "AlxVdiv_GetResLow_ohm",
    "AlxVdiv_GetCurrent_uA",
    "AlxVdiv_GetCurrent_mA",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------------ TimSw module -------
# Tier-2 target: the REAL software timer over the REAL tick counter, with the
# interrupt lock faked because it is CMSIS intrinsics that do not exist on a PC.
# The tick is not faked - it is a plain counter the test advances itself, so the
# timer sees real code and the test owns the clock.
TIMSW_SOURCES = [
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
TIMSW_DEPS = [*TIMSW_SOURCES, Path(__file__)]
TIMSW_DLL = BUILD_DIR / "alxTimSwTest.dll"
TIMSW_EXPORTS = build_tools.Exports("alxTimSwTest", (
    "AlxTimSw_Ctor",
    "AlxTimSw_Start",
    "AlxTimSw_Stop",
    "AlxTimSw_IsRunning",
    "AlxTimSw_Get_ns",
    "AlxTimSw_Get_us",
    "AlxTimSw_Get_ms",
    "AlxTimSw_Get_sec",
    "AlxTimSw_Get_min",
    "AlxTimSw_Get_hr",
    "AlxTimSw_IsTimeout_ns",
    "AlxTimSw_IsTimeout_us",
    "AlxTimSw_IsTimeout_ms",
    "AlxTimSw_IsTimeout_sec",
    "AlxTimSw_IsTimeout_min",
    "AlxTimSw_IsTimeout_hr",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_Get_ns",
    "AlxTick_Get_us",
    "AlxTick_Get_ms",
    "AlxTick_Get_sec",
    "AlxTick_Get_min",
    "AlxTick_Get_hr",
    "AlxTick_Inc_ns",
    "AlxTick_Inc_us",
    "AlxTick_Inc_ms",
    "AlxTick_Inc_sec",
    "AlxTick_Inc_min",
    "AlxTick_Inc_hr",
    "AlxTick_IncRange_ns",
    "AlxTick_IncRange_us",
    "AlxTick_IncRange_ms",
    "AlxTick_IncRange_sec",
    "AlxTick_IncRange_min",
    "AlxTick_IncRange_hr",
    "AlxIrqFake_Reset",
    "AlxIrqFake_LockCount",
    "AlxIrqFake_UnlockCount",
    "AlxIrqFake_Depth",
    "AlxIrqFake_DepthMax",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------- CanParser module -------
# Tier-1 target: the CAN payload codec, 24 context-free functions over an
# 8-byte buffer and nothing else. It is the module every product's CAN frame
# passes through, and it needs no CAN peripheral to test: AlxCan_Msg is a plain
# structure, so the whole codec is host code the moment it is compiled here.
CANPARSER_SOURCES = [
    CLIB_DIR / "alxCanParser.c",
    FAKES_DIR / "alxAssertFake.c",
]
CANPARSER_DEPS = [*CANPARSER_SOURCES, Path(__file__)]
CANPARSER_DLL = BUILD_DIR / "alxCanParserTest.dll"
CANPARSER_EXPORTS = build_tools.Exports("alxCanParserTest", (
    "AlxCanParser_SetBit",
    "AlxCanParser_SetUint8",
    "AlxCanParser_SetInt8",
    "AlxCanParser_SetUint16",
    "AlxCanParser_SetInt16",
    "AlxCanParser_SetUint32",
    "AlxCanParser_SetInt32",
    "AlxCanParser_SetUint64",
    "AlxCanParser_SetInt64",
    "AlxCanParser_SetFloat",
    "AlxCanParser_SetDouble",
    "AlxCanParser_SetEnum",
    "AlxCanParser_GetBit",
    "AlxCanParser_GetUint8",
    "AlxCanParser_GetInt8",
    "AlxCanParser_GetUint16",
    "AlxCanParser_GetInt16",
    "AlxCanParser_GetUint32",
    "AlxCanParser_GetInt32",
    "AlxCanParser_GetUint64",
    "AlxCanParser_GetInt64",
    "AlxCanParser_GetFloat",
    "AlxCanParser_GetDouble",
    "AlxCanParser_GetEnum",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
    HELPERS_DIR / "alxFiltGlitchTestHelpers.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
FILTGLITCH_DEPS = [*FILTGLITCH_SOURCES, Path(__file__)]
FILTGLITCH_DLL = BUILD_DIR / "alxFiltGlitchTest.dll"
FILTGLITCH_EXPORTS = build_tools.Exports("alxFiltGlitchTest", (
    "AlxFiltGlitchBoolTest_New",
    "AlxFiltGlitchBoolTest_New_us",
    "AlxFiltGlitchBoolTest_Delete",
    "AlxFiltGlitchBool_Process",
    "AlxFiltGlitchBool_Reset",
    "AlxFiltGlitchUint32Test_New",
    "AlxFiltGlitchUint32Test_Delete",
    "AlxFiltGlitchUint32_Process",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_Get_ns",
    "AlxTick_IncRange_ns",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
    HELPERS_DIR / "alxMathTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
MATH_DEPS = [*MATH_SOURCES, Path(__file__)]
MATH_DLL = BUILD_DIR / "alxMathTest.dll"
MATH_EXPORTS = build_tools.Exports("alxMathTest", (
    "AlxHys1Test_New",
    "AlxHys1Test_Delete",
    "AlxHys1_Process",
    "AlxHys2Test_New",
    "AlxHys2Test_Delete",
    "AlxHys2_Process",
    "AlxAvgTest_New",
    "AlxAvgTest_Delete",
    "AlxAvg_Process",
    "AlxMathTest_New",
    "AlxMathTest_Delete",
    "AlxMath_Process",
    "AlxGlobal_Ulltoa",
    "AlxGlobal_Slltoa",
    "AlxGlobal_Ntohl",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# -------------------------------------------------- Mapping modules -------
# Tier-1 target: the two ways the library turns an x into a y - a straight line
# through two points, and a lookup table interpolated between its points. Both
# are pure, both are how a raw reading becomes an engineering unit, and neither
# had a test.
LINFUN_SOURCES = [
    CLIB_DIR / "alxLinFun.c",
    CLIB_DIR / "alxInterpLin.c",
    CLIB_DIR / "alxAudioVol.c",
    HELPERS_DIR / "alxLinFunTestHelpers.c",
    HELPERS_DIR / "alxAudioVolTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
LINFUN_DEPS = [*LINFUN_SOURCES, Path(__file__)]
LINFUN_DLL = BUILD_DIR / "alxLinFunTest.dll"
LINFUN_EXPORTS = build_tools.Exports("alxLinFunTest", (
    "AlxLinFunTest_New",
    "AlxLinFunTest_Delete",
    "AlxLinFun_GetY",
    "AlxLinFun_GetX",
    "AlxLinFun_GetY_WithStatus",
    "AlxLinFun_GetX_WithStatus",
    "AlxLinFunIntTest_New",
    "AlxLinFunIntTest_Delete",
    "AlxLinFunInt_GetY",
    "AlxLinFunInt_GetY_WithStatus",
    "AlxInterpLinTest_New",
    "AlxInterpLinTest_Delete",
    "AlxInterpLin_GetY",
    "AlxInterpLin_GetY_WithStatus",
    "AlxLinFunTest_Status_Ok",
    "AlxLinFunTest_Status_ErrMin",
    "AlxLinFunTest_Status_ErrMax",
    "AlxAudioVolTest_New",
    "AlxAudioVolTest_Delete",
    "AlxAudioVolTest_PctMax",
    "AlxAudioVol_Process",
    "AlxAudioVol_Set_pct",
    "AlxAudioVol_Set_dB",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------- Rotary switch module ---
# Tier-2 target: the REAL rotary switch over faked IO pins. The module is a
# handful of pins read together and composed into a number, which is the kind
# of thing that is either right or off by a power of two.
ROTSW_SOURCES = [
    CLIB_DIR / "alxRotSw.c",
    FAKES_DIR / "alxIoPinFake.c",
    HELPERS_DIR / "alxRotSwTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
ROTSW_DEPS = [*ROTSW_SOURCES, Path(__file__)]
ROTSW_DLL = BUILD_DIR / "alxRotSwTest.dll"
ROTSW_EXPORTS = build_tools.Exports("alxRotSwTest", (
    "AlxRotSwTest_New",
    "AlxRotSwTest_Delete",
    "AlxRotSwTest_RotSw",
    "AlxRotSwTest_Pin",
    "AlxRotSwTest_CodeType_Real",
    "AlxRotSwTest_CodeType_Complement",
    "AlxRotSwTest_CodeType_Gray",
    "AlxRotSw_Init",
    "AlxRotSw_DeInit",
    "AlxRotSw_GetCode",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------ RTD temperature sensor module ---
# Tier-2 target: the REAL sensor over a faked converter. Two channels in, a
# divider, a lookup table out - the chain that turns a voltage into a
# temperature, and every step of it is arithmetic a bench cannot vary.
TEMPSENS_SOURCES = [
    CLIB_DIR / "alxTempSensRtdVdiv.c",
    CLIB_DIR / "alxVdiv.c",
    CLIB_DIR / "alxInterpLin.c",
    FAKES_DIR / "alxAdcFake.c",
    HELPERS_DIR / "alxTempSensTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
TEMPSENS_DEPS = [*TEMPSENS_SOURCES, Path(__file__)]
TEMPSENS_DLL = BUILD_DIR / "alxTempSensTest.dll"
TEMPSENS_EXPORTS = build_tools.Exports("alxTempSensTest", (
    "AlxTempSensTest_New",
    "AlxTempSensTest_Delete",
    "AlxTempSensTest_Sens",
    "AlxTempSensTest_Adc",
    "AlxTempSensRtdVdiv_Init",
    "AlxTempSensRtdVdiv_DeInit",
    "AlxTempSensRtdVdiv_GetTemp_degC",
    "AlxAdcFake_Reset",
    "AlxAdcFake_SetVoltage_V",
    "AlxAdcFake_ReadCount",
    "AlxAdcFake_InitCount",
    "AlxAdcFake_DeInitCount",
    "AlxTempSensTest_Status_Ok",
    "AlxTempSensTest_Status_ErrMin",
    "AlxTempSensTest_Status_ErrMax",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
    HELPERS_DIR / "alxPwrTestHelpers.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
PWR_DEPS = [*PWR_SOURCES, Path(__file__)]
PWR_DLL = BUILD_DIR / "alxPwrTest.dll"
PWR_EXPORTS = build_tools.Exports("alxPwrTest", (
    "AlxPwrTest_New",
    "AlxPwrTest_Delete",
    "AlxPwrTest_Val_V",
    "AlxPwrTest_HysSt",
    "AlxPwrTest_IsInRangeRaw",
    "AlxPwrTest_HysSt_Top",
    "AlxPwrTest_HysSt_Mid",
    "AlxPwrTest_HysSt_Bot",
    "AlxPwr_Process",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_Get_ns",
    "AlxTick_IncRange_ns",
    "AlxIrqFake_Reset",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------------- Audio module -----
# Tier-1 target: the sample conversions. No objects, no hardware, no clock -
# seven free functions that turn a PCM sample into a float and back, which is
# where an audio path either keeps its full scale or quietly loses a count.
AUDIO_SOURCES = [
    CLIB_DIR / "alxAudio.c",
    FAKES_DIR / "alxAssertFake.c",
]
AUDIO_DEPS = [*AUDIO_SOURCES, Path(__file__)]
AUDIO_DLL = BUILD_DIR / "alxAudioTest.dll"
AUDIO_EXPORTS = build_tools.Exports("alxAudioTest", (
    "AlxAudio_LinerPcmInt8ToFloat",
    "AlxAudio_LinerPcmUint8ToFloat",
    "AlxAudio_LinerPcmInt16ToFloat",
    "AlxAudio_LinerPcmUint16ToFloat",
    "AlxAudio_FloatToLinerPcmInt8",
    "AlxAudio_FloatToLinerPcmInt16",
    "AlxAudio_StereoToMono",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------ Audio player module -----
# Tier-2 target: the REAL player over the REAL sample conversions. A track is
# an array of bytes the test owns, so "what does it play" is a question with
# an exact answer.
AUDIOPLAYER_SOURCES_STRICT = [
    CLIB_DIR / "alxAudio.c",
    CLIB_DIR / "alxAudioPlayer.c",
    HELPERS_DIR / "alxAudioPlayerTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
AUDIOPLAYER_SOURCES_CLOSURE: list[Path] = []
AUDIOPLAYER_SOURCES = [*AUDIOPLAYER_SOURCES_STRICT, *AUDIOPLAYER_SOURCES_CLOSURE]
AUDIOPLAYER_DEPS = [*AUDIOPLAYER_SOURCES, Path(__file__)]
AUDIOPLAYER_DLL = BUILD_DIR / "alxAudioPlayerTest.dll"
AUDIOPLAYER_EXPORTS = build_tools.Exports("alxAudioPlayerTest", (
    "AlxAudioPlayerTest_New",
    "AlxAudioPlayerTest_Delete",
    "AlxAudioPlayerTest_Player",
    "AlxAudioPlayerTest_LoadOther",
    "AlxAudioPlayerTest_Offset_Byte",
    "AlxAudioPlayerTest_Step_Byte",
    "AlxAudioPlayer_GetSampleL",
    "AlxAudioPlayer_GetSampleR",
    "AlxAudioPlayer_GetSampleMono",
    "AlxAudioPlayer_IncSampleOffset",
    "AlxAudioPlayer_Play",
    "AlxAudioPlayer_Stop",
    "AlxAudioPlayer_Pause",
    "AlxAudioPlayer_Replay",
    "AlxAudioPlayer_LoopOn",
    "AlxAudioPlayer_LoopOff",
    "AlxAudioPlayer_LoopConfig",
    "AlxAudioPlayer_IsPlaying",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# --------------------------------------------------------- NTC module -----
# Tier-1 target: one free function and a 191-entry lookup table. A thermistor
# is the cheapest temperature sensor on a board and the table is the whole
# part - so the test is mostly the table, read back through the function.
NTC_SOURCES_STRICT = [
    CLIB_DIR / "alxBound.c",
    CLIB_DIR / "Ext" / "alxNtcg103jf103ft1s.c",
    FAKES_DIR / "alxAssertFake.c",
]
NTC_SOURCES_CLOSURE: list[Path] = []
NTC_SOURCES = [*NTC_SOURCES_STRICT, *NTC_SOURCES_CLOSURE]
NTC_DEPS = [*NTC_SOURCES, Path(__file__)]
NTC_DLL = BUILD_DIR / "alxNtcTest.dll"
NTC_EXPORTS = build_tools.Exports("alxNtcTest", (
    "AlxNtcg103jf103ft1s_ResToTemp_degC",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------ Safe file storage -----
# Tier-2 target: two copies of every file, each with a CRC, and the logic that
# decides which to believe and which to repair. It is what makes a stored
# setting survive losing power halfway through writing it, and it had no test.
# Over the same file system fake as the key-value store, so a copy can be
# corrupted on the "flash" before the module ever runs.
FSSAFE_SOURCES_STRICT = [
    CLIB_DIR / "alxCrc.c",
    CLIB_DIR / "alxFsSafe.c",
    HELPERS_DIR / "alxFsSafeTestHelpers.c",
    FAKES_DIR / "alxFsFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
FSSAFE_SOURCES_CLOSURE: list[Path] = []
FSSAFE_SOURCES = [*FSSAFE_SOURCES_STRICT, *FSSAFE_SOURCES_CLOSURE]
FSSAFE_DEFINES = ["-D_CRT_SECURE_NO_WARNINGS", "-DALX_FS_SAFE_ASSERT_RST_ENABLE"]
FSSAFE_DEPS = [*FSSAFE_SOURCES, Path(__file__)]
FSSAFE_DLL = BUILD_DIR / "alxFsSafeTest.dll"
FSSAFE_EXPORTS = build_tools.Exports("alxFsSafeTest", (
    "AlxFsSafeTest_New",
    "AlxFsSafeTest_Delete",
    "AlxFsSafeTest_FsSafe",
    "AlxFsSafeTest_CrcLen",
    "AlxFsSafeTest_Crc",
    "AlxFsSafe_File_Read",
    "AlxFsSafe_File_Write",
    "AlxFsFake_Reset",
    "AlxFsFake_FailNext",
    "AlxFsFake_FailSkip",
    "AlxFsFake_CallCount",
    "AlxFsFake_OpenCount",
    "AlxFsFake_CloseCount",
    "AlxFsFake_FormatCount",
    "AlxFsFake_FilesHeld",
    "AlxFsFake_IsMounted",
    "AlxFsFake_Put",
    "AlxFsFake_Get",
    "AlxFsFake_Has",
    "AlxFsFake_LastOpenMode",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
    HELPERS_DIR / "alxLinTestHelpers.c",
    FAKES_DIR / "alxSerialPortFake.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxOsDelayFake.c",
    FAKES_DIR / "alxAssertFake.c",
    # The trace sink. Every enabled ALX_TRACE_<LEVEL> macro calls AlxTrace_WriteLevel(&alxTrace, ...)
    # and BOTH of those symbols live in Mcu/alxTrace.c, which no group compiles - measured, turning
    # this group's traces on without it fails to link with exactly those two undefined and no others.
    FAKES_DIR / "alxTraceFake.c",
]
LIN_DEPS = [*LIN_SOURCES, Path(__file__)]
LIN_DLL = BUILD_DIR / "alxLinTest.dll"
LIN_EXPORTS = build_tools.Exports("alxLinTest", (
    "AlxLinTest_New",
    "AlxLinTest_Delete",
    "AlxLinTest_Lin",
    "AlxLinTest_Port",
    "AlxLinTest_Publish",
    "AlxLin_Master_Init",
    "AlxLin_Master_DeInit",
    "AlxLin_Master_IsInit",
    "AlxLin_SetNad",
    "AlxLin_GetNad",
    "AlxSerialPortFake_TxRead",
    "AlxSerialPortFake_TxNumOfEntries",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
    "AlxLinTest_Subscribe",
    "AlxLinTest_RxFlush",
    "AlxLinTest_RxByte",
    "AlxLinTest_TraceProbe",
    "AlxLinTest_TraceReset",
    "AlxLinTest_TraceArgEvalsWrn",
    "AlxLinTest_TraceArgEvalsDbg",
    "AlxLinTest_TraceArgEvalsVrb",
    "AlxLinTest_BumpWrnDirect",
    "AlxLinTest_BumpDbgDirect",
    "AlxLinTest_BumpVrbDirect",
    "AlxTraceFake_Reset",
    "AlxTraceFake_Count",
    "AlxTraceFake_CountAtLevel",
    "AlxTraceFake_LastLevel",
    "AlxTraceFake_LastLine",
    "AlxTraceFake_LastFile",
    "AlxTraceFake_LastFun",
    "AlxTraceFake_LevelConfigured",
))


# ------------------------------------------------------- Busy-wait delay -----
# Tier-1 target: six one-line busy waits on the global tick. They cannot be
# tested over the real clock the way the timer is, because a busy wait never
# returns to the test that would have to advance it - so alxTick is FAKED here,
# with a clock that advances by itself one step per read. That is what an
# interrupt does on a target while the loop spins, and it is the only group in
# this suite where the clock is not real.
DELAY_SOURCES = [
    CLIB_DIR / "alxDelay.c",
    FAKES_DIR / "alxTickFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
DELAY_DEPS = [*DELAY_SOURCES, Path(__file__)]
DELAY_DLL = BUILD_DIR / "alxDelayTest.dll"
DELAY_EXPORTS = build_tools.Exports("alxDelayTest", (
    "AlxDelay_ns",
    "AlxDelay_us",
    "AlxDelay_ms",
    "AlxDelay_sec",
    "AlxDelay_min",
    "AlxDelay_hr",
    "AlxTickFake_Reset",
    "AlxTickFake_SetNow_ns",
    "AlxTickFake_Now_ns",
    "AlxTickFake_Reads",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------- Parameter key-value store -----
# Tier-2 target: the store every stored parameter on a device passes through,
# over a NEW link-time fake of the file system. The module is a thin shell over
# AlxFs, which makes it look uninteresting until you ask what it does when the
# file system says no - so the fake can be told to fail any operation, once or
# always, and it counts opens against closes.
PARAMKV_SOURCES = [
    CLIB_DIR / "alxParamKvStore.c",
    HELPERS_DIR / "alxParamKvStoreTestHelpers.c",
    FAKES_DIR / "alxFsFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
PARAMKV_DEPS = [*PARAMKV_SOURCES, Path(__file__)]
PARAMKV_DLL = BUILD_DIR / "alxParamKvStoreTest.dll"
PARAMKV_EXPORTS = build_tools.Exports("alxParamKvStoreTest", (
    "AlxParamKvStoreTest_New",
    "AlxParamKvStoreTest_Delete",
    "AlxParamKvStoreTest_Store",
    "AlxParamKvStoreTest_IsInit",
    "AlxParamKvStore_Init",
    "AlxParamKvStore_DeInit",
    "AlxParamKvStore_Get",
    "AlxParamKvStore_Set",
    "AlxParamKvStore_Remove",
    "AlxFsFake_Reset",
    "AlxFsFake_FailNext",
    "AlxFsFake_FailSkip",
    "AlxFsFake_CallCount",
    "AlxFsFake_OpenCount",
    "AlxFsFake_CloseCount",
    "AlxFsFake_FormatCount",
    "AlxFsFake_FilesHeld",
    "AlxFsFake_IsMounted",
    "AlxFsFake_Put",
    "AlxFsFake_Get",
    "AlxFsFake_Has",
    "AlxFsFake_LastOpenMode",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------- Analog multiplexer -----
# Tier-1 target: an enable pin and up to eight select pins, over the library's
# own IO pin fake. Small, and load-bearing - a board that measures more signals
# than it has converter channels reads all of them through one of these, so a
# select code written wrong reports the wrong signal under the right name.
MUX_SOURCES = [
    CLIB_DIR / "alxMux.c",
    HELPERS_DIR / "alxMuxTestHelpers.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
MUX_DEPS = [*MUX_SOURCES, Path(__file__)]
MUX_DLL = BUILD_DIR / "alxMuxTest.dll"
MUX_EXPORTS = build_tools.Exports("alxMuxTest", (
    "AlxMuxTest_New",
    "AlxMuxTest_Delete",
    "AlxMuxTest_Mux",
    "AlxMuxTest_EnPin",
    "AlxMuxTest_SelPin",
    "AlxMux_Init",
    "AlxMux_DeInit",
    "AlxMux_DeInit_Select",
    "AlxMux_Enable",
    "AlxMux_Select",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------- BTS724G high side switch -----
# Tier-2 target: the REAL driver over faked pins, with the REAL glitch filters
# and software timer under it. The part reports two different faults down ONE
# status pin, told apart by whether the output was asked to be on.
BTS_SOURCES = [
    CLIB_DIR / "Ext" / "alxBts724g.c",
    CLIB_DIR / "alxFiltGlitchBool.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxIrqFake.c",
    HELPERS_DIR / "alxBts724gTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
BTS_DEPS = [*BTS_SOURCES, Path(__file__)]
BTS_DLL = BUILD_DIR / "alxBts724gTest.dll"
BTS_EXPORTS = build_tools.Exports("alxBts724gTest", (
    "AlxBts724gTest_New",
    "AlxBts724gTest_Delete",
    "AlxBts724gTest_Bts",
    "AlxBts724gTest_OutPin",
    "AlxBts724gTest_StatusPin",
    "AlxBts724gTest_OpenLoadTrue_ms",
    "AlxBts724gTest_OverTempTrue_ms",
    "AlxBts724gTest_ClearTime_ms",
    "AlxBts724g_Init",
    "AlxBts724g_DeInit",
    "AlxBts724g_Handle",
    "AlxBts724g_SetOut",
    "AlxBts724g_ResetOut",
    "AlxBts724g_WriteOut",
    "AlxBts724g_IsOpenLoadDetected",
    "AlxBts724g_IsOverTempDetected",
    "AlxBts724g_WasOpenLoadDetected",
    "AlxBts724g_WasOverTempDetected",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_IncRange_ns",
    "AlxIrqFake_Reset",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

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
    HELPERS_DIR / "alxBoolTestHelpers.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
BOOL_DEPS = [*BOOL_SOURCES, Path(__file__)]
BOOL_DLL = BUILD_DIR / "alxBoolTest.dll"
BOOL_EXPORTS = build_tools.Exports("alxBoolTest", (
    "AlxBoolTest_New",
    "AlxBoolTest_Delete",
    "AlxBool_Update",
    "AlxBool_IsTrue",
    "AlxBool_IsTrueUpToShortTime",
    "AlxBool_IsTrueUpToLongTime",
    "AlxBool_IsTrueForLongTime",
    "AlxBool_WasTrue",
    "AlxBool_WasTrueForShortTime",
    "AlxBool_WasTrueForLongTime",
    "AlxBool_IsFalse",
    "AlxBool_IsFalseUpToShortTime",
    "AlxBool_IsFalseUpToLongTime",
    "AlxBool_IsFalseForLongTime",
    "AlxBool_WasFalse",
    "AlxBool_WasFalseForShortTime",
    "AlxBool_WasFalseForLongTime",
    "AlxBool_ClearWasTrueFlag",
    "AlxBool_ClearWasTrueForShortTimeFlag",
    "AlxBool_ClearWasTrueForLongTimeFlag",
    "AlxBool_ClearWasFalseFlag",
    "AlxBool_ClearWasFalseForShortTimeFlag",
    "AlxBool_ClearWasFalseForLongTimeFlag",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_Get_ns",
    "AlxTick_IncRange_ns",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------------- RTC module -------
# Tier-1 target: the calendar. Sixteen free functions converting between Unix
# time and a broken-down date in four resolutions, and splitting a second into
# its fractions. No object, no clock, no peripheral - and the one module in the
# library whose correct answers can be computed independently, which is what
# these tests do.
RTC_SOURCES = [
    CLIB_DIR / "alxRtc_Global.c",
    FAKES_DIR / "alxAssertFake.c",
]
RTC_DEPS = [*RTC_SOURCES, Path(__file__)]
RTC_DLL = BUILD_DIR / "alxRtcTest.dll"
RTC_EXPORTS = build_tools.Exports("alxRtcTest", (
    "AlxRtc_MsUsNsToNs",
    "AlxRtc_MsUsToNs",
    "AlxRtc_MsToNs",
    "AlxRtc_MsUsNsToSecFract",
    "AlxRtc_NsToMsUsNs",
    "AlxRtc_NsToMsUs",
    "AlxRtc_NsToMs",
    "AlxRtc_SecFractToMsUsNs",
    "AlxRtc_UnixTimeNsToDateTime",
    "AlxRtc_UnixTimeUsToDateTime",
    "AlxRtc_UnixTimeMsToDateTime",
    "AlxRtc_UnixTimeSecToDateTime",
    "AlxRtc_DateTimeToUnixTimeNs",
    "AlxRtc_DateTimeToUnixTimeUs",
    "AlxRtc_DateTimeToUnixTimeMs",
    "AlxRtc_DateTimeToUnixTimeSec",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
    CLIB_DIR / "alxParamItem.c",
    CLIB_DIR / "alxFtoa.c",
    CLIB_DIR / "alxRange.c",
    CLIB_DIR / "alxBound.c",
    FAKES_DIR / "alxParamKvStoreFake.c",   # the items are constructed WITHOUT a store; this is here
    HELPERS_DIR / "alxParamMgmtTestHelpers.c",   # only because alxParamItem.c references the symbols
    FAKES_DIR / "alxAssertFake.c",
]
PARAMMGMT_SOURCES_CLOSURE: list[Path] = []
PARAMMGMT_ASSERT_DEFINES = [
    "-DALX_PARAM_MGMT_ASSERT_RST_ENABLE", "-DALX_PARAM_ITEM_ASSERT_RST_ENABLE",
    "-DALX_BOUND_ASSERT_RST_ENABLE", "-DALX_FTOA_ASSERT_RST_ENABLE",
    "-DALX_RANGE_ASSERT_RST_ENABLE",
]
PARAMMGMT_DEPS = [*PARAMMGMT_SOURCES_STRICT, *PARAMMGMT_SOURCES_CLOSURE, Path(__file__)]
PARAMMGMT_DLL = BUILD_DIR / "alxParamMgmtTest.dll"
PARAMMGMT_EXPORTS = build_tools.Exports("alxParamMgmtTest", (
    "AlxParamMgmtTest_New",
    "AlxParamMgmtTest_Delete",
    "AlxParamMgmtTest_NumOfItems",
    "AlxParamMgmtTest_ParamType_Param",
    "AlxParamMgmtTest_ParamType_Var",
    "AlxParamMgmtTest_DataType_Uint8",
    "AlxParamMgmtTest_DataType_Uint16",
    "AlxParamMgmtTest_DataType_Uint32",
    "AlxParamMgmtTest_DataType_Int32",
    "AlxParamMgmtTest_DataType_Float",
    "AlxParamMgmtTest_DataType_Bool",
    "AlxParamMgmtTest_Status_Ok",
    "AlxParamMgmtTest_Status_Err",
    "AlxParamMgmt_GetNumOfParamItems",
    "AlxParamMgmt_GetNumOfParamTypeItems",
    "AlxParamMgmt_ByIndex_GetDataType",
    "AlxParamMgmt_ByIndex_GetParamType",
    "AlxParamMgmt_ByIndex_GetKey",
    "AlxParamMgmt_ByIndex_GetId",
    "AlxParamMgmt_ByIndex_GetGroupKey",
    "AlxParamMgmt_ByIndex_GetGroupId",
    "AlxParamMgmt_ByIndex_GetValLen",
    "AlxParamMgmt_ByIndex_GetVal_StrFormat",
    "AlxParamMgmt_ByKey_SetVal_StrFormat",
    "AlxParamMgmt_ById_GetValLen",
    "AlxParamMgmt_ById_Get",
    "AlxParamMgmt_ById_Set",
    "AlxParamMgmt_SetValToDef_Group",
    "AlxParamMgmt_SetValToDef_All",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
# globally. Reg_Write's variable length array (-Wvla) waits for the same
# reason: the register unions are larger on this host than on the target, so a
# buffer sized for the target, as Reg_Read's is, overflows here (A93).
INA228_SOURCES_STRICT = [
    FAKES_DIR / "alxI2cFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
INA228_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxIna228.c",
    HELPERS_DIR / "alxIna228TestHelpers.c",
]
# Asserts OFF here, unlike the other groups that mirror the product: the
# library's weak AlxIna228_RegStruct_SetVal default is an assert, this suite
# cannot override a weak symbol on a COFF host (see alxIna228TestHelpers.c), and
# with asserts on that default would abort the suite inside Init.
INA228_DEFINES = ["-DALX_INA238"]        # the part the product selects
INA228_DEPS = [*INA228_SOURCES_STRICT, *INA228_SOURCES_CLOSURE, Path(__file__)]
INA228_DLL = BUILD_DIR / "alxIna228Test.dll"
INA228_EXPORTS = build_tools.Exports("alxIna228Test", (
    "AlxIna228Test_New",
    "AlxIna228Test_Delete",
    "AlxIna228Test_CurrentLsb_A",
    "AlxIna228Test_ShuntCal",
    "AlxIna228Test_AdcRange_163_84_mV",
    "AlxIna228Test_AdcRange_40_96_mV",
    "AlxIna228Test_Status_Ok",
    "AlxIna228Test_Status_Err",
    "AlxIna228_GetShuntVoltage_V",
    "AlxIna228_GetBusVoltage_V",
    "AlxIna228_GetTemp_degC",
    "AlxIna228_GetCurrent_A",
    "AlxIna228_GetPower_W",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ---------------------------------------------------- Pi4ioe module -------
# Tier-2 target over the same faked I2C bus plus a faked pin: the 40-channel IO
# expander this product drives more of its board through than any other single
# device driver. What it owns is a pin-to-bit mapping and the shape of two bulk
# transfers, which is exactly the kind of thing that is right or wrong and never
# nearly right.
PI4IOE_SOURCES_STRICT = [
    CLIB_DIR / "Ext" / "alxPi4ioe5v6534q.c",
    FAKES_DIR / "alxI2cFake.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
PI4IOE_SOURCES_CLOSURE = [
    HELPERS_DIR / "alxPi4ioeTestHelpers.c",
]
PI4IOE_DEFINES: list[str] = []            # asserts off: the weak RegStruct_SetVal default is one
PI4IOE_DEPS = [*PI4IOE_SOURCES_STRICT, *PI4IOE_SOURCES_CLOSURE, Path(__file__)]
PI4IOE_DLL = BUILD_DIR / "alxPi4ioe5v6534qTest.dll"
PI4IOE_EXPORTS = build_tools.Exports("alxPi4ioe5v6534qTest", (
    "AlxPi4ioeTest_New",
    "AlxPi4ioeTest_Delete",
    "AlxPi4ioeTest_ResetPin",
    "AlxPi4ioeTest_InputPortAddr",
    "AlxPi4ioeTest_OutputPortAddr",
    "AlxPi4ioeTest_Status_Ok",
    "AlxPi4ioeTest_Status_Err",
    "AlxPi4ioe5v6534q_Handle",
    "AlxPi4ioe5v6534q_IoPin_Read",
    "AlxPi4ioe5v6534q_IoPin_Write",
    "AlxPi4ioe5v6534q_IoPin_Set",
    "AlxPi4ioe5v6534q_IoPin_Reset",
    "AlxPi4ioe5v6534q_IoPin_Toggle",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetForcedStatus",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxIoPinFake_WriteCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
ID_SOURCES_STRICT = [
    CLIB_DIR / "alxId.c",
    HELPERS_DIR / "alxIdTestHelpers.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
ID_SOURCES_CLOSURE: list[Path] = []
ID_SOURCES = [*ID_SOURCES_STRICT, *ID_SOURCES_CLOSURE]
# No alxDelay.c and no alxTrace.c: alxId.h includes alxDelay.h but alxId.c calls nothing from it,
# and Test/alxConfig.h sets ALX_TRACE_LEVEL_OFF, which compiles ALX_TRACE_INF itself away - so the
# module's whole trace paragraph reduces to its two assertions. Assertions come from
# _assert_defines, which reads ALX_ID_ASSERT_BKPT_ENABLE out of alxId.h and reaches closure sources
# too: every getter here asserts isInit, so a test that reads one before Init needs
# @pytest.mark.expect_assert.
ID_DEPS = [*ID_SOURCES, Path(__file__)]
ID_DLL = BUILD_DIR / "alxIdTest.dll"
ID_EXPORTS = build_tools.Exports("alxIdTest", (
    "AlxIdTest_Alloc",
    "AlxIdTest_Delete",
    "AlxIdTest_Pin",
    "AlxIdTest_Instance",
    "AlxIdTest_Known",
    "AlxIdTest_SetSupported",
    "AlxIdTest_InstanceSetPcb",
    "AlxIdTest_InstanceSetBom",
    "AlxIdTest_Ctor",
    "AlxIdTest_CtorNoHwId",
    "AlxIdTest_IsInit",
    "AlxIdTest_IsHwIdUsed",
    "AlxIdTest_IdIoPinState",
    "AlxIdTest_CalcHwId",
    "AlxIdTest_TriState_HiZ",
    "AlxIdTest_TriState_Hi",
    "AlxIdTest_TriState_Lo",
    "AlxIdTest_TriState_Undefined",
    "AlxIdTest_FlashAlloc",
    "AlxIdTest_FlashFree",
    "AlxIdTest_FlashFill",
    "AlxIdTest_BootBlobWrite",
    "AlxIdTest_BootBlobFillField",
    "AlxIdTest_BootBlobLen",
    "AlxIdTest_BootBlobMagicNum",
    "AlxIdTest_BootBlobVer",
    "AlxIdTest_BuildName",
    "AlxIdTest_BuildDate",
    "AlxIdTest_BuildNum",
    "AlxIdTest_BuildRev",
    "AlxIdTest_BuildHash",
    "AlxIdTest_BuildHashShort",
    "AlxIdTest_BuildHashShortUint32",
    "AlxIdTest_BuildDateComp",
    "AlxIdTest_CompDate",
    "AlxIdTest_CompTime",
    "AlxId_Init",
    "AlxId_Trace",
    "AlxId_GetFwIsBootUsed",
    "AlxId_GetFwArtf",
    "AlxId_GetFwName",
    "AlxId_GetFwVerMajor",
    "AlxId_GetFwVerMinor",
    "AlxId_GetFwVerPatch",
    "AlxId_GetFwVerDate",
    "AlxId_GetFwVer",
    "AlxId_GetFwHashShort",
    "AlxId_GetFwVerStr",
    "AlxId_GetFwBinStr",
    "AlxId_GetFwBootArtf",
    "AlxId_GetFwBootName",
    "AlxId_GetFwBootVerMajor",
    "AlxId_GetFwBootVerMinor",
    "AlxId_GetFwBootVerPatch",
    "AlxId_GetFwBootVerDate",
    "AlxId_GetFwBootVer",
    "AlxId_GetFwBootHashShort",
    "AlxId_GetFwBootVerStr",
    "AlxId_GetFwBootBinStr",
    "AlxId_GetHwPcbArtf",
    "AlxId_GetHwPcbName",
    "AlxId_GetHwPcbVerMajor",
    "AlxId_GetHwPcbVerMinor",
    "AlxId_GetHwPcbVerPatch",
    "AlxId_GetHwPcbVerDate",
    "AlxId_GetHwPcbVer",
    "AlxId_GetHwPcbVerStr",
    "AlxId_GetHwBomArtf",
    "AlxId_GetHwBomName",
    "AlxId_GetHwBomVerMajor",
    "AlxId_GetHwBomVerMinor",
    "AlxId_GetHwBomVerPatch",
    "AlxId_GetHwBomVerDate",
    "AlxId_GetHwBomVer",
    "AlxId_GetHwBomVerStr",
    "AlxId_GetHwId",
    "AlxId_GetHwMcuUniqueIdStr",
    "AlxId_GetHwMcuUniqueIdUint32",
    "AlxId_GetHwMcuUniqueIdUint8",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_SetTriState",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
    "AlxIdTest_FwCompName",
    "AlxIdTest_FwCompVerMajor",
    "AlxIdTest_FwLangCVer",
    "AlxIdTest_FwLangCLibName",
))


# ------------------------------------------------------- Assert module -------
# Tier-1 target (ALX-1553), and the one module every other group already depends on without ever
# compiling it: alxAssert.c is the funnel all 2293 ALX_*_ASSERT call sites in the library reach.
# Until now it appeared in NO source list here - only alxAssert.h did, in 26 dependency lists -
# because Test/alxAssertFake.c is in every group and supplies strong definitions for all three
# handlers. So the library's own three bodies have run nowhere in this suite.
#
# TWO DLLs, one group, and the second one is not a convenience: all three handlers are ALX_WEAK, a
# strong definition displaces a weak one for the whole image, and both sides of that cannot be
# reachable at once. So the group is built twice from almost the same list.
#
#   ASSERT_WEAK  alxAssert.c WITHOUT alxAssertFake.c - the library's own bodies run. AlxAssert_Rst's
#                default is EMPTY and returns (a shipped board resets only because a product
#                supplies the override), and AlxAssert_Trace's calls AlxTrace_WriteLevel directly
#                rather than through ALX_TRACE_FTL, so ALX_TRACE_LEVEL_OFF does not silence it.
#                Both are observable only here. It exports no AlxAssertFake_* and its wrapper must
#                NOT be registered with _register_lib - safe, because alxAssert.c contains no
#                ALX_*_ASSERT call site of its own for the autouse fixture to watch.
#   ASSERT       alxAssert.c WITH alxAssertFake.c - the same three weak symbols, displaced. It is
#                what proves displacement works at all (nothing else in the suite links a weak
#                default and its override together), and it is the only image where the BKPT form
#                is safe to drive, which is why alxAssertTestHelpers_Bkpt.c is linked here alone.
#
# No assert defines for either: alxAssert.h declares no ALX_<MODULE>_ASSERT_*_ENABLE of its own, so
# _assert_defines returns nothing for it, which is correct - this module is the mechanism, not one
# of its clients. alxAssert.c compiles clean under the full strict set, so neither DLL needs a
# closure.
ASSERT_WEAK_SOURCES = [
    CLIB_DIR / "alxAssert.c",
    HELPERS_DIR / "alxAssertTestHelpers.c",
]
ASSERT_SOURCES = [
    CLIB_DIR / "alxAssert.c",
    HELPERS_DIR / "alxAssertTestHelpers.c",
    HELPERS_DIR / "alxAssertTestHelpers_Bkpt.c",
    FAKES_DIR / "alxAssertFake.c",
]
ASSERT_WEAK_DEPS = [*ASSERT_WEAK_SOURCES, Path(__file__)]
ASSERT_DEPS = [*ASSERT_SOURCES, Path(__file__)]
ASSERT_WEAK_DLL = BUILD_DIR / "alxAssertWeakTest.dll"
ASSERT_DLL = BUILD_DIR / "alxAssertTest.dll"
ASSERT_WEAK_EXPORTS = build_tools.Exports("alxAssertWeakTest", (
    "AlxAssertTest_Reset",
    "AlxAssertTest_TraceCount",
    "AlxAssertTest_TraceLevel",
    "AlxAssertTest_TraceText",
    "AlxAssertTest_TraceFile",
    "AlxAssertTest_TraceFun",
    "AlxAssertTest_TraceLine",
    "AlxAssertTest_TraceLevelFtl",
    "AlxAssertTest_TraceLevelConfigured",
    "AlxAssertTest_TraceLevelOff",
    "AlxAssertTest_File",
    "AlxAssertTest_SideEffects",
    "AlxAssertTest_SideEffectsOff",
    "AlxAssertTest_DriveRst",
    "AlxAssertTest_DriveTrace",
    "AlxAssertTest_DriveRstSideEffect",
    "AlxAssertTest_DriveOffSideEffect",
    "AlxAssertTest_BumpOffDirect",
    "AlxAssertTest_LineRst",
    "AlxAssertTest_LineTrace",
    "AlxAssertTest_ReachedAfterRst",
    "AlxAssertTest_ReachedAfterTrace",
    "AlxAssertTest_CallRstDirect",
    "AlxAssertTest_CallTraceDirect",
))
ASSERT_EXPORTS = build_tools.Exports("alxAssertTest", (
    "AlxAssertTest_Reset",
    "AlxAssertTest_TraceCount",
    "AlxAssertTest_TraceLevel",
    "AlxAssertTest_TraceText",
    "AlxAssertTest_TraceFile",
    "AlxAssertTest_TraceFun",
    "AlxAssertTest_TraceLine",
    "AlxAssertTest_TraceLevelFtl",
    "AlxAssertTest_TraceLevelConfigured",
    "AlxAssertTest_TraceLevelOff",
    "AlxAssertTest_File",
    "AlxAssertTest_SideEffects",
    "AlxAssertTest_SideEffectsOff",
    "AlxAssertTest_DriveRst",
    "AlxAssertTest_DriveTrace",
    "AlxAssertTest_DriveRstSideEffect",
    "AlxAssertTest_DriveOffSideEffect",
    "AlxAssertTest_BumpOffDirect",
    "AlxAssertTest_LineRst",
    "AlxAssertTest_LineTrace",
    "AlxAssertTest_ReachedAfterRst",
    "AlxAssertTest_ReachedAfterTrace",
    "AlxAssertTest_CallRstDirect",
    "AlxAssertTest_CallTraceDirect",
    "AlxAssertTest_DriveBkpt",
    "AlxAssertTest_LineBkpt",
    "AlxAssertTest_FileBkpt",
    "AlxAssertTest_ReachedAfterBkpt",
    "AlxAssertTest_CallBkptDirect",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


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
#   MEMRAW      the five weak defaults intact, with alxAssertFake.c so that the four asserting stubs
#               are observable. Each of Init/DeInit/Read/Write is (void)me, ALX_MEM_RAW_ASSERT
#               (false), return 0 - and 0 is Alx_Ok, so a call that cannot work reports success.
#               The constructor is the odd one out: it does not assert, and it sets neither of the
#               two fields the header declares.
#   MEMRAW_OVR  the same, plus alxMemRawTestHelpers_Override.c, which is what a product supplies. It
#               overrides FOUR of the five and leaves AlxMemRaw_DeInit weak on purpose, so one
#               image shows displacement is per symbol.
#
# Assertions come from _assert_defines, which reads ALX_MEM_RAW_ASSERT_BKPT_ENABLE out of
# alxMemRaw.h and derives the RST form - do NOT hand-write it. Note what that define is doing here:
# without it the module's own macro is ALX_ASSERT_OFF and never evaluates the expression, so a product
# that forgets its override gets Alx_Ok, an untouched buffer and no diagnostic at all. alxMemRaw.c
# compiles clean under the full strict set, so neither DLL needs a closure.
MEMRAW_SOURCES = [
    CLIB_DIR / "alxMemRaw.c",
    HELPERS_DIR / "alxMemRawTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
MEMRAW_OVR_SOURCES = [
    *MEMRAW_SOURCES,
    HELPERS_DIR / "alxMemRawTestHelpers_Override.c",
]
MEMRAW_DEPS = [*MEMRAW_SOURCES, Path(__file__)]
MEMRAW_OVR_DEPS = [*MEMRAW_OVR_SOURCES, Path(__file__)]
MEMRAW_DLL = BUILD_DIR / "alxMemRawTest.dll"
MEMRAW_OVR_DLL = BUILD_DIR / "alxMemRawOvrTest.dll"
MEMRAW_EXPORTS = build_tools.Exports("alxMemRawTest", (
    "AlxMemRawTest_New",
    "AlxMemRawTest_NewNoCtor",
    "AlxMemRawTest_Delete",
    "AlxMemRawTest_WasCtorCalled",
    "AlxMemRawTest_IsInit",
    "AlxMemRawTest_SizeOf",
    "AlxMemRawTest_PoisonByte",
    "AlxMemRawTest_Init",
    "AlxMemRawTest_DeInit",
    "AlxMemRawTest_Read",
    "AlxMemRawTest_Write",
    "AlxMemRawTest_BuffFill",
    "AlxMemRawTest_BuffPeek",
    "AlxMemRawTest_BuffPoke",
    "AlxMemRawTest_BuffLen",
    "AlxMemRawTest_Status_Ok",
    "AlxMemRawTest_Status_Err",
    "AlxMemRawTest_Status_ErrNumOfTries",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))
MEMRAW_OVR_EXPORTS = build_tools.Exports("alxMemRawOvrTest", (
    "AlxMemRawTest_New",
    "AlxMemRawTest_NewNoCtor",
    "AlxMemRawTest_Delete",
    "AlxMemRawTest_WasCtorCalled",
    "AlxMemRawTest_IsInit",
    "AlxMemRawTest_SizeOf",
    "AlxMemRawTest_PoisonByte",
    "AlxMemRawTest_Init",
    "AlxMemRawTest_DeInit",
    "AlxMemRawTest_Read",
    "AlxMemRawTest_Write",
    "AlxMemRawTest_BuffFill",
    "AlxMemRawTest_BuffPeek",
    "AlxMemRawTest_BuffPoke",
    "AlxMemRawTest_BuffLen",
    "AlxMemRawTest_Status_Ok",
    "AlxMemRawTest_Status_Err",
    "AlxMemRawTest_Status_ErrNumOfTries",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
    "AlxMemRawTest_OverrideReset",
    "AlxMemRawTest_OverrideCtorCount",
    "AlxMemRawTest_OverrideInitCount",
    "AlxMemRawTest_OverrideReadCount",
    "AlxMemRawTest_OverrideWriteCount",
    "AlxMemRawTest_OverrideLastAddr",
    "AlxMemRawTest_OverrideLastLen",
    "AlxMemRawTest_OverrideLastNumOfTries",
    "AlxMemRawTest_OverrideLastTimeout_ms",
    "AlxMemRawTest_OverrideLastCheckWithReadEnable",
    "AlxMemRawTest_OverridePeek",
    "AlxMemRawTest_OverridePoke",
    "AlxMemRawTest_OverrideSize",
))


# ------------------------------------------------------------ the fakes themselves -----
# The fakes under host/fakes/ are the library's own files: a consumer links them in place of the
# modules they fake, so each fake's controls are part of what the library promises. Most fakes are
# also linked by the group of a module that calls the faked one; the ones written for consumers
# are linked by no other group. This group links EVERY fake into one image, which is itself a
# proof - a consumer may link any subset, and no two may collide - and tests/framework/
# test_fakes.py proves each control contract once.
FAKES_SOURCES = [
    *sorted(FAKES_DIR.glob("*.c")),
    CLIB_DIR / "alxFifo.c",     # the serial port fake is two FIFOs
    CLIB_DIR / "alxBound.c",    # which bound their lengths
    HELPERS_DIR / "alxFakesTestHelpers.c",
]
FAKES_DEPS = [*FAKES_SOURCES, Path(__file__)]
FAKES_DLL = BUILD_DIR / "alxFakesTest.dll"
FAKES_EXPORTS = build_tools.Exports("alxFakesTest", (
    # ADC
    "AlxAdcFake_Reset",
    "AlxAdcFake_SetVoltage_V",
    "AlxAdcFake_SetVoltageSeq_V",
    "AlxAdcFake_ReadCount",
    "AlxAdcFake_InitCount",
    "AlxAdcFake_DeInitCount",
    "AlxAdc_Init",
    "AlxAdc_DeInit",
    "AlxAdc_GetVoltage_V",
    "AlxAdc_GetVoltage_mV",
    # CAN
    "AlxCanFake_Reset",
    "AlxCanFake_ClearTx",
    "AlxCanFake_TxCount",
    "AlxCanFake_TxMsg",
    "AlxCanFake_QueueRxMsg",
    "AlxCanFake_RxMsgCount",
    "AlxCanFake_SetErr",
    "AlxCan_TxMsg",
    "AlxCan_RxMsg",
    "AlxCan_IsErr",
    # serial port
    "AlxSerialPortFake_Reset",
    "AlxSerialPortFake_Register",
    "AlxSerialPortFake_InjectRx",
    "AlxSerialPortFake_TxRead",
    "AlxSerialPort_ReadStrUntilAny",
    "AlxSerialPort_Write",
    # PWM
    "AlxPwmFake_Reset",
    "AlxPwmFake_Duty_pct",
    "AlxPwmFake_WriteCount",
    "AlxPwm_SetDuty_pct",
    "AlxPwm_SetDuty_permil",
    # SPI
    "AlxSpiFake_Reset",
    "AlxSpiFake_QueueMiso",
    "AlxSpiFake_MosiLen",
    "AlxSpiFake_Mosi",
    "AlxSpiFake_MisoLeft",
    "AlxSpiFake_CsAssertCount",
    "AlxSpiFake_IsCsAsserted",
    "AlxSpiFake_InitCount",
    "AlxSpiFake_DeInitCount",
    "AlxSpiFake_SetForcedStatus",
    "AlxSpi_Init",
    "AlxSpi_DeInit",
    "AlxSpi_Master_Write",
    "AlxSpi_Master_Read",
    "AlxSpi_Master_WriteRead",
    "AlxSpi_Master_AssertCs",
    "AlxSpi_Master_DeAssertCs",
    # I2S
    "AlxI2sFake_Reset",
    "AlxI2sFake_InitCount",
    "AlxI2sFake_DeInitCount",
    "AlxI2sFake_HandleCount",
    "AlxI2s_Init",
    "AlxI2s_DeInit",
    "AlxI2s_Foreground_Handle",
    # INA228
    "AlxIna228Fake_Reset",
    "AlxIna228Fake_SetCurrent_A",
    "AlxIna228Fake_SetBusVoltage_V",
    "AlxIna228Fake_SetShuntVoltage_V",
    "AlxIna228Fake_SetPower_W",
    "AlxIna228Fake_SetTemp_degC",
    "AlxIna228_GetCurrent_A",
    "AlxIna228_GetBusVoltage_V",
    "AlxIna228_GetShuntVoltage_V",
    "AlxIna228_GetPower_W",
    "AlxIna228_GetTemp_degC",
    # PI4IOE5V6534Q
    "AlxPi4ioe5v6534qFake_Reset",
    "AlxPi4ioe5v6534qFake_SetLevel",
    "AlxPi4ioe5v6534qFake_Level",
    "AlxPi4ioe5v6534q_IoPin_Read",
    "AlxPi4ioe5v6534q_IoPin_Write",
    # LP586x
    "AlxLp586xFake_Reset",
    "AlxLp586xFake_SetPresent",
    "AlxLp586xFake_InitPeriphCount",
    "AlxLp586xFake_DeInitPeriphCount",
    "AlxLp586xFake_InitCount",
    "AlxLp586xFake_HandleCount",
    "AlxLp586xFake_LedWriteCount",
    "AlxLp586x_InitPeriph",
    "AlxLp586x_DeInitPeriph",
    "AlxLp586x_Init",
    "AlxLp586x_Handle",
    "AlxLp586x_Led_Write",
    # ID
    "AlxIdFake_Reset",
    "AlxIdFake_SetHwId",
    "AlxId_GetFwArtf",
    "AlxId_GetFwName",
    "AlxId_GetFwVerStr",
    "AlxId_GetFwBinStr",
    "AlxId_GetHwId",
    "AlxId_GetHwMcuUniqueIdStr",
    # IO pin
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_WriteCount",
    "AlxIoPin_Toggle",
    # delay and watchdog
    "AlxDelayFake_Reset",
    "AlxDelayFake_MsCount",
    "AlxDelayFake_UsCount",
    "AlxDelay_ms",
    "AlxDelay_us",
    "AlxWdtFake_Reset",
    "AlxWdtFake_RefreshCount",
    "AlxWdt_Init",
    "AlxWdt_Refresh",
    # safe memory
    "AlxMemSafeFake_Reset",
    "AlxMemSafe_Write",
    "AlxMemSafe_Read",
    "AlxMemSafe_IsWriteDone",
    "AlxMemSafe_IsWriteErr",
    # key-value store
    "AlxParamKvStoreFake_Reset",
    "AlxParamKvStoreFake_Enable",
    "AlxParamKvStoreFake_NumOfKeys",
    "AlxParamKvStore_Ctor",
    "AlxParamKvStore_Init",
    "AlxParamKvStore_Get",
    "AlxParamKvStore_Set",
    # the constructor-only fakes' portable entry points
    "AlxBoot_Ctor",
    "AlxBoot_App_Usb_Update",
    "AlxClk_Init",
    "AlxFs_Ctor",
    "AlxRst_Init",
    "AlxRst_Trace",
    "AlxTmp1075_Ctor",
    "AlxUsb_Irq_Handle",
    # the helper
    "AlxFakesTest_NewI2c",
    "AlxFakesTest_NewId",
    "AlxFakesTest_NewLp586x",
    "AlxFakesTest_NewParamKvStore",
    "AlxFakesTest_Delete",
    "AlxFakesTest_I2c_IsInit",
    "AlxFakesTest_Id_Ctor",
    "AlxFakesTest_Id_HwIsTheConstructorsOwn",
    "AlxFakesTest_Lp586x_ValNew",
    "AlxFakesTest_ParamKvStore_WasCtorCalled",
    "AlxFakesTest_ParamKvStore_IsInit",
    "AlxFakesTest_ParamKvStore_Fs",
    "AlxFakesTest_Status_Ok",
    "AlxFakesTest_Status_Err",
    "AlxFakesTest_Status_FifoErrEmpty",
    "AlxFakesTest_Status_SafeBothCopyErr",
    "AlxFakesTest_Status_SafeUseCopyA",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))


# ------------------------------------------------------------------ build ----
# The mechanics live in the Python lib (alx.verify.host_build): where the tools are, the MSVC build
# environment, the rebuild-if-stale check, the compile database and the two DLL recipes. What stays
# here is what is this repository's - the source lists, the defines and the export lists above.
#
# Driver GNU = clang with GNU-style flags, which is what the dev build has always used; the sanitizer
# and coverage lanes use clang-cl on the same recipe (Test/noxfile.py). Dialect gnu99 = what the
# target ships: never test a dialect you do not ship. -O0 -g for faithful debugging; clang's
# diagnostics are front-end based, so the warning set is the same at any -O.
TOOLCHAIN = build_tools.Toolchain()
INCLUDES = [CONFIG_DIR, CLIB_DIR, CLIB_DIR / "Mcu", CLIB_DIR / "Ext"]
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


def _build_dll(strict, closure, defines, dll: Path, exports: build_tools.Exports, obj_dir_name: str | None) -> None:
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
        def_file=build_tools.write_def_file(BUILD_DIR, exports),
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
    exports: build_tools.Exports
    obj_dir: str | None


# Which groups the variant matrix covers. Adding a row builds four more DLLs and costs nothing else.
#
#   fifo   the assert axis. 12 assertion sites, and ZERO trace sites - so it says nothing at all
#          about the trace half, which is what lin is here for.
#   lin    the trace axis. 23 sites at three levels (16 WRN, 6 DBG, 1 VRB), which straddles the
#          INF/DBG boundary the four variants actually cross, and it is the only module in the
#          library that does so AND already has a group.
VARIANT_GROUPS: dict[str, VariantGroup] = {
    "fifo": VariantGroup(FIFO_DLL, FIFO_SOURCES, (), FIFO_DEPS, FIFO_EXPORTS, None),
    "lin": VariantGroup(LIN_DLL, LIN_SOURCES, (), LIN_DEPS, LIN_EXPORTS, None),
    "memsafe": VariantGroup(MEMSAFE_DLL, MEMSAFE_SOURCES_STRICT, MEMSAFE_SOURCES_CLOSURE,
                            MEMSAFE_DEPS, MEMSAFE_EXPORTS, "memSafeClosure"),
    # cli   the ELISION question at its sharpest. alxCli.c does 21 of its serial writes
    #       INSIDE ALX_CLI_ASSERT, so `off` compiles a CLI that answers nothing at all.
    #       The group already had sources, a closure and an export list; only this line was missing.
    "cli": VariantGroup(CLI_DLL, CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE,
                        CLI_DEPS, CLI_EXPORTS, "cliClosure"),
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
               _variant_dll(group, variant), g.exports, g.obj_dir)




def _fifo_variant_dll(variant: str) -> Path:
    # Kept as a name of its own because test_fifo_variants.py imports it - P529 is a claim about
    # exactly this function.
    return _variant_dll("fifo", variant)


def _build_fifo_dll() -> None:
    # The default group IS the `default` variant now. It used to be _assert_defines() alone, which
    # is assert-RST with no traces and ALX_TRACE_LEVEL_OFF - a combination no product ships.
    _build_variant_dll("fifo", DEFAULT_VARIANT)


def _build_cli_dll() -> None:
    _build_dll(CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE,
               [*CLI_ASSERT_DEFINES, *_assert_defines(CLI_SOURCES_STRICT, CLI_SOURCES_CLOSURE)],
               CLI_DLL, CLI_EXPORTS, "cliClosure")


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
    _build_dll(VDIV_SOURCES, (), _assert_defines(VDIV_SOURCES), VDIV_DLL, VDIV_EXPORTS, None)


def _build_canparser_dll() -> None:
    _build_dll(CANPARSER_SOURCES, (),
               _assert_defines(CANPARSER_SOURCES),
               CANPARSER_DLL, CANPARSER_EXPORTS, None)


def _build_filtglitch_dll() -> None:
    _build_dll(FILTGLITCH_SOURCES, (),
               _assert_defines(FILTGLITCH_SOURCES),
               FILTGLITCH_DLL, FILTGLITCH_EXPORTS, None)


def _build_math_dll() -> None:
    _build_dll(MATH_SOURCES, (), _assert_defines(MATH_SOURCES), MATH_DLL, MATH_EXPORTS, None)


def _build_fssafe_dll() -> None:
    _build_dll(FSSAFE_SOURCES_STRICT, FSSAFE_SOURCES_CLOSURE,
               [*FSSAFE_DEFINES, *_assert_defines(FSSAFE_SOURCES_STRICT, FSSAFE_SOURCES_CLOSURE)],
               FSSAFE_DLL,
               FSSAFE_EXPORTS, "fsSafeClosure")


def _build_lin_dll() -> None:
    # As with FIFO: the plain group is now the `default` variant, which is the only combination a
    # product actually ships. _assert_defines() alone left ALX_TRACE_LEVEL_OFF, so all 23 of
    # alxLin.c's trace call sites were compiled away in every build this suite had ever made.
    _build_variant_dll("lin", DEFAULT_VARIANT)


def _build_delay_dll() -> None:
    _build_dll(DELAY_SOURCES, (), _assert_defines(DELAY_SOURCES), DELAY_DLL,
               DELAY_EXPORTS, None)


def _build_paramkv_dll() -> None:
    _build_dll(PARAMKV_SOURCES, (), _assert_defines(PARAMKV_SOURCES), PARAMKV_DLL,
               PARAMKV_EXPORTS, None)


def _build_mux_dll() -> None:
    _build_dll(MUX_SOURCES, (), _assert_defines(MUX_SOURCES), MUX_DLL, MUX_EXPORTS, None)


def _build_bts_dll() -> None:
    _build_dll(BTS_SOURCES, (), _assert_defines(BTS_SOURCES), BTS_DLL, BTS_EXPORTS, None)


def _build_ntc_dll() -> None:
    _build_dll(NTC_SOURCES_STRICT, NTC_SOURCES_CLOSURE,
               _assert_defines(NTC_SOURCES), NTC_DLL,
               NTC_EXPORTS, "ntcClosure")


def _build_audioplayer_dll() -> None:
    _build_dll(AUDIOPLAYER_SOURCES_STRICT, AUDIOPLAYER_SOURCES_CLOSURE,
               _assert_defines(AUDIOPLAYER_SOURCES), AUDIOPLAYER_DLL,
               AUDIOPLAYER_EXPORTS, "audioPlayerClosure")


def _build_audio_dll() -> None:
    _build_dll(AUDIO_SOURCES, (), _assert_defines(AUDIO_SOURCES), AUDIO_DLL, AUDIO_EXPORTS, None)


def _build_pwr_dll() -> None:
    _build_dll(PWR_SOURCES, (), _assert_defines(PWR_SOURCES), PWR_DLL, PWR_EXPORTS, None)


def _build_tempsens_dll() -> None:
    _build_dll(TEMPSENS_SOURCES, (),
               _assert_defines(TEMPSENS_SOURCES),
               TEMPSENS_DLL, TEMPSENS_EXPORTS, None)


def _build_rotsw_dll() -> None:
    _build_dll(ROTSW_SOURCES, (), _assert_defines(ROTSW_SOURCES), ROTSW_DLL, ROTSW_EXPORTS, None)


def _build_linfun_dll() -> None:
    _build_dll(LINFUN_SOURCES, (), _assert_defines(LINFUN_SOURCES), LINFUN_DLL, LINFUN_EXPORTS, None)


def _build_bool_dll() -> None:
    _build_dll(BOOL_SOURCES, (), _assert_defines(BOOL_SOURCES), BOOL_DLL, BOOL_EXPORTS, None)


def _build_rtc_dll() -> None:
    _build_dll(RTC_SOURCES, (), _assert_defines(RTC_SOURCES), RTC_DLL, RTC_EXPORTS, None)


def _build_parammgmt_dll() -> None:
    _build_dll(PARAMMGMT_SOURCES_STRICT, PARAMMGMT_SOURCES_CLOSURE,
               [*PARAMMGMT_ASSERT_DEFINES, *_assert_defines(PARAMMGMT_SOURCES_STRICT, PARAMMGMT_SOURCES_CLOSURE)],
               PARAMMGMT_DLL, PARAMMGMT_EXPORTS, "paramMgmtClosure")


def _build_ina228_dll() -> None:
    _build_dll(INA228_SOURCES_STRICT, INA228_SOURCES_CLOSURE,
               [*INA228_DEFINES, *_assert_defines(INA228_SOURCES_STRICT, INA228_SOURCES_CLOSURE)],
               INA228_DLL, INA228_EXPORTS, "ina228Closure")


def _build_pi4ioe_dll() -> None:
    _build_dll(PI4IOE_SOURCES_STRICT, PI4IOE_SOURCES_CLOSURE,
               [*PI4IOE_DEFINES, *_assert_defines(PI4IOE_SOURCES_STRICT, PI4IOE_SOURCES_CLOSURE)],
               PI4IOE_DLL, PI4IOE_EXPORTS, "pi4ioeClosure")


def _build_id_dll() -> None:
    _build_dll(ID_SOURCES_STRICT, ID_SOURCES_CLOSURE,
               _assert_defines(ID_SOURCES_STRICT, ID_SOURCES_CLOSURE),
               ID_DLL, ID_EXPORTS, "idClosure")


def _build_timsw_dll() -> None:
    _build_dll(TIMSW_SOURCES, (), _assert_defines(TIMSW_SOURCES), TIMSW_DLL, TIMSW_EXPORTS, None)


def _build_assert_weak_dll() -> None:
    _build_dll(ASSERT_WEAK_SOURCES, (), _assert_defines(ASSERT_WEAK_SOURCES), ASSERT_WEAK_DLL,
               ASSERT_WEAK_EXPORTS, None)


def _build_assert_dll() -> None:
    _build_dll(ASSERT_SOURCES, (), _assert_defines(ASSERT_SOURCES), ASSERT_DLL,
               ASSERT_EXPORTS, None)


def _build_memraw_dll() -> None:
    _build_dll(MEMRAW_SOURCES, (), _assert_defines(MEMRAW_SOURCES), MEMRAW_DLL,
               MEMRAW_EXPORTS, None)


def _build_memraw_ovr_dll() -> None:
    _build_dll(MEMRAW_OVR_SOURCES, (), _assert_defines(MEMRAW_OVR_SOURCES), MEMRAW_OVR_DLL,
               MEMRAW_OVR_EXPORTS, None)


def _build_fakes_dll() -> None:
    _build_dll(FAKES_SOURCES, (), _assert_defines(FAKES_SOURCES), FAKES_DLL, FAKES_EXPORTS, None)


# ------------------------------------------------- BQ24600 battery charger -----
# Tier-2 target (A92): the REAL driver over faked pins, with the REAL software timers and tick
# under it. The charger says what it is doing on two pins, and one of them has a third meaning:
# a STAT pin that blinks at 1 Hz is a fault, which only a window of time can tell from a level.
BQ24600_SOURCES = [
    CLIB_DIR / "Ext" / "alxBq24600.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxIrqFake.c",
    HELPERS_DIR / "alxBq24600TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
BQ24600_DEPS = [*BQ24600_SOURCES, Path(__file__)]
BQ24600_DLL = BUILD_DIR / "alxBq24600Test.dll"
BQ24600_EXPORTS = build_tools.Exports("alxBq24600Test", (
    "AlxBq24600Test_New",
    "AlxBq24600Test_Delete",
    "AlxBq24600Test_Bq",
    "AlxBq24600Test_EnPin",
    "AlxBq24600Test_NpgPin",
    "AlxBq24600Test_StatPin",
    "AlxBq24600Test_Window_ms",
    "AlxBq24600Test_ErrTransitions",
    "AlxBq24600Test_HalfPeriodMin_ms",
    "AlxBq24600Test_HalfPeriodMax_ms",
    "AlxBq24600_Init",
    "AlxBq24600_DeInit",
    "AlxBq24600_Handle",
    "AlxBq24600_Enable",
    "AlxBq24600_Disable",
    "AlxBq24600_IsBatCharging",
    "AlxBq24600_IsBatFull",
    "AlxBq24600_IsSleep",
    "AlxBq24600_IsErr",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_IncRange_ns",
    "AlxIrqFake_Reset",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_bq24600_dll() -> None:
    _build_dll(BQ24600_SOURCES, (), _assert_defines(BQ24600_SOURCES), BQ24600_DLL, BQ24600_EXPORTS, None)


# ------------------------------------------------ MCP73831 battery charger -----
# Tier-2 target (A92): the REAL driver over one faked pin. The charger's STAT output has three
# states - low, high and floating - and the driver reads it either as a tri-state or as a plain
# level, which is the constructor's choice and decides which statuses can be told apart at all.
MCP73831_SOURCES = [
    CLIB_DIR / "Ext" / "alxMcp73831.c",
    FAKES_DIR / "alxIoPinFake.c",
    HELPERS_DIR / "alxMcp73831TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
MCP73831_DEPS = [*MCP73831_SOURCES, Path(__file__)]
MCP73831_DLL = BUILD_DIR / "alxMcp73831Test.dll"
MCP73831_EXPORTS = build_tools.Exports("alxMcp73831Test", (
    "AlxMcp73831Test_New",
    "AlxMcp73831Test_Delete",
    "AlxMcp73831Test_Mcp",
    "AlxMcp73831Test_StatPin",
    "AlxMcp73831Test_St",
    "AlxMcp73831Test_TriState",
    "AlxMcp73831_Init",
    "AlxMcp73831_DeInit",
    "AlxMcp73831_GetSt",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_SetTriState",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_mcp73831_dll() -> None:
    _build_dll(MCP73831_SOURCES, (), _assert_defines(MCP73831_SOURCES), MCP73831_DLL, MCP73831_EXPORTS, None)


# ---------------------------------------------- TPA3255 audio amplifier -----
# Tier-2 target (A92): the REAL driver over faked pins, with the REAL software timer and tick
# under it. The amplifier shuts itself down on a fault and stays down until it is reset; the
# driver waits, pulses the reset pin, waits again and lets the part run - a state machine in time.
TPA3255_SOURCES = [
    CLIB_DIR / "Ext" / "alxTpa3255.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxIrqFake.c",
    HELPERS_DIR / "alxTpa3255TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
TPA3255_DEPS = [*TPA3255_SOURCES, Path(__file__)]
TPA3255_DLL = BUILD_DIR / "alxTpa3255Test.dll"
TPA3255_EXPORTS = build_tools.Exports("alxTpa3255Test", (
    "AlxTpa3255Test_New",
    "AlxTpa3255Test_Delete",
    "AlxTpa3255Test_Tpa",
    "AlxTpa3255Test_NResetPin",
    "AlxTpa3255Test_NFaultPin",
    "AlxTpa3255Test_NClipOtwPin",
    "AlxTpa3255Test_LowLevel_ms",
    "AlxTpa3255Test_Waiting_ms",
    "AlxTpa3255_Init",
    "AlxTpa3255_DeInit",
    "AlxTpa3255_Handle",
    "AlxTpa3255_Enable",
    "AlxTpa3255_Disable",
    "AlxTpa3255_IsErrAsserted",
    "AlxTpa3255_IsWarningAsserted",
    "AlxTpa3255_WasErrAsserted",
    "AlxTpa3255_WasWarningAsserted",
    "AlxTpa3255_ClearWasErrAsserted",
    "AlxTpa3255_ClearWasWarningAsserted",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_IncRange_ns",
    "AlxIrqFake_Reset",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_tpa3255_dll() -> None:
    _build_dll(TPA3255_SOURCES, (), _assert_defines(TPA3255_SOURCES), TPA3255_DLL, TPA3255_EXPORTS, None)


# ---------------------------------------------- TMP1075 temperature sensor -----
# Tier-2 target (A92): the REAL driver over the faked I2C bus. A 12-bit reading, left-justified
# in a 16-bit two's-complement register, read most significant byte first.
TMP1075_SOURCES = [
    CLIB_DIR / "Ext" / "alxTmp1075.c",
    FAKES_DIR / "alxI2cFake.c",
    HELPERS_DIR / "alxTmp1075TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
TMP1075_DEPS = [*TMP1075_SOURCES, Path(__file__)]
TMP1075_DLL = BUILD_DIR / "alxTmp1075Test.dll"
TMP1075_EXPORTS = build_tools.Exports("alxTmp1075Test", (
    "AlxTmp1075Test_New",
    "AlxTmp1075Test_Delete",
    "AlxTmp1075Test_Tmp",
    "AlxTmp1075Test_Status_Ok",
    "AlxTmp1075Test_Status_Err",
    "AlxTmp1075_Init",
    "AlxTmp1075_DeInit",
    "AlxTmp1075_IsInit",
    "AlxTmp1075_GetTemp_degC",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_tmp1075_dll() -> None:
    _build_dll(TMP1075_SOURCES, (), _assert_defines(TMP1075_SOURCES), TMP1075_DLL, TMP1075_EXPORTS, None)


# ---------------------------------------------------- VEML6040 colour sensor -----
# Tier-2 target (A92): the REAL driver over the faked I2C bus. Four 16-bit channels, red, green,
# blue and white, each its own register, least significant byte first.
VEML6040_SOURCES = [
    CLIB_DIR / "Ext" / "alxVeml6040.c",
    FAKES_DIR / "alxI2cFake.c",
    HELPERS_DIR / "alxVeml6040TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
VEML6040_DEPS = [*VEML6040_SOURCES, Path(__file__)]
VEML6040_DLL = BUILD_DIR / "alxVeml6040Test.dll"
VEML6040_EXPORTS = build_tools.Exports("alxVeml6040Test", (
    "AlxVeml6040Test_New",
    "AlxVeml6040Test_Delete",
    "AlxVeml6040Test_Veml",
    "AlxVeml6040Test_Status_Ok",
    "AlxVeml6040Test_Status_Err",
    "AlxVeml6040_Init",
    "AlxVeml6040_DeInit",
    "AlxVeml6040_GetR_raw",
    "AlxVeml6040_GetG_raw",
    "AlxVeml6040_GetB_raw",
    "AlxVeml6040_GetW_raw",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_veml6040_dll() -> None:
    _build_dll(VEML6040_SOURCES, (), _assert_defines(VEML6040_SOURCES), VEML6040_DLL, VEML6040_EXPORTS, None)


# ------------------------------------------------- PCA9539A IO expander -----
# Tier-2 target (A92): the REAL driver over the faked I2C bus. Sixteen pins on two 8-bit ports,
# a shadow of each port in the driver, and Handle as the one moment the shadow meets the part.
PCA9539A_SOURCES = [
    CLIB_DIR / "Ext" / "alxPca9539a.c",
    FAKES_DIR / "alxI2cFake.c",
    HELPERS_DIR / "alxPca9539aTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
PCA9539A_DEPS = [*PCA9539A_SOURCES, Path(__file__)]
PCA9539A_DLL = BUILD_DIR / "alxPca9539aTest.dll"
PCA9539A_EXPORTS = build_tools.Exports("alxPca9539aTest", (
    "AlxPca9539aTest_New",
    "AlxPca9539aTest_Delete",
    "AlxPca9539aTest_Pca",
    "AlxPca9539aTest_Status_Ok",
    "AlxPca9539aTest_Status_Err",
    "AlxPca9539a_Init",
    "AlxPca9539a_DeInit",
    "AlxPca9539a_Handle",
    "AlxPca9539a_irqHandle",
    "AlxPca9539a_IoPin_Read",
    "AlxPca9539a_Read_Port_Raw",
    "AlxPca9539a_IoPin_Write",
    "AlxPca9539a_IoPin_Set",
    "AlxPca9539a_IoPin_Reset",
    "AlxPca9539a_IoPin_Toggle",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_pca9539a_dll() -> None:
    _build_dll(PCA9539A_SOURCES, (), _assert_defines(PCA9539A_SOURCES), PCA9539A_DLL, PCA9539A_EXPORTS, None)


# ------------------------------------------------ PCAL6416A IO expander -----
# Tier-2 target (A92): the REAL driver over the faked I2C bus. Sixteen pins on two ports, the
# PCA9539A's registers plus drive strength, input latch, pulls and interrupt masks; Handle does
# only the transfers its caller names, and a tri-state read is two reads under two pulls.
PCAL6416A_SOURCES = [
    CLIB_DIR / "Ext" / "alxPcal6416a.c",
    FAKES_DIR / "alxI2cFake.c",
    FAKES_DIR / "alxDelayFake.c",
    HELPERS_DIR / "alxPcal6416aTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
PCAL6416A_DEPS = [*PCAL6416A_SOURCES, Path(__file__)]
PCAL6416A_DLL = BUILD_DIR / "alxPcal6416aTest.dll"
PCAL6416A_EXPORTS = build_tools.Exports("alxPcal6416aTest", (
    "AlxPcal6416aTest_New",
    "AlxPcal6416aTest_Delete",
    "AlxPcal6416aTest_Pcal",
    "AlxPcal6416aTest_TriState",
    "AlxPcal6416aTest_Status_Ok",
    "AlxPcal6416aTest_Status_Err",
    "AlxPcal6416a_InitPeriph",
    "AlxPcal6416a_DeInitPeriph",
    "AlxPcal6416a_Init",
    "AlxPcal6416a_DeInit",
    "AlxPcal6416a_Handle",
    "AlxPcal6416a_IoPin_Read",
    "AlxPcal6416a_IoPin_Write",
    "AlxPcal6416a_IoPin_Set",
    "AlxPcal6416a_IoPin_Reset",
    "AlxPcal6416a_IoPin_Toggle",
    "AlxPcal6416a_IoPin_Read_TriState",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_pcal6416a_dll() -> None:
    _build_dll(PCAL6416A_SOURCES, (), _assert_defines(PCAL6416A_SOURCES), PCAL6416A_DLL, PCAL6416A_EXPORTS, None)


# -------------------------------------------- LP586x LED matrix driver -----
# Tier-2 target (A92): the REAL driver over the faked I2C bus and two faked pins. 108 dots, one
# brightness register each; the driver sends a dot only when it changed, then pulses SYNC so the
# part shows every change at once.
LP586X_SOURCES = [
    CLIB_DIR / "Ext" / "alxLp586x.c",
    FAKES_DIR / "alxI2cFake.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxDelayFake.c",
    HELPERS_DIR / "alxLp586xTestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
LP586X_DEPS = [*LP586X_SOURCES, Path(__file__)]
LP586X_DLL = BUILD_DIR / "alxLp586xTest.dll"
LP586X_EXPORTS = build_tools.Exports("alxLp586xTest", (
    "AlxLp586xTest_New",
    "AlxLp586xTest_Delete",
    "AlxLp586xTest_Lp",
    "AlxLp586xTest_EnPin",
    "AlxLp586xTest_SyncPin",
    "AlxLp586xTest_Status_Ok",
    "AlxLp586xTest_Status_Err",
    "AlxLp586x_InitPeriph",
    "AlxLp586x_DeInitPeriph",
    "AlxLp586x_Init",
    "AlxLp586x_DeInit",
    "AlxLp586x_Handle",
    "AlxLp586x_Led_Write",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxIoPinFake_WriteCount",
    "AlxDelayFake_Reset",
    "AlxDelayFake_UsCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_lp586x_dll() -> None:
    _build_dll(LP586X_SOURCES, (), _assert_defines(LP586X_SOURCES), LP586X_DLL, LP586X_EXPORTS, None)


# ------------------------------------------------- ADXL355 accelerometer -----
# Tier-2 target (A92): the REAL driver over the SPI fake, which records what is sent and answers
# from a queue. Three axes of 20-bit two's complement, most significant byte first, and a 12-bit
# temperature; every register access is one chip-select frame.
#
# The driver is CLOSURE. On this host an enumeration bit-field is signed (A93), so setting the 1-bit
# Standby field to 1 is a -Wsingle-bit-bitfield-constant-conversion error the target compilers do
# not raise. Nothing else in the driver fails the strict set (D48, fixed in goal 7).
ADXL355_SOURCES_STRICT = [
    FAKES_DIR / "alxSpiFake.c",
    HELPERS_DIR / "alxAdxl355TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
ADXL355_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxAdxl355.c",
]
ADXL355_DEPS = [*ADXL355_SOURCES_STRICT, *ADXL355_SOURCES_CLOSURE, Path(__file__)]
ADXL355_DLL = BUILD_DIR / "alxAdxl355Test.dll"
ADXL355_EXPORTS = build_tools.Exports("alxAdxl355Test", (
    "AlxAdxl355Test_New",
    "AlxAdxl355Test_Delete",
    "AlxAdxl355Test_Adxl",
    "AlxAdxl355Test_GetXyz_g",
    "AlxAdxl355Test_Status_Ok",
    "AlxAdxl355Test_Status_Err",
    "AlxAdxl355_Init",
    "AlxAdxl355_DeInit",
    "AlxAdxl355_Enable",
    "AlxAdxl355_Disable",
    "AlxAdxl355_GetTemp_degC",
    "AlxSpiFake_Reset",
    "AlxSpiFake_QueueMiso",
    "AlxSpiFake_MosiLen",
    "AlxSpiFake_Mosi",
    "AlxSpiFake_MisoLeft",
    "AlxSpiFake_CsAssertCount",
    "AlxSpiFake_IsCsAsserted",
    "AlxSpiFake_InitCount",
    "AlxSpiFake_DeInitCount",
    "AlxSpiFake_SetForcedStatus",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_adxl355_dll() -> None:
    _build_dll(ADXL355_SOURCES_STRICT, ADXL355_SOURCES_CLOSURE,
               _assert_defines(ADXL355_SOURCES_STRICT, ADXL355_SOURCES_CLOSURE),
               ADXL355_DLL, ADXL355_EXPORTS, "adxl355Closure")


# --------------------------------------------------------- ADS8678 ADC -----
# Tier-2 target (A92): the REAL driver over the SPI fake and a faked reset pin. Eight channels of
# 14 bits, read in the part's auto-scan order with one NO_OP frame each; every register write is
# checked against the echo the part sends back.
ADS8678_SOURCES = [
    CLIB_DIR / "Ext" / "alxAds8678.c",
    FAKES_DIR / "alxSpiFake.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxDelayFake.c",
    HELPERS_DIR / "alxAds8678TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
ADS8678_DEPS = [*ADS8678_SOURCES, Path(__file__)]
ADS8678_DLL = BUILD_DIR / "alxAds8678Test.dll"
ADS8678_EXPORTS = build_tools.Exports("alxAds8678Test", (
    "AlxAds8678Test_New",
    "AlxAds8678Test_Delete",
    "AlxAds8678Test_Ads",
    "AlxAds8678Test_NRstPin",
    "AlxAds8678Test_SetConfig",
    "AlxAds8678Test_GetChVoltageAll_V",
    "AlxAds8678Test_Status_Ok",
    "AlxAds8678Test_Status_Err",
    "AlxAds8678_InitPeriph",
    "AlxAds8678_DeInitPeriph",
    "AlxAds8678_Init",
    "AlxAds8678_DeInit",
    "AlxSpiFake_Reset",
    "AlxSpiFake_QueueMiso",
    "AlxSpiFake_MosiLen",
    "AlxSpiFake_Mosi",
    "AlxSpiFake_MisoLeft",
    "AlxSpiFake_CsAssertCount",
    "AlxSpiFake_IsCsAsserted",
    "AlxSpiFake_InitCount",
    "AlxSpiFake_DeInitCount",
    "AlxSpiFake_SetForcedStatus",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxDelayFake_Reset",
    "AlxDelayFake_MsCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_ads8678_dll() -> None:
    _build_dll(ADS8678_SOURCES, (), _assert_defines(ADS8678_SOURCES), ADS8678_DLL, ADS8678_EXPORTS, None)


# ------------------------------------------------------- ADS114S08 ADC -----
# Tier-2 target (A92): the REAL driver over the SPI fake and three faked pins, with the REAL
# software timer, tick and linear interpolation under it. A conversion is START high, a wait for
# data ready, one RDATA frame, START low. Init does not run on this host until A93 (the helper).
#
# The driver is CLOSURE. On this host an enumeration bit-field is signed (A93), so
# every enumerator above the field's sign bit - MUXP_AIN8 in a 4-bit field, 1 in a 1-bit one - is
# a -Wbitfield-constant-conversion error that the target compilers, where the field is unsigned, do
# not raise. Nothing else in the driver fails the strict set (D48, fixed in goal 7).
ADS114S08_SOURCES_STRICT = [
    CLIB_DIR / "alxInterpLin.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    FAKES_DIR / "alxSpiFake.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxDelayFake.c",
    HELPERS_DIR / "alxAds114s08TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
ADS114S08_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxAds114s08.c",
]
ADS114S08_DEPS = [*ADS114S08_SOURCES_STRICT, *ADS114S08_SOURCES_CLOSURE, Path(__file__)]
ADS114S08_DLL = BUILD_DIR / "alxAds114s08Test.dll"
ADS114S08_EXPORTS = build_tools.Exports("alxAds114s08Test", (
    "AlxAds114s08Test_New",
    "AlxAds114s08Test_Delete",
    "AlxAds114s08Test_Ads",
    "AlxAds114s08Test_NDrdyPin",
    "AlxAds114s08Test_StartPin",
    "AlxAds114s08Test_NRstPin",
    "AlxAds114s08Test_Perform",
    "AlxAds114s08Test_MarkInit",
    "AlxAds114s08Test_Status_Ok",
    "AlxAds114s08Test_Status_Err",
    "AlxAds114s08_InitPeriph",
    "AlxAds114s08_DeInitPeriph",
    "AlxAds114s08_Init",
    "AlxAds114s08_DeInit",
    "AlxSpiFake_Reset",
    "AlxSpiFake_QueueMiso",
    "AlxSpiFake_MosiLen",
    "AlxSpiFake_Mosi",
    "AlxSpiFake_MisoLeft",
    "AlxSpiFake_CsAssertCount",
    "AlxSpiFake_IsCsAsserted",
    "AlxSpiFake_InitCount",
    "AlxSpiFake_DeInitCount",
    "AlxSpiFake_SetForcedStatus",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_SetLevel",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxIoPinFake_WriteCount",
    "AlxDelayFake_Reset",
    "AlxDelayFake_MsCount",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxIrqFake_Reset",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_ads114s08_dll() -> None:
    _build_dll(ADS114S08_SOURCES_STRICT, ADS114S08_SOURCES_CLOSURE,
               _assert_defines(ADS114S08_SOURCES_STRICT, ADS114S08_SOURCES_CLOSURE),
               ADS114S08_DLL, ADS114S08_EXPORTS, "ads114s08Closure")


# --------------------------------------------------- ADAU1961 audio codec -----
# Tier-2 target (A92): the REAL driver over the faked I2C bus and the faked I2S interface. The
# codec's gains are register fields computed from decibels, three scales with three steps; the
# differential input adds a +20 dB boost stage above 35.25 dB.
#
# The driver and its helper are CLOSURE. On this host an enumeration bit-field is signed (A93), so
# assigning an enumerator above the field's sign bit is a -Wbitfield-constant-conversion error the
# target compilers do not raise. Nothing else in the driver fails the strict set (D48, fixed in
# goal 7).
ADAU1961_SOURCES_STRICT = [
    FAKES_DIR / "alxI2cFake.c",
    FAKES_DIR / "alxI2sFake.c",
    FAKES_DIR / "alxAssertFake.c",
]
ADAU1961_SOURCES_CLOSURE = [
    CLIB_DIR / "Ext" / "alxAdau1961.c",
    HELPERS_DIR / "alxAdau1961TestHelpers.c",
]
ADAU1961_DEPS = [*ADAU1961_SOURCES_STRICT, *ADAU1961_SOURCES_CLOSURE, Path(__file__)]
ADAU1961_DLL = BUILD_DIR / "alxAdau1961Test.dll"
ADAU1961_EXPORTS = build_tools.Exports("alxAdau1961Test", (
    "AlxAdau1961Test_New",
    "AlxAdau1961Test_Delete",
    "AlxAdau1961Test_Adau",
    "AlxAdau1961Test_InDiff",
    "AlxAdau1961Test_InAux",
    "AlxAdau1961Test_OutLine",
    "AlxAdau1961Test_Status_Ok",
    "AlxAdau1961Test_Status_Err",
    "AlxAdau1961_Init",
    "AlxAdau1961_DeInit",
    "AlxAdau1961_Foreground_Handle",
    "AlxAdau1961_InDiffL_SetGain_dB",
    "AlxAdau1961_InDiffR_SetGain_dB",
    "AlxAdau1961_InAuxL_SetGain_dB",
    "AlxAdau1961_InAuxR_SetGain_dB",
    "AlxAdau1961_OutLineL_SetGain_dB",
    "AlxAdau1961_OutLineR_SetGain_dB",
    "AlxI2cFake_Reset",
    "AlxI2cFake_SetReg",
    "AlxI2cFake_GetLastWrite",
    "AlxI2cFake_WriteCount",
    "AlxI2cFake_ReadCount",
    "AlxI2cFake_SetSlaveReady",
    "AlxI2cFake_SetForcedStatus",
    "AlxI2sFake_Reset",
    "AlxI2sFake_InitCount",
    "AlxI2sFake_DeInitCount",
    "AlxI2sFake_HandleCount",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_adau1961_dll() -> None:
    _build_dll(ADAU1961_SOURCES_STRICT, ADAU1961_SOURCES_CLOSURE,
               _assert_defines(ADAU1961_SOURCES_STRICT, ADAU1961_SOURCES_CLOSURE),
               ADAU1961_DLL, ADAU1961_EXPORTS, "adau1961Closure")


# ------------------------------------------------------ VNH7040 H-bridge -----
# Tier-2 target (A92): the REAL driver over the faked ADC, PWM and pins, with the REAL software
# timer, tick and voltage divider under it. A duty is a direction and a speed; one multisense pin
# is read once a millisecond for, in turn, the two high-side currents, the chip temperature and
# the supply voltage. Built with the PWM speed input, as the template ships it.
VNH7040_SOURCES = [
    CLIB_DIR / "Ext" / "alxVnh7040.c",
    CLIB_DIR / "alxVdiv.c",
    CLIB_DIR / "alxTimSw.c",
    CLIB_DIR / "alxTick.c",
    FAKES_DIR / "alxAdcFake.c",
    FAKES_DIR / "alxPwmFake.c",
    FAKES_DIR / "alxIoPinFake.c",
    FAKES_DIR / "alxIrqFake.c",
    FAKES_DIR / "alxDelayFake.c",
    HELPERS_DIR / "alxVnh7040TestHelpers.c",
    FAKES_DIR / "alxAssertFake.c",
]
VNH7040_DEPS = [*VNH7040_SOURCES, Path(__file__)]
VNH7040_DLL = BUILD_DIR / "alxVnh7040Test.dll"
VNH7040_EXPORTS = build_tools.Exports("alxVnh7040Test", (
    "AlxVnh7040Test_New",
    "AlxVnh7040Test_Delete",
    "AlxVnh7040Test_Vnh",
    "AlxVnh7040Test_Adc",
    "AlxVnh7040Test_Pwm",
    "AlxVnh7040Test_Pin",
    "AlxVnh7040Test_SenseCh",
    "AlxVnh7040Test_PwmCh",
    "AlxVnh7040_Init",
    "AlxVnh7040_DeInit",
    "AlxVnh7040_Handle",
    "AlxVnh7040_SetDuty_permil",
    "AlxVnh7040_GetCurrent_mA",
    "AlxVnh7040_GetCurrentHsA_mA",
    "AlxVnh7040_GetCurrentHsB_mA",
    "AlxVnh7040_GetSupplyVoltage_mV",
    "AlxVnh7040_GetTemp_degC",
    "AlxVnh7040_GetIsError",
    "AlxVnh7040_Config_BreakType",
    "AlxAdcFake_Reset",
    "AlxAdcFake_SetVoltage_V",
    "AlxAdcFake_SetVoltageSeq_V",
    "AlxAdcFake_ReadCount",
    "AlxPwmFake_Reset",
    "AlxPwmFake_Duty_pct",
    "AlxPwmFake_WriteCount",
    "AlxIoPinFake_Reset",
    "AlxIoPinFake_DidOverflow",
    "AlxIoPinFake_NumOfSlots",
    "AlxIoPinFake_Level",
    "AlxIoPinFake_InitCount",
    "AlxIoPinFake_DeInitCount",
    "AlxDelayFake_Reset",
    "AlxDelayFake_MsCount",
    "alxTick DATA",
    "AlxTick_Ctor",
    "AlxTick_IncRange_ns",
    "AlxIrqFake_Reset",
    "AlxAssertFake_Reset",
    "AlxAssertFake_Count",
    "AlxAssertFake_First",
))

def _build_vnh7040_dll() -> None:
    _build_dll(VNH7040_SOURCES, (), _assert_defines(VNH7040_SOURCES), VNH7040_DLL, VNH7040_EXPORTS, None)


# The groups as DATA, for anything that must rebuild them without running the suite: the MUTATE
# lane names this list on the command line (alx.verify.mutation_hooks rebuild --groups
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
    (FAKES_DLL, FAKES_DEPS, _build_fakes_dll),
    (BQ24600_DLL, BQ24600_DEPS, _build_bq24600_dll),
    (MCP73831_DLL, MCP73831_DEPS, _build_mcp73831_dll),
    (TPA3255_DLL, TPA3255_DEPS, _build_tpa3255_dll),
    (TMP1075_DLL, TMP1075_DEPS, _build_tmp1075_dll),
    (VEML6040_DLL, VEML6040_DEPS, _build_veml6040_dll),
    (PCA9539A_DLL, PCA9539A_DEPS, _build_pca9539a_dll),
    (PCAL6416A_DLL, PCAL6416A_DEPS, _build_pcal6416a_dll),
    (LP586X_DLL, LP586X_DEPS, _build_lp586x_dll),
    (ADXL355_DLL, ADXL355_DEPS, _build_adxl355_dll),
    (ADS8678_DLL, ADS8678_DEPS, _build_ads8678_dll),
    (ADS114S08_DLL, ADS114S08_DEPS, _build_ads114s08_dll),
    (ADAU1961_DLL, ADAU1961_DEPS, _build_adau1961_dll),
    (VNH7040_DLL, VNH7040_DEPS, _build_vnh7040_dll),
]


# ------------------------------------- the library's own assertions, per group -----
# A module built with no ALX_<MODULE>_ASSERT_*_ENABLE macro compiles its assertions down to
# ALX_ASSERT_OFF, which never evaluates the expression - the invariant is not weakened, it is absent.
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

    Test helpers and fakes are skipped - they have no assertions of their own, and alxAssertFake.c is
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
# ALX_TRACE_LEVEL_OFF, which compiles all 549 of the library's trace call sites away and never
# evaluates their arguments.
#
# These four are the standard set. Each earns its place by reaching something none of the others do:
#
#   default       what customers get - alxConfig_TEMPLATE.h's own combination. Must always be green.
#   off           the elision configuration. No assert macro and no traces, so both collapse to
#                 ALX_ASSERT_OFF and ALX_TRACE_OFF and never evaluate their expressions - the shape
#                 of the biggest defect ALX-1553 found (NotesClaude 3g), pinned here so it cannot
#                 come back unnoticed.
#   debug         the only variant that RUNS the DBG and VRB call sites, so what one of them
#                 prints is visible at all.
#   assert_trace  the only form where a failed assertion CONTINUES into the code after the check.
#                 RST resets and BKPT halts; this one carries on with the precondition violated.
#
# BKPT is safe in a DLL because alxAssertFake.c defines all three handlers strongly - it records where
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
    their own, and alxAssertFake.c is where the enabled ones land.
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
