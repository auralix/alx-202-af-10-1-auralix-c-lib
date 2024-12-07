/**
  ******************************************************************************
  * @file		alxI2c_McuStm32.h
  * @brief		Auralix C Library - ALX I2C MCU STM32 Module
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
#ifndef ALX_I2C_MCU_STM32_H
#define ALX_I2C_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_STM32F0)
	AlxI2c_Clk_McuStm32F0_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Hsi_8MHz = 0x2000090E,	// TV: TODO, add config for 100ns rise/fall time
	AlxI2c_Clk_McuStm32F0_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Hsi_8MHz = 0x0000020B,
	#endif
	#if defined(ALX_STM32F4)
	AlxI2c_Clk_McuStm32F4_I2cClk_100kHz_Pclk1Apb1_45MHz = 100000,
	AlxI2c_Clk_McuStm32F4_I2cClk_400kHz_Pclk1Apb1_45MHz = 400000,
	#endif
	#if defined(ALX_STM32F7)
	AlxI2c_Clk_McuStm32F7_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_54MHz = 0x10906999,
	AlxI2c_Clk_McuStm32F7_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_54MHz = 0x6000030D,
	AlxI2c_Clk_McuStm32F7_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_54MHz = 0x00A01B56,
	AlxI2c_Clk_McuStm32F7_I2cClk_1MHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_54MHz = 0x00200922,
	#endif
	#if defined(ALX_STM32G4)
	AlxI2c_Clk_McuStm32G4_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_170MHz = 0x40B285C2,	// TV: TODO, test
	AlxI2c_Clk_McuStm32G4_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_170MHz = 0x40621236,	// TV: TODO, test
	#endif
	#if defined(ALX_STM32L0)
	AlxI2c_Clk_McuStm32L0_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_32MHz = 0x00B07CB4,		// TV: TODO, test
	AlxI2c_Clk_McuStm32L0_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_32MHz = 0x0060112F,		// TV: TODO, test
	#endif
	#if defined(ALX_STM32L4)
	AlxI2c_Clk_McuStm32L4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_80MHz = 0x10909CEC,	// TV: TODO, add config for 100ns rise/fall time
	AlxI2c_Clk_McuStm32L4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_80MHz = 0x00702991,
	AlxI2c_Clk_McuStm32L4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_120MHz = 0x307075B1,
	AlxI2c_Clk_McuStm32L4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_120MHz = 0x00B03FDB,
	#endif
	#if defined(ALX_STM32U5)
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_16MHz = 0x00303D5B,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_16MHz = 0x00503D58,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_16MHz = 0x0010061A,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_16MHz = 0x00300617,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_160MHz = 0x30909DEC,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_160MHz = 0x30D29DE4,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_160MHz = 0x00F07BFF,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_160MHz = 0x10F32A82,
	AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_Pclk1Apb1_160MHz_tPRESC_6ns25_tSCLDEL_100ns_tSDADEL_0ns_tSCLH_1125ns_tSCLL_1125ns = 0x00F0B3B3,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk3Apb3_16MHz = 0x00303D5B,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk3Apb3_16MHz = 0x00503D58,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk3Apb3_16MHz = 0x0010061A,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk3Apb3_16MHz = 0x00300617,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk3Apb3_160MHz = 0x30909DEC,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk3Apb3_160MHz = 0x30D29DE4,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk3Apb3_160MHz = 0x00F07BFF,
	AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk3Apb3_160MHz = 0x10F32A82,
	#endif
} AlxI2c_Clk;

typedef struct
{
	// Defines
	#define ALX_I2C_BUFF_LEN 256

	// Parameters
	I2C_TypeDef* i2c;
	AlxIoPin* io_SCL;
	AlxIoPin* io_SDA;
	AlxClk* clk;
	AlxI2c_Clk i2cClk;

	// Variables
	I2C_HandleTypeDef hi2c;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxI2c;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_TypeDef* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxClk* clk,
	AlxI2c_Clk i2cClk
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_I2C_MCU_STM32_H
