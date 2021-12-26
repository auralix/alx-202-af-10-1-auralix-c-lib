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
	//------------------------------------------------------------------------------
	// Port 0
	//------------------------------------------------------------------------------
	//PIO0_0	- Unused
	//PIO0_1	- Unused
	//PIO0_2	- Unused
	//PIO0_3	- Unused
	//PIO0_4	- Unused
	//PIO0_5	- Unused
	//PIO0_6	- Unused
	//PIO0_7	- Unused
	//PIO0_8	- Unused
	AlxIoPin ai_P0_9_ADC_CH4;					// AlxIoPin PCA943X_nINT
	//PIO0_10	- Unused
	AlxIoPin ao_P0_11_CRN_VCC;
	//PIO0_12	- Unused
	//PIO0_13	- combo I2C / MFIO
	//PIO0_14	- combo I2C / MFIO
	//PIO0_15	- Unused
	AlxIoPin io_P0_16_I2C0_SDA;
	AlxIoPin do_P0_17_I2C0_SCL;
	AlxIoPin do_P0_18_LED205_RD;
	AlxIoPin do_P0_19_LED200_GR;
	//PIO0_20	- Unused
	//PIO0_21	- Unused
	//PIO0_22	- Unused
	AlxIoPin di_P0_23_IRQ1;						//AlxIoPin do_P0_23_LED203_GR;
	AlxIoPin do_P0_24_PWM1;						//AlxIoPin do_P0_24_LED202_GR;
	AlxIoPin do_P0_25_PWM2;						//AlxIoPin do_P0_25_LED201_GR;
	//PIO0_26	- Unused
	//PIO0_27	- Unused
	//PIO0_28	- Unused
	//PIO0_29	- Unused
	//PIO0_30	- Unused
	//PIO0_31	- Unused


	//------------------------------------------------------------------------------
	// Port 1
	//------------------------------------------------------------------------------
	//PIO1_0	- Unused
	//PIO1_1	- Unused
	//PIO1_2	- Unused
	//PIO1_3	- Unused
	//PIO1_4	- Unused
	//PIO1_5	- Unused
	//PIO1_6	- Unused
	//PIO1_7	- Unused
	//PIO1_8	- Unused
	//AlxIoPin ai_P0_9_ADC_CH4;					// AlxIoPin PCA943X_nINT
	//PIO1_10	- Unused
	//AlxIoPin ao_P0_11_CRN_VCC;
	//PIO1_12	- Unused
	//PIO1_13	- Unused
	//PIO1_14	- Unused
	//PIO1_15	- Unused
	//AlxIoPin io_P0_16_I2C0_SDA;
	//AlxIoPin do_P0_17_I2C0_SCL;
	//AlxIoPin do_P0_18_LED205_RD;
	//AlxIoPin do_P0_19_LED200_GR;
	//PIO1_20	- Unused
	//PIO1_21	- Unused
	//PIO1_22	- Unused	-> ALX Trace Handle		//AlxIoPin do_P0_22_LED204_GR;
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
	AlxI2c alxI2c_I2C2_Master;

	// Auralix HW NUCLEO-F429ZI C Library Objects
	AlxHwLpcXpresso55S69_MainIoPin alxIoPin;

	// Info
	bool wasCtorCalled;
} AlxHwLpcXpresso55S69_Main;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxHwLpcXpresso55S69_Main_Ctor(AlxHwLpcXpresso55S69_Main* me)
{
	//******************************************************************************
	// ALX - IoPin
	//******************************************************************************
	/*AlxIoPin_Ctor(&me->alxIoPin.do_PB0_LED1_GR,		GPIOB,	GPIO_PIN_0,		GPIO_MODE_OUTPUT_PP,	GPIO_NOPULL,	GPIO_SPEED_FREQ_HIGH,	ALX_NULL,		false		);
	AlxIoPin_Ctor(&me->alxIoPin.do_PB7_LED2_BL,		GPIOB,	GPIO_PIN_7,		GPIO_MODE_OUTPUT_PP,	GPIO_NOPULL,	GPIO_SPEED_FREQ_HIGH,	ALX_NULL,		false		);
	AlxIoPin_Ctor(&me->alxIoPin.do_PB14_LED3_RD,	GPIOB,	GPIO_PIN_14,	GPIO_MODE_OUTPUT_PP,	GPIO_NOPULL,	GPIO_SPEED_FREQ_HIGH,	ALX_NULL,		false		);
	AlxIoPin_Ctor(&me->alxIoPin.do_PB10_I2C2_SCL,	GPIOB,	GPIO_PIN_10,	GPIO_MODE_AF_OD,		GPIO_PULLUP,	GPIO_SPEED_FREQ_HIGH,	GPIO_AF4_I2C2,	ALX_NULL	);
	AlxIoPin_Ctor(&me->alxIoPin.io_PB11_I2C2_SDA,	GPIOB,	GPIO_PIN_11,	GPIO_MODE_AF_OD,		GPIO_PULLUP,	GPIO_SPEED_FREQ_HIGH,	GPIO_AF4_I2C2,	ALX_NULL	);
*/

	//******************************************************************************
	// ALX - Clock
	//******************************************************************************
	/*AlxClk_Ctor
	(
		&alxClk,
		AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz
	);*/


	//******************************************************************************
	// ALX - Trace
	//******************************************************************************
	/*AlxTrace_Ctor
	(
		&alxTrace,
		GPIOD,
		GPIO_PIN_5,
		GPIO_AF7_USART2,
		USART2,
		AlxGlobal_BaudRate_115200
	);*/


	//******************************************************************************
	// ALX - I2C
	//******************************************************************************
	/*AlxI2c_Ctor
	(
		&me->alxI2c_I2C2_Master,
		I2C2,
		&me->alxIoPin.do_PB10_I2C2_SCL,
		&me->alxIoPin.io_PB11_I2C2_SDA,
		AlxI2c_Clk_400kHz
	);*/


	//******************************************************************************
	// Info
	//******************************************************************************
	me->wasCtorCalled = true;
}


#endif // #if defined(ALX_HW_LPC_XPRESSO_55S69_C_TEST)

#ifdef __cplusplus
}
#endif

#endif // ALX_HW_LPC_XPRESSO_55S69_MAIN_H
