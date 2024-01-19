/**
  ******************************************************************************
  * @file		alxId.c
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
// Includes
//******************************************************************************
#include "alxId.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		fwArtf
  * @param[in]		fwName
  * @param[in]		fwVerMajor
  * @param[in]		fwVerMinor
  * @param[in]		fwVerPatch
  * @param[in]		fwIsBuildJobUsed
  * @param[in]		fwIsBootloader
  * @param[in]		hwInstanceKnownArr
  * @param[in]		hwInstanceKnownArrLen
  * @param[in]		hwInstanceHwIdSupportedArr
  * @param[in]		hwInstanceHwIdSupportedArrLen
  * @param[in]		hwIdIoPinArr
  * @param[in]		hwIdIoPinArrLen
  * @param[in]		hwMcuName
  */
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
)
{
	//------------------------------------------------------------------------------
	// FW
	//------------------------------------------------------------------------------
	me->fwBoot.isBootloader = fwIsBootloader;
	me->fwApp.isBootloader = fwIsBootloader;
	if (fwIsBootloader == true)
	{
		// Const
		strcpy(me->fwBoot.build.name, ALX_BUILD_NAME);
		me->fwBoot.build.date = ALX_BUILD_DATE;
		me->fwBoot.build.dateComp = ALX_BUILD_DATE_COMP;
		me->fwBoot.build.num = ALX_BUILD_NUM;
		strcpy(me->fwBoot.build.hash, ALX_BUILD_HASH);
		strcpy(me->fwBoot.build.hashShort, ALX_BUILD_HASH_SHORT);
		ALX_ID_ASSERT(sscanf(ALX_BUILD_HASH_SHORT, "%lx", &me->fwBoot.build.hashShortUint32) == 1);
		me->fwBoot.build.rev = ALX_BUILD_REV;

		// Parameters
		strcpy(me->fwBoot.artf, fwArtf);
		strcpy(me->fwBoot.name, fwName);
		me->fwBoot.verMajor = fwVerMajor;
		me->fwBoot.verMinor = fwVerMinor;
		me->fwBoot.verPatch = fwVerPatch;
		me->fwBoot.isBuildJobUsed = fwIsBuildJobUsed;

		// Variables
		if (me->fwBoot.isBuildJobUsed)
		{
			me->fwBoot.verDate = me->fwBoot.build.date;
		}
		else
		{
			me->fwBoot.verDate = me->fwBoot.build.dateComp;
		}
		me->fwBoot.ver =
		(
			((uint64_t)me->fwBoot.verMajor << 56) |
			((uint64_t)me->fwBoot.verMinor << 48) |
			((uint64_t)me->fwBoot.verPatch << 32) |
			((uint64_t)me->fwBoot.verDate)
		);
	}
	else if (fwIsBootloader == false)
	{
		// Const
		strcpy(me->fwApp.build.name, ALX_BUILD_NAME);
		me->fwApp.build.date = ALX_BUILD_DATE;
		me->fwApp.build.dateComp = ALX_BUILD_DATE_COMP;
		me->fwApp.build.num = ALX_BUILD_NUM;
		strcpy(me->fwApp.build.hash, ALX_BUILD_HASH);
		strcpy(me->fwApp.build.hashShort, ALX_BUILD_HASH_SHORT);
		ALX_ID_ASSERT(sscanf(ALX_BUILD_HASH_SHORT, "%lx", &me->fwApp.build.hashShortUint32) == 1);
		me->fwApp.build.rev = ALX_BUILD_REV;

		// Parameters
		strcpy(me->fwApp.artf, fwArtf);
		strcpy(me->fwApp.name, fwName);
		me->fwApp.verMajor = fwVerMajor;
		me->fwApp.verMinor = fwVerMinor;
		me->fwApp.verPatch = fwVerPatch;
		me->fwApp.isBuildJobUsed = fwIsBuildJobUsed;

		// Variables
		if (me->fwApp.isBuildJobUsed)
		{
			me->fwApp.verDate = me->fwApp.build.date;
		}
		else
		{
			me->fwApp.verDate = me->fwApp.build.dateComp;
		}
		me->fwApp.ver =
		(
			((uint64_t)me->fwApp.verMajor << 56) |
			((uint64_t)me->fwApp.verMinor << 48) |
			((uint64_t)me->fwApp.verPatch << 32) |
			((uint64_t)me->fwApp.verDate)
		);
		sprintf(me->fwApp.verStr, "%u.%u.%u.%lu.%s", me->fwApp.verMajor, me->fwApp.verMinor, me->fwApp.verPatch, me->fwApp.verDate, me->fwApp.build.hash);
		sprintf(me->fwApp.binStr, "%lu_%s_%s_V%u-%u-%u_%s.bin", me->fwApp.verDate, me->fwApp.artf, me->fwApp.name, me->fwApp.verMajor, me->fwApp.verMinor, me->fwApp.verPatch, me->fwApp.build.hashShort);
	}
	else
	{
		ALX_ID_ASSERT(false);	// We should never get here
		return;
	}


	//------------------------------------------------------------------------------
	// FW - Language C
	//------------------------------------------------------------------------------
	#ifdef ALX_GCC
		// Preprocessor
		#if (__STDC_VERSION__ < ALX_LANG_C_VER_MIN_REQUIRED)
			#error
		#endif

		// Const
		me->fwLangC.ver = __STDC_VERSION__;
		me->fwLangC.verMinRequired = ALX_LANG_C_VER_MIN_REQUIRED;
	#endif


	//------------------------------------------------------------------------------
	// FW - Language C Standard Library
	//------------------------------------------------------------------------------
	#ifdef ALX_NEWLIB_NANO
		// Preprocessor
		#if (ALX_xVER3(__NEWLIB__, __NEWLIB_MINOR__, __NEWLIB_PATCHLEVEL__) < ALX_LANG_C_LIB_VER_MIN_REQUIRED)
			#error
		#endif

		// Const
		strcpy(me->fwLangCLib.name, "Newlib-Nano C Standard Library");
		me->fwLangCLib.verMajor = __NEWLIB__;
		me->fwLangCLib.verMinor = __NEWLIB_MINOR__;
		me->fwLangCLib.verPatch = __NEWLIB_PATCHLEVEL__;
		me->fwLangCLib.ver = ALX_xVER3(__NEWLIB__, __NEWLIB_MINOR__, __NEWLIB_PATCHLEVEL__);
		me->fwLangCLib.verMinRequired = ALX_LANG_C_LIB_VER_MIN_REQUIRED;
	#endif


	//------------------------------------------------------------------------------
	// FW - Compiler
	//------------------------------------------------------------------------------
	#ifdef ALX_GCC
		// Preprocessor
		// GNU -> [G]NU is [N]ot [U]nix
		// GCC -> [G]NU [C]ompiler [C]ollection
		#if (ALX_xVER3(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__) < ALX_COMP_VER_MIN_REQUIRED)
			#error
		#endif

		// Const
		strcpy(me->fwComp.name, "GCC");
		me->fwComp.verMajor = __GNUC__;
		me->fwComp.verMinor = __GNUC_MINOR__;
		me->fwComp.verPatch = __GNUC_PATCHLEVEL__;
		me->fwComp.ver = ALX_xVER3(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
		me->fwComp.verMinRequired = ALX_COMP_VER_MIN_REQUIRED;
	#endif


	//------------------------------------------------------------------------------
	// FW - CMSIS Core
	//------------------------------------------------------------------------------
	#ifdef ALX_CMSIS_CORE
		// Preprocessor
		#if (__CM_CMSIS_VERSION != ALX_CMSIS_CORE_VER_REQUIRED)
			#error
		#endif

		// Const
		me->fwCmsisCore.verMajor = __CM_CMSIS_VERSION_MAIN;
		me->fwCmsisCore.verMinor = __CM_CMSIS_VERSION_SUB;
		me->fwCmsisCore.ver = __CM_CMSIS_VERSION;
		me->fwCmsisCore.verRequired = ALX_CMSIS_CORE_VER_REQUIRED;
	#endif


	//------------------------------------------------------------------------------
	// FW - STM32 - CMSIS
	//------------------------------------------------------------------------------
	#ifdef ALX_STM32
		// Preprocessor
		#if (ALX_STM32_CMSIS_VERSION != ALX_STM32_CMSIS_VER_REQUIRED)
			#error
		#endif

		// Const
		me->fwStm32HalCmsis.verMajor = ALX_STM32_CMSIS_VERSION_MAIN;
		me->fwStm32HalCmsis.verMinor = ALX_STM32_CMSIS_VERSION_SUB1;
		me->fwStm32HalCmsis.verPatch = ALX_STM32_CMSIS_VERSION_SUB2;
		me->fwStm32HalCmsis.verRc = ALX_STM32_CMSIS_VERSION_RC;
		me->fwStm32HalCmsis.ver = ALX_STM32_CMSIS_VERSION;
		me->fwStm32HalCmsis.verRequired = ALX_STM32_CMSIS_VER_REQUIRED;
	#endif


	//------------------------------------------------------------------------------
	// FW - STM32 - HAL
	//------------------------------------------------------------------------------
	#ifdef ALX_STM32
		// Const
		me->fwStm32HalDriver.ver			= HAL_GetHalVersion();
		me->fwStm32HalDriver.verMajor		= (me->fwStm32HalDriver.ver & 0xFF000000) >> 24;
		me->fwStm32HalDriver.verMinor		= (me->fwStm32HalDriver.ver & 0x00FF0000) >> 16;
		me->fwStm32HalDriver.verPatch		= (me->fwStm32HalDriver.ver & 0x0000FF00) >> 8;
		me->fwStm32HalDriver.verRc			= (me->fwStm32HalDriver.ver & 0x000000FF) >> 0;
		me->fwStm32HalDriver.verRequired	= ALX_STM32_HAL_VER_REQUIRED;

		// Assert
		ALX_ID_ASSERT(me->fwStm32HalDriver.ver == me->fwStm32HalDriver.verRequired);
	#endif


	//------------------------------------------------------------------------------
	// HW
	//------------------------------------------------------------------------------

	// Parameters
	me->hw.instanceKnownArr = hwInstanceKnownArr;
	me->hw.instanceKnownArrLen = hwInstanceKnownArrLen;
	me->hw.instanceHwIdSupportedArr = hwInstanceHwIdSupportedArr;
	me->hw.instanceHwIdSupportedArrLen = hwInstanceHwIdSupportedArrLen;
	me->hw.idIoPinArr = hwIdIoPinArr;
	me->hw.idIoPinArrLen = hwIdIoPinArrLen;
	strcpy(me->hw.mcuName, hwMcuName);
	me->hw.instance.id = 255;	// Error
	for (uint32_t i = 0; i < ALX_ID_HW_ID_IO_PIN_ARR_LEN; i++) { me->hw.idIoPinState[i] = AlxIoPin_TriState_Undefined; }


	//------------------------------------------------------------------------------
	// HW - STM32
	//------------------------------------------------------------------------------
	#ifdef ALX_STM32
		// Const
		me->hwStm32.deviceId = HAL_GetDEVID();
		me->hwStm32.revisionId = HAL_GetREVID();
		me->hwStm32.flashSize_kB = *(uint16_t*)(FLASHSIZE_BASE);
		#ifdef ALX_STM32G4
			me->stm32Hw.packageId = (*(uint16_t*)(0x1FFF7500) & 0x001F);
		#endif
		me->hwStm32.mcuUniqueId.uint32[0] = *(uint32_t*)(UID_BASE);
		me->hwStm32.mcuUniqueId.uint32[1] = *(uint32_t*)(UID_BASE + 4);
		me->hwStm32.mcuUniqueId.uint32[2] = *(uint32_t*)(UID_BASE + 8);
		sprintf(me->hwStm32.mcuUniqueIdStr, "%08lX%08lX%08lX", me->hwStm32.mcuUniqueId.uint32[2], me->hwStm32.mcuUniqueId.uint32[1], me->hwStm32.mcuUniqueId.uint32[0]);
	#endif


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->wasCtorCalled = true;
	me->isInit = false;
	me->isHwIdUsed = true;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		fwArtf
  * @param[in]		fwName
  * @param[in]		fwVerMajor
  * @param[in]		fwVerMinor
  * @param[in]		fwVerPatch
  * @param[in]		fwIsBuildJobUsed
  * @param[in]		fwIsBootloader
  * @param[in]		hwInstance
  * @param[in]		hwMcuName
  */
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
)
{
	//------------------------------------------------------------------------------
	// Ctor
	//------------------------------------------------------------------------------
	AlxId_Ctor
	(
		me,
		fwArtf,
		fwName,
		fwVerMajor,
		fwVerMinor,
		fwVerPatch,
		fwIsBuildJobUsed,
		fwIsBootloader,
		NULL,
		ALX_NULL,
		NULL,
		ALX_NULL,
		NULL,
		ALX_NULL,
		hwMcuName
	);


	//------------------------------------------------------------------------------
	// HW
	//------------------------------------------------------------------------------
	me->hw.instance = hwInstance;


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->isHwIdUsed = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxId_Init(AlxId* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == false);


	//------------------------------------------------------------------------------
	// If HW ID is used
	//------------------------------------------------------------------------------
	me->hw.instance.id = 0;
	if (me->isHwIdUsed)
	{
		// Get HW ID pin states
		for (uint32_t i = 0; i < me->hw.idIoPinArrLen; i++)
		{
			AlxIoPin_Init(*(me->hw.idIoPinArr + i));
			me->hw.idIoPinState[i] = AlxIoPin_Read_TriState(*(me->hw.idIoPinArr + i));
			AlxIoPin_DeInit(*(me->hw.idIoPinArr + i));
		}

		// Calculate HW ID
		for (uint32_t i = 0; i < me->hw.idIoPinArrLen; i++)
		{
			me->hw.instance.id = me->hw.instance.id + (uint32_t)me->hw.idIoPinState[i] * (uint32_t)powf(3UL, i);
		}

		// Set HW instance
		for (uint8_t i = 0; i < me->hw.instanceKnownArrLen; i++)
		{
			uint8_t hwId = me->hw.instanceKnownArr[i].id;

			if (hwId == me->hw.instance.id)
			{
				me->hw.instance = me->hw.instanceKnownArr[i];
				break;	// HW instance was found in the array, so we break from the for loop
			}
			else
			{
				if (i ==  (me->hw.instanceKnownArrLen - 1))
				{
					ALX_ID_ASSERT(false);	// HW instance was NOT found in the array, so we ASSERT
					return;
				}
			}
		}

		// Check if HW instance is supported
		for (uint8_t i = 0; i < me->hw.instanceHwIdSupportedArrLen; i++)
		{
			if (me->hw.instance.id == me->hw.instanceHwIdSupportedArr[i])
			{
				break;	// HW instance is supported, so we break from the for loop
			}
			else
			{
				if (i ==  (me->hw.instanceHwIdSupportedArrLen - 1))
				{
					ALX_ID_ASSERT(false);	// HW instance is NOT supported, so we ASSERT
					return;
				}
			}
		}
	}


	//------------------------------------------------------------------------------
	// Set HW instance PCB version
	//------------------------------------------------------------------------------
	me->hw.instance.pcbVer =
	(
		((uint64_t)me->hw.instance.pcbVerMajor << 56) |
		((uint64_t)me->hw.instance.pcbVerMinor << 48) |
		((uint64_t)me->hw.instance.pcbVerPatch << 32) |
		((uint64_t)me->hw.instance.pcbVerDate)
	);
	sprintf(me->hw.instance.pcbVerStr, "%u.%u.%u.%lu", me->hw.instance.pcbVerMajor, me->hw.instance.pcbVerMinor, me->hw.instance.pcbVerPatch, me->hw.instance.pcbVerDate);


	//------------------------------------------------------------------------------
	// Set HW instance BOM version
	//------------------------------------------------------------------------------
	me->hw.instance.bomVer =
	(
		((uint64_t)me->hw.instance.bomVerMajor << 56) |
		((uint64_t)me->hw.instance.bomVerMinor << 48) |
		((uint64_t)me->hw.instance.bomVerPatch << 32) |
		((uint64_t)me->hw.instance.bomVerDate)
	);
	sprintf(me->hw.instance.bomVerStr, "%u.%u.%u.%lu", me->hw.instance.bomVerMajor, me->hw.instance.bomVerMinor, me->hw.instance.bomVerPatch, me->hw.instance.bomVerDate);


	//------------------------------------------------------------------------------
	// Set isInit
	//------------------------------------------------------------------------------
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxId_Trace(AlxId* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Intro
	//------------------------------------------------------------------------------
	ALX_ID_TRACE_FORMAT("\r\n");
	ALX_ID_TRACE_FORMAT("Auralix C Library ALX ID Module Identification:\r\n");
	ALX_ID_TRACE_FORMAT("\r\n");


	//------------------------------------------------------------------------------
	// FW
	//------------------------------------------------------------------------------
	ALX_ID_TRACE_FORMAT("FW:\r\n");
	ALX_ID_TRACE_FORMAT("- artf: %s\r\n", me->fwApp.artf);
	ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->fwApp.name);
	ALX_ID_TRACE_FORMAT("- ver: %s\r\n", me->fwApp.verStr);
	ALX_ID_TRACE_FORMAT("- bin: %s\r\n", me->fwApp.binStr);
	ALX_ID_TRACE_FORMAT("- job_name: %s\r\n", me->fwApp.build.name);
	ALX_ID_TRACE_FORMAT("- job_number: %lu\r\n", me->fwApp.build.num);
	ALX_ID_TRACE_FORMAT("- job_hash: %s\r\n", me->fwApp.build.hash);
	ALX_ID_TRACE_FORMAT("- job_revision: %lu\r\n", me->fwApp.build.rev);
	ALX_ID_TRACE_FORMAT("\r\n");

	if (me->fwBoot.isBootloader)
	{
		ALX_ID_TRACE_FORMAT("FW Boot:\r\n");
		ALX_ID_TRACE_FORMAT("- artf: %s\r\n", me->fwBoot.artf);
		ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->fwBoot.name);
		ALX_ID_TRACE_FORMAT("- ver: %lu.%lu.%lu.%lu\r\n", me->fwBoot.verMajor, me->fwBoot.verMinor, me->fwBoot.verPatch, me->fwBoot.verDate);
		ALX_ID_TRACE_FORMAT("- job_name: %s\r\n", me->fwBoot.build.name);
		ALX_ID_TRACE_FORMAT("- job_number: %lu\r\n", me->fwBoot.build.num);
		ALX_ID_TRACE_FORMAT("- job_hash: %s\r\n", me->fwBoot.build.hash);
		ALX_ID_TRACE_FORMAT("- job_revision: %lu\r\n", me->fwBoot.build.rev);
		ALX_ID_TRACE_FORMAT("\r\n");
	}

	//------------------------------------------------------------------------------
	// FW - Language C & Language C Standard Library
	//------------------------------------------------------------------------------
	ALX_ID_TRACE_FORMAT("FW - Language:\r\n");
	ALX_ID_TRACE_FORMAT("- name: C, ver: %lu\r\n", me->fwLangC.ver);
	ALX_ID_TRACE_FORMAT("- name: %s, ver: %lu.%lu.%lu\r\n", me->fwLangCLib.name, me->fwLangCLib.verMajor, me->fwLangCLib.verMinor, me->fwLangCLib.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");


	//------------------------------------------------------------------------------
	// FW - Compiler
	//------------------------------------------------------------------------------
	ALX_ID_TRACE_FORMAT("FW - Compiler:\r\n");
	ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->fwComp.name);
	ALX_ID_TRACE_FORMAT("- ver: %lu.%lu.%lu\r\n", me->fwComp.verMajor, me->fwComp.verMinor, me->fwComp.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");


	//------------------------------------------------------------------------------
	// FW - CMSIS Core
	//------------------------------------------------------------------------------
	#ifdef ALX_CMSIS_CORE
	ALX_ID_TRACE_FORMAT("FW - CMSIS Core:\r\n");
	ALX_ID_TRACE_FORMAT("- ver: %lu.%lu\r\n", me->fwCmsisCore.verMajor, me->fwCmsisCore.verMinor);
	ALX_ID_TRACE_FORMAT("\r\n");
	#endif


	//------------------------------------------------------------------------------
	// FW - STM32 - CMSIS & HAL
	//------------------------------------------------------------------------------
	#ifdef ALX_STM32
	ALX_ID_TRACE_FORMAT("FW - STM32:\r\n");
	ALX_ID_TRACE_FORMAT("- name: %s HAL CMSIS, ver: %lu.%lu.%lu\r\n", ALX_STM32_STR, me->fwStm32HalCmsis.verMajor, me->fwStm32HalCmsis.verMinor, me->fwStm32HalCmsis.verPatch);
	ALX_ID_TRACE_FORMAT("- name: %s HAL Driver, ver: %lu.%lu.%lu\r\n", ALX_STM32_STR, me->fwStm32HalDriver.verMajor, me->fwStm32HalDriver.verMinor, me->fwStm32HalDriver.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");
	#endif


	//------------------------------------------------------------------------------
	// HW
	//------------------------------------------------------------------------------
	ALX_ID_TRACE_FORMAT("HW:\r\n");
	ALX_ID_TRACE_FORMAT("- pcb_artf: %s\r\n", me->hw.instance.pcbArtf);
	ALX_ID_TRACE_FORMAT("- pcb_name: %s\r\n", me->hw.instance.pcbName);
	ALX_ID_TRACE_FORMAT("- pcb_ver: %lu.%lu.%lu.%lu\r\n", me->hw.instance.pcbVerMajor, me->hw.instance.pcbVerMinor, me->hw.instance.pcbVerPatch, me->hw.instance.pcbVerDate);
	ALX_ID_TRACE_FORMAT("- pcb_id: %lu\r\n", me->hw.instance.id);
	ALX_ID_TRACE_FORMAT("- bom_artf: %s\r\n", me->hw.instance.bomArtf);
	ALX_ID_TRACE_FORMAT("- bom_name: %s\r\n", me->hw.instance.bomName);
	ALX_ID_TRACE_FORMAT("- bom_ver: %lu.%lu.%lu.%lu\r\n", me->hw.instance.bomVerMajor, me->hw.instance.bomVerMinor, me->hw.instance.bomVerPatch, me->hw.instance.bomVerDate);
	ALX_ID_TRACE_FORMAT("- mcu_name: %s\r\n", me->hw.mcuName);
	ALX_ID_TRACE_FORMAT("\r\n");


	//------------------------------------------------------------------------------
	// HW - STM32
	//------------------------------------------------------------------------------
	#ifdef ALX_STM32
	ALX_ID_TRACE_FORMAT("HW - STM32:\r\n");
	ALX_ID_TRACE_FORMAT("- device ID: %#lX\r\n", me->hwStm32.deviceId);
	ALX_ID_TRACE_FORMAT("- revision ID: %#lX\r\n", me->hwStm32.revisionId);
	ALX_ID_TRACE_FORMAT("- flash size: %lukB\r\n", me->hwStm32.flashSize_kB);
	#ifdef ALX_STM32G4
	ALX_ID_TRACE_FORMAT("- package ID: %#lX\r\n", me->stm32Hw.packageId);
	#endif
	ALX_ID_TRACE_FORMAT("- MCU unique ID: %s\r\n", me->hwStm32.mcuUniqueIdStr);
	ALX_ID_TRACE_FORMAT("\r\n");
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetFwAppArtf(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.artf;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetFwAppName(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.name;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetFwAppVerMajor(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.verMajor;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetFwAppVerMinor(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.verMinor;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetFwAppVerPatch(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.verPatch;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxId_GetFwAppVerDate(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.verDate;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxId_GetFwAppHashShort(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.build.hashShortUint32;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxId_GetFwAppVer(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.ver;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetFwAppVerStr(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.verStr;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetFwAppBinStr(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->fwApp.binStr;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwPcbArtf(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbArtf;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwPcbName(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbName;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwPcbVerMajor(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbVerMajor;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwPcbVerMinor(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbVerMinor;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwPcbVerPatch(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbVerPatch;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxId_GetHwPcbVerDate(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbVerDate;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxId_GetHwPcbVer(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbVer;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwPcbVerStr(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.pcbVerStr;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwBomArtf(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomArtf;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwBomName(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomName;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwBomVerMajor(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomVerMajor;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwBomVerMinor(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomVerMinor;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwBomVerPatch(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomVerPatch;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxId_GetHwBomVerDate(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomVerDate;;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxId_GetHwBomVer(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomVer;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwBomVerStr(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.bomVerStr;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint8_t AlxId_GetHwId(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);
	ALX_ID_ASSERT(me->isInit == true);

	// Return
	return me->hw.instance.id;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]	uniqueIdUint32
  * @param[in]	len
  */
void AlxId_GetHwMcuUniqueIdUint32(AlxId* me, uint32_t* uniqueIdUint32, uint8_t len)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	// Return
	#ifdef ALX_STM32
	memcpy(uniqueIdUint32, me->hwStm32.mcuUniqueId.uint32, len);
	#else
	ALX_ID_ASSERT(false);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]	uniqueIdUint8
  * @param[in]	len
  */
void AlxId_GetHwMcuUniqueIdUint8(AlxId* me, uint8_t* uniqueIdUint8, uint8_t len)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	// Return
	#ifdef ALX_STM32
	memcpy(uniqueIdUint8, me->hwStm32.mcuUniqueId.uint8, len);
	#else
	ALX_ID_ASSERT(false);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
const char* AlxId_GetHwMcuUniqueIdStr(AlxId* me)
{
	// Assert
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	// Return
	#ifdef ALX_STM32
	return me->hwStm32.mcuUniqueIdStr;
	#else
	ALX_ID_ASSERT(false);
	return NULL;
	#endif
}


#endif	// #if defined(ALX_C_LIB)
