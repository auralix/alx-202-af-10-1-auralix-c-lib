/**
  ******************************************************************************
  * @file		alxVeml6040.h
  * @brief		Auralix C Library - ALX Light Sensor VEML604 Module
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
#ifndef ALX_VEML6040_H
#define ALX_VEML6040_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_VEML6040_FILE "alxVeml6040.h"

// Assert //
#if defined(_ALX_VEML6040_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_VEML6040_ASSERT(expr) ALX_ASSERT_BKPT(ALX_VEML6040_FILE, expr)
#elif defined(_ALX_VEML6040_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_VEML6040_ASSERT(expr) ALX_ASSERT_TRACE(ALX_VEML6040_FILE, expr)
#elif defined(_ALX_VEML6040_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_VEML6040_ASSERT(expr) ALX_ASSERT_RST(ALX_VEML6040_FILE, expr)
#else
	#define ALX_VEML6040_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_VEML6040_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_VEML6040_TRACE(...) ALX_TRACE_STD(ALX_VEML6040_FILE, __VA_ARGS__)
	#define ALX_VEML6040_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_VEML6040_TRACE(...) do{} while (false)
	#define ALX_VEML6040_TRACE_FORMAT(...) do{} while (false)
#endif


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

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxVeml6040;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxVeml6040_Ctor
(
	AlxVeml6040* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxVeml6040_Init(AlxVeml6040* me);
Alx_Status AlxVeml6040_DeInit(AlxVeml6040* me);
Alx_Status AlxVeml6040_GetR_raw(AlxVeml6040* me, uint16_t* R_raw);
Alx_Status AlxVeml6040_GetG_raw(AlxVeml6040* me, uint16_t* G_raw);
Alx_Status AlxVeml6040_GetB_raw(AlxVeml6040* me, uint16_t* B_raw);
Alx_Status AlxVeml6040_GetW_raw(AlxVeml6040* me, uint16_t* W_raw);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_VEML6040_H
