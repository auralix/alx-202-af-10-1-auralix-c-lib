/**
  ******************************************************************************
  * @file		alxOsMutex.h
  * @brief		Auralix C Library - ALX OS Mutex Module
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
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_MUTEX_FILE "alxOsMutex.h"

// Assert //
#if defined(_ALX_OS_MUTEX_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_MUTEX_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_MUTEX_FILE, expr)
#elif defined(_ALX_OS_MUTEX_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_MUTEX_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_MUTEX_FILE, expr)
#elif defined(_ALX_OS_MUTEX_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_MUTEX_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_MUTEX_FILE, expr)
#else
	#define ALX_OS_MUTEX_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_MUTEX_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_MUTEX_TRACE(...) ALX_TRACE_STD(ALX_OS_MUTEX_FILE, __VA_ARGS__)
#else
	#define ALX_OS_MUTEX_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	SemaphoreHandle_t mutex;
	#endif

	// Info
	bool wasCtorCalled;
} AlxOsMutex;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Lock(AlxOsMutex* me);

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Unlock(AlxOsMutex* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxOsMutex_IsMutexUnlocked(AlxOsMutex* me);	// TV: Not tested


#endif // #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_OS_MUTEX_H
