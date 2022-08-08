/**
  ******************************************************************************
  * @file		alxOsDelayUntil.c
  * @brief		Auralix C Library - ALX OS Delay Until Module
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
#include "alxOsDelayUntil.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsDelayUntil_Ctor
(
	AlxOsDelayUntil* me,
	AlxClk_Tick osTick
)
{
	// Parameters
	me->osTick = osTick;

	// Variables
	#if defined(ALX_FREE_RTOS)
	me->xLastWakeTime = 0;
	#endif

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsDelayUntil_Init(AlxOsDelayUntil* me)
{
	// #1 Assert
	ALX_OS_DELAY_UNTIL_ASSERT(me->isInit == false);
	ALX_OS_DELAY_UNTIL_ASSERT(me->wasCtorCalled == true);

	// #2 Set xLastWakeTime
	me->xLastWakeTime = xTaskGetTickCount();

	// #3 Set isInit
	me->isInit = true;
}
void AlxOsDelayUntil_us(AlxOsDelayUntil* me, uint64_t osTimeIncrement_us)
{
	// #1 Assert
	ALX_OS_DELAY_UNTIL_ASSERT(me->isInit == true);
	ALX_OS_DELAY_UNTIL_ASSERT(me->wasCtorCalled == true);
	ALX_OS_DELAY_UNTIL_ASSERT(osTimeIncrement_us >= (2 * (uint64_t)me->osTick));
	ALX_OS_DELAY_UNTIL_ASSERT((osTimeIncrement_us % (uint64_t)me->osTick) == 0);

	// #2 Convert to osTick
	uint64_t osTimeIncrement_osTick = osTimeIncrement_us / (uint64_t)me->osTick;

	// #3 Delay
	#if defined(ALX_FREE_RTOS)
	vTaskDelayUntil(&me->xLastWakeTime, osTimeIncrement_osTick);
	#endif
}
void AlxOsDelayUntil_ms(AlxOsDelayUntil* me, uint64_t osTimeIncrement_ms)	{ AlxOsDelay_ms(me, osTimeIncrement_ms * 1000); }
void AlxOsDelayUntil_sec(AlxOsDelayUntil* me, uint64_t osTimeIncrement_sec)	{ AlxOsDelay_sec(me, osTimeIncrement_sec * 1000000); }
void AlxOsDelayUntil_min(AlxOsDelayUntil* me, uint64_t osTimeIncrement_min)	{ AlxOsDelay_min(me, osTimeIncrement_min * 60000000); }
void AlxOsDelayUntil_hr(AlxOsDelayUntil* me, uint64_t osTimeIncrement_hr)	{ AlxOsDelay_hr(me, osTimeIncrement_hr * 3600000000); }


#endif	// #if defined(ALX_C_LIB) && defined(ALX_OS) && defined(ALX_FREE_RTOS)
