/**
  ******************************************************************************
  * @file alxHwNfcWlcListenerV3_5b_Main.h
  * @brief Auralix HW NFC WLC Listener V3_5 C Library - Main Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NFC_WLC_LISTENER_V3_5B_MAIN_H
#define ALX_HW_NFC_WLC_LISTENER_V3_5B_MAIN_H

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
#if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	//------------------------------------------------------------------------------
	// Port 0
	//------------------------------------------------------------------------------
	//P0_0	- Unused
	//P0_1	- Unused
	//P0_2	- SWD_IO
	//P0_3	- SWD_CLK
	//P0_4	- Unused
	//P0_5	- Unused
	//P0_6	- Unused
	//P0_7	- Unused
	//P0_8	- Unused
	AlxIoPin ai_P0_9_ADC_In;					// AlxIoPin PCA943X_nINT
	//P0_10	- Unused
	AlxIoPin ao_P0_11_CRN_VCC;
	//P0_12	- Unused
	//P0_13	- Unused
	//P0_14	- Unused
	//P0_15	- Unused
	AlxIoPin io_P0_16_I2C0_SDA;
	AlxIoPin do_P0_17_I2C0_SCL;
	AlxIoPin do_P0_18_LED205_RD;
	AlxIoPin do_P0_19_LED200_GR;
	//P0_20	- Unused
	//P0_21	- Unused
	//P0_22 DBG_UART_TX	-> ALX Trace Handle		//AlxIoPin do_P0_22_LED204_GR;
	AlxIoPin di_P0_23_IRQ1;						//AlxIoPin do_P0_23_LED203_GR;
	AlxIoPin do_P0_24_PWM1;						//AlxIoPin do_P0_24_LED202_GR;
	AlxIoPin do_P0_25_PWM2;						//AlxIoPin do_P0_25_LED201_GR;
	//P0_26	- Unused
	//P0_27	- Unused
	//P0_28	- Unused
	//P0_29	- Unused
	//P0_30	- Unused
} AlxHwNfcWlcListenerV3_5b_MainIoPin;

typedef struct
{
	// ALX Objects
	AlxIoPinIrq alxIrqPin_IRQ1;
	AlxI2c alxI2c_I2C0_Master;
	AlxAdc alxAdc_ADC4_Master;
	AlxPwm alxPwm_Master;

	// Auralix HW NFC WLC LISTENER V3_5B C Library Objects
	AlxHwNfcWlcListenerV3_5b_MainIoPin alxIoPin;

	//--------
	// Adc
	//--------
	AlxIoPin* adcIoPinArr[1];
	Alx_Ch adcChArr[1];

	//--------
	// Pwm
	//--------
	AlxIoPin* pwmIoPinArr[2];
	Alx_Ch pwmChArr[2];
	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint16_t pwmDutyDefaultArr[2];
	#else
	float pwmDutyDefaultArr[2];
	#endif

	// Info
	bool wasCtorCalled;
} AlxHwNfcWlcListenerV3_5b_Main;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxHwNfcWlcListenerV3_5b_Main_Ctor(AlxHwNfcWlcListenerV3_5b_Main* me)
{
	//------------------------------------------------------------------------------
	// ALX - IoPin
	//------------------------------------------------------------------------------
	//P0_0	- Unused
	//P0_1	- Unused
	//P0_2	- SWD_IO
	//P0_3	- SWD_CLK
	//P0_4	- Unused
	//P0_5	- Unused
	//P0_6	- Unused
	//P0_7	- Unused
	//P0_8	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.ai_P0_9_ADC_In,		0,	9,	AlxIoPin_Func_Swm_ADC_CHN4,		IOCON_MODE_INACT,	false,	false,	false	);
	//P0_10	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.ao_P0_11_CRN_VCC,	0,	11,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	true	);
	//P0_12	- Unused
	//P0_13	- Unused
	//P0_14	- Unused
	//P0_15	- Unused
	AlxIoPin_Ctor(&me->alxIoPin.io_P0_16_I2C0_SDA,	0,	16,	AlxIoPin_Func_Swm_I2C0_SDA,		IOCON_MODE_INACT,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_17_I2C0_SCL,	0,	17,	AlxIoPin_Func_Swm_I2C0_SCL,		IOCON_MODE_INACT,	false,	false,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_18_LED205_RD,	0,	18,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	false	);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_19_LED200_GR,	0,	19,	AlxIoPin_Func_GPIO,				IOCON_MODE_PULLUP,	false,	true,	false	);
	//P0_20	- Unused
	//P0_21	- Unused
	//P0_22 DBG_UART_TX	-> ALX Trace Handle																									//AlxIoPin_Ctor(&me->alxIoPin.do_P0_22_LED204_GR, 0, 22, AlxIoPin_Func_GPIO, IOCON_MODE_PULLUP, false, true, false);
	AlxIoPin_Ctor(&me->alxIoPin.di_P0_23_IRQ1,		0,	23,	AlxIoPin_Func_IRQ,				IOCON_MODE_INACT,	false,	true,	true	);	//AlxIoPin_Ctor(&me->alxIoPin.do_P0_23_LED203_GR, 0, 23, AlxIoPin_Func_GPIO, IOCON_MODE_PULLUP, false, true, false);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_24_PWM1,		0,	24,	AlxIoPin_Func_Swm_T0_MAT_CHN1,	IOCON_MODE_INACT,	false,	false,	false	);	//AlxIoPin_Ctor(&me->alxIoPin.do_P0_24_LED202_GR, 0, 24, AlxIoPin_Func_GPIO, IOCON_MODE_PULLUP, false, true, false);
	AlxIoPin_Ctor(&me->alxIoPin.do_P0_25_PWM2,		0,	25,	AlxIoPin_Func_Swm_T0_MAT_CHN2,	IOCON_MODE_INACT,	false,	false,	false	);	//AlxIoPin_Ctor(&me->alxIoPin.do_P0_25_LED201_GR, 0, 25, AlxIoPin_Func_GPIO, IOCON_MODE_PULLUP, false, true, false);
	//P0_26	- Unused
	//P0_27	- Unused
	//P0_28	- Unused
	//P0_29	- Unused
	//P0_30	- Unused


	//------------------------------------------------------------------------------
	// ALX - IrqPin
	//------------------------------------------------------------------------------
	AlxIoPinIrq_Ctor
	(
		&me->alxIrqPin_IRQ1,
		&me->alxIoPin.di_P0_23_IRQ1,
		kPINT_PinInt0,
		kPINT_PinIntEnableRiseEdge,
		Alx_IrqPriority_0
	);


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
		0,
		22,
		USART0,
		AlxGlobal_BaudRate_115200
	);


	//------------------------------------------------------------------------------
	// ALX - ADC
	//------------------------------------------------------------------------------
	me->adcIoPinArr[0] = &me->alxIoPin.ai_P0_9_ADC_In;
	me->adcChArr[0] = Alx_Ch_4;
	#if defined ALX_OPTIMIZE_SIZE_ALL
	AlxAdc_Ctor
	(
		&me->alxAdc_ADC4_Master,
		me->adcIoPinArr,
		me->adcChArr,
		ALX_ARR_LEN(me->adcIoPinArr),
		&alxClk,
		3300U
	);
	#else
	AlxAdc_Ctor
	(
		&me->alxAdc_ADC4_Master,
		me->adcIoPinArr,
		me->adcChArr,
		ALX_ARR_LEN(me->adcIoPinArr),
		&alxClk,
		3.3f
	);
	#endif


	//------------------------------------------------------------------------------
	// ALX - Pwm
	//------------------------------------------------------------------------------
	me->pwmIoPinArr[0] = &me->alxIoPin.do_P0_24_PWM1;
	me->pwmIoPinArr[1] = &me->alxIoPin.do_P0_25_PWM2;
	me->pwmChArr[0] = Alx_Ch_1;
	me->pwmChArr[1] = Alx_Ch_2;
	#if defined ALX_OPTIMIZE_SIZE_ALL
	me->pwmDutyDefaultArr[0] = 543U;
	me->pwmDutyDefaultArr[1] = 123U;
	#else
	me->pwmDutyDefaultArr[0] = 12.34f;
	me->pwmDutyDefaultArr[1] = 50.f;
	#endif
	AlxPwm_Ctor
	(
		&me->alxPwm_Master,
		CTIMER0,
		me->pwmIoPinArr,
		me->pwmChArr,
		ALX_ARR_LEN(me->pwmChArr),
		&alxClk,
		me->pwmDutyDefaultArr,
		0,
		100
	);


	//------------------------------------------------------------------------------
	// ALX - I2C
	//------------------------------------------------------------------------------
	AlxI2c_Ctor
	(
		&me->alxI2c_I2C0_Master,
		I2C0,
		&me->alxIoPin.do_P0_17_I2C0_SCL,
		&me->alxIoPin.io_P0_16_I2C0_SDA,
		AlxI2c_Clk_McuLpc80x_BitRate_100kHz_I2cFuncClk_15MHz
	);


	//------------------------------------------------------------------------------
	// Info
	//------------------------------------------------------------------------------
	me->wasCtorCalled = true;
}


#endif // #if defined(ALX_HW_NFC_ WLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFC_WLC_LISTENER_V3_5B_MAIN_H
