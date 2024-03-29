﻿/**
  ******************************************************************************
  * @file		alxAdc_McuStm32.h
  * @brief		Auralix C Library - ALX ADC MCU STM32 Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_STM32F0)
	AlxAdc_Clk_McuStm32F0_AdcClk_12MHz_Pclk1Apb1_48MHz,
	#endif
	#if defined(ALX_STM32F1)
	AlxAdc_Clk_McuStm32F1_AdcClk_4MHz_Pclk2Apb2_8MHz,
	#endif
	#if defined(ALX_STM32F4)
	AlxAdc_Clk_McuStm32F4_AdcClk_11MHz25_Pclk2Apb2_90MHz,
	AlxAdc_Clk_McuStm32F4_AdcClk_22MHz5_Pclk2Apb2_90MHz,
	#endif
	#if defined(ALX_STM32F7)
	AlxAdc_Clk_McuStm32F7_AdcClk_13MHz5_Pclk2Apb2_108MHz,
	AlxAdc_Clk_McuStm32F7_AdcClk_27MHz_Pclk2Apb2_108MHz,
	#endif
	#if defined(ALX_STM32G4)
	AlxAdc_Clk_McuStm32G4_AdcClk_42MHz5_Pclk2Apb2_170MHz,
	#endif
	#if defined(ALX_STM32L0)
	AlxAdc_Clk_McuStm32L0_AdcClk_525kHz_Pclk2Apb2_2MHz1,
	AlxAdc_Clk_McuStm32L0_AdcClk_8MHz_Pclk2Apb2_32MHz,
	#endif
	#if defined(ALX_STM32L4)
	AlxAdc_Clk_McuStm32L4_AdcClk_20MHz_Sysclk_80MHz,
	AlxAdc_Clk_McuStm32L4_AdcClk_30MHz_Sysclk_120MHz,
	#endif
	#if defined(ALX_STM32U5)
	AlxAdc_Clk_McuStm32U5_AdcClk_16MHz_Hclk_16MHz,
	AlxAdc_Clk_McuStm32U5_AdcClk_20MHz_Hclk_160MHz,
	#endif
} AlxAdc_Clk;

typedef struct
{
	// Defines
	#define ALX_ADC_BUFF_LEN 16
	#define ALX_ADC_RESOLUTION ADC_RESOLUTION_12B

	// Parameters
	ADC_TypeDef* adc;
	AlxIoPin** ioPinArr;
	uint8_t numOfIoPins;
	Alx_Ch* chArr;
	uint8_t numOfCh;
	AlxClk* clk;
	AlxAdc_Clk adcClk;
	uint32_t samplingTime;
	bool isVrefInt_V;
	float vrefExt_V;

	// Variables
	#if defined(ALX_STM32F1)
	RCC_PeriphCLKInitTypeDef iclk;
	#endif
	ADC_HandleTypeDef hadc;
	DMA_TypeDef* dma;
	DMA_HandleTypeDef hdma;
	#if defined(ALX_STM32U5)
	DMA_NodeConfTypeDef ncdma;
	DMA_NodeTypeDef ndma;
	DMA_QListTypeDef qdma;
	#endif
	ADC_ChannelConfTypeDef chadc[ALX_ADC_BUFF_LEN];
	Alx_Ch ch[ALX_ADC_BUFF_LEN];
	uint32_t buff[ALX_ADC_BUFF_LEN];

	// Info
	bool wasCtorCalled;
	bool isInit;
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


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADC_MCU_STM32_H
