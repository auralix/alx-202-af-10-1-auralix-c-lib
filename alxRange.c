/**
  ******************************************************************************
  * @file		alxRange.c
  * @brief		Auralix C Library - ALX Range Module
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
#include "alxRange.h"


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRange_CheckUint8(uint8_t val, uint8_t valMin, uint8_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckUint16(uint16_t val, uint16_t valMin, uint16_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckUint32(uint32_t val, uint32_t valMin, uint32_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckUint64(uint64_t val, uint64_t valMin, uint64_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt8(int8_t val, int8_t valMin, int8_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt16(int16_t val, int16_t valMin, int16_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt32(int32_t val, int32_t valMin, int32_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckInt64(int64_t val, int64_t valMin, int64_t valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckFloat(float val, float valMin, float valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
Alx_Status AlxRange_CheckDouble(double val, double valMin, double valMax)
{
	ALX_RANGE_ASSERT(valMin <= valMax);

	if (val < valMin)
	{
		return Alx_ErrOutOfRangeMin;
	}
	else if (val > valMax)
	{
		return Alx_ErrOutOfRangeMax;
	}

	return Alx_Ok;
}
/*Alx_Status AlxRange_CheckArr(void* val, uint32_t valLenMax)
{
	ALX_RANGE_ASSERT(0 < valLenMax);

	uint32_t valLen = strlen(val);
	if (valLen > (valLenMax - 1))
		return Alx_ErrOutOfRangeLen;
	else
		return Alx_Ok;
}*/	// TO DO
Alx_Status AlxRange_CheckStr(char* val, uint32_t valLenMax)
{
	ALX_RANGE_ASSERT(0 <= valLenMax);

	uint32_t valLen = strlen(val);
	if (valLen > valLenMax )
		return Alx_ErrOutOfRangeLen;
	else
		return Alx_Ok;
}
