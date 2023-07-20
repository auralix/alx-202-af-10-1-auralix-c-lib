/**
  ******************************************************************************
  * @file		alxAvg.c
  * @brief		Auralix C Library - ALX Average Module
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
#include "alxAvg.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_CMSIS_DSP)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAvg_Ctor
(
	AlxAvg* me,
	float* buff,
	uint32_t buffLen,
	uint32_t shiftThreshold
)
{
	// Parameters
	me->buff = buff;
	me->buffLen = buffLen;
	me->shiftThreshold = shiftThreshold;

	// Variables
	me->buffIndex = 0;
	me->shiftCount = 0;
	me->avg = 0;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
float AlxAvg_Process(AlxAvg* me, float in)
{
	// Assert
	ALX_AVG_ASSERT(me->wasCtorCalled == true);

	// Add new sample to buffer
	me->buff[me->buffIndex] = in;

	// Increment buffer index
	me->buffIndex++;

	// Rewind buffer index if necessary
	me->buffIndex = me->buffIndex % me->buffLen;

	// Increment shift counter
	me->shiftCount++;

	// If shift threashold reached (enough new samples were shifted inside buffer), calculate new average
	if(me->shiftCount == me->shiftThreshold)
	{
		// Calculate new average
		arm_mean_f32(me->buff, me->buffLen, &me->avg);

		// Reset shift counter
		me->shiftCount = 0;
	}

	// Return
	return me->avg;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_CMSIS_DSP)
