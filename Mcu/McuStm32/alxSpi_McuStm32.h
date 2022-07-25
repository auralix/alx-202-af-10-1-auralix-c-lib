/**
  ******************************************************************************
  * @file		alxSpi_McuStm32.h
  * @brief		Auralix C Library - ALX SPI MCU STM32 Module
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
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F4)
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz = SPI_BAUDRATEPRESCALER_64,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz = SPI_BAUDRATEPRESCALER_16,
	AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz = SPI_BAUDRATEPRESCALER_8,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_1MHz4_Pclk1Apb1_45MHz = SPI_BAUDRATEPRESCALER_32,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_5MHz625_Pclk1Apb1_45MHz = SPI_BAUDRATEPRESCALER_8,
	AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_11MHz25_Pclk1Apb1_45MHz = SPI_BAUDRATEPRESCALER_4,
	#endif
	#if defined(STM32G4)
	AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz = SPI_BAUDRATEPRESCALER_128,
	AlxSpi_Clk_McuStm32G4_Spi2_Spi3_SpiClk_1MHz33_Pclk1Apb1_170MHz = SPI_BAUDRATEPRESCALER_128,
	#endif
	#if defined(STM32L0)
	AlxSpi_Clk_McuStm32L0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz = SPI_BAUDRATEPRESCALER_32,
	AlxSpi_Clk_McuStm32L0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz = SPI_BAUDRATEPRESCALER_32,
	#endif
} AlxSpi_Clk;

typedef enum
{
	AlxSpi_Mode_0 = 0,	// CPOL = 0, CPHA = 0 --> Clock is idle at 0, data is sampled at leading clock edge, shifted at trailing clock edge
	AlxSpi_Mode_1 = 1,	// CPOL = 0, CPHA = 1 --> Clock is idle at 0, data is sampled at trailing clock edge, shifted at leading clock edge
	AlxSpi_Mode_2 = 2,	// CPOL = 1, CPHA = 0 --> Clock is idle at 1, data is sampled at leading clock edge, shifted at trailing clock edge
	AlxSpi_Mode_3 = 3	// CPOL = 1, CPHA = 1 --> Clock is idle at 1, data is sampled at trailing clock edge, shifted at leading clock edge
} AlxSpi_Mode;

typedef struct
{
	// Objects - External
	AlxIoPin* do_SCK;
	AlxIoPin* do_MOSI;
	AlxIoPin* di_MISO;
	AlxIoPin* do_nCS;
	AlxClk* clk;

	// Parameters
	AlxSpi_Mode mode;
	AlxSpi_Clk spiClk;

	// Variables
	uint32_t clkPolarity;
	uint32_t clkPhase;
	SPI_HandleTypeDef hspi;

	// Info
	bool isInit;
	bool wasCtorCalled;
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
	AlxClk* clk,
	AlxSpi_Clk spiClk
);


#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_SPI_MCU_STM32_H
