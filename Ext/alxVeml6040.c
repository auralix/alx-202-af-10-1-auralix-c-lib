/**
  ******************************************************************************
  * @file		alxVeml6040.c
  * @brief		Auralix C Library - ALX Light Sensor VEML6040 Module
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

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		i2cAddr
  * @param[in]		i2cCheckWithRead
  * @param[in]		i2cNumOfTries
  * @param[in]		i2cTimeout_ms
  */
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
	// Parameters
	me->i2c = i2c;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
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

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_Init(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Set register struct values to default
	AlxVeml6040_RegStruct_SetToDefault(me);

	// Set register values - WEAK
	AlxVeml6040_RegStruct_SetVal(me);

	// Write registers
	status = AlxVeml6040_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

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
Alx_Status AlxVeml6040_DeInit(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxVeml6040_RegStruct_SetToDefault(me);

	// Write registers - Disabled IC
	status = AlxVeml6040_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		R_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetR_raw(AlxVeml6040* me, uint16_t* R_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x08_R_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Set
	*R_raw = me->reg._0x08_R_DATA.val.raw;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		G_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetG_raw(AlxVeml6040* me, uint16_t* G_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x09_G_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Set
	*G_raw = me->reg._0x09_G_DATA.val.raw;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		B_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetB_raw(AlxVeml6040* me, uint16_t* B_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x0A_B_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Set
	*B_raw = me->reg._0x0A_B_DATA.val.raw;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		W_raw
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxVeml6040_GetW_raw(AlxVeml6040* me, uint16_t* W_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxVeml6040_Reg_Read(me, &me->reg._0x0B_W_DATA);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Set
	*W_raw = me->reg._0x0B_W_DATA.val.raw;

	// Return
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

static Alx_Status AlxVeml6040_Reg_Write(AlxVeml6040* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxVeml6040_Reg_Write_All(AlxVeml6040* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	status = AlxVeml6040_Reg_Write(me, &me->reg._0x00_CONF	);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxVeml6040_Reg_Read(AlxVeml6040* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Read
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxVeml6040_RegStruct_SetVal(AlxVeml6040* me)
{
	// Local variables
	(void)me;

	// Assert
	ALX_VEML6040_TRACE("Err");
	ALX_VEML6040_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
