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
#if defined(ALX_BOOT_A)
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
// Constructor
//******************************************************************************
void AlxBoot_Ctor
(
	AlxBoot* me
)
{
	// Variables
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
#if defined(ALX_BOOT_B)
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

	// Trace
	ALX_BOOT_TRACE_INF("AlxBoot - boot_go FINISH");
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


	//------------------------------------------------------------------------------
	// Set App Address
	//------------------------------------------------------------------------------

	// Set
	me->addrVt = me->rsp.br_image_off + me->rsp.br_hdr->ih_hdr_size;
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
#endif


#endif	// #if defined(ALX_C_LIB)
