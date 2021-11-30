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
// Private Functions
//******************************************************************************
static AlxId_HwIdIoPinState AlxId_GetHwIdPinState(AlxIoPin* di_HW_ID);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxId_Ctor
(
	AlxId* me,
	const char* swArtf,
	const char* swName,
	uint8_t swVerMajor,
	uint8_t swVerMinor,
	uint8_t swVerPatch,
	bool swIsBuildJobUsed,
	bool swIsBootloader,
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
	me->swIsBootloader = swIsBootloader;
	strcpy(me->hwMcuName, hwMcuName);


	//******************************************************************************
	// SW
	//******************************************************************************
	if (me->swIsBootloader)
	{
		// Parameters - Const
		strcpy(me->swBoot.build.name, ALX_BUILD_NAME);
		me->swBoot.build.date = ALX_BUILD_DATE;
		me->swBoot.build.dateComp = ALX_BUILD_DATE_COMP;
		me->swBoot.build.num = ALX_BUILD_NUM;
		me->swBoot.build.hash = ALX_BUILD_HASH;
		me->swBoot.build.rev = ALX_BUILD_REV;

		// Parameters
		strcpy(me->swBoot.artf, swArtf);
		strcpy(me->swBoot.name, swName);
		me->swBoot.verMajor = swVerMajor;
		me->swBoot.verMinor = swVerMinor;
		me->swBoot.verPatch = swVerPatch;
		me->swBoot.isBuildJobUsed = swIsBuildJobUsed;
		if (me->swBoot.isBuildJobUsed)
		{
			me->swBoot.verDate = me->swBoot.build.date;
		}
		else
		{
			me->swBoot.verDate = me->swBoot.build.dateComp;
		}
		me->swBoot.ver =	((uint64_t)me->swBoot.verMajor << 56) |
							((uint64_t)me->swBoot.verMinor << 48) |
							((uint64_t)me->swBoot.verPatch << 32) |
							((uint64_t)me->swBoot.verDate);
	}
	else
	{
		// Parameters - Const
		strcpy(me->swApp.build.name, ALX_BUILD_NAME);
		me->swApp.build.date = ALX_BUILD_DATE;
		me->swApp.build.dateComp = ALX_BUILD_DATE_COMP;
		me->swApp.build.num = ALX_BUILD_NUM;
		me->swApp.build.hash = ALX_BUILD_HASH;
		me->swApp.build.rev = ALX_BUILD_REV;

		// Parameters
		strcpy(me->swApp.artf, swArtf);
		strcpy(me->swApp.name, swName);
		me->swApp.verMajor = swVerMajor;
		me->swApp.verMinor = swVerMinor;
		me->swApp.verPatch = swVerPatch;
		me->swApp.isBuildJobUsed = swIsBuildJobUsed;
		if (me->swApp.isBuildJobUsed)
		{
			me->swApp.verDate = me->swApp.build.date;
		}
		else
		{
			me->swApp.verDate = me->swApp.build.dateComp;
		}
		me->swApp.ver =	((uint64_t)me->swApp.verMajor << 56) |
						((uint64_t)me->swApp.verMinor << 48) |
						((uint64_t)me->swApp.verPatch << 32) |
						((uint64_t)me->swApp.verDate);

		sprintf(me->swApp.verStr, "%u.%u.%u.%lu", me->swApp.verMajor, me->swApp.verMinor, me->swApp.verPatch, me->swApp.verDate);
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
	const char* swArtf,
	const char* swName,
	uint8_t swVerMajor,
	uint8_t swVerMinor,
	uint8_t swVerPatch,
	bool swIsBuildJobUsed,
	bool swIsBootloader,
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
		swArtf,
		swName,
		swVerMajor,
		swVerMinor,
		swVerPatch,
		swIsBuildJobUsed,
		swIsBootloader,
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
			me->hw.idIoPinState[i] = AlxId_GetHwIdPinState(*(me->hw.idIoPinArr + i));
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

	ALX_ID_TRACE_FORMAT("Software App:\r\n");
	ALX_ID_TRACE_FORMAT("- artf: %s\r\n", me->swApp.artf);
	ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->swApp.name);
	ALX_ID_TRACE_FORMAT("- ver: %s\r\n", me->swApp.verStr);
	ALX_ID_TRACE_FORMAT("- job_name: %s\r\n", me->swApp.build.name);
	ALX_ID_TRACE_FORMAT("- job_number: %lu\r\n", me->swApp.build.num);
	ALX_ID_TRACE_FORMAT("- job_hash: %lu\r\n", me->swApp.build.hash);
	ALX_ID_TRACE_FORMAT("- job_revision: %lu\r\n", me->swApp.build.rev);
	ALX_ID_TRACE_FORMAT("\r\n");

	ALX_ID_TRACE_FORMAT("Software Boot:\r\n");
	ALX_ID_TRACE_FORMAT("- artf: %s\r\n", me->swBoot.artf);
	ALX_ID_TRACE_FORMAT("- name: %s\r\n", me->swBoot.name);
	ALX_ID_TRACE_FORMAT("- ver: %lu.%lu.%lu.%lu\r\n", me->swBoot.verMajor, me->swBoot.verMinor, me->swBoot.verPatch, me->swBoot.verDate);
	ALX_ID_TRACE_FORMAT("- job_name: %s\r\n", me->swBoot.build.name);
	ALX_ID_TRACE_FORMAT("- job_number: %lu\r\n", me->swBoot.build.num);
	ALX_ID_TRACE_FORMAT("- job_hash: %lu\r\n", me->swBoot.build.hash);
	ALX_ID_TRACE_FORMAT("- job_revision: %lu\r\n", me->swBoot.build.rev);
	ALX_ID_TRACE_FORMAT("\r\n");

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
	ALX_ID_TRACE_FORMAT("STM32 Software:\r\n");
	ALX_ID_TRACE_FORMAT("- name: %s HAL CMSIS, ver: %lu.%lu.%lu\r\n", ALX_STM32_STR, me->stm32HalCmsis.verMajor, me->stm32HalCmsis.verMinor, me->stm32HalCmsis.verPatch);
	ALX_ID_TRACE_FORMAT("- name: %s HAL Driver, ver: %lu.%lu.%lu\r\n", ALX_STM32_STR, me->stm32HalDriver.verMajor, me->stm32HalDriver.verMinor, me->stm32HalDriver.verPatch);
	ALX_ID_TRACE_FORMAT("\r\n");

	ALX_ID_TRACE_FORMAT("STM32 Hardware:\r\n");
	ALX_ID_TRACE_FORMAT("- device: %s\r\n", me->hwMcuName);
	ALX_ID_TRACE_FORMAT("- device ID: %#lX\r\n", me->stm32Hw.deviceId);
	ALX_ID_TRACE_FORMAT("- revision ID: %#lX\r\n", me->stm32Hw.revisionId);
	ALX_ID_TRACE_FORMAT("- flash size: %lu kB\r\n", me->stm32Hw.flashSize_kB);
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

	return me->stm32Hw.uniqueIdStr;
}
uint32_t AlxId_GetSwAppVerDate(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == true);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	return me->swApp.verDate;
}
const char* AlxId_GetSwAppVerStr(AlxId* me)
{
	ALX_ID_ASSERT(me->isInit == true);
	ALX_ID_ASSERT(me->wasCtorCalled == true);

	return me->swApp.verStr;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static AlxId_HwIdIoPinState AlxId_GetHwIdPinState(AlxIoPin* di_HW_ID)
{
	//******************************************************************************
	// #1 Read @ PullUp
	//******************************************************************************
	// Config
	AlxIoPin_Config_PullUp(di_HW_ID);
	// Init
	AlxIoPin_Init(di_HW_ID);
	// Wait
	AlxDelay_ms(2);
	// ReadVal
	bool valPullUp = AlxIoPin_Read(di_HW_ID);
	// DeInit
	AlxIoPin_DeInit(di_HW_ID);


	//******************************************************************************
	// #2 Read @ PullDown
	//******************************************************************************
	// Config
	AlxIoPin_Config_PullDown(di_HW_ID);
	// Init
	AlxIoPin_Init(di_HW_ID);
	// Wait
	AlxDelay_ms(2);
	// ReadVal
	bool valPullDown = AlxIoPin_Read(di_HW_ID);
	// DeInit
	AlxIoPin_DeInit(di_HW_ID);


	//******************************************************************************
	// #3 Handle Return
	//******************************************************************************
	if((valPullUp == true) && (valPullDown == false))
	{
		return Alx_IoPinState_Floating;
	}
	else if ((valPullUp == true) && (valPullDown == true))
	{
		return Alx_IoPinState_High;
	}
	else if ((valPullUp == false) && (valPullDown == false))
	{
		return Alx_IoPinState_Low;
	}
	else
	{
		ALX_ID_ASSERT(false); // We should never get here
		return ALX_NULL;
	}
}
