/**
  ******************************************************************************
  * @file		alxI2c_McuStm32.c
  * @brief		Auralix C Library - ALX I2C MCU STM32 Module
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
#include "alxI2c_McuStm32.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static uint16_t AlxI2c_ParseMemAddrLen(AlxI2c_Master_MemAddrLen memAddrLen);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxI2c_Reset(AlxI2c* me);
static bool AlxI2c_IsClkOk(AlxI2c* me);
static void AlxI2c_Periph_EnableClk(AlxI2c* me);
static void AlxI2c_Periph_DisableClk(AlxI2c* me);
static void AlxI2c_Periph_ForceReset(AlxI2c* me);
static void AlxI2c_Periph_ReleaseReset(AlxI2c* me);


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
  * @param[in]		i2cClk
  */
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_TypeDef* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxClk* clk,
	AlxI2c_Clk i2cClk
)
{
	// Parameters
	me->i2c = i2c;
	me->io_SCL = io_SCL;
	me->io_SDA = io_SDA;
	me->clk = clk;
	me->i2cClk = i2cClk;

	// Variables
	me->hi2c.Instance = i2c;
	me->hi2c.Init.OwnAddress1 = ALX_NULL;
	me->hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	me->hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	me->hi2c.Init.OwnAddress2 = ALX_NULL;
	me->hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	me->hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	#if defined(ALX_STM32F0) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	me->hi2c.Init.Timing = (uint32_t)i2cClk;
	me->hi2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	#endif
	#if defined (ALX_STM32F4)
	me->hi2c.Init.ClockSpeed = (uint32_t)i2cClk;
	me->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	#endif

	// Check clock
	ALX_I2C_ASSERT(AlxI2c_IsClkOk(me));

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

	// Init GPIO
	AlxIoPin_Init(me->io_SCL);
	AlxIoPin_Init(me->io_SDA);

	// Release I2C periphery reset
	AlxI2c_Periph_ReleaseReset(me);

	// Enable I2C periphery clock
	AlxI2c_Periph_EnableClk(me);

	// Init I2C
	if (HAL_I2C_Init(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("Err"); return Alx_Err; }

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

	// DeInit I2C
	if (HAL_I2C_DeInit(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("Err"); return Alx_Err; }

	// Force I2C periphery reset
	AlxI2c_Periph_ForceReset(me);

	// Disable I2C periphery clock
	AlxI2c_Periph_DisableClk(me);

	// DeInit GPIO
	AlxIoPin_DeInit(me->io_SCL);
	AlxIoPin_DeInit(me->io_SDA);

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
	// Assert
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Local variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// Try read for number of tries
	for(uint32_t _try = 1 ; _try <= numOfTries ; _try++)
	{
		status = HAL_I2C_Master_Receive(&me->hi2c, slaveAddr, data, len, timeout_ms);
		if (status == HAL_OK)
		{
			break;	// Read OK
		}
		else
		{
			ALX_I2C_TRACE("Err");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("Err"); return Alx_Err; }
			continue;
		}
	}

	// If we are here, read was OK or number of tries error occured
	if(status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_I2C_TRACE("Err");
		return Alx_ErrNumOfTries;
	}
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
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	(void)memAddr;
	(void)memAddrLen;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Local variables
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t _memAddrLen = AlxI2c_ParseMemAddrLen(memAddrLen);

	// Try read for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		status = HAL_I2C_Mem_Read(&me->hi2c, slaveAddr, memAddr, _memAddrLen, data, len, timeout_ms);
		if (status == HAL_OK)
		{
			break;	// Read OK
		}
		else
		{
			ALX_I2C_TRACE("Err");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("Err"); return Alx_Err; }
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
		ALX_I2C_TRACE("Err");
		return Alx_ErrNumOfTries;
	}
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
	// Assert
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Local variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// Try write for number of tries
	for(uint32_t _try = 1 ; _try <= numOfTries ; _try++)
	{
		status = HAL_I2C_Master_Transmit(&me->hi2c, slaveAddr,(uint8_t*)data, len, timeout_ms);
		if (status == HAL_OK)
		{
			break;	// Write OK
		}
		else
		{
			ALX_I2C_TRACE("Err");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("Err"); return Alx_Err; }
			continue;
		}
	}

	// If we are here, write was OK or number of tries error occured
	if(status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_I2C_TRACE("Err");
		return Alx_ErrNumOfTries;
	}
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
	// Assert
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	ALX_I2C_ASSERT(me->isInit == true);
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

	// Local variables
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t _memAddrLen = AlxI2c_ParseMemAddrLen(memAddrLen);
	uint8_t buff[ALX_I2C_BUFF_LEN] = {};

	// Try write for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		status = HAL_I2C_Mem_Write(&me->hi2c, slaveAddr, memAddr, _memAddrLen, (uint8_t*)data, len, timeout_ms);
		if (status == HAL_OK)
		{
			// If enabled, read & check previously written data
			if (checkWithRead)
			{
				Alx_Status status = Alx_Err;
				status = AlxI2c_Master_StartReadMemStop(me, slaveAddr, memAddr, memAddrLen, buff, len, numOfTries, timeout_ms);
				if (status == Alx_ErrNumOfTries)
				{
					ALX_I2C_TRACE("Err");
					continue;
				}
				else if (status != Alx_Ok)
				{
					ALX_I2C_ASSERT(false);	// We should never get here
					return Alx_Err;
				}
				else if (memcmp(buff, data, len) != 0)	// Check previously written data
				{
					ALX_I2C_TRACE("Err");
					continue;
				}
				else
				{
					break;	// Memory write OK, with check with read
				}
			}
			else
			{
				break;	// Memory write OK, without check with read
			}
		}
		else
		{
			ALX_I2C_TRACE("Err");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("Err"); return Alx_Err; }
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
		ALX_I2C_TRACE("Err");
		return Alx_ErrNumOfTries;
	}
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
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// Try slave
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&me->hi2c, slaveAddr, numOfTries, timeout_ms);
	if (status != HAL_OK)
	{
		ALX_I2C_TRACE("Err");
		if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("Err"); return Alx_Err; }
	}

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static uint16_t AlxI2c_ParseMemAddrLen(AlxI2c_Master_MemAddrLen memAddrLen)
{
	if		(memAddrLen == AlxI2c_Master_MemAddrLen_8bit)	{ return I2C_MEMADD_SIZE_8BIT; }
	else if	(memAddrLen == AlxI2c_Master_MemAddrLen_16bit)	{ return I2C_MEMADD_SIZE_16BIT; }
	else													{ ALX_I2C_ASSERT(false); return ALX_NULL; }	// We should not get here
}


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static Alx_Status AlxI2c_Reset(AlxI2c* me)
{
	// DeInit I2C
	if (HAL_I2C_DeInit(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("Err"); return Alx_Err; }

	// Force I2C periphery reset
	AlxI2c_Periph_ForceReset(me);

	// Clear isInit
	me->isInit = false;

	// Release I2C periphery reset
	AlxI2c_Periph_ReleaseReset(me);

	// Init I2C
	if (HAL_I2C_Init(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("Err"); return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
static bool AlxI2c_IsClkOk(AlxI2c* me)
{
	//------------------------------------------------------------------------------
	// STM32F0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F0)
	return true;	// Always ok, HSI clock is directly used for I2C periphery
	#endif


	//------------------------------------------------------------------------------
	// STM32F4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4)
	if
	(
		(me->i2cClk == AlxI2c_Clk_McuStm32F4_I2cClk_100kHz_Pclk1Apb1_45MHz) ||
		(me->i2cClk == AlxI2c_Clk_McuStm32F4_I2cClk_400kHz_Pclk1Apb1_45MHz)
	)
	{
		if(45000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32F7
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F7)
	if
	(
		(me->i2cClk == AlxI2c_Clk_McuStm32F7_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_54MHz) ||
		(me->i2cClk == AlxI2c_Clk_McuStm32F7_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_54MHz)
	)
	{
		if(54000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32G4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32G4)
	if
	(
		(me->i2cClk == AlxI2c_Clk_McuStm32G4_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_170MHz) ||
		(me->i2cClk == AlxI2c_Clk_McuStm32G4_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_170MHz)
	)
	{
		if(170000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L0
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L0)
	if
	(
		(me->i2cClk == AlxI2c_Clk_McuStm32L0_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_32MHz) ||
		(me->i2cClk == AlxI2c_Clk_McuStm32L0_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_32MHz)
	)
	{
		if(32000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32L4
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32L4)
	if
	(
		(me->i2cClk == AlxI2c_Clk_McuStm32L4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_80MHz) ||
		(me->i2cClk == AlxI2c_Clk_McuStm32L4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_80MHz)
	)
	{
		if(80000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	if
	(
		(me->i2cClk == AlxI2c_Clk_McuStm32L4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_120MHz) ||
		(me->i2cClk == AlxI2c_Clk_McuStm32L4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_120MHz)
	)
	{
		#if !defined(PWR_CR5_R1MODE)
		ALX_I2C_ASSERT(false);
		#endif

		if(120000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
			return true;
		else
			return false;
	}
	#endif


	//------------------------------------------------------------------------------
	// STM32U5
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32U5)
	if ((me->hi2c.Instance == I2C1) || (me->hi2c.Instance == I2C2) || (me->hi2c.Instance == I2C4))
	{
		if
		(
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_16MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_16MHz)
		)
		{
			if(16000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
		if
		(
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_160MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_160MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_160MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c1_I2c2_I2c4_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_160MHz)
		)
		{
			if(160000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
	}
	if (me->hi2c.Instance == I2C3)
	{
		if
		(
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_16MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_16MHz)
		)
		{
			if(16000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor))
				return true;
			else
				return false;
		}
		if
		(
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_160MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_100kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_160MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_0ns_FallTime_0ns_Pclk1Apb1_160MHz) ||
			(me->i2cClk == AlxI2c_Clk_McuStm32U5_I2c3_I2cClk_400kHz_RiseTime_100ns_FallTime_100ns_Pclk1Apb1_160MHz)
		)
		{
			if(160000000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuStm32_Pclk3Apb3_Ctor))
				return true;
			else
				return false;
		}
	}
	#endif


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_I2C_ASSERT(false);	// We should not get here
	return ALX_NULL;
}
static void AlxI2c_Periph_EnableClk(AlxI2c* me)
{
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_CLK_ENABLE(); return; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_CLK_ENABLE(); return; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_CLK_ENABLE(); return; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_CLK_ENABLE(); return; }
	#endif
	#if defined(I2C5)
	if (me->hi2c.Instance == I2C5)	{ __HAL_RCC_I2C5_CLK_ENABLE(); return; }
	#endif
	#if defined(I2C6)
	if (me->hi2c.Instance == I2C6)	{ __HAL_RCC_I2C6_CLK_ENABLE(); return; }
	#endif

	ALX_I2C_ASSERT(false);	// We should not get here
}
static void AlxI2c_Periph_DisableClk(AlxI2c* me)
{
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_CLK_DISABLE(); return; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_CLK_DISABLE(); return; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_CLK_DISABLE(); return; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_CLK_DISABLE(); return; }
	#endif
	#if defined(I2C5)
	if (me->hi2c.Instance == I2C5)	{ __HAL_RCC_I2C5_CLK_DISABLE(); return; }
	#endif
	#if defined(I2C6)
	if (me->hi2c.Instance == I2C6)	{ __HAL_RCC_I2C6_CLK_DISABLE(); return; }
	#endif

	ALX_I2C_ASSERT(false);	// We should not get here
}
static void AlxI2c_Periph_ForceReset(AlxI2c* me)
{
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_FORCE_RESET(); return; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_FORCE_RESET(); return; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_FORCE_RESET(); return; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_FORCE_RESET(); return; }
	#endif
	#if defined(I2C5)
	if (me->hi2c.Instance == I2C5)	{ __HAL_RCC_I2C5_FORCE_RESET(); return; }
	#endif
	#if defined(I2C6)
	if (me->hi2c.Instance == I2C6)	{ __HAL_RCC_I2C6_FORCE_RESET(); return; }
	#endif

	ALX_I2C_ASSERT(false);	// We should not get here
}
static void AlxI2c_Periph_ReleaseReset(AlxI2c* me)
{
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_RELEASE_RESET(); return; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_RELEASE_RESET(); return; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_RELEASE_RESET(); return; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_RELEASE_RESET(); return; }
	#endif
	#if defined(I2C5)
	if (me->hi2c.Instance == I2C5)	{ __HAL_RCC_I2C5_RELEASE_RESET(); return; }
	#endif
	#if defined(I2C6)
	if (me->hi2c.Instance == I2C6)	{ __HAL_RCC_I2C6_RELEASE_RESET(); return; }
	#endif

	ALX_I2C_ASSERT(false);	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))
