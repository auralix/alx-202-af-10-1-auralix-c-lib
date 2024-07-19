/**
  ******************************************************************************
  * @file		alxAdc_McuZephyr.c
  * @brief		Auralix C Library - ALX ADC MCU Zephyr Module
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
// Includes
//******************************************************************************
#include "alxAdc_McuZephyr.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxAdc_Ctor
(
	AlxAdc* me,
	const char* adcName,
	Alx_Ch* chArr,
	uint8_t numOfCh
)
{
	// Parameters
	me->adcName = adcName;
	me->chArr = chArr;
	me->numOfCh = numOfCh;

	// Variables
	me->adc = device_get_binding(adcName);

	// ADC Channel
	for (uint32_t i = 0; i < numOfCh; i++)
	{
		me->chadc[i].gain = ADC_GAIN_1_4;
		me->chadc[i].reference = ADC_REF_VDD_1_4;
		me->chadc[i].acquisition_time = ADC_ACQ_TIME_DEFAULT;
		me->chadc[i].channel_id = chArr[i];
		me->chadc[i].differential = 0;
		me->chadc[i].input_positive = chArr[i];
		me->chadc[i].input_negative = 0;
		me->ch[i] = chArr[i];
	}

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdc_Init(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == false);

	// Init ADC channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (adc_channel_setup(me->adc, &me->chadc[i]) != 0)
		{
			ALX_ADC_TRACE("Err");
			return Alx_Err;
		}
	}

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdc_DeInit(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ch
  * @return
  */
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Get reference voltage
	uint32_t vref_mV = 3300;

	// Get channel voltage
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (ch == Alx_Ch_McuStm32_Vref)
		{
			// Get
			float vref_V = (float)vref_mV / 1000.f;

			// Return
			return vref_V;
		}
		else if (me->ch[i] == ch)
		{
			// Prepare
			uint16_t sample = 0;

			struct adc_sequence sequence;
			sequence.options = NULL;
			sequence.channels = BIT(ch);
			sequence.buffer = &sample;
			sequence.buffer_size = sizeof(sample);
			sequence.resolution = 12;
			sequence.oversampling = 0;
			sequence.calibrate = false;

			// Get
			if (adc_read(me->adc, &sequence) != 0)
			{
				ALX_ADC_TRACE("ADC read error");
				return ALX_NULL;
			}

			// Prepare
			float voltage_V = ((float)sample / 4096.f) * 3.3f;

			// Return
			return voltage_V;
		}
	}

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ch
  * @return
  */
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Assert
	ALX_ADC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
