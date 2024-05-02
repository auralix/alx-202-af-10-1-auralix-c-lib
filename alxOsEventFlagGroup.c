/**
  ******************************************************************************
  * @file		alxOsEventFlagGroup.h
  * @brief		Auralix C Library - ALX OS Event Flag Group Module
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
#include "alxOsEventFlagGroup.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxOsEventFlagGroup_GetTimeout_osTick(AlxOsEventFlagGroup* me, uint32_t timeout_ms);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsEventFlagGroup_Ctor
(
	AlxOsEventFlagGroup* me,
	AlxClk_Tick osTick,
	bool approxDisable
)
{
	// Parameters
	me->osTick = osTick;
	me->approxDisable = approxDisable;

	// Variables
	#if defined(ALX_FREE_RTOS)
	me->eventGroupHandle_t = xEventGroupCreate();	// TV: EventGroup is created once and won't be deleted during a program
	#endif
	AlxOsMutex_Ctor(&me->alxMutex);

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToSet
  * @return
  */
uint32_t AlxOsEventFlagGroup_Set(AlxOsEventFlagGroup* me, uint32_t eventFlagsToSet)
{
	#if defined(ALX_FREE_RTOS)


	// Lock mutex
	AlxOsMutex_Lock(&me->alxMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;

	// Unlock mutex
	AlxOsMutex_Unlock(&me->alxMutex);

	// Set
	eventBits_t = xEventGroupSetBits(eventGroupHandle_t, eventFlagsToSet);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return eventBits_t;


	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToClear
  * @return
  */
uint32_t AlxOsEventFlagGroup_Clear(AlxOsEventFlagGroup* me, uint32_t eventFlagsToClear)
{
	#if defined(ALX_FREE_RTOS)


	// Lock mutex
	AlxOsMutex_Lock(&me->alxMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;

	// Unlock mutex
	AlxOsMutex_Unlock(&me->alxMutex);

	// Clear
	eventBits_t = xEventGroupClearBits(eventGroupHandle_t, eventFlagsToClear);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return eventBits_t;


	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToWait
  * @param[in]		clearEventFlagsOnExit
  * @param[in]		waitForAllEventFlags
  * @param[in]		timeout_ms
  * @return
  */
uint32_t AlxOsEventFlagGroup_Wait(AlxOsEventFlagGroup* me, uint32_t eventFlagsToWait, bool clearEventFlagsOnExit, bool waitForAllEventFlags, uint32_t timeout_ms)
{
	#if defined(ALX_FREE_RTOS)


	// Lock mutex
	AlxOsMutex_Lock(&me->alxMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;
	uint32_t timeout_osTick = 0;

	// Convert to timeout_osTick
	timeout_osTick = AlxOsEventFlagGroup_GetTimeout_osTick(me, timeout_ms);

	// Unlock mutex
	AlxOsMutex_Unlock(&me->alxMutex);

	// Wait
	eventBits_t = xEventGroupWaitBits(eventGroupHandle_t, eventFlagsToWait, clearEventFlagsOnExit, waitForAllEventFlags, timeout_osTick);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return eventBits_t;


	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToSet
  * @param[in]		eventFlagsToWait
  * @param[in]		timeout_ms
  * @return
  */
uint32_t AlxOsEventFlagGroup_Sync(AlxOsEventFlagGroup* me, uint32_t eventFlagsToSet, uint32_t eventFlagsToWait, uint32_t timeout_ms)
{
	#if defined(ALX_FREE_RTOS)


	// Lock mutex
	AlxOsMutex_Lock(&me->alxMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;
	uint32_t timeout_osTick = 0;

	// Convert to timeout_osTick
	timeout_osTick = AlxOsEventFlagGroup_GetTimeout_osTick(me, timeout_ms);

	// Unlock mutex
	AlxOsMutex_Unlock(&me->alxMutex);

	// Sync
	eventBits_t = xEventGroupSync(eventGroupHandle_t, eventFlagsToSet, eventFlagsToWait, timeout_osTick);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxMutex);

	// Return
	return eventBits_t;


	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxOsEventFlagGroup_GetTimeout_osTick(AlxOsEventFlagGroup* me, uint32_t timeout_ms)
{
	#if defined(ALX_FREE_RTOS)


	// Check if approximation is disabled
	if (me->approxDisable)
	{
		ALX_OS_EVENT_FLAG_GROUP_ASSERT((timeout_ms * 1000) >= (2 * (uint32_t)me->osTick));
		ALX_OS_EVENT_FLAG_GROUP_ASSERT(((timeout_ms * 1000) % (uint32_t)me->osTick) == 0);
	}

	// Convert to timeout_osTick
	uint32_t timeout_osTick = (timeout_ms * 1000) / (uint32_t)me->osTick;

	// Return
	return timeout_osTick;


	#endif
}


#endif	// #if defined(ALX_C_LIB)
