/**
  ******************************************************************************
  * @file		alxDelay.h
  * @brief		Auralix C Library - ALX Delay Module
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

#ifndef ALX_DELAY_H
#define ALX_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTick.h"


//******************************************************************************
// Functions
//******************************************************************************
void AlxDelay_ns(uint64_t delay_ns);
void AlxDelay_us(uint64_t delay_us);
void AlxDelay_ms(uint64_t delay_ms);
void AlxDelay_sec( uint64_t delay_sec);
void AlxDelay_min(uint64_t delay_min);
void AlxDelay_hr(uint64_t delay_hr);


#ifdef __cplusplus
}
#endif

#endif // ALX_TIM_H
