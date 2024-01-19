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
Alx_Status AlxSd_SpiMode_ReadNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data);
Alx_Status AlxSd_SpiMode_WriteNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data);
Alx_Status AlxSd_SpiMode_SendPwrUpClkCycles(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd(AlxSd* me, uint8_t* cmd, uint8_t cmdLen, uint8_t expectedRespR1Mask, uint8_t expectedRespR1, uint8_t* actualRespR1, bool enableRespR1b, bool enableRespR2R3R7, uint8_t* expectedRespR2R3R7, uint8_t expectedRespR2R3R7Len);
Alx_Status AlxSd_SpiMode_SendCmd0(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd8(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd9(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd10(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd12(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd13(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd18(AlxSd* me, uint32_t addr);
Alx_Status AlxSd_SpiMode_SendCmd25(AlxSd* me, uint32_t addr);
Alx_Status AlxSd_SpiMode_SendCmd58_SdIdle(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendCmd58_SdReady(AlxSd* me);
Alx_Status AlxSd_SpiMode_SendAcmd23(AlxSd* me, uint32_t numOfBlocksToPreErase);
Alx_Status AlxSd_SpiMode_SendAcmd41(AlxSd* me);
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
	uint16_t cmdRespR1Timeout_ms,
	uint16_t acmd41Timeout_ms,
	uint16_t blockReadStartTokenTimeout_ms,
	uint16_t blockWriteStartTokenTimeout_ms,
	uint16_t blockWriteStopTokenTimeout_ms
)
{
	// Const
	memset(me->DATA_0xFF, 0xFF, ALX_SD_DATA_0xFF_LEN);
	me->NUM_OF_PWR_UP_CLK_CYCLES = 20;

	// Parameters
	me->alxSpi = alxSpi;
	me->spiNumOfTries = spiNumOfTries;
	me->spiTimeout_ms = spiTimeout_ms;
	me->cmdRespR1Timeout_ms = cmdRespR1Timeout_ms;
	me->acmd41Timeout_ms = acmd41Timeout_ms;
	me->blockReadStartTokenTimeout_ms = blockReadStartTokenTimeout_ms;
	me->blockWriteStartTokenTimeout_ms = blockWriteStartTokenTimeout_ms;
	me->blockWriteStopTokenTimeout_ms = blockWriteStopTokenTimeout_ms;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxSd_Init(AlxSd* me, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
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
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Send power-up clock cycles
		status = AlxSd_SpiMode_SendPwrUpClkCycles(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD0 SPI mode - Sends CMD0 with CS asserted, which resets SD card and puts it in SPI mode
		status = AlxSd_SpiMode_SendCmd0(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD8 SPI mode - Sends interface operating conditions and checks if SD card is Version 2.00 or later (currently only 3.3V operation and Version 2.00 or later is supported)
		status = AlxSd_SpiMode_SendCmd8(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD58 SPI mode (SD card idle) - Reads OCR (operating conditions register), check interface operating conditions
		status = AlxSd_SpiMode_SendCmd58_SdIdle(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute ACMD41 SPI mode - Starts SD card initialization process, polls untils initialization is complete, also sends HSC (high capacits support) bit
		status = AlxSd_SpiMode_SendAcmd41(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD58 SPI mode (SD card ready) - Reads OCR (operating conditions register), check interface operating conditions, checks CCD (card capacity status), currently only supported CSS = 1
		status = AlxSd_SpiMode_SendCmd58_SdReady(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD9 SPI mode - Checks CSD (card-specific register) values
		status = AlxSd_SpiMode_SendCmd9(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD10 SPI mode - Checks CID (card identification register) values
		status = AlxSd_SpiMode_SendCmd10(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

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
Alx_Status AlxSd_DeInit(AlxSd* me, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
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
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Send power-up clock cycles
		status = AlxSd_SpiMode_SendPwrUpClkCycles(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD0 SPI mode - Sends CMD0 with CS asserted, which resets SD card and puts it in SPI mode
		status = AlxSd_SpiMode_SendCmd0(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

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
Alx_Status AlxSd_ReadBlock(AlxSd* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Assert
	ALX_SD_ASSERT(me->wasCtorCalled == true);
	ALX_SD_ASSERT(me->isInit == true);
	ALX_SD_ASSERT((numOfBlocks * ALX_SD_BLOCK_LEN) == len);

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Execute CMD18 SPI mode - Sends multiple block read command with specified address at which reading will begin
		status = AlxSd_SpiMode_SendCmd18(me, addr);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Read number of blocks from specified address onward
		status = AlxSd_SpiMode_ReadNumOfBlocks(me, numOfBlocks, data);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD12 SPI mode - Sends stop transmission command
		status = AlxSd_SpiMode_SendCmd12(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_ErrNumOfTries;
}
Alx_Status AlxSd_WriteBlock(AlxSd* me, uint32_t numOfBlocks, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t newTryWaitTime_ms)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Assert
	ALX_SD_ASSERT(me->wasCtorCalled == true);
	ALX_SD_ASSERT(me->isInit == true);
	ALX_SD_ASSERT((numOfBlocks * ALX_SD_BLOCK_LEN) == len);

	// Local variables
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------

	// Try for number of tries
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Execute ACMD23 SPI mode - Sends number of block to pre-erase before writing
		status = AlxSd_SpiMode_SendAcmd23(me, numOfBlocks);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD25 SPI mode - Sends multiple block write command with specified address at which writing will begin
		status = AlxSd_SpiMode_SendCmd25(me, addr);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Write number of blocks from specified address onward
		status = AlxSd_SpiMode_WriteNumOfBlocks(me, numOfBlocks, data);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

		// Execute CMD13 SPI mode - Check if errors occured during multiple block write
		status = AlxSd_SpiMode_SendCmd13(me);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); AlxDelay_ms(newTryWaitTime_ms); continue; }

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
			// Wait for start multiple block read token
			uint8_t rxByte = 0;
			status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0xFE, true, me->blockReadStartTokenTimeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Read block
			uint8_t rxData[ALX_SD_BLOCK_LEN+2] = {};	// Block length + CRC 2 bytes
			status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, rxData, sizeof(rxData), me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Copy
			memcpy(data + offset, rxData, ALX_SD_BLOCK_LEN);

			// Increment offset
			offset = offset + ALX_SD_BLOCK_LEN;
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
Alx_Status AlxSd_SpiMode_WriteNumOfBlocks(AlxSd* me, uint32_t numOfBlocks, uint8_t* data)
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
			// Write start multiple block write token
			uint8_t startMultipleBlockWriteToken = 0xFC;
			status = AlxSpi_Master_Write(me->alxSpi, &startMultipleBlockWriteToken, 1, me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Write block
			status = AlxSpi_Master_Write(me->alxSpi, data + offset, ALX_SD_BLOCK_LEN, me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Wait for multiple block write data accepted token
			uint8_t rxByte = 0;
			status = AlxSd_SpiMode_WaitByte(me, 0x1F, 0x05, true, me->blockWriteStartTokenTimeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Wait for SD card busy
			status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, false, me->blockWriteStopTokenTimeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Increment offset
			offset = offset + ALX_SD_BLOCK_LEN;
		}

		// If status NOT OK
		if (status != Alx_Ok) { break; }

		// Write stop multiple block write token
		uint8_t stopMultipleBlockWriteToken = 0xFD;
		status = AlxSpi_Master_Write(me->alxSpi, &stopMultipleBlockWriteToken, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Wait for SD card busy
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, false, me->blockWriteStopTokenTimeout_ms, &rxByte);
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
Alx_Status AlxSd_SpiMode_SendPwrUpClkCycles(AlxSd* me)
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
Alx_Status AlxSd_SpiMode_SendCmd(AlxSd* me, uint8_t* cmd, uint8_t cmdLen, uint8_t expectedRespR1Mask, uint8_t expectedRespR1, uint8_t* actualRespR1, bool enableRespR1b, bool enableRespR2R3R7, uint8_t* expectedRespR2R3R7, uint8_t expectedRespR2R3R7Len)
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
		status = AlxSpi_Master_Write(me->alxSpi, cmd, cmdLen, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Send sync byte
		status = AlxSpi_Master_Write(me->alxSpi, me->DATA_0xFF, 1, me->spiNumOfTries, me->spiTimeout_ms);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Read response R1
		uint8_t rxByte = 0;
		status = AlxSd_SpiMode_WaitByte(me, expectedRespR1Mask, expectedRespR1, true, me->cmdRespR1Timeout_ms, actualRespR1);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// If response R1b enabled, wait until SD card busy
		if (enableRespR1b)
		{
			status = AlxSd_SpiMode_WaitByte(me, 0xFF, 0x00, false, me->cmdRespR1Timeout_ms, &rxByte);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }
		}

		// If response R2/R3/R7 enabled, read & check response
		if (enableRespR2R3R7)
		{
			// Read
			uint8_t actualRespR2R3R7[4] = {};
			status = AlxSpi_Master_WriteRead(me->alxSpi, me->DATA_0xFF, actualRespR2R3R7, expectedRespR2R3R7Len, me->spiNumOfTries, me->spiTimeout_ms);
			if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

			// Check
			if (expectedRespR2R3R7Len == 1)	// TV: TODO, check for R3/R7
			{
				if (memcmp(actualRespR2R3R7, expectedRespR2R3R7, expectedRespR2R3R7Len) != 0) { ALX_SD_TRACE("Err"); break; }
			}
		}

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
Alx_Status AlxSd_SpiMode_SendCmd0(AlxSd* me)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 0);	// 0x40
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x95;	// CRC, shifted 1 bit left, bit0 = 1

	// Send
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x01, &actualRespR1, false, false, ALX_NULL_PTR, ALX_NULL);

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd8(AlxSd* me)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 8);	// 0x48
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x01;	// VHS = 0b0001, voltage: 2.7V..3.6V
	cmd[4] = 0xAA;	// Check Pattern = 0xAA
	cmd[5] = 0x87;	// CRC, shifted 1 bit left, bit0 = 1

	uint8_t expectedRespR7[4] = {};	// TV: TODO, check response
	expectedRespR7[0] = 0x00;
	expectedRespR7[1] = 0x00;
	expectedRespR7[2] = 0x00;
	expectedRespR7[3] = 0x00;

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x01, &actualRespR1, false, true, expectedRespR7, sizeof(expectedRespR7));

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd9(AlxSd* me)
{
	// Local variables
	(void)me;

	// Return
	return Alx_Ok;
}
Alx_Status AlxSd_SpiMode_SendCmd10(AlxSd* me)
{
	// Local variables
	(void)me;

	// Return
	return Alx_Ok;
}
Alx_Status AlxSd_SpiMode_SendCmd12(AlxSd* me)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 12);	// 0x4C
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x01;	// CRC not needed

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x00, &actualRespR1, true, false, ALX_NULL_PTR, ALX_NULL);

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd13(AlxSd* me)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 13);	// 0x4D
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x01;	// CRC not needed

	uint8_t expectedRespR2[1] = {};	// TV: TODO, check not implemented
	expectedRespR2[0] = 0x00;

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x00, &actualRespR1, false, true, expectedRespR2, sizeof(expectedRespR2));

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd18(AlxSd* me, uint32_t addr)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 18);	// 0x52
	cmd[1] = addr >> 24;
	cmd[2] = addr >> 16;
	cmd[3] = addr >> 8;
	cmd[4] = addr;
	cmd[5] = 0x01;	// CRC not needed

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x00, &actualRespR1, false, false, ALX_NULL_PTR, ALX_NULL);

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd25(AlxSd* me, uint32_t addr)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 25);	// 0x59
	cmd[1] = addr >> 24;
	cmd[2] = addr >> 16;
	cmd[3] = addr >> 8;
	cmd[4] = addr;
	cmd[5] = 0x01;	// CRC not needed

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x00, &actualRespR1, true, false, ALX_NULL_PTR, ALX_NULL);

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd58_SdIdle(AlxSd* me)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 58);	// 0x7A
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x01;	// CRC not needed

	uint8_t expectedRespR3[4] = {};	// TV: TODO, check response
	expectedRespR3[0] = 0x00;
	expectedRespR3[1] = 0x00;
	expectedRespR3[2] = 0x00;
	expectedRespR3[3] = 0x00;

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x01, &actualRespR1, false, true, expectedRespR3, sizeof(expectedRespR3));

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendCmd58_SdReady(AlxSd* me)
{
	// Prepare
	uint8_t cmd[6] = {};
	cmd[0] = (0x40 | 58);	// 0x7A
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x01;	// CRC not needed

	uint8_t expectedRespR3[4] = {};	// TV: TODO, check response
	expectedRespR3[0] = 0x00;
	expectedRespR3[1] = 0x00;
	expectedRespR3[2] = 0x00;
	expectedRespR3[3] = 0x00;

	// Return
	uint8_t actualRespR1 = 0;
	Alx_Status status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFF, 0x00, &actualRespR1, false, true, expectedRespR3, sizeof(expectedRespR3));

	// Return
	return status;
}
Alx_Status AlxSd_SpiMode_SendAcmd23(AlxSd* me, uint32_t numOfBlocksToPreErase)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t cmd[6] = {};
	uint8_t actualRespR1 = 0;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		//------------------------------------------------------------------------------
		// Send CMD55
		//------------------------------------------------------------------------------

		// Prepare
		cmd[0] = (0x40 | 55);	// 0x77
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed

		// Send
		actualRespR1 = 0;
		status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFE, 0x00, &actualRespR1, false, false, ALX_NULL_PTR, ALX_NULL);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }


		//------------------------------------------------------------------------------
		// Send ACMD23
		//------------------------------------------------------------------------------

		// Prepare
		cmd[0] = (0x40 | 23);	// 0x57
		cmd[1] = numOfBlocksToPreErase >> 24;
		cmd[2] = numOfBlocksToPreErase >> 16;
		cmd[3] = numOfBlocksToPreErase >> 8;
		cmd[4] = numOfBlocksToPreErase;
		cmd[5] = 0x01;	// CRC not needed

		// Send
		actualRespR1 = 0;
		status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFE, 0x00, &actualRespR1, false, false, ALX_NULL_PTR, ALX_NULL);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Return
	return Alx_Err;
}
Alx_Status AlxSd_SpiMode_SendAcmd41(AlxSd* me)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t cmd[6] = {};
	uint8_t actualRespR1 = 0;
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);

	// Start timer
	AlxTimSw_Start(&alxTimSw);


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (1)
	{
		//------------------------------------------------------------------------------
		// Send CMD55
		//------------------------------------------------------------------------------

		// Prepare
		cmd[0] = (0x40 | 55);	// 0x77
		cmd[1] = 0x00;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed

		// Send
		actualRespR1 = 0;
		status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFE, 0x00, &actualRespR1, false, false, ALX_NULL_PTR, ALX_NULL);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }


		//------------------------------------------------------------------------------
		// Send ACMD41
		//------------------------------------------------------------------------------

		// Prepare
		cmd[0] = (0x40 | 41);	// 0x69
		cmd[1] = 0x40;
		cmd[2] = 0x00;
		cmd[3] = 0x00;
		cmd[4] = 0x00;
		cmd[5] = 0x01;	// CRC not needed

		// Send
		actualRespR1 = 0;
		status = AlxSd_SpiMode_SendCmd(me, cmd, sizeof(cmd), 0xFE, 0x00, &actualRespR1, false, false, ALX_NULL_PTR, ALX_NULL);
		if (status != Alx_Ok) { ALX_SD_TRACE("Err"); break; }


		//------------------------------------------------------------------------------
		// Check
		//------------------------------------------------------------------------------

		// Check if SD card ready
		if (actualRespR1 == 0x00)
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
	AlxTimSw_Ctor(&alxTimSw, false);

	// Start timer
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
