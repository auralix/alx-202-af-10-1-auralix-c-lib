/**
  ******************************************************************************
  * @file		alxIna228.h
  * @brief		Auralix C Library - ALX Current Monitor INA228 Module
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
#ifndef ALX_INA228_H
#define ALX_INA228_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_INA228_FILE "alxIna228.h"

// Assert //
#if defined(_ALX_INA228_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_INA228_ASSERT(expr) ALX_ASSERT_BKPT(ALX_INA228_FILE, expr)
#elif defined(_ALX_INA228_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_INA228_ASSERT(expr) ALX_ASSERT_TRACE(ALX_INA228_FILE, expr)
#elif defined(_ALX_INA228_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_INA228_ASSERT(expr) ALX_ASSERT_RST(ALX_INA228_FILE, expr)
#else
	#define ALX_INA228_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_INA228_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_INA228_TRACE(...) ALX_TRACE_STD(ALX_INA228_FILE, __VA_ARGS__)
	#define ALX_INA228_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_INA228_TRACE(...) do{} while (false)
	#define ALX_INA228_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint32_t* param;

	// Variables
	uint32_t var;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxIna228;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIna228_Ctor
(
	AlxIna228* me,
	uint32_t* param
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxIna228_InitPeriph(AlxIna228* me);
Alx_Status AlxIna228_DeInitPeriph(AlxIna228* me);
Alx_Status AlxIna228_Init(AlxIna228* me);
Alx_Status AlxIna228_DeInit(AlxIna228* me);
Alx_Status AlxIna228_GetShuntVoltage_V(AlxIna228* me, float* voltage_V);
Alx_Status AlxIna228_GetBusVoltage_V(AlxIna228* me, float* voltage_V);
Alx_Status AlxIna228_GetTemp_degC(AlxIna228* me, float* temp_degC);
Alx_Status AlxIna228_GetCurrent_A(AlxIna228* me, float* current_A);
Alx_Status AlxIna228_GetPower_W(AlxIna228* me, float* power_W);
Alx_Status AlxIna228_GetEnergy_J(AlxIna228* me, float* energy_J);
Alx_Status AlxIna228_GetCharge_C(AlxIna228* me, float* charge_C);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_INA228_H
