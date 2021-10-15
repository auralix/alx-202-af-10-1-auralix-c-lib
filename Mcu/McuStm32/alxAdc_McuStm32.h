/**
  ******************************************************************************
  * @file alxAdc_McuStm32.h
  * @brief ALX ADC Module
  * @version $LastChangedRevision: 5260 $
  * @date $LastChangedDate: 2021-05-28 01:47:42 +0200 (Fri, 28 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADC_MCU_STM32_H
#define ALX_ADC_MCU_STM32_H

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
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F1)
	AlxAdc_Clk_McuStm32F1_AdcClk_4MHz_Pclk2Apb2_8MHz = RCC_ADCPCLK2_DIV2,
	#endif
	#if defined(STM32F4)
	AlxAdc_Clk_McuStm32F4_AdcClk_22MHz5_Pclk2Apb2_90MHz = ADC_CLOCK_SYNC_PCLK_DIV4,
	AlxAdc_Clk_McuStm32F4_AdcClk_11MHz25_Pclk2Apb2_90MHz = ADC_CLOCK_SYNC_PCLK_DIV8,
	#endif
	#if defined(STM32G4)
	AlxAdc_Clk_McuStm32G4_AdcClk_42MHz5_Pclk2Apb2_170MHz = ADC_CLOCK_SYNC_PCLK_DIV4,
	#endif
	#if defined(STM32L0)
	AlxAdc_Clk_McuStm32L0_AdcClk_525kHz_Pclk2Apb2_2MHz1 = ADC_CLOCK_SYNC_PCLK_DIV4,
	AlxAdc_Clk_McuStm32L0_AdcClk_8MHz_Pclk2Apb2_32MHz = ADC_CLOCK_SYNC_PCLK_DIV4
	#endif
} AlxAdc_Clk;

typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;
	AlxClk* clk;

	// Parameters
	uint8_t numOfIoPins;
	Alx_Ch* chArr;
	uint8_t numOfCh;
	AlxAdc_Clk adcClk;
	uint32_t resolution;
	bool isVrefInt_V;
	float vrefExt_V;

	// Variables
	RCC_PeriphCLKInitTypeDef iclk;
	ADC_HandleTypeDef hadc;
	DMA_TypeDef* dma;
	DMA_HandleTypeDef hdma;
	ADC_ChannelConfTypeDef chadc[ALX_ADC_BUFF_LEN];
	Alx_Ch ch[ALX_ADC_BUFF_LEN];
	uint32_t buff[ALX_ADC_BUFF_LEN];

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdc_Ctor
(
	AlxAdc* me,
	ADC_TypeDef* adc,
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins,
	Alx_Ch* chArr,
	uint8_t numOfCh,
	AlxClk* clk,
	AlxAdc_Clk adcClk,
	uint32_t samplingTime,
	bool isVrefInt_V,
	float vrefExt_V
);


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_ADC_MCU_STM32_H
