/**
  ******************************************************************************
  * @file alxI2s_McuStm32.h
  * @brief Auralix C Library - ALX I2S Module
  * @version $LastChangedRevision: 4590 $
  * @date $LastChangedDate: 2021-04-04 03:50:47 +0200 (Sun, 04 Apr 2021) $
  ******************************************************************************
  */

#ifndef ALX_I2S_MCU_STM32_H
#define ALX_I2S_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F4)
	AlxI2s_Clk_McuStm32F4_SampleFreq_16kHz_SaiClk_49MHz143_MainPllInputClk_2MHz = SAI_AUDIO_FREQUENCY_16K,
	#endif
	#if defined(STM32G4)
	#endif
} AlxI2s_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin* do_MCLK;
	AlxIoPin* do_BCLK;
	AlxIoPin* do_LRCLK;
	AlxIoPin* do_SDO;
	AlxIoPin* di_SDI;
	AlxClk* clk;

	// Parameters
	AlxI2s_Clk i2sClk;
	uint32_t protocolDataSize;
	bool isMclkUsed;
	Alx_IrqPriority irqPriority;

	// Variables
	RCC_PeriphCLKInitTypeDef iclk;
	SAI_HandleTypeDef hsaiTx;
	SAI_HandleTypeDef hsaiRx;
	uint32_t sampleBuffTxL;
	uint32_t sampleBuffTxR;
	uint32_t sampleBuffRxL;
	uint32_t sampleBuffRxR;
	bool isTxChLeft;
	bool isRxChLeft;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxI2s;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxI2s_Ctor
(
	AlxI2s* me,
	SAI_Block_TypeDef* i2s,
	AlxIoPin* do_MCLK,
	AlxIoPin* do_BCLK,
	AlxIoPin* do_LRCLK,
	AlxIoPin* do_SDO,
	AlxIoPin* di_SDI,
	AlxClk* clk,
	AlxI2s_Clk i2sClk,
	uint32_t audioMode,
	uint32_t dataSize,
	bool isMclkUsed,
	Alx_IrqPriority txIrqPriority
);

#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_I2S_MCU_STM32_H






































//	uint8_t* txAudioPtr;
//	uint16_t txAudioSize;
//	uint16_t txPrtHandle;
//	uint8_t* rxAudioPtr;
//	uint16_t rxAudioSize;
//	uint16_t rxPrtHandle;

//	bool isErr;
//	bool isTxAudio;
//	bool isRxAudio;
//	// TODO Channel TX Flag
//	// TODO Channel RX Flag
