/**
  ******************************************************************************
  * @file		alxTrace_McuLpc17xx.c
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

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTrace_McuLpc17xx.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC17XX)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	uint8_t func,
	LPC_USART_T* uart,
	AlxGlobal_BaudRate baudRate)
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


//******************************************************************************
// Functions
//******************************************************************************
void AlxTrace_Init(AlxTrace* me)
{
	// GPIO //
	Chip_IOCON_PinMux(LPC_IOCON, me->port, me->pin, IOCON_MODE_INACT, me->func);
	Chip_IOCON_DisableOD(LPC_IOCON, me->port, me->pin);
	Chip_GPIO_SetPinDIR(LPC_GPIO, me->port, me->pin, false);
	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, false);

	// UART //
	Chip_UART_Init(me->uart);
	Chip_UART_SetBaud(me->uart, me->baudRate);
	Chip_UART_ConfigData(me->uart, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
	Chip_UART_SetupFIFOS(me->uart, UART_FCR_FIFO_EN);
	Chip_UART_TXEnable(me->uart);

	me->isInit = true;
}
void AlxTrace_DeInit(AlxTrace* me)
{
	// TODO

	me->isInit = false;
}
void AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	while (*str)
	{
		while ((Chip_UART_ReadLineStatus(me->uart) & UART_LSR_THRE) == false) ; // Wait until TX buffer is empty
		Chip_UART_SendByte(me->uart, (const uint8_t)*str++);
	}
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC17XX)
