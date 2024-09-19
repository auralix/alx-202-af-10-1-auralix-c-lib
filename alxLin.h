/**
  ******************************************************************************
  * @file		alxLin.h
  * @brief		Auralix C Library - ALX LIN Module
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
#ifndef ALX_LIN_H
#define ALX_LIN_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxSerialPort.h"
#include "alxTimSw.h"
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LIN_FILE "alxLin.h"

// Assert //
#if defined(ALX_LIN_ASSERT_BKPT_ENABLE)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LIN_FILE, expr)
#elif defined(ALX_LIN_ASSERT_TRACE_ENABLE)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LIN_FILE, expr)
#elif defined(ALX_LIN_ASSERT_RST_ENABLE)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_RST(ALX_LIN_FILE, expr)
#else
	#define ALX_LIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_LIN_TRACE_ENABLE)
	#define ALX_LIN_TRACE_WRN(...) ALX_TRACE_WRN(ALX_LIN_FILE, __VA_ARGS__)
#else
	#define ALX_LIN_TRACE_WRN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_LIN_BUFF_LEN 250

	// Parameters
	AlxSerialPort* alxSerialPort;

	// Info
	bool wasCtorCalled;
	bool isInitMaster;
	bool isInitSlave;
} AlxLin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLin_Master_Init(AlxLin* me);
Alx_Status AlxLin_Master_DeInit(AlxLin* me);
Alx_Status AlxLin_Slave_Init(AlxLin* me);
Alx_Status AlxLin_Slave_DeInit(AlxLin* me);
bool AlxLin_Master_IsInit(AlxLin* me);
bool AlxLin_Slave_IsInit(AlxLin* me);
Alx_Status AlxLin_Master_Read(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, uint16_t slaveResponseWaitTime_ms, uint8_t numOfTries, bool variableLenEnable, uint32_t variableLenEnable_maxLen, uint32_t* variableLenEnable_actualLen);
Alx_Status AlxLin_Master_Write(AlxLin* me, uint8_t id, uint8_t* data, uint32_t len, bool variableLenEnable);
Alx_Status AlxLin_Slave_ReadLen(AlxLin* me, uint8_t* id, uint8_t* data, uint32_t len, uint16_t timeout_ms, uint8_t numOfTries, uint16_t rxFifoNumOfEntriesNewCheckWaitTime_ms);
void AlxLin_IrqHandler(AlxLin* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LIN_H
