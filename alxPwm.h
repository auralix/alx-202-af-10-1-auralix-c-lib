/**
  ******************************************************************************
  * @file		alxPwm.h
  * @brief		Auralix C Library - ALX PWM Module
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

#ifndef ALX_PWM_H
#define ALX_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

// AlxMcu //
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxPwm_McuStm32.h"

#elif defined(ALX_LPC80X)
#include "alxPwm_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxPwm_McuLpc55S6x.h"

#else
typedef struct { bool dummy; } AlxPwm;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PWM_FILE "alxPwm"

// Assert //
#if defined(_ALX_PWM_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PWM_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PWM_FILE, expr)
#elif defined(_ALX_PWM_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PWM_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PWM_FILE, expr)
#elif defined(_ALX_PWM_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PWM_ASSERT(expr) ALX_ASSERT_RST(ALX_PWM_FILE, expr)
#else
	#define ALX_PWM_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PWM_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PWM_TRACE(...) ALX_TRACE_STD(ALX_PWM_FILE, __VA_ARGS__)
#else
	#define ALX_PWM_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_PWM_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_PWM_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_PWM_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxPwm_Init(AlxPwm* me);
Alx_Status AlxPwm_DeInit(AlxPwm* me);
Alx_Status AlxPwm_SetDuty_pct(AlxPwm* me, Alx_Ch ch, float duty_pct);
Alx_Status AlxPwm_SetDuty_permil(AlxPwm* me, Alx_Ch ch, uint16_t duty_permil);


#ifdef __cplusplus
}
#endif

#endif // ALX_PWM_H
