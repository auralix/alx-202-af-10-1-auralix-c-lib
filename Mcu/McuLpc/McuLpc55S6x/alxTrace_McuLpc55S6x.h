﻿/**
  ******************************************************************************
  * @file		alxTrace_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX Trace MCU LPC55S6X Module
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
#ifndef ALX_TRACE_MCU_LPC55S6X_H
#define ALX_TRACE_MCU_LPC55S6X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	USART_Type* usart;
	AlxGlobal_BaudRate baudRate;

	// Variables
	usart_config_t usartConfig;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTrace;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	USART_Type* usart,
	AlxGlobal_BaudRate baudRate
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TRACE_MCU_LPC55S6X_H
