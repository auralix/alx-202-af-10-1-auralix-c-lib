/**
  ******************************************************************************
  * @file alxTest.h
  * @brief Auralix C Library - ALX TEST Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-11-26 19:02:52 +0100 (Fri, 26 Nov 2021) $
  ******************************************************************************
  */

#ifndef ALX_TEST_H
#define ALX_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes - Main
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxConfig_Test.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_TEST)


//******************************************************************************
// Includes - Main
//******************************************************************************
#include "alxAdc.h"
#include "alxAudio.h"
#include "alxAudioPlayer.h"
#include "alxAudioVol.h"
#include "alxBound.h"
#include "alxBtn.h"
#include "alxBuild.h"
#include "alxCan.h"
#include "alxCanParser.h"
#include "alxClk.h"
#include "alxCrc.h"
#include "alxDac.h"
#include "alxDbgPin.h"
#include "alxDelay.h"
#include "alxFifo.h"
#include "alxFiltGlitchBool.h"
#include "alxFiltGlitchUint32.h"
#include "alxHys1.h"
#include "alxHys2.h"
#include "alxI2c.h"
#include "alxI2s.h"
#include "alxId.h"
#include "alxInterpLin.h"
#include "alxIoPin.h"
#include "alxIoPinIrq.h"
#include "alxLinFun.h"
#include "alxMemRaw.h"
#include "alxMemSafe.h"
#include "alxParamGroup.h"
#include "alxParamItem.h"
#include "alxParamMgmt.h"
#include "alxParamStore.h"
#include "alxPwm.h"
#include "alxRange.h"
#include "alxRtc.h"
#include "alxSerialPort.h"
#include "alxSpi.h"
#include "alxTempSens.h"
#include "alxTempSens_RtdVdiv.h"
#include "alxTimSw.h"
#include "alxTpl.h"
#include "alxTrace.h"
#include "alxTrace_Printf.h"
#include "alxVdiv.h"


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	//-----------
	// AlxAdc
	//-----------
	#if defined(ALX_TEST_ADC)
	AlxIoPin adcTest_IoPin;
	AlxIoPin* adcTest_IoPinArr[1];
	Alx_Ch adcTest_ChArr[1];
	AlxAdc adcTest;
	#endif


	//-----------
	// AlxCan
	//-----------
	#if defined(ALX_TEST_CAN)
	// TODO
	AlxCan canTest;
	#endif


	//-----------
	// AlxClk
	//-----------
	// MF: Don't need, Already in alxClk.c


	//-----------
	// AlxDac
	//-----------
	#if defined(ALX_TEST_DAC)
	// TODO
	AlxDac dacTest;
	#endif


	//------------
	// AlxI2c
	//------------
	#if defined(ALX_TEST_I2C)
	// TODO MF: Popravit da bo delalo
	AlxIoPin do_SDA_P25 = { 0 };
	AlxIoPin do_SCK_P24 = { 0 };
	uint16_t slaveAddr = 0x38;
	uint16_t memAddr = 0x4000U;
	uint8_t i2cData[6] = { 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00 };
	AlxI2c i2cTest;
	#endif


	//-----------
	// AlxI2s
	//-----------
	#if defined(ALX_TEST_I2S)
	// TODO
	AlxI2s i2sTest;
	#endif


	//-------------
	// AlxIoPin
	//-------------
	#if defined(ALX_TEST_IO_PIN)
	// TODO MF: Popravit da bo delalo
	AlxIoPin do_nPIN = { 0 };
	AlxIoPinIrq di_P00_irq = { kPINT_PinInt0, kPINT_PinIntEnableRiseEdge, Alx_IrqPriority_0 };
	AlxIoPin di_P00 = { 0 };
	AlxIoPinIrq di_P04_irq = { kPINT_PinInt1, kPINT_PinIntEnableFallEdge, Alx_IrqPriority_0 };
	AlxIoPin di_P04 = { 0 };
	AlxIoPin ioPinTest;
	#endif


	//----------------
	// AlxIoPinIrq
	//----------------
	#if defined(ALX_TEST_IO_PIN_IRQ)
	// TODO
	AlxIoPinIrq ioPinIrqTest;
	#endif


	//-----------
	// AlxPwm
	//-----------
	#if defined(ALX_TEST_PWM)
	// TODO MF: Popravit da bo delalo
	AlxIoPin do_P24 = { 0 };
	AlxIoPin do_P25 = { 0 };
	AlxIoPin* pwmIoPinArr[2] = { &do_P24, &do_P25 };
	Alx_Ch pwmChArr[2] = { Alx_Ch_1, Alx_Ch_2 };
	float pwmDutyDefaultArr[2] = { 50.f, 50.f };
	float pwmDuty_pct = 50;
	AlxPwm pwmTest;
	#endif


	//-----------
	// AlxRtc
	//-----------
	#if defined(ALX_TEST_RTC)
	// TODO
	AlxRtc rtcTest;
	#endif


	//------------------
	// AlxSerialPort
	//------------------
	#if defined(ALX_TEST_SERIALPORT)
	// TODO
	AlxSerialPort serialPortTest;
	#endif


	//-----------
	// AlxSpi
	//-----------
	#if defined(ALX_TEST_SPI)
	// TODO
	AlxSpi spiTest;
	#endif


	//-------------
	// AlxTrace
	//-------------
	// MF: Don't need, Already in alxTrace.c
} AlxTest;


//******************************************************************************
// Functions
//******************************************************************************
void AlxTest_Ctor(AlxTest* me);
Alx_Status AlxTest_Init(AlxTest* me);
Alx_Status AlxTest_DeInit(AlxTest* me);
void AlxTest_Run_Adc(AlxTest* me);
void AlxTest_Run_Audio(AlxTest* me);
// TODO
void AlxTest_Run_Vdiv(AlxTest* me);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_TEST_MCU_H
