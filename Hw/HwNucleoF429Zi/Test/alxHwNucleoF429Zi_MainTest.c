/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_MainTest.c
  * @brief Auralix HW NUCLEO-F429ZI C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */


//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNucleoF429Zi_MainTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)


//******************************************************************************
// Variables
//******************************************************************************
AlxHwNucleoF429Zi_MainTest alxHwNucleoF429Zi_MainTest = {0};

#if defined(ALX_HW_NUCLEO_F429ZI_JS_TEST_H)
AlxHwNucleoF429Zi_JsTest_G02_Pca9431 alxHwNucleoF429Zi_JsTest_G02_Pca9431 = {};
#endif //#if defined(ALX_HW_NUCLEO_F429ZI_JS_TEST_H)
//******************************************************************************
// IRQ Handlers
//******************************************************************************
void SysTick_Handler(void)
{
	AlxTick_Inc_ms(&alxTick);

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


//******************************************************************************
// Auralix C Library - ALX 01 Module - Weak Functions
//******************************************************************************
#if defined(ALX_HW_NUCLEO_F429ZI_JS_TEST_H)
void AlxPca9431_RegStruct_SetVal(AlxPca9431* me)
{
	// Ldo output
	me->reg._0x07_VOUT_LDO.val.VOUT_LDO	= VoutLdo3V3;
	//me->reg._0x07_VOUT_LDO.val.VOUT_LDO = VoutLdo5V;

	// Vrect treshold
	me->reg._0x08_VRECT_THD.val.VRECT_UVLO = VRectThd3V3;

	// Adc enable
	me->reg._0x0D_ADC_CONTROL.val.ADC_EN = AdcEnabled;
	me->reg._0x0D_ADC_CONTROL.val.ADC_AVG_EN = AdcAveragingEnabled;
	me->reg._0x0D_ADC_CONTROL.val.ADC_RATE = AdcContinuousConversion;

	// Adc sample enable
	me->reg._0x0E_Sample_EN.val.OTP_ADC_EN = OtpAdcEnabled;
	me->reg._0x0E_Sample_EN.val.IOUT_ADC_EN	= IOutAdcEnabled;
	me->reg._0x0E_Sample_EN.val.VOUT_ADC_EN	= VOutAdcEnabled;
	me->reg._0x0E_Sample_EN.val.IRECT_ADC_EN = IRectAdcEnabled;
	me->reg._0x0E_Sample_EN.val.VRECT_ADC_EN = VRectAdcEnabled;
	me->reg._0x0E_Sample_EN.val.VTUNE_ADC_EN = VTuneAdcEnabled;

	// Interrupt mask
	me->reg._0x04_VRECT_INT_MASK.val.VRECT_RDY_INT_MSK = VRectRdyIntMaskOn;
	me->reg._0x04_VRECT_INT_MASK.val.VRECT_GOOD_INT_MSK	= VRectGoodIntMaskOn;
	//me->reg._0x04_VRECT_INT_MASK.val.VRECT_REGHIGH_INT_MSK = VRectRegHighIntMaskOn;
	//me->reg._0x04_VRECT_INT_MASK.val.VRECT_OVW_MSK = VRectOvWarnIntMaskOn;
	me->reg._0x06_VOUTLDO_INT_MASK.val.VOUT_SHORT_INT_MASK = VOutLdoShortIntMaskOff;
}
void AlxIoPinIrq_Foreground_Callback_Pin3(void)
{
	AlxTrace_WriteFormat(&alxTrace, "************************	INTERRUPT	*************************\r\n");
}
#endif //#if defined(ALX_HW_NUCLEO_F429ZI_JS_TEST_H)

//******************************************************************************
// Auralix C Library - ALX 02 Module - Weak Functions
//******************************************************************************



#endif // #if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)
