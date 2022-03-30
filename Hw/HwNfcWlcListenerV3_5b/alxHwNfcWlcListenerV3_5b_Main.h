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
	AlxIoPin di_P0_0_UART_RX;
	//P0_1	- Unused
	//P0_2	- SWD_IO
	//P0_3	- SWD_CLK
	//P0_4 DBG_UART_TX	-> ALX Trace Handle
	//P0_5	- Unused
	//P0_6	- Unused
	AlxIoPin di_P0_7_CRN_FD_IRQ2;
	AlxIoPin do_P0_8_PCA943X_EN;			// JS: PCA943X_EN -- Pca9431 sleep
	AlxIoPin ai_P0_9_ADC_CH4;				// JS: commented Mf needed this for adc
	//AlxIoPin di_P0_9_PCA943X_INT_IRQ1; // JS: PCA943X_I\N\T\
	//P0_10	- Unused
	AlxIoPin ao_P0_11_CRN_VCC;
	//P0_12	- Unused
	AlxIoPin di_P0_13_CRN_ED_IRQ3;
	//P0_14	- Unused
	//P0_15	- Unused
	AlxIoPin io_P0_16_I2C0_SDA;
	AlxIoPin do_P0_17_I2C0_SCL;
	AlxIoPin do_P0_18_LED205_RD;
	AlxIoPin do_P0_19_LED200_GR;
	//P0_20	- Unused
	//P0_21	- Unused
	AlxIoPin do_P0_22_LED204_GR;
	//P0_22 - Unused
	AlxIoPin do_P0_23_LED203_GR;
	AlxIoPin do_P0_24_LED202_GR_PWM1;
	AlxIoPin do_P0_25_LED201_GR_PWM2;
	//P0_26	- Unused
	//P0_27	- Unused
	//P0_28	- Unused
	//P0_29	- Unused
	//P0_30	- Unused
} AlxHwNfcWlcListenerV3_5b_MainIoPin;

typedef struct
{
	//--------------------------------------
	// ALX - IoPin
	//--------------------------------------
	AlxHwNfcWlcListenerV3_5b_MainIoPin alxIoPin;

	//--------------------------------------
	// ALX - I2C
	//--------------------------------------
	AlxI2c alxI2c_I2C0;

	//--------------------------------------
	// ALX - PCA9431
	//--------------------------------------
	AlxIoPinIrq alxIrqPin_IRQ1;
	AlxPca9431 alxPca9431;

	//--------------------------------------
	// Adc
	//--------------------------------------
	AlxAdc alxAdc;
	AlxIoPin* adcIoPinArr[1];
	Alx_Ch adcChArr[1];

	//--------------------------------------
	// Pwm
	//--------------------------------------
	AlxPwm alxPwm;
	AlxIoPin* pwmIoPinArr[2];
	Alx_Ch pwmChArr[2];
	#if defined ALX_OPTIMIZE_SIZE_ALL
	uint16_t pwmDutyDefaultArr[2];
	#else
	float pwmDutyDefaultArr[2];
	#endif

	//--------------------------------------
	// Info
	//--------------------------------------
	bool isInit;
	bool wasCtorCalled;
} AlxHwNfcWlcListenerV3_5b_Main;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxHwNfcWlcListenerV3_5b_Main alxHwNfcWlcListenerV3_5b_Main;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_Ctor(AlxHwNfcWlcListenerV3_5b_Main* me);


//******************************************************************************
// Functions
//******************************************************************************
void AlxHwNfcWlcListenerV3_5b_Main_Init(AlxHwNfcWlcListenerV3_5b_Main* me);
void AlxHwNfcWlcListenerV3_5b_Main_Handle(AlxHwNfcWlcListenerV3_5b_Main* me);


#endif // #if defined(ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NFC_WLC_LISTENER_V3_5B_MAIN_H
