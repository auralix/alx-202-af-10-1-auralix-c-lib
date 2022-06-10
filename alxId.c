/**
  ******************************************************************************
  * @file alxId.c
  * @brief Auralix C Library - ALX ID Module
  * @version $LastChangedRevision: 5738 $
  * @date $LastChangedDate: 2021-08-06 18:04:11 +0200 (Fri, 06 Aug 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxId.h"


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
)
{
	//******************************************************************************
	// Parameters
	//******************************************************************************
	me->fwIsBootloader = fwIsBootloader;
	strcpy(me->hwMcuName, hwMcuName);


	//******************************************************************************
	// FW
	//******************************************************************************
	if (me->fwIsBootloader)
	{
		// Parameters - Const
		strcpy(me->fwBoot.build.name, ALX_BUILD_NAME);
		me->fwBoot.build.date = ALX_BUILD_DATE;
		me->fwBoot.build.dateComp = ALX_BUILD_DATE_COMP;
		me->fwBoot.build.num = ALX_BUILD_NUM;
		strcpy(me->fwBoot.build.hash, ALX_BUILD_HASH);
		me->fwBoot.build.rev = ALX_BUILD_REV;

		// Parameters
		strcpy(me->fwBoot.artf, fwArtf);
		strcpy(me->fwBoot.name, fwName);
		me->fwBoot.verMajor = fwVerMajor;
		me->fwBoot.verMinor = fwVerMinor;
		me->fwBoot.verPatch = fwVerPatch;
		me->fwBoot.isBuildJobUsed = fwIsBuildJobUsed;
		if (me->fwBoot.isBuildJobUsed)
		{
			me->fwBoot.verDate = me->fwBoot.build.date;
		}
		else
		{
			me->fwBoot.verDate = me->fwBoot.build.dateComp;
		}
		me->fwBoot.ver =	((uint64_t)me->fwBoot.verMajor << 56) |
							((uint64_t)me->fwBoot.verMinor << 48) |
							((uint64_t)me->fwBoot.verPatch << 32) |
							((uint64_t)me->fwBoot.verDate);
	}
	else
	{
		// Parameters - Const
		strcpy(me->fwApp.build.name, ALX_BUILD_NAME);
		me->fwApp.build.date = ALX_BUILD_DATE;
		me->fwApp.build.dateComp = ALX_BUILD_DATE_COMP;
		me->fwApp.build.num = ALX_BUILD_NUM;
		strcpy(me->fwApp.build.hash, ALX_BUILD_HASH);
		me->fwApp.build.rev = ALX_BUILD_REV;

		// Parameters
		strcpy(me->fwApp.artf, fwArtf);
		strcpy(me->fwApp.name, fwName);
		me->fwApp.verMajor = fwVerMajor;
		me->fwApp.verMinor = fwVerMinor;
		me->fwApp.verPatch = fwVerPatch;
		me->fwApp.isBuildJobUsed = fwIsBuildJobUsed;
		if (me->fwApp.isBuildJobUsed)
		{
			me->fwApp.verDate = me->fwApp.build.date;
		}
		else
		{
			me->fwApp.verDate = me->fwApp.build.dateComp;
		}
		me->fwApp.ver =	((uint64_t)me->fwApp.verMajor << 56) |
						((uint64_t)me->fwApp.verMinor << 48) |
						((uint64_t)me->fwApp.verPatch << 32) |
						((uint64_t)me->fwApp.verDate);

		sprintf(me->fwApp.verStr, "%u.%u.%u.%lu.%s", me->fwApp.verMajor, me->fwApp.verMinor, me->fwApp.verPatch, me->fwApp.verDate, me->fwApp.build.hash);
	}


	//******************************************************************************
	// HW
	//******************************************************************************
	// Objects - External
	me->hw.instanceKnownArr = hwInstanceKnownArr;
	me->hw.instanceHwIdSupportedArr = hwInstanceHwIdSupportedArr;
	me->hw.idIoPinArr = hwIdIoPinArr;

	// Parameters
	me->hw.instanceKnownArrLen = hwInstanceKnownArrLen;
	me->hw.instanceHwIdSupportedArrLen = hwInstanceHwIdSupportedArrLen;
	me->hw.idIoPinArrLen = hwIdIoPinArrLen;


	//******************************************************************************
	// Language C
	//******************************************************************************
	#ifdef ALX_GCC
		// Preprocessor
		#if (__STDC_VERSION__ < ALX_LANG_C_VER_MIN_REQUIRED)
			#error
		#endif

		// Parameters - Const
		me->langC.ver = __STDC_VERSION__;
		me->langC.verMinRequired = ALX_LANG_C_VER_MIN_REQUIRED;
	#endif


	//******************************************************************************
	// Language C - Standard Library
	//******************************************************************************
	#ifdef ALX_NEWLIB_NANO
		// Preprocessor
		#if (ALX_xVER3(__NEWLIB__, __NEWLIB_MINOR__, __NEWLIB_PATCHLEVEL__) < ALX_LANG_C_LIB_VER_MIN_REQUIRED)
			#error
		#endif

		// Parameters - Const
		strcpy(me->langCLib.name, "Newlib-Nano C Standard Library");
		me->langCLib.verMajor = __NEWLIB__;
		me->langCLib.verMinor = __NEWLIB_MINOR__;
		me->langCLib.verPatch = __NEWLIB_PATCHLEVEL__;
		me->langCLib.ver = ALX_xVER3(__NEWLIB__, __NEWLIB_MINOR__, __NEWLIB_PATCHLEVEL__);
		me->langCLib.verMinRequired = ALX_LANG_C_LIB_VER_MIN_REQUIRED;
	#endif


	//******************************************************************************
	// Compiler
	//******************************************************************************
	#ifdef ALX_GCC
		// Preprocessor
		// GNU -> [G]NU is [N]ot [U]nix
		// GCC -> [G]NU [C]ompiler [C]ollection
		#if (ALX_xVER3(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__) < ALX_COMP_VER_MIN_REQUIRED)
			#error
		#endif

		// Parameters - Const
		strcpy(me->comp.name, "GCC");
		me->comp.verMajor = __GNUC__;
		me->comp.verMinor = __GNUC_MINOR__;
		me->comp.verPatch = __GNUC_PATCHLEVEL__;
		me->comp.ver = ALX_xVER3(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
		me->comp.verMinRequired = ALX_COMP_VER_MIN_REQUIRED;
	#endif


	//******************************************************************************
	// CMSIS Core
	//******************************************************************************
	#ifdef ALX_CMSIS_CORE
		// Preprocessor
		#if (__CM_CMSIS_VERSION != ALX_CMSIS_CORE_VER_REQUIRED)
			#error
		#endif

		// Parameters - Const
		me->cmsisCore.verMajor = __CM_CMSIS_VERSION_MAIN;
		me->cmsisCore.verMinor = __CM_CMSIS_VERSION_SUB;
		me->cmsisCore.ver = __CM_CMSIS_VERSION;
		me->cmsisCore.verRequired = ALX_CMSIS_CORE_VER_REQUIRED;
	#endif


	//******************************************************************************
	// STM32 - CMSIS
	//******************************************************************************
	#ifdef ALX_STM32
		// Preprocessor
		#if (ALX_STM32_CMSIS_VERSION != ALX_STM32_CMSIS_VER_REQUIRED)
			#error
		#endif

		// Parameters - Const
		me->stm32HalCmsis.verMajor = ALX_STM32_CMSIS_VERSION_MAIN;
		me->stm32HalCmsis.verMinor = ALX_STM32_CMSIS_VERSION_SUB1;
		me->stm32HalCmsis.verPatch = ALX_STM32_CMSIS_VERSION_SUB2;
		me->stm32HalCmsis.verRc = ALX_STM32_CMSIS_VERSION_RC;
		me->stm32HalCmsis.ver = ALX_STM32_CMSIS_VERSION;
		me->stm32HalCmsis.verRequired = ALX_STM32_CMSIS_VER_REQUIRED;
	#endif


	//******************************************************************************
	// STM32 - HAL
	//******************************************************************************
	#ifdef ALX_STM32
		// Parameters - Const
		me->stm32HalDriver.ver			= HAL_GetHalVersion();
		me->stm32HalDriver.verMajor		= (me->stm32HalDriver.ver & 0xFF000000) >> 24;
		me->stm32HalDriver.verMinor		= (me->stm32HalDriver.ver & 0x00FF0000) >> 16;
		me->stm32HalDriver.verPatch		= (me->stm32HalDriver.ver & 0x0000FF00) >> 8;
		me->stm32HalDriver.verRc		= (me->stm32HalDriver.ver & 0x000000FF) >> 0;
		me->stm32HalDriver.verRequired	= ALX_STM32_HAL_VER_REQUIRED;

		ALX_ID_ASSERT(me->stm32HalDriver.ver == me->stm32HalDriver.verRequired);
	#endif


	//******************************************************************************
	// STM32 - HW
	//******************************************************************************

	#ifdef ALX_STM32
		// Parameters - Const
		me->stm32Hw.deviceId = HAL_GetDEVID();
		me->stm32Hw.revisionId = HAL_GetREVID();
		me->stm32Hw.flashSize_kB = *(uint16_t*)(FLASHSIZE_BASE);
		#ifdef ALX_STM32G4
			me->stm32Hw.packageId = (*(uint16_t*)(0x1FFF7500) & 0x001F);
		#endif
		me->stm32Hw.uniqueId0 = *(uint32_t*)(UID_BASE);
		me->stm32Hw.uniqueId1 = *(uint32_t*)(UID_BASE + 4);
		me->stm32Hw.uniqueId2 = *(uint32_t*)(UID_BASE + 8);
		sprintf(me->stm32Hw.uniqueIdStr, "%08lX%08lX%08lX", me->stm32Hw.uniqueId2, me->stm32Hw.uniqueId1, me->stm32Hw.uniqueId0);
	#endif

	//******************************************************************************
	// Info
	//******************************************************************************
	me->isInit = false;
	me->wasCtorCalled = true;
	me->isHwIdUsed = true;
}
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
	//******************************************************************************
	// Ctor
	//******************************************************************************
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


	//******************************************************************************
	// HW
	//******************************************************************************
	me->hw.instance = hwInstance;


	//******************************************************************************
	// Info
	//******************************************************************************
	me->isHwIdUsed = false;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxId_Init(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == false);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	if (me->isHwIdUsed)
	{
		// #1 Get HW ID Pin States
		for (uint32_t i = 0; i < me->hw.idIoPinArrLen; i++)
		{
			AlxIoPin_Init(*(me->hw.idIoPinArr + i));
			me->hw.idIoPinState[i] = AlxIoPin_Read_TriState(*(me->hw.idIoPinArr + i));
			AlxIoPin_DeInit(*(me->hw.idIoPinArr + i));
		}

		// #2 Calculate HW ID
		for (uint32_t i = 0; i < me->hw.idIoPinArrLen; i++)
		{
			me->hw.instance.id = me->hw.instance.id + (uint32_t)me->hw.idIoPinState[i] * (uint32_t)powf(3UL, i);
		}

		// #3 Set HW Instance
		for (uint8_t i = 0; i < me->hw.instanceKnownArrLen; i++)
		{
			uint8_t hwId = me->hw.instanceKnownArr[i].id;

			if (hwId == me->hw.instance.id)
			{
				me->hw.instance = me->hw.instanceKnownArr[i];
				break; // HW instance was found in the array, so we break from for loop
			}
			else
			{
				if (i ==  (me->hw.instanceKnownArrLen - 1))
				{
					ALX_ID_ASSERT(false); // HW instance was NOT found in the array, so we ASSERT
					return;
				}
			}
		}

		// #4 Check if HW Instance is Supported
		for (uint8_t i = 0; i < me->hw.instanceHwIdSupportedArrLen; i++)
		{
			if (me->hw.instance.id == me->hw.instanceHwIdSupportedArr[i])
			{
				break; // HW instance is supported, so we break the for loop
			}
			else
			{
				if (i ==  (me->hw.instanceHwIdSupportedArrLen - 1))
				{
					ALX_ID_ASSERT(false); // HW instance is NOT supported, so we ASSERT
					return;
				}
			}
		}
	}

	// #5 Set HW Instance PCB Version
	me->hw.instance.pcbVer =	((uint64_t)me->hw.instance.pcbVerMajor << 56) |
								((uint64_t)me->hw.instance.pcbVerMinor << 48) |
								((uint64_t)me->hw.instance.pcbVerPatch << 32) |
								((uint64_t)me->hw.instance.pcbVerDate);

	// #6 Set HW Instance BOM Version
	me->hw.instance.bomVer =	((uint64_t)me->hw.instance.bomVerMajor << 56) |
								((uint64_t)me->hw.instance.bomVerMinor << 48) |
								((uint64_t)me->hw.instance.bomVerPatch << 32) |
								((uint64_t)me->hw.instance.bomVerDate);

	// #7 Set isInit
	me->isInit = true;
}
void AlxId_Trace(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == true);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	ALX_ID_TRACE_FORMAT("\r\n");
	ALX_ID_TRACE_FORMAT("Auralix C Library ALX ID Module Identification:\r\n");
	ALX_ID_TRACE_FORMAT("\r\n");

	ALX_ID_TRACE_FORMAT("Firmware:\r\n");
	ALX_ID_TRACE_FORMAT("- artf: %s\r\n", me->fwApp.artf);
	ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->fwApp.name);
	ALX_ID_TRACE_FORMAT("- ver: %s\r\n", me->fwApp.verStr);
	ALX_ID_TRACE_FORMAT("- job_name: %s\r\n", me->fwApp.build.name);
	ALX_ID_TRACE_FORMAT("- job_number: %lu\r\n", me->fwApp.build.num);
	ALX_ID_TRACE_FORMAT("- job_hash: %s\r\n", me->fwApp.build.hash);
	ALX_ID_TRACE_FORMAT("- job_revision: %lu\r\n", me->fwApp.build.rev);
	ALX_ID_TRACE_FORMAT("\r\n");

	if (me->fwIsBootloader)
	{
		ALX_ID_TRACE_FORMAT("Software Boot:\r\n");
		ALX_ID_TRACE_FORMAT("- artf: %s\r\n", me->fwBoot.artf);
		ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->fwBoot.name);
		ALX_ID_TRACE_FORMAT("- ver: %lu.%lu.%lu.%lu\r\n", me->fwBoot.verMajor, me->fwBoot.verMinor, me->fwBoot.verPatch, me->fwBoot.verDate);
		ALX_ID_TRACE_FORMAT("- job_name: %s\r\n", me->fwBoot.build.name);
		ALX_ID_TRACE_FORMAT("- job_number: %lu\r\n", me->fwBoot.build.num);
		ALX_ID_TRACE_FORMAT("- job_hash: %s\r\n", me->fwBoot.build.hash);
		ALX_ID_TRACE_FORMAT("- job_revision: %lu\r\n", me->fwBoot.build.rev);
		ALX_ID_TRACE_FORMAT("\r\n");
	}

	ALX_ID_TRACE_FORMAT("Hardware:\r\n");
	ALX_ID_TRACE_FORMAT("- pcb_artf: %s\r\n", me->hw.instance.pcbArtf);
	ALX_ID_TRACE_FORMAT("- pcb_name: %s\r\n", me->hw.instance.pcbName);
	ALX_ID_TRACE_FORMAT("- pcb_ver: %lu.%lu.%lu.%lu\r\n", me->hw.instance.pcbVerMajor, me->hw.instance.pcbVerMinor, me->hw.instance.pcbVerPatch, me->hw.instance.pcbVerDate);
	ALX_ID_TRACE_FORMAT("- pcb_id: %lu\r\n", me->hw.instance.id);
	ALX_ID_TRACE_FORMAT("- bom_artf: %s\r\n", me->hw.instance.bomArtf);
	ALX_ID_TRACE_FORMAT("- bom_name: %s\r\n", me->hw.instance.bomName);
	ALX_ID_TRACE_FORMAT("- bom_ver: %lu.%lu.%lu.%lu\r\n", me->hw.instance.bomVerMajor, me->hw.instance.bomVerMinor, me->hw.instance.bomVerPatch, me->hw.instance.bomVerDate);
	ALX_ID_TRACE_FORMAT("\r\n");

	ALX_ID_TRACE_FORMAT("Language:\r\n");
	ALX_ID_TRACE_FORMAT("- name: C, ver: %lu\r\n", me->langC.ver);
	ALX_ID_TRACE_FORMAT("- name: %s, ver: %lu.%lu.%lu\r\n", me->langCLib.name, me->langCLib.verMajor, me->langCLib.verMinor, me->langCLib.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");

	ALX_ID_TRACE_FORMAT("Compiler:\r\n");
	ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->comp.name);
	ALX_ID_TRACE_FORMAT("- ver: %lu.%lu.%lu\r\n", me->comp.verMajor, me->comp.verMinor, me->comp.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");

	#ifdef ALX_CMSIS_CORE
	ALX_ID_TRACE_FORMAT("CMSIS Core:\r\n");
	ALX_ID_TRACE_FORMAT("- ver: %lu.%lu\r\n", me->cmsisCore.verMajor, me->cmsisCore.verMinor);
	ALX_ID_TRACE_FORMAT("\r\n");
	#endif

	#ifdef ALX_STM32
	ALX_ID_TRACE_FORMAT("STM32 Firmware:\r\n");
	ALX_ID_TRACE_FORMAT("- name: %s HAL CMSIS, ver: %lu.%lu.%lu\r\n", ALX_STM32_STR, me->stm32HalCmsis.verMajor, me->stm32HalCmsis.verMinor, me->stm32HalCmsis.verPatch);
	ALX_ID_TRACE_FORMAT("- name: %s HAL Driver, ver: %lu.%lu.%lu\r\n", ALX_STM32_STR, me->stm32HalDriver.verMajor, me->stm32HalDriver.verMinor, me->stm32HalDriver.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");

	ALX_ID_TRACE_FORMAT("STM32 Hardware:\r\n");
	ALX_ID_TRACE_FORMAT("- device: %s\r\n", me->hwMcuName);
	ALX_ID_TRACE_FORMAT("- device ID: %#lX\r\n", me->stm32Hw.deviceId);
	ALX_ID_TRACE_FORMAT("- revision ID: %#lX\r\n", me->stm32Hw.revisionId);
	ALX_ID_TRACE_FORMAT("- flash size: %lukB\r\n", me->stm32Hw.flashSize_kB);
	#ifdef ALX_STM32G4
	ALX_ID_TRACE_FORMAT("- package ID: %#lX\r\n", me->stm32Hw.packageId);
	#endif
	//ALX_ID_TRACE_FORMAT("- unique ID: %#010lX_%lX_%lX\r\n", me->stm32Hw.uniqueId0, me->stm32Hw.uniqueId1, me->stm32Hw.uniqueId2);
	ALX_ID_TRACE_FORMAT("- unique ID: %s\r\n", me->stm32Hw.uniqueIdStr);
	ALX_ID_TRACE_FORMAT("\r\n");
	#endif
}
uint8_t AlxId_GetHwId(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == true);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	return me->hw.instance.id;
}
const char* AlxId_GetUniqueIdStr(AlxId* me)
{
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	#ifdef ALX_STM32
	return me->stm32Hw.uniqueIdStr;
	#endif
}
uint32_t AlxId_GetFwAppVerDate(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == true);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	return me->fwApp.verDate;
}
const char* AlxId_GetFwAppVerStr(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == true);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	return me->fwApp.verStr;
}
