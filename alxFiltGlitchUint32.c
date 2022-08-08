/**
  ******************************************************************************
  * @file		alxFiltGlitchUint32.c
  * @brief		Auralix C Library - ALX Fliter Glitch Uint32 Module
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
#include "alxFiltGlitchUint32.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchUint32_Ctor
(
	AlxFiltGlitchUint32* me,
	uint32_t valInitial,	// Initial output filtered value
	float stableTime_ms		// Time that val must be stable to change output state to new val
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);

	// Parameters
	me->valInitial = valInitial;
	me->stableTime_ms = stableTime_ms;

	// Variables
	me->valOld = valInitial;
	me->valFiltered = valInitial;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxFiltGlitchUint32_Process(AlxFiltGlitchUint32* me, uint32_t valNew)
{
	if (me->valOld != valNew) // Change detected
	{
		me->valOld = valNew;
		AlxTimSw_Start(&me->tim);
	}
	float stableTime_ms = AlxTimSw_Get_us(&me->tim) / 1000.f;
	if (stableTime_ms >= me->stableTime_ms)
	{
		me->valFiltered = valNew; // Change output filtered value
	}
	return me->valFiltered;
}


#endif	// #if defined(ALX_C_LIB)
