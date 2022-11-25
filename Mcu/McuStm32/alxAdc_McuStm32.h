/**
  ******************************************************************************
  * @file		alxAdc_McuStm32.h
  * @brief		Auralix C Library - ALX ADC MCU STM32 Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Include Guard
//******************************************************************************
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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F0)
	AlxAdc_Clk_McuStm32F0_AdcClk_14MHz_Pclk1Apb1_8MHz = ADC_CLOCK_ASYNC_DIV1,
	#endif
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

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		adc
  * @param[in]		ioPinArr
  * @param[in]		numOfIoPins
  * @param[in]		chArr
  * @param[in]		numOfCh
  * @param[in]		clk
  * @param[in]		adcClk
  * @param[in]		samplingTime
  * @param[in]		isVrefInt_V
  * @param[in]		vrefExt_V
  */
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


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_MCU_STM32_H
