/**
  ******************************************************************************
  * @file		alxHys2.c
  * @brief		Auralix C Library - ALX Hysteresis 2 Module
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
#include "alxHys2.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys2_Ctor
(
	AlxHys2* me,
	float topHigh,
	float topLow,
	float botHigh,
	float botLow
)
{
	// Parameters
	me->topHigh = topHigh;
	me->topLow = topLow;
	me->botHigh = botHigh;
	me->botLow = botLow;

	// SM
	me->st = AlxHys2_StMid;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
AlxHys2_St AlxHys2_Process(AlxHys2* me, float in)
{
	switch(me->st)
	{
		case AlxHys2_StTop:
		{
			// #1 Transition
			if(in <= me->topLow)
				me->st = AlxHys2_StMid;
			// #2 Do
			else
				me->st = AlxHys2_StTop;
			// #3 Return
			return me->st;
		}
		case AlxHys2_StMid:
		{
			// #1 Transition
			if(in >= me->topHigh)
				me->st = AlxHys2_StTop;
			// #2 Transition
			else if(in <= me->botLow)
				me->st = AlxHys2_StBot;
			// #3 Do
			else
				me->st = AlxHys2_StMid;
			// #3 Return
			return me->st;
		}
		case AlxHys2_StBot:
		{
			// #1 Transition
			if(in >= me->botHigh)
				me->st = AlxHys2_StMid;
			// #2 Do
			else
				me->st = AlxHys2_StBot;
			// #3 Return
			return me->st;
		}
		default:
		{
			ALX_HYS2_ASSERT(false); // We should never get here
			break;
		}
	}

	ALX_HYS2_ASSERT(false); // We should never get here
	return ALX_NULL;
}


#endif	// #if defined(ALX_C_LIB)
