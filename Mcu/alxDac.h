/**
  ******************************************************************************
  * @file		AlxDac.h
  * @brief		Auralix C Library - ALX DAC Module
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
#ifndef ALX_DAC_H
#define ALX_DAC_H

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

#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
#include "alxDac_McuStm32.h"

#else
typedef struct { bool dummy; } AlxDac;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_DAC_FILE "alxDac.h"

// Assert //
#if defined(ALX_DAC_ASSERT_BKPT_ENABLE)
	#define ALX_DAC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_DAC_FILE, expr)
#elif defined(ALX_DAC_ASSERT_TRACE_ENABLE)
	#define ALX_DAC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_DAC_FILE, expr)
#elif defined(ALX_DAC_ASSERT_RST_ENABLE)
	#define ALX_DAC_ASSERT(expr) ALX_ASSERT_RST(ALX_DAC_FILE, expr)
#else
	#define ALX_DAC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_DAC_TRACE_ENABLE)
	#define ALX_DAC_TRACE(...) ALX_TRACE_STD(ALX_DAC_FILE, __VA_ARGS__)
#else
	#define ALX_DAC_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxDac_Init(AlxDac* me);
Alx_Status AlxDac_Init_CalibrateVref(AlxDac* me, float vref_V);
Alx_Status AlxDac_DeInit(AlxDac* me);
Alx_Status AlxDac_SetVoltage_V(AlxDac* me, Alx_Ch ch, float voltage_V);
Alx_Status AlxDac_SetVoltage_V_CalibrateVref(AlxDac* me, Alx_Ch ch, float voltage_V, float vref_V);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_DAC_H
