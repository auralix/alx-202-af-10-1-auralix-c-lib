/**
  ******************************************************************************
  * @file		alxMemSafe.h
  * @brief		Auralix C Library - ALX Memory Safe Module
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
#define ALX_MEM_SAFE_FILE "alxMemSafe.h"

// Assert //
#if defined(_ALX_MEM_SAFE_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MEM_SAFE_FILE, expr)
#elif defined(_ALX_MEM_SAFE_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MEM_SAFE_FILE, expr)
#elif defined(_ALX_MEM_SAFE_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_MEM_SAFE_ASSERT(expr) ALX_ASSERT_RST(ALX_MEM_SAFE_FILE, expr)
#else
	#define ALX_MEM_SAFE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_MEM_SAFE_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_MEM_SAFE_TRACE(...) ALX_TRACE_STD(ALX_MEM_SAFE_FILE, __VA_ARGS__)
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

/**
  * @brief
  * @param[in,out] me
  * @param[in] memRaw
  * @param[in] crc
  * @param[in] copyAddrA
  * @param[in] copyAddrB
  * @param[in] copyLen
  * @param[in] nonBlockingEnable
  * @param[in] memSafeReadWriteNumOfTries
  * @param[in] memRawReadWriteNumOfTries
  * @param[in] memRawReadWriteTimeout_ms
  * @param[in] buff1
  * @param[in] buff1Len
  * @param[in] buff2
  * @param[in] buff2Len
  */
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

/**
  * @brief
  * @param[in,out] me
  * @param[out] data
  * @param[in] len
  */
Alx_Status AlxMemSafe_Read(AlxMemSafe* me, uint8_t* data, uint32_t len);

/**
  * @brief
  * @param[in,out] me
  * @param[in] data
  * @param[in] len
  */
Alx_Status AlxMemSafe_Write(AlxMemSafe* me, uint8_t* data, uint32_t len);

/**
  * @brief
  * @param[in] me
  */
bool AlxMemSafe_IsReadDone(AlxMemSafe* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxMemSafe_IsReadErr(AlxMemSafe* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxMemSafe_IsWriteDone(AlxMemSafe* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxMemSafe_IsWriteErr(AlxMemSafe* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MEM_SAFE_H
