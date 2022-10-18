/**
  ******************************************************************************
  * @file		alxPwm_McuStm32.h
  * @brief		Auralix C Library - ALX PWM MCU STM32 Module
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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;

	// Parameters
	Alx_Ch* chArr;
	float* dutyDefaultArr_pct;
	uint8_t numOfCh;

	// Variables
	uint32_t srcClk_Hz; // Timer source clock, which is then divided by prescaler to get timer clock
	uint32_t periodMax;
	uint32_t prescaler;
	uint32_t period;
	TIM_HandleTypeDef htim;
	TIM_OC_InitTypeDef chtim[ALX_PWM_BUFF_LEN];
	Alx_Ch ch[ALX_PWM_BUFF_LEN];
	#if !defined (ALX_PWM_OPTIMIZE_SIZE)
	float dutyDefault_pct[ALX_PWM_BUFF_LEN];
	#endif

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxPwm;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		tim
  * @param[in]		ioPinArr
  * @param[in]		chArr
  * @param[in]		dutyDefaultArr_pct
  * @param[in]		numOfCh
  * @param[in]		clk
  * @param[in]		prescaler
  * @param[in]		period
  */
void AlxPwm_Ctor
(
	AlxPwm* me,
	TIM_TypeDef* tim,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	#if !defined (ALX_PWM_OPTIMIZE_SIZE)
	float* dutyDefaultArr_pct,
	#endif
	uint8_t numOfCh,
 	AlxClk* clk,
	uint32_t prescaler,
	uint32_t period
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PWM_MCU_STM32_H
