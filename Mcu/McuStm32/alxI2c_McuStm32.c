/**
  ******************************************************************************
  * @file		alxI2c_McuStm32.c
  * @brief		Auralix C Library - ALX I2C Module
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
#include "alxI2c_McuStm32.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint16_t AlxI2c_ParseMemAddrLen(AlxI2c_Master_MemAddrLen memAddrLen);
static uint32_t AlxI2c_ParseClk(AlxI2c_Clk clk);
static Alx_Status AlxI2c_Reset(AlxI2c* me);
static void AlxI2c_Periph_EnableClk(AlxI2c* me);
static void AlxI2c_Periph_DisableClk(AlxI2c* me);
static void AlxI2c_Periph_ForceReset(AlxI2c* me);
static void AlxI2c_Periph_ReleaseReset(AlxI2c* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxI2c_Ctor
(
	AlxI2c* me,
	I2C_TypeDef* i2c,
	AlxIoPin* io_SCL,
	AlxIoPin* io_SDA,
	AlxI2c_Clk clk
)
{
	// Objects - External
	me->io_SCL = io_SCL;
	me->io_SDA = io_SDA;

	// Parameters
	me->clk = clk;

	// Variables
	me->hi2c.Instance = i2c;
	me->hi2c.Init.OwnAddress1 = 0;
	me->hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	me->hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	me->hi2c.Init.OwnAddress2 = 0;
	me->hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	me->hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	
	#if defined(ALX_STM32G4) || defined(ALX_STM32L0)
	me->hi2c.Init.Timing = AlxI2c_ParseClk(clk);
	me->hi2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	#endif
	#if defined (ALX_STM32F4)
	me->hi2c.Init.ClockSpeed = AlxI2c_ParseClk(clk);
	me->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	#endif

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxI2c_Init(AlxI2c* me)
{
	ALX_I2C_ASSERT(me->isInit == false);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->io_SCL);
	AlxIoPin_Init(me->io_SDA);

	// #2 Release I2C Periphery Reset
	AlxI2c_Periph_ReleaseReset(me);
	
	// #3 Enable I2C Periphery Clock
	AlxI2c_Periph_EnableClk(me);

	// #4 Init I2C
	if (HAL_I2C_Init(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("ErrInit"); return Alx_Err; }
	
	// #5 Set isInit
	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxI2c_DeInit(AlxI2c* me)
{
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	
	// #1 DeInit I2C
	if (HAL_I2C_DeInit(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("ErrDeInit"); return Alx_Err; }

	// #2 Force I2C Periphery Reset
	AlxI2c_Periph_ForceReset(me);
	
	// #3 Disable I2C Periphery Clock
	AlxI2c_Periph_DisableClk(me);

	// #4 DeInit GPIO
	AlxIoPin_DeInit(me->io_SCL);
	AlxIoPin_DeInit(me->io_SDA);

	// #5 Reset isInit
	me->isInit = false;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxI2c_Master_StartRead(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_StartReadStop(AlxI2c* me, uint16_t slaveAddr, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < len);
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try read memory for number of tries
	for(uint32_t _try = 1 ; _try <= numOfTries ; _try++)
	{
		status = HAL_I2C_Master_Receive(&me->hi2c, slaveAddr, data, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // read OK
		}
		else
		{
			ALX_I2C_TRACE("ErrHal");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}
	}

	// #3 If we are here, read was OK or number of tries error occured
	if(status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxI2c_Master_StartReadMemStop(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
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

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t _memAddrLen = AlxI2c_ParseMemAddrLen(memAddrLen);

	// #2 Try read memory for number of tries
	for (uint32_t _try = 1; _try <= numOfTries; _try++)
	{
		status = HAL_I2C_Mem_Read(&me->hi2c, slaveAddr, memAddr, _memAddrLen, data, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // Memory read OK
		}
		else
		{
			ALX_I2C_TRACE("ErrHal");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}
	}

	// #3 If we are here, read was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxI2c_Master_StartWrite(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_StartWriteStop(AlxI2c* me, uint16_t slaveAddr, const uint8_t* data, uint16_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	(void)data;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;

	// #2 Try write memory for number of tries
	for(uint32_t _try = 1 ; _try <= numOfTries ; _try++)
	{
		status = HAL_I2C_Master_Transmit(&me->hi2c, slaveAddr,(uint8_t*)data, len, timeout_ms);
		if (status == HAL_OK)
		{
			break; // Write OK
		}
		else
		{
			ALX_I2C_TRACE("ErrHal");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}
	}

	// #3 If we are here, read was OK or number of tries error occured
	if(status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxI2c_Master_StartWriteMemStop_Single(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, uint8_t data, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
	return AlxI2c_Master_StartWriteMemStop_Multi(me, slaveAddr, memAddr, memAddrLen, &data, 1, checkWithRead, numOfTries, timeout_ms);
}
Alx_Status AlxI2c_Master_StartWriteMemStop_Multi(AlxI2c* me, uint16_t slaveAddr, uint16_t memAddr, AlxI2c_Master_MemAddrLen memAddrLen, const uint8_t* data, uint16_t len, bool checkWithRead, uint8_t numOfTries, uint16_t timeout_ms)
{
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

	// #1 Prepare variables
	HAL_StatusTypeDef status = HAL_ERROR;
	uint16_t _memAddrLen = AlxI2c_ParseMemAddrLen(memAddrLen);
	uint8_t buff[ALX_I2C_BUFF_LEN] = {};

	// #2 Try write memory for number of tries
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
					ALX_I2C_TRACE("Read_ErrNumOfTries");
					continue;
				}
				else if (status != Alx_Ok)
				{ 
					ALX_I2C_ASSERT(false); // We should never get here
					return Alx_Err;
				}
				else if (memcmp(buff, data, len) != 0) // Check previously written data
				{ 
					ALX_I2C_TRACE("Read_ErrCheckWithRead");
					continue;
				}
				else
				{
					break; // Memory write OK, with check with read
				}
			}
			else
			{
				break; // Memory write OK, without check with read
			}
		}
		else
		{
			ALX_I2C_TRACE("ErrHal");
			if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
			continue;
		}
	}

	// #3 If we are here, write was OK or number of tries error occured
	if (status == HAL_OK)
	{
		return Alx_Ok;
	}
	else
	{
		ALX_I2C_TRACE("ErrNumOfTries");
		return Alx_ErrNumOfTries;
	}
}
Alx_Status AlxI2c_Master_Stop(AlxI2c* me, uint16_t timeout_ms)
{
	// TODO
	ALX_I2C_ASSERT(false);
	return Alx_Err;
}
Alx_Status AlxI2c_Master_IsSlaveReady(AlxI2c* me, uint16_t slaveAddr, uint8_t numOfTries, uint16_t timeout_ms)
{
	ALX_I2C_ASSERT(me->isInit == true);
	ALX_I2C_ASSERT(me->wasCtorCalled == true);
	(void)me;
	(void)slaveAddr;
	ALX_I2C_ASSERT(0 < numOfTries);
	ALX_I2C_ASSERT(0 < timeout_ms);

	// #1 Try slave
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&me->hi2c, slaveAddr, numOfTries, timeout_ms);
	if (status != HAL_OK)
	{
		ALX_I2C_TRACE("ErrHal");
		if (AlxI2c_Reset(me) != Alx_Ok) { ALX_I2C_TRACE("ErrReset"); return Alx_Err; }
	}

	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static uint16_t AlxI2c_ParseMemAddrLen(AlxI2c_Master_MemAddrLen memAddrLen)
{
	if		(memAddrLen == AlxI2c_Master_MemAddrLen_8bit)	{ return I2C_MEMADD_SIZE_8BIT; }
	else if	(memAddrLen == AlxI2c_Master_MemAddrLen_16bit)	{ return I2C_MEMADD_SIZE_16BIT; }
	else													{ ALX_I2C_ASSERT(false); return 0; } // We shouldn't get here
}
static uint32_t AlxI2c_ParseClk(AlxI2c_Clk clk)
{
	if		(clk == AlxI2c_Clk_100kHz)	{ return ALX_I2C_MCU_STM32_CLK_100kHz; }
	else if	(clk == AlxI2c_Clk_400kHz)	{ return ALX_I2C_MCU_STM32_CLK_400kHz; }
	else								{ ALX_I2C_ASSERT(false); return 0; } // We shouldn't get here
}
static Alx_Status AlxI2c_Reset(AlxI2c* me)
{
	// #1 DeInit I2C
	if (HAL_I2C_DeInit(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("ErrDeInit"); return Alx_Err; }

	// #2 Force I2C Periphery Reset
	AlxI2c_Periph_ForceReset(me);

	// #3 Reset isInit
	me->isInit = false;

	// #4 Release I2C Periphery Reset
	AlxI2c_Periph_ReleaseReset(me);
	
	// #5 Init I2C
	if (HAL_I2C_Init(&me->hi2c) != HAL_OK) { ALX_I2C_TRACE("ErrInit"); return Alx_Err; }

	// #6 Set isInit
	me->isInit = true;

	// #7 Return OK
	return Alx_Ok;
}
static void AlxI2c_Periph_EnableClk(AlxI2c* me)
{
	bool isErr = true;
	
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_CLK_ENABLE(); isErr = false; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_CLK_ENABLE(); isErr = false; }
	#endif
	
	if(isErr)						{ ALX_I2C_ASSERT(false); } // We shouldn't get here
}
static void AlxI2c_Periph_DisableClk(AlxI2c* me)
{
	bool isErr = true;
	
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_CLK_DISABLE(); isErr = false; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_CLK_DISABLE(); isErr = false; }
	#endif
	
	if(isErr)						{ ALX_I2C_ASSERT(false); } // We shouldn't get here
}
static void AlxI2c_Periph_ForceReset(AlxI2c* me)
{
	bool isErr = true;
	
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_FORCE_RESET(); isErr = false; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_FORCE_RESET(); isErr = false; }
	#endif
	
	if(isErr)						{ ALX_I2C_ASSERT(false); } // We shouldn't get here
}
static void AlxI2c_Periph_ReleaseReset(AlxI2c* me)
{
	bool isErr = true;
	
	#if defined(I2C1)
	if (me->hi2c.Instance == I2C1)	{ __HAL_RCC_I2C1_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(I2C2)
	if (me->hi2c.Instance == I2C2)	{ __HAL_RCC_I2C2_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(I2C3)
	if (me->hi2c.Instance == I2C3)	{ __HAL_RCC_I2C3_RELEASE_RESET(); isErr = false; }
	#endif
	#if defined(I2C4)
	if (me->hi2c.Instance == I2C4)	{ __HAL_RCC_I2C4_RELEASE_RESET(); isErr = false; }
	#endif
	
	if(isErr)						{ ALX_I2C_ASSERT(false); } // We shouldn't get here
}


#endif
