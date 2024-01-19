/**
  ******************************************************************************
  * @file		alxId.h
  * @brief		Auralix C Library - ALX ID Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Include Guard
//******************************************************************************
#ifndef ALX_ID_H
#define ALX_ID_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxBuild.h"
#include "alxBuild_GENERATED.h"
#include "alxIoPin.h"
#include "alxDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ID_FILE "alxId.h"

// Assert //
#if defined(_ALX_ID_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_ID_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ID_FILE, expr)
#elif defined(_ALX_ID_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_ID_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ID_FILE, expr)
#elif defined(_ALX_ID_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_ID_ASSERT(expr) ALX_ASSERT_RST(ALX_ID_FILE, expr)
#else
	#define ALX_ID_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_ID_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_ID_TRACE(...) ALX_TRACE_STD(ALX_ID_FILE, __VA_ARGS__)
	#define ALX_ID_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_ID_TRACE(...) do{} while (false)
	#define ALX_ID_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Defines
//******************************************************************************
#define ALX_ID_NAME_LEN 40
#define ALX_ID_FW_BUILD_HASH_LEN 50
#define ALX_ID_FW_BUILD_HASH_SHORT_LEN 8
#define ALX_ID_FW_VER_STR_LEN 100
#define ALX_ID_FW_BIN_STR_LEN 100
#define ALX_ID_HW_ID_IO_PIN_ARR_LEN 5
#define ALX_ID_HW_STM32_MCU_UNIQUE_ID_LEN_uint32 3
#define ALX_ID_HW_STM32_MCU_UNIQUE_ID_LEN_uint8 12
#define ALX_ID_HW_STM32_MCU_UNIQUE_ID_STR_LEN 30


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Const
	char name[ALX_ID_NAME_LEN];
	uint32_t date;
	uint32_t dateComp;
	uint32_t num;
	char hash[ALX_ID_FW_BUILD_HASH_LEN];
	char hashShort[ALX_ID_FW_BUILD_HASH_SHORT_LEN];
	uint32_t hashShortUint32;
	uint32_t rev;
} AlxId_Fw_Build;

typedef struct
{
	// Const
	AlxId_Fw_Build build;

	// Parameters
	char artf[ALX_ID_NAME_LEN];
	char name[ALX_ID_NAME_LEN];
	uint8_t verMajor;
	uint8_t verMinor;
	uint8_t verPatch;
	bool isBuildJobUsed;
	bool isBootloader;

	// Variables
	uint32_t verDate;
	uint64_t ver;
	char verStr[ALX_ID_FW_VER_STR_LEN];
	char binStr[ALX_ID_FW_BIN_STR_LEN];
} AlxId_Fw;

typedef struct
{
	// Const
	uint32_t ver;
	uint32_t verMinRequired;
} AlxId_Fw_LangC;

typedef struct
{
	// Const
	char name[ALX_ID_NAME_LEN];
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t verPatch;
	uint32_t ver;
	uint32_t verMinRequired;
} AlxId_Fw_LangCLib;

typedef struct
{
	// Const
	char name[ALX_ID_NAME_LEN];
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t verPatch;
	uint32_t ver;
	uint32_t verMinRequired;
} AlxId_Fw_Comp;

#ifdef ALX_CMSIS_CORE
typedef struct
{
	// Const
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t ver;
	uint32_t verRequired;
} AlxId_Fw_CmsisCore;
#endif

#ifdef ALX_STM32
typedef struct
{
	// Const
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t verPatch;
	uint32_t verRc;
	uint32_t ver;
	uint32_t verRequired;
} AlxId_Fw_Stm32;
#endif

typedef struct
{
	uint8_t id;

	char pcbArtf[ALX_ID_NAME_LEN];
	char pcbName[ALX_ID_NAME_LEN];
	uint8_t pcbVerMajor;
	uint8_t pcbVerMinor;
	uint8_t pcbVerPatch;
	uint32_t pcbVerDate;
	uint64_t pcbVer;						// Variable
	char pcbVerStr[ALX_ID_FW_VER_STR_LEN];	// Variable

	char bomArtf[ALX_ID_NAME_LEN];
	char bomName[ALX_ID_NAME_LEN];
	uint8_t bomVerMajor;
	uint8_t bomVerMinor;
	uint8_t bomVerPatch;
	uint32_t bomVerDate;
	uint64_t bomVer;						// Variable
	char bomVerStr[ALX_ID_FW_VER_STR_LEN];	// Variable
} AlxId_HwInstance;

typedef struct
{
	// Parameters
	AlxId_HwInstance* instanceKnownArr;
	uint8_t instanceKnownArrLen;
	uint8_t* instanceHwIdSupportedArr;
	uint8_t instanceHwIdSupportedArrLen;
	AlxIoPin** idIoPinArr;
	uint8_t idIoPinArrLen;
	char mcuName[ALX_ID_NAME_LEN];

	// Variables
	AlxId_HwInstance instance;
	AlxIoPin_TriState idIoPinState[ALX_ID_HW_ID_IO_PIN_ARR_LEN];
} AlxId_Hw;

#ifdef ALX_STM32
typedef union
{
	uint32_t uint32[ALX_ID_HW_STM32_MCU_UNIQUE_ID_LEN_uint32];
	uint8_t uint8[ALX_ID_HW_STM32_MCU_UNIQUE_ID_LEN_uint8];
} AlxId_Hw_Stm32_McuUniqueId;

typedef struct
{
	// Const
	uint32_t deviceId;
	uint32_t revisionId;
	uint32_t flashSize_kB;
	#ifdef ALX_STM32G4
	uint32_t packageId;
	#endif
	AlxId_Hw_Stm32_McuUniqueId mcuUniqueId;
	char mcuUniqueIdStr[ALX_ID_HW_STM32_MCU_UNIQUE_ID_STR_LEN];
} AlxId_Hw_Stm32;
#endif

typedef struct
{
	//------------------------------------------------------------------------------
	// FW
	//------------------------------------------------------------------------------

	// Parameters
	AlxId_Fw fwApp;
	AlxId_Fw fwBoot;
	AlxId_Fw_LangC fwLangC;
	AlxId_Fw_LangCLib fwLangCLib;
	AlxId_Fw_Comp fwComp;
	#ifdef ALX_CMSIS_CORE
	AlxId_Fw_CmsisCore fwCmsisCore;
	#endif
	#ifdef ALX_STM32
	AlxId_Fw_Stm32 fwStm32HalCmsis;
	AlxId_Fw_Stm32 fwStm32HalDriver;
	#endif


	//------------------------------------------------------------------------------
	// HW
	//------------------------------------------------------------------------------

	// Parameters
	AlxId_Hw hw;
	#ifdef ALX_STM32
	AlxId_Hw_Stm32 hwStm32;
	#endif


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	bool wasCtorCalled;
	bool isInit;
	bool isHwIdUsed;
} AlxId;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxId_Ctor
(
	AlxId* me,
	const char* fwArtf,
	const char* fwName,
	uint8_t fwVerMajor,
	uint8_t fwVerMinor,
	uint8_t fwVerPatch,
	bool fwIsBuildJobUsed,
	bool fwIsBootloader,
	AlxId_HwInstance* hwInstanceKnownArr,
	uint8_t hwInstanceKnownArrLen,
	uint8_t* hwInstanceHwIdSupportedArr,
	uint8_t hwInstanceHwIdSupportedArrLen,
	AlxIoPin** hwIdIoPinArr,
	uint8_t hwIdIoPinArrLen,
	const char* hwMcuName
);
void AlxId_Ctor_NoHwId
(
	AlxId* me,
	const char* fwArtf,
	const char* fwName,
	uint8_t fwVerMajor,
	uint8_t fwVerMinor,
	uint8_t fwVerPatch,
	bool fwIsBuildJobUsed,
	bool fwIsBootloader,
	AlxId_HwInstance hwInstance,
	const char* hwMcuName
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxId_Init(AlxId* me);
void AlxId_Trace(AlxId* me);
const char* AlxId_GetFwAppArtf(AlxId* me);
const char* AlxId_GetFwAppName(AlxId* me);
uint8_t AlxId_GetFwAppVerMajor(AlxId* me);
uint8_t AlxId_GetFwAppVerMinor(AlxId* me);
uint8_t AlxId_GetFwAppVerPatch(AlxId* me);
uint32_t AlxId_GetFwAppVerDate(AlxId* me);
uint64_t AlxId_GetFwAppVer(AlxId* me);
uint32_t AlxId_GetFwAppHashShort(AlxId* me);
const char* AlxId_GetFwAppVerStr(AlxId* me);
const char* AlxId_GetFwAppBinStr(AlxId* me);
const char* AlxId_GetHwPcbArtf(AlxId* me);
const char* AlxId_GetHwPcbName(AlxId* me);
uint8_t AlxId_GetHwPcbVerMajor(AlxId* me);
uint8_t AlxId_GetHwPcbVerMinor(AlxId* me);
uint8_t AlxId_GetHwPcbVerPatch(AlxId* me);
uint32_t AlxId_GetHwPcbVerDate(AlxId* me);
uint64_t AlxId_GetHwPcbVer(AlxId* me);
const char* AlxId_GetHwPcbVerStr(AlxId* me);
const char* AlxId_GetHwBomArtf(AlxId* me);
const char* AlxId_GetHwBomName(AlxId* me);
uint8_t AlxId_GetHwBomVerMajor(AlxId* me);
uint8_t AlxId_GetHwBomVerMinor(AlxId* me);
uint8_t AlxId_GetHwBomVerPatch(AlxId* me);
uint32_t AlxId_GetHwBomVerDate(AlxId* me);
uint64_t AlxId_GetHwBomVer(AlxId* me);
const char* AlxId_GetHwBomVerStr(AlxId* me);
uint8_t AlxId_GetHwId(AlxId* me);
void AlxId_GetHwMcuUniqueIdUint32(AlxId* me, uint32_t* uniqueIdUint32, uint8_t len);
void AlxId_GetHwMcuUniqueIdUint8(AlxId* me, uint8_t* uniqueIdUint8, uint8_t len);
const char* AlxId_GetHwMcuUniqueIdStr(AlxId* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ID_H
