/**
  ******************************************************************************
  * @file		alxLp586x.h
  * @brief		Auralix C Library - ALX LED Driver LP586x Module
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
#ifndef ALX_LP586x_H
#define ALX_LP586x_H

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
#include "alxI2c.h"
#include "alxDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LP586x_FILE "alxLp586x.h"

// Assert //
#if defined(ALX_LP586x_ASSERT_BKPT_ENABLE)
	#define ALX_LP586x_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LP586x_FILE, expr)
#elif defined(ALX_LP586x_ASSERT_TRACE_ENABLE)
	#define ALX_LP586x_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LP586x_FILE, expr)
#elif defined(ALX_LP586x_ASSERT_RST_ENABLE)
	#define ALX_LP586x_ASSERT(expr) ALX_ASSERT_RST(ALX_LP586x_FILE, expr)
#else
	#define ALX_LP586x_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_LP586x_TRACE_ENABLE)
	#define ALX_LP586x_TRACE_WRN(...) ALX_TRACE_WRN(ALX_LP586x_FILE, __VA_ARGS__)
	#define ALX_LP586x_TRACE_ERR(...) ALX_TRACE_ERR(ALX_LP586x_FILE, __VA_ARGS__)
#else
	#define ALX_LP586x_TRACE_WRN(...) do{} while (false)
	#define ALX_LP586x_TRACE_ERR(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//******************************************************************************
// Register Structures
//******************************************************************************


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
} AlxLp586x_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxLp586x_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInitPeriph;
	bool isInit;
} AlxLp586x;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLp586x_Ctor
(
	AlxLp586x* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLp586x_InitPeriph(AlxLp586x* me);
Alx_Status AlxLp586x_DeInitPeriph(AlxLp586x* me);
Alx_Status AlxLp586x_Init(AlxLp586x* me);
Alx_Status AlxLp586x_DeInit(AlxLp586x* me);
Alx_Status AlxLp586x_Handle(AlxLp586x* me);
void AlxLp586x_Led_Write(AlxLp586x* me, uint8_t ledNum, bool val);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LP586x_H
