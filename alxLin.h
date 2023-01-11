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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LIN_FILE "alxLin.h"

// Assert //
#if defined(_ALX_LIN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LIN_FILE, expr)
#elif defined(_ALX_LIN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LIN_FILE, expr)
#elif defined(_ALX_LIN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_LIN_ASSERT(expr) ALX_ASSERT_RST(ALX_LIN_FILE, expr)
#else
	#define ALX_LIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_LIN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_LIN_TRACE(...) ALX_TRACE_STD(ALX_LIN_FILE, __VA_ARGS__)
	#define ALX_LIN_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_LIN_TRACE(...) do{} while (false)
	#define ALX_LIN_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	uint8_t id;
	uint8_t dataLen;
	uint8_t data[8];
} AlxLin_Payload;

typedef struct
{
	// Parameters
	AlxSerialPort* alxSerialPort;

	// Variables

	// Info
	bool wasCtorCalled;
	bool isInit;
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
Alx_Status AlxLin_Init(AlxLin* me);
Alx_Status AlxLin_DeInit(AlxLin* me);
Alx_Status AlxLin_TxFrame(AlxLin* me, AlxLin_Payload* frame);
Alx_Status AlxLin_RxFrame(AlxLin* me, AlxLin_Payload* frame);
void AlxLin_IrqHandler(AlxLin* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LIN_H
