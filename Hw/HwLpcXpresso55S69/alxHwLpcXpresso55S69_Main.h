/**
  ******************************************************************************
  * @file alxHwLpcXpresso55S69_Main.h
  * @brief Auralix HW LPC Xpresso 55S69 C Library - Main Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_LPC_XPRESSO_55S69_MAIN_H
#define ALX_HW_LPC_XPRESSO_55S69_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <alxGlobal.h>
#include <alxAdc.h>
#include <alxAssert.h>
#include <alxAudio.h>
#include <alxAudioPlayer.h>
#include <alxAudioVol.h>
#include <alxBound.h>
#include <alxBtn.h>
#include <alxBuild.h>
#include <alxCan.h>
#include <alxCanParser.h>
#include <alxClk.h>
#include <alxCrc.h>
#include <alxDac.h>
#include <alxDelay.h>
#include <alxFifo.h>
#include <alxFiltGlitchBool.h>
#include <alxFiltGlitchUint32.h>
#include <alxHys1.h>
#include <alxHys2.h>
#include <alxI2c.h>
#include <alxI2s.h>
#include <alxId.h>
#include <alxInterpLin.h>
#include <alxIoPin.h>
#include <alxIoPinIrq.h>
#include <alxLinFun.h>
#include <alxMemRaw.h>
#include <alxMemSafe.h>
#include <alxParamGroup.h>
#include <alxParamItem.h>
#include <alxParamMgmt.h>
#include <alxParamStore.h>
#include <alxPwm.h>
#include <alxRange.h>
#include <alxRtc.h>
#include <alxRtc_Global.h>
#include <alxSerialPort.h>
#include <alxSpi.h>
#include <alxTempSens.h>
#include <alxTempSens_RtdVdiv.h>
#include <alxTick.h>
#include <alxTimSw.h>
#include <alxTpl.h>
#include <alxTrace.h>
#include <alxTrace_Printf.h>
#include <alxVdiv.h>

#include <alxAdau1961.h>
#include <alxAdxl355.h>
#include <alxBq24600.h>
#include <alxBts724g.h>
#include <alxCrn120.h>
#include <alxNtcg103jf103ft1s.h>
#include <alxPca9430.h>
#include <alxPca9431.h>
#include <alxTmp1075.h>
#include <alxTpa3255.h>
#include <alxVnh7040.h>


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	///------------------------------------------------------------------------------
	/// Port 0
	///------------------------------------------------------------------------------
	//PIO0_0	- Unused
	AlxIoPin do_P0_1_DBG01;
	AlxIoPin di_P0_2_SPI_ACC_MISO;
	AlxIoPin do_P0_3_SPI_ACC_MOSI;
	AlxIoPin do_P0_4_SPI_ACC_nCS;
	//PIO0_5	- Unused
	AlxIoPin do_P0_6_SPI_ACC_SCLK;
	//PIO0_7	- Unused
	//PIO0_8	- Unused
	//PIO0_9	- Unused
	//PIO0_10	- Unused
	//PIO0_11	- SWD_CLK
	//PIO0_12	- SWD_IO
	//PIO0_13	- Combo I2C / MFIO
	//PIO0_14	- Combo I2C / MFIO
	//PIO0_15	- Unused
	AlxIoPin ai_P0_16_ADC_CH8;
	//PIO0_17	- Unused
	//PIO0_18	- Unused
	AlxIoPin di_P0_19_SPI_MISO;
	AlxIoPin do_P0_20_SPI_MOSI;
	AlxIoPin do_P0_21_SPI_SCLK;
	//PIO0_22	- Unused
	AlxIoPin ai_P0_23_ADC_CH0;
	//PIO0_24	- Unused
	//PIO0_25	- Unused
	AlxIoPin do_P0_26_CLKOUT;
	//PIO0_27	- Unused
	//PIO0_28	- Unused
	//PIO0_29	- Unused
	//PIO0_30	- Unused
	//PIO0_31	- Unused


	///------------------------------------------------------------------------------
	/// Port 1
	///------------------------------------------------------------------------------
	//PIO1_0	- Unused
	//PIO1_1	- Unused
	//PIO1_2	- Unused
	//PIO1_3	- Unused
	AlxIoPin do_P1_4_PWM1;		// MF: AlxIoPin do_P1_4_UsrLED_BL;
	//PIO1_5	- Unused
	AlxIoPin do_P1_6_UsrLED_RD;
	AlxIoPin do_P1_7_PWM2;		// MF: AlxIoPin do_P1_7_UsrLED_GR;
	//PIO1_8	- Unused
	AlxIoPin do_P1_9_GPIO;
	//PIO1_10	- Unused
	//PIO1_11	- Unused
	//PIO1_12	- Unused
	//PIO1_13	- Unused
	AlxIoPin do_P1_14_DBG02;
	//PIO1_15	- Unused
	//PIO1_16	- Unused
	//PIO1_17	- Unused
	AlxIoPin di_P1_18_WakeBtn_IRQ1;
	//PIO1_19	- Unused
	AlxIoPin do_P1_20_SPI_nCS;
	//PIO1_21	- Unused
	//PIO1_22	- Unused
	//PIO1_23	- Unused
	//PIO1_24	- Unused
	//PIO1_25	- Unused
	//PIO1_26	- Unused
	//PIO1_27	- Unused
	//PIO1_28	- Unused
	//PIO1_29	- Unused
	//PIO1_30	- Unused
	//PIO1_31	- Unused
} AlxHwLpcXpresso55S69_MainIoPin;

typedef struct
{
	// ALX Objects
	AlxIoPinIrq alxIrqPin_IRQ1;
	AlxAdc alxAdc;
	AlxPwm alxPwm;
	AlxSpi alxSpi;
	AlxSpi alxSpiAcc;

	// Auralix HW LPC Xpresso 55S69 C Library Objects
	AlxHwLpcXpresso55S69_MainIoPin alxIoPin;

	///--------
	/// Adc
	///--------
	AlxIoPin* adcIoPinArr[2];
	Alx_Ch adcChArr[2];

	///--------
	/// Pwm
	///--------
	AlxIoPin* pwmIoPinArr[2];
	Alx_Ch pwmChArr[2];
	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint16_t pwmDutyDefaultArr[2];
	#else
	float pwmDutyDefaultArr[2];
	#endif

	// Info
	bool wasCtorCalled;
} AlxHwLpcXpresso55S69_Main;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_Main_Ctor(AlxHwLpcXpresso55S69_Main* me)
{
	///------------------------------------------------------------------------------
	/// ALX - IoPin
	///------------------------------------------------------------------------------
	// Port 0
	//PIO0_0	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_1_DBG01,			0,	1,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_2_SPI_ACC_MISO,	0,	2,	AlxIoPin_Func_1,		IOCON_MODE_PULLUP,	true,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_3_SPI_ACC_MOSI,	0,	3,	AlxIoPin_Func_1,		IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_4_SPI_ACC_nCS,	0,	4,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_PULLUP,	true,	false,	true,	true	); // MF: Spi nCS is config as GPIO and is controlled by SW
	//PIO0_5	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_6_SPI_ACC_SCLK,	0,	6,	AlxIoPin_Func_1,		IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	//PIO0_7	- Unused
	//PIO0_8	- Unused
	//PIO0_9	- Unused
	//PIO0_10	- Unused
	//PIO0_11	- SWD_CLK
	//PIO0_12	- SWD_IO
	//PIO0_13	- Combo I2C / MFIO
	//PIO0_14	- Combo I2C / MFIO
	//PIO0_15	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.ai_P0_16_ADC_CH8,		0,	16,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_INACT,	false,	false,	false,	false	);
	//PIO0_17	- Unused
	//PIO0_18	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_19_SPI_MISO,		0,	19,	AlxIoPin_Func_7,		IOCON_MODE_PULLUP,	true,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_20_SPI_MOSI,		0,	20,	AlxIoPin_Func_7,		IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_21_SPI_SCLK,		0,	21,	AlxIoPin_Func_7,		IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	//PIO0_22	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.ai_P0_23_ADC_CH0,		0,	23,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_INACT,	false,	false,	false,	false	);
	//PIO0_24	- Unused
	//PIO0_25	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_26_CLKOUT,		0,	26,	AlxIoPin_Func_2,		IOCON_MODE_INACT,	true,	false,	false,	false	);
	//PIO0_27	- Unused
	//PIO0_28	- Unused
	//PIO0_29	- Unused
	//PIO0_30	- DBG_UART_TX
	//PIO0_31	- Unused

	// Port 1
	//PIO1_0	- Unused
	//PIO1_1	- Unused
	//PIO1_2	- Unused
	//PIO1_3	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P1_4_PWM1,			1,	4,	AlxIoPin_Func_3,		IOCON_MODE_PULLUP,	true,	false,	true,	false	);	// MF: AlxIoPin_Ctor(&me->alxIoPin.do_P1_4_UsrLED_BL, 1, 4, AlxIoPin_Func_0_GPIO, IOCON_MODE_PULLUP, false, true, false);
	//PIO1_5	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P1_6_UsrLED_RD,		1,	6,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P1_7_PWM2,			1,	7,	AlxIoPin_Func_3,		IOCON_MODE_PULLUP,	true,	false,	true,	false	);	// MF: AlxIoPin_Ctor(&me->alxIoPin.do_P1_7_UsrLED_GR, 1, 6, AlxIoPin_Func_0_GPIO, IOCON_MODE_PULLUP, false, true, false);
	//PIO1_8	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P1_9_GPIO,			1,	9,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	//PIO1_10	- Unused
	//PIO1_11	- Unused
	//PIO1_12	- Unused
	//PIO1_13	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P1_14_DBG02,			0,	1,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_PULLUP,	true,	false,	true,	false	);
	//PIO1_15	- Unused
	//PIO1_16	- Unused
	//PIO1_17	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.di_P1_18_WakeBtn_IRQ1,	1,	18,	AlxIoPin_Func_IRQ,		IOCON_MODE_INACT,	true,	false,	false,	false	);
	//PIO1_19	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.do_P1_20_SPI_nCS,		1,	20,	AlxIoPin_Func_0_GPIO,	IOCON_MODE_PULLUP,	true,	false,	true,	true	);	// MF: Spi nCS is config as GPIO and is controlled by SW
	//PIO1_21	- Unused
	//PIO1_22	- Unused
	//PIO1_23	- Unused
	//PIO1_24	- Unused
	//PIO1_25	- Unused
	//PIO1_26	- Unused
	//PIO1_27	- Unused
	//PIO1_28	- Unused
	//PIO1_29	- Unused
	//PIO1_30	- Unused
	//PIO1_31	- Unused


	///------------------------------------------------------------------------------
	/// ALX - Clock
	///------------------------------------------------------------------------------
	AlxClk_Ctor
	(
		&alxClk,
		AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz,
		AlxClk_Tick_1ms
	);


	///------------------------------------------------------------------------------
	/// ALX - Trace
	///------------------------------------------------------------------------------
	AlxTrace_Ctor
	(
		&alxTrace,
		0,
		30,
		USART0,
		AlxGlobal_BaudRate_115200
	);


	///------------------------------------------------------------------------------
	/// ALX - IrqPin
	///------------------------------------------------------------------------------
	AlxIoPinIrq_Ctor
	(
		&me->alxIrqPin_IRQ1,
		&me->alxIoPin.di_P1_18_WakeBtn_IRQ1,
		kPINT_PinInt0,
		kPINT_PinIntEnableFallEdge,
		Alx_IrqPriority_0
	);


	///------------------------------------------------------------------------------
	/// ALX - Adc
	///------------------------------------------------------------------------------
	me->adcIoPinArr[0] = &me->alxIoPin.ai_P0_23_ADC_CH0;
	me->adcIoPinArr[1] = &me->alxIoPin.ai_P0_16_ADC_CH8;
	me->adcChArr[0] = Alx_Ch_0;
	me->adcChArr[1] = Alx_Ch_8;
	AlxAdc_Ctor
	(
		&me->alxAdc,
		me->adcIoPinArr,
		me->adcChArr,
		ALX_ARR_LEN(me->adcIoPinArr),
		&alxClk,
		AlxAdc_Clk_McuLpc55S6x_AdcClk_18MHz75_MainClk_150MHz,
		#if defined ALX_OPTIMIZE_SIZE_ALL
		3300U
		#else
		3.3f
		#endif
	);


	///------------------------------------------------------------------------------
	/// ALX - PWM
	///------------------------------------------------------------------------------
	me->pwmIoPinArr[0] = &me->alxIoPin.do_P1_4_PWM1;
	me->pwmIoPinArr[1] = &me->alxIoPin.do_P1_7_PWM2;
	me->pwmChArr[0] = Alx_Ch_1;
	me->pwmChArr[1] = Alx_Ch_2;
	#if defined ALX_OPTIMIZE_SIZE_ALL
	me->pwmDutyDefaultArr[0] = 444U;
	me->pwmDutyDefaultArr[1] = 666U;
	#else
	me->pwmDutyDefaultArr[0] = 44.44f;
	me->pwmDutyDefaultArr[1] = 66.66f;
	#endif
	AlxPwm_Ctor
	(
		&me->alxPwm,
		CTIMER2,
		me->pwmIoPinArr,
		me->pwmChArr,
		ALX_ARR_LEN(me->pwmChArr),
		&alxClk,
		me->pwmDutyDefaultArr,
		0,
		500
	);


	///------------------------------------------------------------------------------
	/// ALX - SPI
	///------------------------------------------------------------------------------
	AlxSpi_Ctor
	(
		&me->alxSpi,
		SPI7,
		&me->alxIoPin.do_P0_21_SPI_SCLK,
		&me->alxIoPin.do_P0_20_SPI_MOSI,
		&me->alxIoPin.di_P0_19_SPI_MISO,
		&me->alxIoPin.do_P1_20_SPI_nCS,
		AlxSpi_Mode_2,
		AlxSpi_Clk_McuLpc55S6x_SpiClk_10MHz
	);
	AlxSpi_Ctor
	(
		&me->alxSpiAcc,
		SPI3,
		&me->alxIoPin.do_P0_6_SPI_ACC_SCLK,
		&me->alxIoPin.do_P0_3_SPI_ACC_MOSI,
		&me->alxIoPin.di_P0_2_SPI_ACC_MISO,
		&me->alxIoPin.do_P0_4_SPI_ACC_nCS,
		AlxSpi_Mode_2,
		AlxSpi_Clk_McuLpc55S6x_SpiClk_10MHz
	);

	// Info
	me->wasCtorCalled = true;
}


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_LPC_XPRESSO_55S69_MAIN_H
