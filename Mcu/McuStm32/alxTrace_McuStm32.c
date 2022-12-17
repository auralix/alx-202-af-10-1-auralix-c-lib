/**
  ******************************************************************************
  * @file		alxTrace_McuStm32.c
  * @brief		Auralix C Library - ALX Trace MCU STM32 Module
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
#include "alxTrace_McuStm32.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && ((defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)) && (!defined(ALX_MBED)))


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_Reset(AlxTrace* me);
static void AlxTrace_Periph_EnableClk(AlxTrace* me);
static void AlxTrace_Periph_DisableClk(AlxTrace* me);
static void AlxTrace_Periph_ForceReset(AlxTrace* me);
static void AlxTrace_Periph_ReleaseReset(AlxTrace* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTrace_Ctor
(
	AlxTrace* me,
	GPIO_TypeDef* port,
	uint16_t pin,
	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	uint32_t alternate,
	#endif
	USART_TypeDef* uart,
	AlxGlobal_BaudRate baudRate
)
{
	// Const
	me->TIMEOUT_ms = 1000;

	// Parameters
	me->port = port;
	me->igpio.Pin = pin;
	me->igpio.Mode = GPIO_MODE_AF_PP;
	me->igpio.Pull = GPIO_NOPULL;
	me->igpio.Speed = GPIO_SPEED_HIGH;
	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
	me->igpio.Alternate = alternate;
	#endif
	me->huart.Instance = uart;
	me->huart.Init.BaudRate = (uint32_t)baudRate;
	me->huart.Init.WordLength = UART_WORDLENGTH_8B;
	me->huart.Init.StopBits = UART_STOPBITS_1;
	me->huart.Init.Parity = UART_PARITY_NONE;
	me->huart.Init.Mode = UART_MODE_TX;
	me->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	me->huart.Init.OverSampling = UART_OVERSAMPLING_16;
	#if defined(ALX_STM32F0)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	#endif
	#if defined(ALX_STM32G4)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	#endif
	#if defined(ALX_STM32L0)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	#endif
	#if defined(ALX_STM32L4)
	me->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	me->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	me->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	#endif

	// Variables
	#if defined(ALX_OS)
	AlxOsMutex_Ctor(&me->mutex);
	#endif

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTrace_Init(AlxTrace* me, bool threadSafe)
{
	// Lock mutex
	#if defined(ALX_OS)
	if (threadSafe) AlxOsMutex_Lock(&me->mutex);
	#endif

	// Init GPIO
	HAL_GPIO_WritePin(me->port, me->igpio.Pin, (GPIO_PinState)false);	// Set initial output value, before config
	HAL_GPIO_Init(me->port, &me->igpio);
	HAL_GPIO_WritePin(me->port, me->igpio.Pin, (GPIO_PinState)false);	// Set initial output value, after config

	// Release UART periphery reset
	AlxTrace_Periph_ReleaseReset(me);

	// Enable UART periphery clock
	AlxTrace_Periph_EnableClk(me);

	// Init UART
	if (HAL_UART_Init(&me->huart) != HAL_OK) { return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Unlock mutex
	#if defined(ALX_OS)
	if (threadSafe) AlxOsMutex_Unlock(&me->mutex);
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxTrace_DeInit(AlxTrace* me, bool threadSafe)
{
	// Lock mutex
	#if defined(ALX_OS)
	if (threadSafe) AlxOsMutex_Lock(&me->mutex);
	#endif

	// DeInit UART
	if (HAL_UART_DeInit(&me->huart) != HAL_OK) { return Alx_Err; }

	// Disable UART periphery clock
	AlxTrace_Periph_DisableClk(me);

	// Force UART periphery reset
	AlxTrace_Periph_ForceReset(me);

	// DeInit GPIO
	HAL_GPIO_DeInit(me->port, me->igpio.Pin);

	// Clear isInit
	me->isInit = false;

	// Unlock mutex
	#if defined(ALX_OS)
	if (threadSafe) AlxOsMutex_Unlock(&me->mutex);
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str, bool threadSafe)
{
	// Lock mutex
	#if defined(ALX_OS)
	if (threadSafe) AlxOsMutex_Lock(&me->mutex);
	#endif

	// Write
	if (HAL_UART_Transmit(&me->huart, (uint8_t*)str, strlen(str), me->TIMEOUT_ms) != HAL_OK)
	{
		// Reset
		AlxTrace_Reset(me);

		// Unlock mutex
		#if defined(ALX_OS)
		if (threadSafe) AlxOsMutex_Unlock(&me->mutex);
		#endif

		// Return
		return Alx_Err;
	}

	// Unlock mutex
	#if defined(ALX_OS)
	if (threadSafe) AlxOsMutex_Unlock(&me->mutex);
	#endif

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxTrace_Reset(AlxTrace* me)
{
	// DeInit UART
	if (HAL_UART_DeInit(&me->huart) != HAL_OK) { return Alx_Err; }

	// Clear isInit
	me->isInit = false;

	// Force UART periphery reset
	AlxTrace_Periph_ForceReset(me);

	// Release UART periphery reset
	AlxTrace_Periph_ReleaseReset(me);

	// Init UART
	if (HAL_UART_Init(&me->huart) != HAL_OK) { return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static void AlxTrace_Periph_EnableClk(AlxTrace* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_CLK_ENABLE(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_CLK_ENABLE(); isErr = false; }
	#endif

	if (isErr)
	{
		// We should not get here
	}
}
static void AlxTrace_Periph_DisableClk(AlxTrace* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_CLK_DISABLE(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_CLK_DISABLE(); isErr = false; }
	#endif

	if (isErr)
	{
		// We should not get here
	}
}
static void AlxTrace_Periph_ForceReset(AlxTrace* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_FORCE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_FORCE_RESET(); isErr = false; }
	#endif

	if (isErr)
	{
		// We should not get here
	}
}
static void AlxTrace_Periph_ReleaseReset(AlxTrace* me)
{
	bool isErr = true;

	#ifdef USART1
	if (me->huart.Instance == USART1)	{ __HAL_RCC_USART1_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART2
	if (me->huart.Instance == USART2)	{ __HAL_RCC_USART2_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART3
	if (me->huart.Instance == USART3)	{ __HAL_RCC_USART3_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART4
	if (me->huart.Instance == USART4)	{ __HAL_RCC_USART4_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART5
	if (me->huart.Instance == USART5)	{ __HAL_RCC_USART5_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART6
	if (me->huart.Instance == USART6)	{ __HAL_RCC_USART6_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART7
	if (me->huart.Instance == USART7)	{ __HAL_RCC_USART7_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART8
	if (me->huart.Instance == USART8)	{ __HAL_RCC_USART8_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART9
	if (me->huart.Instance == USART9)	{ __HAL_RCC_USART9_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef USART10
	if (me->huart.Instance == USART10)	{ __HAL_RCC_USART10_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART1
	if (me->huart.Instance == UART1)	{ __HAL_RCC_UART1_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART2
	if (me->huart.Instance == UART2)	{ __HAL_RCC_UART2_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART3
	if (me->huart.Instance == UART3)	{ __HAL_RCC_UART3_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART4
	if (me->huart.Instance == UART4)	{ __HAL_RCC_UART4_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART5
	if (me->huart.Instance == UART5)	{ __HAL_RCC_UART5_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART6
	if (me->huart.Instance == UART6)	{ __HAL_RCC_UART6_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART7
	if (me->huart.Instance == UART7)	{ __HAL_RCC_UART7_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART8
	if (me->huart.Instance == UART8)	{ __HAL_RCC_UART8_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART9
	if (me->huart.Instance == UART9)	{ __HAL_RCC_UART9_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef UART10
	if (me->huart.Instance == UART10)	{ __HAL_RCC_UART10_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART1
	if (me->huart.Instance == LPUART1)	{ __HAL_RCC_LPUART1_RELEASE_RESET(); isErr = false; }
	#endif
	#ifdef LPUART2
	if (me->huart.Instance == LPUART2)	{ __HAL_RCC_LPUART2_RELEASE_RESET(); isErr = false; }
	#endif

	if (isErr)
	{
		// We should not get here
	}
}


#endif	// #if defined(ALX_C_LIB) && ((defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)) && (!defined(ALX_MBED)))
