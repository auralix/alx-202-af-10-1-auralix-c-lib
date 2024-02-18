/**
  ******************************************************************************
  * @file		alxI2c_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX I2C MCU LPC55S6X Module
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
#include "alxI2c_McuLpc55S6x.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxI2c_Reset(AlxI2c* me);
static uint8_t AlxI2c_GetMemAddrLen(AlxI2c_Master_MemAddrLen* memAddrLen);
static void AlxI2c_Periph_AttachClk(AlxI2c* me);
static uint32_t AlxI2c_GetFlexCommClkFreq(AlxI2c* me);

// MF: These functions are copied from FSL so that Timeout can be added
static status_t AlxI2c_MasterStart(AlxI2c* me, I2C_Type* base, uint8_t address, i2c_direction_t direction, uint16_t timeout);
static status_t AlxI2c_MasterWriteBlocking(AlxI2c* me, I2C_Type* base, const void* txBuff, size_t txSize, uint32_t flags, uint16_t timeout);
static status_t AlxI2c_MasterReadBlocking(AlxI2c* me, I2C_Type* base, void* rxBuff, size_t rxSize, uint32_t flags, uint16_t timeout);
static status_t AlxI2c_MasterStop(AlxI2c* me, I2C_Type *base, uint16_t timeout);
static uint32_t AlxI2c_PendingStatusWait(AlxI2c* me, I2C_Type* base, uint16_t timeout);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		io_SCL
  * @param[in]		io_SDA
  * @param[in]		clk
  */
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_Type* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
)
{
	// Parameters
	me->i2c = i2c;
	me->io_SCL = io_SCL;
	me->io_SDA = io_SDA;
	me->clk = clk;

	// Variables
	me->i2cConfig.enableMaster = true;
	me->i2cConfig.baudRate_Bps = me->clk;	// MF: baudRate = bit rate
	me->i2cConfig.enableTimeout = false;
	me->i2cConfig.timeout_Ms = 1;			// MF: We will use our own timeout. Must be at least "1" otherwise FSL triggers Assert
	AlxTimSw_Ctor(&me->tim, false);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
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
	ALX_I2C_ASSERT(me->isInit == false);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);

	// Init GPIO
	AlxIoPin_Init(me->io_SCL);
	AlxIoPin_Init(me->io_SDA);

	// Select I2C Periphery Clock
	AlxI2c_Periph_AttachClk(me);

	// Init I2C
	I2C_MasterInit(me->i2c, &me->i2cConfig, AlxI2c_GetFlexCommClkFreq(me)); // MF: "Periph_Reset" and "EnableClk" happens here. MF: srcClock_Hz = I2cFuncClk = MainClk

	// Set isInit
	me->isInit = true;

	// Return OK
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
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);

	// DeInit I2C
	I2C_MasterDeinit(me->i2c);	// MF: "DisableClk" happens here

	// DeInit GPIO
	AlxIoPin_DeInit(me->io_SCL);
	AlxIoPin_DeInit(me->io_SDA);

	// Reset isInit
	me->isInit = false;

	// Return OK
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Send Start Condition
	status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Read, timeout_ms);
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrStartCondition");
		if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
	}

	// Return OK
	return Alx_Ok;
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Try read for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Read, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Read Data
		status = AlxI2c_MasterReadBlocking(me, me->i2c, data, len, kI2C_TransferDefaultFlag, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrFlsRead");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Send Stop Condition
		status = AlxI2c_MasterStop(me, me->i2c, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStopCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Return Ok
		return Alx_Ok;
	}

	// If we are here, the number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}

	// Assert	// We should not get here
	ALX_I2C_ASSERT(false);
	return Alx_Err;
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)memAddr;
	(void)memAddrLen;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;
	uint8_t _memAddr[2] = { memAddr & 0xFF, (memAddr >> 8) & 0xFF };
	uint8_t _memAddrLen = AlxI2c_GetMemAddrLen(&memAddrLen);

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Try read memory for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Write Slave Address
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, _memAddr, _memAddrLen, kI2C_TransferDefaultFlag, timeout_ms);	// MF: Write memAddr, then Stop
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrWriteSlaveAddr");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Read, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Read Memory
		status = AlxI2c_MasterReadBlocking(me, me->i2c, data, len, kI2C_TransferDefaultFlag, timeout_ms);	// MF: Read data, then Stop
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrFlsRead");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Send Stop Condition
		status = AlxI2c_MasterStop(me, me->i2c, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStopCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Return Ok
		return Alx_Ok;
	}

	// If we are here, the number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}

	// Assert	// We should not get here
	ALX_I2C_ASSERT(false);
	return Alx_Err;
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Send Start Condition
	status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrStartCondition");
		if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
	}

	// Return OK
	return Alx_Ok;
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Try write for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Write Data
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, data, len, kI2C_TransferDefaultFlag, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrWriteSlaveAddr");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Send Stop Condition
		status = AlxI2c_MasterStop(me, me->i2c, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStopCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Return Ok
		return Alx_Ok;
	}

	// If we are here, the number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}

	// Assert	// We should not get here
	ALX_I2C_ASSERT(false);
	return Alx_Err;
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
	return AlxI2c_Master_StartWriteMemStop_Multi(me, slaveAddr, memAddr, memAddrLen, &data, 1, checkWithRead, numOfTries, timeout_ms);	// MF: Asserts are checked here
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)memAddr;
	(void)memAddrLen;
	(void)data;
	if (checkWithRead)	{ ALX_I2C_ASSERT((0 < len) && (len <= ALX_I2C_BUFF_LEN)); }
	else				{ ALX_I2C_ASSERT(0 < len); }
	(void)checkWithRead;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;
	uint8_t _memAddr[2] = { memAddr & 0xFF, (memAddr >> 8) & 0xFF };
	uint8_t _memAddrLen = AlxI2c_GetMemAddrLen(&memAddrLen);
	uint8_t buff[ALX_I2C_BUFF_LEN] = { 0 };

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Try write memory for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Write Slave Address
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, _memAddr, _memAddrLen, kI2C_TransferNoStopFlag, timeout_ms);	// MF: Write memAddr, no Stop
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrWriteSlaveAddr");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Write Memory
		status = AlxI2c_MasterWriteBlocking(me, me->i2c, data, len, kI2C_TransferNoStartFlag, timeout_ms);	// MF: Write data then Stop
		if (status == kStatus_Success)
		{
			// If enabled, read & check previously written data
			if (checkWithRead)
			{
				Alx_Status alxStat = Alx_Err;
				alxStat = AlxI2c_Master_StartReadMemStop(me, slaveAddr, memAddr, memAddrLen, buff, len, numOfTries, timeout_ms);
				if (alxStat == Alx_ErrNumOfTries)
				{
					ALX_I2C_TRACE("Read_ErrNumOfTries");
					continue;
				}
				else if (alxStat != Alx_Ok)
				{
					ALX_I2C_ASSERT(false); // We should never get here
					return Alx_Err;
				}
				else if (memcmp(buff, data, len) != 0)	// MF: Check previously written data
				{
					ALX_I2C_TRACE("Read_ErrCheckWithRead");
					continue;
				}
			}

			// Send Stop Condition
			status = AlxI2c_MasterStop(me, me->i2c, timeout_ms);
			if (status != kStatus_Success)
			{
				ALX_I2C_TRACE("ErrStopCondition");
				if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
				continue;
			}

			// Return Ok
			return Alx_Ok;		// MF: Memory write OK, without check with read
		}
		else
		{
			ALX_I2C_TRACE("ErrFlsWrite");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}
	}

	// If we are here, write was OK or number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}

	// Assert	// We should not get here
	ALX_I2C_ASSERT(false);
	return Alx_Err;
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
	// Assert
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Send Stop Condition
	status = AlxI2c_MasterStop(me, me->i2c, timeout_ms);
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrStopCondition");
		if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
	}

	// Return OK
	return Alx_Ok;
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
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Prepare variables
	status_t status = kStatus_Fail;
	Alx_Status alxStat = Alx_Err;
	uint32_t flag = 0;

	// Start Timeout
	AlxTimSw_Start(&me->tim);

	// Try Generate start and send Slave address for number of tries
	for (uint32_t _tryNo = 1; _tryNo <= numOfTries; _tryNo++)
	{
		// Send Start Condition
		status = AlxI2c_MasterStart(me, me->i2c, slaveAddr, kI2C_Write, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStartCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Get status flag and check if NACK was return
		flag = I2C_GetStatusFlags(me->i2c);
		if (flag & (uint32_t)kI2C_MasterAddrNackFlag)
		{
			ALX_I2C_TRACE("ErrNack");
			status = kStatus_Fail;
			alxStat = Alx_Err;
			continue;
		}

		// Send Stop Condition
		status = AlxI2c_MasterStop(me, me->i2c, timeout_ms);
		if (status != kStatus_Success)
		{
			ALX_I2C_TRACE("ErrStopCondition");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}

		// Return Ok
		return Alx_Ok;
	}

	// If we are here, the number of tries error occured
	if (status != kStatus_Success)
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}

	// Assert	// We should not get here
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxI2c_Reset(AlxI2c* me)
{
	// DeInit I2C
	I2C_MasterDeinit(me->i2c); // MF: "DisableClk" happens here

	// DeInit GPIO
	AlxIoPin_DeInit(me->io_SCL);
	AlxIoPin_DeInit(me->io_SDA);

	// Reset isInit
	me->isInit = false;

	// Init GPIO
	AlxIoPin_Init(me->io_SCL);
	AlxIoPin_Init(me->io_SDA);

	// Select I2C Periphery Clock
	AlxI2c_Periph_AttachClk(me);

	// Init I2C
	I2C_MasterInit(me->i2c, &me->i2cConfig, AlxI2c_GetFlexCommClkFreq(me)); // MF: "Periph_Reset" and "EnableClk" happens here. MF: srcClock_Hz = I2cFuncClk = MainClk

	// Set isInit
	me->isInit = true;

	// Return OK
	return Alx_Ok;
}
static uint8_t AlxI2c_GetMemAddrLen(AlxI2c_Master_MemAddrLen* memAddrLen)
{
	if		(*memAddrLen == AlxI2c_Master_MemAddrLen_8bit)		{ return 1; }
	else if	(*memAddrLen == AlxI2c_Master_MemAddrLen_16bit)		{ return 2; }
	else														{ ALX_I2C_ASSERT(false); return 0; }	// MF: We shouldn't get here
}
static void AlxI2c_Periph_AttachClk(AlxI2c* me)
{
	// Assert
	(void)me;

	// Attach Clk to FlexComm
	#if defined(I2C0)
	if (me->i2c == I2C0)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM0); return; }
	#endif
	#if defined(I2C1)
	if (me->i2c == I2C1)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1); return; }
	#endif
	#if defined(I2C2)
	if (me->i2c == I2C2)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM2); return; }
	#endif
	#if defined(I2C3)
	if (me->i2c == I2C3)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3); return; }
	#endif
	#if defined(I2C4)
	if (me->i2c == I2C4)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM4); return; }
	#endif
	#if defined(I2C5)
	if (me->i2c == I2C5)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM5); return; }
	#endif
	#if defined(I2C6)
	if (me->i2c == I2C6)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM6); return; }
	#endif
	#if defined(I2C7)
	if (me->i2c == I2C7)	{ CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM7); return; }
	#endif

	//Assert
	ALX_I2C_ASSERT(false); // We shouldn't get here
	return;
}
static uint32_t AlxI2c_GetFlexCommClkFreq(AlxI2c* me)
{
	// Assert
	(void)me;

	// Attach Clk to FlexComm
	#if defined(I2C0)
	if (me->i2c == I2C0)	{ return CLOCK_GetFlexCommClkFreq(0U); }
	#endif
	#if defined(I2C1)
	if (me->i2c == I2C1)	{ return CLOCK_GetFlexCommClkFreq(1U); }
	#endif
	#if defined(I2C2)
	if (me->i2c == I2C2)	{ return CLOCK_GetFlexCommClkFreq(2U); }
	#endif
	#if defined(I2C3)
	if (me->i2c == I2C3)	{ return CLOCK_GetFlexCommClkFreq(3U); }
	#endif
	#if defined(I2C4)
	if (me->i2c == I2C4)	{ return CLOCK_GetFlexCommClkFreq(4U); }
	#endif
	#if defined(I2C5)
	if (me->i2c == I2C5)	{ return CLOCK_GetFlexCommClkFreq(5U); }
	#endif
	#if defined(I2C6)
	if (me->i2c == I2C6)	{ return CLOCK_GetFlexCommClkFreq(6U); }
	#endif
	#if defined(I2C7)
	if (me->i2c == I2C7)	{ return CLOCK_GetFlexCommClkFreq(7U); }
	#endif

	//Assert
	ALX_I2C_ASSERT(false); // We shouldn't get here
	return 0xFFFFFFFF;
}
static status_t AlxI2c_MasterStart(AlxI2c* me, I2C_Type* base, uint8_t address, i2c_direction_t direction, uint16_t timeout)
{
	if (AlxI2c_PendingStatusWait(me, base, timeout) == (uint32_t)kStatus_I2C_Timeout)
	{
		return kStatus_I2C_Timeout;
	}

	/* Write Address and RW bit to data register */
	base->MSTDAT = (uint32_t)address | ((uint32_t)direction & 1u);
	/* Start the transfer */
	base->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;

	return kStatus_Success;
}
static status_t AlxI2c_MasterWriteBlocking(AlxI2c* me, I2C_Type* base, const void* txBuff, size_t txSize, uint32_t flags, uint16_t timeout)
{
	uint32_t status;
	uint32_t master_state;
	status_t err;

	const uint8_t* buf = (const uint8_t*)txBuff;

	err = kStatus_Success;
	while (txSize != 0U)
	{
		status = AlxI2c_PendingStatusWait(me, base, timeout);

		if (status == kStatus_I2C_Timeout)
		{
			ALX_I2C_TRACE("ErrTimeout");
			return kStatus_I2C_Timeout;
		}

		master_state = (status & I2C_STAT_MSTSTATE_MASK) >> I2C_STAT_MSTSTATE_SHIFT;
		switch (master_state)
		{
		case I2C_STAT_MSTCODE_TXREADY:
			/* ready to send next byte */
			base->MSTDAT = *buf++;
			txSize--;
			base->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
			break;

		case I2C_STAT_MSTCODE_NACKADR:
			/* slave nacked the address */
			err = kStatus_I2C_Addr_Nak;
			break;

		case I2C_STAT_MSTCODE_NACKDAT:
			/* slave nacked the last byte */
			err = kStatus_I2C_Nak;
			break;

		default:
			/* unexpected state */
			err = kStatus_I2C_UnexpectedState;
			break;
		}

		if (err != kStatus_Success) { return err; }
	}

	status = AlxI2c_PendingStatusWait(me, base, timeout);

	if (status == kStatus_I2C_Timeout)
	{
		ALX_I2C_TRACE("ErrTimeout");
		return kStatus_I2C_Timeout;
	}

	if ((flags & (uint32_t)kI2C_TransferNoStopFlag) == 0U)
	{
		/* Initiate stop */
		base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
		status = AlxI2c_PendingStatusWait(me, base, timeout);

		if (status == kStatus_I2C_Timeout)
		{
			ALX_I2C_TRACE("ErrTimeout");
			return kStatus_I2C_Timeout;
		}
	}

	return kStatus_Success;
}
static status_t AlxI2c_MasterReadBlocking(AlxI2c* me, I2C_Type* base, void* rxBuff, size_t rxSize, uint32_t flags, uint16_t timeout)
{
	uint32_t status = 0;
	uint32_t master_state;
	status_t err;

	uint8_t* buf = (uint8_t*)(rxBuff);

	err = kStatus_Success;
	while (rxSize != 0U)
	{
		status = AlxI2c_PendingStatusWait(me, base, timeout);

		if (status == kStatus_I2C_Timeout)
		{
			ALX_I2C_TRACE("ErrTimeout");
			return kStatus_I2C_Timeout;
		}

		master_state = (status & I2C_STAT_MSTSTATE_MASK) >> I2C_STAT_MSTSTATE_SHIFT;
		switch (master_state)
		{
			case I2C_STAT_MSTCODE_RXREADY:
				/* ready to receive next byte */
				*(buf++) = (uint8_t)base->MSTDAT;
				if (--rxSize != 0U)
				{
					base->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
				}
				else
				{
					if ((flags & (uint32_t)kI2C_TransferNoStopFlag) == 0U)
					{
						/* initiate NAK and stop */
						base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
						status = AlxI2c_PendingStatusWait(me, base, timeout);

						if (status == kStatus_I2C_Timeout)
						{
							ALX_I2C_TRACE("ErrTimeout");
							return kStatus_I2C_Timeout;
						}
					}
				}
				break;

			case I2C_STAT_MSTCODE_NACKADR:
			case I2C_STAT_MSTCODE_NACKDAT:
				/* slave nacked the last byte */
				err = kStatus_I2C_Nak;
				break;

			default:
				/* unexpected state */
				err = kStatus_I2C_UnexpectedState;
				break;
		}

		if (err != kStatus_Success) { return err; }
	}

	return kStatus_Success;
}
static status_t AlxI2c_MasterStop(AlxI2c* me, I2C_Type *base, uint16_t timeout)
{
	if (AlxI2c_PendingStatusWait(me, base, timeout) == (uint32_t)kStatus_I2C_Timeout)
	{
		return kStatus_I2C_Timeout;
	}

	base->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
	return kStatus_Success;
}
static uint32_t AlxI2c_PendingStatusWait(AlxI2c* me, I2C_Type* base, uint16_t timeout)
{
	status_t result = kStatus_Success;
	uint32_t status;

	do
	{
		// MF: Check Timeout
		if (AlxTimSw_IsTimeout_ms(&me->tim, timeout))
		{
			ALX_I2C_TRACE("ErrTimeout");
			return (uint32_t)kStatus_I2C_Timeout;
		}

		status = I2C_GetStatusFlags(base);
	} while ((status & (uint32_t)kI2C_MasterPendingFlag) == 0U);

	if ((status & (uint32_t)kI2C_MasterArbitrationLostFlag) != 0U)
	{
		result = kStatus_I2C_ArbitrationLost;
	}

	if ((status & (uint32_t)kI2C_MasterStartStopErrorFlag) != 0U)
	{
		result = kStatus_I2C_StartStopError;
	}

	/* Clear controller state. */
	I2C_ClearStatusFlags(base, (uint32_t)kI2C_MasterAllClearFlags | (uint32_t)kI2C_EventTimeoutFlag | (uint32_t)kI2C_SclTimeoutFlag);

	return status;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
