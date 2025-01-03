﻿/**
  ******************************************************************************
  * @file		alxPwm_McuStm32.h
  * @brief		Auralix C Library - ALX PWM MCU STM32 Module
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
#ifndef ALX_PWM_MCU_STM32_H
#define ALX_PWM_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_PWM_BUFF_LEN 6

	// Parameters
	TIM_TypeDef* tim;
	AlxIoPin** ioPinArr;
	Alx_Ch* chArr;
	uint8_t numOfCh;
	AlxClk* clk;
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	uint16_t* dutyDefaultArr_permil;
	#else
	float* dutyDefaultArr_pct;
	#endif
	uint32_t prescaler;
	uint32_t period;

	// Variables
	uint32_t srcClk_Hz;	// Timer source clock, which is then divided by prescaler to get timer clock
	uint32_t periodMax;
	TIM_HandleTypeDef htim;
	TIM_OC_InitTypeDef chtim[ALX_PWM_BUFF_LEN];
	Alx_Ch ch[ALX_PWM_BUFF_LEN];
	#if !defined (ALX_PWM_OPTIMIZE_SIZE)
	float dutyDefault_pct[ALX_PWM_BUFF_LEN];
	#endif

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxPwm;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPwm_Ctor
(
	AlxPwm* me,
	TIM_TypeDef* tim,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfCh,
 	AlxClk* clk,
	#if defined(ALX_PWM_OPTIMIZE_SIZE)
	uint16_t* dutyDefaultArr_permil,
	#else
	float* dutyDefaultArr_pct,
	#endif
	uint32_t prescaler,
	uint32_t period
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PWM_MCU_STM32_H
