/**
  ******************************************************************************
  * @file		alxOsThread.h
  * @brief		Auralix C Library - ALX OS Thread Module
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
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_THREAD_FILE "alxOsThread.h"

// Assert //
#if defined(ALX_OS_THREAD_ASSERT_BKPT_ENABLE)
	#define ALX_OS_THREAD_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_THREAD_FILE, expr)
#elif defined(ALX_OS_THREAD_ASSERT_TRACE_ENABLE)
	#define ALX_OS_THREAD_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_THREAD_FILE, expr)
#elif defined(ALX_OS_THREAD_ASSERT_RST_ENABLE)
	#define ALX_OS_THREAD_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_THREAD_FILE, expr)
#else
	#define ALX_OS_THREAD_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_OS_THREAD_TRACE_ENABLE)
	#define ALX_OS_THREAD_TRACE(...) ALX_TRACE_WRN(ALX_OS_THREAD_FILE, __VA_ARGS__)
#else
	#define ALX_OS_THREAD_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	void (*func)(void*);
	const char* name;
	uint32_t stackLen_byte;
	void* param;
	uint32_t priority;

	// Variables
	#if defined(ALX_FREE_RTOS)
	configSTACK_DEPTH_TYPE stackLen_word;
	TaskHandle_t taskHandle;
	#endif

	// Info
	bool wasThreadStarted;
	bool wasCtorCalled;
} AlxOsThread;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsThread_Ctor
(
	AlxOsThread* me,
	void (*func)(void*),
	const char* name,
	uint32_t stackLen_byte,
	void* param,
	uint32_t priority
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxOsThread_Start(AlxOsThread* me);
void AlxOsThread_Yield(AlxOsThread* me);
void AlxOsThread_Terminate(AlxOsThread* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_THREAD_H
