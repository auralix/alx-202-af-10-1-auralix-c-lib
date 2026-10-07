/**
  ******************************************************************************
  * @file		alxAdc_McuLpc17xx.c
  * @brief		Auralix C Library - ALX ADC MCU LPC17XX Module
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
#include "alxAdc_McuLpc17xx.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC17XX)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint16_t AlxAdc_GetResult(AlxAdc* me, Alx_Ch ch);
static ADC_CHANNEL_T AlxAdc_GetCh(Alx_Ch ch);
static ADC_CHANNEL_T AlxAdc_GetIoPinCh(AlxIoPin* ioPin);
static bool AlxAdc_Ctor_AreIoPinsAndChOk(AlxAdc* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ioPinArr
  * @param[in]		chArr
  * @param[in]		numOfIoPinsAndCh
  * @param[in]		vRef_mV
  * @param[in]		vRef_V
  */
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	#if defined(ALX_ADC_OPTIMIZE_SIZE)
	uint32_t vRef_mV
	#else
	float vRef_V
	#endif
)
{
	// Assert
	ALX_ADC_ASSERT(numOfIoPinsAndCh <= ALX_ADC_BUFF_LEN);

	// Parameters
	me->ioPinArr = ioPinArr;
	me->chArr = chArr;
	me->numOfIoPinsAndCh = numOfIoPinsAndCh;
	#if defined(ALX_ADC_OPTIMIZE_SIZE)
	me->vRef_mV = vRef_mV;
	#else
	me->vRef_V = vRef_V;
	#endif

	// Check IoPins and channels - each IoPin must be the pin of its channel
	ALX_ADC_ASSERT(AlxAdc_Ctor_AreIoPinsAndChOk(me));
	(void)AlxAdc_Ctor_AreIoPinsAndChOk;

	// Variables
	me->adcClkSetup.adcRate = 0;
	me->adcClkSetup.bitsAccuracy = 0;
	me->adcClkSetup.burstMode = false;
	me->adcResult = 0;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
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
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// Init GPIO
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		AlxIoPin_Init(*(me->ioPinArr + i));
	}

	// Init ADC - clock, power and control register
	Chip_ADC_Init(LPC_ADC, &me->adcClkSetup);
	Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);

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
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// DeInit ADC - clock, power and control register
	Chip_ADC_DeInit(LPC_ADC);

	// DeInit GPIO
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		AlxIoPin_DeInit(*(me->ioPinArr + i));
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
	// Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// Optimize Guard
	#if defined(ALX_ADC_OPTIMIZE_SIZE)
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else
	// Get result
	uint16_t result = AlxAdc_GetResult(me, ch);

	// Prepare
	float voltage_V = ((result * me->vRef_V) / 4095);

	// Return
	return voltage_V;
	#endif
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
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// Optimize Guard
	#if !(defined(ALX_ADC_OPTIMIZE_SIZE))
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else
	// Get result
	uint16_t result = AlxAdc_GetResult(me, ch);

	// Prepare
	uint32_t voltage_mV = ((result * me->vRef_mV) / 4095);

	// Return
	return voltage_mV;
	#endif
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// LPC17xx has no internal temperature sensor
	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint16_t AlxAdc_GetResult(AlxAdc* me, Alx_Ch ch)
{
	// Prepare
	ADC_CHANNEL_T adcCh = AlxAdc_GetCh(ch);

	// There must be Enable and Disable function for each chennel before convertion.
	// I tried only enabling channels in Init, but it doens't work if there is no disable
	Chip_ADC_EnableChannel(LPC_ADC, adcCh, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (Chip_ADC_ReadStatus(LPC_ADC, adcCh, ADC_DR_DONE_STAT) != SET) {}
	Chip_ADC_ReadValue(LPC_ADC, adcCh, &me->adcResult);
	Chip_ADC_EnableChannel(LPC_ADC, adcCh, DISABLE);

	// Return
	return me->adcResult;
}
static ADC_CHANNEL_T AlxAdc_GetCh(Alx_Ch ch)
{
	if (ch == Alx_Ch_0)	return ADC_CH0;
	if (ch == Alx_Ch_1)	return ADC_CH1;
	if (ch == Alx_Ch_2)	return ADC_CH2;
	if (ch == Alx_Ch_3)	return ADC_CH3;
	if (ch == Alx_Ch_4)	return ADC_CH4;
	if (ch == Alx_Ch_5)	return ADC_CH5;
	if (ch == Alx_Ch_6)	return ADC_CH6;
	if (ch == Alx_Ch_7)	return ADC_CH7;

	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}
static ADC_CHANNEL_T AlxAdc_GetIoPinCh(AlxIoPin* ioPin)
{
	if (ioPin->port == 0 && ioPin->pin == 23)	return ADC_CH0;
	if (ioPin->port == 0 && ioPin->pin == 24)	return ADC_CH1;
	if (ioPin->port == 0 && ioPin->pin == 25)	return ADC_CH2;
	if (ioPin->port == 0 && ioPin->pin == 26)	return ADC_CH3;
	if (ioPin->port == 1 && ioPin->pin == 30)	return ADC_CH4;
	if (ioPin->port == 1 && ioPin->pin == 31)	return ADC_CH5;
	if (ioPin->port == 0 && ioPin->pin == 3)	return ADC_CH6;
	if (ioPin->port == 0 && ioPin->pin == 2)	return ADC_CH7;
	return 0xFF;	// Not an ADC pin
}
static bool AlxAdc_Ctor_AreIoPinsAndChOk(AlxAdc* me)
{
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		if (AlxAdc_GetIoPinCh(*(me->ioPinArr + i)) != AlxAdc_GetCh(*(me->chArr + i)))
		{
			return false;
		}
	}
	return true;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC17XX)
