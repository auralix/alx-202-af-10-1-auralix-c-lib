/**
  ******************************************************************************
  * @file		alxSpi_McuStm32.c
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
// Includes
//******************************************************************************
#include "alxSpi_McuStm32.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxSpi_Ctor_IsClkOk(AlxSpi* me);
static void AlxSpi_Ctor_ParseMode(AlxSpi* me);
static Alx_Status AlxSpi_Reset(AlxSpi* me);
static void AlxSpi_Periph_EnableClk(AlxSpi* me);
static void AlxSpi_Periph_DisableClk(AlxSpi* me);
static void AlxSpi_Periph_ForceReset(AlxSpi* me);
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me);


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
)
{
	// Objects - External
	me->do_SCK = do_SCK;
	me->do_MOSI = do_MOSI;
	me->di_MISO = di_MISO;
	me->do_nCS = do_nCS;
	me->clk = clk;

	// Parameters
	me->hspi.Instance = spi;
	me->mode = mode;
	me->spiClk = spiClk;

	// Variables
	ALX_SPI_ASSERT(AlxSpi_Ctor_IsClkOk(me));
	AlxSpi_Ctor_ParseMode(me);

	me->hspi.Init.Mode = SPI_MODE_MASTER;
	me->hspi.Init.Direction = SPI_DIRECTION_2LINES;
	me->hspi.Init.DataSize = SPI_DATASIZE_8BIT;
	me->hspi.Init.CLKPolarity = me->clkPolarity;
	me->hspi.Init.CLKPhase = me->clkPhase;
	me->hspi.Init.NSS = SPI_NSS_SOFT;
	me->hspi.Init.BaudRatePrescaler = (uint32_t)me->spiClk;
	me->hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	me->hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	me->hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	me->hspi.Init.CRCPolynomial = 0x0007;
	#if defined(ALX_STM32G4)
	me->hspi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	me->hspi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	#endif

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxSpi_Init(AlxSpi* me)
{
	ALX_SPI_ASSERT(me->isInit == false);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->do_SCK);
	AlxIoPin_Init(me->do_MOSI);
	AlxIoPin_Init(me->di_MISO);
	AlxIoPin_Init(me->do_nCS);

	// #2 Release SPI Periphery Reset
	AlxSpi_Periph_ReleaseReset(me);

	// #3 Enable SPI Periphery Clock
	AlxSpi_Periph_EnableClk(me);

	// #4 Init SPI
	if (HAL_SPI_Init(&me->hspi) != HAL_OK) { ALX_SPI_TRACE("ErrInit"); return Alx_Err; };

	// #5 Set isInit
	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxSpi_DeInit(AlxSpi* me)
{
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit SPI
	if (HAL_SPI_DeInit(&me->hspi) != HAL_OK) { ALX_SPI_TRACE("ErrDeInit"); return Alx_Err; };

	// #2 Force SPI Periphery Reset
	AlxSpi_Periph_ForceReset(me);

	// #3 Disable SPI Periphery Clock
	AlxSpi_Periph_DisableClk(me);

	// #4 DeInit GPIO
	AlxIoPin_DeInit(me->do_SCK);
	AlxIoPin_DeInit(me->do_MOSI);
	AlxIoPin_DeInit(me->di_MISO);
	AlxIoPin_DeInit(me->do_nCS);

	// #5 Reset isInit
	me->isInit = false;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{

		status = HAL_SPI_Transmit(&me->hspi, writeData, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // SPI write/read OK
		}
		else
		{
			ALX_SPI_TRACE("ErrHal");
			if(AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;};
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{

		status = HAL_SPI_Receive(&me->hspi, readData, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // SPI write/read OK
		}
		else
		{
			ALX_SPI_TRACE("ErrHal");
			if(AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;};
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)writeData;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try SPI write/read
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{

		status = HAL_SPI_TransmitReceive(&me->hspi, writeData, readData, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // SPI write/read OK
		}
		else
		{
			ALX_SPI_TRACE("ErrHal");
			if(AlxSpi_Reset(me) != Alx_Ok) { ALX_SPI_TRACE("ErrReset"); return Alx_Err;};
			continue;
		}
	}

	// #3 If we are here, SPI write/read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_SPI_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
void AlxSpi_Master_AssertCs(AlxSpi* me)
{
	AlxIoPin_Reset(me->do_nCS);
}
void AlxSpi_Master_DeAssertCs(AlxSpi* me)
{
	AlxIoPin_Set(me->do_nCS);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxSpi_Ctor_IsClkOk(AlxSpi* me)
{
	#if defined(STM32F4)
	if((me->hspi.Instance == SPI1) || (me->hspi.Instance == SPI4))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz)
		)
		{
			if(90000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if((me->hspi.Instance == SPI2) || (me->hspi.Instance == SPI3))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_1MHz4_Pclk1Apb1_45MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_5MHz625_Pclk1Apb1_45MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_11MHz25_Pclk1Apb1_45MHz)
		)
		{
			if(45000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	#if defined(STM32G4)
	if(me->hspi.Instance == SPI1)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz)
		{
			if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	#if !defined(STM32G431xx)
	if(me->hspi.Instance == SPI4)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz)
		{
			if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	if((me->hspi.Instance == SPI2) || (me->hspi.Instance == SPI3))
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32G4_Spi2_Spi3_SpiClk_1MHz33_Pclk1Apb1_170MHz)
		{
			if(170000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif
	#if defined(STM32L0)
	if(me->hspi.Instance == SPI1)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32L0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz)
		{
			if(32000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if(me->hspi.Instance == SPI2)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32L0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz)
		{
			if(32000000UL == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif

	ALX_SPI_ASSERT(false); // We shouldn't get here
	return ALX_NULL;
}
static void AlxSpi_Ctor_ParseMode(AlxSpi* me)
{
	if (me->mode == AlxSpi_Mode_0)	{ me->clkPolarity = SPI_POLARITY_LOW; me->clkPhase = SPI_PHASE_1EDGE; return;}
	if (me->mode == AlxSpi_Mode_1)	{ me->clkPolarity = SPI_POLARITY_LOW; me->clkPhase = SPI_PHASE_2EDGE; return;}
	if (me->mode == AlxSpi_Mode_2)	{ me->clkPolarity = SPI_POLARITY_HIGH; me->clkPhase = SPI_PHASE_1EDGE; return;}
	if (me->mode == AlxSpi_Mode_3)	{ me->clkPolarity = SPI_POLARITY_HIGH; me->clkPhase = SPI_PHASE_2EDGE; return;}

	ALX_SPI_ASSERT(false); // We shouldn't get here
}
static Alx_Status AlxSpi_Reset(AlxSpi* me)
{
	// #1 DeInit SPI
	if (HAL_SPI_DeInit(&me->hspi) != HAL_OK) { ALX_SPI_TRACE("ErrDeInit"); return Alx_Err; }

	// #2 Force SPI Periphery Reset
	AlxSpi_Periph_ForceReset(me);

	// #3 Reset isInit
	me->isInit = false;

	// #4 Release SPI Periphery Reset
	AlxSpi_Periph_ReleaseReset(me);

	// #5 Init SPI
	if (HAL_SPI_Init(&me->hspi) != HAL_OK) { ALX_SPI_TRACE("ErrInit"); return Alx_Err; }

	// #6 Set isInit
	me->isInit = true;

	// #7 Return OK
	return Alx_Ok;
}
static void AlxSpi_Periph_EnableClk(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_CLK_ENABLE(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}
static void AlxSpi_Periph_DisableClk(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_CLK_DISABLE(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}
static void AlxSpi_Periph_ForceReset(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_FORCE_RESET(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me)
{
	bool isErr = true;

	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_RELEASE_RESET(); isErr = false; }
	#endif

	if(isErr)						{ ALX_SPI_ASSERT(false); } // We shouldn't get here
}


#endif // #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))
