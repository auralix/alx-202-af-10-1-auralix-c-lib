/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc84x.h
  * @brief		Auralix C Library - ALX IO Pin MCU LPC84X Module
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
// Include Guard
//******************************************************************************
#ifndef ALX_IO_PIN_MCU_LPC84X_H
#define ALX_IO_PIN_MCU_LPC84X_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC84X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	// Movable
	AlxIoPin_SwmFunc_USART0_TXD = 0U,
	AlxIoPin_SwmFunc_USART0_RXD = 1U,
	AlxIoPin_SwmFunc_USART0_RTS = 2U,
	AlxIoPin_SwmFunc_USART0_CTS = 3U,
	AlxIoPin_SwmFunc_USART0_SCLK = 4U,
	AlxIoPin_SwmFunc_USART1_TXD = 5U,
	AlxIoPin_SwmFunc_USART1_RXD = 6U,
	AlxIoPin_SwmFunc_USART1_RTS = 7U,
	AlxIoPin_SwmFunc_USART1_CTS = 8U,
	AlxIoPin_SwmFunc_USART1_SCLK = 9U,

	AlxIoPin_SwmFunc_USART2_TXD = 10U,
	AlxIoPin_SwmFunc_USART2_RXD = 11U,
	AlxIoPin_SwmFunc_USART2_RTS = 12U,
	AlxIoPin_SwmFunc_USART2_CTS = 13U,
	AlxIoPin_SwmFunc_USART2_SCLK = 14U,

	AlxIoPin_SwmFunc_SPI0_SCK = 15U,
	AlxIoPin_SwmFunc_SPI0_MOSI = 16U,
	AlxIoPin_SwmFunc_SPI0_MISO = 17U,
	AlxIoPin_SwmFunc_SPI0_SSEL0 = 18U,
	AlxIoPin_SwmFunc_SPI0_SSEL1 = 19U,
	AlxIoPin_SwmFunc_SPI0_SSEL2 = 20U,
	AlxIoPin_SwmFunc_SPI0_SSEL3 = 21U,

	AlxIoPin_SwmFunc_SPI1_SCK = 22U,
	AlxIoPin_SwmFunc_SPI1_MOSI = 23U,
	AlxIoPin_SwmFunc_SPI1_MISO = 24U,
	AlxIoPin_SwmFunc_SPI1_SSEL0 = 25U,
	AlxIoPin_SwmFunc_SPI1_SSEL1 = 26U,
	AlxIoPin_SwmFunc_SCT_PIN0 = 27U,
	AlxIoPin_SwmFunc_SCT_PIN1 = 28U,
	AlxIoPin_SwmFunc_SCT_PIN2 = 29U,
	AlxIoPin_SwmFunc_SCT_PIN3 = 30U,
	AlxIoPin_SwmFunc_SCT_OUT0 = 31U,
	AlxIoPin_SwmFunc_SCT_OUT1 = 32U,
	AlxIoPin_SwmFunc_SCT_OUT2 = 33U,
	AlxIoPin_SwmFunc_SCT_OUT3 = 34U,
	AlxIoPin_SwmFunc_SCT_OUT4 = 35U,
	AlxIoPin_SwmFunc_SCT_OUT5 = 36U,
	AlxIoPin_SwmFunc_SCT_OUT6 = 37U,
	AlxIoPin_SwmFunc_I2C1_SDA = 38U,
	AlxIoPin_SwmFunc_I2C1_SCL = 39U,
	AlxIoPin_SwmFunc_I2C2_SDA = 40U,
	AlxIoPin_SwmFunc_I2C2_SCL = 41U,
	AlxIoPin_SwmFunc_I2C3_SDA = 42U,
	AlxIoPin_SwmFunc_I2C3_SCL = 43U,

	AlxIoPin_SwmFunc_ACMP_OUT = 44U,
	AlxIoPin_SwmFunc_CLKOUT = 45U,
	AlxIoPin_SwmFunc_GPIO_INT_BMAT = 46U,

	AlxIoPin_SwmFunc_USART3_TXD = 47U,
	AlxIoPin_SwmFunc_USART3_RXD = 48U,
	AlxIoPin_SwmFunc_USART3_SCLK = 49U,

	AlxIoPin_SwmFunc_USART4_TXD = 50U,
	AlxIoPin_SwmFunc_USART4_RXD = 51U,
	AlxIoPin_SwmFunc_USART4_SCLK = 52U,
	AlxIoPin_SwmFunc_T0_MAT_CHN0 = 53U,
	AlxIoPin_SwmFunc_T0_MAT_CHN1 = 54U,
	AlxIoPin_SwmFunc_T0_MAT_CHN2 = 55U,
	AlxIoPin_SwmFunc_T0_MAT_CHN3 = 56U,
	AlxIoPin_SwmFunc_T0_CAP_CHN0 = 57U,
	AlxIoPin_SwmFunc_T0_CAP_CHN1 = 58U,
	AlxIoPin_SwmFunc_T0_CAP_CHN2 = 59U,

	// Fixed
	AlxIoPin_SwmFunc_ACMP_INPUT1 = 60U,
	AlxIoPin_SwmFunc_ACMP_INPUT2 = 61U,
	AlxIoPin_SwmFunc_ACMP_INPUT3 = 62U,
	AlxIoPin_SwmFunc_ACMP_INPUT4 = 63U,
	AlxIoPin_SwmFunc_ACMP_INPUT5 = 64U,

	AlxIoPin_SwmFunc_SWCLK = 65U,
	AlxIoPin_SwmFunc_SWDIO = 66U,
	AlxIoPin_SwmFunc_XTALIN = 67U,
	AlxIoPin_SwmFunc_XTALOUT = 68U,
	AlxIoPin_SwmFunc_RESETN = 69U,
	AlxIoPin_SwmFunc_CLKIN = 70U,
	AlxIoPin_SwmFunc_VDDCMP = 71U,
	AlxIoPin_SwmFunc_I2C0_SDA = 72U,
	AlxIoPin_SwmFunc_I2C0_SCL = 73U,

	AlxIoPin_SwmFunc_ADC_CHN0 = 74U,
	AlxIoPin_SwmFunc_ADC_CHN1 = 75U,
	AlxIoPin_SwmFunc_ADC_CHN2 = 76U,
	AlxIoPin_SwmFunc_ADC_CHN3 = 77U,
	AlxIoPin_SwmFunc_ADC_CHN4 = 78U,
	AlxIoPin_SwmFunc_ADC_CHN5 = 79U,
	AlxIoPin_SwmFunc_ADC_CHN6 = 80U,
	AlxIoPin_SwmFunc_ADC_CHN7 = 81U,
	AlxIoPin_SwmFunc_ADC_CHN8 = 82U,
	AlxIoPin_SwmFunc_ADC_CHN9 = 83U,
	AlxIoPin_SwmFunc_ADC_CHN10 = 84U,
	AlxIoPin_SwmFunc_ADC_CHN11 = 85U,

	AlxIoPin_SwmFunc_DAC_OUT0 = 86U,
	AlxIoPin_SwmFunc_DAC_OUT1 = 87U,
	AlxIoPin_SwmFunc_CAPT_X0 = 88U,
	AlxIoPin_SwmFunc_CAPT_X1 = 89U,
	AlxIoPin_SwmFunc_CAPT_X2 = 90U,
	AlxIoPin_SwmFunc_CAPT_X3 = 91U,
	AlxIoPin_SwmFunc_CAPT_X4 = 92U,
	AlxIoPin_SwmFunc_CAPT_X5 = 93U,
	AlxIoPin_SwmFunc_CAPT_X6 = 94U,
	AlxIoPin_SwmFunc_CAPT_X7 = 95U,
	AlxIoPin_SwmFunc_CAPT_X8 = 96U,
	AlxIoPin_SwmFunc_CAPT_YL = 97U,
	AlxIoPin_SwmFunc_CAPT_YH = 98U,

	// No SWM Function
	AlxIoPin_SwmFunc_NO = 255U
} AlxIoPin_SwmFunc;

typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	AlxIoPin_SwmFunc swmFunc;
	uint32_t mode;
	bool isOpenDrain;
	//bool hysteresis;
	//bool invertInput;
	//uint32_t i2cMode;
	//uint32_t sMode;
	//uint32_t clkDiv;
	//bool dacMode;
	bool dir;
	bool val;

	// Variables
	bool swmFunc_isMovable;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_SwmFunc swmFunc,
	uint32_t mode,
	bool isOpenDrain,
	bool dir,
	bool val
);


#endif // #if defined(ALX_C_LIB) && defined(ALX_LPC84X)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_IO_PIN_MCU_LPC84X_H
