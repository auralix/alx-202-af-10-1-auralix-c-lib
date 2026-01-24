/**
  ******************************************************************************
  * @file		alxOsMutex.h
  * @brief		Auralix C Library - ALX OS Mutex Module
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
#ifndef ALX_OS_MUTEX_H
#define ALX_OS_MUTEX_H

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
// Preprocessor
//******************************************************************************
#define ALX_OS_MUTEX_FILE "alxOsMutex.h"

// Assert //
#if defined(ALX_OS_MUTEX_ASSERT_BKPT_ENABLE)
	#define ALX_OS_MUTEX_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_MUTEX_FILE, expr)
#elif defined(ALX_OS_MUTEX_ASSERT_TRACE_ENABLE)
	#define ALX_OS_MUTEX_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_MUTEX_FILE, expr)
#elif defined(ALX_OS_MUTEX_ASSERT_RST_ENABLE)
	#define ALX_OS_MUTEX_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_MUTEX_FILE, expr)
#else
	#define ALX_OS_MUTEX_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_OS_MUTEX_TRACE_ENABLE)
	#define ALX_OS_MUTEX_TRACE_WRN(...) ALX_TRACE_WRN(ALX_OS_MUTEX_FILE, __VA_ARGS__)
#else
	#define ALX_OS_MUTEX_TRACE_WRN(...) do{} while (false)
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	StaticSemaphore_t semaphore;
	SemaphoreHandle_t semaphoreHandle;
	#endif
	#if defined(ALX_ZEPHYR)
	struct k_mutex mutex;
	#endif

	// Info
	bool wasCtorCalled;
} AlxOsMutex;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsMutex_Lock(AlxOsMutex* me);
void AlxOsMutex_Unlock(AlxOsMutex* me);
bool AlxOsMutex_IsUnlocked(AlxOsMutex* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_MUTEX_H
