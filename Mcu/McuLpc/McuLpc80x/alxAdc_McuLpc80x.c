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
#if defined(ALX_C_LIB) && defined(ALX_LPC80X)


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
void AlxAdc_Ctor
(
	AlxAdc* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	AlxClk* clk,
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	uint32_t vRef_mV
	#else
	float vRef_V
	#endif
)
{
	// Assert
	(void)me;
	(void)ioPinArr;
	(void)chArr;
	ALX_ADC_ASSERT(numOfIoPinsAndCh <= ALX_ADC_BUFF_LEN);
	(void)clk;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	(void)vRef_mV;
	#else
	(void)vRef_V;
	#endif

	// Objects - External
	me->ioPinArr = ioPinArr;
	me->clk = clk;

	// Parameters
	me->chArr = chArr;
	me->numOfIoPinsAndCh = numOfIoPinsAndCh;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	me->vRef_mV = vRef_mV;
	#else
	me->vRef_V = vRef_V;
	#endif

	// Check channel sequence
	for (uint32_t i = 0; i < numOfIoPinsAndCh - 1; i++) ALX_ADC_ASSERT(AlxAdc_GetCh(me, chArr[i]) < AlxAdc_GetCh(me, chArr[i + 1])); // Channel sequence must be from low to high number

	// Check clock
	ALX_ADC_ASSERT(AlxAdc_Ctor_IsSysClkOk(me));

	// Variables
	//me->adcConfig.clockMode = kADC_ClockSynchronousMode;	// MF: Doesn't work on Lpc804
	me->adcConfig.clockDividerNumber = 0;					// MF: Clkdiv = 0 ensures that sampling rate si appropriate (see User Manual)
	me->adcConfig.enableLowPowerMode = false;
	//me->adcConfig.voltageRange = kADC_HighVoltageRange;	// MF: Doesn't work on Lpc804

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
Alx_Status AlxAdc_Init(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Init one pin for each channel
	for(uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++)
		AlxIoPin_Init(*(me->ioPinArr + i));

	// #2 Reset ADC Periphery
	RESET_PeripheralReset(kADC_RST_N_SHIFT_RSTn);

	// #3 Init Clk, Power
	CLOCK_Select(kADC_Clk_From_Fro);
	CLOCK_SetClkDivider(kCLOCK_DivAdcClk, me->adcConfig.clockDividerNumber);	// MF: Clkdiv = 0 ensures that sampling rate si appropriate
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);											// MF: Disabling PD gives power!!!

	// #4 Calibration
	#if defined(ALX_LPC84x)
	while(ADC_DoSelfCalibration(ALX_ADC_LPC_8XX, (CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk))) == false) {}	// Doesn't work on Lpc804
	#endif

	// #5 Configure ADC
	ADC_Init(ALX_ADC_LPC8XX, &me->adcConfig);

	// #6.1 Configure Ch for Coversion Cequence A
	for(uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++)							// MF: Loop through channels to add appropriate channelMask
		me->adcConvSeqConfig.channelMask |= (1U << AlxAdc_GetCh(me, me->chArr[i]));

	// #6.2 Configure and Enable Coversion Sequence A
	ADC_SetConvSeqAConfig(ALX_ADC_LPC8XX, &me->adcConvSeqConfig);
	ADC_EnableConvSeqA(ALX_ADC_LPC8XX, true);

	// #7 Clear result register
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC8XX);
	ADC_GetConvSeqAGlobalConversionResult(ALX_ADC_LPC8XX, &me->adcResult);		// MF: Clears global ADC conversion infomation of sequence A.

	// #8 Set isInit
	me->isInit = true;

	// #9 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdc_DeInit(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit Clk
	ADC_Deinit(ALX_ADC_LPC8XX);

	// #2 Clears all ADC registers and BOTH ConvSeq registers
	SYSCON->PRESETCTRL0 &= ~(1U << 24U);
	SYSCON->PRESETCTRL0 |= (1U << 24U);

	// #3 DeInit Power
	POWER_EnablePD(kPDRUNCFG_PD_ADC0);

	// #4 DeInit pin for each channel
	for(uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++)
		AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #5 Reset isInit
	me->isInit = false;

	// #6 Return OK
	return Alx_Ok;
}
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch)
{
	// Optimize Guard
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else

	// Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Return Voltage
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC8XX);
	while (!ADC_GetChannelConversionResult(ALX_ADC_LPC8XX, AlxAdc_GetCh(me, ch), &me->adcResult)) {}
	return ((me->adcResult.result * me->vRef_V) / 4095);

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
	#endif
}
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	// Optimize Guard
	#if !(defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL))
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else

	// Init Ctor Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Return Voltage
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC8XX);
	while (!ADC_GetChannelConversionResult(ALX_ADC_LPC8XX, AlxAdc_GetCh(me, ch), &me->adcResult)) {}
	return ((me->adcResult.result * me->vRef_mV) / 4095);

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
	#endif
}
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// MF: Lpc80x doesn't support internal TempSens_GetTemp

	ALX_ADC_ASSERT(false); // We shouldn't get here
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
	#if defined(ALX_LPC80X)
	if (15000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor))
		return true;
	else
		return false;

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
	#endif

	#if defined(ALX_LPC81X) || defined(ALX_LPC82X) || defined(ALX_LPC83X) || defined(ALX_LPC84X)
	// TODO
	#endif
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)
