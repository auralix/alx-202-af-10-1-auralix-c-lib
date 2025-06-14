/**
  ******************************************************************************
  * @file		alxSpi_McuZephyr.c
  * @brief		Auralix C Library - ALX SPI MCU Zephyr Module
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
#include "alxSpi_McuZephyr.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		deviceName
  * @param[in]		do_nCS
  * @param[in]		frequency
  * @param[in]		operation
  */
void AlxSpi_Ctor
(
	AlxSpi* me,
	const char* deviceName,
	AlxIoPin* do_nCS,
	uint32_t frequency,
	spi_operation_t operation
)
{
	// Parameters
	me->deviceName = deviceName;
	me->do_nCS = do_nCS;
	me->config.frequency = frequency;
	me->config.operation = operation;
	me->config.slave = 0;
	memset(&me->config.cs, 0, sizeof(me->config.cs));

	// Variables
	me->device = NULL;

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
	// Trace
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s", me->deviceName);

	// Assert
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == false);
	ALX_SPI_ASSERT(me->device == NULL);

	// Init GPIO
	AlxIoPin_Init(me->do_nCS);

	// Init SPI
	me->device = device_get_binding(me->deviceName);
	if (me->device == NULL)
	{
		ALX_SPI_TRACE_ERR("FAIL: device_get_binding() deviceName %s", me->deviceName);
		return Alx_Err;
	}

	// Set isInit
	me->isInit = true;

	// Trace
	ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);

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
	// Trace
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s", me->deviceName);

	// Assert
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	ALX_SPI_ASSERT(me->device != NULL);

	// DeInit SPI
	me->device = NULL;

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_nCS);

	// Clear isInit
	me->isInit = false;

	// Trace
	ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		writeData
  * @param[in]		len
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  * @retval			Alx_ErrNumOfTries
  */
Alx_Status AlxSpi_Master_Write(AlxSpi* me, uint8_t* writeData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s len %u numOfTries %u timeout_ms %u", me->deviceName, len, numOfTries, timeout_ms);


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	(void)me;
	(void)writeData;
	(void)len;
	(void)numOfTries;
	ALX_SPI_ASSERT(timeout_ms == 0);	// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		//------------------------------------------------------------------------------
		// Trace
		//------------------------------------------------------------------------------
		ALX_SPI_TRACE_VRB("DO: spi_write() deviceName %s try %u len %u", me->deviceName, _try, len);
		#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
		ALX_SPI_TRACE_FORMAT("    writeData: ");
		for (uint32_t i = 0; i < len; i++)
		{
			ALX_SPI_TRACE_FORMAT("%02X ", writeData[i]);
		}
		ALX_SPI_TRACE_FORMAT("\r\n");
		#endif


		//------------------------------------------------------------------------------
		// Write
		//------------------------------------------------------------------------------

		// Prepare
		struct spi_buf writeBuff = {};
		writeBuff.buf = writeData;
		writeBuff.len = len;

		struct spi_buf_set writeBuffSet = {};
		writeBuffSet.buffers = &writeBuff;
		writeBuffSet.count = 1;

		// Write
		int32_t status = spi_write(me->device, &me->config, &writeBuffSet);
		if (status != 0)
		{
			#if ALX_TRACE_LEVEL < ALX_TRACE_LEVEL_VRB
			ALX_SPI_TRACE_WRN("FAIL: spi_write() deviceName %s status %ld try %u", me->deviceName, status, _try);
			#else
			ALX_SPI_TRACE_VRB("FAIL: spi_write() deviceName %s status %ld try %u len %u numOfTries %u timeout_ms %u", me->deviceName, status, _try, len, numOfTries, timeout_ms);
			#endif
			continue;
		}


		//------------------------------------------------------------------------------
		// Trace
		//------------------------------------------------------------------------------

		// DONE
		ALX_SPI_TRACE_VRB("DONE: spi_write() deviceName %s status %ld", me->deviceName, status);

		// EXIT
		ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);


		//------------------------------------------------------------------------------
		// Return
		//------------------------------------------------------------------------------
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_SPI_TRACE_ERR("FAIL: Alx_ErrNumOfTries deviceName %s", me->deviceName);

	// Return
	return Alx_ErrNumOfTries;
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
  * @retval			Alx_ErrNumOfTries
  */
Alx_Status AlxSpi_Master_Read(AlxSpi* me, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s len %u numOfTries %u timeout_ms %u", me->deviceName, len, numOfTries, timeout_ms);


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	(void)me;
	(void)readData;
	(void)len;
	(void)numOfTries;
	ALX_SPI_ASSERT(timeout_ms == 0);	// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		//------------------------------------------------------------------------------
		// Trace
		//------------------------------------------------------------------------------
		ALX_SPI_TRACE_VRB("DO: spi_read() deviceName %s try %u len %u", me->deviceName, _try, len);


		//------------------------------------------------------------------------------
		// Read
		//------------------------------------------------------------------------------

		// Prepare
		struct spi_buf readBuff = {};
		readBuff.buf = readData;
		readBuff.len = len;

		struct spi_buf_set readBuffSet = {};
		readBuffSet.buffers = &readBuff;
		readBuffSet.count = 1;

		// Read
		int32_t status = spi_read(me->device, &me->config, &readBuffSet);
		if (status != 0)
		{
			#if ALX_TRACE_LEVEL < ALX_TRACE_LEVEL_VRB
			ALX_SPI_TRACE_WRN("FAIL: spi_read() deviceName %s status %ld try %u", me->deviceName, status, _try);
			#else
			ALX_SPI_TRACE_VRB("FAIL: spi_read() deviceName %s status %ld try %u len %u numOfTries %u timeout_ms %u", me->deviceName, status, _try, len, numOfTries, timeout_ms);
			#endif
			continue;
		}


		//-----------------------------------------------------------------------------
		// Trace
		//-----------------------------------------------------------------------------

		// DONE
		ALX_SPI_TRACE_VRB("DONE: spi_read() deviceName %s status %ld", me->deviceName, status);
		#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
		ALX_SPI_TRACE_FORMAT("    readData: ");
		for (uint32_t i = 0; i < len; i++)
		{
			ALX_SPI_TRACE_FORMAT("%02X ", readData[i]);
		}
		ALX_SPI_TRACE_FORMAT("\r\n");
		#endif

		// EXIT
		ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);


		//-----------------------------------------------------------------------------
		// Return
		//-----------------------------------------------------------------------------
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_SPI_TRACE_ERR("FAIL: Alx_ErrNumOfTries deviceName %s", me->deviceName);

	// Return
	return Alx_ErrNumOfTries;
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
  * @retval			Alx_ErrNumOfTries
  */
Alx_Status AlxSpi_Master_WriteRead(AlxSpi* me, uint8_t* writeData, uint8_t* readData, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s len %u numOfTries %u timeout_ms %u", me->deviceName, len, numOfTries, timeout_ms);


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);
	(void)me;
	(void)writeData;
	(void)readData;
	(void)len;
	(void)numOfTries;
	ALX_SPI_ASSERT(timeout_ms == 0);	// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		//------------------------------------------------------------------------------
		// Trace
		//------------------------------------------------------------------------------
		ALX_SPI_TRACE_VRB("DO: spi_transceive() deviceName %s try %u len %u", me->deviceName, _try, len);
		#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
		ALX_SPI_TRACE_FORMAT("    writeData: ");
		for (uint32_t i = 0; i < len; i++)
		{
			ALX_SPI_TRACE_FORMAT("%02X ", writeData[i]);
		}
		ALX_SPI_TRACE_FORMAT("\r\n");
		#endif


		//------------------------------------------------------------------------------
		// Write/Read
		//------------------------------------------------------------------------------

		// Prepare
		struct spi_buf writeBuff = {};
		writeBuff.buf = writeData;
		writeBuff.len = len;
		struct spi_buf_set writeBuffSet = {};
		writeBuffSet.buffers = &writeBuff;
		writeBuffSet.count = 1;

		struct spi_buf readBuff = {};
		readBuff.buf = readData;
		readBuff.len = len;
		struct spi_buf_set readBuffSet = {};
		readBuffSet.buffers = &readBuff;
		readBuffSet.count = 1;

		// Write/Read
		int32_t status = spi_transceive(me->device, &me->config, &writeBuffSet, &readBuffSet);
		if (status != 0)
		{
			#if ALX_TRACE_LEVEL < ALX_TRACE_LEVEL_VRB
			ALX_SPI_TRACE_WRN("FAIL: spi_transceive() deviceName %s status %ld try %u", me->deviceName, status, _try);
			#else
			ALX_SPI_TRACE_VRB("FAIL: spi_transceive() deviceName %s status %ld try %u len %u numOfTries %u timeout_ms %u", me->deviceName, status, _try, len, numOfTries, timeout_ms);
			#endif
			continue;
		}


		//------------------------------------------------------------------------------
		// Trace
		//------------------------------------------------------------------------------

		// DONE
		ALX_SPI_TRACE_VRB("DONE: spi_transceive() deviceName %s status %ld", me->deviceName, status);
		#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
		ALX_SPI_TRACE_FORMAT("    readData: ");
		for (uint32_t i = 0; i < len; i++)
		{
			ALX_SPI_TRACE_FORMAT("%02X ", readData[i]);
		}
		ALX_SPI_TRACE_FORMAT("\r\n");
		#endif

		// EXIT
		ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);


		//------------------------------------------------------------------------------
		// Return
		//------------------------------------------------------------------------------
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_SPI_TRACE_ERR("FAIL: Alx_ErrNumOfTries deviceName %s", me->deviceName);

	// Return
	return Alx_ErrNumOfTries;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSpi_Master_AssertCs(AlxSpi* me)
{
	// Trace
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s", me->deviceName);

	// Assert
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);

	// Assert GPIO
	AlxIoPin_Reset(me->do_nCS);

	// Trace
	ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxSpi_Master_DeAssertCs(AlxSpi* me)
{
	// Trace
	ALX_SPI_TRACE_DBG("ENTER: deviceName %s", me->deviceName);

	// Assert
	ALX_SPI_ASSERT(me->wasCtorCalled == true);
	ALX_SPI_ASSERT(me->isInit == true);

	// DeAssert GPIO
	AlxIoPin_Set(me->do_nCS);

	// Trace
	ALX_SPI_TRACE_DBG("EXIT: deviceName %s", me->deviceName);
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
