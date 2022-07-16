/**
  ******************************************************************************
  * @file		alxHys1.c
  * @brief		Auralix C Library - ALX Hysteresis 1 Module
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
#include "alxHys1.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys1_Ctor
(
	AlxHys1* me,
	float high,
	float low
)
{
	// Parameters
	me->high = high;
	me->low = low;

	// SM
	me->st = AlxHys1_StHigh;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
AlxHys1_St AlxHys1_Process(AlxHys1* me, float in)
{
	switch(me->st)
	{
		case AlxHys1_StHigh:
		{
			// #1 Transition
			if(in <= me->low)
				me->st = AlxHys1_StLow;
			// #2 Do
			else
				me->st = AlxHys1_StHigh;
			// #3 Return
			return me->st;
		}
		case AlxHys1_StLow:
		{
			// #1 Transition
			if(in >= me->high)
				me->st = AlxHys1_StHigh;
			// #2 Do
			else
				me->st = AlxHys1_StLow;
			// #3 Return
			return me->st;
		}
		default:
		{
			ALX_HYS1_ASSERT(false); // We should never get here
			break;
		}
	}

	ALX_HYS1_ASSERT(false); // We should never get here
	return ALX_NULL;
}
