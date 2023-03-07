/**
  ******************************************************************************
  * @file		alxLinFun.h
  * @brief		Auralix C Library - ALX Linear Function Module
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
#ifndef ALX_LIN_FUN_H
#define ALX_LIN_FUN_H

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
#define ALX_LIN_FUN_FILE "alxLinFun.h"

// Assert //
#if defined(_ALX_LIN_FUN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_LIN_FUN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LIN_FUN_FILE, expr)
#elif defined(_ALX_LIN_FUN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_LIN_FUN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LIN_FUN_FILE, expr)
#elif defined(_ALX_LIN_FUN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_LIN_FUN_ASSERT(expr) ALX_ASSERT_RST(ALX_LIN_FUN_FILE, expr)
#else
	#define ALX_LIN_FUN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_LIN_FUN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_LIN_FUN_TRACE(...) ALX_TRACE_STD(ALX_LIN_FUN_FILE, __VA_ARGS__)
#else
	#define ALX_LIN_FUN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float x1;
	float y1;
	float x2;
	float y2;
	float min;
	float max;
	bool isLimitOnX;

	// Variables
	float coefK;
	float coefN;

	// Info
	bool wasCtorCalled;
} AlxLinFun;

typedef struct
{
	// Parameters
	int32_t x1;
	int32_t y1;
	int32_t x2;
	int32_t y2;
	int32_t min;
	int32_t max;
	bool isLimitOnX;

	// Info
	bool wasCtorCalled;
} AlxLinFunInt;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLinFun_Ctor
(
	AlxLinFun *me,
	float x1,
	float point1_y,
	float point2_x,
	float point2_y,
	float min,
	float max,
	bool isLimitOnX
);
void AlxLinFunInt_Ctor
(
	AlxLinFunInt *me,
	int32_t x1,
	int32_t point1_y,
	int32_t point2_x,
	int32_t point2_y,
	int32_t min,
	int32_t max,
	bool isLimitOnX
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLinFun_GetY_WithStatus(AlxLinFun* me, float x, float* y);
Alx_Status AlxLinFun_GetX_WithStatus(AlxLinFun* me, float y, float* x);
float AlxLinFun_GetY(AlxLinFun* me, float x);
float AlxLinFun_GetX(AlxLinFun* me, float y);
Alx_Status AlxLinFunInt_GetY_WithStatus(AlxLinFunInt* me, int32_t x, int32_t* y);
int32_t AlxLinFunInt_GetY(AlxLinFunInt* me, int32_t x);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LIN_FUN_H
