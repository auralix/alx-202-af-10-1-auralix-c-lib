/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_MfTest.h
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - MF Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H
#define ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H

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
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)


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
	// Assert
	(void)me;

	// Init
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
	// Assert
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxAdc_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_mV(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master, me->alxHwNfcWlcListenerV3_5b_Main.adcChArr[0]));
		#else
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_V(&me->alxHwNfcWlcListenerV3_5b_Main.alxAdc_ADC4_Master, me->alxHwNfcWlcListenerV3_5b_Main.adcChArr[0]));
		#endif

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxPwm_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master);

	// Setup Delay	// MF: To see how duty changes from 50% (set in Ctor) to what is set below
	AlxDelay_ms(500);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxPwm_SetDuty_permil(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[0], 111);
		AlxPwm_SetDuty_permil(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[1], 999);
		#else
		AlxPwm_SetDuty_pct(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[0], 10.34f);
		AlxPwm_SetDuty_pct(&me->alxHwNfcWlcListenerV3_5b_Main.alxPwm_Master, me->alxHwNfcWlcListenerV3_5b_Main.pwmChArr[1], 90);
		#endif

		AlxDelay_ms(500);
	}
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAdrReceive = 0b1010101;
	uint16_t memAddr = 0x00U;	// MF: Sth is not right here
	uint8_t i2cData[6] = { 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00 };

	// Init
	AlxIoPin_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIoPin.ao_P0_11_CRN_VCC);
	AlxI2c_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0_Master);

	while (1)
	{
		AlxI2c_Master_StartReadMemStop(&me->alxHwNfcWlcListenerV3_5b_Main.alxI2c_I2C0_Master, devAdrReceive, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData, 1, 20, 100);
		//AlxI2c_Master_StartWriteMemStop_Single(&i2c, slaveAddr, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData[0], true, 5, 100);

		AlxDelay_ms(500);
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
	AlxIoPinIrq_Init(&me->alxHwNfcWlcListenerV3_5b_Main.alxIrqPin_IRQ1);	// MF: IoPinIrq is always Inited

	// Info
	me->isInit = true;
}
static inline void AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_Run(AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp* me)
{
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T01_Led(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T02_Trace(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T03_Adc(me);
	AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T04_Pwm(me);
	//AlxHwNfcWlcListenerV3_5b_Main_MfTest_G01_BringUp_T05_I2c(me);
}


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFC_WLC_LISTENER_V3_5B_MF_TEST_H
