/**
  ******************************************************************************
  * @file		alxHys2.h
  * @brief		Auralix C Library - ALX Hysteresis 2 Module
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
#ifndef ALX_HYS2_H
#define ALX_HYS2_H

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
#define ALX_HYS2_FILE "alxHys2.h"

// Assert //
#if defined(_ALX_HYS2_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_HYS2_ASSERT(expr) ALX_ASSERT_BKPT(ALX_HYS2_FILE, expr)
#elif defined(_ALX_HYS2_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_HYS2_ASSERT(expr) ALX_ASSERT_TRACE(ALX_HYS2_FILE, expr)
#elif defined(_ALX_HYS2_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_HYS2_ASSERT(expr) ALX_ASSERT_RST(ALX_HYS2_FILE, expr)
#else
	#define ALX_HYS2_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_HYS2_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_HYS2_TRACE(...) ALX_TRACE_STD(ALX_HYS2_FILE, __VA_ARGS__)
#else
	#define ALX_HYS2_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxHys2_StTop = 0,
	AlxHys2_StMid = 1,
	AlxHys2_StBot = 2
} AlxHys2_St;

typedef struct
{
	// Parameters
	float topHigh;
	float topLow;
	float botHigh;
	float botLow;

	// SM
	AlxHys2_St st;

	// Info
	bool wasCtorCalled;
} AlxHys2;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxHys2_Ctor
(
	AlxHys2* me,
	float topHigh,
	float topLow,
	float botHigh,
	float botLow
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
AlxHys2_St AlxHys2_Process(AlxHys2* me, float in);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_HYS2_H
