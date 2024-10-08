﻿/**
  ******************************************************************************
  * @file		alxWdt.h
  * @brief		Auralix C Library - ALX Watchdog Timer Module
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
#ifndef ALX_WDT_H
#define ALX_WDT_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
#include "alxWdt_McuStm32.h"

#elif defined(ALX_LPC55S6X)
#include "alxWdt_McuLpc55S6x.h"

#elif defined(ALX_LPC80X) || defined(ALX_LPC84X)
#include "alxWdt_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxWdt;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_WDT_FILE "alxWdt.h"

// Assert //
#if defined(ALX_WDT_ASSERT_BKPT_ENABLE)
	#define ALX_WDT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_WDT_FILE, expr)
#elif defined(ALX_WDT_ASSERT_TRACE_ENABLE)
	#define ALX_WDT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_WDT_FILE, expr)
#elif defined(ALX_WDT_ASSERT_RST_ENABLE)
	#define ALX_WDT_ASSERT(expr) ALX_ASSERT_RST(ALX_WDT_FILE, expr)
#else
	#define ALX_WDT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_WDT_TRACE_ENABLE)
	#define ALX_WDT_TRACE(...) ALX_TRACE_WRN(ALX_WDT_FILE, __VA_ARGS__)
#else
	#define ALX_WDT_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxWdt_Init(AlxWdt* me);
Alx_Status AlxWdt_Refresh(AlxWdt* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_WDT_H
