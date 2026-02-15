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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Variables
//******************************************************************************
AlxTick alxTick = {};


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Ctor
(
	AlxTick* me
)
{
	// Variables
	me->tick_ns = 0;	// IRQ Safe

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
uint64_t AlxTick_Get_ns(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	// Void
	(void)me;

	// Get
	#if IS_ENABLED(CONFIG_TIMER_HAS_64BIT_CYCLE_COUNTER)
	uint64_t cycles = k_cycle_get_64();	// AB: Use direct Zephyr cycle counter for high precision timing
	uint64_t tick_ns = k_cyc_to_ns_floor64(cycles);
	#else
	uint64_t ticks = k_uptime_ticks();
	uint64_t tick_ns = k_ticks_to_ns_floor64(ticks);
	#endif

	// Return
	return tick_ns;

	#else
	// Prepare
	uint64_t tick_ns = 0;

	// Get
	uint32_t key = AlxIrq_Lock();
	tick_ns = me->tick_ns;
	AlxIrq_Unlock(key);

	// Return
	return tick_ns;
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_us(AlxTick* me)
{
	return AlxTick_Get_ns(me) / 1000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_ms(AlxTick* me)
{
	return AlxTick_Get_ns(me) / 1000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_sec(AlxTick* me)
{
	return AlxTick_Get_ns(me) / 1000000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_min(AlxTick* me)
{
	return AlxTick_Get_ns(me) / 60000000000;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_hr(AlxTick* me)
{
	return AlxTick_Get_ns(me) / 3600000000000;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_ns(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	#else
	AlxTick_IncRange_ns(me, 1);
	#endif
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_us(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	#else
	AlxTick_IncRange_ns(me, 1000);
	#endif
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_ms(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	#else
	AlxTick_IncRange_ns(me, 1000000);
	#endif
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_sec(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	#else
	AlxTick_IncRange_ns(me, 1000000000);
	#endif
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_min(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	#else
	AlxTick_IncRange_ns(me, 60000000000);
	#endif
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_hr(AlxTick* me)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	#else
	AlxTick_IncRange_ns(me, 3600000000000);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_ns
  */
void AlxTick_IncRange_ns(AlxTick* me, uint64_t ticks_ns)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	(void)ticks_ns;
	#else
	uint32_t key = AlxIrq_Lock();
	me->tick_ns = me->tick_ns + ticks_ns;
	AlxIrq_Unlock(key);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_us
  */
void AlxTick_IncRange_us(AlxTick* me, uint64_t ticks_us)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	(void)ticks_us;
	#else
	AlxTick_IncRange_ns(me, ticks_us * 1000);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_ms
  */
void AlxTick_IncRange_ms(AlxTick* me, uint64_t ticks_ms)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	(void)ticks_ms;
	#else
	AlxTick_IncRange_ns(me, ticks_ms * 1000000);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_sec
  */
void AlxTick_IncRange_sec(AlxTick* me, uint64_t ticks_sec)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	(void)ticks_sec;
	#else
	AlxTick_IncRange_ns(me, ticks_sec * 1000000000);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_min
  */
void AlxTick_IncRange_min(AlxTick* me, uint64_t ticks_min)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	(void)ticks_min;
	#else
	AlxTick_IncRange_ns(me, ticks_min * 60000000000);
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_hr
  */
void AlxTick_IncRange_hr(AlxTick* me, uint64_t ticks_hr)
{
	#if defined(ALX_ZEPHYR)
	(void)me;
	(void)ticks_hr;
	#else
	AlxTick_IncRange_ns(me, ticks_hr * 3600000000000);
	#endif
}


#endif	// #if defined(ALX_C_LIB)
