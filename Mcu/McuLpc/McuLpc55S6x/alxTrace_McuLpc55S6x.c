/**
  ******************************************************************************
  * @file		alxTrace_McuLpc55S6x.c
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
// Includes
//******************************************************************************
#include "alxTrace_McuLpc55S6x.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_Reset(AlxTrace* me);
static uint8_t AlxTrace_Periph_GetIoconFunc(AlxTrace* me);
static uint32_t AlxTrace_Periph_GetClk(AlxTrace* me);
static void AlxTrace_Periph_AttachClk(AlxTrace* me);
static void AlxTrace_Periph_DisableClkAndReset(AlxTrace* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		usart
  * @param[in]		baudRate
  */
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
	me->usartConfig.baudRate_Bps				= (uint32_t)baudRate;
	me->usartConfig.parityMode					= kUSART_ParityDisabled;
	me->usartConfig.stopBitCount				= kUSART_OneStopBit;
	me->usartConfig.bitCountPerChar				= kUSART_8BitsPerChar;
	me->usartConfig.loopback					= false;
	me->usartConfig.enableRx					= false;
	me->usartConfig.enableTx					= true;
	me->usartConfig.enableContinuousSCLK		= false;
	me->usartConfig.enableMode32k				= false;
	me->usartConfig.enableHardwareFlowControl	= false;
	me->usartConfig.txWatermark					= kUSART_TxFifo0;
	me->usartConfig.rxWatermark					= kUSART_RxFifo1;
	me->usartConfig.syncMode					= kUSART_SyncModeDisabled;
	me->usartConfig.clockPolarity				= kUSART_RxSampleOnFallingEdge;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	// Init GPIO
	IOCON_PinMuxSet(IOCON, me->port, me->pin, AlxTrace_Periph_GetIoconFunc(me));

	// Attach USART periphery clock
	AlxTrace_Periph_AttachClk(me);

	// Init USART
	if (USART_Init(me->usart, &me->usartConfig, AlxTrace_Periph_GetClk(me)) != kStatus_Success) { return Alx_Err; }	// MF: FlexComm Init (Periph_Reset and EnableClk) happens in "USART_Init()"

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{
	// DeInit USART
	USART_Deinit(me->usart);

	// Disable USART periphery clock and reset USART periphery
	AlxTrace_Periph_DisableClkAndReset(me);

	// DeInit GPIO
	IOCON_PinMuxSet(IOCON, me->port, me->pin, IOCON_FUNC0);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	// Write
	if (USART_WriteBlocking(me->usart, (const uint8_t*)str, strlen(str)) != kStatus_Success)
	{
		return AlxTrace_Reset(me);
	}

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_Reset(AlxTrace* me)
{
	// DeInit USART
	USART_Deinit(me->usart);

	// Disable USART periphery clock and reset USART periphery
	AlxTrace_Periph_DisableClkAndReset(me);

	// Reset isInit
	me->isInit = false;

	// Attach USART periphery clock
	AlxTrace_Periph_AttachClk(me);

	// Init USART
	if (USART_Init(me->usart, &me->usartConfig, AlxTrace_Periph_GetClk(me)) != kStatus_Success) { return Alx_Err; }	// MF: FlexComm Init (Periph_Reset and EnableClk) happens in "USART_Init()"

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static uint8_t AlxTrace_Periph_GetIoconFunc(AlxTrace* me)
{
	bool isErr = true;

	// Get IOCON Func (see Table 340-342 in User Manual on page 343)
	#if defined(USART0)
	if (me->usart == USART0)						{ isErr = false; return IOCON_FUNC1; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)
	{
		if		(me->port == 0 && me->pin == 10)	{ isErr = false; return IOCON_FUNC4; }
		else if (me->port == 0 && me->pin == 14)	{ isErr = false; return IOCON_FUNC6; }
		else if (me->port == 1 && me->pin == 11)	{ isErr = false; return IOCON_FUNC2; }
		else										{ isErr = true; }
	}
	#endif
	#if defined(USART2)
	if (me->usart == USART2)						{ isErr = false; return IOCON_FUNC1; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)						{ isErr = false; return IOCON_FUNC1; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)
	{
		if		(me->port == 0 && me->pin == 16)	{ isErr = false; return IOCON_FUNC1; }
		else if (me->port == 0 && me->pin == 20)	{ isErr = false; return IOCON_FUNC11; }
		else if (me->port == 1 && me->pin == 20)	{ isErr = false; return IOCON_FUNC5; }
		else										{ isErr = true; }
	}
	#endif
	#if defined(USART5)
	if (me->usart == USART5)						{ isErr = false; return IOCON_FUNC3; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)
	{
		if		(me->port == 0 && me->pin == 12)	{ isErr = false; return IOCON_FUNC7; }
		else if (me->port == 0 && me->pin == 22)	{ isErr = false; return IOCON_FUNC1; }
		else if (me->port == 1 && me->pin == 16)	{ isErr = false; return IOCON_FUNC2; }
		else										{ isErr = true; }
	}
	#endif
	#if defined(USART7)
	if (me->usart == USART7)
	{
		if		(me->port == 0 && me->pin == 19)	{ isErr = false; return IOCON_FUNC7; }
		else if (me->port == 1 && me->pin == 30)	{ isErr = false; return IOCON_FUNC1; }
		else										{ isErr = true; }
	}
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
	return 0xFF;
}
static uint32_t AlxTrace_Periph_GetClk(AlxTrace* me)
{
	bool isErr = true;

	#if defined(USART0)
	if (me->usart == USART0)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(0U); }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(1U); }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(2U); }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(3U); }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(4U); }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(5U); }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(6U); }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ isErr = false; return CLOCK_GetFlexCommClkFreq(7U); }
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
	return 0xFFFFFFFF;
}
static void AlxTrace_Periph_AttachClk(AlxTrace* me)
{
	bool isErr = true;

	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM0); isErr = false; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1); isErr = false; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM2); isErr = false; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3); isErr = false; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4); isErr = false; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5); isErr = false; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6); isErr = false; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7); isErr = false; }
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
}
static void AlxTrace_Periph_DisableClkAndReset(AlxTrace* me)
{
	bool isErr = true;

	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_DisableClock(kCLOCK_FlexComm0); RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_DisableClock(kCLOCK_FlexComm1); RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_DisableClock(kCLOCK_FlexComm2); RESET_PeripheralReset(kFC2_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_DisableClock(kCLOCK_FlexComm3); RESET_PeripheralReset(kFC3_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_DisableClock(kCLOCK_FlexComm4); RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ CLOCK_DisableClock(kCLOCK_FlexComm5); RESET_PeripheralReset(kFC5_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ CLOCK_DisableClock(kCLOCK_FlexComm6); RESET_PeripheralReset(kFC6_RST_SHIFT_RSTn); isErr = false; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ CLOCK_DisableClock(kCLOCK_FlexComm7); RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn); isErr = false; }
	#endif

	if (isErr)
	{
		// We shouldn't get here
	}
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
