﻿/**
  ******************************************************************************
  * @file		alxTrace_McuStm32.h
  * @brief		Auralix C Library - ALX Trace MCU STM32 Module
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
#ifndef ALX_TRACE_MCU_STM32_H
#define ALX_TRACE_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_TRACE_TIMEOUT_ms 1000

	// Parameters
	GPIO_TypeDef* port;
	uint16_t pin;
	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	uint32_t alternate;
	#endif
	USART_TypeDef* uart;
	AlxGlobal_BaudRate baudRate;

	// Variables
	GPIO_InitTypeDef igpio;
	UART_HandleTypeDef huart;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxTrace;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	GPIO_TypeDef* port,
	uint16_t pin,
	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	uint32_t alternate,
	#endif
	USART_TypeDef* uart,
	AlxGlobal_BaudRate baudRate
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TRACE_MCU_STM32_H
