/**
  ******************************************************************************
  * @file		alxIoPin.h
  * @brief		Auralix C Library - ALX IO Pin Module
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
#ifndef ALX_IO_PIN_H
#define ALX_IO_PIN_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxDelay.h"

#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
#include "alxIoPin_McuStm32.h"

#elif defined(ALX_LPC17XX)
#include "alxIoPin_McuLpc17xx.h"

#elif defined(ALX_LPC55S6X)
#include "alxIoPin_McuLpc55S6x.h"

#elif defined(ALX_LPC80X) || defined(ALX_LPC84X)
#include "alxIoPin_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxIoPin;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_IO_PIN_FILE "alxIoPin.h"

// Assert //
#if defined(ALX_IO_PIN_ASSERT_BKPT_ENABLE)
	#define ALX_IO_PIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_IO_PIN_FILE, expr)
#elif defined(ALX_IO_PIN_ASSERT_TRACE_ENABLE)
	#define ALX_IO_PIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_IO_PIN_FILE, expr)
#elif defined(ALX_IO_PIN_ASSERT_RST_ENABLE)
	#define ALX_IO_PIN_ASSERT(expr) ALX_ASSERT_RST(ALX_IO_PIN_FILE, expr)
#else
	#define ALX_IO_PIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_IO_PIN_TRACE_ENABLE)
	#define ALX_IO_PIN_TRACE(...) ALX_TRACE_STD(ALX_IO_PIN_FILE, __VA_ARGS__)
#else
	#define ALX_IO_PIN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxIoPin_TriState_HiZ = 0,
	AlxIoPin_TriState_Hi = 1,
	AlxIoPin_TriState_Lo = 2,
	AlxIoPin_TriState_Undefined = 3
} AlxIoPin_TriState;


//******************************************************************************
// Functions
//******************************************************************************
void AlxIoPin_Init(AlxIoPin* me);
void AlxIoPin_DeInit(AlxIoPin* me);
bool AlxIoPin_Read(AlxIoPin* me);
void AlxIoPin_Write(AlxIoPin* me, bool val);
void AlxIoPin_Set(AlxIoPin* me);
void AlxIoPin_Reset(AlxIoPin* me);
void AlxIoPin_Toggle(AlxIoPin* me);
AlxIoPin_TriState AlxIoPin_Read_TriState(AlxIoPin* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_IO_PIN_H
