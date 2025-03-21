/**
  ******************************************************************************
  * @file		alxAdc_McuSam.h
  * @brief		Auralix C Library - ALX ADC MCU SAM Module
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
#ifndef ALX_ADC_MCU_SAM_H
#define ALX_ADC_MCU_SAM_H

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
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines
	#define ALX_ADC_VREF_mV 1000
	#define ALX_ADC_12BIT_FULL_SCALE_COUNT 4095

	// Parameters
	AlxIoPin** ioPinArr;
	uint8_t numOfIoPins;

	// Variables
	struct adc_sync_descriptor descr;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxAdc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_MCU_SAM_H
