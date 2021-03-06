/**
  ******************************************************************************
  * @file		alxGlobal.c
  * @brief		Auralix C Library - ALX Global Module
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
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Functions
//******************************************************************************
void AlxGlobal_DisableIrq(void)
{
	#if defined(ALX_MBED)
	#else
		#if defined(__GNUC__)
		__disable_irq();
		#endif
	#endif
}
void AlxGlobal_EnableIrq(void)
{
	#if defined(ALX_MBED)
	#else
		#if defined(__GNUC__)
		__enable_irq();
		#endif
	#endif
}
void AlxGlobal_Uint64ToStr(uint64_t uint64, char* str)
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
Alx_Status AlxGlobal_BoundUint32(uint32_t* valPtr, uint32_t valMin, uint32_t valMax)
{
	if (valMin <= valMax)
	{
		if (*valPtr < valMin)
			*valPtr = valMin;
		else if (*valPtr > valMax)
			*valPtr = valMax;
		else
			return Alx_Ok;
	}

	return Alx_Err;
}
Alx_Status AlxGlobal_BoundFloat(float* valPtr, float valMin, float valMax)
{
	if (valMin <= valMax)
	{
		if (*valPtr < valMin)
			*valPtr = valMin;
		else if (*valPtr > valMax)
			*valPtr = valMax;
		else
			return Alx_Ok;
	}

	return Alx_Err;
}
uint32_t AlxGlobal_Round(float val)
{
	uint32_t dummy = val;

	if ((val - dummy) >= 0.5)		// MF: 0.4444445 if we want to raound up
		dummy += 1;

	return dummy;
}
bool AlxGlobal_IsOutOfRangeUint8(uint8_t val, uint8_t valMin, uint8_t valMax)
{
	if ((valMin > val) || (val > valMax))
		return true;
	else
		return false;
}
uint32_t AlxGlobal_Ntohl(uint32_t val)
{
	uint32_t ret = (val & 0xff) << 24;
	ret |= (val & 0xff00) << 8;
	ret |= (val & 0xff0000UL) >> 8;
	ret |= (val & 0xff000000UL) >> 24;

	return ret;
}
