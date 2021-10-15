/**
  ******************************************************************************
  * @file alxAdc_McuLpc84.c
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdc_McuLpc84.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC84)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdcMcu_GetChannel(AlxIoPin* pin);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxAdcMcu_Ctor
(
	AlxAdc_Mcu* me,
	AlxIoPin** channels,
	uint8_t numChannels,
	uint8_t adcClkDiv,
	uint16_t voltageRefP_mV)
{
	// Objects - External
	me->channels = channels;

	// Parameters
	me->numChannels = numChannels;
	me->adcClkDiv = adcClkDiv;
	me->voltageRefP_mV = voltageRefP_mV;

	// Variables
	me->adcConfig.clockMode = kADC_ClockSynchronousMode;
	me->adcConfig.clockDividerNumber = me->adcClkDiv;
	me->adcConfig.enableLowPowerMode = false;
	me->adcConfig.voltageRange = kADC_HighVoltageRange;

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
void AlxAdcMcu_Init(AlxAdc_Mcu* me)
{
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Init one pin for each channel
	for(uint8_t i = 0 ; i < me->numChannels ; i++)	{ AlxIoPin_Init((*(me->channels + i))); }

	// #3 Init Clk, Power
	CLOCK_Select(kADC_Clk_From_Fro);
	CLOCK_SetClkDivider(kCLOCK_DivAdcClk, me->adcClkDiv);
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);						// Disabling PD gives power!!!

	// #4 Calibration
	while(ADC_DoSelfCalibration(ADC0, (CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk))) == false) {}

	// #5 Configure ADC
	ADC_Init(ADC0, &me->adcConfig);

	// #6 Configure coversion sequence A
	for(uint8_t i = 0 ; i < me->numChannels ; i++)	// Loop through channels to add appropriate channelMask
	{
		me->adcConvSeqConfig.channelMask |= (1U << AlxAdcMcu_GetChannel(*(me->channels + i)));
	}
	ADC_SetConvSeqAConfig(ADC0, &me->adcConvSeqConfig);
	ADC_EnableConvSeqA(ADC0, true);

	// #7 Clear result register
	ADC_DoSoftwareTriggerConvSeqA(ADC0);
	ADC_GetConvSeqAGlobalConversionResult(ADC0, &me->adcResult); 	// Clears global ADC conversion infomation of sequence A.

	ALX_ADC_ASSERT(me->isInit == true);
}
void AlxAdcMcu_DeInit(AlxAdc_Mcu* me)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit Clk
	ADC_Deinit(ADC0);

	// #2 Clears all ADC registers and BOTH ConvSeq registers
	SYSCON->PRESETCTRL0 &= ~(1U << 24U);
	SYSCON->PRESETCTRL0 |= (1U << 24U);

	// #3 DeInit Power
	POWER_EnablePD(kPDRUNCFG_PD_ADC0);

	// #4 DeInit pin for each channel
	for(uint8_t i = 0 ; i < me->numChannels ; i++)	{ AlxIoPin_DeInit((*(me->channels + i))); }

	// #4 Set isInit attribute
	me->isInit = false;
}
uint32_t AlxAdcMcu_GetVoltage_mV(AlxAdc_Mcu* me, AlxIoPin* pin)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	uint8_t ch = AlxAdcMcu_GetChannel(pin);
	ADC_DoSoftwareTriggerConvSeqA(ADC0);
	while (!ADC_GetChannelConversionResult(ADC0, ch, &me->adcResult)) {}
	return ((me->adcResult.result * me->voltageRefP_mV) / 4095);	// Returns mV
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdcMcu_GetChannel(AlxIoPin* pin)
{
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN0)		return 0U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN1)		return 1U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN2)		return 2U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN3)		return 3U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN4)		return 4U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN5)		return 5U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN6)		return 6U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN7)		return 7U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN8)		return 8U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN9)		return 9U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN10)		return 10U;
	if (pin->swmFunc == AlxIoPin_SwmFunc_ADC_CHN11)		return 11U;

	else												return 0xFF;	// We shouldn't get here
}


#endif
