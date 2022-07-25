/**
  ******************************************************************************
  * @file		alxTimSw.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_TIM_SW_H
#define ALX_TIM_SW_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	uint64_t ticksStart_ns;
	bool isRunning;

	//Info
	bool wasCtorCalled;
} AlxTimSw;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTimSw_Ctor(AlxTimSw* me, bool start);


//******************************************************************************
// Functions
//******************************************************************************
void AlxTimSw_Start(AlxTimSw* me);
void AlxTimSw_Stop(AlxTimSw* me);
bool AlxTimSw_IsRunning(AlxTimSw* me);
uint64_t AlxTimSw_Get_ns(AlxTimSw* me);
uint64_t AlxTimSw_Get_us(AlxTimSw* me);
uint64_t AlxTimSw_Get_ms(AlxTimSw* me);
uint64_t AlxTimSw_Get_sec(AlxTimSw* me);
uint64_t AlxTimSw_Get_min(AlxTimSw* me);
uint64_t AlxTimSw_Get_hr(AlxTimSw* me);
bool AlxTimSw_IsTimeout_ns(AlxTimSw* me, uint64_t timeout_ns);
bool AlxTimSw_IsTimeout_us(AlxTimSw* me, uint64_t timeout_us);
bool AlxTimSw_IsTimeout_ms(AlxTimSw* me, uint64_t timeout_ms);
bool AlxTimSw_IsTimeout_sec(AlxTimSw* me, uint64_t timeout_sec);
bool AlxTimSw_IsTimeout_min(AlxTimSw* me, uint64_t timeout_min);
bool AlxTimSw_IsTimeout_hr(AlxTimSw* me, uint64_t timeout_hr);


#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_TIM_SW_H
