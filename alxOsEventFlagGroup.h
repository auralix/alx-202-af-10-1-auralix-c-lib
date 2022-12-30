/**
  ******************************************************************************
  * @file		alxOsEventFlagGroup.h
  * @brief		Auralix C Library - ALX OS Event Flag Group Module
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
#ifndef ALX_OS_EVENT_FLAG_GROUP_H
#define ALX_OS_EVENT_FLAG_GROUP_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxOsMutex.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_EVENT_FLAG_GROUP_FILE "alxOsEventFlagGroup.h"

// Assert //
#if defined(_ALX_OS_EVENT_FLAG_GROUP_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_EVENT_FLAG_GROUP_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_EVENT_FLAG_GROUP_FILE, expr)
#elif defined(_ALX_OS_EVENT_FLAG_GROUP_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_EVENT_FLAG_GROUP_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_EVENT_FLAG_GROUP_FILE, expr)
#elif defined(_ALX_OS_EVENT_FLAG_GROUP_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_EVENT_FLAG_GROUP_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_EVENT_FLAG_GROUP_FILE, expr)
#else
	#define ALX_OS_EVENT_FLAG_GROUP_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_EVENT_FLAG_GROUP_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_EVENT_FLAG_GROUP_TRACE(...) ALX_TRACE_STD(ALX_OS_EVENT_FLAG_GROUP_FILE, __VA_ARGS__)
#else
	#define ALX_OS_EVENT_FLAG_GROUP_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxClk_Tick osTick;
	bool approxDisable;

	// Variables
	EventGroupHandle_t eventGroupHandle_t;
	AlxOsMutex alxMutex;

	// Info
	bool wasCtorCalled;
} AlxOsEventFlagGroup;


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
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToSet
  * @return
  */
uint32_t AlxOsEventFlagGroup_Set(AlxOsEventFlagGroup* me, uint32_t eventFlagsToSet);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToClear
  * @return
  */
uint32_t AlxOsEventFlagGroup_Clear(AlxOsEventFlagGroup* me, uint32_t eventFlagsToClear);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToWait
  * @param[in]		clearEventFlagsOnExit
  * @param[in]		waitForAllEventFlags
  * @param[in]		timeout_ms
  * @return
  */
uint32_t AlxOsEventFlagGroup_Wait(AlxOsEventFlagGroup* me, uint32_t eventFlagsToWait, bool clearEventFlagsOnExit, bool waitForAllEventFlags, uint32_t timeout_ms);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		eventFlagsToSet
  * @param[in]		eventFlagsToWait
  * @param[in]		timeout_ms
  * @return
  */
uint32_t AlxOsEventFlagGroup_Sync(AlxOsEventFlagGroup* me, uint32_t eventFlagsToSet, uint32_t eventFlagsToWait, uint32_t timeout_ms);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_EVENT_FLAG_GROUP_H
