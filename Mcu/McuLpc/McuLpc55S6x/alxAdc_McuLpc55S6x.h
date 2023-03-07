/**
  ******************************************************************************
  * @file		alxAdc_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX ADC MCU LPC55S6X Module
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
#ifndef ALX_ADC_MCU_LPC55S6X_H
#define ALX_ADC_MCU_LPC55S6X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxAdc_Clk_McuLpc55S6x_AdcClk_12MHz_MainClk_12MHz = 1U,		// MF: ClkDiv = 1
	AlxAdc_Clk_McuLpc55S6x_AdcClk_24MHz_MainClk_96MHz = 4U,		// MF: ClkDiv = 4
	AlxAdc_Clk_McuLpc55S6x_AdcClk_18MHz75_MainClk_150MHz = 8U	// MF: ClkDiv = 8
} AlxAdc_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;

	// Parameters
	Alx_Ch* chArr;
	uint8_t numOfIoPinsAndCh;
	AlxAdc_Clk adcClk;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV;
	#else
	float vRef_V;
	#endif

	// Variables
	lpadc_config_t adcConfig;
	lpadc_conv_command_config_t adcConvCommConfig;
	lpadc_conv_trigger_config_t adcConvTrigConfig;
	lpadc_conv_result_t adcConvResult;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	AlxClk* clk,
	AlxAdc_Clk adcClk,
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV
	#else
	float vRef_V
	#endif
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_MCU_LPC55S6X_H
