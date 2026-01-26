/**
  ******************************************************************************
  * @file		alxSpi_McuStm32.c
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
// Includes
//******************************************************************************
#include "alxSpi_McuStm32.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxSpi_ParseMode(AlxSpi* me);
static uint32_t AlxSpi_GetDataSize(AlxSpi* me);
static uint32_t AlxSpi_GetClkPrescaler(AlxSpi* me);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxSpi_Reset(AlxSpi* me);
static bool AlxSpi_IsClkOk(AlxSpi* me);
static void AlxSpi_Periph_EnableClk(AlxSpi* me);
static void AlxSpi_Periph_DisableClk(AlxSpi* me);
static void AlxSpi_Periph_ForceReset(AlxSpi* me);
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		spi
  * @param[in]		do_SCK
  * @param[in]		do_MOSI
  * @param[in]		di_MISO
  * @param[in]		do_nCS
  * @param[in]		mode
  * @param[in]		clk
  * @param[in]		spiClk
  */
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
)
{
	// Parameters
	me->spi = spi;
	me->do_SCK = do_SCK;
	me->do_MOSI = do_MOSI;
	me->di_MISO = di_MISO;
	me->do_nCS = do_nCS;
	me->mode = mode;
	me->dataSize = dataSize;
	me->clk = clk;
	me->spiClk = spiClk;
	me->isWriteReadLowLevel = isWriteReadLowLevel;

	// Variables
	AlxSpi_ParseMode(me);
	me->hspi.Instance = spi;
	me->hspi.Init.Mode = SPI_MODE_MASTER;
	me->hspi.Init.Direction = SPI_DIRECTION_2LINES;
	me->hspi.Init.DataSize = AlxSpi_GetDataSize(me);
	me->hspi.Init.CLKPolarity = me->clkPolarity;
	me->hspi.Init.CLKPhase = me->clkPhase;
	me->hspi.Init.NSS = SPI_NSS_SOFT;
	me->hspi.Init.BaudRatePrescaler = AlxSpi_GetClkPrescaler(me);
	me->hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	me->hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	me->hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	me->hspi.Init.CRCPolynomial = 0x0007;
	#if defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4)
	me->hspi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	me->hspi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	#endif

	// Check clock
	ALX_SPI_ASSERT(AlxSpi_IsClkOk(me));

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSpi_Init(AlxSpi* me)
{
	// Assert
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == false);

	// Init GPIO
	AlxIoPin_Init(me->do_SCK);
	AlxIoPin_Init(me->do_MOSI);
	AlxIoPin_Init(me->di_MISO);
	AlxIoPin_Init(me->do_nCS);

	// Release SPI periphery reset
	AlxSpi_Periph_ReleaseReset(me);

	// Enable SPI periphery clock
	AlxSpi_Periph_EnableClk(me);

	// Init SPI
	if (HAL_SPI_Init(&me->hspi) != HAL_OK)
	{
		ALX_SPI_TRACE_ERR("Err");
		return Alx_Err;
	};

	// If write/read low level, set FIFO threshold & enable SPI
	if (me->isWriteReadLowLevel)
	{
		#if defined(ALX_STM32F7)
		LL_SPI_SetRxFIFOThreshold(me->hspi.Instance, LL_SPI_RX_FIFO_TH_QUARTER);
		#endif
		LL_SPI_Enable(me->hspi.Instance);
	}

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSpi_DeInit(AlxSpi* me)
{
	// Assert
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);

	// DeInit SPI
	if (HAL_SPI_DeInit(&me->hspi) != HAL_OK)
	{
		ALX_SPI_TRACE_ERR("Err");
		return Alx_Err;
	};

	// Force SPI periphery reset
	AlxSpi_Periph_ForceReset(me);

	// Disable SPI periphery clock
	AlxSpi_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_SCK);
	AlxIoPin_DeInit(me->do_MOSI);
	AlxIoPin_DeInit(me->di_MISO);
	AlxIoPin_DeInit(me->do_nCS);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

Alx_Status AlxSpi_Reconfigure
(
	AlxSpi* me,
	AlxSpi_Mode mode,
	AlxSpi_DataSize dataSize,
	AlxSpi_Clk spiClk,
	bool isWriteReadLowLevel
)
{
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	bool wasInit = false;

	if (me->isInit)
	{
		AlxSpi_DeInit(me);
		wasInit = true;
	}

	me->mode = mode;
	me->dataSize = dataSize;
	me->spiClk = spiClk;
	me->isWriteReadLowLevel = isWriteReadLowLevel;

	me->hspi.Init.DataSize = AlxSpi_GetDataSize(me);

	AlxSpi_ParseMode(me);
	me->hspi.Init.CLKPolarity = me->clkPolarity;
	me->hspi.Init.CLKPhase = me->clkPhase;

	me->hspi.Init.BaudRatePrescaler = AlxSpi_GetClkPrescaler(me);

	// Check clock
	ALX_SPI_ASSERT(AlxSpi_IsClkOk(me));

	if (wasInit)
	{
		AlxSpi_Init(me);
	}

	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		writeData
  * @param[in]		len Number of data words (can be different than sizeof(writeData))
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	(void)me;
	(void)writeData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);


	//------------------------------------------------------------------------------
	// HAL vs LL
	//------------------------------------------------------------------------------
	if (me->isWriteReadLowLevel == false)
	{
		//------------------------------------------------------------------------------
		// HAL
		//------------------------------------------------------------------------------

		// Local variables
		HAL_StatusTypeDef status = HAL_ERROR;

		// Try write for number of tries
		for (uint32_t _try = 1; _try <= numOfTries; _try++)
		{
			status = HAL_SPI_Transmit(&me->hspi, writeData, len, timeout_ms);
			if (status == HAL_OK)
			{
				break;	// Write OK
			}
			else
			{
				ALX_SPI_TRACE_WRN("Wrn");
				if(AlxSpi_Reset(me) != Alx_Ok)
				{
					ALX_SPI_TRACE_ERR("Err");
					return Alx_Err;
				};
				continue;
			}
		}

		// If we are here, write was OK or number of tries error occured
		if (status == HAL_OK)
		{
			return Alx_Ok;
		}
		else
		{
			ALX_SPI_TRACE_ERR("Err");
			return Alx_ErrNumOfTries;
		}
	}
	else
	{
		//------------------------------------------------------------------------------
		// LL
		//------------------------------------------------------------------------------

		// Local variables
		uint16_t lenToWrite = len;
		uint16_t lenToRead = len;
		uint16_t writeIndex = 0;
		uint32_t tickstart = HAL_GetTick();
		uint32_t timeout = (uint32_t)timeout_ms;

		// Execute write/read
		while(lenToRead > 0)
		{
			// If TXE = 1 (transmit buffer empty) and lenToWrite > 0 (we got some data to write)
			if
			(
				(LL_SPI_IsActiveFlag_TXE(me->hspi.Instance)) &&
				(lenToWrite > 0)
			)
			{
				// Write
				if (me->dataSize <= AlxSpi_DataSize_8bit)
				{
					LL_SPI_TransmitData8(me->hspi.Instance, writeData[writeIndex++]);
				}
				else
				{
					LL_SPI_TransmitData16(me->hspi.Instance, *(uint16_t*)&writeData[writeIndex]);
					writeIndex += 2;  // Advance by 2 because it points into byte buffer
				}

				// Decrement
				lenToWrite--;
			}

			// If RXNE = 1 (receive buffer not empty)
			if(LL_SPI_IsActiveFlag_RXNE(me->hspi.Instance))
			{
				// Read
				if (me->dataSize <= AlxSpi_DataSize_8bit)
				{
					LL_SPI_ReceiveData8(me->hspi.Instance);
				}
				else
				{
					LL_SPI_ReceiveData16(me->hspi.Instance);
				}

				// Decrement
				lenToRead--;
			}

			// Timeout management
			if ((lenToRead > 0) || (lenToWrite > 0))
			{
				if ((((HAL_GetTick() - tickstart) >= timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
				{
					return Alx_Err;
				}
			}
		}

		// Return
		return Alx_Ok;
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		readData
  * @param[in]		len
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	(void)me;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);


	//------------------------------------------------------------------------------
	// HAL vs LL
	//------------------------------------------------------------------------------
	if (me->isWriteReadLowLevel == false)
	{
		//------------------------------------------------------------------------------
		// HAL
		//------------------------------------------------------------------------------

		// Local variables
		HAL_StatusTypeDef status = HAL_ERROR;

		// Try read for number of tries
		for (uint32_t _try = 1; _try <= numOfTries; _try++)
		{
			status = HAL_SPI_Receive(&me->hspi, readData, len, timeout_ms);
			if (status == HAL_OK)
			{
				break;	// Read OK
			}
			else
			{
				ALX_SPI_TRACE_WRN("Wrn");
				if(AlxSpi_Reset(me) != Alx_Ok)
				{
					ALX_SPI_TRACE_ERR("Err");
					return Alx_Err;
				};
				continue;
			}
		}

		// If we are here, read was OK or number of tries error occured
		if (status == HAL_OK)
		{
			return Alx_Ok;
		}
		else
		{
			ALX_SPI_TRACE_ERR("Err");
			return Alx_ErrNumOfTries;
		}
	}
	else
	{
		//------------------------------------------------------------------------------
		// LL
		//------------------------------------------------------------------------------

		// Local variables
		uint16_t lenToWrite = len;
		uint16_t lenToRead = len;
		uint16_t readIndex = 0;
		uint32_t tickstart = HAL_GetTick();
		uint32_t timeout = (uint32_t)timeout_ms;

		// Execute write/read
		while(lenToRead > 0)
		{
			// If TXE = 1 (transmit buffer empty) and lenToWrite > 0 (we got some data to write)
			if
			(
				(LL_SPI_IsActiveFlag_TXE(me->hspi.Instance)) &&
				(lenToWrite > 0)
			)
			{
				// Write
				if (me->dataSize <= AlxSpi_DataSize_8bit)
				{
					LL_SPI_TransmitData8(me->hspi.Instance, 0x00);
				}
				else
				{
					LL_SPI_TransmitData16(me->hspi.Instance, 0x0000);
				}

				// Decrement
				lenToWrite--;
			}

			// If RXNE = 1 (receive buffer not empty)
			if(LL_SPI_IsActiveFlag_RXNE(me->hspi.Instance))
			{
				// Read
				if (me->dataSize <= AlxSpi_DataSize_8bit)
				{
					readData[readIndex++] = LL_SPI_ReceiveData8(me->hspi.Instance);
				}
				else
				{
					*(uint16_t*)&readData[readIndex] = LL_SPI_ReceiveData16(me->hspi.Instance);
					readIndex += 2;  // Advance by 2 because it points into byte buffer
				}

				// Decrement
				lenToRead--;
			}

			// Timeout management
			if ((lenToRead > 0) || (lenToWrite > 0))
			{
				if ((((HAL_GetTick() - tickstart) >= timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
				{
					return Alx_Err;
				}
			}
		}

		// Return
		return Alx_Ok;
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		writeData
  * @param[out]		readData
  * @param[in]		len
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	(void)me;
	(void)writeData;
	(void)readData;
	ALX_SPI_ASSERT(0 < len);
	ALX_SPI_ASSERT(0 < numOfTries);
	ALX_SPI_ASSERT(0 < timeout_ms);


	//------------------------------------------------------------------------------
	// HAL vs LL
	//------------------------------------------------------------------------------
	if (me->isWriteReadLowLevel == false)
	{
		//------------------------------------------------------------------------------
		// HAL
		//------------------------------------------------------------------------------

		// Local variables
		HAL_StatusTypeDef status = HAL_ERROR;

		// Try write/read for number of tries
		for (uint32_t _try = 1; _try <= numOfTries; _try++)
		{
			status = HAL_SPI_TransmitReceive(&me->hspi, writeData, readData, len, timeout_ms);
			if (status == HAL_OK)
			{
				break;	// Write/Read OK
			}
			else
			{
				ALX_SPI_TRACE_WRN("Wrn");
				if(AlxSpi_Reset(me) != Alx_Ok)
				{
					ALX_SPI_TRACE_ERR("Err");
					return Alx_Err;
				};
				continue;
			}
		}

		// If we are here, write/read was OK or number of tries error occured
		if (status == HAL_OK)
		{
			return Alx_Ok;
		}
		else
		{
			ALX_SPI_TRACE_ERR("Err");
			return Alx_ErrNumOfTries;
		}
	}
	else
	{
		//------------------------------------------------------------------------------
		// LL
		//------------------------------------------------------------------------------

		// Local variables
		uint16_t lenToWrite = len;
		uint16_t lenToRead = len;
		uint16_t writeIndex = 0;
		uint16_t readIndex = 0;
		uint32_t tickstart = HAL_GetTick();
		uint32_t timeout = (uint32_t)timeout_ms;

		// Execute write/read
		while(lenToRead > 0)
		{
			// If TXE = 1 (transmit buffer empty) and lenToWrite > 0 (we got some data to write)
			if
			(
				(LL_SPI_IsActiveFlag_TXE(me->hspi.Instance)) &&
				(lenToWrite > 0)
			)
			{
				// Write
				if (me->dataSize <= AlxSpi_DataSize_8bit)
				{
					LL_SPI_TransmitData8(me->hspi.Instance, writeData[writeIndex++]);
				}
				else
				{
					LL_SPI_TransmitData16(me->hspi.Instance, *(uint16_t*)&writeData[writeIndex]);
					writeIndex += 2;  // Advance by 2 because it points into byte buffer
				}

				// Decrement
				lenToWrite--;
			}

			// If RXNE = 1 (receive buffer not empty)
			if(LL_SPI_IsActiveFlag_RXNE(me->hspi.Instance))
			{
				// Read
				if (me->dataSize <= AlxSpi_DataSize_8bit)
				{
					readData[readIndex++] = LL_SPI_ReceiveData8(me->hspi.Instance);
				}
				else
				{
					*(uint16_t*)&readData[readIndex] = LL_SPI_ReceiveData16(me->hspi.Instance);
					readIndex += 2;  // Advance by 2 because it points into byte buffer
				}

				// Decrement
				lenToRead--;
			}

			// Timeout management
			if ((lenToRead > 0) || (lenToWrite > 0))
			{
				if ((((HAL_GetTick() - tickstart) >= timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
				{
					return Alx_Err;
				}
			}
		}

		// Return
		return Alx_Ok;
	}
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSpi_Master_AssertCs(AlxSpi* me)
{
	AlxIoPin_Reset(me->do_nCS);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSpi_Master_DeAssertCs(AlxSpi* me)
{
	AlxIoPin_Set(me->do_nCS);
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxSpi_ParseMode(AlxSpi* me)
{
	if (me->mode == AlxSpi_Mode_0) { me->clkPolarity = SPI_POLARITY_LOW; me->clkPhase = SPI_PHASE_1EDGE; return; }
	if (me->mode == AlxSpi_Mode_1) { me->clkPolarity = SPI_POLARITY_LOW; me->clkPhase = SPI_PHASE_2EDGE; return; }
	if (me->mode == AlxSpi_Mode_2) { me->clkPolarity = SPI_POLARITY_HIGH; me->clkPhase = SPI_PHASE_1EDGE; return; }
	if (me->mode == AlxSpi_Mode_3) { me->clkPolarity = SPI_POLARITY_HIGH; me->clkPhase = SPI_PHASE_2EDGE; return; }

	ALX_SPI_ASSERT(false);	// We should not get here
}

static uint32_t AlxSpi_GetDataSize(AlxSpi* me)
{
	switch (me->dataSize)
	{
//		case AlxSpi_DataSize_4bit: return SPI_DATASIZE_4BIT;
//		case AlxSpi_DataSize_5bit: return SPI_DATASIZE_5BIT;
//		case AlxSpi_DataSize_6bit: return SPI_DATASIZE_6BIT;
//		case AlxSpi_DataSize_7bit: return SPI_DATASIZE_7BIT;
		case AlxSpi_DataSize_8bit: return SPI_DATASIZE_8BIT;
//		case AlxSpi_DataSize_9bit: return SPI_DATASIZE_9BIT;
//		case AlxSpi_DataSize_10bit: return SPI_DATASIZE_10BIT;
//		case AlxSpi_DataSize_11bit: return SPI_DATASIZE_11BIT;
//		case AlxSpi_DataSize_12bit: return SPI_DATASIZE_12BIT;
//		case AlxSpi_DataSize_13bit: return SPI_DATASIZE_13BIT;
//		case AlxSpi_DataSize_14bit: return SPI_DATASIZE_14BIT;
//		case AlxSpi_DataSize_15bit: return SPI_DATASIZE_15BIT;
		case AlxSpi_DataSize_16bit: return SPI_DATASIZE_16BIT;
		default: ALX_SPI_ASSERT(false);
	}

	ALX_SPI_ASSERT(false);
	return 0;
}

static uint32_t AlxSpi_GetClkPrescaler(AlxSpi* me)
{
	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_350kHz_Pclk2Apb2_90MHz)		return SPI_BAUDRATEPRESCALER_256;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_700kHz_Pclk2Apb2_90MHz)		return SPI_BAUDRATEPRESCALER_128;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz)		return SPI_BAUDRATEPRESCALER_64;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz)	return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz)	return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_22MHz5_Pclk2Apb2_90MHz)		return SPI_BAUDRATEPRESCALER_4;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_45MHz_Pclk2Apb2_90MHz)		return SPI_BAUDRATEPRESCALER_2;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_1MHz4_Pclk1Apb1_45MHz)		return SPI_BAUDRATEPRESCALER_32;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_5MHz625_Pclk1Apb1_45MHz)	return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi2_Spi3_SpiClk_11MHz25_Pclk1Apb1_45MHz)	return SPI_BAUDRATEPRESCALER_4;
	#endif


	//------------------------------------------------------------------------------
	// STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F7)
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_422kHz_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_256;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_844kHz_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_128;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_1MHz688_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_64;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_3MHz375_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_32;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_6MHz75_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_13MHz5_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_27MHz_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_4;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_54MHz_Pclk2Apb2_108MHz)		return SPI_BAUDRATEPRESCALER_2;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_211kHz_Pclk1Apb1_54MHz)					return SPI_BAUDRATEPRESCALER_256;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_422kHz_Pclk1Apb1_54MHz)					return SPI_BAUDRATEPRESCALER_128;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_844kHz_Pclk1Apb1_54MHz)					return SPI_BAUDRATEPRESCALER_64;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_1MHz688_Pclk1Apb1_54MHz)				return SPI_BAUDRATEPRESCALER_32;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_3MHz375_Pclk1Apb1_54MHz)				return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_6MHz75_Pclk1Apb1_54MHz)					return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_13MHz5_Pclk1Apb1_54MHz)					return SPI_BAUDRATEPRESCALER_4;
	if(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_27MHz_Pclk1Apb1_54MHz)					return SPI_BAUDRATEPRESCALER_2;
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if(me->spiClk == AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz)	return SPI_BAUDRATEPRESCALER_128;
	if(me->spiClk == AlxSpi_Clk_McuStm32G4_Spi2_Spi3_SpiClk_1MHz33_Pclk1Apb1_170MHz)	return SPI_BAUDRATEPRESCALER_128;
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	if(me->spiClk == AlxSpi_Clk_McuStm32L0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz)			return SPI_BAUDRATEPRESCALER_32;
	if(me->spiClk == AlxSpi_Clk_McuStm32L0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz)			return SPI_BAUDRATEPRESCALER_32;
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_5MHz_Pclk2Apb2_80MHz)			return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_10MHz_Pclk2Apb2_80MHz)			return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_20MHz_Pclk2Apb2_80MHz)			return SPI_BAUDRATEPRESCALER_4;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_7MHz5_Pclk2Apb2_120MHz)			return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_15MHz_Pclk2Apb2_120MHz)			return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_30MHz_Pclk2Apb2_120MHz)			return SPI_BAUDRATEPRESCALER_4;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_5MHz_Pclk1Apb1_80MHz)		return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_10MHz_Pclk1Apb1_80MHz)		return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_20MHz_Pclk1Apb1_80MHz)		return SPI_BAUDRATEPRESCALER_4;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_1MHz875_Pclk1Apb1_120MHz)	return SPI_BAUDRATEPRESCALER_64;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_7MHz5_Pclk1Apb1_120MHz)		return SPI_BAUDRATEPRESCALER_16;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_15MHz_Pclk1Apb1_120MHz)		return SPI_BAUDRATEPRESCALER_8;
	if(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_30MHz_Pclk1Apb1_120MHz)		return SPI_BAUDRATEPRESCALER_4;
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(false);	// We should not get here
	return ALX_NULL;
}


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxSpi_Reset(AlxSpi* me)
{
	// DeInit SPI
	if (HAL_SPI_DeInit(&me->hspi) != HAL_OK)
	{
		ALX_SPI_TRACE_ERR("Err");
		return Alx_Err;
	}

	// Force SPI periphery reset
	AlxSpi_Periph_ForceReset(me);

	// Clear isInit
	me->isInit = false;

	// Release SPI periphery reset
	AlxSpi_Periph_ReleaseReset(me);

	// Init SPI
	if (HAL_SPI_Init(&me->hspi) != HAL_OK)
	{
		ALX_SPI_TRACE_ERR("Err");
		return Alx_Err;
	}

	// If write/read low level, set FIFO threshold & enable SPI
	if (me->isWriteReadLowLevel)
	{
		#if defined(ALX_STM32F7)
		LL_SPI_SetRxFIFOThreshold(me->hspi.Instance, LL_SPI_RX_FIFO_TH_QUARTER);
		#endif
		LL_SPI_Enable(me->hspi.Instance);
	}

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static bool AlxSpi_IsClkOk(AlxSpi* me)
{
	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
//	if((me->hspi.Instance == SPI1) || (me->hspi.Instance == SPI4))
	if(me->hspi.Instance == SPI1)
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_350kHz_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_700kHz_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_1MHz4_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_5MHz625_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_11MHz25_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_22MHz5_Pclk2Apb2_90MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F4_Spi1_Spi4_SpiClk_45MHz_Pclk2Apb2_90MHz)
		)
		{
			if(90000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
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
			if(45000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F7)
	if((me->hspi.Instance == SPI1) || (me->hspi.Instance == SPI4) || (me->hspi.Instance == SPI5) || (me->hspi.Instance == SPI6))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_422kHz_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_844kHz_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_1MHz688_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_3MHz375_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_6MHz75_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_13MHz5_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_27MHz_Pclk2Apb2_108MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi1_Spi4_Spi5_Spi6_SpiClk_54MHz_Pclk2Apb2_108MHz)
		)
		{
			if(108000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if((me->hspi.Instance == SPI2) || (me->hspi.Instance == SPI3))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_211kHz_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_422kHz_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_844kHz_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_1MHz688_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_3MHz375_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_6MHz75_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_13MHz5_Pclk1Apb1_54MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32F7_Spi2_Spi3_SpiClk_27MHz_Pclk1Apb1_54MHz)
		)
		{
			if(54000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if(me->hspi.Instance == SPI1)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32G4_Spi1_Spi4_SpiClk_1MHz33_Pclk2Apb2_170MHz)
		{
			if(170000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
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
			if(170000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
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
			if(170000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	if(me->hspi.Instance == SPI1)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32L0_Spi1_SpiClk_1MHz_Pclk2Apb2_32MHz)
		{
			if(32000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if(me->hspi.Instance == SPI2)
	{
		if(me->spiClk == AlxSpi_Clk_McuStm32L0_Spi2_SpiClk_1MHz_Pclk1Apb1_32MHz)
		{
			if(32000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if (me->hspi.Instance == SPI1)
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_5MHz_Pclk2Apb2_80MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_10MHz_Pclk2Apb2_80MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_20MHz_Pclk2Apb2_80MHz)
		)
		{
			if (80000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
		else if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_7MHz5_Pclk2Apb2_120MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_15MHz_Pclk2Apb2_120MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi1_SpiClk_30MHz_Pclk2Apb2_120MHz)
		)
		{
			if (120000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	if ((me->hspi.Instance == SPI2) || (me->hspi.Instance == SPI3))
	{
		if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_5MHz_Pclk1Apb1_80MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_10MHz_Pclk1Apb1_80MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_20MHz_Pclk1Apb1_80MHz)
		)
		{
			if (80000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
		else if
		(
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_1MHz875_Pclk1Apb1_120MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_7MHz5_Pclk1Apb1_120MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_15MHz_Pclk1Apb1_120MHz) ||
			(me->spiClk == AlxSpi_Clk_McuStm32L4_Spi2_Spi3_SpiClk_30MHz_Pclk1Apb1_120MHz)
		)
		{
			if (120000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static void AlxSpi_Periph_EnableClk(AlxSpi* me)
{
	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_CLK_ENABLE(); return; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_CLK_ENABLE(); return; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_CLK_ENABLE(); return; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_CLK_ENABLE(); return; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_CLK_ENABLE(); return; }
	#endif
	#if defined(SPI6)
	if (me->hspi.Instance == SPI6)	{ __HAL_RCC_SPI6_CLK_ENABLE(); return; }
	#endif

	ALX_SPI_ASSERT(false);	// We should not get here
}
static void AlxSpi_Periph_DisableClk(AlxSpi* me)
{
	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_CLK_DISABLE(); return; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_CLK_DISABLE(); return; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_CLK_DISABLE(); return; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_CLK_DISABLE(); return; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_CLK_DISABLE(); return; }
	#endif
	#if defined(SPI6)
	if (me->hspi.Instance == SPI6)	{ __HAL_RCC_SPI6_CLK_DISABLE(); return; }
	#endif

	ALX_SPI_ASSERT(false);	// We should not get here
}
static void AlxSpi_Periph_ForceReset(AlxSpi* me)
{
	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_FORCE_RESET(); return; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_FORCE_RESET(); return; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_FORCE_RESET(); return; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_FORCE_RESET(); return; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_FORCE_RESET(); return; }
	#endif
	#if defined(SPI6)
	if (me->hspi.Instance == SPI6)	{ __HAL_RCC_SPI6_FORCE_RESET(); return; }
	#endif

	ALX_SPI_ASSERT(false);	// We should not get here
}
static void AlxSpi_Periph_ReleaseReset(AlxSpi* me)
{
	#if defined(SPI1)
	if (me->hspi.Instance == SPI1)	{ __HAL_RCC_SPI1_RELEASE_RESET(); return; }
	#endif
	#if defined(SPI2)
	if (me->hspi.Instance == SPI2)	{ __HAL_RCC_SPI2_RELEASE_RESET(); return; }
	#endif
	#if defined(SPI3)
	if (me->hspi.Instance == SPI3)	{ __HAL_RCC_SPI3_RELEASE_RESET(); return; }
	#endif
	#if defined(SPI4)
	if (me->hspi.Instance == SPI4)	{ __HAL_RCC_SPI4_RELEASE_RESET(); return; }
	#endif
	#if defined(SPI5)
	if (me->hspi.Instance == SPI5)	{ __HAL_RCC_SPI5_RELEASE_RESET(); return; }
	#endif
	#if defined(SPI6)
	if (me->hspi.Instance == SPI6)	{ __HAL_RCC_SPI6_RELEASE_RESET(); return; }
	#endif

	ALX_SPI_ASSERT(false);	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))
