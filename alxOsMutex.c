/**
  ******************************************************************************
  * @file		alxOsMutex.c
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
// Includes
//******************************************************************************
#include "alxOsMutex.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
)
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	me->semaphoreHandle = xSemaphoreCreateBinaryStatic(&me->semaphore);
	xSemaphoreGive(me->semaphoreHandle);
	#endif
	#if defined(ALX_ZEPHYR)
	k_mutex_init(&me->mutex);
	#endif

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Lock(AlxOsMutex* me)
{
	#if defined(ALX_FREE_RTOS)
	xSemaphoreTake(me->semaphoreHandle, portMAX_DELAY);
	#endif
	#if defined(ALX_ZEPHYR)
	k_mutex_lock(&me->mutex, K_FOREVER);
	#endif
}

/**
  * @brief
  * @param[in] me
  */
void AlxOsMutex_Unlock(AlxOsMutex* me)
{
	#if defined(ALX_FREE_RTOS)
	xSemaphoreGive(me->semaphoreHandle);
	#endif
	#if defined(ALX_ZEPHYR)
	k_mutex_unlock(&me->mutex);
	#endif
}

/**
  * @brief
  * @param[in]	me
  * @retval		false
  * @retval		true
  */
bool AlxOsMutex_IsUnlocked(AlxOsMutex* me)
{
	#if defined(ALX_FREE_RTOS)
	return uxSemaphoreGetCount(me->semaphoreHandle);
	#endif
	#if defined(ALX_ZEPHYR)
	if (k_mutex_lock(&me->mutex, K_NO_WAIT) == 0)
	{
		k_mutex_unlock(&me->mutex);
		return true;
	}
	return false;
	#endif
}


#endif	// #if defined(ALX_C_LIB)
