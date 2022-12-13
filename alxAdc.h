/**
  ******************************************************************************
  * @file		alxAdc.h
  * @brief		Auralix C Library - ALX ADC Module
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

// AlxMcu //
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxAdc_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxAdc_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxAdc_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxAdc_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxAdc_McuLpc55S6x.h"

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
#if defined(_ALX_ADC_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADC_FILE, expr)
#elif defined(_ALX_ADC_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADC_FILE, expr)
#elif defined(_ALX_ADC_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_RST(ALX_ADC_FILE, expr)
#else
	#define ALX_ADC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_ADC_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_ADC_TRACE(...) ALX_TRACE_STD(ALX_ADC_FILE, __VA_ARGS__)
#else
	#define ALX_ADC_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdc_Init(AlxAdc* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdc_DeInit(AlxAdc* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ch
  * @return
  */
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ch
  * @return
  */
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch);	// MF: Optimized function

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_H
