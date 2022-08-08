/**
  ******************************************************************************
  * @file		alxOsDelayUntil.h
  * @brief		Auralix C Library - ALX OS Delay Until Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
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
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_DELAY_UNTIL_FILE "alxOsDelayUntil.h"

// Assert //
#if defined(_ALX_OS_DELAY_UNTIL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_DELAY_UNTIL_FILE, expr)
#elif defined(_ALX_OS_DELAY_UNTIL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_DELAY_UNTIL_FILE, expr)
#elif defined(_ALX_OS_DELAY_UNTIL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_DELAY_UNTIL_FILE, expr)
#else
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_DELAY_UNTIL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_DELAY_UNTIL_TRACE(...) ALX_TRACE_STD(ALX_OS_DELAY_UNTIL_FILE, __VA_ARGS__)
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

/**
  * @brief
  * @param[in,out] me
  * @param[in] osTick
  */
void AlxOsDelayUntil_Ctor
(
	AlxOsDelayUntil* me,
	AlxClk_Tick osTick
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsDelayUntil_Init(AlxOsDelayUntil* me);

/**
  * @brief
  * @param[in] me
  * @param[in] osDelayUntilIncrement_us
  */
void AlxOsDelayUntil_us(AlxOsDelayUntil* me, uint64_t osDelayUntilIncrement_us);

/**
  * @brief
  * @param[in] me
  * @param[in] osTimeIncrement_ms
  */
void AlxOsDelayUntil_ms(AlxOsDelayUntil* me, uint64_t osTimeIncrement_ms);

/**
  * @brief
  * @param[in] me
  * @param[in] osTimeIncrement_sec
  */
void AlxOsDelayUntil_sec(AlxOsDelayUntil* me, uint64_t osTimeIncrement_sec);

/**
  * @brief
  * @param[in] me
  * @param[in] osTimeIncrement_min
  */
void AlxOsDelayUntil_min(AlxOsDelayUntil* me, uint64_t osTimeIncrement_min);

/**
  * @brief
  * @param[in] me
  * @param[in] osTimeIncrement_hr
  */
void AlxOsDelayUntil_hr(AlxOsDelayUntil* me, uint64_t osTimeIncrement_hr);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_DELAY_UNTIL_H
