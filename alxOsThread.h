/**
  ******************************************************************************
  * @file		alxOsThread.h
  * @brief		Auralix C Library - ALX OS Thread Module
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
#ifndef ALX_OS_THREAD_H
#define ALX_OS_THREAD_H

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
#define ALX_OS_THREAD_FILE "alxOsThread.h"

// Assert //
#if defined(_ALX_OS_THREAD_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_THREAD_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_THREAD_FILE, expr)
#elif defined(_ALX_OS_THREAD_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_THREAD_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_THREAD_FILE, expr)
#elif defined(_ALX_OS_THREAD_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_THREAD_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_THREAD_FILE, expr)
#else
	#define ALX_OS_THREAD_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_THREAD_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_THREAD_TRACE(...) ALX_TRACE_STD(ALX_OS_THREAD_FILE, __VA_ARGS__)
#else
	#define ALX_OS_THREAD_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	#if defined(ALX_FREE_RTOS)
	// Parameters
	TaskFunction_t pxTaskCode;
	const char* pcName;
	configSTACK_DEPTH_TYPE usStackDepth;
	void* pvParameters;
	UBaseType_t uxPriority;
	AlxClk_Tick osTick;

	// Variables
	TaskHandle_t* pxCreatedTask;
	#endif

	// Info
	bool wasThreadStarted;
	bool wasCtorCalled;
} AlxOsThread;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxOsThread_Ctor
(
	AlxOsThread* me,
	TaskFunction_t pxTaskCode,
	const char* const pcName,
	const configSTACK_DEPTH_TYPE usStackDepth,
	void* const pvParameters,
	UBaseType_t uxPriority
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
Alx_Status AlxOsThread_Start(AlxOsThread* me);


#endif // #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_OS_THREAD_H
