/**
  ******************************************************************************
  * @file		alxOsDelay.c
  * @brief		Auralix C Library - ALX OS Delay Module
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
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Variables
//******************************************************************************
AlxOsDelay alxOsDelay = {0};


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osTick
  * @param[in]		approxDisable
  */
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

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_us
  */
void AlxOsDelay_us(AlxOsDelay* me, uint64_t osDelay_us)
{
	// Assert
	ALX_OS_DELAY_ASSERT(me->wasCtorCalled == true);

	// Check if approximation is disabled
	if (me->approxDisable)
	{
		ALX_OS_DELAY_ASSERT(osDelay_us >= (2 * (uint64_t)me->osTick));
		ALX_OS_DELAY_ASSERT((osDelay_us % (uint64_t)me->osTick) == 0);
	}

	// Convert to osTick
	uint64_t osDelay_osTick = osDelay_us / (uint64_t)me->osTick;

	// Delay
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(osDelay_osTick);
	#endif
	#if defined(ALX_ZEPHYR)
	k_sleep(K_USEC(osDelay_us));
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_ms
  */
void AlxOsDelay_ms(AlxOsDelay* me, uint64_t osDelay_ms)
{
	AlxOsDelay_us(me, osDelay_ms * 1000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_sec
  */
void AlxOsDelay_sec(AlxOsDelay* me, uint64_t osDelay_sec)
{
	AlxOsDelay_us(me, osDelay_sec * 1000000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_min
  */
void AlxOsDelay_min(AlxOsDelay* me, uint64_t osDelay_min)
{
	AlxOsDelay_us(me, osDelay_min * 60000000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		osDelay_hr
  */
void AlxOsDelay_hr(AlxOsDelay* me, uint64_t osDelay_hr)
{
	AlxOsDelay_us(me, osDelay_hr * 3600000000);
}


#endif	// #if defined(ALX_C_LIB)
