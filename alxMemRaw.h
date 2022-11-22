/**
  ******************************************************************************
  * @file		alxMemRaw.h
  * @brief		Auralix C Library - ALX Memory Raw Module
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
#ifndef ALX_MEM_RAW_H
#define ALX_MEM_RAW_H

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
#define ALX_MEM_RAW_FILE "alxMemRaw.h"

// Assert //
#if defined(_ALX_MEM_RAW_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_MEM_RAW_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MEM_RAW_FILE, expr)
#elif defined(_ALX_MEM_RAW_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_MEM_RAW_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MEM_RAW_FILE, expr)
#elif defined(_ALX_MEM_RAW_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_MEM_RAW_ASSERT(expr) ALX_ASSERT_RST(ALX_MEM_RAW_FILE, expr)
#else
	#define ALX_MEM_RAW_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_MEM_RAW_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_MEM_RAW_TRACE(...) ALX_TRACE_STD(ALX_MEM_RAW_FILE, __VA_ARGS__)
#else
	#define ALX_MEM_RAW_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxMemRaw;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxMemRaw_Ctor
(
	AlxMemRaw* me
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
Alx_Status AlxMemRaw_Init(AlxMemRaw* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMemRaw_DeInit(AlxMemRaw* me);

/**
  * @brief
  * @param[in]	me
  * @param[in]	addr
  * @param[out]	data
  * @param[in]	len
  * @param[in]	numOfTries
  * @param[in]	timeout_ms
  * @retval		Alx_Ok
  * @retval		Alx_Err
  */
Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms);

/**
  * @brief
  * @param[in]	me
  * @param[in]	addr
  * @param[in]	data
  * @param[in]	len
  * @param[in]	checkWithReadEnable
  * @param[in]	numOfTries
  * @param[in]	timeout_ms
  * @retval		Alx_Ok
  * @retval		Alx_Err
  */
Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable, uint8_t numOfTries, uint16_t timeout_ms);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MEM_RAW_H
