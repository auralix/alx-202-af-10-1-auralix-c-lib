/**
  ******************************************************************************
  * @file		alxI2c_McuZephyr.h
  * @brief		Auralix C Library - ALX I2C MCU Zephyr Module
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
#ifndef ALX_I2C_MCU_ZEPHYR_H
#define ALX_I2C_MCU_ZEPHYR_H

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
#if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_I2C_BUFF_LEN 256

	// Parameters
	const char* deviceName;

	// Variables
	const struct device* device;

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
	const char* deviceName
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_I2C_MCU_ZEPHYR_H
