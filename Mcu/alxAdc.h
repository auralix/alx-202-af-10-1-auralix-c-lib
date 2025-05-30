﻿/**
  ******************************************************************************
  * @file		alxAdc.h
  * @brief		Auralix C Library - ALX ADC Module
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
#ifndef ALX_ADC_H
#define ALX_ADC_H

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

#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
#include "alxAdc_McuStm32.h"

#elif defined(ALX_LPC17XX)
#include "alxAdc_McuLpc17xx.h"

#elif defined(ALX_LPC55S6X)
#include "alxAdc_McuLpc55S6x.h"

#elif defined(ALX_LPC80X) || defined(ALX_LPC84X)
#include "alxAdc_McuLpc80x.h"

#elif defined(ALX_SAM)
#include "alxAdc_McuSam.h"

#elif defined(ALX_ZEPHYR)
#include "alxAdc_McuZephyr.h"

#else
typedef struct { bool dummy; } AlxAdc;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADC_FILE "alxAdc.h"

// Assert //
#if defined(ALX_ADC_ASSERT_BKPT_ENABLE)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADC_FILE, expr)
#elif defined(ALX_ADC_ASSERT_TRACE_ENABLE)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADC_FILE, expr)
#elif defined(ALX_ADC_ASSERT_RST_ENABLE)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_RST(ALX_ADC_FILE, expr)
#else
	#define ALX_ADC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_ADC_TRACE_ENABLE)
	#define ALX_ADC_TRACE_ERR(...) ALX_TRACE_ERR(ALX_ADC_FILE, __VA_ARGS__)
	#define ALX_ADC_TRACE_WRN(...) ALX_TRACE_WRN(ALX_ADC_FILE, __VA_ARGS__)
	#define ALX_ADC_TRACE_DBG(...) ALX_TRACE_DBG(ALX_ADC_FILE, __VA_ARGS__)
	#define ALX_ADC_TRACE_VRB(...) ALX_TRACE_VRB(ALX_ADC_FILE, __VA_ARGS__)
#else
	#define ALX_ADC_TRACE_ERR(...) do{} while (false)
	#define ALX_ADC_TRACE_WRN(...) do{} while (false)
	#define ALX_ADC_TRACE_DBG(...) do{} while (false)
	#define ALX_ADC_TRACE_VRB(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAdc_Init(AlxAdc* me);
Alx_Status AlxAdc_DeInit(AlxAdc* me);
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch);
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch);
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_H
