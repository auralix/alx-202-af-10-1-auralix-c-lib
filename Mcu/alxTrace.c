/**
  ******************************************************************************
  * @file		alxTrace.c
  * @brief		Auralix C Library - ALX Trace Module
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
#include "alxTrace.h"
#include "alxRtc_Global.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Variables
//******************************************************************************
AlxTrace alxTrace = {0};


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxTrace_WriteStr_Callback(AlxTrace* me, const char* str);
AlxRtc_DateTime AlxTrace_GetRtcDateTime_Callback(AlxTrace* me);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		format
  */
void AlxTrace_WriteFormat(AlxTrace* me, const char* format, ...)
{
	// Prepare formatStr
	char formatStr[ALX_TRACE_LEN_MAX] = {};
	va_list args = {};
	va_start(args, format);
	vsnprintf(formatStr, sizeof(formatStr), format, args);
	va_end(args);

	// Trace
	AlxTrace_WriteStr_Callback(me, formatStr);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		level
  * @param[in]		file
  * @param[in]		line
  * @param[in]		fun
  * @param[in]		format
  */
void AlxTrace_WriteLevel(AlxTrace* me, uint8_t level, const char* file, uint32_t line, const char* fun, const char* format, ...)
{
	//------------------------------------------------------------------------------
	// Prepare dateTimeStr
	//------------------------------------------------------------------------------
	char dateTimeStr[32] = "";
	AlxRtc_DateTime dateTime = AlxTrace_GetRtcDateTime_Callback(me);
	sprintf
	(
		dateTimeStr,
		"20%02u-%02u-%02u %02u:%02u:%02u.%03u",
		dateTime.yr,
		dateTime.mo,
		dateTime.day,
		dateTime.hr,
		dateTime.min,
		dateTime.sec,
		dateTime.ms
	);


	//------------------------------------------------------------------------------
	// Prepare levelStr
	//------------------------------------------------------------------------------
	const char* levelStr = "";
	switch (level)
	{
		case ALX_TRACE_LEVEL_FTL: levelStr = "FTL"; break;
		case ALX_TRACE_LEVEL_ERR: levelStr = "ERR"; break;
		case ALX_TRACE_LEVEL_WRN: levelStr = "WRN"; break;
		case ALX_TRACE_LEVEL_INF: levelStr = "INF"; break;
		case ALX_TRACE_LEVEL_DBG: levelStr = "DBG"; break;
		case ALX_TRACE_LEVEL_VRB: levelStr = "VRB"; break;
		default: levelStr = ""; break;
	}


	//------------------------------------------------------------------------------
	// Prepare formatStr
	//------------------------------------------------------------------------------
	char formatStr[ALX_TRACE_LEN_MAX] = {};
	va_list args = {};
	va_start(args, format);
	vsnprintf(formatStr, sizeof(formatStr), format, args);
	va_end(args);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	if ((ALX_TRACE_LEVEL_FTL <= level) && (level <= ALX_TRACE_LEVEL_WRN))
	{
		AlxTrace_WriteFormat(me, "[%s] [%s] [%s:%lu %s] %s\r\n", dateTimeStr, levelStr, file, line, fun, formatStr);
	}
	else if ((ALX_TRACE_LEVEL_INF <= level) && (level <= ALX_TRACE_LEVEL_VRB))
	{
		AlxTrace_WriteFormat(me, "[%s] [%s] %s\r\n", dateTimeStr, levelStr, formatStr);
	}
	else
	{
		// ALX_TRACE_LEVEL_OFF
	}
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxTrace_WriteStr_Callback(AlxTrace* me, const char* str)
{
	AlxTrace_WriteStr(me, str);
}
ALX_WEAK AlxRtc_DateTime AlxTrace_GetRtcDateTime_Callback(AlxTrace* me)
{
	// Get tickTime_ms
	uint64_t tickTime_ms = AlxTick_Get_ms(&alxTick);

	// Increment unixTime_ms
	uint64_t unixTime_ms = 946684800000 + tickTime_ms;	 // Add offset: Date and time (GMT): Saturday, January 1, 2000 0:00:00

	// Get dateTime
	AlxRtc_DateTime dateTime = AlxRtc_UnixTimeMsToDateTime(unixTime_ms);

	// Return
	return dateTime;
}


#endif	// #if defined(ALX_C_LIB)
