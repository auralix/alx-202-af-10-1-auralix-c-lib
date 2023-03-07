/**
  ******************************************************************************
  * @file		alxHys1.h
  * @brief		Auralix C Library - ALX Hysteresis 1 Module
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
#ifndef ALX_HYS1_H
#define ALX_HYS1_H

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
#define ALX_HYS1_FILE "alxHys1.h"

// Assert //
#if defined(_ALX_HYS1_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_HYS1_ASSERT(expr) ALX_ASSERT_BKPT(ALX_HYS1_FILE, expr)
#elif defined(_ALX_HYS1_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_HYS1_ASSERT(expr) ALX_ASSERT_TRACE(ALX_HYS1_FILE, expr)
#elif defined(_ALX_HYS1_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_HYS1_ASSERT(expr) ALX_ASSERT_RST(ALX_HYS1_FILE, expr)
#else
	#define ALX_HYS1_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_HYS1_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_HYS1_TRACE(...) ALX_TRACE_STD(ALX_HYS1_FILE, __VA_ARGS__)
#else
	#define ALX_HYS1_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxHys1_StHigh = 0,
	AlxHys1_StLow = 1
} AlxHys1_St;

typedef struct
{
	// Parameters
	float high; // Threshold for transition to Primary state
	float low; // Threshold for transition to Secondary state

	// SM
	AlxHys1_St st;

	// Info
	bool wasCtorCalled;
} AlxHys1;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys1_Ctor
(
	AlxHys1* me,
	float high,
	float low
);


//******************************************************************************
// Functions
//******************************************************************************
AlxHys1_St AlxHys1_Process(AlxHys1* me, float in);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_HYS1_H
