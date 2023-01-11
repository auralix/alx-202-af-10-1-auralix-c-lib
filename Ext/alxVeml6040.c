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
	// Parameters
	me->i2c = i2c;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables

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

	// #9 Return OK
	return Alx_Ok;
}
Alx_Status AlxVeml6040_DeInit(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetR_raw(AlxVeml6040* me, uint16_t* R_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Local variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	uint16_t slaveAddr = 0x10;
	uint8_t* data = 0x00;
	uint16_t len = 1;
	uint16_t timeout_ms = 100;
	status = AlxI2c_Init(me->i2c);
	status = AlxI2c_Master_StartRead(me->i2c, slaveAddr, data, len, timeout_ms);

	//------------------------------------------------------------------------------
	// Read @ Red color  lux
	//------------------------------------------------------------------------------
	//status = AlxPcal6416a_Reg_Read(me, &me->reg._00h_InputPort_0);
	//if (status != Alx_Ok) { ALX_VEML6040_TRACE("Err"); return status; }

	// Return
	*R_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetG_raw(AlxVeml6040* me, uint16_t* G_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*G_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetB_raw(AlxVeml6040* me, uint16_t* B_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*B_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetW_raw(AlxVeml6040* me, uint16_t* W_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*W_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_Reg_Write(AlxVeml6040* me, void* reg)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

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
Alx_Status AlxVeml6040_Reg_Read(AlxVeml6040* me, void* reg)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

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


#endif	// #if defined(ALX_C_LIB)
