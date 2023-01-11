/**
  ******************************************************************************
  * @file		alxVeml6040.h
  * @brief		Auralix C Library - ALX Light Sensor VEML604 Module
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
#ifndef ALX_VEML6040_H
#define ALX_VEML6040_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_VEML6040_FILE "alxVeml6040.h"

// Assert //
#if defined(_ALX_VEML6040_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_VEML6040_ASSERT(expr) ALX_ASSERT_BKPT(ALX_VEML6040_FILE, expr)
#elif defined(_ALX_VEML6040_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_VEML6040_ASSERT(expr) ALX_ASSERT_TRACE(ALX_VEML6040_FILE, expr)
#elif defined(_ALX_VEML6040_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_VEML6040_ASSERT(expr) ALX_ASSERT_RST(ALX_VEML6040_FILE, expr)
#else
	#define ALX_VEML6040_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_VEML6040_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_VEML6040_TRACE(...) ALX_TRACE_STD(ALX_VEML6040_FILE, __VA_ARGS__)
	#define ALX_VEML6040_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_VEML6040_TRACE(...) do{} while (false)
	#define ALX_VEML6040_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//******************************************************************************
// Address: 0x00, Reset: 0x01 0x00, Name: CONF
//******************************************************************************
typedef enum
{
	ColorSensor_Enabled		= 0,
	ColorSensor_Disabled	= 1
} AlxVeml6040_RegEnum_0x00_ColorSensorEnable;
typedef enum
{
	AutoMode	= 0,
	ForceMode	= 1
} AlxVeml6040_RegEnum_0x00_ModeSelect;
typedef enum
{
	TriggerDisabled	= 0,
	TriggerEnabled	= 1
} AlxVeml6040_RegEnum_0x00_TriggerEnable;
typedef enum
{
	IntegrationTimeSelect_40_ms		= 0b000,
	IntegrationTimeSelect_80_ms		= 0b001,
	IntegrationTimeSelect_160_ms	= 0b010,
	IntegrationTimeSelect_320_ms	= 0b011,
	IntegrationTimeSelect_640_ms	= 0b100,
	IntegrationTimeSelect_1280_ms	= 0b101
} AlxVeml6040_RegEnum_0x00_IntegrationTimeSelect;
typedef union
{
	struct __attribute__((packed))
	{
		AlxVeml6040_RegEnum_0x00_ColorSensorEnable SD : 1;
		AlxVeml6040_RegEnum_0x00_ModeSelect AF : 1;
		AlxVeml6040_RegEnum_0x00_TriggerEnable TRIG : 1;
		uint8_t unused_3 : 1;  // bit 3 is unused
		AlxVeml6040_RegEnum_0x00_IntegrationTimeSelect IT : 3;
		uint8_t unused_7 : 1;  // bit 7 is unused
	};
	uint16_t raw;
} AlxVeml6040_RegVal_0x00_CONF;


//******************************************************************************
// Address: 0x08, Reset: 0x00 0x00, Name: R_DATA
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t R_DATA_L;
		uint8_t R_DATA_H;
	};
	uint16_t raw;
} AlxVeml6040_RegVal_0x08_R_DATA;


//******************************************************************************
// Address: 0x09, Reset: 0x00 0x00, Name: G_DATA
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t G_DATA_L;
		uint8_t G_DATA_H;
	};
	uint16_t raw;
} AlxVeml6040_RegVal_0x09_G_DATA;


//******************************************************************************
// Address: 0x0A, Reset: 0x00 0x00, Name: B_DATA
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t B_DATA_L;
		uint8_t B_DATA_H;
	};
	uint16_t raw;
} AlxVeml6040_RegVal_0x0A_B_DATA;


//******************************************************************************
// Address: 0x0B, Reset: 0x00 0x00, Name: W_DATA
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t W_DATA_L;
		uint8_t W_DATA_H;
	};
	uint16_t raw;
} AlxVeml6040_RegVal_0x0B_W_DATA;


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
	AlxVeml6040_RegVal_0x00_CONF val;
}AlxVeml6040_Reg_0x00_CONF;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxVeml6040_RegVal_0x08_R_DATA val;
} AlxVeml6040_Reg_0x08_R_DATA;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxVeml6040_RegVal_0x09_G_DATA val;
} AlxVeml6040_Reg_0x09_G_DATA;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxVeml6040_RegVal_0x0A_B_DATA val;
} AlxVeml6040_Reg_0x0A_B_DATA;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxVeml6040_RegVal_0x0B_W_DATA val;
} AlxVeml6040_Reg_0x0B_W_DATA;


//******************************************************************************
// Register Group Structures
//******************************************************************************


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxVeml6040_Reg_0x00_CONF	_0x00_CONF;
	AlxVeml6040_Reg_0x08_R_DATA	_0x08_R_DATA;
	AlxVeml6040_Reg_0x09_G_DATA	_0x09_G_DATA;
	AlxVeml6040_Reg_0x0A_B_DATA	_0x0A_B_DATA;
	AlxVeml6040_Reg_0x0B_W_DATA	_0x0B_W_DATA;
} AlxVeml6040_Reg;




//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxI2c* i2c;

	// Parameters
	uint8_t i2cAddr;
	uint8_t i2cMemAddr;
	AlxI2c_Master_MemAddrLen i2cMemAddrLen;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxVeml6040_Reg reg;
	uint16_t r_16bit;
	uint16_t g_16bit;
	uint16_t b_16bit;
	uint16_t w_16bit;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxVeml6040;


//******************************************************************************
// Constructor
//******************************************************************************

	/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		i2cAddr
  * @param[in]		i2cCheckWithRead
  * @param[in]		i2cNumOfTries
  * @param[in]		i2cTimeout_ms
  */
void AlxVeml6040_Ctor
(
	AlxVeml6040* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_Init(AlxVeml6040* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_DeInit(AlxVeml6040* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxVeml6040_IsInit(AlxVeml6040* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		R_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetR_raw(AlxVeml6040* me, uint16_t* R_raw);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		G_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetG_raw(AlxVeml6040* me, uint16_t* G_raw);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		B_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetB_raw(AlxVeml6040* me, uint16_t* B_raw);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		W_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetW_raw(AlxVeml6040* me, uint16_t* W_raw);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_VEML6040_H
