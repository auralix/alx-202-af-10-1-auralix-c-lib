/**
  ******************************************************************************
  * @file		alxTmp1075.h
  * @brief		Auralix C Library - ALX Temperature Sensor TMP1075 Module
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
#ifndef ALX_TMP1075_H
#define ALX_TMP1075_H

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
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TMP1075_FILE "alxTmp1075.h"

// Assert //
#if defined(ALX_TMP1075_ASSERT_BKPT_ENABLE)
	#define ALX_TMP1075_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TMP1075_FILE, expr)
#elif defined(ALX_TMP1075_ASSERT_TRACE_ENABLE)
	#define ALX_TMP1075_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TMP1075_FILE, expr)
#elif defined(ALX_TMP1075_ASSERT_RST_ENABLE)
	#define ALX_TMP1075_ASSERT(expr) ALX_ASSERT_RST(ALX_TMP1075_FILE, expr)
#else
	#define ALX_TMP1075_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_TMP1075_TRACE_ENABLE)
	#define ALX_TMP1075_TRACE_WRN(...) ALX_TRACE_WRN(ALX_TMP1075_FILE, __VA_ARGS__)
	#define ALX_TMP1075_TRACE_INF(...) ALX_TRACE_INF(ALX_TMP1075_FILE, __VA_ARGS__)
#else
	#define ALX_TMP1075_TRACE_WRN(...) do{} while (false)
	#define ALX_TMP1075_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************

// R0: Temp  //
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t T;
	};
	uint16_t raw;
} AlxTmp1075_RegVal_R0_Temp;

// R1: Configuration  //
typedef enum
{
	ShutdownMode_Disabled	= 0,
	ShutdownMode_Enabled	= 1
} AlxTmp1075_R1_ShutdownModeEnable;
typedef enum
{
	AlertPinModeSelect_Comperator	= 0,
	AlertPinModeSelect_Interrupt	= 1
} AlxTmp1075_R1_AlertPinModeSelect;
typedef enum
{
	AlertPinPolaritySelect_ActiveLow	= 0,
	AlertPinPolaritySelect_ActiveHigh	= 1
} AlxTmp1075_R1_AlertPinPolaritySelect;
typedef enum
{
	ConsecutiveFaultsToTriggerAlert_1	= 0b00,
	ConsecutiveFaultsToTriggerAlert_2	= 0b01,
	ConsecutiveFaultsToTriggerAlert_3	= 0b10,
	ConsecutiveFaultsToTriggerAlert_4	= 0b11
} AlxTmp1075_R1_ConsecutiveFaultsToTriggerAlert;
typedef enum
{
	ConversionRateSelect_27ms5	= 0b00,
	ConversionRateSelect_55_ms	= 0b01,
	ConversionRateSelect_110_ms	= 0b10,
	ConversionRateSelect_220_ms	= 0b11
} AlxTmp1075_R1_ConversionRateSelect;
typedef enum
{
	OneShotConversionMode_Disabled	= 0,
	OneShotConversionMode_Enabled	= 1
} AlxTmp1075_R1_OneShotConversionMode;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_7 : 8;
		AlxTmp1075_R1_ShutdownModeEnable SD : 1;
		AlxTmp1075_R1_AlertPinModeSelect TM : 1;
		AlxTmp1075_R1_AlertPinPolaritySelect POL : 1;
		AlxTmp1075_R1_ConsecutiveFaultsToTriggerAlert F : 2;
		AlxTmp1075_R1_ConversionRateSelect R : 2;
		AlxTmp1075_R1_OneShotConversionMode OS:1;
	};
	uint16_t raw;
	} AlxTmp1075_RegVal_R1_Config;

// R2: Low limit  //
typedef union
	{
		struct __attribute__((packed))
		{
			uint16_t L;
		};
		uint16_t raw;
	} AlxTmp1075_RegVal_R2_LimitLow;

// R3: High limit  //
typedef union
	{
		struct __attribute__((packed))
		{
			uint16_t H;
		};
		uint16_t raw;
	} AlxTmp1075_RegVal_R3_LimitHigh;

// R4: Device ID register  //
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t DID;
	};
	uint16_t raw;
} AlxTmp1075_RegVal_R4_DeviceId;


//******************************************************************************
// Register Group Values Unions
//******************************************************************************


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxTmp1075_RegVal_R0_Temp val;
} AlxTmp1075_Reg_R0_Temp;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxTmp1075_RegVal_R1_Config val;
} AlxTmp1075_Reg_R1_Config;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxTmp1075_RegVal_R2_LimitLow val;
} AlxTmp1075_Reg_R2_LimitLow;
typedef struct
	{
		uint8_t addr;
		uint8_t len;
		AlxTmp1075_RegVal_R3_LimitHigh val;
	} AlxTmp1075_Reg_R3_LimitHigh;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxTmp1075_RegVal_R4_DeviceId val;
} AlxTmp1075_Reg_R4_DeviceId;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxTmp1075_Reg_R0_Temp R0_Temp;
	AlxTmp1075_Reg_R1_Config R1_Config;
	AlxTmp1075_Reg_R2_LimitLow R2_LimitLow;
	AlxTmp1075_Reg_R3_LimitHigh R3_LimitHigh;
	AlxTmp1075_Reg_R4_DeviceId R4_DeviceId;
} AlxTmp1075_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_TMP1075_DEG_C_PER_BIT 0.0625

	// Parameters
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxTmp1075_Reg reg;
	int16_t temp_raw;
	float temp_degC;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTmp1075;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTmp1075_Ctor
(
	AlxTmp1075* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTmp1075_Init(AlxTmp1075* me);
Alx_Status AlxTmp1075_DeInit(AlxTmp1075* me);
bool AlxTmp1075_IsInit(AlxTmp1075* me);
float AlxTmp1075_GetTemp_degC(AlxTmp1075* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TMP1075_H
