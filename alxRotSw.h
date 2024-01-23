﻿/**
  ******************************************************************************
  * @file		alxRotSw.h
  * @brief		Auralix C Library - ALX Rotary Switch Module
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
#ifndef ALX_ROT_SW_H
#define ALX_ROT_SW_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
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
#define ALX_ROT_SW_FILE "alxRotSw.h"

// Assert //
#if defined(ALX_ROT_SW_ASSERT_BKPT_ENABLE)
	#define ALX_ROT_SW_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ROT_SW_FILE, expr)
#elif defined(ALX_ROT_SW_ASSERT_TRACE_ENABLE)
	#define ALX_ROT_SW_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ROT_SW_FILE, expr)
#elif defined(ALX_ROT_SW_ASSERT_RST_ENABLE)
	#define ALX_ROT_SW_ASSERT(expr) ALX_ASSERT_RST(ALX_ROT_SW_FILE, expr)
#else
	#define ALX_ROT_SW_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_ROT_SW_TRACE_ENABLE)
	#define ALX_ROT_SW_TRACE(...) ALX_TRACE_STD(ALX_ROT_SW_FILE, __VA_ARGS__)
	#define ALX_ROT_SW_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_ROT_SW_TRACE(...) do{} while (false)
	#define ALX_ROT_SW_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxRotSw_CodeType_Real,
	AlxRotSw_CodeType_Complement,
	AlxRotSw_CodeType_Gray
} AlxRotSw_CodeType;

typedef struct
{
	// Defines
	#define ALX_ROT_SW_IO_PIN_VAL_ARR_LEN 8

	// Parameters
	AlxIoPin** ioPinArr;
	uint8_t ioPinArrLen;
	AlxRotSw_CodeType codeType;

	// Variables
	bool ioPinValArr[ALX_ROT_SW_IO_PIN_VAL_ARR_LEN];
	uint32_t code;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxRotSw;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxRotSw_Ctor
(
	AlxRotSw* me,
	AlxIoPin** ioPinArr,
	uint8_t ioPinArrLen,
	AlxRotSw_CodeType codeType
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxRotSw_Init(AlxRotSw* me);
void AlxRotSw_DeInit(AlxRotSw* me);
uint32_t AlxRotSw_GetCode(AlxRotSw* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ROT_SW_H
