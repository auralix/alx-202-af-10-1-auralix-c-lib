/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_Main.c
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - Main Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */


//******************************************************************************
// Includes
//******************************************************************************
#include <alxHwNfcWlcListenerV3_5b_Main.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_Ctor(AlxHwNfcWlcListenerV3_5b_Main* me)
{
	//------------------------------------------------------------------------------
	// ALX - IoPin
	//------------------------------------------------------------------------------
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_0_UART_RX,			0,	0,	AlxIoPin_Func_Swm_USART1_RXD,	IOCON_MODE_INACT,	false,	true,	true	);
	//P0_1	- Unused
	//P0_2	- SWD_IO
	//P0_3	- SWD_CLK
	//P0_4	- DBG_UART_TX -> ALX Trace Handle
	//P0_5	- nRST
	//P0_6	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_7_CRN_FD_IRQ2,		0,	7,	AlxIoPin_Func_IRQ,				IOCON_MODE_INACT,	false,	true,	true	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_8_PCA943X_EN, 		0,	8,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLDOWN,	false,	true,	false	);	// TV: When testing ADC use this: AlxIoPin_Ctor(&me->alxIoPin.ai_P0_9_ADC_CH4,			0,	9,	AlxIoPin_Func_Swm_ADC_CHN4,		IOCON_MODE_INACT,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_9_PCA943X_nINT_IRQ1,	0,	9,	AlxIoPin_Func_IRQ,				IOCON_MODE_INACT,	false,	true,	true	);
	//P0_10	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.ao_P0_11_CRN_VCC,			0,	11,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	true	);
	//P0_12	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_13_CRN_ED_IRQ3,		0,	13, AlxIoPin_Func_IRQ,				IOCON_MODE_INACT,	false,	true,	true	);
	//P0_14	- Unused
	//P0_15	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.io_P0_16_I2C0_SDA,			0,	16,	AlxIoPin_Func_Swm_I2C0_SDA,		IOCON_MODE_INACT,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_17_I2C0_SCL,			0,	17,	AlxIoPin_Func_Swm_I2C0_SCL,		IOCON_MODE_INACT,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_18_LED_RD,			0,	18,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_19_LED_GR,			0,	19,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	false	);
	//P0_20	- Unused
	//P0_21	- Unused
	//P0_22 - Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_22_LED_GR, 			0,	22, AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_23_LED_GR, 			0,	23, AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_24_LED_GR_PWM1,		0,	24,	AlxIoPin_Func_Swm_T0_MAT_CHN1,	IOCON_MODE_INACT,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_25_LED_GR_PWM2,		0,	25,	AlxIoPin_Func_Swm_T0_MAT_CHN2,	IOCON_MODE_INACT,	false,	false,	false	);
	//P0_26	- Unused
	//P0_27	- Unused
	//P0_28	- Unused
	//P0_29	- Unused
	//P0_30	- Unused


	//------------------------------------------------------------------------------
	// ALX - Clock
	//------------------------------------------------------------------------------
	AlxClk_Ctor
	(
		&alxClk,
		AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz,
		AlxClk_Tick_1ms
	);


	//------------------------------------------------------------------------------
	// ALX - Trace
	//------------------------------------------------------------------------------
	AlxTrace_Ctor
	(
		&alxTrace,
		0,	// port
		4,	// pin
		USART1,
		AlxGlobal_BaudRate_115200
	);


	//------------------------------------------------------------------------------
	// ALX - I2C
	//------------------------------------------------------------------------------
	AlxI2c_Ctor
	(
		&me->alxI2c_I2C0,
		I2C0,
		&me->alxIoPin.do_P0_17_I2C0_SCL,
		&me->alxIoPin.io_P0_16_I2C0_SDA,
		AlxI2c_Clk_McuLpc80x_BitRate_400kHz
	);


	//------------------------------------------------------------------------------
	// ALX - ADC
	//------------------------------------------------------------------------------

	// JS: optimize size for Lpc804 MCU

	//me->adcIoPinArr[0] = &me->alxIoPin.ai_P0_9_ADC_CH4;	// TV: When testing ADC use this
	//me->adcChArr[0] = Alx_Ch_4;
	//#if defined ALX_OPTIMIZE_SIZE_ALL
	//AlxAdc_Ctor
	//(
	//	&me->alxAdc,
	//	me->adcIoPinArr,
	//	me->adcChArr,
	//	ALX_ARR_LEN(me->adcIoPinArr),
	//	&alxClk,
	//	3300U
	//);
	//#else
	//AlxAdc_Ctor
	//(
	//	&me->alxAdc,
	//	me->adcIoPinArr,
	//	me->adcChArr,
	//	ALX_ARR_LEN(me->adcIoPinArr),
	//	&alxClk,
	//	3.3f
	//);
	//#endif


	//------------------------------------------------------------------------------
	// ALX - PWM
	//------------------------------------------------------------------------------

	// JS: optimize size for Lpc804 MCU

	//me->pwmIoPinArr[0] = &me->alxIoPin.do_P0_24_LED_GR_PWM1;
	//me->pwmIoPinArr[1] = &me->alxIoPin.do_P0_25_LED_GR_PWM2;
	//me->pwmChArr[0] = Alx_Ch_1;
	//me->pwmChArr[1] = Alx_Ch_2;
	//#if defined ALX_OPTIMIZE_SIZE_ALL
	//me->pwmDutyDefaultArr[0] = 543U;
	//me->pwmDutyDefaultArr[1] = 123U;
	//#else
	//me->pwmDutyDefaultArr[0] = 12.34f;
	//me->pwmDutyDefaultArr[1] = 50.f;
	//#endif
	//AlxPwm_Ctor
	//(
	//	&me->alxPwm,
	//	CTIMER0,
	//	me->pwmIoPinArr,
	//	me->pwmChArr,
	//	ALX_ARR_LEN(me->pwmChArr),
	//	&alxClk,
	//	me->pwmDutyDefaultArr,
	//	0,
	//	100
	//);


	//------------------------------------------------------------------------------
	// ALX - PCA9431
	//------------------------------------------------------------------------------
	AlxIoPinIrq_Ctor
	(
		&me->alxIoIrqPin_di_P0_9_PCA943X_nINT_IRQ1,
		&me->alxIoPin.di_P0_9_PCA943X_nINT_IRQ1,
		kPINT_PinInt1,
		kPINT_PinIntEnableFallEdge,
		Alx_IrqPriority_0
	);
	AlxPca9431_Ctor
	(
		&me->alxPca9431,
		&me->alxI2c_I2C0,
		0b11100010,	// i2cAddr
		true,		// i2cCheckWithRead
		3,			// i2cNumOfTries
		1000		// i2cTimeout_ms
	);


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Variables
//******************************************************************************
AlxHwNfcWlcListenerV3_5b_Main alxHwNfcWlcListenerV3_5b_Main = {0};


//******************************************************************************
// IRQ Handlers
//******************************************************************************
void SysTick_Handler(void)
{
	AlxTick_Inc_ms(&alxTick);
}


//******************************************************************************
// Auralix C Library - ALX Clock Module - Weak Functions
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config,
	AlxClk_Tick tick
)
{
	(void)config;
	(void)tick;

	me->coreSysClk_Ctor	= 12000000U;
	me->mainClk_Ctor	= 12000000U;
	me->fro_Ctor		= 24000000U;
	//me->lpo_Ctor		=  1000000U;	// MF: We don't need this for program to work
}
Alx_Status AlxClk_Init(AlxClk* me)
{
	(void)me;

	// Enable Clk to Gpio0
	*(volatile uint32_t *)(((uint32_t)(&SYSCON->SYSAHBCLKCTRL0)) + CLK_GATE_GET_REG(kCLOCK_Gpio0)) |= 1UL << CLK_GATE_GET_BITS_SHIFT(kCLOCK_Gpio0);

	// Set Fro Osc Freq
	g_Fro_Osc_Freq = (uint32_t)24000U * 1000UL;	// MF: This has to be set, otherwise "uint32_t CLOCK_GetFroFreq(void)" returns 0

	// Configure SysTick
	SysTick->LOAD  = (12000U - 1UL);									// set reload register
	NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);	// set Priority for Systick Interrupt
	SysTick->VAL   = 0UL;												// Load the SysTick Counter Value
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	return Alx_Ok;
}


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)
