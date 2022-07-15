/**
  ******************************************************************************
  * @file		alxAdc_McuLpc84.h
  * @brief		Auralix C Library - ALX ADC Module
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

#ifndef ALX_ADC_MCU_LPC84_H
#define ALX_ADC_MCU_LPC84_H

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


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC84)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Obejcts - External
	AlxIoPin** channels;

	// Parameters
	uint8_t numChannels;
	uint8_t adcClkDiv;
	uint16_t voltageRefP_mV;

	// Variables
	adc_config_t adcConfig;
	adc_conv_seq_config_t adcConvSeqConfig;
	adc_result_info_t adcResult;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdc_Mcu;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdcMcu_Ctor
(
	AlxAdc_Mcu* me,
	AlxIoPin** channels,
	uint8_t numChannels,
	uint8_t adcClkDiv,
	uint16_t voltageRefP_mV
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_ADC_MCU_LPC84_H
