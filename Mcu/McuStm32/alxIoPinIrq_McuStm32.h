/**
  ******************************************************************************
  * @file		alxIoPinIrq_McuStm32.h
  * @brief		Auralix C Library - ALX IO Pin IRQ MCU STM32 Module
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
#ifndef ALX_IO_PIN_IRQ_MCU_STM32_H
#define ALX_IO_PIN_IRQ_MCU_STM32_H

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
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;

	// Parameters
	uint8_t numOfIoPins;
	Alx_IrqPriority* irqPriorityArr;

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
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins,
	Alx_IrqPriority* irqPriorityArr
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_IO_PIN_IRQ_MCU_STM32_H
