/**
  ******************************************************************************
  * @file		alxI2c_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX I2C MCU LPC55S6X Module
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
#ifndef ALX_I2C_MCU_LPC55S6X_H
#define ALX_I2C_MCU_LPC55S6X_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTimSw.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxI2c_Clk_McuLpc55S6x_BitRate_100kHz = 100000U,
	AlxI2c_Clk_McuLpc55S6x_BitRate_400kHz = 400000U,
} AlxI2c_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin* io_SCL;
	AlxIoPin* io_SDA;

	// Parameters
	I2C_Type* i2c;
	AlxI2c_Clk clk;

	// Variables
	i2c_master_config_t i2cConfig;
	AlxTimSw tim;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxI2c;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_Type* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
);


#endif // #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_I2C_MCU_LPC55S6X_H
