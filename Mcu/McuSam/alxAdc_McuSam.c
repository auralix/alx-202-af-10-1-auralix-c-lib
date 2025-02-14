/**
  ******************************************************************************
  * @file		alxAdc_McuSam.c
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
// Includes
//******************************************************************************
#include "alxAdc_McuSam.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ioPinArr
  * @param[in]		numOfIoPins
  */
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins
)
{
	// Parameters
	me->ioPinArr = ioPinArr;
	me->numOfIoPins = numOfIoPins;

	// Variables
	memset(&me->descr, 0, sizeof(me->descr));

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

	// Init GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Init ADC clock
	_pm_enable_bus_clock(PM_BUS_APBC, ADC);
	_gclk_enable_channel(ADC_GCLK_ID, CONF_GCLK_ADC_SRC);

	// Init ADC
	adc_sync_init(&me->descr, ADC, ALX_NULL);	// TV: Returns always OK

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

	// DeInit ADC
	adc_sync_deinit(&me->descr);	// TV: Returns always OK

	// DeInit ADC clock
	_pm_disable_bus_clock(PM_BUS_APBC, ADC);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->numOfIoPins; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

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
	// Unsupported
	ALX_ADC_ASSERT(false);
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

	// Types
	typedef union
	{
		uint16_t val;
		uint8_t raw[2];
	} AlxAdc_Voltage_12bit;
	AlxAdc_Voltage_12bit voltage_12bit = {};

	// Get channel voltage
	adc_sync_set_inputs(&me->descr, ch, 0x18, ALX_NULL);				// TV: Returns always OK, neg_input = Fixed = 0x18 = Internal ground
	adc_sync_read_channel(&me->descr, ALX_NULL, voltage_12bit.raw, 2);	// TV: We will not handle return

	// Calculate
	uint32_t voltage_mV = (voltage_12bit.val * ALX_ADC_VREF_mV) / ALX_ADC_12BIT_FULL_SCALE_COUNT;

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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)
