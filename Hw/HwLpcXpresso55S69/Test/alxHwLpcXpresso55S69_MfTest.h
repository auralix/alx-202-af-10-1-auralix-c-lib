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

	// Variables
	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint32_t ai_P0_23_ADC_CH0_voltage_mV;
	uint32_t ai_P0_16_ADC_CH8_voltage_mV;
	#else
	float ai_P0_23_ADC_CH0_voltage_V;
	float ai_P0_16_ADC_CH8_voltage_V;
	#endif

	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint32_t do_P1_4_PWM1_duty_permil;
	uint32_t do_P1_7_PWM2_duty_permil;
	#else
	float do_P1_4_PWM1_duty_pct;
	float do_P1_7_PWM2_duty_pct;
	#endif

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxHwLpcXpresso55S69_MfTest_G01_BringUp;


//******************************************************************************
// Private Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T01_Led(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_4_UsrLED_BL);
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);
	//AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_7_UsrLED_GR);
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);	// MF: To see toggling with Logic Analyzer

	while (1)
	{
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_4_UsrLED_BL);
		AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_6_UsrLED_RD);
		//AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_7_UsrLED_GR);
		AlxIoPin_Toggle(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P1_9_GPIO);

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T02_Trace(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	while (1)
	{
		ALX_TRACE_FORMAT("T02_Trace\r\n");
		AlxDelay_ms(1000);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T03_Adc(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxAdc_Init(&me->alxHwLpcXpresso55S69_Main.alxAdc);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		me->ai_P0_23_ADC_CH0_voltage_mV = AlxAdc_GetVoltage_mV(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]);
		me->ai_P0_16_ADC_CH8_voltage_mV = AlxAdc_GetVoltage_mV(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[1]);
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", me->ai_P0_23_ADC_CH0_voltage_mV);
		AlxTrace_WriteFormat(&alxTrace, "Ch8, P2 = %d\r\n", me->ai_P0_16_ADC_CH8_voltage_mV);
		#else
		me->ai_P0_23_ADC_CH0_voltage_V = AlxAdc_GetVoltage_V(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[0]);
		me->ai_P0_16_ADC_CH8_voltage_V = AlxAdc_GetVoltage_V(&me->alxHwLpcXpresso55S69_Main.alxAdc, me->alxHwLpcXpresso55S69_Main.adcChArr[1]);
		AlxTrace_WriteFormat(&alxTrace, "Ch0, P1 = %d\r\n", me->ai_P0_23_ADC_CH0_voltage_V);
		AlxTrace_WriteFormat(&alxTrace, "Ch8, P2 = %d\r\n", me->ai_P0_16_ADC_CH8_voltage_V);
		#endif

		AlxDelay_ms(20);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T04_Pwm(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Init
	AlxPwm_Init(&me->alxHwLpcXpresso55S69_Main.alxPwm);

	// Setup Delay	// MF: To see how duty changes from what is in Ctor to what is set below
	AlxDelay_ms(500);

	while (1)
	{
		#if defined ALX_OPTIMIZE_SIZE_ALL
		AlxPwm_SetDuty_permil(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[0], me->do_P1_4_PWM1_duty_permil);
		AlxPwm_SetDuty_permil(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[1], me->do_P1_7_PWM2_duty_permil);
		#else
		AlxPwm_SetDuty_pct(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[0], me->do_P1_4_PWM1_duty_pct);
		AlxPwm_SetDuty_pct(&me->alxHwLpcXpresso55S69_Main.alxPwm, me->alxHwLpcXpresso55S69_Main.pwmChArr[1], me->do_P1_7_PWM2_duty_pct);
		#endif

		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Assert
	(void)me;

	// Variables
	uint8_t srcBuffWrite[2];
	uint8_t srcBuffRead[2];
	srcBuffWrite[0] = 0b11111111;	// MF: Writing (0b1) to 7Fh register returns CTN530 Verion and shoudl be 0x1A
	srcBuffWrite[1] = 0b11111111;
	srcBuffRead[0] = 0b00000000;
	srcBuffRead[1] = 0b00000000;

	// Init
	AlxSpi_Init(&me->alxHwLpcXpresso55S69_Main.alxSpi);

	// Setup Delay
	AlxDelay_ms(500);

	while (1)
	{
		// WriteRead
		AlxSpi_Master_AssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi);
		if (AlxSpi_Master_WriteRead(&me->alxHwLpcXpresso55S69_Main.alxSpi, srcBuffWrite, srcBuffRead, sizeof(srcBuffWrite), 2, 0) != Alx_Ok)	{ ALX_TRACE_FORMAT("Pujhnalo\r\n"); }
		AlxSpi_Master_DeAssertCs(&me->alxHwLpcXpresso55S69_Main.alxSpi);


		// Read Default Reg

		// Write

		// Read Written rReg


		AlxDelay_ms(500);
	}
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T06_Clk(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Init
	AlxIoPin_Init(&me->alxHwLpcXpresso55S69_Main.alxIoPin.do_P0_26_CLKOUT);

	if		(alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 12U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else if (alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 96U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else if (alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 150U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else if (alxClk.config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)	// MF. Clock Out on P0_26 should be 1MHz
	{
		CLOCK_SetClkDiv(kCLOCK_DivClkOut, 150U, true);
		CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);
	}
	else { ALX_TRACE_FORMAT("Svinjak\r\n"); }

	while (1) {}
}


//******************************************************************************
// Constructor & Functions
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Ctor(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Ctor
	AlxHwLpcXpresso55S69_Main_Ctor(&me->alxHwLpcXpresso55S69_Main);

	// Variables
	#if defined ALX_OPTIMIZE_SIZE_ALL
	me->ai_P0_23_ADC_CH0_voltage_mV = 0ul;
	me->ai_P0_16_ADC_CH8_voltage_mV = 0ul;
	#else
	me->ai_P0_23_ADC_CH0_voltage_V = 0.f;
	me->ai_P0_16_ADC_CH8_voltage_V = 0.f;
	#endif

	#if defined ALX_OPTIMIZE_SIZE_ALL
	me->do_P1_4_PWM1_duty_permil = 111ul;
	me->do_P1_7_PWM2_duty_permil = 999ul;
	#else
	me->do_P1_4_PWM1_duty_pct = 11.11f;
	me->do_P1_7_PWM2_duty_pct = 88.88f;
	#endif

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Init(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	// Init
	AlxClk_Init(&alxClk);
	AlxTrace_Init(&alxTrace);

	// IoPinIrq
	AlxIoPinIrq_Init(&me->alxHwLpcXpresso55S69_Main.alxIrqPin_IRQ1);	// MF: IoPinIrq is initialized for all tests

	// Info
	me->isInit = true;
}
static inline void AlxHwLpcXpresso55S69_MfTest_G01_BringUp_Run(AlxHwLpcXpresso55S69_MfTest_G01_BringUp* me)
{
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T01_Led(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T02_Trace(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T03_Adc(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T04_Pwm(me);
	AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T05_Spi(me);
	//AlxHwLpcXpresso55S69_MfTest_G01_BringUp_T06_Clk(me);
}


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_LPC_XPRESSO_55S69_MF_TEST_H
