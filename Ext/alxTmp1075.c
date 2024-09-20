/**
  ******************************************************************************
  * @file		alxTmp1075.c
  * @brief		Auralix C Library - ALX Temperature Sensor TMP1075 Module
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
#include "alxTmp1075.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxTmp1075_RegStruct_SetAddr(AlxTmp1075* me);
static void AlxTmp1075_RegStruct_SetLen(AlxTmp1075* me);
static void AlxTmp1075_RegStruct_SetValToZero(AlxTmp1075* me);
static void AlxTmp1075_RegStruct_SetToDefault(AlxTmp1075* me);
static Alx_Status AlxTmp1075_Reg_Read(AlxTmp1075* me, void* reg);
static Alx_Status AlxTmp1075_Reg_Write(AlxTmp1075* me, void* reg);
static Alx_Status AlxTmp1075_Reg_Write_All(AlxTmp1075* me);
static Alx_Status AlxTmp1075_TraceId(AlxTmp1075* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxTmp1075_RegStruct_SetVal(AlxTmp1075* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		i2cAddr
  * @param[in]		i2cCheckWithRead
  * @param[in]		i2cNumOfTries
  * @param[in]		i2cTimeout_ms
  */
void AlxTmp1075_Ctor
(
	AlxTmp1075* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters
	me->i2c = i2c;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxTmp1075_RegStruct_SetAddr(me);
	AlxTmp1075_RegStruct_SetLen(me);
	AlxTmp1075_RegStruct_SetValToZero(me);
	me->temp_raw = 0;
	me->temp_degC = 0;

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
Alx_Status AlxTmp1075_Init(AlxTmp1075* me)
{
	// Assert
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);
	ALX_TMP1075_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); return status; }

	// Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 3, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); return status; }

	// Set register struct values to default
	AlxTmp1075_RegStruct_SetToDefault(me);

	//Set register values - WEAK
	AlxTmp1075_RegStruct_SetVal(me);

	// Read ID register & Trace ID
	status = AlxTmp1075_TraceId(me);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); return status; }

	// Write registers
	status = AlxTmp1075_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); return status;}

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
Alx_Status AlxTmp1075_DeInit(AlxTmp1075* me)
{
	// Assert
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);
	ALX_TMP1075_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); return status; }

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxTmp1075_IsInit(AlxTmp1075* me)
{
	// Assert
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isInit;
}

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxTmp1075_GetTemp_degC(AlxTmp1075* me)
{
	// Assert
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);
	ALX_TMP1075_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read temperature data
	status = AlxTmp1075_Reg_Read(me, &me->reg.R0_Temp);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); }

	// Shift value to get rid of the 4 unused bits
	me->temp_raw = me->reg.R0_Temp.val.T >> 4;

	// Calculate
	me->temp_degC = me->temp_raw * ALX_TMP1075_DEG_C_PER_BIT;

	// Return
	return me->temp_degC;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxTmp1075_RegStruct_SetAddr(AlxTmp1075* me)
{
	me->reg.R0_Temp			.addr = 0x00;
	me->reg.R1_Config		.addr = 0x01;
	me->reg.R2_LimitLow		.addr = 0x02;
	me->reg.R3_LimitHigh	.addr = 0x03;
	me->reg.R4_DeviceId		.addr = 0x0F;
}
static void AlxTmp1075_RegStruct_SetLen(AlxTmp1075* me)
{
	me->reg.R0_Temp			.len = sizeof(me->reg.R0_Temp		.val);
	me->reg.R1_Config		.len = sizeof(me->reg.R1_Config		.val);
	me->reg.R2_LimitLow		.len = sizeof(me->reg.R2_LimitLow	.val);
	me->reg.R3_LimitHigh	.len = sizeof(me->reg.R3_LimitHigh	.val);
	me->reg.R4_DeviceId		.len = sizeof(me->reg.R4_DeviceId	.val);
}
static void AlxTmp1075_RegStruct_SetValToZero(AlxTmp1075* me)
{
	me->reg.R0_Temp			.val.raw = 0x0000;
	me->reg.R1_Config		.val.raw = 0x0000;
	me->reg.R2_LimitLow		.val.raw = 0x0000;
	me->reg.R3_LimitHigh	.val.raw = 0x0000;
	me->reg.R4_DeviceId		.val.raw = 0x0000;
}
static void AlxTmp1075_RegStruct_SetToDefault(AlxTmp1075* me)
{
	me->reg.R0_Temp			.val.raw = 0x0000;
	me->reg.R1_Config		.val.raw = 0x00FF;
	me->reg.R2_LimitLow		.val.raw = 0x4B00;
	me->reg.R3_LimitHigh	.val.raw = 0x5000;
	me->reg.R4_DeviceId		.val.raw = 0x7500;
}
static Alx_Status AlxTmp1075_Reg_Read(AlxTmp1075* me, void* reg)
{
	// Local variables
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint16_t *regValPtr = (uint16_t*)((uint8_t*)reg + sizeof(regAddr) + sizeof(regLen));
	uint8_t data[2] = {};

	// Read
	Alx_Status status =  AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);

	// Merge bytes, convert from big endian to little endian
	*regValPtr = data[0] << 8 | data[1];

	// Return
	return status;
}
static Alx_Status AlxTmp1075_Reg_Write(AlxTmp1075* me, void* reg)
{
	// Local variables
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint16_t *regValPtr = (uint16_t*)((uint8_t*)reg + sizeof(regAddr) + sizeof(regLen));
	uint8_t data[2] = {};

	// Divide to bytes, convert from little endian to big endian
	data[0] = *regValPtr >> 8;
	data[1] = *regValPtr;

	// Write
	return AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, regLen, false, me->i2cNumOfTries, me->i2cTimeout_ms);
}
static Alx_Status AlxTmp1075_Reg_Write_All(AlxTmp1075* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	status = AlxTmp1075_Reg_Write(me, &me->reg.R1_Config);
	if (status != Alx_Ok) { ALX_TMP1075_ASSERT("Err"); return status;}

	status = AlxTmp1075_Reg_Write(me, &me->reg.R2_LimitLow);
	if (status != Alx_Ok) { ALX_TMP1075_ASSERT("Err"); return status;}

	status = AlxTmp1075_Reg_Write(me, &me->reg.R3_LimitHigh);
	if (status != Alx_Ok) { ALX_TMP1075_ASSERT("Err"); return status;}

	// Return
	return Alx_Ok;
}
static Alx_Status AlxTmp1075_TraceId(AlxTmp1075* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Read ID register
	status = AlxTmp1075_Reg_Read(me, &me->reg.R4_DeviceId);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE_WRN("Err"); return status;}

	// Trace
	ALX_TMP1075_TRACE_INF("");
	ALX_TMP1075_TRACE_INF("Auralix C Library ALX Temperature Sensor TMP1075 Module Identification:");
	ALX_TMP1075_TRACE_INF("- Device ID: %X", me->reg.R4_DeviceId.val.DID);
	ALX_TMP1075_TRACE_INF("");

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxTmp1075_RegStruct_SetVal(AlxTmp1075* me)
{
	(void)me;
}


#endif	// #if defined(ALX_C_LIB)
