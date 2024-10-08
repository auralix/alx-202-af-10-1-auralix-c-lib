﻿/**
  ******************************************************************************
  * @file		alxBq25890.c
  * @brief		Auralix C Library - ALX Battery charger
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
#include "alxBq25890.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxBq25890_RegStruct_SetAddr(AlxBq25890* me);
static void AlxBq25890_RegStruct_SetLen(AlxBq25890* me);
static Alx_Status AlxBq25890_RegStruct_SetValToDefault(AlxBq25890* me);

//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxBq25890_RegStruct_SetVal(AlxBq25890* me);
static Alx_Status AlxBq25890_RegStruct_Write(AlxBq25890* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		i2c
  * @param[in]		IINLIM Input current limit
  * @param[in]		JEITA_VSET High Temperature Voltage Setting
  * @param[in]		BATFET_DIS Enable ship mode
  */
void AlxBq25890_Ctor
(
	AlxBq25890* me,
	AlxI2c* i2c,
	uint8_t IINLIM,
	bool JEITA_VSET,
	bool BATFET_DIS
)
{

	// Variables
	AlxBq25890_RegStruct_SetAddr(me);
	AlxBq25890_RegStruct_SetLen(me);

	// Parameters
	me->i2c = i2c;
	me->i2cAddr = 0xD4; //0x6A;
	me->i2cCheckWithRead = true;
	me->i2cNumOfTries = 3;
	me->i2cTimeout_ms = 500;

	if(IINLIM != 0)
		me->IINLIM = IINLIM;
	me->JEITA_VSET = JEITA_VSET;
	me->BATFET_DIS = BATFET_DIS;

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
Alx_Status AlxBq25890_InitPeriph(AlxBq25890* me)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

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
Alx_Status AlxBq25890_DeInitPeriph(AlxBq25890* me)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

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
Alx_Status AlxBq25890_Init(AlxBq25890* me)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Multiple drivers use the same I2C instance, so it could already be initialized.
	if (me->i2c->isInit == false)
	{
		status = AlxBq25890_InitPeriph(me);
		if (status != Alx_Ok) return status;
	}
	else
	{
		me->isInitPeriph = true;
	}

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	// Set register struct values to default (read from IC)
	AlxBq25890_RegStruct_SetValToDefault(me);

	// Any custom configuration goes here:
	me->reg.REG_00.val.IINLIM = me->IINLIM;
	me->reg.REG_09.val.BATFET_DIS = me->BATFET_DIS;
	me->reg.REG_09.val.JEITA_VSET = me->JEITA_VSET;

	status = AlxBq25890_RegStruct_Write(me);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

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
Alx_Status AlxBq25890_DeInit(AlxBq25890* me)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	ALX_BQ25890_ASSERT(me->isInit == true);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief Poll (read) all registers
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_Poll(AlxBq25890* me)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	ALX_BQ25890_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Handle
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_00);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_09);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_0B);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_0C);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	return Alx_Ok;
}

/**
  * @brief Read registers (make sure to poll the driver first)
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_Read(AlxBq25890* me, AlxBq25890_Reg *reg)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	ALX_BQ25890_ASSERT(me->isInit == true);

	memcpy(reg, &me->reg, sizeof(AlxBq25890_Reg));

	return Alx_Ok;
}

/**
  * @brief Parse faults (make sure to poll the driver first)
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_GetFaults(AlxBq25890* me, AlxBq2890_faults_t *faults)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	ALX_BQ25890_ASSERT(me->isInit == true);

	// Local variables

	faults->BAT_FAULT = me->reg.REG_0C.val.BAT_FAULT;
	faults->CHRG_FAULT = me->reg.REG_0C.val.CHGR_FAULT;
	faults->NTC_FAULT = me->reg.REG_0C.val.NTC_FAULT;

	return Alx_Ok;
}

/**
  * @brief Parse charging status (make sure to poll the driver first)
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_GetChargingStatus(AlxBq25890* me, uint8_t *status)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	ALX_BQ25890_ASSERT(me->isInit == true);

	*status = me->reg.REG_0B.val.CHRG_STAT;

	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		reg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_Reg_Read(AlxBq25890* me, void* reg)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		reg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_Reg_Write(AlxBq25890* me, void* reg)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		value
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_set_IINLIM(AlxBq25890* me, uint8_t value)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	Alx_Status ret;
	ret = AlxBq25890_Reg_Read(me, &me->reg.REG_00);
	if (ret != Alx_Ok) return ret;

	value = value & 0x3F; // bound the incoming value to 6 bits

	me->reg.REG_00.val.IINLIM = value;

	ret = AlxBq25890_Reg_Write(me, &me->reg.REG_00);
	if (ret != Alx_Ok) return ret;

	return ret;

}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		value
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_set_JEITA_VSET(AlxBq25890* me, bool value)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	Alx_Status ret;
	ret = AlxBq25890_Reg_Read(me, &me->reg.REG_09);
	if (ret != Alx_Ok) return ret;

	me->reg.REG_09.val.JEITA_VSET = value;

	ret = AlxBq25890_Reg_Write(me, &me->reg.REG_09);
	if (ret != Alx_Ok) return ret;

	return ret;

}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		value
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxBq25890_set_shipping_mode(AlxBq25890* me)
{
	// Assert
	ALX_BQ25890_ASSERT(me->wasCtorCalled == true);
	ALX_BQ25890_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	Alx_Status ret;
	ret = AlxBq25890_Reg_Read(me, &me->reg.REG_09);
	if (ret != Alx_Ok) return ret;

	me->reg.REG_09.val.BATFET_DIS = 1;

	ret = AlxBq25890_Reg_Write(me, &me->reg.REG_09);
	if (ret != Alx_Ok) return ret;

	return ret;

}

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxBq25890_RegStruct_SetAddr(AlxBq25890* me)
{
	me->reg.REG_00.addr = 0x00;
	me->reg.REG_09.addr = 0x09;
	me->reg.REG_0B.addr = 0x0B;
	me->reg.REG_0C.addr = 0x0C;

}
static void AlxBq25890_RegStruct_SetLen(AlxBq25890* me)
{
	me->reg.REG_00.len = sizeof(me->reg.REG_00.val);
	me->reg.REG_09.len = sizeof(me->reg.REG_09.val);
	me->reg.REG_0B.len = sizeof(me->reg.REG_0B.val);
	me->reg.REG_0C.len = sizeof(me->reg.REG_0C.val);
}

static Alx_Status AlxBq25890_RegStruct_SetValToDefault(AlxBq25890* me)
{
	Alx_Status status = Alx_Ok;
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_00);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_09);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_0B);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	status = AlxBq25890_Reg_Read(me, &me->reg.REG_0C);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }
	return Alx_Ok;
}

static Alx_Status AlxBq25890_RegStruct_Write(AlxBq25890* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	status = AlxBq25890_Reg_Write(me, &me->reg.REG_00);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	status = AlxBq25890_Reg_Write(me, &me->reg.REG_09);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	status = AlxBq25890_Reg_Write(me, &me->reg.REG_0B);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	status = AlxBq25890_Reg_Write(me, &me->reg.REG_0C);
	if (status != Alx_Ok) { ALX_BQ25890_TRACE("Err"); return status; }

	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)