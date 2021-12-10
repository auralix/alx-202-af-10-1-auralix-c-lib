/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_MfTest.h
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - MF Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFCWLC_LISTENER_V3_5B_MF_TEST_H
#define ALX_HW_NFCWLC_LISTENER_V3_5B_MF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
//******************************************************************************
// G01_BringUp
//******************************************************************************
//******************************************************************************


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects
	AlxHwNfcWlcListenerV3_5b_Main alxHwNfcWlcListenerV3_5b_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED205_RD);
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED200_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED204_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED203_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_LED202_GR);
	//AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_LED201_GR);

	while (1)
	{
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_18_LED205_RD);
		AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_19_LED200_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_22_LED204_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_23_LED203_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_LED202_GR);
		//AlxIoPin_Toggle(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_LED201_GR);
		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	(void)me;

	// 
	AlxIoPin* adcIoPinArr[1] = { &me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.ai_P0_9_ADC_In };
	Alx_Ch adcChArr[1] = { Alx_Ch_4 };

	// Adc Ctor
	#if defined ALX_OPTIMIZE_SIZE_ALL
	AlxAdc_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master, adcIoPinArr, adcChArr, ALX_ARR_LEN(adcIoPinArr), &alxClk, 3300U);
	#else
	AlxAdc_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master, adcIoPinArr, adcChArr, ALX_ARR_LEN(adcIoPinArr), &alxClk, 3.3f);
	#endif

	// Adc Init
	AlxAdc_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_mV(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master, adcChArr[0]));
		#else
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master, adcChArr[0]));
		#endif

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	(void)me;

	AlxIoPin* pwmIoPinArr[2] = { &me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_24_PWM1, &me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.do_P0_25_PWM2 };
	Alx_Ch pwmChArr[2] = { Alx_Ch_1, Alx_Ch_2 };
	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint16_t pwmDutyDefaultArr[2] = { 50, 50 };
	#else
	float pwmDutyDefaultArr[2] = { 50.f, 50.f };
	#endif

	// Pwm Ctor
	AlxPwm_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, CTIMER0, pwmIoPinArr, pwmChArr, ALX_ARR_LEN(pwmChArr), &alxClk, pwmDutyDefaultArr, 0, 100);

	// Pwm Init
	AlxPwm_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master);

	AlxDelay_ms(500);
	// Setup Delay

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxPwm_SetDuty_permil(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, pwmChArr[0], 111);
		AlxPwm_SetDuty_permil(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, pwmChArr[1], 999);
		#else
		AlxPwm_SetDuty_pct(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, pwmChArr[0], 10);
		AlxPwm_SetDuty_pct(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, pwmChArr[1], 90);
		#endif

		AlxDelay_ms(800);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	(void)me;

	////uint8_t devAdrSend = 0b11100010;
	uint8_t devAdrReceive = 0b1010101;
	uint16_t memAddr = 0x00U; // = 0b11100011;
	uint8_t i2cData[6] = { 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00 };

	while (1)
	{
		AlxI2c_Master_StartReadMemStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0_Master, devAdrReceive, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData, 1, 20, 100);
		//AlxI2c_Master_StartWriteMemStop_Single(&i2c, slaveAddr, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData[0], true, 5, 100);

		AlxDelay_ms(1000);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Ctor(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Ctor
	AlxHwNfcWlcListenerV3_5b_Main_Ctor(&me->alxHwNfcWlcListenerV3_5b_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Init(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIrqPin_IRQ1);

	// I2c
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.ao_P0_11_CRN_VCC);
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0_Master);

	// Info
	me->isInit = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(me);
}


#endif // #if defined(ALX_HW_NFCWLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFCWLC_LISTENER_V3_5B_MF_TEST_H
