/**
  ******************************************************************************
  * @file		alxDelay.c
  * @brief		Auralix C Library - ALX Delay Module
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
#include "alxDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Functions
//******************************************************************************
void AlxDelay_ns(uint64_t delay_ns)
{
	uint64_t ticksStart_ns = AlxTick_Get_ns(&alxTick);
	while ((AlxTick_Get_ns(&alxTick) - ticksStart_ns) < delay_ns);
}
void AlxDelay_us(uint64_t delay_us)		{ AlxDelay_ns(delay_us  * 1000); }
void AlxDelay_ms(uint64_t delay_ms)		{ AlxDelay_ns(delay_ms  * 1000000); }
void AlxDelay_sec(uint64_t delay_sec)	{ AlxDelay_ns(delay_sec * 1000000000); }
void AlxDelay_min(uint64_t delay_min)	{ AlxDelay_ns(delay_min * 60000000000); }
void AlxDelay_hr(uint64_t delay_hr)		{ AlxDelay_ns(delay_hr  * 3600000000000); }


#endif	// #if defined(ALX_C_LIB)
