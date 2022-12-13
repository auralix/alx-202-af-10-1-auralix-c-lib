/**
  ******************************************************************************
  * @file		alxTmp1075.c
  * @brief		Auralix C Library - ALX Temperature Sensor TMP1075 Module
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
	// Objects - External
	me->i2c = i2c;

	// Parameters
	me->DEG_C_PER_BIT = 0.0625;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	me->temp_raw = 0;
	me->temp_degC = 0;

	AlxTmp1075_RegStruct_SetAddr(me);
	AlxTmp1075_RegStruct_SetLen(me);
	AlxTmp1075_RegStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTmp1075_Init(AlxTmp1075* me)
{
	ALX_TMP1075_ASSERT(me->isInit == false);
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_AlxI2c_Init"); return status; }

	// #2 Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 3, 1000);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }

	// #3 Set register struct values to default
	AlxTmp1075_RegStruct_SetToDefault(me);

	// #4 Set register values - WEAK
	AlxTmp1075_RegStruct_SetVal(me);

	// #5 Read ID registers & Trace ID
	status = AlxTmp1075_TraceId(me);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_TraceId"); return status; }

	// #6 Write registers
	status = AlxTmp1075_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_Reg_WriteNonClkVal"); return status;}

	me->isInit = true;

	// #7 Return OK
	return Alx_Ok;
}
Alx_Status AlxTmp1075_DeInit(AlxTmp1075* me)
{
	ALX_TMP1075_ASSERT(me->isInit == true);
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);

	Alx_Status status = Alx_Err;

	// #1 DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_AlxI2c_DeInit"); return status; }

	// #2 Reset isInit
	me->isInit = false;

	// #3 Return OK
	return Alx_Ok;
}
bool AlxTmp1075_IsInit(AlxTmp1075* me)
{
	// #1 Return isInit
	return me->isInit ;
}
float AlxTmp1075_GetTemp_degC(AlxTmp1075* me)
{
	ALX_TMP1075_ASSERT(me->isInit == true);
	ALX_TMP1075_ASSERT(me->wasCtorCalled == true);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read temperature data
	status = AlxTmp1075_Reg_Read(me, &me->reg.R0_Temp);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_AlxTmp1075_Reg_Read"); }

	// #3 Shift value to get rid of the 4 unused bits
	me->temp_raw = me->reg.R0_Temp.val.T  >> 4;

	// #4 Calculate
	me->temp_degC = me->temp_raw  * me->DEG_C_PER_BIT;

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
	Alx_Status status = Alx_Err;

	uint8_t regAddr		= *((uint8_t*)reg);
	uint8_t regLen		= *((uint8_t*)reg + sizeof(regAddr));
	uint16_t *regValPtr	= (uint16_t*)((uint8_t*)reg + sizeof(regAddr) + sizeof(regLen));
	uint8_t data[2];

	// #1 Read data
	status =  AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);

	// #2 Merge bytes, convert from big endian to little endian
	*regValPtr = data[0] << 8 | data[1];

	return status;
}
static Alx_Status AlxTmp1075_Reg_Write(AlxTmp1075* me, void* reg)
{
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint16_t *regValPtr	= (uint16_t*)((uint8_t*)reg + sizeof(regAddr) + sizeof(regLen));
	uint8_t data[2];

	// #1 Divide to bytes, convert from little endian to big endian
	data[0] = *regValPtr >> 8;
	data[1] = *regValPtr;

	// #2 Write data
	return AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, regLen, false, me->i2cNumOfTries, me->i2cTimeout_ms);
}
static Alx_Status AlxTmp1075_Reg_Write_All(AlxTmp1075* me)
{
	Alx_Status status = Alx_Err;

	status = AlxTmp1075_Reg_Write(me, &me->reg.R1_Config	);
	if (status != Alx_Ok) { ALX_TMP1075_ASSERT("Err_R1_Config	"); return status;}

	status = AlxTmp1075_Reg_Write(me, &me->reg.R2_LimitLow	);
	if (status != Alx_Ok) { ALX_TMP1075_ASSERT("Err_R2_LimitLow	"); return status;}

	status = AlxTmp1075_Reg_Write(me, &me->reg.R3_LimitHigh	);
	if (status != Alx_Ok) { ALX_TMP1075_ASSERT("Err_R3_LimitHigh"); return status;}

	return Alx_Ok;
}
static Alx_Status AlxTmp1075_TraceId(AlxTmp1075* me)
{
	Alx_Status status = Alx_Err;

	// #1 Read ID register
	status = AlxTmp1075_Reg_Read(me, &me->reg.R4_DeviceId);
	if (status != Alx_Ok) { ALX_TMP1075_TRACE("Err_R4_DeviceId	"); return status;}

	// #2 Trace
	ALX_TMP1075_TRACE_FORMAT("\r\n");
	ALX_TMP1075_TRACE_FORMAT("Auralix C Library ALX Temperature Sensor TMP1075 Module Identification:\r\n");
	ALX_TMP1075_TRACE_FORMAT("- Device ID: %X\r\n", me->reg.R4_DeviceId.val.DID);
	ALX_TMP1075_TRACE_FORMAT("\r\n");

	// #3 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxTmp1075_RegStruct_SetVal(AlxTmp1075* me)
{
	(void)me;

	ALX_TMP1075_TRACE("Define 'AlxTmp1075_RegStruct_SetVal' function in your application.");
	ALX_TMP1075_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
