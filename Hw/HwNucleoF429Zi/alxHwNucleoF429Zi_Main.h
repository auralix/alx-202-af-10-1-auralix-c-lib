/**
  ******************************************************************************
  * @file alxHwNucleoF429Zi_Main.h
  * @brief Auralix HW NUCLEO-F429ZI C Library - Main Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HW_NUCLEO_F429ZI_MAIN_H
#define ALX_HW_NUCLEO_F429ZI_MAIN_H

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
#include <alxDbgPin.h>
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
#if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	//******************************************************************************
	// Port A
	//******************************************************************************
	// PA0	- Unused
	// PA1	- Unused
	// PA2	- Unused
	// PA3	- Unused
	// PA4	- Unused
	// PA5	- Unused
	// PA6	- Unused
	// PA7	- Unused
	// PA8	- Unused
	// PA9	- Unused
	// PA10	- Unused
	// PA11	- Unused
	// PA12	- Unused
	// PA13	- SWD_IO
	// PA14	- SWD_CLK
	// PA15	- Unused


	//******************************************************************************
	// Port B
	//******************************************************************************
	AlxIoPin do_PB0_LED1_GR;
	// PB1	- Unused
	// PB2	- Unused
	// PB3	- Unused
	// PB4	- Unused
	// PB5	- Unused
	// PB6	- Unused
	AlxIoPin do_PB7_LED2_BL;
	// PB8	- Unused
	// PB9	- Unused
	AlxIoPin do_PB10_I2C2_SCL;
	AlxIoPin io_PB11_I2C2_SDA;
	// PB12	- Unused
	// PB13	- Unused
	AlxIoPin do_PB14_LED3_RD;
	// PB15	- Unused


	//******************************************************************************
	// Port C
	//******************************************************************************
	// PC0	- Unused
	// PC1	- Unused
	// PC2	- Unused
	// PC3	- Unused
	// PC4	- Unused
	// PC5	- Unused
	// PC6	- Unused
	// PC7	- Unused
	// PC8	- Unused
	// PC9	- Unused
	// PC10	- Unused
	// PC11	- Unused
	// PC12	- Unused
	// PC13	- Unused
	// PC14	- LSE
	// PC15	- LSE


	//******************************************************************************
	// Port D
	//******************************************************************************
	// PD0	- Unused
	// PD1	- Unused
	// PD2	- Unused
	// PD3	- Unused
	// PD4	- Unused
	// PD5	- DBG_UART_TX	-> ALX Trace Handle
	// PD6	- Unused
	// PD7	- Unused
	// PD8	- Unused
	// PD9	- Unused
	// PD10	- Unused
	// PD11	- Unused
	// PD12	- Unused
	// PD13	- Unused
	// PD14	- Unused
	// PD15	- Unused


	//******************************************************************************
	// Port E
	//******************************************************************************
	// PE0	- Unused
	// PE1	- Unused
	// PE2	- Unused
	// PE3	- Unused
	// PE4	- Unused
	// PE5	- Unused
	// PE6	- Unused
	// PE7	- Unused
	// PE8	- Unused
	// PE9	- Unused
	// PE10	- Unused
	// PE11	- Unused
	// PE12	- Unused
	// PE13	- Unused
	// PE14	- Unused
	// PE15	- Unused


	//******************************************************************************
	// Port F
	//******************************************************************************
	// PF0	- Unused
	// PF1	- Unused
	// PF2	- Unused
	// PF3	- Unused
	// PF4	- Unused
	// PF5	- Unused
	// PF6	- Unused
	// PF7	- Unused
	// PF8	- Unused
	// PF9	- Unused
	// PF10	- Unused
	// PF11	- Unused
	// PF12	- Unused
	// PF13	- Unused
	// PF14	- Unused
	// PF15	- Unused


	//******************************************************************************
	// Port G
	//******************************************************************************
	// PG0	- Unused
	// PG1	- Unused
	// PG2	- Unused
	// PG3	- Unused
	// PG4	- Unused
	// PG5	- Unused
	// PG6	- Unused
	// PG7	- Unused
	// PG8	- Unused
	// PG9	- Unused
	// PG10	- Unused
	// PG11	- Unused
	// PG12	- Unused
	// PG13	- Unused
	// PG14	- Unused
	// PG15	- Unused


	//******************************************************************************
	// Port H
	//******************************************************************************
	// PH0 - HSE
	// PH1 - HSE
} AlxHwNucleoF429Zi_MainIoPin;

typedef struct
{
	// ALX Objects
	AlxI2c alxI2c_I2C2_Master;

	// Auralix HW NUCLEO-F429ZI C Library Objects
	AlxHwNucleoF429Zi_MainIoPin alxIoPin;

	// Info
	bool wasCtorCalled;
} AlxHwNucleoF429Zi_Main;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxHwNucleoF429Zi_Main_Ctor(AlxHwNucleoF429Zi_Main* me)
{
	//******************************************************************************
	// ALX - IoPin
	//******************************************************************************
	AlxIoPin_Ctor(&me->alxIoPin.do_PB0_LED1_GR,		GPIOB,	GPIO_PIN_0,		GPIO_MODE_OUTPUT_PP,	GPIO_NOPULL,	GPIO_SPEED_FREQ_HIGH,	ALX_NULL,		false		);
	AlxIoPin_Ctor(&me->alxIoPin.do_PB7_LED2_BL,		GPIOB,	GPIO_PIN_7,		GPIO_MODE_OUTPUT_PP,	GPIO_NOPULL,	GPIO_SPEED_FREQ_HIGH,	ALX_NULL,		false		);
	AlxIoPin_Ctor(&me->alxIoPin.do_PB14_LED3_RD,	GPIOB,	GPIO_PIN_14,	GPIO_MODE_OUTPUT_PP,	GPIO_NOPULL,	GPIO_SPEED_FREQ_HIGH,	ALX_NULL,		false		);
	AlxIoPin_Ctor(&me->alxIoPin.do_PB10_I2C2_SCL,	GPIOB,	GPIO_PIN_10,	GPIO_MODE_AF_OD,		GPIO_PULLUP,	GPIO_SPEED_FREQ_HIGH,	GPIO_AF4_I2C2,	ALX_NULL	);
	AlxIoPin_Ctor(&me->alxIoPin.io_PB11_I2C2_SDA,	GPIOB,	GPIO_PIN_11,	GPIO_MODE_AF_OD,		GPIO_PULLUP,	GPIO_SPEED_FREQ_HIGH,	GPIO_AF4_I2C2,	ALX_NULL	);


	//******************************************************************************
	// ALX - Clock
	//******************************************************************************
	AlxClk_Ctor
	(
		&alxClk,
		AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz
	);


	//******************************************************************************
	// ALX - Trace
	//******************************************************************************
	AlxTrace_Ctor
	(
		&alxTrace,
		GPIOD,
		GPIO_PIN_5,
		GPIO_AF7_USART2,
		USART2,
		AlxGlobal_BaudRate_115200
	);


	//******************************************************************************
	// ALX - I2C
	//******************************************************************************
	AlxI2c_Ctor
	(
		&me->alxI2c_I2C2_Master,
		I2C2,
		&me->alxIoPin.do_PB10_I2C2_SCL,
		&me->alxIoPin.io_PB11_I2C2_SDA,
		AlxI2c_Clk_400kHz
	);


	//******************************************************************************
	// Info
	//******************************************************************************
	me->wasCtorCalled = true;
}


#endif // #if defined(ALX_HW_NUCLEO_F429ZI_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_NUCLEO_F429ZI_MAIN_H
