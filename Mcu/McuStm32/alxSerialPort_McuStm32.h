/**
  ******************************************************************************
  * @file		alxSerialPort_McuStm32.h
  * @brief		Auralix C Library - ALX Serial Port MCU STM32 Module
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

#ifndef ALX_SERIAL_PORT_MCU_STM32_H
#define ALX_SERIAL_PORT_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFifo.h"
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
	// Objects - Internal
	AlxFifo rxFifo;

	// Objects - External
	AlxIoPin* do_TX;
	AlxIoPin* di_RX;

	// Parameters
	uint16_t txTimeout_ms;
	Alx_IrqPriority rxIrqPriority;

	// Variables
	UART_HandleTypeDef huart;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxSerialPort;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	USART_TypeDef* uart,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	AlxGlobal_BaudRate baudRate,
	uint32_t dataWidth,
	uint32_t stopBits,
	uint32_t parity,
	uint16_t txTimeout_ms,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority rxIrqPriority
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_SERIAL_PORT_MCU_STM32_H
