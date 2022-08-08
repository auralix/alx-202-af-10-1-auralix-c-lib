/**
  ******************************************************************************
  * @file		alxOsMutex.c
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
// Includes
//******************************************************************************
#include "alxOsMutex.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
)
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	me->mutex = xSemaphoreCreateBinary();	// MF: Mutex is created once and won't be deleted during a program
	xSemaphoreGive(me->mutex);
	#endif

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsMutex_Lock(AlxOsMutex* me)
{
	// Lock Mutex
	#if defined(ALX_FREE_RTOS)
	xSemaphoreTake(me->mutex, portMAX_DELAY);
	#endif
}
void AlxOsMutex_Unlock(AlxOsMutex* me)
{
	// Unlock Mutex
	#if defined(ALX_FREE_RTOS)
	xSemaphoreGive(me->mutex);
	#endif
}
bool AlxOsMutex_IsMutexUnlocked(AlxOsMutex* me)
{
	// Get Status
	#if defined(ALX_FREE_RTOS)
	return uxSemaphoreGetCount(me->mutex);
	#endif
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)
