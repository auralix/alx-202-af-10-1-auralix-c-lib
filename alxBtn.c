/**
  ******************************************************************************
  * @file		alxBtn.c
  * @brief		Auralix C Library - ALX Button Module
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
#include "alxBtn.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBtn_Ctor
(
	AlxBtn* me,
	bool valInitial,
	float pressedLongTime_ms,	// min time of long button press [ms]
	float filterTime	// time of stable button status to change it's state [ms]
)
{
	// Ctor
	AlxTimSw_Ctor(&me->timPressed, true);
	AlxFiltGlitchBool_Ctor(&me->filtGlitchBool, false, filterTime, filterTime);

	// Parameters Const
	me->longTime_ms = pressedLongTime_ms;
	me->filterTime = filterTime;

	// Variables
	me->isPressedOld = valInitial;
	me->isPressed			= false;
	me->wasPressed			= false;
	me->wasReleased			= false;
	me->isPressedShort		= false;
	me->wasPressedShort		= false;
	me->isPressedLong		= false;
	me->wasPressedLong		= false;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxBtn_Handle(AlxBtn* me, bool isPressedRaw)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	// #1 Filtered input
	me->isPressed = AlxFiltGlitchBool_Process(&me->filtGlitchBool, isPressedRaw);
	// #2 isPressed - High line
	if(me->isPressed)
	{
		me->wasPressed = true; // WasPressed
		// Check timeout
		if(AlxTimSw_IsTimeout_ms(&me->timPressed, me->longTime_ms)) // Long pressed
		{
			me->isPressedShort	= false;	// IsPressedShort
			me->isPressedLong	= true;		// IsPressedLong
			me->wasPressedLong	= true;		// WasPressedLong
		}
		else // Short pressed
		{
			me->isPressedShort	= true;		// IsPressedShort
			me->isPressedLong	= false;	// IsPressedLong
		}
	}
	// #3 isPressed - Low line
	else
	{
		if (me->isPressedOld) // Falling edge detected
		{
			if (AlxTimSw_IsTimeout_ms(&me->timPressed, me->longTime_ms) == false)
			{
				me->wasPressedShort	= true;		// WasPressedShort
			}
			me->wasReleased		= true;		// WasReleased
			me->isPressedShort	= false;	// IsPressedShort
			me->isPressedLong	= false;	// IsPressedLong
		}
		AlxTimSw_Start(&me->timPressed); // Reset timer
	}
	// #4 Copy value to isPressedOld
	me->isPressedOld = me->isPressed;
}
bool AlxBtn_IsPressed(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->isPressed;
}
bool AlxBtn_WasPressed(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->wasPressed;
}
bool AlxBtn_WasReleased(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->wasReleased;
}
bool AlxBtn_IsPressedShort(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->isPressedShort;
}
bool AlxBtn_WasPressedShort(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->wasPressedShort;
}
bool AlxBtn_IsPressedLong(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->isPressedLong;
}
bool AlxBtn_WasPressedLong(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	return me->wasPressedLong;
}
void AlxBtn_ClearWasPressed(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	me->wasPressed = false;
}
void AlxBtn_ClearWasReleased(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	me->wasReleased = false;
}
void AlxBtn_ClearWasPressedShort(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	me->wasPressedShort = false;
}
void AlxBtn_ClearWasPressedLong(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);

	me->wasPressedLong = false;
}


#endif // #if defined(ALX_C_LIB)
