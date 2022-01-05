/**
  ******************************************************************************
  * @file alxHwLpcXpresso55S69_MfTest.h
  * @brief Auralix HW LPC Xpresso 55S69 C Library - MF Test Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H
#define ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwLpcXpresso55S69_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)


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
	AlxHwLpcXpresso55S69_Main alxHwLpcXpresso55S69_Main;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T01_Led(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_4_UsrLED_BL);
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_7_UsrLED_GR);
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);

	while (1)
	{
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_4_UsrLED_BL);
		AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_7_UsrLED_GR);
		AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T02_Trace(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T03_Adc(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxAdc_Init(&me->alxHwLpcXpresso55S69_Main.alxAdc);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		//AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_mV(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]));
		AlxTrace_WriteFormat(&alxTrace, "Ch8, P2 = %d\r\n", AlxAdc_GetVoltage_mV(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]));
		#else
		//AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", AlxAdc_GetVoltage_V(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]));
		#endif

		AlxDelay_ms(1000);
	}
}
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T04_Pwm(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxPwm_Init(&me->alxHwLpcXpresso55S69_Main.alxPwm);

	// Setup Delay	// MF: To see how duty changes from 50% (set in Ctor) to what is set below
	AlxDelay_ms(500);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxPwm_SetDuty_permil(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[0], 111);
		AlxPwm_SetDuty_permil(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[1], 999);
		#else
		AlxPwm_SetDuty_pct(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[0], 11.11f);
		AlxPwm_SetDuty_pct(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[1], 88.88f);
		#endif

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T05_I2c(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t devAdrReceive = 0b1010101;
	uint16_t memAddr = 0x00U;	// MF: Sth is not right here
	uint8_t i2cData[6] = { 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00 };

	// Init
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.ao_P0_11_CRN_VCC);
	//AlxI2c_Init(&me->alxHwLpcXpresso55S69_Main.alxI2c_I2C0);

	while (1)
	{
		//AlxI2c_Master_StartReadMemStop(&me->alxHwLpcXpresso55S69_Main.alxI2c_I2C0, devAdrReceive, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData, 1, 20, 100);
		//AlxI2c_Master_StartWriteMemStop_Single(&i2c, slaveAddr, memAddr, AlxI2c_Master_MemAddrLen_16bit, i2cData[0], true, 5, 100);

		AlxDelay_ms(500);
	}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_Ctor(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Ctor
	AlxHwLpcXpresso55S69_Main_Ctor(&me->alxHwLpcXpresso55S69_Main);

	// Info
	me->wasCtorCalled = true;
}
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_Init(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwLpcXpresso55S69_Main.alxIrqPin_IRQ1);	// MF: IoPinIrq is always Inited

	// Info
	me->isInit = true;
}
static inline void AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_Run(AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp* me)
{
	//AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T01_Led(me);
	//AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T02_Trace(me);
	AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T03_Adc(me);
	//AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T04_Pwm(me);
	//AlxHwLpcXpresso55S69_Main_MfTest_G01_BringUp_T05_I2c(me);
}


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H
