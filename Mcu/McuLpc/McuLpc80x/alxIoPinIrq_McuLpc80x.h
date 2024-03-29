﻿/**
  ******************************************************************************
  * @file		alxIoPinIrq_McuLpc80x.h
  * @brief		Auralix C Library - ALX IO Pin IRQ MCU LPC80X Module
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
#ifndef ALX_IO_PIN_IRQ_MCU_LPC80X_H
#define ALX_IO_PIN_IRQ_MCU_LPC80X_H

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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin* ioPin;

	// Variables
	pint_pin_int_t irqPin;
	pint_pin_enable_t irqType;
	Alx_IrqPriority irqPriority;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxIoPinIrq;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPinIrq_Ctor
(
	AlxIoPinIrq* me,
	AlxIoPin* ioPin,
	pint_pin_int_t irqPin,
	pint_pin_enable_t irqType,
	Alx_IrqPriority irqPriority
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_IO_PIN_IRQ_MCU_LPC80X_H
