/**
  ******************************************************************************
  * @file		alxTrace_McuLpc17xx.h
  * @brief		Auralix C Library - ALX Trace MCU LPC17XX Module
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

#ifndef ALX_TRACE_MCU_LPC17XX_H
#define ALX_TRACE_MCU_LPC17XX_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17XX)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	uint8_t func;
	LPC_USART_T* uart;
	uint32_t baudRate;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTrace;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	uint8_t func,
	LPC_USART_T* uart,
	AlxGlobal_BaudRate baudRate
)
{
	// Parameters
	me->port = port;
	me->pin = pin;
	me->func = func;
	me->uart = uart;
	me->baudRate = (uint32_t)baudRate;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_TRACE_MCU_LPC17XX_H
