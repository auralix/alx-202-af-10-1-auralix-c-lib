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
#include "alxDac_Mcu.h"
#include "alxDac.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxDacMcu_GetCh(Alx_Ch ch);
static void AlxDacMcu_Periph_EnableClk(AlxDac_Mcu* me);
static void AlxDacMcu_Periph_DisableClk(AlxDac_Mcu* me);
static void AlxDacMcu_Periph_ForceReset(AlxDac_Mcu* me);
static void AlxDacMcu_Periph_ReleaseReset(AlxDac_Mcu* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxDacMcu_Ctor
(
	AlxDac_Mcu* me,
	DAC_TypeDef* dac,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	float* setVoltageDefaultArr_V,
	uint8_t numOfCh,
	bool isVrefInt_V,
	float vrefExt_V
)
{
	// Assert
	(void)me;
	(void)dac;
	(void)ioPinArr;
	(void)chArr;
	(void)setVoltageDefaultArr_V;
	ALX_DAC_MCU_ASSERT(numOfCh <= ALX_DAC_BUFF_LEN);
	(void)isVrefInt_V;
	(void)vrefExt_V;
	for (uint8_t i = 0; i < numOfCh - 1; i++) ALX_DAC_MCU_ASSERT(chArr[i] < chArr[i + 1]); // Channel sequence must be from low to high number

	// Objects - External
	me->ioPinArr = ioPinArr;

	// Parameters
	me->chArr = chArr;
	me->setVoltageDefaultArr_V = setVoltageDefaultArr_V;
	me->numOfCh = numOfCh;
	me->resolution = DAC_ALIGN_12B_R;
	me->isVrefInt_V = isVrefInt_V;
	me->vrefExt_V = vrefExt_V;

	// Variables
	// DAC Common
	me->hdac.Instance = dac;

	// DAC Channel
	for (uint32_t buffPos = 0; buffPos < me->numOfCh; buffPos++)
	{
		me->chdac[buffPos].DAC_Trigger = DAC_TRIGGER_NONE;
		me->chdac[buffPos].DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
		me->ch[buffPos] = *(me->chArr + buffPos);
		me->setVoltageDefault_V[buffPos] = *(me->setVoltageDefaultArr_V + buffPos);

		#if defined(STM32G4)
			ALX_DAC_MCU_ASSERT(false); // Not yet implemented!!!
		#endif
	}

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxDacMcu_Init(AlxDac_Mcu* me)
{
	ALX_DAC_MCU_ASSERT(me->isInit == false);
	ALX_DAC_MCU_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Release DAC Periphery Reset
	AlxDacMcu_Periph_ReleaseReset(me);

	// #3 Enable DAC Periphery Clock
	AlxDacMcu_Periph_EnableClk(me);

	// #4 Init DAC
	if(HAL_DAC_Init(&me->hdac) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrInit"); return Alx_Err; }

	// #5 Init DAC Channels
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(HAL_DAC_ConfigChannel(&me->hdac, &me->chdac[i], AlxDacMcu_GetCh(me->ch[i])) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrInitCh"); return Alx_Err; }

	// #6 Set isInit
	me->isInit = true;

	// #7 Set DAC Channels Default Set Voltage
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(AlxDacMcu_SetVoltage_V(me, &me->ch[i], &me->setVoltageDefault_V[i]) != Alx_Ok) { ALX_DAC_MCU_TRACE("ErrSetDuty"); return Alx_Err; }

	// #8 Start DAC Channels
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(HAL_DAC_Start(&me->hdac, AlxDacMcu_GetCh(me->ch[i])) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrStart"); return Alx_Err; }

	// #9 Return OK
	return Alx_Ok;
}
Alx_Status AlxDacMcu_Init_CalibrateVref(AlxDac_Mcu* me, float* vref_V)
{
	ALX_DAC_MCU_ASSERT(me->isInit == false);
	ALX_DAC_MCU_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_Init((*(me->ioPinArr + i)));

	// #2 Release DAC Periphery Reset
	AlxDacMcu_Periph_ReleaseReset(me);

	// #3 Enable DAC Periphery Clock
	AlxDacMcu_Periph_EnableClk(me);

	// #4 Init DAC
	if(HAL_DAC_Init(&me->hdac) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrInit"); return Alx_Err; }

	// #5 Init DAC Channels
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(HAL_DAC_ConfigChannel(&me->hdac, &me->chdac[i], AlxDacMcu_GetCh(me->ch[i])) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrInitCh"); return Alx_Err; }

	// #6 Set isInit
	me->isInit = true;

	// #7 Set DAC Channels Default Set Voltage
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(AlxDacMcu_SetVoltage_V_CalibrateVref(me, &me->ch[i], &me->setVoltageDefault_V[i], vref_V) != Alx_Ok) { ALX_DAC_MCU_TRACE("ErrSetDuty"); return Alx_Err; }

	// #8 Start DAC Channels
	for(uint32_t i = 0; i < me->numOfCh; i++)
		if(HAL_DAC_Start(&me->hdac, AlxDacMcu_GetCh(me->ch[i])) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrStart"); return Alx_Err; }

	// #9 Return OK
	return Alx_Ok;
}
Alx_Status AlxDacMcu_DeInit(AlxDac_Mcu* me)
{
	ALX_DAC_MCU_ASSERT(me->isInit == true);
	ALX_DAC_MCU_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit DAC
	if(HAL_DAC_DeInit(&me->hdac) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrDeInit"); return Alx_Err; }

	// #2 Force DAC Periphery Reset
	AlxDacMcu_Periph_ForceReset(me);

	// #3 Disable DAC Periphery Clock
	AlxDacMcu_Periph_DisableClk(me);

	// #4 DeInit GPIO
	for (uint32_t i = 0; i < me->numOfCh; i++)
		AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #5 Reset isInit
	me->isInit = false;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxDacMcu_SetVoltage_V(AlxDac_Mcu* me, Alx_Ch* ch, float* voltage_V)
{
	ALX_DAC_MCU_ASSERT(me->isInit == true);
	ALX_DAC_MCU_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_MCU_ASSERT(me->isVrefInt_V == false);

	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == *ch)
		{
			// #1 Bond Set Voltage
			float dacVoltage_V = *voltage_V;
			if(AlxGlobal_BoundFloat(&dacVoltage_V, 0, me->vrefExt_V) != Alx_Ok) { ALX_DAC_MCU_TRACE("ErrBond"); return Alx_Err; }

			// #2 Get Channel
			uint32_t dacCh = AlxDacMcu_GetCh(*ch);

			// #3 Get Data
			uint32_t dacVoltage_mV = (uint32_t)(dacVoltage_V * 1000.f);
			uint32_t vrefExt_mV = (uint32_t)(me->vrefExt_V * 1000.f);
			uint32_t dacData = __LL_DAC_CALC_VOLTAGE_TO_DATA(vrefExt_mV, dacVoltage_mV, me->resolution);

			// #4 Set Voltage
			if(HAL_DAC_SetValue(&me->hdac, dacCh, me->resolution, dacData) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrSetVal"); return Alx_Err; }

			// #5 Return OK
			return Alx_Ok;
		}
	}

	ALX_DAC_MCU_ASSERT(false); // We shouldn't get here
	return Alx_Err;
}
Alx_Status AlxDacMcu_SetVoltage_V_CalibrateVref(AlxDac_Mcu* me, Alx_Ch* ch, float* voltage_V, float* vref_V)
{
	ALX_DAC_MCU_ASSERT(me->isInit == true);
	ALX_DAC_MCU_ASSERT(me->wasCtorCalled == true);
	ALX_DAC_MCU_ASSERT(me->isVrefInt_V == true);

	for (uint32_t i = 0; i < me->numOfCh; i++)
	{
		if (me->ch[i] == *ch)
		{
			// #1 Bond Set Voltage
			float dacVoltage_V = *voltage_V;
			if(AlxGlobal_BoundFloat(&dacVoltage_V, 0, *vref_V) != Alx_Ok) { ALX_DAC_MCU_TRACE("ErrBond"); return Alx_Err; }

			// #2 Get Channel
			uint32_t dacCh = AlxDacMcu_GetCh(*ch);

			// #3 Get Data
			uint32_t dacVoltage_mV = (uint32_t)(dacVoltage_V * 1000.f);
			uint32_t vref_mV = (uint32_t)(*vref_V * 1000.f);
			uint32_t dacData = __LL_DAC_CALC_VOLTAGE_TO_DATA(vref_mV, dacVoltage_mV, me->resolution);

			// #4 Set Voltage
			if(HAL_DAC_SetValue(&me->hdac, dacCh, me->resolution, dacData) != HAL_OK) { ALX_DAC_MCU_TRACE("ErrSetVal"); return Alx_Err; }

			// #5 Return OK
			return Alx_Ok;
		}
	}

	ALX_DAC_MCU_ASSERT(false); // We shouldn't get here
	return Alx_Err;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxDacMcu_GetCh(Alx_Ch ch)
{
	if(ch == Alx_Ch_1 ) return DAC_CHANNEL_1;
	#if defined(STM32F4) || \
		defined(STM32L072xx) || defined(STM32L073xx) || defined(STM32L082xx) || defined(STM32L083xx)
	if(ch == Alx_Ch_2 ) return DAC_CHANNEL_2;
	#endif

	ALX_DAC_ASSERT(false); // We shouldn't get here
	return 0;
}
static void AlxDacMcu_Periph_EnableClk(AlxDac_Mcu* me)
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

	if(isErr)						{ ALX_DAC_MCU_ASSERT(false); } // We shouldn't get here
}
static void AlxDacMcu_Periph_DisableClk(AlxDac_Mcu* me)
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

	if(isErr)						{ ALX_DAC_MCU_ASSERT(false); } // We shouldn't get here
}
static void AlxDacMcu_Periph_ForceReset(AlxDac_Mcu* me)
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

	if(isErr)						{ ALX_DAC_MCU_ASSERT(false); } // We shouldn't get here
}
static void AlxDacMcu_Periph_ReleaseReset(AlxDac_Mcu* me)
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

	if(isErr)						{ ALX_DAC_MCU_ASSERT(false); } // We shouldn't get here
}


#endif
