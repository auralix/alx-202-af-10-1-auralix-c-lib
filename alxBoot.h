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
#include "alxId.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_MCU_BOOT)


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
	#define ALX_BOOT_TRACE(...) ALX_TRACE_STD(ALX_BOOT_FILE, __VA_ARGS__)
	#define ALX_BOOT_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_BOOT_TRACE(...) do{} while (false)
	#define ALX_BOOT_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	struct boot_rsp rsp;
	uint32_t addrVt;
	uint32_t addrMsp;
	uint32_t addrJmp;

	// Info
	bool wasCtorCalled;
	bool isPrepared;
} AlxBoot;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBoot_Ctor
(
	AlxBoot* me
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxBoot_Prepare(AlxBoot* me);
void AlxBoot_Jump(AlxBoot* me);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_MCU_BOOT)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BOOT_H
