/**
  ******************************************************************************
  * @file		alxAssert.c
  * @brief		Auralix C Library - ALX Assert Module
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
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in]	file
  * @param[in]	line
  * @param[in]	fun
  */
void ALX_WEAK AlxAssert_Bkpt(const char* file, uint32_t line, const char* fun)
{
	(void)file;
	(void)line;
	(void)fun;

	ALX_BKPT;
}

/**
  * @brief
  * @param[in]	file
  * @param[in]	line
  * @param[in]	fun
  */
void ALX_WEAK AlxAssert_Trace(const char* file, uint32_t line, const char* fun)
{
	AlxTrace_WriteLevel(&alxTrace, ALX_TRACE_LEVEL_FTL, file, line, fun, "ASSERT");
}

/**
  * @brief
  * @param[in]	file
  * @param[in]	line
  * @param[in]	fun
  */
void ALX_WEAK AlxAssert_Rst(const char* file, uint32_t line, const char* fun)
{
	(void)file;
	(void)line;
	(void)fun;
}


#endif	// #if defined(ALX_C_LIB)
