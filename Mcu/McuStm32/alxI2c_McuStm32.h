/**
  ******************************************************************************
  * @file		alxI2c_McuStm32.h
  * @brief		Auralix C Library - ALX I2C MCU STM32 Module
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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxI2c_Clk_100kHz = 100U,
	AlxI2c_Clk_400kHz = 400U
} AlxI2c_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin* io_SCL;
	AlxIoPin* io_SDA;

	// Parameters
	AlxI2c_Clk clk;

	// Variables
	I2C_HandleTypeDef hi2c;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxI2c;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		io_SCL
  * @param[in]		io_SDA
  * @param[in]		clk
  */
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_TypeDef* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_I2C_MCU_STM32_H
