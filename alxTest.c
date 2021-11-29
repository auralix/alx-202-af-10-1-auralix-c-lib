/**
  ******************************************************************************
  * @file alxTest.c
  * @brief Auralix C Library - ALX TEST Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-11-26 19:02:52 +0100 (Fri, 26 Nov 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTest.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_TEST)


//******************************************************************************
// Functions
//******************************************************************************
void AlxTest_Ctor(AlxTest* me)
{
	//-----------
	// AlxClk		// MF: Must be First otherwise FUKA
	//-----------
	#if defined(ALX_TEST_CLK)
	AlxClk_Ctor(&alxClk, ALX_TEST_CLK_CONFIG, ALX_TEST_CLK_TICK);
	#endif


	//-----------
	// AlxAdc
	//-----------
	#if defined(ALX_TEST_ADC)
	me->adcTest_IoPinArr[0] = &me->adcTest_IoPin;
	me->adcTest_ChArr[0] = ALX_TEST_ADC_CH;
	
	AlxIoPin_Ctor(&me->adcTest_IoPin, ALX_TEST_ADC_PORT, ALX_TEST_ADC_PIN, ALX_TEST_ADC_IO_PIN_FUNC_SWM, IOCON_MODE_INACT, false, false, false, ALX_NULL_PTR);
	AlxAdc_Ctor(&me->adcTest, me->adcTest_IoPinArr, me->adcTest_ChArr, ALX_ARR_LEN(me->adcTest_IoPinArr), &alxClk, ALX_TEST_ADC_VREF);
	#endif


	//-----------
	// AlxCan
	//-----------
	// TODO


	//-----------
	// AlxDac
	//-----------
	// TODO


	//------------
	// AlxI2c
	//------------
//	AlxIoPin_Ctor(&do_SCK_P24, 0, 24, AlxIoPin_Func_Swm_I2C0_SCL, IOCON_MODE_INACT, false, false, false, ALX_NULL_PTR);
//	AlxIoPin_Ctor(&do_SDA_P25, 0, 25, AlxIoPin_Func_Swm_I2C0_SDA, IOCON_MODE_INACT, false, false, false, ALX_NULL_PTR);
//	AlxI2c_Ctor(&me->i2cTest, I2C0, &do_SCK_P24, &do_SDA_P25, AlxI2c_Clk_McuLpc80x_BitRate_400kHz_I2cFuncClk_15MHz);
//
//
//	//-----------
//	// AlxI2s
//	//-----------
//	// TODO
//
//
//	//-------------
//	// AlxIoPin
//	//-------------
//	AlxIoPin_Ctor(&do_nPIN, 0, 24, AlxIoPin_Func_GPIO, IOCON_MODE_PULLUP, false, true, false, ALX_NULL_PTR);
//	AlxIoPin_Ctor(&di_P00, 0, 0, AlxIoPin_Func_IRQ, IOCON_MODE_INACT, false, true, true, &di_P00_irq);
//	AlxIoPin_Ctor(&di_P04, 0, 4, AlxIoPin_Func_IRQ, IOCON_MODE_INACT, false, true, true, &di_P04_irq);
//
//
//
//	//----------------
//	// AlxIoPinIrq
//	//----------------
//	// TODO
//
//
//	//-----------
//	// AlxPwm
//	//-----------
//	AlxIoPin_Ctor(&do_P24, 0, 24, AlxIoPin_Func_Swm_T0_MAT_CHN1, IOCON_MODE_INACT, false, false, false, ALX_NULL_PTR);
//	AlxIoPin_Ctor(&do_P25, 0, 25, AlxIoPin_Func_Swm_T0_MAT_CHN2, IOCON_MODE_INACT, false, false, false, ALX_NULL_PTR);
//	AlxPwm_Ctor(&me->pwmTest, CTIMER0, pwmIoPinArr, pwmChArr, pwmDutyDefaultArr, ALX_ARR_LEN(pwmChArr), &alxClk, 0, 100);
//
//
//	//-----------
//	// AlxRtc
//	//-----------
//	// TODO
//
//
//	//------------------
//	// AlxSerialPort
//	//------------------
//	// TODO
//
//
//	//-----------
//	// AlxSpi
//	//-----------
//	// TODO


	//-------------
	// AlxTrace
	//-------------
	#if defined(ALX_TEST_TRACE)
	AlxTrace_Ctor(&alxTrace, ALX_TEST_TRACE_PORT, ALX_TEST_TRACE_PIN, ALX_TEST_TRACE_UART, ALX_TEST_TRACE_BAUDRATE);
	#endif
}
Alx_Status AlxTest_Init(AlxTest* me)
{
	#if defined(ALX_TEST_ADC)
	if (AlxAdc_Init(&me->adcTest) != Alx_Ok)	return Alx_Err;
	#endif
	#if defined(ALX_TEST_CAN)
	if (AlxCan_Init(&me->canTest) != Alx_Ok)	return Alx_Err;
	#endif
	#if defined(ALX_TEST_CLK)
	if (AlxClk_Init(&alxClk) != Alx_Ok)			return Alx_Err;
	#endif
	// TODO
	#if defined(ALX_TEST_TRACE)
	if (AlxTrace_Init(&alxTrace) != Alx_Ok)		return Alx_Err;
	#endif

	return Alx_Ok;
}
Alx_Status AlxTest_DeInit(AlxTest* me)
{
	#if defined(ALX_TEST_ADC)
	if (AlxAdc_DeInit(&me->adcTest) != Alx_Ok)	return Alx_Err;
	#endif
	#if defined(ALX_TEST_CAN)
	if (AlxCan_DeInit(&me->canTest) != Alx_Ok)	return Alx_Err;
	#endif
	#if defined(ALX_TEST_CLK)
	// MF: No "DeInit()" function
	#endif
	// TODO
	#if defined(ALX_TEST_TRACE)
	if (AlxTrace_DeInit(&alxTrace) != Alx_Ok)	return Alx_Err;
	#endif

	return Alx_Ok;
}
ALX_WEAK void AlxTest_Run_Adc(AlxTest* me)
{
	#if defined(ALX_TEST_ADC)
	float adcTest_voltage_V = AlxAdc_GetVoltage_V(&me->adcTest, me->adcTest_ChArr[0]);
	uint32_t adcTest_voltage_mV = AlxAdc_GetVoltage_mV(&me->adcTest, me->adcTest_ChArr[0]);
	float adcTest_temp_degC = AlxAdc_TempSens_GetTemp_degC(&me->adcTest);
	#endif
}
ALX_WEAK void AlxTest_Run_Audio(AlxTest* me)
{
	// TODO
}
// TODO
ALX_WEAK void AlxTest_Run_Vdiv(AlxTest* me)
{
	// TODO
}


#endif // Module Guard
