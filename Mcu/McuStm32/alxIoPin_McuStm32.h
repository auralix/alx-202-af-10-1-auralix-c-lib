/**
  ******************************************************************************
  * @file		alxIoPin_McuStm32.h
  * @brief		Auralix C Library - ALX IO Pin MCU STM32 Module
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

#ifndef ALX_IO_PIN_MCU_STM32_H
#define ALX_IO_PIN_MCU_STM32_H

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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	GPIO_TypeDef* port;
	bool val;

	// Variables
	GPIO_InitTypeDef igpio;

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
	GPIO_TypeDef* port,
	uint16_t pin,
	uint32_t mode,
	uint32_t pull,
	uint32_t speed,
	#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	uint32_t alternate,
	#endif // defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	bool val
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_MCU_STM32_H
