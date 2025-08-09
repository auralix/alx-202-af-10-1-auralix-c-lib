/**
  ******************************************************************************
  * @file		alxTrace_McuZephyr.c
  * @brief		Auralix C Library - ALX Trace MCU Zephyr Module
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
#include "alxTrace_McuZephyr.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)

#if defined(ALX_ZEPHYR_TRACE_WITH_LOG)
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
LOG_MODULE_REGISTER(alx, LOG_LEVEL_INF);

// Mutex for thread-safe access to accumulation buffer
K_MUTEX_DEFINE(alx_trace_mutex);
#endif

//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxTrace_Ctor
(
	AlxTrace* me
)
{
	// Variables
	me->device = NULL;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
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
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	// Init
	#if !defined(ALX_ZEPHYR_TRACE_WITH_LOG)

	#if DT_NODE_HAS_STATUS(DT_CHOSEN(zephyr_console), okay)
	me->device = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	if (me->device == NULL)
	{
		return Alx_Err;
	}
	#endif

	#endif

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{
	// DeInit
	me->device = NULL;

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	// Write
	#if !defined(ALX_ZEPHYR_TRACE_WITH_LOG)
	if (me->device)
	{
		while (*str)
		{
			uart_poll_out(me->device, *str++);
		}
	}
	#else
	// Static accumulation buffer
	static char accumBuffer[ALX_TRACE_LEN_MAX];
	static size_t accumLen = 0;
	
	// Lock mutex for thread-safe access
	k_mutex_lock(&alx_trace_mutex, K_FOREVER);
	
	// Check if string contains newline
	const char* newlinePos = strpbrk(str, "\r\n");
	
	if (newlinePos != NULL)
	{
		// Copy up to (but not including) the newline
		size_t copyLen = newlinePos - str;
		
		// Check if we have space in the accumulation buffer
		if (accumLen + copyLen < ALX_TRACE_LEN_MAX - 1)
		{
			// Append to accumulation buffer
			memcpy(accumBuffer + accumLen, str, copyLen);
			accumLen += copyLen;
			accumBuffer[accumLen] = '\0';
			
			// Flush the accumulated buffer
			if (accumLen > 0)
			{
				LOG_INF("%s", accumBuffer);
			}
			
			// Reset accumulation
			accumLen = 0;
		}
		else
		{
			// Buffer overflow - flush what we have and log the new part
			if (accumLen > 0)
			{
				LOG_INF("%s", accumBuffer);
			}
			
			// Log the current part (up to newline)
			static char tempBuffer[ALX_TRACE_LEN_MAX];
			size_t tempLen = (copyLen < ALX_TRACE_LEN_MAX - 1) ? copyLen : ALX_TRACE_LEN_MAX - 1;
			memcpy(tempBuffer, str, tempLen);
			tempBuffer[tempLen] = '\0';
			LOG_INF("%s", tempBuffer);
			
			// Reset accumulation
			accumLen = 0;
		}
	}
	else
	{
		// No newline - accumulate the string
		size_t strLen = strlen(str);
		
		if (accumLen + strLen < ALX_TRACE_LEN_MAX - 1)
		{
			// Append to accumulation buffer
			memcpy(accumBuffer + accumLen, str, strLen);
			accumLen += strLen;
			accumBuffer[accumLen] = '\0';
		}
		else
		{
			// Buffer overflow - flush what we have and start new accumulation
			if (accumLen > 0)
			{
				LOG_INF("%s", accumBuffer);
			}
			
			// Start new accumulation with current string
			size_t copyLen = (strLen < ALX_TRACE_LEN_MAX - 1) ? strLen : ALX_TRACE_LEN_MAX - 1;
			memcpy(accumBuffer, str, copyLen);
			accumLen = copyLen;
			accumBuffer[accumLen] = '\0';
		}
	}
	
	// Unlock mutex
	k_mutex_unlock(&alx_trace_mutex);
	#endif

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
