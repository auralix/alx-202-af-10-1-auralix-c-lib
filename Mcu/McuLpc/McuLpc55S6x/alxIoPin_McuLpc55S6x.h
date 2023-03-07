/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX IO Pin MCU LPC55S6X Module
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
#ifndef ALX_IO_PIN_MCU_LPC55S6X_H
#define ALX_IO_PIN_MCU_LPC55S6X_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxIoPin_Func_0_GPIO	= IOCON_FUNC0,
	AlxIoPin_Func_1			= IOCON_FUNC1,
	AlxIoPin_Func_2			= IOCON_FUNC2,
	AlxIoPin_Func_3			= IOCON_FUNC3,
	AlxIoPin_Func_4			= IOCON_FUNC4,
	AlxIoPin_Func_5			= IOCON_FUNC5,
	AlxIoPin_Func_6			= IOCON_FUNC6,
	AlxIoPin_Func_7			= IOCON_FUNC7,
	AlxIoPin_Func_8			= IOCON_FUNC8,
	AlxIoPin_Func_9			= IOCON_FUNC9,
	AlxIoPin_Func_10		= IOCON_FUNC10,
	AlxIoPin_Func_11		= IOCON_FUNC11,

	AlxIoPin_Func_IRQ		= IOCON_FUNC0	// MF: IRQ has to be configured as GPIO
} AlxIoPin_Iocon_Func;

typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	AlxIoPin_Iocon_Func func;
	uint32_t mode;		// MF: PullUp or PulDown
	bool digiMode;		// MF: True = Digital Mode, False = Analog Mode
	bool isOpenDrain;
	bool dir;			// MF: True = digital output, False = digital input
	bool val;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Iocon_Func func,
	uint32_t mode,
	bool digiMode,
	bool isOpenDrain,
	bool dir,
	bool val
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_IO_PIN_MCU_LPC55S6X_H
