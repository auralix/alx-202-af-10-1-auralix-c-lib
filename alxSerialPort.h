/**
  ******************************************************************************
  * @file		alxSerialPort.h
  * @brief		Auralix C Library - ALX Serial Port Module
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
#ifndef ALX_SERIAL_PORT_H
#define ALX_SERIAL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4)
#include "alxSerialPort_McuStm32.h"

#else
typedef struct { bool dummy; } AlxSerialPort;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_SERIAL_PORT_FILE "alxSerialPort.h"

// Assert //
#if defined(_ALX_SERIAL_PORT_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_SERIAL_PORT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_SERIAL_PORT_FILE, expr)
#elif defined(_ALX_SERIAL_PORT_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_SERIAL_PORT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_SERIAL_PORT_FILE, expr)
#elif defined(_ALX_SERIAL_PORT_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_SERIAL_PORT_ASSERT(expr) ALX_ASSERT_RST(ALX_SERIAL_PORT_FILE, expr)
#else
	#define ALX_SERIAL_PORT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_SERIAL_PORT_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_SERIAL_PORT_TRACE(...) ALX_TRACE_STD(ALX_SERIAL_PORT_FILE, __VA_ARGS__)
#else
	#define ALX_SERIAL_PORT_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Init(AlxSerialPort* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_DeInit(AlxSerialPort* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Read(AlxSerialPort* me, uint8_t* data, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		str
  * @param[in]		delim
  * @param[in]		maxLen
  * @param[out]		numRead
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_ReadStrUntil(AlxSerialPort* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_Write(AlxSerialPort* me, uint8_t data);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		data
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_WriteMulti(AlxSerialPort* me, const uint8_t* data, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSerialPort_WriteStr(AlxSerialPort* me, const char* str);

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSerialPort_Foreground_Handle(AlxSerialPort* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SERIAL_PORT_H
