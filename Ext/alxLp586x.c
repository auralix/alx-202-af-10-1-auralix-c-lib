/**
  ******************************************************************************
  * @file		alxLp586x.c
  * @brief		Auralix C Library - ALX LED Driver LP586x Module
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
#include "alxLp586x.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxLp586x_RegStruct_SetAddr(AlxLp586x* me);
static void AlxLp586x_RegStruct_SetLen(AlxLp586x* me);
static void AlxLp586x_RegStruct_SetValToZero(AlxLp586x* me);
static void AlxLp586x_RegStruct_SetValToDefault(AlxLp586x* me);
static Alx_Status AlxLp586x_RegStruct_Write(AlxLp586x* me);

//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxLp586x_RegStruct_SetVal(AlxLp586x* me);


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
void AlxLp586x_Ctor
(
	AlxLp586x* me,
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
	AlxLp586x_RegStruct_SetAddr(me);
	AlxLp586x_RegStruct_SetLen(me);
	AlxLp586x_RegStruct_SetValToZero(me);

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
Alx_Status AlxLp586x_InitPeriph(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxI2c_Init() status %ld", status);
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
Alx_Status AlxLp586x_DeInitPeriph(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxI2c_DeInit() status %ld", status);
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
Alx_Status AlxLp586x_Init(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	ALX_LP586x_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxI2c_Master_IsSlaveReady() status %ld", status);
		return status;
	}

	// Set register struct values to default
	AlxLp586x_RegStruct_SetValToDefault(me);

	// Set register struct values - WEAK
	AlxLp586x_RegStruct_SetVal(me);

	// Write register struct - WEAK
	status = AlxLp586x_RegStruct_Write(me);
	if (status != Alx_Ok)
	{
		ALX_LP586x_TRACE_ERR("FAIL: AlxLp586x_RegStruct_Write() status %ld", status);
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
Alx_Status AlxLp586x_DeInit(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	ALX_LP586x_ASSERT(me->isInit == true);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxLp586x_Handle(AlxLp586x* me)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	ALX_LP586x_ASSERT(me->isInitPeriph == true);
	ALX_LP586x_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		ledNum
  * @retval			false
  * @retval			true
  */
void AlxLp586x_Led_Write(AlxLp586x* me, uint8_t ledNum, bool val)
{
	// Assert
	ALX_LP586x_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_LP586x_ASSERT(ledNum <= 107);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxLp586x_RegStruct_SetAddr(AlxLp586x* me)
{
}
static void AlxLp586x_RegStruct_SetLen(AlxLp586x* me)
{
}
static void AlxLp586x_RegStruct_SetValToZero(AlxLp586x* me)
{
}
static void AlxLp586x_RegStruct_SetValToDefault(AlxLp586x* me)
{
}
Alx_Status AlxLp586x_RegStruct_Write(AlxLp586x* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxLp586x_RegStruct_SetVal(AlxLp586x* me)
{
	(void)me;
	ALX_LP586x_ASSERT(false);	// Implement function in APP
}


#endif	// #if defined(ALX_C_LIB)
