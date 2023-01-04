/**
  ******************************************************************************
  * @file		alxTrace_McuLpc80x.c
  * @brief		Auralix C Library - ALX Trace MCU LPC80X Module
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
// Includes
//******************************************************************************
#include "alxTrace_McuLpc80x.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC80X)


//******************************************************************************
// Private Functions
//******************************************************************************
static swm_port_pin_type_t AlxTrace_GetSwmPortPinIndex(AlxTrace* me);
static swm_select_movable_t AlxTrace_GetUartFunc(AlxTrace* me);
static Alx_Status AlxTrace_Reset(AlxTrace* me);
static void AlxTrace_Periph_SelectClk(AlxTrace* me);


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
)
{
	// Assert
	(void)me;
	(void)port;
	(void)pin;
	(void)usart;
	(void)baudRate;

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
// Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me, bool threadSafe)
{
	// #1 Init SWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, AlxTrace_GetUartFunc(me), AlxTrace_GetSwmPortPinIndex(me));
	CLOCK_DisableClock(kCLOCK_Swm);

	// #2 Select I2C Periphery Clock
	AlxTrace_Periph_SelectClk(me);

	// #3 Init UART
	if (USART_Init(me->usart, &me->usartConfig, CLOCK_GetMainClkFreq()) != kStatus_Success) { return Alx_Err; }		// MF: "Periph_Reset" and "EnableClk" happens here

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_DeInit(AlxTrace* me, bool threadSafe)
{
	// #1 DeInit UART
	USART_Deinit(me->usart);	// MF: "DisableClk" happens here

	// #2 DeInit SWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, AlxTrace_GetUartFunc(me), kSWM_PortPin_Reset);
	CLOCK_DisableClock(kCLOCK_Swm);

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str, bool threadSafe)
{
	// #1 Write
	if (USART_WriteBlocking(me->usart, (const uint8_t*)str, strlen(str)) != kStatus_Success)
	{
		AlxTrace_Reset(me);
		return Alx_Err;
	}

	// #2 Return OK
	return Alx_Ok;
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
	#if defined(USART0)
	if (me->usart == USART0)	return kSWM_USART0_TXD;
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	return kSWM_USART1_TXD;
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	return kSWM_USART2_TXD;
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	return kSWM_USART3_TXD;
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	return kSWM_USART4_TXD;
	#endif

	else						return 0xFF; // We shouldn't get here
}
static Alx_Status AlxTrace_Reset(AlxTrace* me)
{
	// #1 DeInit Trace
	// #1.1 DeInit UART
	USART_Deinit(me->usart); // MF: "DisableClk" happens here

	// #1.2 DeInit SWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, AlxTrace_GetUartFunc(me), kSWM_PortPin_Reset);
	CLOCK_DisableClock(kCLOCK_Swm);

	// #2 Reset isInit
	me->isInit = false;

	// #3 Init Trace
	// #3.1 Init SWM
	CLOCK_EnableClock(kCLOCK_Swm);
	SWM_SetMovablePinSelect(SWM0, AlxTrace_GetUartFunc(me), AlxTrace_GetSwmPortPinIndex(me));
	CLOCK_DisableClock(kCLOCK_Swm);

	// #3.2 Select I2C Periphery Clock
	AlxTrace_Periph_SelectClk(me);

	// #3.3 Init UART
	if (USART_Init(me->usart, &me->usartConfig, CLOCK_GetMainClkFreq()) != kStatus_Success) { return Alx_Err; }		// MF: "Periph_Reset" and "EnableClk" happens here

	// #4 Set isInit
	me->isInit = true;

	// #5 Return OK
	return Alx_Ok;
}
static void AlxTrace_Periph_SelectClk(AlxTrace* me)
{
	bool isErr = true;

	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_Select(kUART0_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_Select(kUART1_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_Select(kUART2_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_Select(kUART3_Clk_From_MainClk); isErr = false; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_Select(kUART4_Clk_From_MainClk); isErr = false; }
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)
