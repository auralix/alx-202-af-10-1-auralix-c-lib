/**
  ******************************************************************************
  * @file		alxIoPinIrq.h
  * @brief		Auralix C Library - ALX IO Pin IRQ Module
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
#ifndef ALX_IO_PIN_IRQ_H
#define ALX_IO_PIN_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
#include "alxIoPinIrq_McuStm32.h"

#elif defined(ALX_LPC55S6X)
#include "alxIoPinIrq_McuLpc55S6x.h"

#elif defined(ALX_LPC80X)
#include "alxIoPinIrq_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxIoPinIrq;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_IO_PIN_IRQ_FILE "alxIoPinIrq.h"

// Assert //
#if defined(_ALX_IO_PIN_IRQ_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_IO_PIN_IRQ_ASSERT(expr) ALX_ASSERT_BKPT(ALX_IO_PIN_IRQ_FILE, expr)
#elif defined(_ALX_IO_PIN_IRQ_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_IO_PIN_IRQ_ASSERT(expr) ALX_ASSERT_TRACE(ALX_IO_PIN_IRQ_FILE, expr)
#elif defined(_ALX_IO_PIN_IRQ_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_IO_PIN_IRQ_ASSERT(expr) ALX_ASSERT_RST(ALX_IO_PIN_IRQ_FILE, expr)
#else
	#define ALX_IO_PIN_IRQ_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_IO_PIN_IRQ_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_IO_PIN_IRQ_TRACE(...) ALX_TRACE_STD(ALX_IO_PIN_IRQ_FILE, __VA_ARGS__)
#else
	#define ALX_IO_PIN_IRQ_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
void AlxIoPinIrq_Init(AlxIoPinIrq* me);
void AlxIoPinIrq_DeInit(AlxIoPinIrq* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_IO_PIN_IRQ_H
