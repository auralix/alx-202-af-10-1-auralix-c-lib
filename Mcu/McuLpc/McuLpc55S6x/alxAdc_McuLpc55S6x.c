/**
  ******************************************************************************
  * @file alxAdc_McuLpc55S6x.c
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdc_McuLpc55S6x.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdc_GetCh(AlxAdc* me, Alx_Ch ch);
static bool AlxAdc_Ctor_IsSysClkOk(AlxAdc* me);
static bool AlxAdc_Ctor_CheckCh(AlxAdc* me);
lpadc_sample_channel_mode_t AlxAdc_SetSampleChannelMode(AlxAdc* me, Alx_Ch ch);


//******************************************************************************
// Specific Functions
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

	// Check if right channel
	ALX_ADC_ASSERT(AlxAdc_Ctor_CheckCh(me));

	// Check clock
	//ALX_ADC_ASSERT(AlxAdc_Ctor_IsSysClkOk(me));	// MF Assert je bil

	// Variables															// MF: Everything is set to default (see "void LPADC_GetDefault___()" functions) except "adcConfig.referenceVoltageSource" and "adcConvTrigConfig.targetCommandId"
	me->adcConfig.enableInDozeMode = true;
	me->adcConfig.conversionAverageMode = kLPADC_ConversionAverage1;
	me->adcConfig.enableAnalogPreliminary = false;
	me->adcConfig.powerUpDelay = 0x80;
	me->adcConfig.referenceVoltageSource = kLPADC_ReferenceVoltageAlt2;		// MF: Sets REFSEL beats to VREFH = voltage on VDDA pin in CFG Register
	me->adcConfig.powerLevelMode = kLPADC_PowerLevelAlt1;
	me->adcConfig.triggerPriorityPolicy = kLPADC_TriggerPriorityPreemptImmediately;
	me->adcConfig.enableConvPause = false;
	me->adcConfig.convPauseDelay = 0U;
	me->adcConfig.FIFO0Watermark = 0U;
	me->adcConfig.FIFO1Watermark = 0U;

	me->adcConvCommConfig.sampleChannelMode = kLPADC_SampleChannelSingleEndSideB;
	me->adcConvCommConfig.channelNumber = 0U;								// MF: Channel is set before every trigger in "AlxAdc_GetVoltage_mV()"
	me->adcConvCommConfig.chainedNextCommandNumber = 0U;
	me->adcConvCommConfig.enableAutoChannelIncrement = false;
	me->adcConvCommConfig.loopCount = 0U;
	me->adcConvCommConfig.hardwareAverageMode = kLPADC_HardwareAverageCount1;
	me->adcConvCommConfig.sampleTimeMode = kLPADC_SampleTimeADCK3;
	me->adcConvCommConfig.hardwareCompareMode = kLPADC_HardwareCompareDisabled;
	me->adcConvCommConfig.hardwareCompareValueHigh = 0U;
	me->adcConvCommConfig.hardwareCompareValueLow = 0U;
	me->adcConvCommConfig.conversionResolutionMode = kLPADC_ConversionResolutionStandard;
	me->adcConvCommConfig.enableWaitTrigger = false;

	me->adcConvTrigConfig.targetCommandId = 1U;								// MF: 0U is not valid, 1U means trigger will be executed on CMD1 (the one we will use) (see User Manual page 777 ans alxWiki)
	me->adcConvTrigConfig.delayPower = 0U;
	me->adcConvTrigConfig.priority = 0U;
	me->adcConvTrigConfig.channelAFIFOSelect = 0U;
	me->adcConvTrigConfig.channelBFIFOSelect = 0U;
	me->adcConvTrigConfig.enableHardwareTrigger = false;

	me->adcConvResult.commandIdSource = 0U;
	me->adcConvResult.loopCountIndex = 0U;
	me->adcConvResult.triggerIdSource = 0U;
	me->adcConvResult.convValue = 0U;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxAdc_Init(AlxAdc* me)
{
	// Assert
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Init one pin for each channel
	for (uint8_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		AlxIoPin_Init(*(me->ioPinArr + i));
		IOCON->PIO[me->ioPinArr[i]->port][me->ioPinArr[i]->pin] |= (0x1 << 10U);	// MF: Set ASW bit meaning analog switch is enabled
	}

	// #2 Reset ADC Periphery
	RESET_PeripheralReset(kADC0_RST_SHIFT_RSTn);

	// #3 Init Clk, Power
	CLOCK_SetClkDiv(kCLOCK_DivAdcAsyncClk, 8U, true);	//TODO
	CLOCK_AttachClk(kMAIN_CLK_to_ADC_CLK);
	POWER_DisablePD(kPDRUNCFG_PD_LDOGPADC);	// MF: Don't know if we need this

	// #4 Init ADC
	LPADC_Init(ADC0, &me->adcConfig);

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

	// #1 DeInit Clk
	LPADC_Deinit(ADC0);

	// #2 Clears all ADC registers and BOTH ConvSeq registers
	//SYSCON->PRESETCTRL0 &= ~(1U << 24U);
	//SYSCON->PRESETCTRL0 |= (1U << 24U);

	// #3 DeInit Power
	//POWER_EnablePD(kPDRUNCFG_PD_ADC0);

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
	ADC_DoSoftwareTriggerConvSeqA(ADC0);
	while (!ADC_GetChannelConversionResult(ADC0, AlxAdc_GetCh(me, ch), &me->adcResult)) {}
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

	// #1 Set Channel
	me->adcConvCommConfig.sampleChannelMode = AlxAdc_SetSampleChannelMode(me, ch);
	me->adcConvCommConfig.channelNumber = AlxAdc_GetCh(me, ch);
	LPADC_SetConvCommandConfig(ADC0, 1U, &me->adcConvCommConfig);			// MF: 1U means CMD1 is used

	// #2 Return Voltage
	LPADC_DoSoftwareTrigger(ADC0, 1U);										// MF: It always has to be 1 I don't understand why
	while (!LPADC_GetConvResult(ADC0, &me->adcConvResult, 0U)) {}			// MF: 0U is for FIFO A and it is used for "Single ended" comvertion mode that we are using
	return (((me->adcConvResult.convValue * me->vRef_mV) / 4095) >> 3U);	// MF: I dont understand why shiffting is needed (I copied it from example)

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
	#endif
}
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me)
{
	// MF: Lpc55S6x doesn't support internal TempSens_GetTemp

	// Assert
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
	if (ch == Alx_Ch_12)	return 12;
	if (ch == Alx_Ch_13)	return 13;
	if (ch == Alx_Ch_14)	return 14;
	if (ch == Alx_Ch_15)	return 15;

	ALX_ADC_ASSERT(false);	// We shouldn't get here
	return ALX_NULL;
}
static bool AlxAdc_Ctor_IsSysClkOk(AlxAdc* me)
{
	if (15000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc55s6x_CoreSysClk))
		return true;
	else
		return false;

	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static bool AlxAdc_Ctor_CheckCh(AlxAdc* me)
{
	// #1 Check IoPins
	for (uint32_t i = 0; i < me->numOfIoPinsAndCh; i++)
	{
		if (!(	(me->chArr[i] == Alx_Ch_0)  ||
				(me->chArr[i] == Alx_Ch_1)  ||
				(me->chArr[i] == Alx_Ch_2)  ||
				(me->chArr[i] == Alx_Ch_3)  ||
				(me->chArr[i] == Alx_Ch_4)  ||
				(me->chArr[i] == Alx_Ch_8)  ||
				(me->chArr[i] == Alx_Ch_9)  ||
				(me->chArr[i] == Alx_Ch_10) ||
				(me->chArr[i] == Alx_Ch_11) ||
				(me->chArr[i] == Alx_Ch_12) ))	{ return false; }
	}

	// #2 Return
	return true;
}

lpadc_sample_channel_mode_t AlxAdc_SetSampleChannelMode(AlxAdc* me, Alx_Ch ch)
{
	// #1 Check if channels mode A
	if ((ch == Alx_Ch_0) || (ch == Alx_Ch_1) || (ch == Alx_Ch_2) || (ch == Alx_Ch_3) || (ch == Alx_Ch_4))
	{
		return kLPADC_SampleChannelSingleEndSideA;
	}

	// #2 Check if channels mode B
	if ((ch == Alx_Ch_8) || (ch == Alx_Ch_9) || (ch == Alx_Ch_10) || (ch == Alx_Ch_11) || (ch == Alx_Ch_12))
	{
		return kLPADC_SampleChannelSingleEndSideB;
	}

	// Assert
	ALX_ADC_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}

#endif // Module Guard
