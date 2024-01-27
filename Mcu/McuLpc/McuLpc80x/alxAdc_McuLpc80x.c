/**
  ******************************************************************************
  * @file		alxAdc_McuLpc80x.c
  * @brief		Auralix C Library - ALX ADC MCU LPC80X Module
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
#include "alxAdc_McuLpc80x.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))


//******************************************************************************
// Private Preprocessor
//******************************************************************************
#if defined(ALX_LPC80X)
#define ALX_ADC_LPC8XX ADC
#endif
#if defined(ALX_LPC84X)
#define ALX_ADC_LPC8XX ADC0
#endif


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch);
static bool AlxAdc_Ctor_IsSysClkOk(AlxAdc* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ioPinArr
  * @param[in]		chArr
  * @param[in]		numOfIoPinsAndCh
  * @param[in]		clk
  * @param[in]		vRef_mV
  * @param[in]		vRef_V
  */
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	AlxClk* clk,
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
	me->clk = clk;
	#if defined(ALX_ADC_OPTIMIZE_SIZE)
	me->vRef_mV = vRef_mV;
	#else
	me->vRef_V = vRef_V;
	#endif

	// Check channel sequence
	for (uint32_t i = 0; i < numOfIoPinsAndCh - 1; i++) ALX_ADC_ASSERT(AlxAdc_GetCh(me, chArr[i]) < AlxAdc_GetCh(me, chArr[i + 1]));	// Channel sequence must be from low to high number

	// Check clock
	ALX_ADC_ASSERT(AlxAdc_Ctor_IsSysClkOk(me));

	// Variables
	me->adcConfig.clockDividerNumber = 0;					// MF: Clkdiv = 0 ensures that sampling rate si appropriate (see User Manual)
	me->adcConfig.enableLowPowerMode = false;
	#if defined(ALX_LPC84X)
	me->adcConfig.clockMode = kADC_ClockSynchronousMode;	// MF: Doesn't work on Lpc804
	me->adcConfig.voltageRange = kADC_HighVoltageRange;		// MF: Doesn't work on Lpc804
	#endif

	me->adcConvSeqConfig.channelMask = 0U;
	me->adcConvSeqConfig.triggerMask = 0U;
	me->adcConvSeqConfig.triggerPolarity = kADC_TriggerPolarityNegativeEdge;
	me->adcConvSeqConfig.enableSyncBypass = false;
	me->adcConvSeqConfig.enableSingleStep = false;
	me->adcConvSeqConfig.interruptMode = kADC_InterruptForEachConversion;

	me->adcResult.result = 0U;
	me->adcResult.thresholdCompareStatus = kADC_ThresholdCompareInRange;
	me->adcResult.thresholdCorssingStatus = kADC_ThresholdCrossingNoDetected;
	me->adcResult.channelNumber = 0U;
	me->adcResult.overrunFlag = false;

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

	// Reset ADC periphery
	RESET_PeripheralReset(kADC_RST_N_SHIFT_RSTn);

	// Init ADC periphery
	CLOCK_Select(kADC_Clk_From_Fro);
	CLOCK_SetClkDivider(kCLOCK_DivAdcClk, me->adcConfig.clockDividerNumber);	// MF: Clkdiv = 0 ensures that sampling rate si appropriate
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);											// MF: Disabling PD gives power!!!

	// Calibrate ADC
	#if defined(ALX_LPC84X)
//	while(ADC_DoSelfCalibration(ALX_ADC_LPC8XX, (CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk))) == false) {}	// TV: TODO, seems like we need to configure ADC clock to 500 kHz prior calibration..
	#endif

	// Init ADC
	ADC_Init(ALX_ADC_LPC8XX, &me->adcConfig);

	// Init ADC channels
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)							// MF: Loop through channels to add appropriate channelMask
	{
		me->adcConvSeqConfig.channelMask |= (1U << AlxAdc_GetCh(me, me->chArr[i]));
	}

	// Init ADC coversion sequence
	ADC_SetConvSeqAConfig(ALX_ADC_LPC8XX, &me->adcConvSeqConfig);
	ADC_EnableConvSeqA(ALX_ADC_LPC8XX, true);

	// Clear result register
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC8XX);
	ADC_GetConvSeqAGlobalConversionResult(ALX_ADC_LPC8XX, &me->adcResult);		// MF: Clears global ADC conversion infomation of sequence A.

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

	// DeInit ADC
	ADC_Deinit(ALX_ADC_LPC8XX);

	// Clear all ADC registers and both coversion sequence registers
	SYSCON->PRESETCTRL0 &= ~(1U << 24U);
	SYSCON->PRESETCTRL0 |= (1U << 24U);

	// DeInit ADC periphery
	POWER_EnablePD(kPDRUNCFG_PD_ADC0);

	// DeInit GPIO
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)
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
	// Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// Optimize Guard
	#if defined(ALX_ADC_OPTIMIZE_SIZE)
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else

	// Trigger
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC8XX);

	// Wait
	while (!ADC_GetChannelConversionResult(ALX_ADC_LPC8XX, AlxAdc_GetCh(me, ch), &me->adcResult)) {}

	// Prepare
	float voltage_V = ((me->adcResult.result * me->vRef_V) / 4095);

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

	// Trigger
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC8XX);

	// Wait
	while (!ADC_GetChannelConversionResult(ALX_ADC_LPC8XX, AlxAdc_GetCh(me, ch), &me->adcResult)) {}

	// Prepare
	uint32_t voltage_mV = ((me->adcResult.result * me->vRef_mV) / 4095);

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
	// MF: Lpc80x doesn't support internal TempSens_GetTemp

	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch)
{
	if (ch == Alx_Ch_0)		return  0;
	if (ch == Alx_Ch_1)		return  1;
	if (ch == Alx_Ch_2)		return  2;
	if (ch == Alx_Ch_3)		return  3;
	if (ch == Alx_Ch_4)		return  4;
	if (ch == Alx_Ch_5)		return  5;
	if (ch == Alx_Ch_6)		return  6;
	if (ch == Alx_Ch_7)		return  7;
	if (ch == Alx_Ch_8)		return  8;
	if (ch == Alx_Ch_9)		return  9;
	if (ch == Alx_Ch_10)	return 10;
	if (ch == Alx_Ch_11)	return 11;

	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}
static bool AlxAdc_Ctor_IsSysClkOk(AlxAdc* me)
{
	//------------------------------------------------------------------------------
	// LPC80X
	//------------------------------------------------------------------------------
	#if defined(ALX_LPC80X)
	if (15000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_AhbClk_Ctor))
		return true;
	else
		return false;

	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
	#endif


	//------------------------------------------------------------------------------
	// LPC84X
	//------------------------------------------------------------------------------
	#if defined(ALX_LPC84X)
	if (30000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_AhbClk_Ctor))
		return true;
	else
		return false;

	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
	#endif
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))
