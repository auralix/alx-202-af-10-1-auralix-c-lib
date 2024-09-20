/**
  ******************************************************************************
  * @file		alxMemSafe.h
  * @brief		Auralix C Library - ALX Memory Safe Module
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
#ifndef ALX_MEM_SAFE_H
#define ALX_MEM_SAFE_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxMemRaw.h"
#include "alxCrc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MEM_SAFE_MODULE "AlxMemSafe.h"

// Assert //
#if defined(ALX_MEM_SAFE_ASSERT_BKPT_ENABLE)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MEM_SAFE_MODULE, expr)
#elif defined(ALX_MEM_SAFE_ASSERT_TRACE_ENABLE)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MEM_SAFE_MODULE, expr)
#elif defined(ALX_MEM_SAFE_ASSERT_RST_ENABLE)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_RST(ALX_MEM_SAFE_MODULE, expr)
#else
	#define ALX_MEM_SAFE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_MEM_SAFE_TRACE_ENABLE)
	#define ALX_MEM_SAFE_TRACE(...) ALX_TRACE_WRN(ALX_MEM_SAFE_MODULE, __VA_ARGS__)
#else
	#define ALX_MEM_SAFE_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxMemRaw* memRaw;
	AlxCrc* crc;
	uint32_t copyAddrA;
	uint32_t copyAddrB;
	uint32_t copyLen;
	uint32_t copyCrcLen;
	uint32_t copyLenWithCrc;
	bool nonBlockingEnable;
	uint8_t memSafeReadWriteNumOfTries;
	uint8_t memRawReadWriteNumOfTries;
	uint16_t memRawReadWriteTimeout_ms;
	uint8_t* buff1;
	uint32_t buff1Len;
	uint8_t* buff2;
	uint32_t buff2Len;

	// Variables
	uint32_t crcCopyA;
	uint32_t crcCopyB;
	bool isCopyAValid;
	bool isCopyBValid;
	uint32_t crcToWrite;
	bool isReadDone;
	bool isReadErr;
	bool isWriteDone;
	bool isWriteErr;

	// Info
	bool wasCtorCalled;
} AlxMemSafe;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMemSafe_Ctor
(
	AlxMemSafe* me,
	AlxMemRaw* memRaw,
	AlxCrc* crc,
	uint32_t copyAddrA,
	uint32_t copyAddrB,
	uint32_t copyLen,
	bool nonBlockingEnable,
	uint8_t memSafeReadWriteNumOfTries,
	uint8_t memRawReadWriteNumOfTries,
	uint16_t memRawReadWriteTimeout_ms,
	uint8_t* buff1,
	uint32_t buff1Len,
	uint8_t* buff2,
	uint32_t buff2Len
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMemSafe_Read(AlxMemSafe* me, uint8_t* data, uint32_t len);
Alx_Status AlxMemSafe_Write(AlxMemSafe* me, uint8_t* data, uint32_t len);
bool AlxMemSafe_IsReadDone(AlxMemSafe* me);
bool AlxMemSafe_IsReadErr(AlxMemSafe* me);
bool AlxMemSafe_IsWriteDone(AlxMemSafe* me);
bool AlxMemSafe_IsWriteErr(AlxMemSafe* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MEM_SAFE_H
