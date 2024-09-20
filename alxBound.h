/**
  ******************************************************************************
  * @file		alxBound.h
  * @brief		Auralix C Library - ALX Bound Module
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
#ifndef ALX_BOUND_H
#define ALX_BOUND_H

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
#define ALX_BOUND_MODULE "AlxBound.h"

// Assert //
#if defined(ALX_BOUND_ASSERT_BKPT_ENABLE)
	#define ALX_BOUND_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BOUND_MODULE, expr)
#elif defined(ALX_BOUND_ASSERT_TRACE_ENABLE)
	#define ALX_BOUND_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BOUND_MODULE, expr)
#elif defined(ALX_BOUND_ASSERT_RST_ENABLE)
	#define ALX_BOUND_ASSERT(expr) ALX_ASSERT_RST(ALX_BOUND_MODULE, expr)
#else
	#define ALX_BOUND_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_BOUND_TRACE_ENABLE)
	#define ALX_BOUND_TRACE(...) ALX_TRACE_WRN(ALX_BOUND_MODULE, __VA_ARGS__)
#else
	#define ALX_BOUND_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxBound_Uint8(uint8_t* val, uint8_t valMin, uint8_t valMax);
Alx_Status AlxBound_Uint16(uint16_t* val, uint16_t valMin, uint16_t valMax);
Alx_Status AlxBound_Uint32(uint32_t* val, uint32_t valMin, uint32_t valMax);
Alx_Status AlxBound_Uint64(uint64_t* val, uint64_t valMin, uint64_t valMax);
Alx_Status AlxBound_Int8(int8_t* val, int8_t valMin, int8_t valMax);
Alx_Status AlxBound_Int16(int16_t* val, int16_t valMin, int16_t valMax);
Alx_Status AlxBound_Int32(int32_t* val, int32_t valMin, int32_t valMax);
Alx_Status AlxBound_Int64(int64_t* val, int64_t valMin, int64_t valMax);
Alx_Status AlxBound_Float(float* val, float valMin, float valMax);
Alx_Status AlxBound_Double(double* val, double valMin, double valMax);
//Alx_Status AlxBound_Arr(char* val, uint32_t valLenMax);	// TV: TODO
Alx_Status AlxBound_Str(char* valBounded, char* val, uint32_t valMaxLenWithNullTerm);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BOUND_H
