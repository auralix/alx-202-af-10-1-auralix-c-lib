/**
  ******************************************************************************
  * @file		alxMath.c
  * @brief		Auralix C Library - ALX Mathematics Module
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
#include "alxMath.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMath_Ctor(AlxMath* me)
{
	// Variables
	me->data.count = 0;
	me->data.sum = 0;
	me->data.avg = 0;
	me->data.min = 4294967295;
	me->data.max = 0;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
AlxMath_Data AlxMath_Process(AlxMath* me, uint32_t in)
{
	// Assert
	ALX_MATH_ASSERT(me->wasCtorCalled == true);

	// Update count
	me->data.count++;

	// Update sum
	me->data.sum = me->data.sum + in;

	// Update average
	me->data.avg = me->data.sum / me->data.count;

	// Update min
	if (in < me->data.min)
	{
		me->data.min = in;
	}

	// Update max
	if (in > me->data.max)
	{
		me->data.max = in;
	}

	// Return
	return me->data;
}


#endif	// #if defined(ALX_C_LIB)
