/**
  ******************************************************************************
  * @file alxAdc_McuLpc80x.c
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdc_McuLpc80x.h"
#include "alxAdc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdcMcu_GetCh(AlxAdc_Mcu* me, Alx_Ch ch);
static bool AlxAdcMcu_Ctor_IsSysClkOk(AlxAdc_Mcu* me);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxAdcMcu_Ctor
(
	AlxAdc_Mcu* me,
	AlxIoPin** ioPinArr,
	Alx_Ch* chArr,
	uint8_t numOfIoPinsAndCh,
	AlxClk* clk,
	float vRef_V
)
{
	//Assert
	(void)me;
	(void)ioPinArr;
	(void)chArr;
	ALX_ADC_MCU_ASSERT(numOfIoPinsAndCh <= ALX_ADC_BUFF_LEN);
	(void)clk;
	(void)vRef_V;

	// Objects - External
	me->ioPinArr = ioPinArr;
	me->clk = clk;

	// Parameters
	me->chArr = chArr;
	me->numOfIoPinsAndCh = numOfIoPinsAndCh;
	me->vRef_V = vRef_V;

	// Check channel sequence
	for (uint32_t i = 0; i < numOfIoPinsAndCh - 1; i++)
		ALX_ADC_MCU_ASSERT(AlxAdcMcu_GetCh(me, chArr[i]) < AlxAdcMcu_GetCh(me, chArr[i + 1])); // Channel sequence must be from low to high number

	// Check clock
	ALX_ADC_MCU_ASSERT(AlxAdcMcu_Ctor_IsSysClkOk(me));

	// Variables
	//me->adcConfig.clockMode = kADC_ClockSynchronousMode;	// Doesn't work on Lpc804
	me->adcConfig.clockDividerNumber = 0;					// Clkdiv = 0 ensures that sampling rate si appropriate (see User Manual)
	me->adcConfig.enableLowPowerMode = false;
	//me->adcConfig.voltageRange = kADC_HighVoltageRange;	// Doesn't work on Lpc804

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
Alx_Status AlxAdcMcu_Init(AlxAdc_Mcu* me)
{
	ALX_ADC_ASSERT(me->isInit == false);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Init one pin for each channel
	for(uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++)
		AlxIoPin_Init(*(me->ioPinArr + i));

	// #2 Reset ADC Periphery
	RESET_PeripheralReset(kADC_RST_N_SHIFT_RSTn);

	// #3 Init Clk, Power
	CLOCK_Select(kADC_Clk_From_Fro);
	CLOCK_SetClkDivider(kCLOCK_DivAdcClk, me->adcConfig.clockDividerNumber);	// Clkdiv = 0 ensures that sampling rate si appropriate
	POWER_DisablePD(kPDRUNCFG_PD_ADC0);											// Disabling PD gives power!!!

	// #4 Calibration
	#if defined(ALX_LPC84x)
	while(ADC_DoSelfCalibration(ALX_ADC_LPC_8XX, (CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk))) == false) {}	// Doesn't work on Lpc804
	#endif

	// #5 Configure ADC
	ADC_Init(ALX_ADC_LPC_8XX, &me->adcConfig);

	// #6.1 Configure Ch for Coversion Cequence A
	for(uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++)							// Loop through channels to add appropriate channelMask
		me->adcConvSeqConfig.channelMask |= (1U << AlxAdcMcu_GetCh(me, me->chArr[i]));

	// #6.2 Configure and Enable Coversion Sequence A
	ADC_SetConvSeqAConfig(ALX_ADC_LPC_8XX, &me->adcConvSeqConfig);
	ADC_EnableConvSeqA(ALX_ADC_LPC_8XX, true);

	// #7 Clear result register
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC_8XX);
	ADC_GetConvSeqAGlobalConversionResult(ALX_ADC_LPC_8XX, &me->adcResult);		// Clears global ADC conversion infomation of sequence A.

	// #8 Set isInit
	me->isInit = true;

	// #9 Return OK
	return Alx_Ok;
}
Alx_Status AlxAdcMcu_DeInit(AlxAdc_Mcu* me)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit Clk
	ADC_Deinit(ALX_ADC_LPC_8XX);

	// #2 Clears all ADC registers and BOTH ConvSeq registers
	SYSCON->PRESETCTRL0 &= ~(1U << 24U);
	SYSCON->PRESETCTRL0 |= (1U << 24U);

	// #3 DeInit Power
	POWER_EnablePD(kPDRUNCFG_PD_ADC0);

	// #4 DeInit pin for each channel
	for(uint8_t i = 0 ; i < me->numOfIoPinsAndCh; i++)
		AlxIoPin_DeInit((*(me->ioPinArr + i)));

	// #4 Reset isInit
	me->isInit = false;
	
	return Alx_Ok;
}
float AlxAdcMcu_GetVoltage_V(AlxAdc_Mcu* me, Alx_Ch* ch)
{
	ALX_ADC_ASSERT(me->isInit == true);
	ALX_ADC_ASSERT(me->wasCtorCalled == true);

	// #1 Return Voltage
	ADC_DoSoftwareTriggerConvSeqA(ALX_ADC_LPC_8XX);
	while (!ADC_GetChannelConversionResult(ALX_ADC_LPC_8XX, AlxAdcMcu_GetCh(me, *ch), &me->adcResult)) {}
	return ((me->adcResult.result * me->vRef_V) / 4095);
	
	ALX_ADC_MCU_ASSERT(false); // We shouldn't get here
	return 0;
}
float AlxAdcMcu_TempSens_GetTemp_degC(AlxAdc_Mcu* me)
{
	// MF: Lpc80x doesn't support internal TempSens_GetTemp

	ALX_ADC_MCU_ASSERT(false); // We shouldn't get here
	return 0;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxAdcMcu_GetCh(AlxAdc_Mcu* me, Alx_Ch ch)
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
	return 0;
}
static bool AlxAdcMcu_Ctor_IsSysClkOk(AlxAdc_Mcu* me)
{
	#if defined(ALX_LPC80x)
	if (15000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor))
		return true;
	else
		return false;

	ALX_ADC_MCU_ASSERT(false); // We shouldn't get here
	return 0;
	#endif

	#if defined(ALX_LPC81x) || defined(ALX_LPC82x) || defined(ALX_LPC83x) || defined(ALX_LPC84x)
	// TODO
	#endif
}

#endif // Module Guard
