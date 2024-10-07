/**
  ******************************************************************************
  * @file		alxFifo.h
  * @brief		Auralix C Library - ALX FIFO Module
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
#ifndef ALX_FIFO_H
#define ALX_FIFO_H

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
// Preprocessor
//******************************************************************************
#define ALX_FIFO_FILE "alxFifo.h"

// Assert //
#if defined(ALX_FIFO_ASSERT_BKPT_ENABLE)
	#define ALX_FIFO_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FIFO_FILE, expr)
#elif defined(ALX_FIFO_ASSERT_TRACE_ENABLE)
	#define ALX_FIFO_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FIFO_FILE, expr)
#elif defined(ALX_FIFO_ASSERT_RST_ENABLE)
	#define ALX_FIFO_ASSERT(expr) ALX_ASSERT_RST(ALX_FIFO_FILE, expr)
#else
	#define ALX_FIFO_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_FIFO_TRACE_ENABLE)
	#define ALX_FIFO_TRACE(...) ALX_TRACE_WRN(ALX_FIFO_FILE, __VA_ARGS__)
#else
	#define ALX_FIFO_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t* buff;
	uint32_t buffLen;

	// Variables
	uint32_t head;
	uint32_t tail;
	uint32_t numOfEntries;
	bool isFull;
	bool isEmpty;

	// Info
	bool wasCtorCalled;
} AlxFifo;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFifo_Ctor
(
	AlxFifo* me,
	uint8_t* buff,
	uint32_t buffLen
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxFifo_Flush(AlxFifo* me);
Alx_Status AlxFifo_Read(AlxFifo* me, uint8_t* data, uint32_t len);
Alx_Status AlxFifo_ReadStrUntil(AlxFifo* me, char* str, const char* delim, uint32_t maxLen, uint32_t* numRead);
Alx_Status AlxFifo_Write(AlxFifo* me, uint8_t data);
Alx_Status AlxFifo_WriteMulti(AlxFifo* me, const uint8_t* data, uint32_t len);
Alx_Status AlxFifo_WriteStr(AlxFifo* me, const char* str);
uint32_t AlxFifo_GetNumOfEntries(AlxFifo* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FIFO_H
