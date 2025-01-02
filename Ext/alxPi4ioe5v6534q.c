/**
  ******************************************************************************
  * @file		alxPi4ioe5v6534q.c
  * @brief		Auralix C Library - ALX IO Expander PI4IOE5V6534Q Module
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
#include "alxPi4ioe5v6534q.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPi4ioe5v6534q_RegStruct_SetAddr(AlxPi4ioe5v6534q* me);
static void AlxPi4ioe5v6534q_RegStruct_SetLen(AlxPi4ioe5v6534q* me);
static void AlxPi4ioe5v6534q_RegStruct_SetValToZero(AlxPi4ioe5v6534q* me);
static void AlxPi4ioe5v6534q_RegStruct_SetValToDefault(AlxPi4ioe5v6534q* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPi4ioe5v6534q_RegStruct_SetVal(AlxPi4ioe5v6534q* me);
Alx_Status AlxPi4ioe5v6534q_RegStruct_Write(AlxPi4ioe5v6534q* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		do_nRESET
  * @param[in]		i2c
  * @param[in]		i2cAddr
  * @param[in]		i2cCheckWithRead
  * @param[in]		i2cNumOfTries
  * @param[in]		i2cTimeout_ms
  */
void AlxPi4ioe5v6534q_Ctor
(
	AlxPi4ioe5v6534q* me,
	AlxIoPin* do_nRESET,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters
	me->do_nRESET = do_nRESET;
	me->i2c = i2c;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxPi4ioe5v6534q_RegStruct_SetAddr(me);
	AlxPi4ioe5v6534q_RegStruct_SetLen(me);
	AlxPi4ioe5v6534q_RegStruct_SetValToZero(me);

	// Info
	me->wasCtorCalled = true;
	me->isInitPeriph = false;
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
Alx_Status AlxPi4ioe5v6534q_InitPeriph(AlxPi4ioe5v6534q* me)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_PI4IOE5V6534Q_TRACE_ERR("FAIL: AlxI2c_Init() status %ld", status);
		return status;
	}

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPi4ioe5v6534q_DeInitPeriph(AlxPi4ioe5v6534q* me)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_PI4IOE5V6534Q_TRACE_ERR("FAIL: AlxI2c_DeInit() status %ld", status);
		return status;
	}

	// Reset isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPi4ioe5v6534q_Init(AlxPi4ioe5v6534q* me)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInitPeriph == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init GPIO
	AlxIoPin_Init(me->do_nRESET);

	// Enable device
	AlxIoPin_Set(me->do_nRESET);

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok)
	{
		ALX_PI4IOE5V6534Q_TRACE_ERR("FAIL: AlxI2c_Master_IsSlaveReady() status %ld", status);
		return status;
	}

	// Set register struct values to default
	AlxPi4ioe5v6534q_RegStruct_SetValToDefault(me);

	// Set register struct values - WEAK
	AlxPi4ioe5v6534q_RegStruct_SetVal(me);

	// Write register struct - WEAK
	status = AlxPi4ioe5v6534q_RegStruct_Write(me);
	if (status != Alx_Ok)
	{
		ALX_PI4IOE5V6534Q_TRACE_ERR("FAIL: AlxPi4ioe5v6534q_RegStruct_Write() status %ld", status);
		return status;
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
Alx_Status AlxPi4ioe5v6534q_DeInit(AlxPi4ioe5v6534q* me)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInitPeriph == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInit == true);

	// Disable device
	AlxIoPin_Reset(me->do_nRESET);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_nRESET);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		inPortNum - Number of input ports to handle, can be from 0 (none) to 5 (max)
  * @param[in]		outPortNum - Number of output ports to handle, can be from 0 (none) to 5 (max)
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPi4ioe5v6534q_Handle(AlxPi4ioe5v6534q* me, uint8_t inPortNum, uint8_t outPortNum)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInitPeriph == true);
	ALX_PI4IOE5V6534Q_ASSERT(me->isInit == true);
	ALX_PI4IOE5V6534Q_ASSERT((0 <= inPortNum) && (inPortNum <= 5));
	ALX_PI4IOE5V6534Q_ASSERT((0 <= outPortNum) && (outPortNum <= 5));

	// Local variables
	Alx_Status status = Alx_Err;

	// Handle input ports
	if (inPortNum != 0)
	{
		status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, me->reg._0x00_InputPort.addr, AlxI2c_Master_MemAddrLen_8bit, me->reg._0x00_InputPort.val.raw, inPortNum, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_PI4IOE5V6534Q_TRACE_WRN("FAIL: AlxI2c_Master_StartReadMemStop() status %ld", status);
			return status;
		}
	}

	// Handle output ports
	if (outPortNum != 0)
	{
		status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, me->reg._0x05_OutputPort.addr, AlxI2c_Master_MemAddrLen_8bit, me->reg._0x05_OutputPort.val.raw, outPortNum, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
		if (status != Alx_Ok)
		{
			ALX_PI4IOE5V6534Q_TRACE_WRN("FAIL: AlxI2c_Master_StartWriteMemStop_Multi() status %ld", status);
			return status;
		}
	}

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @retval			false
  * @retval			true
  */
bool AlxPi4ioe5v6534q_IoPin_Read(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PI4IOE5V6534Q_ASSERT((0 <= port) && (port <= 4));
	ALX_PI4IOE5V6534Q_ASSERT((0 <= pin) && (pin <= 7));

	// Read
	if (me->reg._0x00_InputPort.val.raw[port] & (1 << pin))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		val
  */
void AlxPi4ioe5v6534q_IoPin_Write(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin, bool val)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PI4IOE5V6534Q_ASSERT((0 <= port) && (port <= 4));
	ALX_PI4IOE5V6534Q_ASSERT((0 <= pin) && (pin <= 7));

	// Write
	if (val == true)
	{
		me->reg._0x05_OutputPort.val.raw[port] |=  (1 << pin);
	}
	else
	{
		me->reg._0x05_OutputPort.val.raw[port] &= ~(1 << pin);
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  */
void AlxPi4ioe5v6534q_IoPin_Set(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PI4IOE5V6534Q_ASSERT((0 <= port) && (port <= 4));
	ALX_PI4IOE5V6534Q_ASSERT((0 <= pin) && (pin <= 7));

	// Set
	AlxPi4ioe5v6534q_IoPin_Write(me, port, pin, true);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  */
void AlxPi4ioe5v6534q_IoPin_Reset(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PI4IOE5V6534Q_ASSERT((0 <= port) && (port <= 4));
	ALX_PI4IOE5V6534Q_ASSERT((0 <= pin) && (pin <= 7));

	// Reset
	AlxPi4ioe5v6534q_IoPin_Write(me, port, pin, false);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  */
void AlxPi4ioe5v6534q_IoPin_Toggle(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PI4IOE5V6534Q_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PI4IOE5V6534Q_ASSERT((0 <= port) && (port <= 4));
	ALX_PI4IOE5V6534Q_ASSERT((0 <= pin) && (pin <= 7));

	// Toggle
	me->reg._0x05_OutputPort.val.raw[port] ^=  (1 << pin);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPi4ioe5v6534q_RegStruct_SetAddr(AlxPi4ioe5v6534q* me)
{
	me->reg._0x00_InputPort				.addr = 0x00;
	me->reg._0x05_OutputPort			.addr = 0x05;
	me->reg._0x0F_ConfigurationPort		.addr = 0x0F;
}
static void AlxPi4ioe5v6534q_RegStruct_SetLen(AlxPi4ioe5v6534q* me)
{
	me->reg._0x00_InputPort				.len = sizeof(me->reg._0x00_InputPort			.val);
	me->reg._0x05_OutputPort			.len = sizeof(me->reg._0x05_OutputPort			.val);
	me->reg._0x0F_ConfigurationPort		.len = sizeof(me->reg._0x0F_ConfigurationPort	.val);
}
static void AlxPi4ioe5v6534q_RegStruct_SetValToZero(AlxPi4ioe5v6534q* me)
{
	memset(me->reg._0x00_InputPort			.val.raw, 0x00, sizeof(me->reg._0x00_InputPort			.val.raw));
	memset(me->reg._0x05_OutputPort			.val.raw, 0x00, sizeof(me->reg._0x05_OutputPort			.val.raw));
	memset(me->reg._0x0F_ConfigurationPort	.val.raw, 0x00, sizeof(me->reg._0x0F_ConfigurationPort	.val.raw));
}
static void AlxPi4ioe5v6534q_RegStruct_SetValToDefault(AlxPi4ioe5v6534q* me)
{
	memset(me->reg._0x00_InputPort			.val.raw, 0x00, sizeof(me->reg._0x00_InputPort			.val.raw));
	memset(me->reg._0x05_OutputPort			.val.raw, 0xFF, sizeof(me->reg._0x05_OutputPort			.val.raw));
	memset(me->reg._0x0F_ConfigurationPort	.val.raw, 0xFF, sizeof(me->reg._0x0F_ConfigurationPort	.val.raw));
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxPi4ioe5v6534q_RegStruct_SetVal(AlxPi4ioe5v6534q* me)
{
	(void)me;
	ALX_PI4IOE5V6534Q_ASSERT(false);	// Implement function in APP
}
ALX_WEAK Alx_Status AlxPi4ioe5v6534q_RegStruct_Write(AlxPi4ioe5v6534q* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	// _0x00_InputPort	// Read only register

	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, me->reg._0x05_OutputPort.addr, AlxI2c_Master_MemAddrLen_8bit, me->reg._0x05_OutputPort.val.raw, me->reg._0x05_OutputPort.len, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok)
	{
		ALX_PI4IOE5V6534Q_TRACE_WRN("FAIL: AlxI2c_Master_StartWriteMemStop_Multi() status %ld", status);
		return status;
	}

	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, me->reg._0x0F_ConfigurationPort.addr, AlxI2c_Master_MemAddrLen_8bit, me->reg._0x0F_ConfigurationPort.val.raw, me->reg._0x0F_ConfigurationPort.len, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok)
	{
		ALX_PI4IOE5V6534Q_TRACE_WRN("FAIL: AlxI2c_Master_StartWriteMemStop_Multi() status %ld", status);
		return status;
	}

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
