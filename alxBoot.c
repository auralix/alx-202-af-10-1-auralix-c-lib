/**
  ******************************************************************************
  * @file		alxBoot.c
  * @brief		Auralix C Library - ALX Bootloader Module
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
#include "alxBoot.h"
#if defined(ALX_BOOT_A) && !defined(ALX_BUILD_CONFIG_NO_BOOT)
	#include ALX_BOOT_A_PRE_COMP_BOOT_HDR_FILE
	#if defined(ALX_BUILD_CONFIG_DEBUG)
		#include "alxBootMetadata_GENERATED.h"
	#endif
	#if defined(ALX_BUILD_CONFIG_FW_UP)
		static const unsigned char app_header[0x0200] __attribute__((section(".app_header"), used)) = {0xDE, 0xAD, 0xBE, 0xEF};
		static const unsigned char app_trailer[0x0028] __attribute__((section(".app_trailer"), used)) = {0xDE, 0xAD, 0xBE, 0xEF};
	#endif
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Variables
//******************************************************************************
#if defined(ALX_BOOT_B)
static const AlxId_FwBootId boot_id __attribute__((section(".boot_id"), used)) =
{
	.magicNum = ALX_ID_BOOT_ID_MAGIC_NUM,
	.ver = ALX_ID_BOOT_ID_VER,
	.fw =
	{
		.build =
		{
			.name = ALX_BUILD_NAME,
			.date = ALX_BUILD_DATE,
			.dateComp = ALX_BUILD_DATE_COMP,
			.num = ALX_BUILD_NUM,
			.hash = ALX_BUILD_HASH,
			.hashShort = ALX_BUILD_HASH_SHORT,
			.hashShortUint32 = ALX_BUILD_HASH_SHORT_UINT32,
			.rev = ALX_BUILD_REV
		},
		.artf = ALX_BOOT_B_ID_FW_ARTF,
		.name = ALX_BOOT_B_ID_FW_NAME,
		.verMajor = ALX_BUILD_FW_VER_MAJOR,
		.verMinor = ALX_BUILD_FW_VER_MINOR,
		.verPatch = ALX_BUILD_FW_VER_PATCH,
		.isBuildJobUsed = true,
		.verDate = 0,	// Will be set by AlxId
		.ver = 0,		// Will be set by AlxId
		.verStr = "",	// Will be set by AlxId
		.binStr = ""	// Will be set by AlxId
	},
	.crc = 0			// For future use
};
#endif


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxBoot_App_Usb_FwCand_ChunkRead_Callback(void* ctx, void* chunkData, uint32_t chunkLenActual);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxBoot_App_Usb_StatusChange_Callback(AlxBoot* me, AlxBoot_App_Status status);
void AlxBoot_App_Usb_FwCandStagingProgress_Callback(AlxBoot* me, uint8_t progress_pct);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBoot_Ctor
(
	AlxBoot* me,
	AlxFs* alxFs,
	AlxId* alxId,
	AlxUsb* alxUsb,
	uint16_t usbReadyTimeout_ms
)
{
	// Parameters
	me->alxFs = alxFs;
	me->alxId = alxId;
	me->alxUsb = alxUsb;
	me->usbReadyTimeout_ms = usbReadyTimeout_ms;

	// Variables
	memset(me->buff, 0, sizeof(me->buff));
	#if defined(ALX_BOOT_A)
	me->fa = NULL;
	me->fwCandStaging_LenWritten_bytes = 0;
	me->fwCandStaging_ProgressStep10_pct = 255;
	#endif
	#if defined(ALX_BOOT_B)
	memset(&me->rsp, 0, sizeof(me->rsp));
	#endif
	me->addrVt = 0;
	me->addrMsp = 0;
	me->addrJmp = 0;

	// Info
	me->wasCtorCalled = true;
	me->isPrepared = false;
}


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
// App
//------------------------------------------------------------------------------
void AlxBoot_App_Usb_Update(AlxBoot* me)
{
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	ALX_BOOT_ASSERT(me->wasCtorCalled == true);




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// FW Candidate Discovery
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_BOOT_TRACE_INF("");
	ALX_BOOT_TRACE_INF("AlxBoot_App_Usb - FW Candidate Discovery START");
	AlxTimSw fwCandDiscovery_AlxTimSw;
	AlxTimSw_Ctor(&fwCandDiscovery_AlxTimSw, true);


	//------------------------------------------------------------------------------
	// Find USB
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("DO: FindUsb()");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FindUsbStart);

	// Init USB
	Alx_Status status = Alx_Err;
	status = AlxUsb_Init(me->alxUsb);
	if (status != Alx_Ok)
	{
		ALX_BOOT_TRACE_ERR("FAIL: AlxUsb_Init() status %ld", status);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_Err);
		return;
	}

	// Wait USB ready
	AlxTimSw alxUsb_alxTimSw;
	AlxTimSw_Ctor(&alxUsb_alxTimSw, true);
	while (1)
	{
		// Handle USB
		status = AlxUsb_Handle(me->alxUsb);
		if (status != Alx_Ok)
		{
			ALX_BOOT_TRACE_ERR("FAIL: AlxUsb_Handle() status %ld", status);
			AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_Err);
			return;
		}

		// Check if USB ready
		bool isReady = AlxUsb_IsReady(me->alxUsb);
		if (isReady)
		{
			break;
		}

		// Check if USB ready timeout
		bool isTimeout = AlxTimSw_IsTimeout_ms(&alxUsb_alxTimSw, me->usbReadyTimeout_ms);
		if (isTimeout)
		{
			ALX_BOOT_TRACE_WRN("FAIL: CheckUsbReadyTimeout() usbReadyTimeout_ms %lu", me->usbReadyTimeout_ms);
			AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_UsbNotFound);
			return;
		}

		// Delay
		AlxDelay_ms(10);
	}

	// Trace
	ALX_BOOT_TRACE_INF("DONE: FindUsb() -> OK");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FindUsbDone);


	//------------------------------------------------------------------------------
	// Find FW Candidate
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("DO: FindFwCand()");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FindFwCandStart);

	// Mount
	status = AlxFs_Mount(me->alxFs);
	if (status != Alx_Ok)
	{
		ALX_BOOT_TRACE_WRN("FAIL: AlxFs_Mount() status %ld", status);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FwCandNotFound);
		return;
	}

	// Delay
	AlxDelay_ms(100);

	// Open dir
	const char* fwArtf = AlxId_GetFwArtf(me->alxId);
	const char* fwName = AlxId_GetFwName(me->alxId);
	AlxFs_Dir dir = {};
	char dirPath[128] = "";
	sprintf(dirPath, "/%s", fwName);
	status = AlxFs_Dir_Open(me->alxFs, &dir, dirPath);
	if (status != Alx_Ok)
	{
		ALX_BOOT_TRACE_WRN("FAIL: AlxFs_Dir_Open() status %ld dirPath %s", status, dirPath);
		AlxFs_UnMount(me->alxFs);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FwCandNotFound);
		return;
	}

	// Loop
	bool fwCandFound = false;
	uint32_t fwCandVerDate = 0;
	char fwCandFilePath[128] = "";
	while (true)
	{
		// Read dir
		AlxFs_Info info = {};
		status = AlxFs_Dir_Read(me->alxFs, &dir, &info);
		if (status == AlxFs_EndOfDir)
		{
			AlxFs_Dir_Close(me->alxFs, &dir);
			break;
		}
		else if (status != Alx_Ok)
		{
			ALX_BOOT_TRACE_WRN("FAIL: AlxFs_Dir_Read() status %ld", status);
			AlxFs_Dir_Close(me->alxFs, &dir);
			AlxFs_UnMount(me->alxFs);
			AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FwCandNotFound);
			return;
		}

		// Check if file
		if (info.fatfsInfo.fattrib & (AM_DIR | AM_HID | AM_SYS))
		{
			ALX_BOOT_TRACE_WRN("SKIP: CheckIfFile() fattrib %lu fname %s", info.fatfsInfo.fattrib, info.fatfsInfo.fname);
			continue;
		}

		// Check file size
		if (info.fatfsInfo.fsize != ALX_MCU_BOOT_IMAGE_SIZE)
		{
			ALX_BOOT_TRACE_WRN("SKIP: CheckFileSize() fsize %lu fname %s", info.fatfsInfo.fsize, info.fatfsInfo.fname);
			continue;
		}

		// Parse file name
		uint32_t _fwCandVerDate = 0;
		char fwCandArtf[ALX_ID_NAME_LEN] = "";
		char fwCandName[ALX_ID_NAME_LEN] = "";
		uint32_t fwCandVerMajor = 0;
		uint32_t fwCandVerMinor = 0;
		uint32_t fwCandVerPatch = 0;
		char fwCandHashShort[ALX_ID_FW_BUILD_HASH_SHORT_LEN] = "";
		int charConsumed = -1;
		uint32_t fwCandFileNameLen = strlen(info.fatfsInfo.fname);
		int sscanfStatus = sscanf
		(
			info.fatfsInfo.fname,
			"%lu_%39[^_]_%39[^_]_V%lu-%lu-%lu_%7[^_]_Signed.bin%n",
			&_fwCandVerDate,
			fwCandArtf,
			fwCandName,
			&fwCandVerMajor,
			&fwCandVerMinor,
			&fwCandVerPatch,
			fwCandHashShort,
			&charConsumed
		);
		if (sscanfStatus != 7 || charConsumed != (int)fwCandFileNameLen)
		{
			ALX_BOOT_TRACE_WRN("SKIP: ParseFileName() fname %s", info.fatfsInfo.fname);
			continue;
		}

		// Check file name FW artifact
		if (strcmp(fwCandArtf, fwArtf) != 0)
		{
			ALX_BOOT_TRACE_WRN("SKIP: CheckFileNameFwArtf() fwCandArtf %s fwArtf %s fname %s", fwCandArtf, fwArtf, info.fatfsInfo.fname);
			continue;
		}

		// Check file name FW name
		if (strcmp(fwCandName, fwName) != 0)
		{
			ALX_BOOT_TRACE_WRN("SKIP: CheckFileNameFwName() fwCandName %s fwName %s fname %s", fwCandName, fwName, info.fatfsInfo.fname);
			continue;
		}

		// Update FW candidate file path
		if (_fwCandVerDate > fwCandVerDate)
		{
			// Update
			fwCandFound = true;
			fwCandVerDate = _fwCandVerDate;
			sprintf(fwCandFilePath, "/%s/%s", fwName, info.fatfsInfo.fname);

			// Trace
			ALX_BOOT_TRACE_INF("UPDATE: FwCandFilePathUpdated() fwCandFilePath %s", fwCandFilePath);
		}
	}

	// Check if FW candidate NOT found
	if (fwCandFound == false)
	{
		ALX_BOOT_TRACE_WRN("FAIL: 'FW candidate NOT found' CheckFwCandNotFound()");
		AlxFs_UnMount(me->alxFs);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FwCandNotFound);
		return;
	}

	// Trace
	ALX_BOOT_TRACE_INF("DONE: FindFwCand() fwCandFilePath %s", fwCandFilePath);
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FindFwCandDone);


	//------------------------------------------------------------------------------
	// Check FW Candidate Version
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("DO: CheckFwCandVer()");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_CheckFwCandVerStart);

	// Check if FW candidate version is the same as active FW version
	uint32_t fwVerDate = AlxId_GetFwVerDate(me->alxId);
	if (fwCandVerDate == fwVerDate)
	{
		ALX_BOOT_TRACE_WRN("FAIL: 'FW candidate REJECTED - FW candidate version SAME as active FW version' CheckFwCandVer() fwCandVerDate %lu fwVerDate %lu", fwCandVerDate, fwVerDate);
		AlxFs_UnMount(me->alxFs);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_FwCandSameVer);
		return;
	}

	// Trace
	ALX_BOOT_TRACE_INF("DONE: 'FW candidate ACCEPTED - FW candidate version DIFFERENT from active FW version' CheckFwCandVer() fwCandVerDate %lu fwVerDate %lu", fwCandVerDate, fwVerDate);
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandDiscovery_CheckFwCandVerDone);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	uint32_t fwCandDiscovery_ExecutionTime_sec = AlxTimSw_Get_sec(&fwCandDiscovery_AlxTimSw);
	ALX_BOOT_TRACE_INF("AlxBoot_App_Usb - FW Candidate Discovery DONE - Execution Time: %lu sec", fwCandDiscovery_ExecutionTime_sec);




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// FW Candidate Staging
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_BOOT_TRACE_INF("");
	ALX_BOOT_TRACE_INF("AlxBoot_App_Usb - FW Candidate Staging START");
	AlxTimSw fwCandStaging_AlxTimSw;
	AlxTimSw_Ctor(&fwCandStaging_AlxTimSw, true);


	//------------------------------------------------------------------------------
	// Erase FLASH
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("DO: EraseFlash()");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandStaging_EraseFlashStart);

	// Open FLASH
	int statusBoot = -1;
	int id = flash_area_id_from_image_slot(1);
	statusBoot = flash_area_open(id, &me->fa);
	if (statusBoot != 0)
	{
		ALX_BOOT_TRACE_WRN("FAIL: flash_area_open() statusBoot %ld", statusBoot);
		AlxFs_UnMount(me->alxFs);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_Err);
		return;
	}

	// Erase FLASH
	statusBoot = flash_area_erase(me->fa, 0, ALX_MCU_BOOT_IMAGE_SIZE);
	if (statusBoot != 0)
	{
		ALX_BOOT_TRACE_WRN("FAIL: flash_area_erase() statusBoot %ld", statusBoot);
		AlxFs_UnMount(me->alxFs);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_Err);
		return;
	}

	// Trace
	ALX_BOOT_TRACE_INF("DONE: EraseFlash() -> OK");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandStaging_EraseFlashDone);


	//------------------------------------------------------------------------------
	// Write FLASH
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("DO: WriteFlash()");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandStaging_WriteFlashStart);

	// Perform staging
	uint32_t readLen = 0;
	status = AlxFs_File_ReadInChunks(me->alxFs, fwCandFilePath, me->buff, sizeof(me->buff), AlxBoot_App_Usb_FwCand_ChunkRead_Callback, me, &readLen, NULL);
	if (status != Alx_Ok)
	{
		ALX_BOOT_TRACE_WRN("FAIL: AlxFs_File_ReadInChunks() status %ld fwCandFilePath %s", status, fwCandFilePath);
		AlxFs_UnMount(me->alxFs);
		AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_Err);
		return;
	}

	// Close FLASH
	flash_area_close(me->fa);

	// UnMount
	AlxFs_UnMount(me->alxFs);

	// Trace
	ALX_BOOT_TRACE_INF("DONE: WriteFlash() -> OK");
	AlxBoot_App_Usb_StatusChange_Callback(me, AlxBoot_App_Status_FwCandStaging_WriteFlashDone);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	uint32_t fwCandStaging_ExecutionTime_sec = AlxTimSw_Get_sec(&fwCandStaging_AlxTimSw);
	ALX_BOOT_TRACE_INF("AlxBoot_App_Usb - FW Candidate Staging DONE - Execution Time: %lu sec", fwCandStaging_ExecutionTime_sec);




	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	// Reset
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	ALX_BOOT_TRACE_INF("AlxBoot_App_Usb - Reset will occur now..");
	NVIC_SystemReset();
}


//------------------------------------------------------------------------------
// Boot
//------------------------------------------------------------------------------
Alx_Status AlxBoot_Prepare(AlxBoot* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_BOOT_ASSERT(me->wasCtorCalled == true);
	ALX_BOOT_ASSERT(me->isPrepared == false);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_BOOT_TRACE_INF("");
	ALX_BOOT_TRACE_INF("AlxBoot - Prepare START");


	//------------------------------------------------------------------------------
	// Execute boot_go
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("AlxBoot - boot_go START");

	// Execute
	#if defined(ALX_BOOT_B)
	fih_ret status = boot_go(&me->rsp);
	if (status != FIH_SUCCESS)
	{
		ALX_BOOT_TRACE_WRN("Err");
		return Alx_Err;
	}

	// Check FLASH_DEVICE_ID
	if (me->rsp.br_flash_dev_id != ALX_MCU_BOOT_FLASH_DEVICE_ID)
	{
		ALX_BOOT_TRACE_WRN("Err");
		return Alx_Err;
	}
	#endif

	// Trace
	ALX_BOOT_TRACE_INF("AlxBoot - boot_go FINISH");
	#if defined(ALX_BOOT_B)
	ALX_BOOT_TRACE_INF("- br_hdr->ih_magic = 0x%08lX", me->rsp.br_hdr->ih_magic);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_load_addr = 0x%08lX", me->rsp.br_hdr->ih_load_addr);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_hdr_size = 0x%04lX", me->rsp.br_hdr->ih_hdr_size);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_protect_tlv_size = 0x%04lX", me->rsp.br_hdr->ih_protect_tlv_size);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_img_size = %lu", me->rsp.br_hdr->ih_img_size);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_flags = 0x%08lX", me->rsp.br_hdr->ih_flags);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_ver.iv_major = %lu", me->rsp.br_hdr->ih_ver.iv_major);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_ver.iv_minor = %lu", me->rsp.br_hdr->ih_ver.iv_minor);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_ver.iv_revision = %lu", me->rsp.br_hdr->ih_ver.iv_revision);
	ALX_BOOT_TRACE_INF("- br_hdr->ih_ver.iv_build_num = %lu", me->rsp.br_hdr->ih_ver.iv_build_num);
	ALX_BOOT_TRACE_INF("- br_flash_dev_id = %lu", me->rsp.br_flash_dev_id);
	ALX_BOOT_TRACE_INF("- br_image_off = 0x%08lX", me->rsp.br_image_off);
	#endif


	//------------------------------------------------------------------------------
	// Set App Address
	//------------------------------------------------------------------------------

	// Set
	#if defined(ALX_BOOT_B)
	me->addrVt = me->rsp.br_image_off + me->rsp.br_hdr->ih_hdr_size;
	#endif
	me->addrMsp = *(volatile uint32_t*)(me->addrVt);
	me->addrJmp = *(volatile uint32_t*)(me->addrVt + 4);

	// Trace
	ALX_BOOT_TRACE_INF("- addrVt = 0x%08lX", me->addrVt);
	ALX_BOOT_TRACE_INF("- addrMsp = 0x%08lX", me->addrMsp);
	ALX_BOOT_TRACE_INF("- addrJmp = 0x%08lX", me->addrJmp);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Trace
	ALX_BOOT_TRACE_INF("AlxBoot - Prepare FINISH, ready to jump to application");

	// Set isInit
	me->isPrepared = true;

	// Return
	return Alx_Ok;
}
void AlxBoot_Jump(AlxBoot* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_BOOT_ASSERT(me->wasCtorCalled == true);
	ALX_BOOT_ASSERT(me->isPrepared == true);


	//------------------------------------------------------------------------------
	// Jump
	//------------------------------------------------------------------------------

	// Disable global IRQ
	__disable_irq();

	// Disable all NVIC IRQ
	for (uint32_t i = 0; i < ALX_ARR_LEN(NVIC->ICER); i++)
	{
		NVIC->ICER[i] = 0xFFFFFFFF;
	}

	// Clear all pending NVIC IRQ
	for (uint32_t i = 0; i < ALX_ARR_LEN(NVIC->ICPR); i++)
	{
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}

	// Set VTOR to the app's vector table address
	SCB->VTOR = (uint32_t)me->addrVt;

	// Set app's stack pointer
	__set_MSP(me->addrMsp);

	// Ensure that app starts in privileged mode and uses MSP
	__set_CONTROL(0x00);

	// Ensure that all previous ARM's core register changes are completed
	__ISB();

	// Jump to app's reset handler
	((void(*)(void))me->addrJmp)();	// Cast jump address to function pointer & then execute it - Compiler automatically handles, that the real jump address is odd number, so it automatically adds +1 to addrJmp, this is needed for all ARM Cortex-M series processor (for THUMB instruction execution)
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxBoot_App_Usb_FwCand_ChunkRead_Callback(void* ctx, void* chunkData, uint32_t chunkLenActual)
{
	// Local variables
	AlxBoot* me = (AlxBoot*)ctx;

	// Write FLASH area
	int statusBoot = flash_area_write(me->fa, me->fwCandStaging_LenWritten_bytes, chunkData, chunkLenActual);
	if (statusBoot != 0)
	{
		ALX_BOOT_TRACE_WRN("FAIL: flash_area_write() statusBoot %ld", statusBoot);
		return Alx_Err;
	}

	// Calculate progress
	uint8_t fwCandStaging_Progress_pct = ((uint64_t)me->fwCandStaging_LenWritten_bytes * 100) / ALX_MCU_BOOT_IMAGE_SIZE;
	uint8_t fwCandStaging_ProgressStep10_pct = fwCandStaging_Progress_pct / 10 * 10;

	// Trace progress
	if (fwCandStaging_ProgressStep10_pct != me->fwCandStaging_ProgressStep10_pct)
	{
		// Update
		me->fwCandStaging_ProgressStep10_pct = fwCandStaging_ProgressStep10_pct;

		// Trace
		ALX_BOOT_TRACE_INF("%02u%% - %lu/%lu bytes", fwCandStaging_ProgressStep10_pct, me->fwCandStaging_LenWritten_bytes, ALX_MCU_BOOT_IMAGE_SIZE);

		// Callback
		AlxBoot_App_Usb_FwCandStagingProgress_Callback(me, fwCandStaging_ProgressStep10_pct);
	}

	// Update length written
	me->fwCandStaging_LenWritten_bytes = me->fwCandStaging_LenWritten_bytes + chunkLenActual;

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxBoot_App_Usb_StatusChange_Callback(AlxBoot* me, AlxBoot_App_Status status)
{
	(void)me;
	(void)status;
}
ALX_WEAK void AlxBoot_App_Usb_FwCandStagingProgress_Callback(AlxBoot* me, uint8_t progress_pct)
{
	(void)me;
	(void)progress_pct;
}


#endif	// #if defined(ALX_C_LIB)
