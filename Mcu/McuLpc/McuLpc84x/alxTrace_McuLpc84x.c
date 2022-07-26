/**
  ******************************************************************************
  * @file		alxTrace_McuLpc84x.c
  * @brief		Auralix C Library - ALX Trace MCU LPC84X Module
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
#include "alxTrace_McuLpc84x.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC84X)


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxTrace_GetSwmPortPinIndex(AlxTrace* me);
static swm_select_movable_t AlxTrace_GetUartFunc(AlxTrace* me);
static clock_select_t AlxTrace_GetUartClk(AlxTrace* me);
static uint8_t AlxTrace_GetUartResetIndex(AlxTrace* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t port,
	uint8_t pin,
	USART_Type* usart,
	AlxGlobal_BaudRate baudRate)
{
	// Parameters
	me->port = port;
	me->pin = pin;
	me->usart = usart;
	me->baudRate = (uint32_t)baudRate;

	// Variables
	me->usartConfig.baudRate_Bps = (uint32_t)baudRate;
	me->usartConfig.enableRx = false;
	me->usartConfig.enableTx = true;
	me->usartConfig.loopback = false;
	me->usartConfig.enableContinuousSCLK = false;
	me->usartConfig.parityMode = kUSART_ParityDisabled;
	me->usartConfig.stopBitCount = kUSART_OneStopBit;
	me->usartConfig.bitCountPerChar = kUSART_8BitsPerChar;
	me->usartConfig.syncMode = kUSART_SyncModeDisabled;
	me->usartConfig.clockPolarity = kUSART_RxSampleOnFallingEdge;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxTrace_Init(AlxTrace* me)
{
	// #1 Init SWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, AlxTrace_GetUartFunc(me), AlxTrace_GetSwmPortPinIndex(me));
	CLOCK_DisableClock(kCLOCK_Swm);

	// 2# Init UART
	CLOCK_Select(AlxTrace_GetUartClk(me));									// URGENT!!! this sets clock from Main and not Fro, frg0 or frg1
	USART_Init(me->usart, &me->usartConfig, CLOCK_GetFreq(kCLOCK_MainClk));	// URGENT!!! this sets clock from Main and not Fro, frg0 or frg1

	me->isInit = true;
}
void AlxTrace_DeInit(AlxTrace* me)
{
	// #1 DeInit UART
	USART_Deinit(me->usart);	// DeInit Clk and clears all USARTs Registers

	// #2 DeInit SWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, AlxTrace_GetUartFunc(me), kSWM_PortPin_Reset);
	CLOCK_DisableClock(kCLOCK_Swm);

	me->isInit = false;
}
void AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	USART_WriteBlocking(me->usart, (const uint8_t*)str, strlen(str));
}


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxTrace_GetSwmPortPinIndex(AlxTrace* me)
{
	if (me->port == 0)		return me->pin;
	else					return (me->pin + 32U);
}
static swm_select_movable_t AlxTrace_GetUartFunc(AlxTrace* me)
{
	if (me->usart == USART0)		return kSWM_USART0_TXD;
	if (me->usart == USART1)		return kSWM_USART1_TXD;
	if (me->usart == USART2)		return kSWM_USART2_TXD;
	if (me->usart == USART3)		return kSWM_USART3_TXD;
	if (me->usart == USART4)		return kSWM_USART4_TXD;

	else							return 0xFF;		// We shouldn't get here
}
static clock_select_t AlxTrace_GetUartClk(AlxTrace* me)
{
	if (me->usart == USART0)		return kUART0_Clk_From_MainClk;
	if (me->usart == USART1)		return kUART1_Clk_From_MainClk;
	if (me->usart == USART2)		return kUART2_Clk_From_MainClk;
	if (me->usart == USART3)		return kUART3_Clk_From_MainClk;
	if (me->usart == USART4)		return kUART4_Clk_From_MainClk;

	else							return 0xFF;		// We shouldn't get here
}
static uint8_t AlxTrace_GetUartResetIndex(AlxTrace* me)
{
	if (me->usart == USART0)		return 14U;
	if (me->usart == USART1)		return 15U;
	if (me->usart == USART2)		return 16U;
	if (me->usart == USART3)		return 30U;
	if (me->usart == USART4)		return 31U;

	else							return 0xFF;		// We shouldn't get here
}


#endif
