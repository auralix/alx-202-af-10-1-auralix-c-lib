/**
  ******************************************************************************
  * @file		alxOsDelay.c
  * @brief		Auralix C Library - ALX OS Delay Module
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
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Variables
//******************************************************************************
AlxOsDelay alxOsDelay = {0};


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsDelay_Ctor
(
	AlxOsDelay* me,
	AlxClk_Tick osTick,
	bool approxDisable
)
{
	// Parameters
	me->osTick = osTick;
	me->approxDisable = approxDisable;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsDelay_us(AlxOsDelay* me, uint64_t osDelay_us)
{
	// #1 Assert
	ALX_OS_DELAY_ASSERT(me->wasCtorCalled == true);

	// #2 Check if approximation is disabled
	if (me->approxDisable)
	{
		ALX_OS_DELAY_ASSERT(osDelay_us >= (2 * (uint64_t)me->osTick));
		ALX_OS_DELAY_ASSERT((osDelay_us % (uint64_t)me->osTick) == 0);
	}

	// #3 Convert to osTick
	uint64_t osDelay_osTick = osDelay_us / (uint64_t)me->osTick;

	// #4 Delay
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(osDelay_osTick);
	#endif
}
void AlxOsDelay_ms(AlxOsDelay* me, uint64_t osDelay_ms)		{ AlxOsDelay_us(me, osDelay_ms * 1000); }
void AlxOsDelay_sec(AlxOsDelay* me, uint64_t osDelay_sec)	{ AlxOsDelay_us(me, osDelay_sec * 1000000); }
void AlxOsDelay_min(AlxOsDelay* me, uint64_t osDelay_min)	{ AlxOsDelay_us(me, osDelay_min * 60000000); }
void AlxOsDelay_hr(AlxOsDelay* me, uint64_t osDelay_hr)		{ AlxOsDelay_us(me, osDelay_hr * 3600000000); }


#endif // #if defined(ALX_FREE_RTOS)
