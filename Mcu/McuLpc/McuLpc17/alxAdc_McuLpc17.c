/**
  ******************************************************************************
  * @file alxAdc_McuLpc17.c
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdc_McuLpc17.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17)


//******************************************************************************
// Private Functions
//******************************************************************************
static ADC_CHANNEL_T AlxAdcMcu_GetChannel(AlxIoPin* pin);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxAdcMcu_Ctor
(
	AlxAdc_Mcu* me,
	AlxIoPin** channels,
	uint8_t numChannels,
	uint16_t voltageRefP_mV)
{
	// Obejcts - External
	me->channels = channels;

	// Parameters
	me->numChannels = numChannels;
	me->voltageRefP_mV = voltageRefP_mV;

	// Variables
	me->adcClkSetup.adcRate = 0;
	me->adcClkSetup.bitsAccuracy = 0;
	me->adcClkSetup.burstMode = false;
	me->adcResult = 0;

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
	for(uint8_t i = 0 ; i < me->numChannels ; i++)	{ AlxIoPin_Init(*(me->channels + i)); }

	// #3 Init Clk, Power, Control Register
	Chip_ADC_Init(LPC_ADC, &me->adcClkSetup);
	Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);
}
void AlxAdcMcu_DeInit(AlxAdc_Mcu* me)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = false;

	// #2 DeInit Clk, Power, Control Register
	Chip_ADC_DeInit(LPC_ADC);

	// #3 DeInit pin for each channel
	for(uint8_t i = 0 ; i < me->numChannels ; i++)	{ AlxIoPin_DeInit(*(me->channels + i)); }
}
uint32_t AlxAdcMcu_GetVoltage_mV(AlxAdc_Mcu* me, AlxIoPin* channel)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	ADC_CHANNEL_T ch = AlxAdcMcu_GetChannel(channel);

	// There must be Enable and Disable function for each chennel before convertion.
	// I tried only enabling channels in Init, but it doens't work if there is no disable
	Chip_ADC_EnableChannel(LPC_ADC, ch, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	while (Chip_ADC_ReadStatus(LPC_ADC, ch, ADC_DR_DONE_STAT) != SET) {}
	Chip_ADC_ReadValue(LPC_ADC, ch, &me->adcResult);
	Chip_ADC_EnableChannel(LPC_ADC, ch, DISABLE);

	return ((me->adcResult * me->voltageRefP_mV) / 4095); 	// Returns mV
}


//******************************************************************************
// Private Functions
//******************************************************************************
static ADC_CHANNEL_T AlxAdcMcu_GetChannel(AlxIoPin* pin)
{
	if (pin->port == 0 && pin->pin == 23)	return ADC_CH0;
	if (pin->port == 0 && pin->pin == 24)	return ADC_CH1;
	if (pin->port == 0 && pin->pin == 25)	return ADC_CH2;
	if (pin->port == 0 && pin->pin == 26)	return ADC_CH3;
	if (pin->port == 1 && pin->pin == 30)	return ADC_CH4;
	if (pin->port == 1 && pin->pin == 31)	return ADC_CH5;
	if (pin->port == 0 && pin->pin == 3)	return ADC_CH6;
	if (pin->port == 0 && pin->pin == 2)	return ADC_CH7;

	else									return 0xFF;
}


#endif
