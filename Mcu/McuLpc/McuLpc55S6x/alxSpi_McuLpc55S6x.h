/**
  ******************************************************************************
  * @file		alxSpi_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX SPI MCU LPC55S6X Module
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
#ifndef ALX_SPI_MCU_Lpc55S6x_H
#define ALX_SPI_MCU_Lpc55S6x_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxSpi_Clk_McuLpc55S6x_SpiClk_500kHz	= 500000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_1MHz		= 1000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_2MHz		= 2000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_3MHz		= 3000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_4MHz		= 4000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_6MHz		= 6000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_10MHz		= 10000000U,
	AlxSpi_Clk_McuLpc55S6x_SpiClk_12MHz		= 12000000U
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
	SPI_Type* spi;
	AlxIoPin* do_SCK;
	AlxIoPin* do_MOSI;
	AlxIoPin* di_MISO;
	AlxIoPin* do_nCS;

	// Parameters
	AlxSpi_Mode mode;
	AlxSpi_Clk spiClk;

	// Variables
	spi_master_config_t spiMasterConfig;	// MF: clkPolarity and clkPhase are in this struct
	spi_transfer_t spiTransfer;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxSpi;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] spi
  * @param[in] do_SCK
  * @param[in] do_MOSI
  * @param[in] di_MISO
  * @param[in] do_nCS
  * @param[in] mode
  * @param[in] spiClk
  */
void AlxSpi_Ctor
(
	AlxSpi* me,
	SPI_Type* spi,
	AlxIoPin* do_SCK,
	AlxIoPin* do_MOSI,
	AlxIoPin* di_MISO,
	AlxIoPin* do_nCS,
	AlxSpi_Mode mode,
	AlxSpi_Clk spiClk
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SPI_MCU_Lpc55S6x_H
