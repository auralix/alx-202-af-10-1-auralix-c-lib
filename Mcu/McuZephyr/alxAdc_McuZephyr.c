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
// Private Functions
//******************************************************************************
uint32_t AlxAdc_GetVoltage_mV_Private(AlxAdc* me, Alx_Ch ch);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		deviceName
  * @param[in]		chArr
  * @param[in]		numOfCh
  * @param[in]		gain
  */
void AlxAdc_Ctor
(
	AlxAdc* me,
	const char* deviceName,
	Alx_Ch* chArr,
	uint8_t numOfCh,
	enum adc_gain gain
)
{
	// Parameters
	me->deviceName = deviceName;
	me->chArr = chArr;
	me->numOfCh = numOfCh;
	me->gain = gain;

	// Variables
	me->device = NULL;
	for (uint32_t i = 0; i < numOfCh; i++)
	{
		me->ch[i].gain = gain;
		me->ch[i].reference = ADC_REF_INTERNAL;
		me->ch[i].acquisition_time = ADC_ACQ_TIME_DEFAULT;
		me->ch[i].channel_id = chArr[i];
		me->ch[i].differential = 0;
		me->ch[i].input_positive = chArr[i];
		me->ch[i].input_negative = 0;
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
	ALX_ADC_ASSERT(me->device == NULL);

	// Init
	me->device = device_get_binding(me->deviceName);
	if (me->device == NULL)
	{
		ALX_ADC_TRACE("Err: device_get_binding");
		return Alx_Err;
	}

	// Init Channel
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		int32_t status = adc_channel_setup(me->device, &me->ch[i]);
		if (status != 0)
		{
			ALX_ADC_TRACE("Err: %d", status);
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
	ALX_ADC_ASSERT(me->device != NULL);

	// DeInit
	me->device = NULL;

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

	// Get
	uint32_t voltage_mV = AlxAdc_GetVoltage_mV_Private(me, ch);
	float voltage_V = (float)voltage_mV / 1000.f;

	// Return
	return voltage_V;
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

	// Return
	return AlxAdc_GetVoltage_mV_Private(me, ch);
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// Unsupported
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
}


//******************************************************************************
// Private Functions
//******************************************************************************
uint32_t AlxAdc_GetVoltage_mV_Private(AlxAdc* me, Alx_Ch ch)
{
	// Local variables
	int32_t status = 0;
	uint16_t vref_mV = 0;
	int16_t voltage_raw = 0;	// TV: ADC can occasionaly return 0xFFFF (negative value), https://devzone.nordicsemi.com/f/nordic-q-a/19688/nrf52832-saadc-occasionally-returns-0xffff
	int32_t voltage_mV = 0;
	struct adc_sequence sequence = {};

	// Get vref
	vref_mV = adc_ref_internal(me->device);
	if (vref_mV == 0)
	{
		ALX_ADC_TRACE("Err: %u", vref_mV);
		return 0;
	}

	// Get voltage
	sequence.options = NULL;
	sequence.channels = BIT(ch);
	sequence.buffer = &voltage_raw;
	sequence.buffer_size = sizeof(voltage_raw);
	sequence.resolution = ALX_ADC_RESOLUTION;
	sequence.oversampling = 0;
	sequence.calibrate = true;
	status = adc_read(me->device, &sequence);
	if (status != 0)
	{
		ALX_ADC_TRACE("Err: %d", status);
		return 0;
	}

	// Bound
	AlxBound_Int16(&voltage_raw, 0x0000, 0x7FFF);

	// Prepare
	voltage_mV = voltage_raw;
	status = adc_raw_to_millivolts(vref_mV, me->gain, ALX_ADC_RESOLUTION, &voltage_mV);
	if (status != 0)
	{
		ALX_ADC_TRACE("Err: %d", status);
		return 0;
	}

	// Return
	return voltage_mV;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
