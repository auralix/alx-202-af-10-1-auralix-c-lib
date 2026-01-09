/**
  ******************************************************************************
  * @file		alxBoot.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_BOOT_H
#define ALX_BOOT_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFs.h"
#include "alxId.h"
#include "alxUsb.h"
#include "alxTimSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_BOOT_FILE "alxBoot.h"

// Assert //
#if defined(ALX_BOOT_ASSERT_BKPT_ENABLE)
	#define ALX_BOOT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BOOT_FILE, expr)
#elif defined(ALX_BOOT_ASSERT_TRACE_ENABLE)
	#define ALX_BOOT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BOOT_FILE, expr)
#elif defined(ALX_BOOT_ASSERT_RST_ENABLE)
	#define ALX_BOOT_ASSERT(expr) ALX_ASSERT_RST(ALX_BOOT_FILE, expr)
#else
	#define ALX_BOOT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_BOOT_TRACE_ENABLE)
	#define ALX_BOOT_TRACE_ERR(...) ALX_TRACE_ERR(ALX_BOOT_FILE, __VA_ARGS__)
	#define ALX_BOOT_TRACE_WRN(...) ALX_TRACE_WRN(ALX_BOOT_FILE, __VA_ARGS__)
	#define ALX_BOOT_TRACE_INF(...) ALX_TRACE_INF(ALX_BOOT_FILE, __VA_ARGS__)
#else
	#define ALX_BOOT_TRACE_ERR(...) do{} while (false)
	#define ALX_BOOT_TRACE_WRN(...) do{} while (false)
	#define ALX_BOOT_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxBoot_App_Status_FwCandDiscovery_FindUsbStart,
	AlxBoot_App_Status_FwCandDiscovery_UsbNotFound,
	AlxBoot_App_Status_FwCandDiscovery_FindUsbDone,

	AlxBoot_App_Status_FwCandDiscovery_FindFwCandStart,
	AlxBoot_App_Status_FwCandDiscovery_FwCandNotFound,
	AlxBoot_App_Status_FwCandDiscovery_FindFwCandDone,

	AlxBoot_App_Status_FwCandDiscovery_CheckFwCandVerStart,
	AlxBoot_App_Status_FwCandDiscovery_FwCandSameVer,
	AlxBoot_App_Status_FwCandDiscovery_CheckFwCandVerDone,

	AlxBoot_App_Status_FwCandStaging_EraseFlashStart,
	AlxBoot_App_Status_FwCandStaging_EraseFlashDone,

	AlxBoot_App_Status_FwCandStaging_WriteFlashStart,
	AlxBoot_App_Status_FwCandStaging_WriteFlashDone,

	AlxBoot_App_Status_Err
} AlxBoot_App_Status;

typedef struct
{
	// Defines
	#define ALX_BOOT_BUFF_LEN 512

	// Parameters
	AlxFs* alxFs;
	AlxId* alxId;
	AlxUsb* alxUsb;
	uint16_t usbReadyTimeout_ms;

	// Variables
	uint8_t buff[ALX_BOOT_BUFF_LEN];
	#if defined(ALX_BOOT_A)
	const struct flash_area* fa;
	#endif
	uint32_t fwCandStaging_LenWritten_bytes;
	uint8_t fwCandStaging_ProgressStep10_pct;
	#if defined(ALX_BOOT_B)
	struct boot_rsp rsp;
	#endif
	uint32_t addrVt;
	uint32_t addrMsp;
	uint32_t addrJmp;

	// Info
	bool wasCtorCalled;
	bool isReadyToJumpToApp;
} AlxBoot;


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
);


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
// App
//------------------------------------------------------------------------------
void AlxBoot_App_Usb_Update(AlxBoot* me);


//------------------------------------------------------------------------------
// Boot
//------------------------------------------------------------------------------
Alx_Status AlxBoot_Boot_Run(AlxBoot* me);
void AlxBoot_Boot_JumpToApp(AlxBoot* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BOOT_H
