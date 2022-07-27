/**
  ******************************************************************************
  * @file		alxFiltGlitchBool.c
  * @brief		Auralix C Library - ALX Fliter Glitch Bool Module
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
#include "alxFiltGlitchBool.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchBool_Ctor
(
	AlxFiltGlitchBool* me,
	bool valInitial,
	float stableTrueTime_ms,
	float stableFalseTime_ms
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);

	// Parameters
	me->stableTrueTime_us = (uint64_t)(stableTrueTime_ms * 1000.f);
	me->stableFalseTime_us = (uint64_t)(stableFalseTime_ms * 1000.f);

	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;

	// Info
	me->wasCtorCalled = true;
}
void AlxFiltGlitchBool_Ctor_us
(
	AlxFiltGlitchBool* me,
	bool valInitial,
	uint64_t stableTrueTime_us,
	uint64_t stableFalseTime_us
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);

	// Parameters
	me->stableTrueTime_us = stableTrueTime_us;
	me->stableFalseTime_us = stableFalseTime_us;

	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
bool AlxFiltGlitchBool_Process(AlxFiltGlitchBool* me, bool in)
{
	me->valRaw = in;
	if (me->valRaw != me->valFiltered) // Unstable state detected
	{
		uint64_t unstableTime_us = AlxTimSw_Get_us(&me->tim);

		if (me->valRaw == true) // We are in false state and it becomes unstable, because "in = true"
		{
			if (unstableTime_us > me->stableTrueTime_us)
			{
				me->valFiltered = true; // Change output filtered value
				AlxTimSw_Start(&me->tim);
			}
		}
		else // We are in true state and it becomes unstable, bacause "in = false"
		{
			if (unstableTime_us > me->stableFalseTime_us)
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
