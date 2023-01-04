/**
  ******************************************************************************
  * @file		alxI2s.h
  * @brief		Auralix C Library - ALX I2S Module
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
#ifndef ALX_I2S_H
#define ALX_I2S_H

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
#if defined(ALX_STM32F4)
#include "alxI2s_McuStm32.h"

#else
typedef struct { bool dummy; } AlxI2s;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_I2S_FILE "alxI2s.h"

// Assert //
#if defined(_ALX_I2S_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_BKPT(ALX_I2S_FILE, expr)
#elif defined(_ALX_I2S_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_TRACE(ALX_I2S_FILE, expr)
#elif defined(_ALX_I2S_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_RST(ALX_I2S_FILE, expr)
#elif defined(_ALX_I2S_ASSERT_MOCK) || defined(_ALX_ASSERT_MOCK_ALL)
	#define ALX_I2S_ASSERT(expr) ALX_ASSERT_MOCK(ALX_I2S_FILE, expr)
#else
	#define ALX_I2S_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_I2S_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_I2S_TRACE(...) ALX_TRACE_STD(ALX_I2S_FILE, __VA_ARGS__)
#else
	#define ALX_I2S_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2s_Init(AlxI2s* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2s_DeInit(AlxI2s* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxI2s_Foreground_Handle(AlxI2s* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_I2S_H
