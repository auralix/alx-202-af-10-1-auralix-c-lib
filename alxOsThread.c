/**
  ******************************************************************************
  * @file		alxOsThread.c
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
// Includes
//******************************************************************************
#include "alxOsThread.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsThread_Ctor
(
	AlxOsThread* me,
	TaskFunction_t pxTaskCode,
	const char* const pcName,
	uint32_t usStackDepth_byte,
	void* const pvParameters,
	UBaseType_t uxPriority
)
{
	#if defined(ALX_FREE_RTOS)
	// Parameters
	me->pxTaskCode = pxTaskCode;
	me->pcName = pcName;
	me->usStackDepth_byte = usStackDepth_byte;
	me->pvParameters = pvParameters;
	me->uxPriority = uxPriority;

	// Variables
	me->usStackDepth_word = usStackDepth_byte / 4;	// TV: FreeRTOS stack is mesured in words, 1 word = 4 bytes
	me->pxCreatedTask = NULL;
	#endif

	// Info
	me->wasThreadStarted = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxOsThread_Start(AlxOsThread* me)
{
	// Assert
	ALX_OS_THREAD_ASSERT(me->wasThreadStarted == false);
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);

	// Start
	#if defined(ALX_FREE_RTOS)
	BaseType_t status = xTaskCreate
	(
		me->pxTaskCode,
		me->pcName,
		me->usStackDepth_word,
		me->pvParameters,
		me->uxPriority,
		me->pxCreatedTask
	);
	if (status != pdPASS) { ALX_OS_THREAD_TRACE("Err"); return Alx_Err; }
	#endif

	// Set wasThreadStarted
	me->wasThreadStarted = true;

	// Return
	return Alx_Ok;
}
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)
