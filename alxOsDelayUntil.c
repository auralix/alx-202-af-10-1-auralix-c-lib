/**
  ******************************************************************************
  * @file		alxOsDelayUntil.c
  * @brief		Auralix C Library - ALX OS Delay Until Module
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
#include "alxOsDelayUntil.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTick
  */
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

/**
  * @brief
  * @param[in,out] me
  */
void AlxOsDelayUntil_Init(AlxOsDelayUntil* me)
{
	// Assert
	ALX_OS_DELAY_UNTIL_ASSERT(me->isInit == false);
	ALX_OS_DELAY_UNTIL_ASSERT(me->wasCtorCalled == true);

	// Set xLastWakeTime
	#if defined(ALX_FREE_RTOS)
	me->xLastWakeTime = xTaskGetTickCount();
	#endif

	// Set isInit
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelayUntilIncrement_us
  */
void AlxOsDelayUntil_us(AlxOsDelayUntil* me, uint64_t osTimeIncrement_us)
{
	// Assert
	ALX_OS_DELAY_UNTIL_ASSERT(me->isInit == true);
	ALX_OS_DELAY_UNTIL_ASSERT(me->wasCtorCalled == true);
	ALX_OS_DELAY_UNTIL_ASSERT(osTimeIncrement_us >= (2 * (uint64_t)me->osTick));
	ALX_OS_DELAY_UNTIL_ASSERT((osTimeIncrement_us % (uint64_t)me->osTick) == 0);

	// Convert to osTick
	uint64_t osTimeIncrement_osTick = osTimeIncrement_us / (uint64_t)me->osTick;

	// Delay
	#if defined(ALX_FREE_RTOS)
	vTaskDelayUntil(&me->xLastWakeTime, osTimeIncrement_osTick);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTimeIncrement_ms
  */
void AlxOsDelayUntil_ms(AlxOsDelayUntil* me, uint64_t osTimeIncrement_ms)	{ AlxOsDelayUntil_us(me, osTimeIncrement_ms * 1000); }

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTimeIncrement_sec
  */
void AlxOsDelayUntil_sec(AlxOsDelayUntil* me, uint64_t osTimeIncrement_sec)	{ AlxOsDelayUntil_us(me, osTimeIncrement_sec * 1000000); }

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTimeIncrement_min
  */
void AlxOsDelayUntil_min(AlxOsDelayUntil* me, uint64_t osTimeIncrement_min)	{ AlxOsDelayUntil_us(me, osTimeIncrement_min * 60000000); }

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTimeIncrement_hr
  */
void AlxOsDelayUntil_hr(AlxOsDelayUntil* me, uint64_t osTimeIncrement_hr)	{ AlxOsDelayUntil_us(me, osTimeIncrement_hr * 3600000000); }


#endif	// #if defined(ALX_C_LIB)
