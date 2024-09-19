/**
  ******************************************************************************
  * @file		alxOsDelayUntil.h
  * @brief		Auralix C Library - ALX OS Delay Until Module
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
#ifndef ALX_OS_DELAY_UNTIL_H
#define ALX_OS_DELAY_UNTIL_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_DELAY_UNTIL_FILE "alxOsDelayUntil.h"

// Assert //
#if defined(ALX_OS_DELAY_UNTIL_ASSERT_BKPT_ENABLE)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_DELAY_UNTIL_FILE, expr)
#elif defined(ALX_OS_DELAY_UNTIL_ASSERT_TRACE_ENABLE)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_DELAY_UNTIL_FILE, expr)
#elif defined(ALX_OS_DELAY_UNTIL_ASSERT_RST_ENABLE)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_DELAY_UNTIL_FILE, expr)
#else
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_OS_DELAY_UNTIL_TRACE_ENABLE)
	#define ALX_OS_DELAY_UNTIL_TRACE(...) ALX_TRACE_WRN(ALX_OS_DELAY_UNTIL_FILE, __VA_ARGS__)
#else
	#define ALX_OS_DELAY_UNTIL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxClk_Tick osTick;

	// Variables
	#if defined(ALX_FREE_RTOS)
	TickType_t xLastWakeTime;
	#endif

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxOsDelayUntil;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsDelayUntil_Ctor
(
	AlxOsDelayUntil* me,
	AlxClk_Tick osTick
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsDelayUntil_Init(AlxOsDelayUntil* me);
void AlxOsDelayUntil_us(AlxOsDelayUntil* me, uint64_t osDelayUntilIncrement_us);
void AlxOsDelayUntil_ms(AlxOsDelayUntil* me, uint64_t osTimeIncrement_ms);
void AlxOsDelayUntil_sec(AlxOsDelayUntil* me, uint64_t osTimeIncrement_sec);
void AlxOsDelayUntil_min(AlxOsDelayUntil* me, uint64_t osTimeIncrement_min);
void AlxOsDelayUntil_hr(AlxOsDelayUntil* me, uint64_t osTimeIncrement_hr);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_DELAY_UNTIL_H
