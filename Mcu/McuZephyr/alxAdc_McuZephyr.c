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
	for (uint8_t i = 0; i < numOfCh; i++)
	{
		me->ch[i].gain = gain;
		me->ch[i].reference = ADC_REF_INTERNAL;
		me->ch[i].acquisition_time = ADC_ACQ_TIME_DEFAULT;
		me->ch[i].channel_id = chArr[i];
		me->ch[i].differential = 0;
		#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
		me->ch[i].input_positive = chArr[i];
		me->ch[i].input_negative = 0;
		#endif
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
	// Trace
	ALX_ADC_TRACE_DBG("ENTER: deviceName %s", me->deviceName);

	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->device == NULL);

	// Init
	me->device = device_get_binding(me->deviceName);
	if (me->device == NULL)
	{
		ALX_ADC_TRACE_ERR("FAIL: device_get_binding() deviceName %s", me->deviceName);
		return Alx_Err;
	}

	// Init Channel
	for (uint8_t i = 0; i < me->numOfCh; i++)
	{
		int32_t status = adc_channel_setup(me->device, &me->ch[i]);
		if (status != 0)
		{
			ALX_ADC_TRACE_ERR("FAIL: adc_channel_setup() deviceName %s status %ld ch %u", me->deviceName, status, me->chArr[i]);
			return Alx_Err;
		}
	}

	// Set isInit
	me->isInit = true;

	// Trace
	ALX_ADC_TRACE_DBG("EXIT: deviceName %s", me->deviceName);

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
	// Trace
	ALX_ADC_TRACE_DBG("ENTER: deviceName %s", me->deviceName);

	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->device != NULL);

	// DeInit
	me->device = NULL;

	// Clear isInit
	me->isInit = false;

	// Trace
	ALX_ADC_TRACE_DBG("EXIT: deviceName %s", me->deviceName);

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
	// Trace
	ALX_ADC_TRACE_DBG("ENTER: deviceName %s ch %lu", me->deviceName, ch);

	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Get
	uint32_t voltage_mV = AlxAdc_GetVoltage_mV_Private(me, ch);
	float voltage_V = (float)voltage_mV / 1000.f;

	// Trace
	ALX_ADC_TRACE_DBG("EXIT: deviceName %s voltage_V %f", me->deviceName, voltage_V);

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
	// Trace
	ALX_ADC_TRACE_DBG("ENTER: deviceName %s ch %lu", me->deviceName, ch);

	// Assert
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	ALX_ADC_ASSERT(me->isInit == true);

	// Get
	uint32_t voltage_mV = AlxAdc_GetVoltage_mV_Private(me, ch);

	// Trace
	ALX_ADC_TRACE_DBG("EXIT: deviceName %s voltage_mV %lu", me->deviceName, voltage_mV);

	// Return
	return voltage_mV;
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
	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_ADC_TRACE_DBG("ENTER: deviceName %s ch %lu", me->deviceName, ch);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	int32_t status = 0;
	uint16_t vref_mV = 0;
	int16_t voltage_raw = 0;	// TV: ADC can occasionaly return 0xFFFF (negative value), https://devzone.nordicsemi.com/f/nordic-q-a/19688/nrf52832-saadc-occasionally-returns-0xffff
	int32_t voltage_mV = 0;
	struct adc_sequence seq = {};


	//------------------------------------------------------------------------------
	// Get Vref
	//------------------------------------------------------------------------------
	ALX_ADC_TRACE_VRB("DO: adc_ref_internal() deviceName %s", me->deviceName);
	vref_mV = adc_ref_internal(me->device);
	if (vref_mV == 0)
	{
		ALX_ADC_TRACE_ERR("FAIL: adc_ref_internal() deviceName %s vref_mV %u", me->deviceName, vref_mV);
		return 0;
	}
	ALX_ADC_TRACE_VRB("DONE: adc_ref_internal() deviceName %s vref_mV %u", me->deviceName, vref_mV);


	//------------------------------------------------------------------------------
	// Get Voltage
	//------------------------------------------------------------------------------
	seq.options = NULL;
	seq.channels = BIT(ch);
	seq.buffer = &voltage_raw;
	seq.buffer_size = sizeof(voltage_raw);
	seq.resolution = ALX_ADC_RESOLUTION;
	seq.oversampling = 8;
	seq.calibrate = false;
	ALX_ADC_TRACE_VRB("DO: adc_read() deviceName %s channels 0x%08lX resolution %u oversampling %u calibrate %u", me->deviceName, seq.channels, seq.resolution, seq.oversampling, seq.calibrate);
	status = adc_read(me->device, &seq);
	if (status != 0)
	{
		ALX_ADC_TRACE_ERR("FAIL: adc_read() deviceName %s status %ld", me->deviceName, status);
		return 0;
	}
	ALX_ADC_TRACE_VRB("DONE: adc_read() deviceName %s status %ld voltage_raw %d", me->deviceName, status, voltage_raw);


	//------------------------------------------------------------------------------
	// Bound
	//------------------------------------------------------------------------------
	ALX_ADC_TRACE_VRB("DO: AlxBound_Int16() deviceName %s voltage_raw %d", me->deviceName, voltage_raw);
	AlxBound_Int16(&voltage_raw, 0x0000, 0x7FFF);
	ALX_ADC_TRACE_VRB("DONE: AlxBound_Int16() deviceName %s voltage_raw %d", me->deviceName, voltage_raw);


	//------------------------------------------------------------------------------
	// Convert
	//------------------------------------------------------------------------------
	voltage_mV = voltage_raw;
	ALX_ADC_TRACE_VRB("DO: adc_raw_to_millivolts() deviceName %s vref_mV %ld, gain %lu resolution %u voltage_mV %ld", me->deviceName, vref_mV, me->gain, seq.resolution, voltage_mV);
	status = adc_raw_to_millivolts(vref_mV, me->gain, ALX_ADC_RESOLUTION, &voltage_mV);
	if (status != 0)
	{
		ALX_ADC_TRACE_ERR("FAIL: adc_raw_to_millivolts() deviceName %s status %ld", me->deviceName, status);
		return 0;
	}
	ALX_ADC_TRACE_VRB("DONE: adc_raw_to_millivolts() deviceName %s status %ld, voltage_mV %ld", me->deviceName, status, voltage_mV);


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_ADC_TRACE_DBG("EXIT: deviceName %s voltage_mV %lu", me->deviceName, voltage_mV);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return voltage_mV;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
