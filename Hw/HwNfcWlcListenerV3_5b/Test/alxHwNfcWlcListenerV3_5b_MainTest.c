/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_MainTest.c
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - Main Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */


//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_MainTest.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
// Variables
//******************************************************************************
AlxHwNfcWlcListenerV3_5b_MainTest alxHwNfcWlcListenerV3_5b_MainTest = { 0 };

//******************************************************************************
// IRQ Handlers
//******************************************************************************
void SysTick_Handler(void)
{
	AlxTick_Inc_ms(&alxTick);
}


//******************************************************************************
// Auralix C Library - Weak Functions
//******************************************************************************

//------------------------------------------------------------------------------
// Gh
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Gk
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Jk
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Js
//------------------------------------------------------------------------------
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_JS_TEST_H)
void AlxPca9431_RegStruct_SetVal(AlxPca9431* me)
{
	// Ldo output
	me->reg._07h_VOUT_LDO.val.VOUT_LDO	= VoutLdo_3V3;
	//me->reg._07h_VOUT_LDO.val.VOUT_LDO = VoutLdo_5V; // JS: commented

	// Vrect treshold
	me->reg._08h_VRECT_THD.val.VRECT_UVLO = VRectThd_3V3;


	// Adc enable
	me->reg._0Dh_ADC_CONTROL.val.ADC_EN = AdcEn_Enabled;
	me->reg._0Dh_ADC_CONTROL.val.ADC_AVG_EN = AdcAveragingEn_Enabled;
	me->reg._0Dh_ADC_CONTROL.val.ADC_RATE = AdcRate_ContinuousConversion;

	// Adc sample enable
	me->reg._0Eh_Sample_EN.val.OTP_ADC_EN = OtpAdcSamplingEn_Enabled;
	me->reg._0Eh_Sample_EN.val.IOUT_ADC_EN	= IOutAdcSamplingEn_Enabled;
	me->reg._0Eh_Sample_EN.val.VOUT_ADC_EN	= VOutAdcSamplingEn_Enabled;
	me->reg._0Eh_Sample_EN.val.IRECT_ADC_EN = IRectAdcSamplingEn_Enabled;
	me->reg._0Eh_Sample_EN.val.VRECT_ADC_EN = VRectAdcSamplingEn_Enabled;
	me->reg._0Eh_Sample_EN.val.VTUNE_ADC_EN = VTuneAdcSamplingEn_Enabled;

	// Interrupt mask
	me->reg._04h_VRECT_INT_MASK.val.VRECT_RDY_INT_MSK = VRectRdyIntMask_NotTrig;
	me->reg._04h_VRECT_INT_MASK.val.VRECT_GOOD_INT_MSK	= VRectGoodIntMask_NotTrig;
	//me->reg._04h_VRECT_INT_MASK.val.VRECT_REGHIGH_INT_MSK = VRectRegHighIntMask_NotTrig;	// JS: commented
	//me->reg._04h_VRECT_INT_MASK.val.VRECT_OVW_MSK = VRectOvWarnIntMask_NotTrig;			// JS: commented
	me->reg._06h_VOUTLDO_INT_MASK.val.VOUT_SHORT_INT_MASK = VOutLdoShortInt_NotMasked;
}
void AlxIoPinIrq_Foreground_Callback_Pin3(void)
{
	AlxTrace_WriteFormat(&alxTrace, "************************	INTERRUPT	*************************\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	AlxTrace_WriteStr(&alxTrace, "RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	AlxTrace_WriteStr(&alxTrace, "FallEdge\r\n");
}
#endif //#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_JS_TEST_H)


//------------------------------------------------------------------------------
// Mf
//------------------------------------------------------------------------------
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H)
void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	AlxTrace_WriteStr(&alxTrace, "RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	AlxTrace_WriteStr(&alxTrace, "FallEdge\r\n");
}
#endif //#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H)


//------------------------------------------------------------------------------
// Tv
//------------------------------------------------------------------------------


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)
