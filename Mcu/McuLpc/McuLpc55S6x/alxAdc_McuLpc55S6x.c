/**
  ******************************************************************************
  * @file		alxAdc_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX ADC MCU LPC55S6X Module
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
#include "alxAdc_McuLpc55S6x.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch);
static bool AlxAdc_Ctor_CheckCh(AlxAdc* me);
static bool AlxAdc_Ctor_IsMainClkOk(AlxAdc* me);
static void AlxAdc_SetClkDiv(AlxAdc* me);
static lpadc_sample_channel_mode_t AlxAdc_SetSampleChannelMode(AlxAdc* me, Alx_Ch ch);


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
	AlxAdc_Clk adcClk,
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
	(void)adcClk;
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
	me->adcClk = adcClk;
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	me->vRef_mV = vRef_mV;
	#else
	me->vRef_V = vRef_V;
	#endif

	// Check channel sequence
	for (uint32_t i = 0; i < (uint32_t)(numOfIoPinsAndCh - 1); i++) ALX_ADC_ASSERT(AlxAdc_GetCh(me, chArr[i]) <= AlxAdc_GetCh(me, chArr[i + 1]));	// MF: Channel sequence must be from low to high number

	// Check if right channels are used
	ALX_ADC_ASSERT(AlxAdc_Ctor_CheckCh(me));

	// Check if right MainClk is configured
	ALX_ADC_ASSERT(AlxAdc_Ctor_IsMainClkOk(me));

	// Variables										// MF: Everything is set to default (see "void LPADC_GetDefault___()" functions) except "adcConfig.referenceVoltageSource" and "adcConvTrigConfig.targetCommandId"
	me->adcConfig.enableInDozeMode						= true;
	me->adcConfig.conversionAverageMode					= kLPADC_ConversionAverage1;
	me->adcConfig.enableAnalogPreliminary				= false;
	me->adcConfig.powerUpDelay							= 0x80;
	me->adcConfig.referenceVoltageSource				= kLPADC_ReferenceVoltageAlt2;	// MF: Sets REFSEL to VREFH = voltage on VDDA pin in CFG Register
	me->adcConfig.powerLevelMode						= kLPADC_PowerLevelAlt1;
	me->adcConfig.triggerPriorityPolicy					= kLPADC_TriggerPriorityPreemptImmediately;
	me->adcConfig.enableConvPause						= false;
	me->adcConfig.convPauseDelay						= 0U;
	me->adcConfig.FIFO0Watermark						= 0U;
	me->adcConfig.FIFO1Watermark						= 0U;

	me->adcConvCommConfig.sampleChannelMode				= kLPADC_SampleChannelSingleEndSideB;
	me->adcConvCommConfig.channelNumber					= 0U;	// MF: Channel is set before every trigger in "AlxAdc_GetVoltage_x()"
	me->adcConvCommConfig.chainedNextCommandNumber		= 0U;
	me->adcConvCommConfig.enableAutoChannelIncrement	= false;
	me->adcConvCommConfig.loopCount						= 0U;
	me->adcConvCommConfig.hardwareAverageMode			= kLPADC_HardwareAverageCount1;
	me->adcConvCommConfig.sampleTimeMode				= kLPADC_SampleTimeADCK3;
	me->adcConvCommConfig.hardwareCompareMode			= kLPADC_HardwareCompareDisabled;
	me->adcConvCommConfig.hardwareCompareValueHigh		= 0U;
	me->adcConvCommConfig.hardwareCompareValueLow		= 0U;
	me->adcConvCommConfig.conversionResolutionMode		= kLPADC_ConversionResolutionStandard;
	me->adcConvCommConfig.enableWaitTrigger				= false;

	me->adcConvTrigConfig.targetCommandId				= 1U;	// MF: 0U is not valid, 1U means trigger will be executed on CMD1 (the one we will use) (see User Manual page 777 and alxWiki)
	me->adcConvTrigConfig.delayPower					= 0U;
	me->adcConvTrigConfig.priority						= 0U;
	me->adcConvTrigConfig.channelAFIFOSelect			= 0U;
	me->adcConvTrigConfig.channelBFIFOSelect			= 0U;
	me->adcConvTrigConfig.enableHardwareTrigger			= false;

	me->adcConvResult.commandIdSource					= 0U;
	me->adcConvResult.loopCountIndex					= 0U;
	me->adcConvResult.triggerIdSource					= 0U;
	me->adcConvResult.convValue							= 0U;

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
	(void)me;

	// #1 Init one pin for each channel
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		AlxIoPin_Init(*(me->ioPinArr + i));
		IOCON->PIO[me->ioPinArr[i]->port][me->ioPinArr[i]->pin] |= (0x1 << 10U);	// MF: Set ASW bit meaning analog switch is enabled
	}

	// #2 Reset ADC Periphery
	RESET_PeripheralReset(kADC0_RST_SHIFT_RSTn);

	// #3 Init Clk, Power
	CLOCK_AttachClk(kMAIN_CLK_to_ADC_CLK);
	POWER_DisablePD(kPDRUNCFG_PD_LDOGPADC);
	AlxAdc_SetClkDiv(me);

	// #4 Init ADC
	LPADC_Init(ADC0, &me->adcConfig);	// MF: "EnableClk" happens here

	// #5 Calibration
	LPADC_EnableOffsetCalibration(ADC0, true);
	LPADC_DoAutoCalibration(ADC0);

	// #6 Set conversion CMD configuration
	LPADC_SetConvCommandConfig(ADC0, 1U, &me->adcConvCommConfig);	// MF: 1U means CMD1 is used

	// #7 Set Trigger TCTRL Config
	LPADC_SetConvTriggerConfig(ADC0, 0U, &me->adcConvTrigConfig);	// MF: 0U means TCTRL0 is used

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
	(void)me;

	// #1 DeInit Adc
	LPADC_Deinit(ADC0);		// MF: "DisableClk" happens here

	// #2 DeInit Power
	POWER_EnablePD(kPDRUNCFG_PD_LDOGPADC);

	// #3 DeInit pin for each channel
	for (uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++) AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #4 Reset isInit
	me->isInit = false;

	// #5 Return OK
	return Alx_Ok;
}
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)ch;

	// Optimize Guard
	#if defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL)
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else

	// #1 Set Channel
	me->adcConvCommConfig.sampleChannelMode = AlxAdc_SetSampleChannelMode(me, ch);
	me->adcConvCommConfig.channelNumber = AlxAdc_GetCh(me, ch);
	LPADC_SetConvCommandConfig(ADC0, 1U, &me->adcConvCommConfig);			// MF: 1U means CMD1 is used

	// #2 Return Voltage
	LPADC_DoSoftwareTrigger(ADC0, 1U);										// MF: It has to be 1 I don't understand why
	while (!LPADC_GetConvResult(ADC0, &me->adcConvResult, 0U)) {}			// MF: 0U is for FIFO A and it is used for "Single ended" comvertion mode that we are using
	return (((me->adcConvResult.convValue >> 3U) * me->vRef_V) / 4095);	// MF: When 12-bit single ended resolution is used, first 3 bits are cleared, that's why we need to shift for 3U (see User Manual page 782)

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
	#endif
}
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)ch;

	// Optimize Guard
	#if !(defined(ALX_ADC_OPTIMIZE_SIZE) || defined(ALX_OPTIMIZE_SIZE_ALL))
	ALX_ADC_ASSERT(false);
	return ALX_NULL;
	#else

	// #1 Set Channel
	me->adcConvCommConfig.sampleChannelMode = AlxAdc_SetSampleChannelMode(me, ch);
	me->adcConvCommConfig.channelNumber = AlxAdc_GetCh(me, ch);
	LPADC_SetConvCommandConfig(ADC0, 1U, &me->adcConvCommConfig);			// MF: 1U means CMD1 is used

	// #2 Return Voltage
	LPADC_DoSoftwareTrigger(ADC0, 1U);										// MF: It has to be 1 I don't understand why
	while (!LPADC_GetConvResult(ADC0, &me->adcConvResult, 0U)) {}			// MF: 0U is for FIFO A and it is used for "Single ended" comvertion mode that we are using
	return (((me->adcConvResult.convValue >> 3U) * me->vRef_mV) / 4095);	// MF: When 12-bit single ended resolution is used, first 3 bits are cleared, that's why we need to shift for 3U (see User Manual page 782)

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
	#endif
}
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// Assert
	(void)me;

	// TODO

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	(void)me;
	(void)ch;

	// #1 Return Ch
	if (ch == Alx_Ch_0)		{ return 0; }
	if (ch == Alx_Ch_1)		{ return 1; }
	if (ch == Alx_Ch_2)		{ return 2; }
	if (ch == Alx_Ch_3)		{ return 3; }
	if (ch == Alx_Ch_4)		{ return 4; }

	if (ch == Alx_Ch_8)		{ return 0; }	// MF: The LPC55S6x ADC channels. See alxWiki for explanation
	if (ch == Alx_Ch_9)		{ return 1; }
	if (ch == Alx_Ch_10)	{ return 2; }
	if (ch == Alx_Ch_11)	{ return 3; }
	if (ch == Alx_Ch_12)	{ return 4; }

	// Assert
	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}
static bool AlxAdc_Ctor_CheckCh(AlxAdc* me)
{
	// Assert
	(void)me;

	// #1 Check IoPins
	for (uint32_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		if (!(	(me->chArr[i] == Alx_Ch_0)	||
				(me->chArr[i] == Alx_Ch_1)	||
				(me->chArr[i] == Alx_Ch_2)	||
				(me->chArr[i] == Alx_Ch_3)	||
				(me->chArr[i] == Alx_Ch_4)	||
				(me->chArr[i] == Alx_Ch_8)	||
				(me->chArr[i] == Alx_Ch_9)	||
				(me->chArr[i] == Alx_Ch_10)	||
				(me->chArr[i] == Alx_Ch_11)	||
				(me->chArr[i] == Alx_Ch_12)	))	{ return false; }
	}

	// #2 Return
	return true;
}
static bool AlxAdc_Ctor_IsMainClkOk(AlxAdc* me)
{
	// Assert
	(void)me;

	// #1 Check Clk
	if (me->adcClk == AlxAdc_Clk_McuLpc55S6x_AdcClk_12MHz_MainClk_12MHz)
	{
		if (12000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55s6x_MainClk_Ctor))	{ return true;  }
		else																				{ return false; }
	}
	if (me->adcClk == AlxAdc_Clk_McuLpc55S6x_AdcClk_24MHz_MainClk_96MHz)
	{
		if (96000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55s6x_MainClk_Ctor))	{ return true;  }
		else																				{ return false; }
	}
	if (me->adcClk == AlxAdc_Clk_McuLpc55S6x_AdcClk_18MHz75_MainClk_150MHz)
	{
		if (150000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55s6x_MainClk_Ctor))	{ return true;  }
		else																				{ return false; }
	}

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return false;
}
static void AlxAdc_SetClkDiv(AlxAdc* me)
{
	// Assert
	(void)me;

	// #1 Set Clk Div
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)	{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)			{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0)	{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)			{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz)			{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz)			{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }
	if (me->clk->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz)			{ CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, (uint32_t)me->adcClk, true); return; }

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return;
}
static lpadc_sample_channel_mode_t AlxAdc_SetSampleChannelMode(AlxAdc* me, Alx_Ch ch)
{
	// Assert
	(void)me;
	(void)ch;

	// #1 Check if channels mode A
	if ((ch == Alx_Ch_0) || (ch == Alx_Ch_1) || (ch == Alx_Ch_2 ) || (ch == Alx_Ch_3 ) || (ch == Alx_Ch_4 ))	{ return kLPADC_SampleChannelSingleEndSideA; }

	// #2 Check if channels mode B
	if ((ch == Alx_Ch_8) || (ch == Alx_Ch_9) || (ch == Alx_Ch_10) || (ch == Alx_Ch_11) || (ch == Alx_Ch_12))	{ return kLPADC_SampleChannelSingleEndSideB; }

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
