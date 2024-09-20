/**
  ******************************************************************************
  * @file		alxMath.h
  * @brief		Auralix C Library - ALX Mathematics Module
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
#ifndef ALX_MATH_H
#define ALX_MATH_H

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
#define ALX_MATH_MODULE "AlxMath.h"

// Assert //
#if defined(ALX_MATH_ASSERT_BKPT_ENABLE)
	#define ALX_MATH_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MATH_MODULE, expr)
#elif defined(ALX_MATH_ASSERT_TRACE_ENABLE)
	#define ALX_MATH_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MATH_MODULE, expr)
#elif defined(ALX_MATH_ASSERT_RST_ENABLE)
	#define ALX_MATH_ASSERT(expr) ALX_ASSERT_RST(ALX_MATH_MODULE, expr)
#else
	#define ALX_MATH_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_MATH_TRACE_ENABLE)
	#define ALX_MATH_TRACE(...) ALX_TRACE_WRN(ALX_MATH_MODULE, __VA_ARGS__)
#else
	#define ALX_MATH_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	uint32_t count;
	uint64_t sum;
	uint32_t avg;
	uint32_t min;
	uint32_t max;
} AlxMath_Data;

typedef struct
{
	// Variables
	AlxMath_Data data;

	// Info
	bool wasCtorCalled;
} AlxMath;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMath_Ctor(AlxMath* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxMath_Data AlxMath_Process(AlxMath* me, uint32_t in);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MATH_H
