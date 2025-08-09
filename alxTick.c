/**
  ******************************************************************************
  * @file		alxTick.c
  * @brief		Auralix C Library - ALX Tick Module
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
#include "alxTick.h"

#if defined(ALX_ZEPHYR)
#include <zephyr/kernel.h>
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Variables
//******************************************************************************
volatile AlxTick alxTick = {0};


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Ctor
(
	volatile AlxTick* me
)
{
	// Variables
	me->tick_ns = 0;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_ns(volatile AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	// Use direct Zephyr cycle counter for high precision timing
	(void)me;  // Suppress unused parameter warning
	uint64_t cycles = k_cycle_get_64();
	return k_cyc_to_ns_floor64(cycles);
	#else
	volatile uint64_t tick_ns = 0;

	AlxGlobal_DisableIrq();		// Start of critical section
	tick_ns = me->tick_ns;		// Shared resource tick variable is used by:
								// Background - GetTick (gets current tick value)
								// Foreground - IrqHandler (incremnets tick value)
	AlxGlobal_EnableIrq();		// End of critical section

	return tick_ns;
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_us(volatile AlxTick* me)
{
	return AlxTick_Get_ns(me) / 1000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_ms(volatile AlxTick* me)
{
	return AlxTick_Get_ns(me) / 1000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_sec(volatile AlxTick* me)
{
	return AlxTick_Get_ns(me) / 1000000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_min(volatile AlxTick* me)
{
	return AlxTick_Get_ns(me) / 60000000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_hr(volatile AlxTick* me)
{
	return AlxTick_Get_ns(me) / 3600000000000;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_ns(volatile AlxTick* me)
{
	me->tick_ns = me->tick_ns + 1;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_us(volatile AlxTick* me)
{
	me->tick_ns = me->tick_ns + 1000;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_ms(volatile AlxTick* me)
{
	me->tick_ns = me->tick_ns + 1000000;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_sec(volatile AlxTick* me)
{
	me->tick_ns = me->tick_ns + 1000000000;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_min(volatile AlxTick* me)
{
	me->tick_ns = me->tick_ns + 60000000000;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_hr(volatile AlxTick* me)
{
	me->tick_ns = me->tick_ns + 3600000000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_ns
  */
void AlxTick_IncRange_ns(volatile AlxTick* me, uint64_t ticks_ns)
{
	me->tick_ns = me->tick_ns + ticks_ns;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_us
  */
void AlxTick_IncRange_us(volatile AlxTick* me, uint64_t ticks_us)
{
	me->tick_ns = me->tick_ns + (ticks_us * 1000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_ms
  */
void AlxTick_IncRange_ms(volatile AlxTick* me, uint64_t ticks_ms)
{
	me->tick_ns = me->tick_ns + (ticks_ms * 1000000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_sec
  */
void AlxTick_IncRange_sec(volatile AlxTick* me, uint64_t ticks_sec)
{
	me->tick_ns = me->tick_ns + (ticks_sec * 1000000000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_min
  */
void AlxTick_IncRange_min(volatile AlxTick* me, uint64_t ticks_min)
{
	me->tick_ns = me->tick_ns + (ticks_min * 60000000000);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_hr
  */
void AlxTick_IncRange_hr(volatile AlxTick* me, uint64_t ticks_hr)
{
	me->tick_ns = me->tick_ns + (ticks_hr * 3600000000000);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Reset(volatile AlxTick* me)
{
  #if !defined(ALX_ZEPHYR)
	AlxGlobal_DisableIrq();	// Start of critical section
	me->tick_ns = 0;		// Shared resource tick variable is used by:
							// Background - GetTick (gets current tick value)
							// Foreground - IrqHandler (incremnets tick value)
	AlxGlobal_EnableIrq();	// End of critical section
  #else
  (void)me;
  #endif // !defined(ALX_ZEPHYR)
}


#endif	// #if defined(ALX_C_LIB)
