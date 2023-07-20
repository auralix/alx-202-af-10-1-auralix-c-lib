/**
  ******************************************************************************
  * @file		alxAvg.h
  * @brief		Auralix C Library - ALX Average Module
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
#ifndef ALX_AVG_H
#define ALX_AVG_H

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
#if defined(ALX_C_LIB) && defined(ALX_CMSIS_DSP)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_AVG_FILE "alxAvg.h"

// Assert //
#if defined(_ALX_AVG_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_AVG_ASSERT(expr) ALX_ASSERT_BKPT(ALX_AVG_FILE, expr)
#elif defined(_ALX_AVG_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_AVG_ASSERT(expr) ALX_ASSERT_TRACE(ALX_AVG_FILE, expr)
#elif defined(_ALX_AVG_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_AVG_ASSERT(expr) ALX_ASSERT_RST(ALX_AVG_FILE, expr)
#else
	#define ALX_AVG_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_AVG_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_AVG_TRACE(...) ALX_TRACE_STD(ALX_AVG_FILE, __VA_ARGS__)
#else
	#define ALX_AVG_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float* buff;
	uint32_t buffLen;
	uint32_t shiftThreshold;

	// Variables
	uint32_t buffIndex;
	uint32_t shiftCount;
	float avg;

	// Info
	bool wasCtorCalled;
} AlxAvg;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAvg_Ctor
(
	AlxAvg* me,
	float* buff,
	uint32_t buffLen,
	uint32_t shiftThreshold
);


//******************************************************************************
// Functions
//******************************************************************************
float AlxAvg_Process(AlxAvg* me, float in);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_CMSIS_DSP)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_AVG_H
