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
// Private Functions
//******************************************************************************
#if defined(ALX_ZEPHYR)
static void AlxOsThread_Thread(void* p1, void* p2, void* p3);
#endif


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
		&me->threadHandle,						// new_thread
		me->stackBuff,							// stack
		me->stackLen_byte,						// stack_size
		(k_thread_entry_t)AlxOsThread_Thread,	// entry
		me,										// p1
		NULL,									// p2
		NULL,									// p3
		me->priority,							// prio
		0,										// options
		K_NO_WAIT								// delay
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
	// Zephyr thread termination
	k_thread_abort(&me->threadHandle);
	#endif
	me->wasStarted = false;
}
Alx_Status AlxOsThread_Join(AlxOsThread* me, uint32_t timeout_ms)
{
	// Assert
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);
	ALX_OS_THREAD_ASSERT(me->wasStarted == true);

	// Join - wait for thread to finish
	#if defined(ALX_FREE_RTOS)
	// FreeRTOS doesn't have a native join mechanism
	// We need to poll task state and wait for deletion
	TickType_t timeout_ticks = (timeout_ms == UINT32_MAX) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
	TickType_t start_time = xTaskGetTickCount();
	
	while (true)
	{
		// Check if task still exists by trying to get its state
		eTaskState task_state = eTaskGetState(me->taskHandle);
		if (task_state == eDeleted || task_state == eInvalid)
		{
			// Task has finished/been deleted
			return Alx_Ok;
		}
		
		// Check timeout
		if (timeout_ms != UINT32_MAX)
		{
			TickType_t current_time = xTaskGetTickCount();
			if ((current_time - start_time) >= timeout_ticks)
			{
				return Alx_ErrNumOfTries;
			}
		}
		
		// Small delay to avoid busy waiting
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	#endif
	
	#if defined(ALX_ZEPHYR)
	// Zephyr has k_thread_join function
	k_timeout_t timeout = (timeout_ms == UINT32_MAX) ? K_FOREVER : K_MSEC(timeout_ms);
	
	int ret = k_thread_join(&me->threadHandle, timeout);
	if (ret == 0)
	{
		return Alx_Ok;
	}
	else if (ret == -EAGAIN)
	{
		return Alx_ErrNumOfTries;
	}
	else
	{
		return Alx_Err;
	}
	#endif
	me->wasStarted = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
#if defined(ALX_ZEPHYR)
static void AlxOsThread_Thread(void* p1, void* p2, void* p3)
{
	// Void
	(void)p2;
	(void)p3;

	// Prepare me from p1
	AlxOsThread* me = (AlxOsThread*)p1;

	// Call actual user function
	me->func(me->param);
}
#endif


#endif	// #if defined(ALX_C_LIB)
