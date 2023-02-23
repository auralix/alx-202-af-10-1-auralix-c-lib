/**
  ******************************************************************************
  * @file		alxSd.c
  * @brief		Auralix C Library - ALX SD Card Module
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
#include "alxSd.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************

// Init
Alx_Status AlxSd_SpiMode_GeneratePwrUpClkCycles(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteCmd0(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteCmd8(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteCmd58_SdIdle(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteAcmd41(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteCmd58_SdReady(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteCmd9(AlxSd* me);
Alx_Status AlxSd_SpiMode_ExecuteCmd10(AlxSd* me);

// Read Block
Alx_Status AlxSd_SpiMode_ExecuteCmd18(AlxSd* me, uint32_t addr);
Alx_Status AlxSd_SpiMode_ReadNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data);
Alx_Status AlxSd_SpiMode_ExecuteCmd12(AlxSd* me);

// Write Block
Alx_Status AlxSd_SpiMode_ExecuteAcmd23(AlxSd* me, uint32_t numOfBlocksToPreErase);
Alx_Status AlxSd_SpiMode_ExecuteCmd25(AlxSd* me, uint32_t addr);
Alx_Status AlxSd_SpiMode_WriteNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data, uint32_t len);
Alx_Status AlxSd_SpiMode_ExecuteCmd13(AlxSd* me);

// Private
Alx_Status AlxSd_SpiMode_WaitByte(AlxSd* me, uint8_t rxByteMask, uint8_t rxByteToWait, bool isEqual, uint16_t timeout_ms, uint8_t* rxByte);
Alx_Status AlxSd_SpiMode_AssertCs_SendSyncByte(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendSyncByte_DeAssertCs(AlxSd* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSd_Ctor
(
	AlxSd* me,
	AlxSpi* alxSpi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	uint8_t initDeInitNumOfTries,
	uint16_t initDeInitInitialAndNewTryWaitTime_ms,
	uint16_t cmdRespTimeout_ms,
	uint16_t acmd41Timeout_ms,
	uint16_t blockReadTokenFeTimeout_ms,
	uint16_t blockWriteDataAcceptedTokenTimeout_ms,
	uint16_t blockWriteSdBusyTimeout_ms
)
{
	// Const
	memset(me->DATA_0xFF, 0xFF, DATA_0xFF_LEN);
	me->NUM_OF_PWR_UP_CLK_CYCLES = 20;

	// Parameters
	me->alxSpi = alxSpi;
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;
	me->initDeInitNumOfTries = initDeInitNumOfTries;
	me->initDeInitInitialAndNewTryWaitTime_ms = initDeInitInitialAndNewTryWaitTime_ms;
	me->cmdRespTimeout_ms = cmdRespTimeout_ms;
	me->acmd41Timeout_ms = acmd41Timeout_ms;
	me->blockReadTokenFeTimeout_ms = blockReadTokenFeTimeout_ms;
	me->blockWriteDataAcceptedTokenTimeout_ms = blockWriteDataAcceptedTokenTimeout_ms;
	me->blockWriteSdBusyTimeout_ms = blockWriteSdBusyTimeout_ms;

	// Variables
	me->var = 0;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSd_Init(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Assert
	ALX_SD_ASSERT(me->wasCtorCalled == true);
	ALX_SD_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init SPI
	status = AlxSpi_Init(me->alxSpi);
	if (status != Alx_Ok) { ALX_SD_TRACE("Err"); return Alx_Err; }


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint8_t _try = 1; _try <= me->initDeInitNumOfTries; _try++)
	{
		// Initial try wait
		AlxDelay_ms(me->initDeInitInitialAndNewTryWaitTime_ms);

		// Send power-up clock cycles
		status = AlxSd_SpiMode_GeneratePwrUpClkCycles(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD0 SPI mode - Sends CMD0 with CS asserted, which resets SD card and puts it in SPI mode
		status = AlxSd_SpiMode_ExecuteCmd0(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD8 SPI mode - Sends interface operating conditions and checks if SD card is Version 2.00 or later (currently only 3.3V operation and Version 2.00 or later is supported)
		status = AlxSd_SpiMode_ExecuteCmd8(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD58 SPI mode (SD card idle) - Reads OCR (operating conditions register), check interface operating conditions
		status = AlxSd_SpiMode_ExecuteCmd58_SdIdle(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute ACMD41 SPI mode - Starts SD card initialization process, polls untils initialization is complete, also sends HSC (high capacits support) bit
		status = AlxSd_SpiMode_ExecuteAcmd41(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD58 SPI mode (SD card ready) - Reads OCR (operating conditions register), check interface operating conditions, checks CCD (card capacity status), currently only supported CSS = 1
		status = AlxSd_SpiMode_ExecuteCmd58_SdReady(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD9 SPI mode - Checks CSD (card-specific register) values
		status = AlxSd_SpiMode_ExecuteCmd9(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD10 SPI mode - Checks CID (card identification register) values
		status = AlxSd_SpiMode_ExecuteCmd10(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Set isInit
		me->isInit = true;

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeInit SPI
	status = AlxSpi_DeInit(me->alxSpi);
	if (status != Alx_Ok) { ALX_SD_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_ErrNumOfTries;
}
Alx_Status AlxSd_DeInit(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Assert
	ALX_SD_ASSERT(me->wasCtorCalled == true);
	ALX_SD_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint8_t _try = 1; _try <= me->initDeInitNumOfTries; _try++)
	{
		// Initial try wait
		AlxDelay_ms(me->initDeInitInitialAndNewTryWaitTime_ms);

		// Send power-up clock cycles
		status = AlxSd_SpiMode_GeneratePwrUpClkCycles(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD0 SPI mode - Sends CMD0 with CS asserted, which resets SD card and puts it in SPI mode
		status = AlxSd_SpiMode_ExecuteCmd0(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// DeInit SPI
		status = AlxSpi_DeInit(me->alxSpi);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); return Alx_Err; }

		// Clear isInit
		me->isInit = false;

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_ErrNumOfTries;
}
Alx_Status AlxSd_ReadBlock(AlxSd* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Assert
	ALX_SD_ASSERT(me->wasCtorCalled == true);
	ALX_SD_ASSERT(me->isInit == true);
	ALX_SD_ASSERT((numOfBlocks * BLOCK_LEN) == len);

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Execute CMD18 SPI mode - Sends multiple block read command with specified address at which reading will begin
		status = AlxSd_SpiMode_ExecuteCmd18(me, addr);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Read number of blocks from specified address onward
		status = AlxSd_SpiMode_ReadNumOfBlocks(me, numOfBlocks, data);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD12 SPI mode - Sends stop transmission command
		status = AlxSd_SpiMode_ExecuteCmd12(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_ErrNumOfTries;
}
Alx_Status AlxSd_WriteBlock(AlxSd* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Assert
	ALX_SD_ASSERT(me->wasCtorCalled == true);
	ALX_SD_ASSERT(me->isInit == true);
	ALX_SD_ASSERT((numOfBlocks * BLOCK_LEN) == len);

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Execute ACMD23 SPI mode - Sends number of block to pre-erase before writing
		status = AlxSd_SpiMode_ExecuteAcmd23(me, numOfBlocks);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD25 SPI mode - Sends multiple block write command with specified address at which writing will begin
		status = AlxSd_SpiMode_ExecuteCmd25(me, addr);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Write number of blocks from specified address onward
		status = AlxSd_SpiMode_WriteNumOfBlocks(me, numOfBlocks, data, len);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Execute CMD13 SPI mode - Check if errors occured during multiple block write
		status = AlxSd_SpiMode_ExecuteCmd13(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); continue; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_ErrNumOfTries;
}


//******************************************************************************
// Private Functions
//******************************************************************************
Alx_Status AlxSd_SpiMode_GeneratePwrUpClkCycles(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Generate power-up clock cycles
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, me->NUM_OF_PWR_UP_CLK_CYCLES, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd0(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 0);	// 0x40
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x95;	// CRC, shifted 1 bit left, bit0 = 1
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card idle
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x01, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd8(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 8);	// 0x48
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x01;	// VHS = 0b0001, voltage: 2.7V..3.6V
		cmd[4] = 0xAA;	// Check Pattern = 0xAA
		cmd[5] = 0x87;	// CRC, shifted 1 bit left, bit0 = 1
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card idle
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x01, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R7 (rest of bytes)
		uint8_t respR7[4] = {};
		status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, respR7, sizeof(respR7), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Check response R7 (rest of bytes)
		// TODO

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd58_SdIdle(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 58);	// 0x7A
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card idle
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x01, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R3 (rest of bytes)
		uint8_t respR3[4] = {};
		status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, respR3, sizeof(respR3), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Check response R3 (rest of bytes)
		// TODO

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteAcmd41(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t cmd[6] = {};
	uint8_t rxByte = 0;
	AlxTimSw alxTimSw;

	// Start timer
	AlxTimSw_Ctor(&alxTimSw, false);
	AlxTimSw_Start(&alxTimSw);


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		//------------------------------------------------------------------------------
		// Send CMD55
		//------------------------------------------------------------------------------

		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		cmd[0] = (0x40 | 55);	// 0x77
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card idle or SD card ready
		status = AlxSd_SpiMode_WaitByte(me, 0xFE, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }


		//------------------------------------------------------------------------------
		// Send ACMD41
		//------------------------------------------------------------------------------

		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		cmd[0] = (0x40 | 41);	// 0x69
		cmd[1] = 0x40;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card idle or SD card ready
		status = AlxSd_SpiMode_WaitByte(me, 0xFE, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Check if SD card ready
		if (rxByte == 0x00)
		{
			// Return
			return Alx_Ok;
		}

		// Check if timeout
		if (AlxTimSw_IsTimeout_ms(&alxTimSw, me->acmd41Timeout_ms)) { ALX_SD_TRACE("Err"); break; }
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd58_SdReady(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 58);	// 0x7A
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R3 (rest of bytes)
		uint8_t respR3[4] = {};
		status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, respR3, sizeof(respR3), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Check response R3 (rest of bytes)
		// TODO

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd9(AlxSd* me)
{
	// Local variables
	(void)me;

	// Return
	return Alx_Ok;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd10(AlxSd* me)
{
	// Local variables
	(void)me;

	// Return
	return Alx_Ok;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd18(AlxSd* me, uint32_t addr)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 18);	// 0x52
		cmd[1] = addr >> 24;
		cmd[2] = addr >> 16;
		cmd[3] = addr >> 8;
		cmd[4] = addr;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ReadNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint32_t offset = 0;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read number of blocks
		for (uint32_t i = 0; i < numOfBlocks; i++)
		{
			// Wait token 0xFE
			uint8_t rxByte = 0;
			status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0xFE, true, me->blockReadTokenFeTimeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Read block
			uint8_t rxData[BLOCK_LEN+2] = {};	// Block length + CRC 2 bytes
			status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, rxData, sizeof(rxData), me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Copy
			memcpy(data + offset, rxData, BLOCK_LEN);

			// Increment offset
			offset = offset + BLOCK_LEN;
		}

		// If status NOT OK
		if (status != Alx_Ok) { break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd12(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t rxByte = 0;

	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 12);	// 0x4C
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1b, until SD card busy
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, false, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteAcmd23(AlxSd* me, uint32_t numOfBlocksToPreErase)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t cmd[6] = {};
	uint8_t rxByte = 0;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		//------------------------------------------------------------------------------
		// Send CMD55
		//------------------------------------------------------------------------------

		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		cmd[0] = (0x40 | 55);	// 0x77
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }


		//------------------------------------------------------------------------------
		// Send ACMD23
		//------------------------------------------------------------------------------

		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		cmd[0] = (0x40 | 23);	// 0x57
		cmd[1] = numOfBlocksToPreErase >> 24;
		cmd[2] = numOfBlocksToPreErase >> 16;
		cmd[3] = numOfBlocksToPreErase >> 8;
		cmd[4] = numOfBlocksToPreErase;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd25(AlxSd* me, uint32_t addr)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 25);	// 0x59
		cmd[1] = addr >> 24;
		cmd[2] = addr >> 16;
		cmd[3] = addr >> 8;
		cmd[4] = addr;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_WriteNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint32_t offset = 0;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read number of blocks
		for (uint32_t i = 0; i < numOfBlocks; i++)
		{
			// Write start token
			uint8_t startToken = 0xFC;
			status = AlxSpi_Master_Write(me->alxSpi, &startToken, 1, me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Write block
			status = AlxSpi_Master_Write(me->alxSpi, data + offset, BLOCK_LEN, me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Wait for data accepted token
			uint8_t rxByte = 0;
			status = AlxSd_SpiMode_WaitByte(me, 0x1F, 0x05, true, me->blockWriteDataAcceptedTokenTimeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Wait for SD card busy
			status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, false, me->blockWriteSdBusyTimeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Increment offset
			offset = offset + BLOCK_LEN;
		}

		// If status NOT OK
		if (status != Alx_Ok) { break; }

		// Write stop token
		uint8_t startToken = 0xFD;
		status = AlxSpi_Master_Write(me->alxSpi, &startToken, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Wait for SD card busy
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, false, me->blockWriteSdBusyTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_ExecuteCmd13(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Assert CS + Send sync byte
		status = AlxSd_SpiMode_AssertCs_SendSyncByte(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send CMD
		uint8_t cmd[6] = {};
		cmd[0] = (0x40 | 13);	// 0x4D
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed
		status = AlxSpi_Master_Write(me->alxSpi, cmd, sizeof(cmd), me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1, expect SD card ready
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, true, me->cmdRespTimeout_ms, &rxByte);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R2 (rest of bytes)
		uint8_t respR2 = 0;
		status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, &respR2, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Check response R2 (rest of bytes)
		if (respR2 != 0x00) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte + DeAssert CS
		status = AlxSd_SpiMode_SendSyncByte_DeAssertCs(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_WaitByte(AlxSd* me, uint8_t rxByteMask, uint8_t rxByteToWait, bool isEqual, uint16_t timeout_ms, uint8_t* rxByte)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	AlxTimSw alxTimSw;

	// Start timer
	AlxTimSw_Ctor(&alxTimSw, false);
	AlxTimSw_Start(&alxTimSw);


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		// Read
		status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, rxByte, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Check if byteToWait
		uint8_t rxByteMasked = *rxByte & rxByteMask;
		if (isEqual)
		{
			if (rxByteMasked == rxByteToWait)
			{
				// Return
				return Alx_Ok;
			}
		}
		else	// Not equal
		{
			if (rxByteMasked != rxByteToWait)
			{
				// Return
				return Alx_Ok;
			}
		}

		// Check if timeout
		if (AlxTimSw_IsTimeout_ms(&alxTimSw, timeout_ms)) { ALX_SD_TRACE("Err"); break; }
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_AssertCs_SendSyncByte(AlxSd* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Assert CS
	AlxSpi_Master_AssertCs(me->alxSpi);

	// Send sync byte
	status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { ALX_SD_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxSd_SpiMode_SendSyncByte_DeAssertCs(AlxSd* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Send sync byte
	status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
	if (status != Alx_Ok) { ALX_SD_TRACE("Err"); return Alx_Err; }

	// DeAssert CS
	AlxSpi_Master_DeAssertCs(me->alxSpi);

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
