//****************************************************************************
// @file alxHwNfcWlcListenerV3_5b_MainTest.c
// @brief Auralix HW NFC WLC Listener V3.5 - Main Test Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//****************************************************************************


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
AlxHwNfcWlcListenerV3_5b_MainTest alxHwNfcWlcListenerV3_5b_MainTest = {0};


//******************************************************************************
// IRQ Handlers & Weak Functions
//******************************************************************************
//------------------------------------------------------------------------------
// Gh
//------------------------------------------------------------------------------
#if defined(ALX_TEST_GH)
#endif

//------------------------------------------------------------------------------
// Gk
//------------------------------------------------------------------------------
#if defined(ALX_TEST_GK)
#endif

//------------------------------------------------------------------------------
// Jk
//------------------------------------------------------------------------------
#if defined(ALX_TEST_JK)
#endif

//------------------------------------------------------------------------------
// Js
//------------------------------------------------------------------------------
#if defined(ALX_TEST_JS)
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
	//me->reg._02h_SYSTEM_INT_MASK.val.VPWR_SCP_INT_MSK = VpwrScpIntMask_NotTrig;			// JS: commented
	me->reg._05h_VOUTLDO_INT.val.VOUT_SHORT_INT = VOutLdoShortInt_Detect;
	me->reg._06h_VOUTLDO_INT_MASK.val.VOUT_SHORT_INT_MASK = VOutLdoShortInt_NotMasked;
}
void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	ALX_TRACE_FORMAT("RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	ALX_TRACE_FORMAT("FallEdge	************************	INTERRUPT	*************************\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin3()
{
	ALX_TRACE_FORMAT("************************	INTERRUPT	*************************\r\n");
}
#endif

//------------------------------------------------------------------------------
// Mf
//------------------------------------------------------------------------------
#if defined(ALX_TEST_MF)
/*void AlxIoPinIrq_Foreground_Callback_Pin0()
{
	ALX_TRACE_FORMAT(&alxTrace, "RiseEdge\r\n");
}
void AlxIoPinIrq_Foreground_Callback_Pin1()
{
	ALX_TRACE_FORMAT(&alxTrace, "FallEdge\r\n");
}*/
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
}
#endif

//------------------------------------------------------------------------------
// Tv
//------------------------------------------------------------------------------
#if defined(ALX_TEST_TV)
#endif


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)
