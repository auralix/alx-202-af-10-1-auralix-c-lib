/**
  ******************************************************************************
  * @file		alxSerialPort.c
  * @brief		Auralix C Library - ALX Serial Port Module
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
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxSerialPort.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableClk(AlxSerialPort* me);
static void AlxSerialPort_Periph_ForceReset(AlxSerialPort* me);
static void AlxSerialPort_Periph_ReleaseReset(AlxSerialPort* me);
static void AlxSerialPort_Periph_EnableRxIrq(AlxSerialPort* me);
static void AlxSerialPort_Periph_DisableRxIrq(AlxSerialPort* me);


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
)
{
	// Objects - Internal
	AlxFifo_Ctor(&me->rxFifo, rxFifoBuff, rxFifoBuffLen);
	
	// Objects - External
	me->do_TX = do_TX;
	me->di_RX = di_RX;
	
	// Parameters
	me->txTimeout_ms = txTimeout_ms;
	me->rxIrqPriority = rxIrqPriority;
	
	// Variables
	me->huart.Instance = uart;
	me->huart.Init.BaudRate = (uint32_t)baudRate;
	me->huart.Init.WordLength = dataWidth;
	me->huart.Init.StopBits = stopBits;
	me->huart.Init.Parity = parity;
	me->huart.Init.Mode = UART_MODE_TX_RX;
	me->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	me->huart.Init.OverSampling = UART_OVERSAMPLING_16;
	
	#if defined(ALX_STM32G4) || defined(ALX_STM32L0)
		me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		#if defined(ALX_STM32G4)
		me->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
		#endif
		me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
		me->huart.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
	#endif
	
	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxSerialPort_Init(AlxSerialPort* me)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == false);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	
	// #1 Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);
	
	// #2 Init GPIO
	AlxIoPin_Init(me->do_TX);
	AlxIoPin_Init(me->di_RX);
	
	// #3 Release UART Periphery Reset
	AlxSerialPort_Periph_ReleaseReset(me);
	
	// #4 Enable UART Periphery Clock
	AlxSerialPort_Periph_EnableClk(me);
	
	// #5 Init UART
	if(HAL_UART_Init(&me->huart) != HAL_OK) { ALX_SERIAL_PORT_TRACE("ErrInit"); return Alx_Err; };
	
	// #6 Enable UART RX IRQ
	__HAL_UART_ENABLE_IT(&me->huart, UART_IT_RXNE);
	AlxSerialPort_Periph_EnableRxIrq(me);
	
	// #7 Set isInit
	me->isInit = true;
	
	// #8 Return OK
	return Alx_Ok;
}
Alx_Status AlxSerialPort_DeInit(AlxSerialPort* me)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	
	// #1 Disable UART RX IRQ
	AlxSerialPort_Periph_DisableRxIrq(me);
	__HAL_UART_DISABLE_IT(&me->huart, UART_IT_RXNE); // We will not clear flag, becasue there are differences between STM32 HALs, flag will be cleared when UART periphery is reset
	
	// #2 DeInit UART
	if(HAL_UART_DeInit(&me->huart) != HAL_OK) { ALX_SERIAL_PORT_TRACE("ErrDeInit"); return Alx_Err; };
	
	// #3 Force UART Periphery Reset
	AlxSerialPort_Periph_ForceReset(me);
	
	// #4 Disable UART Periphery Clock
	AlxSerialPort_Periph_DisableClk(me);
	
	// #5 DeInit GPIO
	AlxIoPin_DeInit(me->do_TX);
	AlxIoPin_DeInit(me->di_RX);
	
	// #6 Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);
	
	// #7 Reset isInit
	me->isInit = false;
	
	// #8 Return OK
	return Alx_Ok;
}
Alx_Status AlxSerialPort_Read(AlxSerialPort* me, uint8_t* data, uint32_t len)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);

	AlxGlobal_DisableIrq();
	Alx_Status status = AlxFifo_Read(&me->rxFifo, data, len);
	AlxGlobal_EnableIrq();
	
	return status;
}
Alx_Status AlxSerialPort_ReadStrUntil(AlxSerialPort* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);

	AlxGlobal_DisableIrq();
	Alx_Status status = AlxFifo_ReadStrUntil(&me->rxFifo, str, delim, maxLen, numRead);
	AlxGlobal_EnableIrq();
	
	return status;
}
Alx_Status AlxSerialPort_Write(AlxSerialPort* me, uint8_t data)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	
	return AlxSerialPort_WriteMulti(me, &data, 1);
}
Alx_Status AlxSerialPort_WriteMulti(AlxSerialPort* me, const uint8_t* data, uint32_t len)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	
	if(HAL_UART_Transmit(&me->huart, (uint8_t*)data, len, me->txTimeout_ms) != HAL_OK)
	{
		ALX_SERIAL_PORT_TRACE("ErrWriteMulti");
		if(AlxSerialPort_Reset(me) != Alx_Ok) { ALX_SERIAL_PORT_TRACE("ErrReset"); return Alx_Err;};
	}
	
	return Alx_Ok;
}
Alx_Status AlxSerialPort_WriteStr(AlxSerialPort* me, const char* str)
{
	ALX_SERIAL_PORT_ASSERT(me->isInit == true);
	ALX_SERIAL_PORT_ASSERT(me->wasCtorCalled == true);
	
	return AlxSerialPort_WriteMulti(me, (const uint8_t*)str, strlen(str));
}
void AlxSerialPort_Foreground_Handle(AlxSerialPort* me)
{
	#if defined(ALX_STM32F4)
	// TV:	Overrun error handling, periphery doesn't have overrun error disable functionality.
	//		We clear overrun flag with sequence of first reading status register, and then data register.
	volatile const uint32_t dummy = me->huart.Instance->SR;
	#endif
	// TV: No overrun error handling, overrun error must be disabled @ Uart initialization.
	uint8_t data = LL_USART_ReceiveData8(me->huart.Instance); // Clears RXNE = 0
	AlxFifo_Write(&me->rxFifo, data);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxSerialPort_Reset(AlxSerialPort* me)
{
	// #1 Disable UART RX IRQ
	AlxSerialPort_Periph_DisableRxIrq(me);
	__HAL_UART_DISABLE_IT(&me->huart, UART_IT_RXNE); // We will not clear flag, because of differences between STM32 HALs, flag will be cleared when UART periphery is reset
	
	// #2 DeInit UART
	if(HAL_UART_DeInit(&me->huart) != HAL_OK) { ALX_SERIAL_PORT_TRACE("ErrDeInit"); return Alx_Err; };
	
	// #3 Force UART Periphery Reset
	AlxSerialPort_Periph_ForceReset(me);
	
	// #4 Flush RX FIFO
	AlxFifo_Flush(&me->rxFifo);
	
	// #5 Reset isInit
	me->isInit = false;
	
	// #6 Release UART Periphery Reset
	AlxSerialPort_Periph_ReleaseReset(me);
	
	// #7 Init UART
	if(HAL_UART_Init(&me->huart) != HAL_OK) { ALX_SERIAL_PORT_TRACE("ErrInit"); return Alx_Err; };

	// #8 Enable UART RX IRQ
	__HAL_UART_ENABLE_IT(&me->huart, UART_IT_RXNE);
	AlxSerialPort_Periph_EnableRxIrq(me);
	
	// #9 Set isInit
	me->isInit = true;
	
	// #10 Return OK
	return Alx_Ok;
}
static void AlxSerialPort_Periph_EnableClk(AlxSerialPort* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)		{ __HAL_RCC_USART1_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)		{ __HAL_RCC_USART2_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)		{ __HAL_RCC_USART3_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)		{ __HAL_RCC_USART4_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)		{ __HAL_RCC_USART5_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)		{ __HAL_RCC_USART6_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)		{ __HAL_RCC_USART7_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)		{ __HAL_RCC_USART8_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)		{ __HAL_RCC_USART9_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)		{ __HAL_RCC_USART10_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)		{ __HAL_RCC_UART1_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)		{ __HAL_RCC_UART2_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)		{ __HAL_RCC_UART3_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)		{ __HAL_RCC_UART4_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)		{ __HAL_RCC_UART5_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)		{ __HAL_RCC_UART6_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)		{ __HAL_RCC_UART7_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)		{ __HAL_RCC_UART8_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)		{ __HAL_RCC_UART9_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)		{ __HAL_RCC_UART10_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)		{ __HAL_RCC_LPUART1_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)		{ __HAL_RCC_LPUART2_CLK_ENABLE(); isErr = false; }
	#endif

	if(isErr)								{ ALX_SERIAL_PORT_ASSERT(false); } // We shouldn't get here
}
static void AlxSerialPort_Periph_DisableClk(AlxSerialPort* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)		{ __HAL_RCC_USART1_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)		{ __HAL_RCC_USART2_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)		{ __HAL_RCC_USART3_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)		{ __HAL_RCC_USART4_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)		{ __HAL_RCC_USART5_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)		{ __HAL_RCC_USART6_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)		{ __HAL_RCC_USART7_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)		{ __HAL_RCC_USART8_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)		{ __HAL_RCC_USART9_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)		{ __HAL_RCC_USART10_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)		{ __HAL_RCC_UART1_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)		{ __HAL_RCC_UART2_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)		{ __HAL_RCC_UART3_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)		{ __HAL_RCC_UART4_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)		{ __HAL_RCC_UART5_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)		{ __HAL_RCC_UART6_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)		{ __HAL_RCC_UART7_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)		{ __HAL_RCC_UART8_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)		{ __HAL_RCC_UART9_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)		{ __HAL_RCC_UART10_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)		{ __HAL_RCC_LPUART1_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)		{ __HAL_RCC_LPUART2_CLK_DISABLE(); isErr = false; }
	#endif

	if(isErr)								{ ALX_SERIAL_PORT_ASSERT(false); } // We shouldn't get here
}
static void AlxSerialPort_Periph_ForceReset(AlxSerialPort* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)		{ __HAL_RCC_USART1_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)		{ __HAL_RCC_USART2_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)		{ __HAL_RCC_USART3_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)		{ __HAL_RCC_USART4_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)		{ __HAL_RCC_USART5_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)		{ __HAL_RCC_USART6_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)		{ __HAL_RCC_USART7_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)		{ __HAL_RCC_USART8_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)		{ __HAL_RCC_USART9_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)		{ __HAL_RCC_USART10_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)		{ __HAL_RCC_UART1_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)		{ __HAL_RCC_UART2_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)		{ __HAL_RCC_UART3_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)		{ __HAL_RCC_UART4_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)		{ __HAL_RCC_UART5_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)		{ __HAL_RCC_UART6_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)		{ __HAL_RCC_UART7_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)		{ __HAL_RCC_UART8_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)		{ __HAL_RCC_UART9_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)		{ __HAL_RCC_UART10_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)		{ __HAL_RCC_LPUART1_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)		{ __HAL_RCC_LPUART2_FORCE_RESET(); isErr = false; }
	#endif

	if(isErr)								{ ALX_SERIAL_PORT_ASSERT(false); } // We shouldn't get here
}
static void AlxSerialPort_Periph_ReleaseReset(AlxSerialPort* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)		{ __HAL_RCC_USART1_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)		{ __HAL_RCC_USART2_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)		{ __HAL_RCC_USART3_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)		{ __HAL_RCC_USART4_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)		{ __HAL_RCC_USART5_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)		{ __HAL_RCC_USART6_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)		{ __HAL_RCC_USART7_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)		{ __HAL_RCC_USART8_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)		{ __HAL_RCC_USART9_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)		{ __HAL_RCC_USART10_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)		{ __HAL_RCC_UART1_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)		{ __HAL_RCC_UART2_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)		{ __HAL_RCC_UART3_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)		{ __HAL_RCC_UART4_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)		{ __HAL_RCC_UART5_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)		{ __HAL_RCC_UART6_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)		{ __HAL_RCC_UART7_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)		{ __HAL_RCC_UART8_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)		{ __HAL_RCC_UART9_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)		{ __HAL_RCC_UART10_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)		{ __HAL_RCC_LPUART1_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)		{ __HAL_RCC_LPUART2_RELEASE_RESET(); isErr = false; }
	#endif

	if(isErr)								{ ALX_SERIAL_PORT_ASSERT(false); } // We shouldn't get here
}
static void AlxSerialPort_Periph_EnableRxIrq(AlxSerialPort* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)		{ HAL_NVIC_SetPriority(USART1_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART1_IRQn); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)		{ HAL_NVIC_SetPriority(USART2_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART2_IRQn); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)		{ HAL_NVIC_SetPriority(USART3_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART3_IRQn); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)		{ HAL_NVIC_SetPriority(USART4_5_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART4_5_IRQn); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)		{ HAL_NVIC_SetPriority(USART4_5_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART4_5_IRQn); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)		{ HAL_NVIC_SetPriority(USART6_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART6_IRQn); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)		{ HAL_NVIC_SetPriority(USART7_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART7_IRQn); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)		{ HAL_NVIC_SetPriority(USART8_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART8_IRQn); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)		{ HAL_NVIC_SetPriority(USART9_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART9_IRQn); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)		{ HAL_NVIC_SetPriority(USART10_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(USART10_IRQn); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)		{ HAL_NVIC_SetPriority(UART1_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART1_IRQn); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)		{ HAL_NVIC_SetPriority(UART2_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART2_IRQn); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)		{ HAL_NVIC_SetPriority(UART3_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART3_IRQn); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)		{ HAL_NVIC_SetPriority(UART4_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART4_IRQn); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)		{ HAL_NVIC_SetPriority(UART5_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART5_IRQn); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)		{ HAL_NVIC_SetPriority(UART6_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART6_IRQn); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)		{ HAL_NVIC_SetPriority(UART7_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART7_IRQn); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)		{ HAL_NVIC_SetPriority(UART8_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART8_IRQn); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)		{ HAL_NVIC_SetPriority(UART9_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART9_IRQn); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)		{ HAL_NVIC_SetPriority(UART10_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(UART10_IRQn); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)		{ HAL_NVIC_SetPriority(LPUART1_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(LPUART1_IRQn); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)		{ HAL_NVIC_SetPriority(LPUART2_IRQn, me->rxIrqPriority, 0); HAL_NVIC_EnableIRQ(LPUART2_IRQn); isErr = false; }
	#endif

	if(isErr)								{ ALX_SERIAL_PORT_ASSERT(false); } // We shouldn't get here
}
static void AlxSerialPort_Periph_DisableRxIrq(AlxSerialPort* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)		{ HAL_NVIC_DisableIRQ(USART1_IRQn); HAL_NVIC_ClearPendingIRQ(USART1_IRQn); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)		{ HAL_NVIC_DisableIRQ(USART2_IRQn); HAL_NVIC_ClearPendingIRQ(USART2_IRQn); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)		{ HAL_NVIC_DisableIRQ(USART3_IRQn); HAL_NVIC_ClearPendingIRQ(USART3_IRQn); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)		{ HAL_NVIC_DisableIRQ(USART4_5_IRQn); HAL_NVIC_ClearPendingIRQ(USART4_5_IRQn);isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)		{ HAL_NVIC_DisableIRQ(USART4_5_IRQn); HAL_NVIC_ClearPendingIRQ(USART4_5_IRQn);isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)		{ HAL_NVIC_DisableIRQ(USART6_IRQn); HAL_NVIC_ClearPendingIRQ(USART6_IRQn); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)		{ HAL_NVIC_DisableIRQ(USART7_IRQn); HAL_NVIC_ClearPendingIRQ(USART7_IRQn); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)		{ HAL_NVIC_DisableIRQ(USART8_IRQn); HAL_NVIC_ClearPendingIRQ(USART8_IRQn); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)		{ HAL_NVIC_DisableIRQ(USART9_IRQn); HAL_NVIC_ClearPendingIRQ(USART9_IRQn); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)		{ HAL_NVIC_DisableIRQ(USART10_IRQn); HAL_NVIC_ClearPendingIRQ(USART10_IRQn); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)		{ HAL_NVIC_DisableIRQ(UART1_IRQn); HAL_NVIC_ClearPendingIRQ(UART1_IRQn); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)		{ HAL_NVIC_DisableIRQ(UART2_IRQn); HAL_NVIC_ClearPendingIRQ(UART2_IRQn); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)		{ HAL_NVIC_DisableIRQ(UART3_IRQn); HAL_NVIC_ClearPendingIRQ(UART3_IRQn); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)		{ HAL_NVIC_DisableIRQ(UART4_IRQn); HAL_NVIC_ClearPendingIRQ(UART4_IRQn); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)		{ HAL_NVIC_DisableIRQ(UART5_IRQn); HAL_NVIC_ClearPendingIRQ(UART5_IRQn); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)		{ HAL_NVIC_DisableIRQ(UART6_IRQn); HAL_NVIC_ClearPendingIRQ(UART6_IRQn); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)		{ HAL_NVIC_DisableIRQ(UART7_IRQn); HAL_NVIC_ClearPendingIRQ(UART7_IRQn); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)		{ HAL_NVIC_DisableIRQ(UART8_IRQn); HAL_NVIC_ClearPendingIRQ(UART8_IRQn); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)		{ HAL_NVIC_DisableIRQ(UART9_IRQn); HAL_NVIC_ClearPendingIRQ(UART9_IRQn); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)		{ HAL_NVIC_DisableIRQ(UART10_IRQn); HAL_NVIC_ClearPendingIRQ(UART10_IRQn); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)		{ HAL_NVIC_DisableIRQ(LPUART1_IRQn); HAL_NVIC_ClearPendingIRQ(LPUART1_IRQn); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)		{ HAL_NVIC_DisableIRQ(LPUART2_IRQn); HAL_NVIC_ClearPendingIRQ(LPUART2_IRQn); isErr = false; }
	#endif

	if(isErr)								{ ALX_SERIAL_PORT_ASSERT(false); } // We shouldn't get here
}


#endif
