/**
  ******************************************************************************
  * @file		alxClk.h
  * @brief		Auralix C Library - ALX Clock Module
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
#ifndef ALX_CLK_H
#define ALX_CLK_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

// AlxMcu //
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxClk_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxClk_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxClk_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxClk_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxClk_McuLpc55S6x.h"

#else
typedef struct { bool dummy; } AlxClk;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CLK_FILE "alxClk.h"

// Assert //
#if defined(_ALX_CLK_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_CLK_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CLK_FILE, expr)
#elif defined(_ALX_CLK_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_CLK_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CLK_FILE, expr)
#elif defined(_ALX_CLK_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_CLK_ASSERT(expr) ALX_ASSERT_RST(ALX_CLK_FILE, expr)
#else
	#define ALX_CLK_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_CLK_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_CLK_TRACE(...) ALX_TRACE_STD(ALX_CLK_FILE, __VA_ARGS__)
#else
	#define ALX_CLK_TRACE(...) do{} while (false)
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
Alx_Status AlxClk_Init(AlxClk* me);

	/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxClk_DeInit(AlxClk* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		clk
  * @return
  */
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk);

/**
  * @brief
  * @param[in,out] me
  */
void AlxClk_Irq_Handle(AlxClk* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CLK_H
