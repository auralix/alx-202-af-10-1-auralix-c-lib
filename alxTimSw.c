/**
  ******************************************************************************
  * @file		alxTimSw.c
  * @brief		Auralix C Library - ALX Timer SW Module
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
#include "alxTimSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTimSw_Ctor
(
	AlxTimSw* me,
	bool start
)
{
	// Variables
	me->ticksStart_ns = 0;
	me->isRunning = start;

	//Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxTimSw_Start(AlxTimSw* me)
{
	me->ticksStart_ns = AlxTick_Get_ns(&alxTick);
	me->isRunning = true;
}
void AlxTimSw_Stop(AlxTimSw* me)			{ me->isRunning = false; }
bool AlxTimSw_IsRunning(AlxTimSw* me)		{ return me->isRunning; }
uint64_t AlxTimSw_Get_ns(AlxTimSw* me)
{
	if (me->isRunning)
		return AlxTick_Get_ns(&alxTick) - me->ticksStart_ns;
	else
		return 0;
}
uint64_t AlxTimSw_Get_us(AlxTimSw*  me)		{ return AlxTimSw_Get_ns(me) / 1000; }
uint64_t AlxTimSw_Get_ms(AlxTimSw* me)		{ return AlxTimSw_Get_ns(me) / 1000000; }
uint64_t AlxTimSw_Get_sec(AlxTimSw* me)		{ return AlxTimSw_Get_ns(me) / 1000000000; }
uint64_t AlxTimSw_Get_min(AlxTimSw* me)		{ return AlxTimSw_Get_ns(me) / 60000000000; }
uint64_t AlxTimSw_Get_hr(AlxTimSw* me)		{ return AlxTimSw_Get_ns(me) / 3600000000000; }
bool AlxTimSw_IsTimeout_ns(AlxTimSw* me, uint64_t timeout_ns)
{
	uint64_t ticks_ns = AlxTimSw_Get_ns(me);
	if (ticks_ns >= timeout_ns)
		return true;
	else
		return false;
}
bool AlxTimSw_IsTimeout_us(AlxTimSw* me, uint64_t timeout_us)	{ return AlxTimSw_IsTimeout_ns(me, timeout_us * 1000); }
bool AlxTimSw_IsTimeout_ms(AlxTimSw* me, uint64_t timeout_ms)	{ return AlxTimSw_IsTimeout_ns(me, timeout_ms * 1000000); }
bool AlxTimSw_IsTimeout_sec(AlxTimSw* me, uint64_t timeout_sec)	{ return AlxTimSw_IsTimeout_ns(me, timeout_sec * 1000000000); }
bool AlxTimSw_IsTimeout_min(AlxTimSw* me, uint64_t timeout_min)	{ return AlxTimSw_IsTimeout_ns(me, timeout_min * 60000000000); }
bool AlxTimSw_IsTimeout_hr(AlxTimSw* me, uint64_t timeout_hr)	{ return AlxTimSw_IsTimeout_ns(me, timeout_hr * 3600000000000); }


#endif // #if defined(ALX_C_LIB)
