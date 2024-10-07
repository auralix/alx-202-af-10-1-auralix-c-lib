/**
  ******************************************************************************
  * @file		alxI2c.h
  * @brief		Auralix C Library - ALX I2C Module
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
#ifndef ALX_I2C_H
#define ALX_I2C_H

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

#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
#include "alxI2c_McuStm32.h"

#elif defined(ALX_LPC55S6X)
#include "alxI2c_McuLpc55S6x.h"

#elif defined(ALX_LPC80X) || defined(ALX_LPC84X)
#include "alxI2c_McuLpc80x.h"

#elif defined(ALX_ZEPHYR)
#include "alxI2c_McuZephyr.h"

#else
typedef struct { bool dummy; } AlxI2c;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_I2C_FILE "alxI2c.h"

// Assert //
#if defined(ALX_I2C_ASSERT_BKPT_ENABLE)
	#define ALX_I2C_ASSERT(expr) ALX_ASSERT_BKPT(ALX_I2C_FILE, expr)
#elif defined(ALX_I2C_ASSERT_TRACE_ENABLE)
	#define ALX_I2C_ASSERT(expr) ALX_ASSERT_TRACE(ALX_I2C_FILE, expr)
#elif defined(ALX_I2C_ASSERT_RST_ENABLE)
	#define ALX_I2C_ASSERT(expr) ALX_ASSERT_RST(ALX_I2C_FILE, expr)
#else
	#define ALX_I2C_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_I2C_TRACE_ENABLE)
	#define ALX_I2C_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
	#define ALX_I2C_TRACE(...) ALX_TRACE_WRN(ALX_I2C_FILE, __VA_ARGS__)
	#define ALX_I2C_TRACE_VRB(...) ALX_TRACE_VRB(ALX_I2C_FILE, __VA_ARGS__)
#else
	#define ALX_I2C_TRACE_FORMAT(...) do{} while (false)
	#define ALX_I2C_TRACE(...) do{} while (false)
	#define ALX_I2C_TRACE_VRB(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxI2c_Master_MemAddrLen_8bit = 1,
	AlxI2c_Master_MemAddrLen_16bit = 2
} AlxI2c_Master_MemAddrLen;


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxI2c_Init(AlxI2c* me);
Alx_Status AlxI2c_DeInit(AlxI2c* me);
Alx_Status AlxI2c_Master_StartRead(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartReadStop(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartReadMemStop(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWrite(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWriteStop(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWriteMemStop_Single(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t data, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_StartWriteMemStop_Multi(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, const uint8_t* data, uint16_t len, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_Stop(AlxI2c* me, uint16_t timeout_ms);
Alx_Status AlxI2c_Master_IsSlaveReady(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_I2C_H
