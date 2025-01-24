/**
  ******************************************************************************
  * @file		alxGlobal.c
  * @brief		Auralix C Library - ALX Global Module
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
#include "alxGlobal.h"
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
  */
void AlxGlobal_DisableIrq(void)
{
	#if defined(__GNUC__)
	//__disable_irq();
	#endif
}

/**
  * @brief
  */
void AlxGlobal_EnableIrq(void)
{
	#if defined(__GNUC__)
	//__enable_irq();
	#endif
}

/**
  * @brief
  * @param[in]	uint64
  * @param[out]	str
  */
void AlxGlobal_Ulltoa(uint64_t uint64, char* str)
{
	uint32_t num1 = uint64 / 1000000000000000UL;
	uint64_t foo1 = uint64 - ((uint64_t)num1 * 1000000000000000UL);
	uint32_t num2 = foo1 / 10000000000UL;
	uint64_t foo2 = foo1 - ((uint64_t)num2 * 10000000000UL);
	uint32_t num3 = foo2 / 100000UL;
	uint64_t foo3 = foo2 - ((uint64_t)num3 * 100000UL);
	uint32_t num4 = foo3 / 1UL;

	if((num1 == 0) && (num2 == 0) && (num3 == 0))
		snprintf (str, 50, "%lu", num4);
	else if((num1 == 0) && (num2 == 0))
		snprintf (str, 50, "%lu%05lu", num3, num4);
	else if(num1 == 0)
		snprintf (str, 50, "%lu%05lu%05lu", num2, num3, num4);
	else
		snprintf (str, 50, "%lu%05lu%05lu%05lu", num1, num2, num3, num4);
}

/**
  * @brief
  * @param[in] val
  * @return
  */
uint32_t AlxGlobal_Ntohl(uint32_t val)
{
	uint32_t ret = (val & 0xff) << 24;
	ret |= (val & 0xff00) << 8;
	ret |= (val & 0xff0000UL) >> 8;
	ret |= (val & 0xff000000UL) >> 24;

	return ret;
}


#endif	// #if defined(ALX_C_LIB)
