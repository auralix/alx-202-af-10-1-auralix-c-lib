/**
  ******************************************************************************
  * @file		alxMux.h
  * @brief		Auralix C Library - ALX Analog Multiplexer Module
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
#ifndef ALX_MUX_H
#define ALX_MUX_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************rp
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MUX_FILE "alxMux.h"

// Assert //
#if defined(ALX_MUX_ASSERT_BKPT_ENABLE)
	#define ALX_MUX_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MUX_FILE, expr)
#elif defined(ALX_MUX_ASSERT_TRACE_ENABLE)
	#define ALX_MUX_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MUX_FILE, expr)
#elif defined(ALX_MUX_ASSERT_RST_ENABLE)
	#define ALX_MUX_ASSERT(expr) ALX_ASSERT_RST(ALX_MUX_FILE, expr)
#else
	#define ALX_MUX_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_MUX_TRACE_ENABLE)
	#define ALX_MUX_TRACE_WRN(...) ALX_TRACE_WRN(ALX_MUX_FILE, __VA_ARGS__)
#else
	#define ALX_MUX_TRACE_WRN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxMux_CodeType_Real,
	AlxMux_CodeType_Complement,
	AlxMux_CodeType_Gray
} AlxMux_CodeType;

typedef struct
{
	// Defines
	#define ALX_MUX_IO_PIN_VAL_ARR_LEN 8

	// Parameters
	AlxIoPin** ioPinArr;
	uint8_t ioPinArrLen;
	AlxMux_CodeType codeType;

	// Variables
	bool ioPinValArr[ALX_MUX_IO_PIN_VAL_ARR_LEN];
	uint32_t code;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxMux;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMux_Ctor
(
	AlxMux* me,
	AlxIoPin** ioPinArr,
	uint8_t ioPinArrLen,
	AlxMux_CodeType codeType
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxMux_Init(AlxMux* me);
void AlxMux_DeInit(AlxMux* me);
uint32_t AlxMux_GetCode(AlxMux* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MUX_H
