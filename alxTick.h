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
	// Internal
	volatile uint64_t tick_ns;

	// Info
	bool wasCtorCalled;
} AlxTick;


//******************************************************************************
// Variables
//******************************************************************************
extern volatile AlxTick alxTick;


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
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_ns(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_us(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_ms(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_sec(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_min(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxTick_Get_hr(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_ns(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_us(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_ms(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_sec(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_min(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Inc_hr(volatile AlxTick* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_ns
  */
void AlxTick_IncRange_ns(volatile AlxTick* me, uint64_t ticks_ns);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_us
  */
void AlxTick_IncRange_us(volatile AlxTick* me, uint64_t ticks_us);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_ms
  */
void AlxTick_IncRange_ms(volatile AlxTick* me, uint64_t ticks_ms);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_sec
  */
void AlxTick_IncRange_sec(volatile AlxTick* me, uint64_t ticks_sec);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_min
  */
void AlxTick_IncRange_min(volatile AlxTick* me, uint64_t ticks_min);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ticks_hr
  */
void AlxTick_IncRange_hr(volatile AlxTick* me, uint64_t ticks_hr);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTick_Reset(volatile AlxTick* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TICK_H
