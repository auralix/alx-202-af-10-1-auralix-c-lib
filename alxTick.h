/**
  ******************************************************************************
  * @file		alxTick.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_TICK_H
#define ALX_TICK_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	uint64_t tick_ns;

	// Info
	bool wasCtorCalled;
} AlxTick;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxTick alxTick;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTick_Ctor
(
	AlxTick* me
);


//******************************************************************************
// Functions
//******************************************************************************
uint64_t AlxTick_Get_ns(AlxTick* me);
uint64_t AlxTick_Get_us(AlxTick* me);
uint64_t AlxTick_Get_ms(AlxTick* me);
uint64_t AlxTick_Get_sec(AlxTick* me);
uint64_t AlxTick_Get_min(AlxTick* me);
uint64_t AlxTick_Get_hr(AlxTick* me);
void AlxTick_Inc_ns(AlxTick* me);
void AlxTick_Inc_us(AlxTick* me);
void AlxTick_Inc_ms(AlxTick* me);
void AlxTick_Inc_sec(AlxTick* me);
void AlxTick_Inc_min(AlxTick* me);
void AlxTick_Inc_hr(AlxTick* me);
void AlxTick_IncRange_ns(AlxTick* me, uint64_t ticks_ns);
void AlxTick_IncRange_us(AlxTick* me, uint64_t ticks_us);
void AlxTick_IncRange_ms(AlxTick* me, uint64_t ticks_ms);
void AlxTick_IncRange_sec(AlxTick* me, uint64_t ticks_sec);
void AlxTick_IncRange_min(AlxTick* me, uint64_t ticks_min);
void AlxTick_IncRange_hr(AlxTick* me, uint64_t ticks_hr);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TICK_H
