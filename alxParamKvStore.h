/**
  ******************************************************************************
  * @file		alxParamKvStore.h
  * @brief		Auralix C Library - ALX Parameter Key-Value Store Module
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
#ifndef ALX_PARAM_KV_STORE_H
#define ALX_PARAM_KV_STORE_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFs.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_KV_STORE_FILE "alxParamKvStore.h"

// Assert //
#if defined(ALX_PARAM_KV_STORE_ASSERT_BKPT_ENABLE)
	#define ALX_PARAM_KV_STORE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_KV_STORE_FILE, expr)
#elif defined(ALX_PARAM_KV_STORE_ASSERT_TRACE_ENABLE)
	#define ALX_PARAM_KV_STORE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_KV_STORE_FILE, expr)
#elif defined(ALX_PARAM_KV_STORE_ASSERT_RST_ENABLE)
	#define ALX_PARAM_KV_STORE_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_KV_STORE_FILE, expr)
#else
	#define ALX_PARAM_KV_STORE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PARAM_KV_STORE_TRACE_ENABLE)
	#define ALX_PARAM_KV_STORE_TRACE(...) ALX_TRACE_STD(ALX_PARAM_KV_STORE_FILE, __VA_ARGS__)
	#define ALX_PARAM_KV_STORE_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_PARAM_KV_STORE_TRACE(...) do{} while (false)
	#define ALX_PARAM_KV_STORE_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxFs* fs;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxParamKvStore;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamKvStore_Ctor
(
	AlxParamKvStore* me,
	AlxFs* fs
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxParamKvStore_Init(AlxParamKvStore* me);
Alx_Status AlxParamKvStore_DeInit(AlxParamKvStore* me);
Alx_Status AlxParamKvStore_Get(AlxParamKvStore* me, const char* key, void* data, uint32_t lenMax, uint32_t* lenActual);
Alx_Status AlxParamKvStore_Set(AlxParamKvStore* me, const char* key, void* data, uint32_t len);
Alx_Status AlxParamKvStore_Remove(AlxParamKvStore* me, const char* key);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_KV_STORE_H
