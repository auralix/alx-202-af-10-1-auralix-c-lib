/**
  ******************************************************************************
  * @file		alxTpl.c
  * @brief		Auralix C Library - ALX Template Module
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
#include "alxTpl.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTpl_Ctor
(
	AlxTpl* me,
	bool valInitial,
	float stableTrueTime_ms,
	float stableFalseTime_ms
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);

	// Parameters
	me->stableTrueTime_ms = stableTrueTime_ms;
	me->stableFalseTime_ms = stableFalseTime_ms;

	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
bool AlxTpl_Process(AlxTpl* me, bool in)
{
	me->valRaw = in;
	if (me->valRaw != me->valFiltered) // Unstable state detected
	{
		float unstableTime_ms = AlxTimSw_Get_us(&me->tim) / 1000.f;

		if (me->valRaw == true) // We are in false state and it becomes unstable, because "in = true"
		{
			if (unstableTime_ms > me->stableTrueTime_ms)
			{
				me->valFiltered = true; // Change output filtered value
				AlxTimSw_Start(&me->tim);
			}
		}
		else // We are in true state and it becomes unstable, bacause "in = false"
		{
			if (unstableTime_ms > me->stableFalseTime_ms)
			{
				me->valFiltered = false; // Change output filtered value
				AlxTimSw_Start(&me->tim);
			}
		}
	}
	else
	{
		AlxTimSw_Start(&me->tim);
	}

	return me->valFiltered;
}


#endif // #if defined(ALX_C_LIB)
