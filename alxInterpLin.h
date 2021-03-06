/**
  ******************************************************************************
  * @file		alxInterpLin.h
  * @brief		Auralix C Library - ALX Linear Interpolation Module
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

#ifndef ALX_INTERP_LIN_H
#define ALX_INTERP_LIN_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_INTERP_LIN_FILE "alxInterpLin"

// Assert //
#if defined(_ALX_INTERP_LIN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_INTERP_LIN_FILE, expr)
#elif defined(_ALX_INTERP_LIN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_INTERP_LIN_FILE, expr)
#elif defined(_ALX_INTERP_LIN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_RST(ALX_INTERP_LIN_FILE, expr)
#else
	#define ALX_INTERP_LIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_INTERP_LIN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_INTERP_LIN_TRACE(...) ALX_TRACE_STD(ALX_INTERP_LIN_FILE, __VA_ARGS__)
#else
	#define ALX_INTERP_LIN_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_INTERP_LIN_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_INTERP_LIN_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_INTERP_LIN_DBG_PIN(...) do{} while (false)
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
	
	// Objects - Internal
	
	// Variables;
	
	// Info
	bool wasCtorCalled;
	
} AlxInterpLin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxInterpLin_Ctor
(
	AlxInterpLin *me,
	float *xPointArr,			// Points to 0th element of the X arr.
	float *yPointArr,			// Points to 0th element of the Y arr.
	uint32_t numOfArrPoints,	// Length of the arrays
	bool isisXpointArrRising	// True if X[n+1] > X[n]
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxInterpLin_GetY_WithStatus(AlxInterpLin* me, float x, float* y);
float AlxInterpLin_GetY(AlxInterpLin* me, float x); 


#ifdef __cplusplus
}
#endif

#endif // ALX_INTERP_LIN_H
