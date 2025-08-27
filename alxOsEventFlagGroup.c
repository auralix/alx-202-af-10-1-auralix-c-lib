/**
  ******************************************************************************
  * @file		alxOsEventFlagGroup.c
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
#if defined(ALX_FREE_RTOS)
static uint32_t AlxOsEventFlagGroup_GetTimeout_osTick(AlxOsEventFlagGroup* me, uint32_t timeout_ms);
#endif


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
	AlxOsMutex_Ctor(&me->alxOsMutex);
	#endif
	#if defined(ALX_ZEPHYR)
	k_event_init(&me->event);
	#endif

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
  * @param[in]		irqSafe
  * @return
  */
void AlxOsEventFlagGroup_Set(AlxOsEventFlagGroup* me, uint32_t eventFlagsToSet, bool irqSafe)
{
	//------------------------------------------------------------------------------
	// ALX_FREE_RTOS
	//------------------------------------------------------------------------------
	#if defined(ALX_FREE_RTOS)
	if (irqSafe)	// TV: Original TV implementation
	{
		// Lock
		AlxOsMutex_Lock(&me->alxOsMutex);

		// Assert
		ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

		// Local variables
		EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
		EventBits_t eventBits_t = 0;

		// Unlock
		AlxOsMutex_Unlock(&me->alxOsMutex);

		// Set
		eventBits_t = xEventGroupSetBits(eventGroupHandle_t, eventFlagsToSet);

		// Set me->eventGroupHandle_t
		AlxOsMutex_Lock(&me->alxOsMutex);
		me->eventGroupHandle_t = eventGroupHandle_t;
		AlxOsMutex_Unlock(&me->alxOsMutex);
	}
	else		// TV: AB implementation, updated by TV - Not tested
	{
		// Assert
		ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

		// Set
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		BaseType_t result = xEventGroupSetBitsFromISR(me->eventGroupHandle_t, eventFlagsToSet, &xHigherPriorityTaskWoken);
		if (result != pdFAIL)
		{
			// Yield if a higher priority task was woken
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// ALX_ZEPHYR
	//------------------------------------------------------------------------------
	#if defined(ALX_ZEPHYR)

	// Void
	(void)irqSafe;

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Set
	k_event_post(&me->event, eventFlagsToSet);	// AB: Zephyr's k_event_post is inherently ISR-safe
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
	//------------------------------------------------------------------------------
	// ALX_FREE_RTOS
	//------------------------------------------------------------------------------
	#if defined(ALX_FREE_RTOS)	// TV: Original TV implementation

	// Lock
	AlxOsMutex_Lock(&me->alxOsMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;

	// Unlock
	AlxOsMutex_Unlock(&me->alxOsMutex);

	// Clear
	eventBits_t = xEventGroupClearBits(eventGroupHandle_t, eventFlagsToClear);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxOsMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxOsMutex);

	// Return
	return eventBits_t;
	#endif


	//------------------------------------------------------------------------------
	// ALX_ZEPHYR
	//------------------------------------------------------------------------------
	#if defined(ALX_ZEPHYR)	// TV: AB implementation, updated by TV - Not tested

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	uint32_t eventBits = 0;

	// Get current state before clearing
	eventBits = k_event_test(&me->event, UINT32_MAX) & eventFlagsToClear;

	// Clear event flags
	k_event_clear(&me->event, eventFlagsToClear);

	// Return
	return eventBits;
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
	//------------------------------------------------------------------------------
	// ALX_FREE_RTOS
	//------------------------------------------------------------------------------
	#if defined(ALX_FREE_RTOS)	// TV: Original TV implementation

	// Lock
	AlxOsMutex_Lock(&me->alxOsMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;
	uint32_t timeout_osTick = 0;

	// Convert to timeout_osTick
	timeout_osTick = AlxOsEventFlagGroup_GetTimeout_osTick(me, timeout_ms);

	// Unlock
	AlxOsMutex_Unlock(&me->alxOsMutex);

	// Wait
	eventBits_t = xEventGroupWaitBits(eventGroupHandle_t, eventFlagsToWait, clearEventFlagsOnExit, waitForAllEventFlags, timeout_osTick);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxOsMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxOsMutex);

	// Return
	return eventBits_t;
	#endif


	//------------------------------------------------------------------------------
	// ALX_ZEPHYR
	//------------------------------------------------------------------------------
	#if defined(ALX_ZEPHYR)	// TV: AB implementation, updated by TV

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Convert timeout
	k_timeout_t timeout = (timeout_ms == 0) ? K_NO_WAIT : K_MSEC(timeout_ms);

	// Wait for ANY vs ALL.
	// Zephyr's wait functions do not do atomic clear-on-exit, so we handle that manually.
	// Multi-consumer note: this post-return clear is not atomic with the wake.
	// If multiple threads wait on the same bits, one can clear while another is waking,
	// causing missed events.
	uint32_t got = waitForAllEventFlags
		? k_event_wait_all(&me->event, eventFlagsToWait, false, timeout)
		: k_event_wait    (&me->event, eventFlagsToWait, false, timeout);

	// Check
	uint32_t matched = got & eventFlagsToWait;
	if (matched == 0)
	{
		return 0;	// Timeout
	}

	// Post-return clear (not atomic with wake; OK for single-consumer)
	if (clearEventFlagsOnExit)
	{
		k_event_clear(&me->event, matched);
	}

	// Return
	return matched;
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
	//------------------------------------------------------------------------------
	// ALX_FREE_RTOS
	//------------------------------------------------------------------------------
	#if defined(ALX_FREE_RTOS)	// TV: Original TV implementation

	// Lock
	AlxOsMutex_Lock(&me->alxOsMutex);

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	EventGroupHandle_t eventGroupHandle_t = me->eventGroupHandle_t;
	EventBits_t eventBits_t = 0;
	uint32_t timeout_osTick = 0;

	// Convert to timeout_osTick
	timeout_osTick = AlxOsEventFlagGroup_GetTimeout_osTick(me, timeout_ms);

	// Unlock
	AlxOsMutex_Unlock(&me->alxOsMutex);

	// Sync
	eventBits_t = xEventGroupSync(eventGroupHandle_t, eventFlagsToSet, eventFlagsToWait, timeout_osTick);

	// Set me->eventGroupHandle_t
	AlxOsMutex_Lock(&me->alxOsMutex);
	me->eventGroupHandle_t = eventGroupHandle_t;
	AlxOsMutex_Unlock(&me->alxOsMutex);

	// Return
	return eventBits_t;
	#endif


	//------------------------------------------------------------------------------
	// ALX_ZEPHYR
	//------------------------------------------------------------------------------
	#if defined(ALX_ZEPHYR)	// TV: AB implementation, updated by TV - Not tested

	// Assert
	ALX_OS_EVENT_FLAG_GROUP_ASSERT(me->wasCtorCalled == true);

	// Local variables
	uint32_t eventBits = 0;

	// Convert timeout
	k_timeout_t timeout = (timeout_ms == 0) ? K_NO_WAIT : K_MSEC(timeout_ms);

	// Set our flags
	k_event_post(&me->event, eventFlagsToSet);

	// Wait for all flags to be set (sync means wait for ALL)
	eventBits = k_event_wait_all(&me->event, eventFlagsToWait, false, timeout) & eventFlagsToWait;
	if (eventBits == eventFlagsToWait)
	{
		/* Auto-clear barrier mask so next round starts clean
		NOTE: This is NOT atomic with the wake; multiple threads may race to clear.
			  In practice it's OK for a simple barrier if all bits belong to the same round. */
		k_event_clear(&me->event, eventFlagsToWait);
	}

	// Return
	return eventBits;
	#endif
}


//******************************************************************************
// Private Functions
//******************************************************************************
#if defined(ALX_FREE_RTOS)
static uint32_t AlxOsEventFlagGroup_GetTimeout_osTick(AlxOsEventFlagGroup* me, uint32_t timeout_ms)
{
	// Local variables
	uint32_t timeout_osTick = 0;

	// Check if approximation is disabled
	if (me->approxDisable)
	{
		ALX_OS_EVENT_FLAG_GROUP_ASSERT((timeout_ms * 1000) >= (2 * (uint32_t)me->osTick));
		ALX_OS_EVENT_FLAG_GROUP_ASSERT(((timeout_ms * 1000) % (uint32_t)me->osTick) == 0);
	}

	// Convert to timeout_osTick
	timeout_osTick = (timeout_ms * 1000) / (uint32_t)me->osTick;

	// Return
	return timeout_osTick;
}
#endif


#endif	// #if defined(ALX_C_LIB)
