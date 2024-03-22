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
	char str[ALX_TRACE_LEN_MAX] = {};
	va_list args = {};

	va_start(args, format);
	vsnprintf(str, sizeof(str), format, args);
	va_end(args);

	AlxTrace_WriteStr_Callback(me, str);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		file
  * @param[in]		line
  * @param[in]		fun
  * @param[in]		format
  */
void AlxTrace_WriteStd(AlxTrace* me, const char* file, uint32_t line, const char* fun, const char* format, ...)
{
	char str[ALX_TRACE_LEN_MAX] = {};
	va_list args = {};

	AlxGlobal_Ulltoa(AlxTick_Get_ms(&alxTick), str);
	AlxTrace_WriteFormat(me, "trace;%s;%s;%lu;%s;", str, file, line, fun);

	va_start(args, format);
	vsnprintf(str, sizeof(str), format, args);
	va_end(args);

	AlxTrace_WriteFormat(me, "%s\r\n", str);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		smLevel
  * @param[in]		smName
  * @param[in]		stName
  * @param[in]		acName
  */
void AlxTrace_WriteSm(AlxTrace* me, uint8_t smLevel, const char* smName, const char* stName, const char* acName)
{
	if ((smName != NULL) && (stName != NULL) && (acName != NULL))
	{
		char smLevelStr[50] = {};
		AlxTrace_GetSmLevelStr(smLevel, smLevelStr);

		char tickStr[50] = {};
		AlxGlobal_Ulltoa(AlxTick_Get_ms(&alxTick), tickStr);

		AlxTrace_WriteFormat(me, "traceSm;%s;%s%s_%s_%s\r\n", tickStr, smLevelStr, smName, stName, acName);
	}
}

/**
  * @brief
  * @param[in]	smLevel
  * @param[in]	smLevelStr
  */
void AlxTrace_GetSmLevelStr(uint32_t smLevel, char* smLevelStr)
{
	switch (smLevel)
	{
		case 0: strcpy(smLevelStr, ""); break;
		case 1: strcpy(smLevelStr, "____"); break;
		case 2: strcpy(smLevelStr, "________"); break;
		case 3: strcpy(smLevelStr, "____________"); break;
		case 4: strcpy(smLevelStr, "________________"); break;
		case 5: strcpy(smLevelStr, "____________________"); break;
		default: strcpy(smLevelStr, ""); break;
	}
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxTrace_WriteStr_Callback(AlxTrace* me, const char* str)
{
	AlxTrace_WriteStr(me, str);
}


#endif	// #if defined(ALX_C_LIB)
