﻿/**
  ******************************************************************************
  * @file		alxDac_McuStm32.h
  * @brief		Auralix C Library - ALX DAC MCU STM32 Module
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
#ifndef ALX_DAC_MCU_STM32_H
#define ALX_DAC_MCU_STM32_H

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
#include "alxBound.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_DAC_BUFF_LEN 2

	// Const
	uint32_t RESOLUTION;

	// Parameters
	DAC_TypeDef* dac;
	AlxIoPin** ioPinArr;
	Alx_Ch* chArr;
	float* setVoltageDefaultArr_V;
	uint8_t numOfCh;
	bool isVrefInt_V;
	float vrefExt_V;

	// Variables
	DAC_HandleTypeDef hdac;
	DAC_ChannelConfTypeDef chdac[ALX_DAC_BUFF_LEN];
	Alx_Ch ch[ALX_DAC_BUFF_LEN];
	float setVoltageDefault_V[ALX_DAC_BUFF_LEN];

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxDac;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxDac_Ctor
(
	AlxDac* me,
	DAC_TypeDef* dac,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	float* setVoltageDefaultArr_V,
	uint8_t numOfCh,
	bool isVrefInt_V,
	float vrefExt_V
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_DAC_MCU_STM32_H
