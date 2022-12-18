/**
  ******************************************************************************
  * @file		alxRtc_McuStm32.h
  * @brief		Auralix C Library - ALX RTC MCU STM32 Module
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
#ifndef ALX_RTC_MCU_STM32_H
#define ALX_RTC_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxRtc_Global.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32L4))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_256Hz_RtcClk_32kHz768_LseCrystal_32kHz768_Default = 0,
	AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseCrystal_32kHz768 = 1,
	AlxRtc_Clk_McuStm32_SyncClk_1Hz_AsynClk_32kHz768_RtcClk_32kHz768_LseOscillator_32kHz768 = 2,
} AlxRtc_Clk;

typedef struct
{
	// Parameters
	AlxRtc_Clk rtcClk;

	// Variables
	RCC_OscInitTypeDef iosc;
	RCC_PeriphCLKInitTypeDef iclk;
	RTC_HandleTypeDef hrtc;
	uint64_t rtcTick_ns;
	uint32_t PRER_Expected;
	AlxRtc_DateTime lastSetDateTime;
	uint32_t lastSetSubSec;
	uint64_t lastSetUnixTime_ns;
	bool isDateTimeConfigured;

	// Info
	bool wasCtorCalled;
	bool isInit;
	bool isErr;
} AlxRtc;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		rtcClk
  */
void AlxRtc_Ctor
(
	AlxRtc* me,
	AlxRtc_Clk rtcClk
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_RTC_MCU_STM32_H
