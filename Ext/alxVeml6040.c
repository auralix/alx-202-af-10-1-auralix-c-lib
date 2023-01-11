/**
  ******************************************************************************
  * @file		alxVeml6040.c
  * @brief		Auralix C Library - ALX Light Sensor VEML604 Module
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
#include "alxVeml6040.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxVeml6040_RegStruct_SetAddr(AlxVeml6040* me);
static void AlxVeml6040_RegStruct_SetLen(AlxVeml6040* me);
static void AlxVeml6040_RegStruct_SetValToZero(AlxVeml6040* me);
static void AlxVeml6040_RegStruct_SetToDefault(AlxVeml6040* me);
static Alx_Status AlxVeml6040_Reg_Write(AlxVeml6040* me, void* reg);
static Alx_Status AlxVeml6040_Reg_Write_All(AlxVeml6040* me);
static Alx_Status AlxVeml6040_Reg_Read(AlxVeml6040* me, void* reg);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxVeml6040_RegStruct_SetVal(AlxVeml6040* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxVeml6040_Ctor
(
	AlxVeml6040* me,
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
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	me->r_16bit = 0;
	me->g_16bit = 0;
	me->b_16bit = 0;
	me->w_16bit = 0;

	AlxVeml6040_RegStruct_SetAddr(me);
	AlxVeml6040_RegStruct_SetLen(me);
	AlxVeml6040_RegStruct_SetValToZero(me);

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxVeml6040_Init(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == false);

	Alx_Status status = Alx_Err;

	// #1 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxI2c_Init"); return status; }

	// #2 Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, 3, 1000);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxI2c_IsSlaveReady"); return status; }

	// #3 Set register struct values to default
	AlxVeml6040_RegStruct_SetToDefault(me);

	// #4 Set register values - WEAK
	AlxVeml6040_RegStruct_SetVal(me);

	// #5 Write registers
	status = AlxVeml6040_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_Reg_WriteNonClkVal"); return status;}

	me->isInit = true;

	// #6 Return OK
	return Alx_Ok;
}
Alx_Status AlxVeml6040_DeInit(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	Alx_Status status = Alx_Err;

	// #1 DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxI2c_DeInit"); return status; }

	// #2 Reset isInit
	me->isInit = false;

	// #3 Return OK
	return Alx_Ok;
}
bool AlxVeml6040_IsInit(AlxVeml6040* me)
{
	// #1 Return isInit
	return me->isInit;
}
Alx_Status AlxVeml6040_GetR_raw(AlxVeml6040* me, uint16_t* R_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read R color data
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x08_R_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxVeml6040_GetR_raw"); return status; }

	//#3 Return OK and data
	*R_raw = me->reg._0x08_R_DATA.val.raw;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetG_raw(AlxVeml6040* me, uint16_t* G_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read R color data
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x09_G_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxVeml6040_GetG_raw"); return status; }

	//#3 Return OK and data
	*G_raw = me->reg._0x09_G_DATA.val.raw;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetB_raw(AlxVeml6040* me, uint16_t* B_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read R color data
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x0A_B_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxVeml6040_GetB_raw"); return status; }

	//#3 Return OK and data
	*B_raw = me->reg._0x0A_B_DATA.val.raw;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetW_raw(AlxVeml6040* me, uint16_t* W_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Read R color data
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x0B_W_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_AlxVeml6040_GetW_raw"); return status; }

	//#3 Return OK and data
	*W_raw = me->reg._0x0B_W_DATA.val.raw;
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxVeml6040_RegStruct_SetAddr(AlxVeml6040* me)
{
	me->reg._0x00_CONF		.addr = 0x00;
	me->reg._0x08_R_DATA	.addr = 0x08;
	me->reg._0x09_G_DATA	.addr = 0x09;
	me->reg._0x0A_B_DATA	.addr = 0x0A;
	me->reg._0x0B_W_DATA	.addr = 0x0B;
}
static void AlxVeml6040_RegStruct_SetLen(AlxVeml6040* me)
{
	me->reg._0x00_CONF		.len = sizeof(me->reg._0x00_CONF	.val);
	me->reg._0x08_R_DATA	.len = sizeof(me->reg._0x08_R_DATA	.val);
	me->reg._0x09_G_DATA	.len = sizeof(me->reg._0x09_G_DATA	.val);
	me->reg._0x0A_B_DATA	.len = sizeof(me->reg._0x0A_B_DATA	.val);
	me->reg._0x0B_W_DATA	.len = sizeof(me->reg._0x0B_W_DATA	.val);
}
static void AlxVeml6040_RegStruct_SetValToZero(AlxVeml6040* me)
{
	me->reg._0x00_CONF		.val.raw = 0x0000;
	me->reg._0x08_R_DATA	.val.raw = 0x0000;
	me->reg._0x09_G_DATA	.val.raw = 0x0000;
	me->reg._0x0A_B_DATA	.val.raw = 0x0000;
	me->reg._0x0B_W_DATA	.val.raw = 0x0000;
}
static void AlxVeml6040_RegStruct_SetToDefault(AlxVeml6040* me)
{
	me->reg._0x00_CONF		.val.raw = 0x0001;
	me->reg._0x08_R_DATA	.val.raw = 0x0000;
	me->reg._0x09_G_DATA	.val.raw = 0x0000;
	me->reg._0x0A_B_DATA	.val.raw = 0x0000;
	me->reg._0x0B_W_DATA	.val.raw = 0x0000;
}

Alx_Status AlxVeml6040_Reg_Write(AlxVeml6040* me, void* reg)
{
	// #1 Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #2 Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #3 Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxVeml6040_Reg_Write_All(AlxVeml6040* me)
{
	// #1 Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #2 Local variables
	Alx_Status status = Alx_Err;

	// #3 Write
	status = AlxVeml6040_Reg_Write(me, &me->reg._0x00_CONF	);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err_0x00_CONF"); return status;}

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxVeml6040_Reg_Read(AlxVeml6040* me, void* reg)
{
	// #1 Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// #2 Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #3 Read
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// #4 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxVeml6040__RegStruct_SetVal(AlxVeml6040* me)
{
	(void)me;
	ALX_VEML6040_TRACE("Define 'AlxVeml6040__RegStruct_SetVal' function in your application.");
	ALX_VEML6040_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
