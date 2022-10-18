/**
  ******************************************************************************
  * @file		alxOsDelay.h
  * @brief		Auralix C Library - ALX OS Delay Module
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
#ifndef ALX_OS_DELAY_H
#define ALX_OS_DELAY_H

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
#define ALX_OS_DELAY_FILE "alxOsDelay.h"

// Assert //
#if defined(_ALX_OS_DELAY_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_DELAY_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_DELAY_FILE, expr)
#elif defined(_ALX_OS_DELAY_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_DELAY_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_DELAY_FILE, expr)
#elif defined(_ALX_OS_DELAY_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_DELAY_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_DELAY_FILE, expr)
#else
	#define ALX_OS_DELAY_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_DELAY_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_DELAY_TRACE(...) ALX_TRACE_STD(ALX_OS_DELAY_FILE, __VA_ARGS__)
#else
	#define ALX_OS_DELAY_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxClk_Tick osTick;
	bool approxDisable;

	// Info
	bool wasCtorCalled;
} AlxOsDelay;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxOsDelay alxOsDelay;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTick
  * @param[in]		approxDisable
  */
void AlxOsDelay_Ctor
(
	AlxOsDelay* me,
	AlxClk_Tick osTick,
	bool approxDisable
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_us
  */
void AlxOsDelay_us(AlxOsDelay* me, uint64_t osDelay_us);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_ms
  */
void AlxOsDelay_ms(AlxOsDelay* me, uint64_t osDelay_ms);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_sec
  */
void AlxOsDelay_sec(AlxOsDelay* me, uint64_t osDelay_sec);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_min
  */
void AlxOsDelay_min(AlxOsDelay* me, uint64_t osDelay_min);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_hr
  */
void AlxOsDelay_hr(AlxOsDelay* me, uint64_t osDelay_hr);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_DELAY_H
