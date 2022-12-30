/**
  ******************************************************************************
  * @file		alxDac_McuStm32.c
  * @brief		Auralix C Library - ALX DAC MCU STM32 Module
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
// Includes
//******************************************************************************
#include "alxDac_McuStm32.h"
#include "alxDac.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxDac_Init_Private(AlxDac* me, bool calibrateVref, float vref_V);
static Alx_Status AlxDac_SetVoltage_V_Private(AlxDac* me, Alx_Ch ch, float voltage_V, float vref_V);
static uint32_t AlxDac_GetCh(Alx_Ch ch);
static void AlxDac_Periph_EnableClk(AlxDac* me);
static void AlxDac_Periph_DisableClk(AlxDac* me);
static void AlxDac_Periph_ForceReset(AlxDac* me);
static void AlxDac_Periph_ReleaseReset(AlxDac* me);


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
)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	(void)me;
	(void)dac;
	(void)ioPinArr;
	(void)chArr;
	(void)setVoltageDefaultArr_V;
	ALX_DAC_ASSERT(numOfCh <= ALX_DAC_BUFF_LEN);
	(void)isVrefInt_V;
	(void)vrefExt_V;


	//------------------------------------------------------------------------------
	// Const
	//------------------------------------------------------------------------------
	me->RESOLUTION = DAC_ALIGN_12B_R;


	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	me->dac = dac;
	me->ioPinArr = ioPinArr;
	me->chArr = chArr;
	me->setVoltageDefaultArr_V = setVoltageDefaultArr_V;
	me->numOfCh = numOfCh;
	me->isVrefInt_V = isVrefInt_V;
	me->vrefExt_V = vrefExt_V;

	// Check channel sequence
	for (uint8_t i = 0; i < numOfCh - 1; i++)
	{
		ALX_DAC_ASSERT(chArr[i] < chArr[i + 1]);	// Channel sequence must be from low to high number
	}


	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------

	// DAC common
	me->hdac.Instance = dac;

	// DAC channel
	for (uint32_t buffPos = 0; buffPos < me->numOfCh; buffPos++)
	{
		me->ch[buffPos] = *(me->chArr + buffPos);
		me->setVoltageDefault_V[buffPos] = *(me->setVoltageDefaultArr_V + buffPos);

		#if defined(ALX_STM32F4) || defined(ALX_STM32L0)
		me->chdac[buffPos].DAC_Trigger = DAC_TRIGGER_NONE;
		me->chdac[buffPos].DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
		#endif

		#if defined(ALX_STM32L4)
		me->chdac[buffPos].DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_DISABLE;
		me->chdac[buffPos].DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
		me->chdac[buffPos].DAC_Trigger = DAC_TRIGGER_NONE;
		me->chdac[buffPos].DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
		me->chdac[buffPos].DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
		me->chdac[buffPos].DAC_UserTrimming = DAC_TRIMMING_FACTORY;
		me->chdac[buffPos].DAC_TrimmingValue = 1;
		me->chdac[buffPos].DAC_SampleAndHoldConfig.DAC_SampleTime = 0;
		me->chdac[buffPos].DAC_SampleAndHoldConfig.DAC_HoldTime = 0;
		me->chdac[buffPos].DAC_SampleAndHoldConfig.DAC_RefreshTime = 0;
		#endif

		#if defined(ALX_STM32G4)
		ALX_DAC_ASSERT(false);	// TODO - Not yet implemented
		#endif
	}


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxDac_Init(AlxDac* me)
{
	// Assert
	ALX_DAC_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_ASSERT(me->isInit == false);

	// Return
	return AlxDac_Init_Private(me, false, ALX_NULL);
}
Alx_Status AlxDac_Init_CalibrateVref(AlxDac* me, float vref_V)
{
	// Assert
	ALX_DAC_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_ASSERT(me->isInit == false);

	// Return
	return AlxDac_Init_Private(me, true, vref_V);
}
Alx_Status AlxDac_DeInit(AlxDac* me)
{
	// Assert
	ALX_DAC_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_ASSERT(me->isInit == true);

	// DeInit DAC
	if(HAL_DAC_DeInit(&me->hdac) != HAL_OK) { ALX_DAC_TRACE("Err"); return Alx_Err; }

	// Force DAC periphery reset
	AlxDac_Periph_ForceReset(me);

	// Disable DAC periphery clock
	AlxDac_Periph_DisableClk(me);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxDac_SetVoltage_V(AlxDac* me, Alx_Ch ch, float voltage_V)
{
	// Assert
	ALX_DAC_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_ASSERT(me->isInit == true);
	ALX_DAC_ASSERT(me->isVrefInt_V == false);

	// Return
	return AlxDac_SetVoltage_V_Private(me, ch, voltage_V, me->vrefExt_V);
}
Alx_Status AlxDac_SetVoltage_V_CalibrateVref(AlxDac* me, Alx_Ch ch, float voltage_V, float vref_V)
{
	// Assert
	ALX_DAC_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_ASSERT(me->isInit == true);
	ALX_DAC_ASSERT(me->isVrefInt_V == true);

	// Return
	return AlxDac_SetVoltage_V_Private(me, ch, voltage_V, vref_V);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxDac_Init_Private(AlxDac* me, bool calibrateVref, float vref_V)
{
	// Init GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Release DAC periphery reset
	AlxDac_Periph_ReleaseReset(me);

	// Enable DAC periphery clock
	AlxDac_Periph_EnableClk(me);

	// Init DAC
	if(HAL_DAC_Init(&me->hdac) != HAL_OK) { ALX_DAC_TRACE("Err"); return Alx_Err; }

	// Init DAC channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (HAL_DAC_ConfigChannel(&me->hdac, &me->chdac[i], AlxDac_GetCh(me->ch[i])) != HAL_OK) { ALX_DAC_TRACE("Err"); return Alx_Err; }
	}

	// Set isInit
	me->isInit = true;

	// Set DAC channels default voltage
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (calibrateVref)
		{
			if(AlxDac_SetVoltage_V_CalibrateVref(me, me->ch[i], me->setVoltageDefault_V[i], vref_V) != Alx_Ok) { ALX_DAC_TRACE("Err"); return Alx_Err; }
		}
		else
		{
			if (AlxDac_SetVoltage_V(me, me->ch[i], me->setVoltageDefault_V[i]) != Alx_Ok) { ALX_DAC_TRACE("Err"); return Alx_Err; }
		}
	}

	// Start DAC channels
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (HAL_DAC_Start(&me->hdac, AlxDac_GetCh(me->ch[i])) != HAL_OK) { ALX_DAC_TRACE("Err"); return Alx_Err; }
	}

	// Return
	return Alx_Ok;
}
static Alx_Status AlxDac_SetVoltage_V_Private(AlxDac* me, Alx_Ch ch, float voltage_V, float vref_V)
{
	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == ch)
		{
			// Local variables
			float dacVoltage_V = voltage_V;
			uint32_t dacVoltage_mV = (uint32_t)(dacVoltage_V * 1000.f);
			uint32_t vref_mV = (uint32_t)(vref_V * 1000.f);

			// Bound
			if(AlxBound_Float(&dacVoltage_V, 0, vref_V) != Alx_Ok) { ALX_DAC_TRACE("Err"); return Alx_Err; }

			// Get channel
			uint32_t dacCh = AlxDac_GetCh(ch);

			// Get data
			uint32_t dacData = __LL_DAC_CALC_VOLTAGE_TO_DATA(vref_mV, dacVoltage_mV, me->RESOLUTION);

			// Set voltage
			if(HAL_DAC_SetValue(&me->hdac, dacCh, me->RESOLUTION, dacData) != HAL_OK) { ALX_DAC_TRACE("Err"); return Alx_Err; }

			// Return
			return Alx_Ok;
		}
	}

	// Assert
	ALX_DAC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static uint32_t AlxDac_GetCh(Alx_Ch ch)
{
	if(ch == Alx_Ch_1 ) return DAC_CHANNEL_1;
	#if defined(ALX_STM32F4) || \
	defined(STM32L072xx) || defined(STM32L073xx) || defined(STM32L082xx) || defined(STM32L083xx) || \
	defined(STM32L431xx) || defined(STM32L432xx) || defined(STM32L433xx) || defined(STM32L442xx) || defined(STM32L443xx) || \
	defined(STM32L471xx) || defined(STM32L475xx) || defined(STM32L476xx) || defined(STM32L485xx) || defined(STM32L486xx) || defined(STM32L496xx) || defined(STM32L4A6xx) || \
	defined(STM32L4P5xx) || defined(STM32L4Q5xx) || \
	defined(STM32L4R5xx) || defined(STM32L4R7xx) || defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
	if(ch == Alx_Ch_2 ) return DAC_CHANNEL_2;
	#endif

	ALX_DAC_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

static void AlxDac_Periph_EnableClk(AlxDac* me)
{
	bool isErr = true;

	#if defined(DAC1)
	if (me->hdac.Instance == DAC1)	{ __HAL_RCC_DAC1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(DAC2)
	if (me->hdac.Instance == DAC2)	{ __HAL_RCC_DAC2_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(DAC3)
	if (me->hdac.Instance == DAC3)	{ __HAL_RCC_DAC3_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(DAC4)
	if (me->hdac.Instance == DAC4)	{ __HAL_RCC_DAC4_CLK_ENABLE(); isErr = false; }
	#endif

	if(isErr)						{ ALX_DAC_ASSERT(false); }	// We should not get here
}
static void AlxDac_Periph_DisableClk(AlxDac* me)
{
	bool isErr = true;

	#if defined(DAC1)
	if (me->hdac.Instance == DAC1)	{ __HAL_RCC_DAC1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(DAC2)
	if (me->hdac.Instance == DAC2)	{ __HAL_RCC_DAC2_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(DAC3)
	if (me->hdac.Instance == DAC3)	{ __HAL_RCC_DAC3_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(DAC4)
	if (me->hdac.Instance == DAC4)	{ __HAL_RCC_DAC4_CLK_DISABLE(); isErr = false; }
	#endif

	if(isErr)						{ ALX_DAC_ASSERT(false); }	// We should not get here
}
static void AlxDac_Periph_ForceReset(AlxDac* me)
{
	bool isErr = true;

	#if defined(DAC1)
	if (me->hdac.Instance == DAC1)	{ __HAL_RCC_DAC1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(DAC2)
	if (me->hdac.Instance == DAC2)	{ __HAL_RCC_DAC2_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(DAC3)
	if (me->hdac.Instance == DAC3)	{ __HAL_RCC_DAC3_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(DAC4)
	if (me->hdac.Instance == DAC4)	{ __HAL_RCC_DAC4_FORCE_RESET(); isErr = false; }
	#endif

	if(isErr)						{ ALX_DAC_ASSERT(false); }	// We should not get here
}
static void AlxDac_Periph_ReleaseReset(AlxDac* me)
{
	bool isErr = true;

	#if defined(DAC1)
	if (me->hdac.Instance == DAC1)	{ __HAL_RCC_DAC1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(DAC2)
	if (me->hdac.Instance == DAC2)	{ __HAL_RCC_DAC2_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(DAC3)
	if (me->hdac.Instance == DAC3)	{ __HAL_RCC_DAC3_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(DAC4)
	if (me->hdac.Instance == DAC4)	{ __HAL_RCC_DAC4_RELEASE_RESET(); isErr = false; }
	#endif

	if(isErr)						{ ALX_DAC_ASSERT(false); }	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))
