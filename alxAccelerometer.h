/**
  ******************************************************************************
  * @file		alxAccelerometer.h
  * @brief		Auralix C Library - ALX Accelerometer Module
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
#ifndef ALX_ACCELEROMETER_H
#define ALX_ACCELEROMETER_H

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
#if defined(ALX_C_LIB)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	ALX_ACC_DEVICE_ADXL355,
	ALX_ACC_DEVICE_ADXL357,
	ALX_ACC_DEVICE_A352,
	ALX_ACC_DEVICE_Max
} AlxAccDevice;

typedef enum
{
	ALX_ACC_RANGE_0,
	ALX_ACC_RANGE_1,
	ALX_ACC_RANGE_2,
	ALX_ACC_RANGE_Max
} AlxAccRange;

typedef enum
{
	ALX_ACC_SYNC_MODE_MEAS_CLK,
	ALX_ACC_SYNC_MODE_PLL
} AlxAccSyncMode;

typedef struct __attribute__((packed))
{
	float x;
	float y;
	float z;
	float temp;
} AccDataPoint;


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ACCELEROMETER_H
