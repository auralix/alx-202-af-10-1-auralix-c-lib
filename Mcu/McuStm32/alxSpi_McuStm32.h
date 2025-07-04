﻿/**
  ******************************************************************************
  * @file		alxSpi_McuStm32.h
  * @brief		Auralix C Library - ALX SPI MCU STM32 Module
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
#ifndef ALX_SPI_MCU_STM32_H
#define ALX_SPI_MCU_STM32_H

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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_STM32F4)
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_350kHz_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_700kHz_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_22MHz5_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_45MHz_Pclk2Apb2_90MHz,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_1MHz4_Pclk1Apb1_45MHz,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_5MHz625_Pclk1Apb1_45MHz,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_11MHz25_Pclk1Apb1_45MHz,
	#endif
	#if defined(ALX_STM32F7)
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_422kHz_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_844kHz_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_1MHz688_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_3MHz375_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_6MHz75_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_13MHz5_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_27MHz_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_54MHz_Pclk2Apb2_108MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_211kHz_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_422kHz_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_844kHz_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_1MHz688_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_3MHz375_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_6MHz75_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_13MHz5_Pclk1Apb1_54MHz,
	AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_27MHz_Pclk1Apb1_54MHz,
	#endif
	#if defined(ALX_STM32G4)
	AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz,
	AlxSpi_Clk_McuStm32G4_Spi2_Spi3_SpiClk_1MHz33_Pclk1Apb1_170MHz,
	#endif
	#if defined(ALX_STM32L0)
	AlxSpi_Clk_McuStm32L0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz,
	AlxSpi_Clk_McuStm32L0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz,
	#endif
	#if defined(ALX_STM32L4)
	AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_5MHz_Pclk2Apb2_80MHz,
	AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_10MHz_Pclk2Apb2_80MHz,
	AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_20MHz_Pclk2Apb2_80MHz,
	AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_7MHz5_Pclk2Apb2_120MHz,
	AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_15MHz_Pclk2Apb2_120MHz,
	AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_30MHz_Pclk2Apb2_120MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_5MHz_Pclk1Apb1_80MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_10MHz_Pclk1Apb1_80MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_20MHz_Pclk1Apb1_80MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_1MHz875_Pclk1Apb1_120MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_7MHz5_Pclk1Apb1_120MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_15MHz_Pclk1Apb1_120MHz,
	AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_30MHz_Pclk1Apb1_120MHz,
	#endif
} AlxSpi_Clk;

typedef enum
{
	AlxSpi_Mode_0 = 0,	// CPOL = 0, CPHA = 0 --> Clock is idle at 0, data is sampled at leading clock edge, shifted at trailing clock edge
	AlxSpi_Mode_1 = 1,	// CPOL = 0, CPHA = 1 --> Clock is idle at 0, data is sampled at trailing clock edge, shifted at leading clock edge
	AlxSpi_Mode_2 = 2,	// CPOL = 1, CPHA = 0 --> Clock is idle at 1, data is sampled at leading clock edge, shifted at trailing clock edge
	AlxSpi_Mode_3 = 3	// CPOL = 1, CPHA = 1 --> Clock is idle at 1, data is sampled at trailing clock edge, shifted at leading clock edge
} AlxSpi_Mode;

typedef enum
{
	AlxSpi_DataSize_4bit,
	AlxSpi_DataSize_5bit,
	AlxSpi_DataSize_6bit,
	AlxSpi_DataSize_7bit,
	AlxSpi_DataSize_8bit,
	AlxSpi_DataSize_9bit,
	AlxSpi_DataSize_10bit,
	AlxSpi_DataSize_11bit,
	AlxSpi_DataSize_12bit,
	AlxSpi_DataSize_13bit,
	AlxSpi_DataSize_14bit,
	AlxSpi_DataSize_15bit,
	AlxSpi_DataSize_16bit,
} AlxSpi_DataSize;

typedef struct
{
	// Parameters
	SPI_TypeDef* spi;
	AlxIoPin* do_SCK;
	AlxIoPin* do_MOSI;
	AlxIoPin* di_MISO;
	AlxIoPin* do_nCS;
	AlxSpi_Mode mode;
	AlxSpi_DataSize dataSize;
	AlxClk* clk;
	AlxSpi_Clk spiClk;
	bool isWriteReadLowLevel;

	// Variables
	uint32_t clkPolarity;
	uint32_t clkPhase;
	SPI_HandleTypeDef hspi;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxSpi;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSpi_Ctor
(
	AlxSpi* me,
	SPI_TypeDef* spi,
	AlxIoPin* do_SCK,
	AlxIoPin* do_MOSI,
	AlxIoPin* di_MISO,
	AlxIoPin* do_nCS,
	AlxSpi_Mode mode,
	AlxSpi_DataSize dataSize,
	AlxClk* clk,
	AlxSpi_Clk spiClk,
	bool isWriteReadLowLevel
);

Alx_Status AlxSpi_Reconfigure
(
	AlxSpi* me,
	AlxSpi_Mode mode,
	AlxSpi_DataSize dataSize,
	AlxSpi_Clk spiClk,
	bool isWriteReadLowLevel
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SPI_MCU_STM32_H
