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
  * @param[in]		stackLen_byte
  * @param[in]		param
  * @param[in]		priority
  */
void AlxOsThread_Ctor
(
	AlxOsThread* me,
	void (*func)(void*),
	const char* name,
	uint32_t stackLen_byte,
	void* param,
	uint32_t priority
)
{
	// Parameters
	me->func = func;
	me->name = name;
	me->stackLen_byte = stackLen_byte;
	me->param = param;
	me->priority = priority;

	// Variables
	#if defined(ALX_FREE_RTOS)
	me->stackLen_word = stackLen_byte / 4;	// TV: FreeRTOS stack is mesured in words, 1 word = 4 bytes
	me->taskHandle = NULL;
	#endif

	// Info
	me->wasThreadStarted = false;
	me->wasCtorCalled = true;
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
	ALX_OS_THREAD_ASSERT(me->wasThreadStarted == false);
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);

	// Start
	#if defined(ALX_FREE_RTOS)
	BaseType_t status = xTaskCreate
	(
		(TaskFunction_t)me->func,
		me->name,
		me->stackLen_word,
		me->param,
		(UBaseType_t)me->priority,
		me->taskHandle
	);
	if (status != pdPASS) { ALX_OS_THREAD_TRACE("Err"); return Alx_Err; }
	#endif

	// Set wasThreadStarted
	me->wasThreadStarted = true;

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
	ALX_OS_THREAD_ASSERT(me->wasThreadStarted == true);
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);

	// Yield
	#if defined(ALX_FREE_RTOS)
	taskYIELD();
	#endif
}


#endif	// #if defined(ALX_C_LIB)
