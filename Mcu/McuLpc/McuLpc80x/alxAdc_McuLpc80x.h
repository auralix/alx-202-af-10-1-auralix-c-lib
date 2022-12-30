/**
  ******************************************************************************
  * @file		alxAdc_McuLpc80x.h
  * @brief		Auralix C Library - ALX ADC MCU LPC80X Module
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
#ifndef ALX_ADC_MCU_LPC80X_H
#define ALX_ADC_MCU_LPC80X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Obejcts - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;

	// Parameters
	Alx_Ch* chArr;
	uint8_t numOfIoPinsAndCh;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV;
	#else
	float vRef_V;
	#endif

	// Variables
	adc_config_t adcConfig;
	adc_conv_seq_config_t adcConvSeqConfig;
	adc_result_info_t adcResult;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdc;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ioPinArr
  * @param[in]		chArr
  * @param[in]		numOfIoPinsAndCh
  * @param[in]		clk
  * @param[in]		vRef_mV
  * @param[in]		vRef_V
  */
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	AlxClk* clk,
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV
	#else
	float vRef_V
	#endif
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_MCU_LPC80X_H
