/**
  ******************************************************************************
  * @file		alxSerialPort_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX Serial Port MCU LPC55S6X Module
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
#include "alxSerialPort_McuLpc55S6x.h"
#include "alxSerialPort.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me);
static uint32_t AlxSerialPort_Periph_GetClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_AttachClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableClkAndReset(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableIrq(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableIrq(AlxSerialPort* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		usart
  * @param[in]		do_TX
  * @param[in]		di_RX
  * @param[in]		baudRate
  * @param[in]		rxFifoBuff
  * @param[in]		rxFifoBuffLen
  * @param[in]		rxIrqPriority
  */
void AlxSerialPort_Ctor
(
	AlxSerialPort* me,
	USART_Type* usart,
	AlxIoPin* do_TX,
	AlxIoPin* di_RX,
	AlxGlobal_BaudRate baudRate,
	uint8_t* rxFifoBuff,
	uint32_t rxFifoBuffLen,
	Alx_IrqPriority rxIrqPriority
)
{
	// Parameters
	me->usart = usart;
	me->do_TX = do_TX;
	me->di_RX = di_RX;
	me->baudRate = baudRate;
	me->rxFifoBuff = rxFifoBuff;
	me->rxFifoBuffLen = rxFifoBuffLen;
	me->rxIrqPriority = rxIrqPriority;

	// Variables
	me->usartConfig.baudRate_Bps				= (uint32_t)baudRate;
	me->usartConfig.parityMode					= kUSART_ParityDisabled;
	me->usartConfig.stopBitCount				= kUSART_OneStopBit;
	me->usartConfig.bitCountPerChar				= kUSART_8BitsPerChar;
	me->usartConfig.loopback					= false;
	me->usartConfig.enableRx					= true;
	me->usartConfig.enableTx					= true;
	me->usartConfig.enableContinuousSCLK		= false;
	me->usartConfig.enableMode32k				= false;
	me->usartConfig.enableHardwareFlowControl	= false;
	me->usartConfig.txWatermark					= kUSART_TxFifo0;
	me->usartConfig.rxWatermark					= kUSART_RxFifo1;
	me->usartConfig.syncMode					= kUSART_SyncModeDisabled;
	me->usartConfig.clockPolarity				= kUSART_RxSampleOnFallingEdge;

	AlxFifo_Ctor(&me->rxFifo, rxFifoBuff, rxFifoBuffLen);

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
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
Alx_Status AlxSerialPort_Init(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == false);

	// Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);

	// Init GPIO
	AlxIoPin_Init(me->do_TX);
	AlxIoPin_Init(me->di_RX);

	// Attach USART periphery clock
	AlxSerialPort_Periph_AttachClk(me);

	// Init USART
	if (USART_Init(me->usart, &me->usartConfig, AlxSerialPort_Periph_GetClk(me)) != kStatus_Success) { return Alx_Err; }	// MF: FlexComm Init (Periph_Reset and EnableClk) happens in "USART_Init()"

	// Enable UART RX IRQ
	USART_EnableInterrupts(me->usart, kUSART_RxLevelInterruptEnable);
	AlxSerialPort_Periph_EnableIrq(me);

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
Alx_Status AlxSerialPort_DeInit(AlxSerialPort* me)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Disable UART RX IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	USART_DisableInterrupts(me->usart, kUSART_RxLevelInterruptEnable);

	// DeInit USART
	USART_Deinit(me->usart);

	// Disable USART periphery clock and reset USART periphery
	AlxSerialPort_Periph_DisableClkAndReset(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_TX);
	AlxIoPin_DeInit(me->di_RX);

	// Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Read(AlxSerialPort* me, uint8_t* data, uint32_t len)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Read
	AlxGlobal_DisableIrq();
	Alx_Status status = AlxFifo_Read(&me->rxFifo, data, len);
	AlxGlobal_EnableIrq();

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		str
  * @param[in]		delim
  * @param[in]		maxLen
  * @param[out]		numRead
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_ReadStrUntil(AlxSerialPort* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Read
	AlxGlobal_DisableIrq();
	Alx_Status status = AlxFifo_ReadStrUntil(&me->rxFifo, str, delim, maxLen, numRead);
	AlxGlobal_EnableIrq();

	// Return
	return status;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Write(AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Write
	if (USART_WriteBlocking(me->usart, data, len) != kStatus_Success)
	{
		ALX_SERIAL_PORT_TRACE_ERR("Err");
		if(AlxSerialPort_Reset(me) != Alx_Ok)
		{
			ALX_SERIAL_PORT_TRACE_ERR("Err");
			return Alx_Err;
		}
	}

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
Alx_Status AlxSerialPort_WriteStr(AlxSerialPort* me, const char* str)
{
	// Assert
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);

	// Return
	return AlxSerialPort_Write(me, (const uint8_t*)str, strlen(str));
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_IrqHandler(AlxSerialPort* me)
{
	uint8_t data = USART_ReadByte(me->usart);
	AlxFifo_Write(&me->rxFifo, &data, 1);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_FlushRxFifo(AlxSerialPort* me)
{
	AlxFifo_Flush(&me->rxFifo);
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint32_t AlxSerialPort_GetRxFifoNumOfEntries(AlxSerialPort* me)
{
	return AlxFifo_GetNumOfEntries(&me->rxFifo);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me)
{
	// Disable UART RX IRQ
	AlxSerialPort_Periph_DisableIrq(me);
	USART_DisableInterrupts(me->usart, kUSART_RxLevelInterruptEnable);

	// DeInit USART
	USART_Deinit(me->usart);

	// Disable USART periphery clock and reset USART periphery
	AlxSerialPort_Periph_DisableClkAndReset(me);

	// Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);

	// Clear isInit
	me->isInit = false;

	// Attach USART periphery clock
	AlxSerialPort_Periph_AttachClk(me);

	// Init USART
	if (USART_Init(me->usart, &me->usartConfig, AlxSerialPort_Periph_GetClk(me)) != kStatus_Success) { return Alx_Err; }	// MF: FlexComm Init (Periph_Reset and EnableClk) happens in "USART_Init()"

	// Enable UART RX IRQ
	USART_EnableInterrupts(me->usart, kUSART_RxLevelInterruptEnable);
	AlxSerialPort_Periph_EnableIrq(me);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static uint32_t AlxSerialPort_Periph_GetClk(AlxSerialPort* me)
{
	#if defined(USART0)
	if (me->usart == USART0)	{ return CLOCK_GetFlexCommClkFreq(0U); }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ return CLOCK_GetFlexCommClkFreq(1U); }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ return CLOCK_GetFlexCommClkFreq(2U); }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ return CLOCK_GetFlexCommClkFreq(3U); }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ return CLOCK_GetFlexCommClkFreq(4U); }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ return CLOCK_GetFlexCommClkFreq(5U); }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ return CLOCK_GetFlexCommClkFreq(6U); }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ return CLOCK_GetFlexCommClkFreq(7U); }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We shouldn't get here
	return 0;
}
static void AlxSerialPort_Periph_AttachClk(AlxSerialPort* me)
{
	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM0); return; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1); return; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM2); return; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3); return; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4); return; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5); return; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6); return; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We shouldn't get here
}
static void AlxSerialPort_Periph_DisableClkAndReset(AlxSerialPort* me)
{
	#if defined(USART0)
	if (me->usart == USART0)	{ CLOCK_DisableClock(kCLOCK_FlexComm0); RESET_PeripheralReset(kFC0_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ CLOCK_DisableClock(kCLOCK_FlexComm1); RESET_PeripheralReset(kFC1_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ CLOCK_DisableClock(kCLOCK_FlexComm2); RESET_PeripheralReset(kFC2_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ CLOCK_DisableClock(kCLOCK_FlexComm3); RESET_PeripheralReset(kFC3_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ CLOCK_DisableClock(kCLOCK_FlexComm4); RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ CLOCK_DisableClock(kCLOCK_FlexComm5); RESET_PeripheralReset(kFC5_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ CLOCK_DisableClock(kCLOCK_FlexComm6); RESET_PeripheralReset(kFC6_RST_SHIFT_RSTn); return; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ CLOCK_DisableClock(kCLOCK_FlexComm7); RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We shouldn't get here
}
static void AlxSerialPort_Periph_EnableIrq(AlxSerialPort* me)
{
	#if defined(USART0)
	if (me->usart == USART0)	{ NVIC_SetPriority(FLEXCOMM0_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM0_IRQn); return; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ NVIC_SetPriority(FLEXCOMM1_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM1_IRQn); return; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ NVIC_SetPriority(FLEXCOMM2_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM2_IRQn); return; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ NVIC_SetPriority(FLEXCOMM3_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM3_IRQn); return; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ NVIC_SetPriority(FLEXCOMM4_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM4_IRQn); return; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ NVIC_SetPriority(FLEXCOMM5_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM5_IRQn); return; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ NVIC_SetPriority(FLEXCOMM6_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM6_IRQn); return; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ NVIC_SetPriority(FLEXCOMM7_IRQn, me->rxIrqPriority); NVIC_EnableIRQ(FLEXCOMM7_IRQn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We shouldn't get here
}
static void AlxSerialPort_Periph_DisableIrq(AlxSerialPort* me)
{
	#if defined(USART0)
	if (me->usart == USART0)	{ NVIC_DisableIRQ(FLEXCOMM0_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM0_IRQn); return; }
	#endif
	#if defined(USART1)
	if (me->usart == USART1)	{ NVIC_DisableIRQ(FLEXCOMM1_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM1_IRQn); return; }
	#endif
	#if defined(USART2)
	if (me->usart == USART2)	{ NVIC_DisableIRQ(FLEXCOMM2_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM2_IRQn); return; }
	#endif
	#if defined(USART3)
	if (me->usart == USART3)	{ NVIC_DisableIRQ(FLEXCOMM3_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM3_IRQn); return; }
	#endif
	#if defined(USART4)
	if (me->usart == USART4)	{ NVIC_DisableIRQ(FLEXCOMM4_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM4_IRQn); return; }
	#endif
	#if defined(USART5)
	if (me->usart == USART5)	{ NVIC_DisableIRQ(FLEXCOMM5_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM5_IRQn); return; }
	#endif
	#if defined(USART6)
	if (me->usart == USART6)	{ NVIC_DisableIRQ(FLEXCOMM6_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM6_IRQn); return; }
	#endif
	#if defined(USART7)
	if (me->usart == USART7)	{ NVIC_DisableIRQ(FLEXCOMM7_IRQn); NVIC_ClearPendingIRQ(FLEXCOMM7_IRQn); return; }
	#endif

	ALX_SERIAL_PORT_ASSERT(false);	// We shouldn't get here
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
