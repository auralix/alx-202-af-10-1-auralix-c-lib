﻿/**
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
void AlxTrace_WriteStr_Callback(AlxTrace* me, const char* str, bool threadSafe);


//******************************************************************************
// Functions
//******************************************************************************
#if defined(ALX_PC) || defined(ALX_MBED)

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		threadSafe
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_Init(AlxTrace* me, bool threadSafe)
{
	// Void
	(void)me;
	(void)threadSafe;

	// Return
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		threadSafe
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_DeInit(AlxTrace* me, bool threadSafe)
{
	// Void
	(void)me;
	(void)threadSafe;

	// Return
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @param[in]		threadSafe
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str, bool threadSafe)
{
	// Void
	(void)me;
	(void)threadSafe;

	// Write
	printf(str);

	// Return
	return Alx_Ok;
}
#endif

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		threadSafe
  * @param[in]		format
  */
void AlxTrace_WriteFormat(AlxTrace* me, bool threadSafe, const char* format, ...)
{
	char buff[256] = {0};
	va_list args = {0};

	va_start(args, format);
	vsnprintf(buff, 256, format, args);
	va_end(args);

	AlxTrace_WriteStr_Callback(me, buff, threadSafe);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		file
  * @param[in]		line
  * @param[in]		fun
  * @param[in]		threadSafe
  * @param[in]		format
  */
void AlxTrace_WriteStd(AlxTrace* me, const char* file, uint32_t line, const char* fun, bool threadSafe, const char* format, ...)
{
	char buff[256] = {0};
	va_list args = {0};

	AlxGlobal_Uint64ToStr(AlxTick_Get_ms(&alxTick), buff);
	AlxTrace_WriteFormat(me, threadSafe, "trace;%s;%s;%lu;%s;", buff, file, line, fun);

	va_start(args, format);
	vsnprintf(buff, 256, format, args);
	va_end(args);

	AlxTrace_WriteFormat(me, threadSafe, "%s\r\n", buff);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		smLevel
  * @param[in]		smName
  * @param[in]		stName
  * @param[in]		acName
  * @param[in]		threadSafe
  */
void AlxTrace_WriteSm(AlxTrace* me, uint8_t smLevel, const char* smName, const char* stName, const char* acName, bool threadSafe)
{
	if ((smName != NULL) && (stName != NULL) && (acName != NULL))
	{
		char smLevelStr[50] = {0};
		AlxTrace_GetSmLevelStr(smLevel, smLevelStr);

		char tickStr[50] = {0};
		AlxGlobal_Uint64ToStr(AlxTick_Get_ms(&alxTick), tickStr);

		AlxTrace_WriteFormat(me, threadSafe, "traceSm;%s;%s%s_%s_%s\r\n", tickStr, smLevelStr, smName, stName, acName);
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
ALX_WEAK void AlxTrace_WriteStr_Callback(AlxTrace* me, const char* str, bool threadSafe)
{
	AlxTrace_WriteStr(me, str, threadSafe);
}


#endif	// #if defined(ALX_C_LIB)
