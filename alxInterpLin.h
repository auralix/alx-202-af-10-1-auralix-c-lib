/**
  ******************************************************************************
  * @file		alxInterpLin.h
  * @brief		Auralix C Library - ALX Linear Interpolation Module
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
#ifndef ALX_INTERP_LIN_H
#define ALX_INTERP_LIN_H

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
#define ALX_INTERP_LIN_FILE "alxInterpLin.h"

// Assert //
#if defined(ALX_INTERP_LIN_ASSERT_BKPT_ENABLE)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_INTERP_LIN_FILE, expr)
#elif defined(ALX_INTERP_LIN_ASSERT_TRACE_ENABLE)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_INTERP_LIN_FILE, expr)
#elif defined(ALX_INTERP_LIN_ASSERT_RST_ENABLE)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_RST(ALX_INTERP_LIN_FILE, expr)
#else
	#define ALX_INTERP_LIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_INTERP_LIN_TRACE_ENABLE)
	#define ALX_INTERP_LIN_TRACE(...) ALX_TRACE_WRN(ALX_INTERP_LIN_FILE, __VA_ARGS__)
#else
	#define ALX_INTERP_LIN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float* xPointArr;
	float* yPointArr;
	uint32_t numOfArrPoints;
	bool isXpointArrRising;

	// Info
	bool wasCtorCalled;
} AlxInterpLin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxInterpLin_Ctor
(
	AlxInterpLin* me,
	float* xPointArr,			// Points to 0th element of the X arr.
	float* yPointArr,			// Points to 0th element of the Y arr.
	uint32_t numOfArrPoints,	// Length of the arrays
	bool isisXpointArrRising	// True if X[n+1] > X[n]
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxInterpLin_GetY_WithStatus(AlxInterpLin* me, float x, float* y);
float AlxInterpLin_GetY(AlxInterpLin* me, float x);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_INTERP_LIN_H
