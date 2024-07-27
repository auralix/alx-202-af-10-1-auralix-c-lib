/**
  ******************************************************************************
  * @file		alxI2c_McuZephyr.c
  * @brief		Auralix C Library - ALX I2C MCU Zephyr Module
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
#include "alxI2c_McuZephyr.h"
#include "alxI2c.h"


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
  */
void AlxI2c_Ctor
(
	AlxI2c* me,
	const char* deviceName
)
{
	// Parameters
	me->deviceName = deviceName;

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
Alx_Status AlxI2c_Init(AlxI2c* me)
{
	// Assert
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == false);
	ALX_I2C_ASSERT(me->device == NULL);

	// Init
	me->device = device_get_binding(me->deviceName);
	if (me->device == NULL)
	{
		ALX_I2C_TRACE("Err: device_get_binding");
		return Alx_Err;
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
Alx_Status AlxI2c_DeInit(AlxI2c* me)
{
	// Assert
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->device != NULL);

	// DeInit
	me->device = NULL;

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[out]		data
  * @param[in]		len
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartRead(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint16_t timeout_ms)
{
	// TV: TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[out]		data
  * @param[in]		len
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartReadStop(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(timeout_ms == 0);	// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Read
		int32_t status = i2c_read(me->device, data, len, slaveAddr >> 1);
		if (status != 0)
		{
			ALX_I2C_TRACE("Err: %d, slaveAddr=0x%02X, len=%u, try=%u", status, slaveAddr, len, _try);
			continue;
		}

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_I2C_TRACE("Err: Alx_ErrNumOfTries");

	// Return
	return Alx_ErrNumOfTries;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[in]		memAddr
  * @param[in]		memAddrLen
  * @param[out]		data
  * @param[in]		len
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartReadMemStop(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	ALX_I2C_ASSERT(memAddr <= 0xFF);								// TV: Only 8-bit memory address supported
	ALX_I2C_ASSERT(memAddrLen == AlxI2c_Master_MemAddrLen_8bit);	// TV: Only 8-bit memory address supported
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(timeout_ms == 0);								// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Read
		int32_t status = i2c_burst_read(me->device, slaveAddr >> 1, memAddr, data, len);
		if (status != 0)
		{
			ALX_I2C_TRACE("Err: %d, slaveAddr=0x%02X, memAddr=0x%02X, len=%u, try=%u", status, slaveAddr, memAddr, len, _try);
			continue;
		}

		// Return
		return Alx_Ok;

	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_I2C_TRACE("Err: Alx_ErrNumOfTries");

	// Return
	return Alx_ErrNumOfTries;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[in]		data
  * @param[in]		len
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartWrite(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint16_t timeout_ms)
{
	// TV: TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[in]		data
  * @param[in]		len
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartWriteStop(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(timeout_ms == 0);	// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Read
		int32_t status = i2c_write(me->device, data, len, slaveAddr >> 1);
		if (status != 0)
		{
			ALX_I2C_TRACE("Err: %d, slaveAddr=0x%02X, len=%u, try=%u", status, slaveAddr, len, _try);
			continue;
		}

		// Return
		return Alx_Ok;

	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_I2C_TRACE("Err: Alx_ErrNumOfTries");

	// Return
	return Alx_ErrNumOfTries;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[in]		memAddr
  * @param[in]		memAddrLen
  * @param[in]		data
  * @param[in]		checkWithRead
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartWriteMemStop_Single(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t data, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
	return AlxI2c_Master_StartWriteMemStop_Multi(me, slaveAddr, memAddr, memAddrLen, &data, 1, checkWithRead, numOfTries, timeout_ms);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[in]		memAddr
  * @param[in]		memAddrLen
  * @param[in]		data
  * @param[in]		len
  * @param[in]		checkWithRead
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_StartWriteMemStop_Multi(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, const uint8_t* data, uint16_t len, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	ALX_I2C_ASSERT(memAddr <= 0xFF);								// TV: Only 8-bit memory address supported
	ALX_I2C_ASSERT(memAddrLen == AlxI2c_Master_MemAddrLen_8bit);	// TV: Only 8-bit memory address supported
	(void)data;
	if (checkWithRead)	{ ALX_I2C_ASSERT((0 < len) && (len <= ALX_I2C_BUFF_LEN)); }
	else				{ ALX_I2C_ASSERT(0 < len); }
	(void)checkWithRead;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(timeout_ms == 0);								// TV: Unsupported


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	for (uint8_t _try = 1; _try <= numOfTries; _try++)
	{
		// Write
		int32_t status = i2c_burst_write(me->device, slaveAddr >> 1, memAddr, data, len);
		if (status != 0)
		{
			ALX_I2C_TRACE("Err: %d, slaveAddr=0x%02X, memAddr=0x%02X, len=%u, try=%u", status, slaveAddr, memAddr, len, _try);
			continue;
		}

		// If check with read enabled
		if (checkWithRead)
		{
			// Read
			Alx_Status status = Alx_Err;
			uint8_t buff[ALX_I2C_BUFF_LEN] = {};
			status = AlxI2c_Master_StartReadMemStop(me, slaveAddr, memAddr, memAddrLen, buff, len, numOfTries, timeout_ms);
			if (status != Alx_Ok)
			{
				ALX_I2C_TRACE("Err: %d, slaveAddr=0x%02X, memAddr=0x%02X, len=%u", status, slaveAddr, memAddr, len, _try);
				continue;
			}

			// Check
			if (memcmp(buff, data, len) != 0)
			{
				ALX_I2C_TRACE("Err: memcmp");
				continue;
			}
		}

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_I2C_TRACE("Err: Alx_ErrNumOfTries");

	// Return
	return Alx_ErrNumOfTries;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_Stop(AlxI2c* me, uint16_t timeout_ms)
{
	// TV: TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		slaveAddr
  * @param[in]		numOfTries
  * @param[in]		timeout_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxI2c_Master_IsSlaveReady(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms)
{
	// Assert
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	(void)numOfTries;
	(void)timeout_ms;

	// Return
	return Alx_Ok;	// TV: Unsupported, just return Alx_Ok
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
