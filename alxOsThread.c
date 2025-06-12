/**
  ******************************************************************************
  * @file		alxOsThread.c
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
// Includes
//******************************************************************************
#include "alxOsThread.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		func
  * @param[in]		name
  * @param[in]		stackBuff
  * @param[in]		stackLen_byte
  * @param[in]		param
  * @param[in]		priority
  */
void AlxOsThread_Ctor
(
	AlxOsThread* me,
	void (*func)(void*),
	const char* name,
	#if defined(ALX_ZEPHYR)
	k_thread_stack_t* stackBuff,
	#endif
	uint32_t stackLen_byte,
	void* param,
	int32_t priority
)
{
	// Parameters
	me->func = func;
	me->name = name;
	#if defined(ALX_ZEPHYR)
	me->stackBuff = stackBuff;
	#endif
	me->stackLen_byte = stackLen_byte;
	me->param = param;
	me->priority = priority;

	// Variables
	#if defined(ALX_FREE_RTOS)
	me->taskHandle = NULL;
	me->stackLen_word = stackLen_byte / sizeof(StackType_t);	// TV: FreeRTOS stack is mesured in words, 1 word = sizeof(StackType_t) bytes
	#endif
	#if defined(ALX_ZEPHYR)
	memset(&me->threadHandle, 0, sizeof(me->threadHandle));
	#endif

	// Info
	me->wasCtorCalled = true;
	me->wasStarted = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxOsThread_Start(AlxOsThread* me)
{
	// Assert
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);
	ALX_OS_THREAD_ASSERT(me->wasStarted == false);

	// Start
	#if defined(ALX_FREE_RTOS)
	ALX_OS_THREAD_ASSERT(0 <= me->priority && me->priority < configMAX_PRIORITIES);
	BaseType_t status = xTaskCreate
	(
		(TaskFunction_t)me->func,
		me->name,
		me->stackLen_word,
		me->param,
		(UBaseType_t)me->priority,
		&me->taskHandle
	);
	if (status != pdPASS)
	{
		ALX_OS_THREAD_TRACE_ERR("FAIL: xTaskCreate() status %ld name %s stackLen_word %lu priority %ld", status, me->name, me->stackLen_word, me->priority);
		return Alx_Err;
	}
	#endif
	#if defined(ALX_ZEPHYR)
	k_tid_t threadId = k_thread_create
	(
		&me->threadHandle,			// new_thread
		me->stackBuff,				// stack
		me->stackLen_byte,			// stack_size
		(k_thread_entry_t)me->func,	// entry
		me->param,					// p1
		NULL,						// p2
		NULL,						// p3
		me->priority,				// prio
		0,							// options
		K_NO_WAIT					// delay
	);
	#if defined(CONFIG_THREAD_NAME)
	int32_t status = k_thread_name_set(threadId, me->name);
	if (status != 0)
	{
		ALX_OS_THREAD_TRACE_ERR("FAIL: k_thread_name_set() status %ld name %s stackLen_byte %lu priority %ld", status, me->name, me->stackLen_byte, me->priority);
		return Alx_Err;
	}
	#endif
	#endif

	// Set
	me->wasStarted = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsThread_Yield(AlxOsThread* me)
{
	// Assert
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);
	ALX_OS_THREAD_ASSERT(me->wasStarted == true);

	// Yield
	#if defined(ALX_FREE_RTOS)
	taskYIELD();
	#endif
	#if defined(ALX_ZEPHYR)
	k_yield();
	#endif
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsThread_Terminate(AlxOsThread* me)
{
	// Assert
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);
	ALX_OS_THREAD_ASSERT(me->wasStarted == true);

	// Terminate
	#if defined(ALX_FREE_RTOS)
	vTaskDelete(me->taskHandle);
	#endif
	#if defined(ALX_ZEPHYR)
	// TODO
	ALX_OS_THREAD_ASSERT(false);
	#endif
}


#endif	// #if defined(ALX_C_LIB)
