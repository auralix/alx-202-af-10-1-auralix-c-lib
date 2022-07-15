/**
  ******************************************************************************
  * @file		alxBound.c
  * @brief		Auralix C Library - ALX Bound Module
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
#include "alxBound.h"


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxBound_Uint8(uint8_t* val, uint8_t valMin, uint8_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Uint16(uint16_t* val, uint16_t valMin, uint16_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Uint32(uint32_t* val, uint32_t valMin, uint32_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Uint64(uint64_t* val, uint64_t valMin, uint64_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int8(int8_t* val, int8_t valMin, int8_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int16(int16_t* val, int16_t valMin, int16_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int32(int32_t* val, int32_t valMin, int32_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Int64(int64_t* val, int64_t valMin, int64_t valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Float(float* val, float valMin, float valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
Alx_Status AlxBound_Double(double* val, double valMin, double valMax)
{
	ALX_BOUND_ASSERT(valMin <= valMax);

	if (*val < valMin)
	{
		*val = valMin;
		return Alx_ErrBoundMin;
	}
	else if (*val > valMax)
	{
		*val = valMax;
		return Alx_ErrBoundMax;
	}

	return Alx_Ok;
}
/*Alx_Status AlxBound_Arrr(char* valBounded, char* val, uint32_t valLenMax)
{
	ALX_BOUND_ASSERT(0 < valLenMax);

	uint32_t valLen = strlen(val);
	if (valLen >= (valLenMax - 1))
	{
		memcpy(valBounded, val, valLenMax - 1);
		valBounded[valLenMax - 1] = 0;	// Terminate string
		return Alx_ErrBoundLen;
	}
	else
	{
		strcpy(valBounded, val);
		return Alx_Ok;
	}
}*/	// TO DO
Alx_Status AlxBound_Str(char* valBounded, char* val, uint32_t valLenMax)
{
	ALX_BOUND_ASSERT(0 < valLenMax);

	uint32_t valLen = strlen(val);
	if (valLen > (valLenMax - 1))
	{
		memcpy(valBounded, val, valLenMax - 1);
		valBounded[valLenMax - 1] = 0;	// Terminate string
		return Alx_ErrBoundLen;
	}
	else
	{
		strcpy(valBounded, val);
		return Alx_Ok;
	}
}
