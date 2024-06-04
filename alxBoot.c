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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBoot_Ctor
(
	AlxBoot* me
)
{
	// Parameters

	// Variables
	memset(&me->rsp, 0, sizeof(me->rsp));
	me->addrVt = 0;
	me->addrMsp = 0;
	me->addrJmp = 0;

	// Info
	me->wasCtorCalled = true;
	me->isAppPrepared = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxBoot_PrepareApp(AlxBoot* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_BOOT_ASSERT(me->wasCtorCalled == true);
	ALX_BOOT_ASSERT(me->isAppPrepared == false);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_BOOT_TRACE_FORMAT("\r\n");
	ALX_BOOT_TRACE_FORMAT("AlxBoot - PrepareApp started\r\n");
	ALX_BOOT_TRACE_FORMAT("\r\n");


	//------------------------------------------------------------------------------
	// Execute boot_go
	//------------------------------------------------------------------------------

	// Execute
	fih_ret status = boot_go(&me->rsp);
	if (status != FIH_SUCCESS)
	{
		ALX_BOOT_TRACE("Err");
		return Alx_Err;
	}

	// Check FLASH_DEVICE_ID
	if (me->rsp.br_flash_dev_id != ALX_MCU_BOOT_FLASH_DEVICE_ID)
	{
		ALX_BOOT_TRACE("Err");
		return Alx_Err;
	}

	// Trace
	ALX_BOOT_TRACE_FORMAT("\r\n");
	ALX_BOOT_TRACE_FORMAT("AlxBoot - boot_go OK\r\n");
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_magic = 0x%08lX\r\n", me->rsp.br_hdr->ih_magic);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_load_addr = 0x%08lX\r\n", me->rsp.br_hdr->ih_load_addr);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_hdr_size = 0x%04lX\r\n", me->rsp.br_hdr->ih_hdr_size);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_protect_tlv_size = 0x%04lX\r\n", me->rsp.br_hdr->ih_protect_tlv_size);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_img_size = %lu\r\n", me->rsp.br_hdr->ih_img_size);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_flags = 0x%08lX\r\n", me->rsp.br_hdr->ih_flags);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_ver.iv_major = %lu\r\n", me->rsp.br_hdr->ih_ver.iv_major);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_ver.iv_minor = %lu\r\n", me->rsp.br_hdr->ih_ver.iv_minor);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_ver.iv_revision = %lu\r\n", me->rsp.br_hdr->ih_ver.iv_revision);
	ALX_BOOT_TRACE_FORMAT("- br_hdr->ih_ver.iv_build_num = %lu\r\n", me->rsp.br_hdr->ih_ver.iv_build_num);
	ALX_BOOT_TRACE_FORMAT("- br_flash_dev_id = %lu\r\n", me->rsp.br_flash_dev_id);
	ALX_BOOT_TRACE_FORMAT("- br_image_off = 0x%08lX\r\n", me->rsp.br_image_off);


	//------------------------------------------------------------------------------
	// Set App Address
	//------------------------------------------------------------------------------

	// Set
	me->addrVt = me->rsp.br_image_off + me->rsp.br_hdr->ih_hdr_size;
	me->addrMsp = *(volatile uint32_t*)(me->addrVt);
	me->addrJmp = *(volatile uint32_t*)(me->addrVt + 4);

	// Trace
	ALX_BOOT_TRACE_FORMAT("AlxBoot - App address set\r\n");
	ALX_BOOT_TRACE_FORMAT("- addrVt = 0x%08lX\r\n", me->addrVt);
	ALX_BOOT_TRACE_FORMAT("- addrMsp = 0x%08lX\r\n", me->addrMsp);
	ALX_BOOT_TRACE_FORMAT("- addrJmp = 0x%08lX\r\n", me->addrJmp);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Set isInit
	me->isAppPrepared = true;

	// Return
	return Alx_Ok;
}
void AlxBoot_JumpToApp(AlxBoot* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_BOOT_ASSERT(me->wasCtorCalled == true);
	ALX_BOOT_ASSERT(me->isAppPrepared == true);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_BOOT_TRACE_FORMAT("AlxBoot - JumpToApp started\r\n");
	ALX_BOOT_TRACE_FORMAT("\r\n");


	//------------------------------------------------------------------------------
	// Jump
	//------------------------------------------------------------------------------
	void(*pf)(void) = (void(*)(void))(me->addrJmp);	// Cast jump address to function pointer
	__disable_irq();								// Disable global interrupts
	__set_MSP(me->addrMsp);							// Set app's stack pointer
	(*pf)();										// Dereference function pointer, jumps to addrJmp address, there is located app's Reset Handler address.
													// Compiler automatically handles, that the real jump address is odd number, so it automatically adds +1 to addrJmp,
													// this is needed for all ARM Cortex-M series processor (for THUMB instruction execution).
}


#endif	// #if defined(ALX_C_LIB)
