/**
  ******************************************************************************
  * @file		alxPca9539a.c
  * @brief		Auralix C Library - ALX IO Expander PCA9539A Module
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
#include "alxPca9539a.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************
typedef enum
{
	AlxPca9539a_Pull_None = 0,
	AlxPca9539a_Pull_Up = 1,
	AlxPca9539a_Pull_Down = 2
} AlxPca9539a_Pull;


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPca9539a_RegStruct_SetAddr(AlxPca9539a* me);
static void AlxPca9539a_RegStruct_SetLen(AlxPca9539a* me);
static void AlxPca9539a_RegStruct_SetValToDefault(AlxPca9539a* me);
static void AlxPca9539a_RegStruct_SetValToZero(AlxPca9539a* me);

void AlxPca9539a_IoPin_ConfigPull(AlxPca9539a* me, uint8_t port, uint8_t pin, AlxPca9539a_Pull pull);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPca9539a_RegStruct_SetVal(AlxPca9539a* me);
Alx_Status AlxPca9539a_RegStruct_Write(AlxPca9539a* me);


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
void AlxPca9539a_Ctor
(
	AlxPca9539a* me,
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
	AlxPca9539a_RegStruct_SetAddr(me);
	AlxPca9539a_RegStruct_SetLen(me);
	AlxPca9539a_RegStruct_SetValToZero(me);

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
Alx_Status AlxPca9539a_InitPeriph(AlxPca9539a* me)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

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
Alx_Status AlxPca9539a_DeInitPeriph(AlxPca9539a* me)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

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
Alx_Status AlxPca9539a_Init(AlxPca9539a* me)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInit == false);

	// Multiple drivers use the same I2C instance, so it could already be initialized.
	if (me->i2c->isInit == false)
	{
		AlxPca9539a_InitPeriph(me);
	}
	else
	{
		me->isInitPeriph = true;
	}

	// Local variables
	Alx_Status status = Alx_Err;

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	// Set register struct values to default
	AlxPca9539a_RegStruct_SetValToDefault(me);

	// Set register struct values - WEAK
	//AlxPca9539a_RegStruct_SetVal(me);

	// Write register struct - WEAK
	status = AlxPca9539a_RegStruct_Write(me);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

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
Alx_Status AlxPca9539a_DeInit(AlxPca9539a* me)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == true);
	ALX_PCA9539A_ASSERT(me->isInit == true);

	AlxPca9539a_DeInitPeriph(me);
	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief Poll function
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPca9539a_Handle(AlxPca9539a* me)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == true);
	ALX_PCA9539A_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t mask;

	// Handle

	status = AlxPca9539a_Reg_Read(me, &me->reg._00h_InputPort_0);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Read(me, &me->reg._01h_InputPort_1);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	if (me->_OpenDrain_0.raw != 0b00000000)
	{
		mask = me->_OpenDrain_0.raw & me->reg._02h_OutputPort_0.val.raw;
		me->reg._06h_Configuration_0.val.raw = (me->reg._06h_Configuration_0.val.raw & ~me->_OpenDrain_0.raw) | mask;

		status = AlxPca9539a_Reg_Write(me, &me->reg._06h_Configuration_0);
		if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }
	}

	if (me->_OpenDrain_1.raw != 0b00000000)
	{
		mask = me->_OpenDrain_1.raw & me->reg._03h_OutputPort_1.val.raw;
		me->reg._07h_Configuration_1.val.raw = (me->reg._07h_Configuration_1.val.raw & ~me->_OpenDrain_1.raw) | mask;

		status = AlxPca9539a_Reg_Write(me, &me->reg._07h_Configuration_1);
		if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }
	}

	status = AlxPca9539a_Reg_Write(me, &me->reg._02h_OutputPort_0);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Write(me, &me->reg._03h_OutputPort_1);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief GPIO interrupt handle
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPca9539a_irqHandle(AlxPca9539a* me)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == true);
	ALX_PCA9539A_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Ok;

	status = AlxPca9539a_Reg_Read(me, &me->reg._00h_InputPort_0);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Read(me, &me->reg._01h_InputPort_1);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

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
bool AlxPca9539a_IoPin_Read(AlxPca9539a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCA9539A_ASSERT((port == 0) || (port == 1));
	ALX_PCA9539A_ASSERT((0 <= pin) && (pin <= 8));

	// Read if port 0 is used
	if (port == 0)
	{
		if (me->reg._00h_InputPort_0.val.raw & (1 << pin))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// Read if port 1 is used
	else if (port == 1)
	{
		if (me->reg._01h_InputPort_1.val.raw & (1 << pin))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// Assert
	else
	{
		ALX_PCA9539A_ASSERT(false);	// We should never get here
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
uint8_t AlxPca9539a_Read_Port_Raw(AlxPca9539a* me, uint8_t port)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCA9539A_ASSERT((port == 0) || (port == 1));

	if(port == 0)
	{
		return me->reg._00h_InputPort_0.val.raw;
	}

	else if(port == 1)
	{
		return me->reg._01h_InputPort_1.val.raw;
	}

	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		val
  */
void AlxPca9539a_IoPin_Write(AlxPca9539a* me, uint8_t port, uint8_t pin, bool val)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCA9539A_ASSERT((port == 0) || (port == 1));
	ALX_PCA9539A_ASSERT((0 <= pin) && (pin <= 8));

	// Write if port 0 is used
	if (port == 0)
	{
		if (val == true)
		{
			me->reg._02h_OutputPort_0.val.raw |=  (1 << pin);
		}
		else
		{
			me->reg._02h_OutputPort_0.val.raw &= ~(1 << pin);
		}
	}

	// Write if port 1 is used
	else if (port == 1)
	{
		if (val == true)
		{
			me->reg._03h_OutputPort_1.val.raw |=  (1 << pin);
		}
		else
		{
			me->reg._03h_OutputPort_1.val.raw &= ~(1 << pin);
		}
	}

	// Assert
	else
	{
		ALX_PCA9539A_ASSERT(false);	// We should never get here
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  */
void AlxPca9539a_IoPin_Set(AlxPca9539a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCA9539A_ASSERT((port == 0) || (port == 1));
	ALX_PCA9539A_ASSERT((0 <= pin) && (pin <= 8));

	// Set
	AlxPca9539a_IoPin_Write(me, port, pin, true);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  */
void AlxPca9539a_IoPin_Reset(AlxPca9539a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCA9539A_ASSERT((port == 0) || (port == 1));
	ALX_PCA9539A_ASSERT((0 <= pin) && (pin <= 8));

	// Reset
	AlxPca9539a_IoPin_Write(me, port, pin, false);
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  */
void AlxPca9539a_IoPin_Toggle(AlxPca9539a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCA9539A_ASSERT((port == 0) || (port == 1));
	ALX_PCA9539A_ASSERT((0 <= pin) && (pin <= 8));

	// Write if port 0 is used
	if (port == 0)
	{
		me->reg._02h_OutputPort_0.val.raw ^= (1 << pin);
	}

	// Write if port 1 is used
	else if (port == 1)
	{
		me->reg._03h_OutputPort_1.val.raw ^= (1 << pin);
	}

	// Assert
	else
	{
		ALX_PCA9539A_ASSERT(false);	// We should never get here
	}
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		reg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPca9539a_Reg_Write(AlxPca9539a* me, void* reg)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		reg
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxPca9539a_Reg_Read(AlxPca9539a* me, void* reg)
{
	// Assert
	ALX_PCA9539A_ASSERT(me->wasCtorCalled == true);
	ALX_PCA9539A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Read
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPca9539a_RegStruct_SetAddr(AlxPca9539a* me)
{
	me->reg._00h_InputPort_0			.addr = 0x00;
	me->reg._01h_InputPort_1			.addr = 0x01;
	me->reg._02h_OutputPort_0			.addr = 0x02;
	me->reg._03h_OutputPort_1			.addr = 0x03;
	me->reg._04h_PolarityInversion_0	.addr = 0x04;
	me->reg._05h_PolarityInversion_1	.addr = 0x05;
	me->reg._06h_Configuration_0		.addr = 0x06;
	me->reg._07h_Configuration_1		.addr = 0x07;
}
static void AlxPca9539a_RegStruct_SetLen(AlxPca9539a* me)
{
	me->reg._00h_InputPort_0			.len = sizeof(me->reg._00h_InputPort_0				.val);
	me->reg._01h_InputPort_1			.len = sizeof(me->reg._01h_InputPort_1				.val);
	me->reg._02h_OutputPort_0			.len = sizeof(me->reg._02h_OutputPort_0				.val);
	me->reg._03h_OutputPort_1			.len = sizeof(me->reg._03h_OutputPort_1				.val);
	me->reg._04h_PolarityInversion_0	.len = sizeof(me->reg._04h_PolarityInversion_0		.val);
	me->reg._05h_PolarityInversion_1	.len = sizeof(me->reg._05h_PolarityInversion_1		.val);
	me->reg._06h_Configuration_0		.len = sizeof(me->reg._06h_Configuration_0			.val);
	me->reg._07h_Configuration_1		.len = sizeof(me->reg._07h_Configuration_1			.val);

}

static void AlxPca9539a_RegStruct_SetValToZero(AlxPca9539a* me)
{
	//me->reg._00h_InputPort_0			.val.raw = 0bXXXXXXXX;	// MF: Read Only Reg
	//me->reg._01h_InputPort_1			.val.raw = 0bXXXXXXXX;	// MF: Read Only Reg
	me->reg._02h_OutputPort_0.val.raw = 0b00000000;	// TV: We will handle OutputPort config outside of Init
	me->reg._03h_OutputPort_1.val.raw = 0b00000000; // TV: We will handle OutputPort config outside of Init
	me->reg._04h_PolarityInversion_0.val.raw = 0b00000000;
	me->reg._05h_PolarityInversion_1.val.raw = 0b00000000;
	me->reg._06h_Configuration_0.val.raw = 0b00000000;
	me->reg._07h_Configuration_1.val.raw = 0b00000000;
	me->_OpenDrain_0.raw = 0b00000000;
	me->_OpenDrain_1.raw = 0b00000000;
}

static void AlxPca9539a_RegStruct_SetValToDefault(AlxPca9539a* me)
{
	//me->reg._00h_InputPort_0			.val.raw = 0bXXXXXXXX;	// MF: Read Only Reg
	//me->reg._01h_InputPort_1			.val.raw = 0bXXXXXXXX;	// MF: Read Only Reg
	//me->reg._02h_OutputPort_0			.val.raw = 0b11111111;	// TV: We will handle OutputPort config outside of Init
	//me->reg._03h_OutputPort_1			.val.raw = 0b11111111;	// TV: We will handle OutputPort config outside of Init
	me->reg._04h_PolarityInversion_0	.val.raw = 0b00000000;
	me->reg._05h_PolarityInversion_1	.val.raw = 0b00000000;
	me->reg._06h_Configuration_0		.val.raw = 0b11000000;
	me->reg._07h_Configuration_1		.val.raw = 0b11111111;
	me->_OpenDrain_0.raw = 0b01000000;
	me->_OpenDrain_1.raw = 0b00000000;
}

//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxPca9539a_RegStruct_SetVal(AlxPca9539a* me)
{
	(void)me;
	ALX_PCA9539A_TRACE("Define function!");
	ALX_PCA9539A_ASSERT(false);
}
ALX_WEAK Alx_Status AlxPca9539a_RegStruct_Write(AlxPca9539a* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	//status = AlxPca9539a_Reg_Write(me, &me->reg._00h_InputPort_0);		// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	//status = AlxPca9539a_Reg_Write(me, &me->reg._01h_InputPort_1);		// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	//status = AlxPca9539a_Reg_Write(me, &me->reg._02h_OutputPort_0);		// TV: We will handle OutputPort config outside of Init
	//if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	//status = AlxPca9539a_Reg_Write(me, &me->reg._03h_OutputPort_1);		// TV: We will handle OutputPort config outside of Init
	//if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Write(me, &me->reg._04h_PolarityInversion_0);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Write(me, &me->reg._05h_PolarityInversion_1);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Write(me, &me->reg._06h_Configuration_0);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	status = AlxPca9539a_Reg_Write(me, &me->reg._07h_Configuration_1);
	if (status != Alx_Ok) { ALX_PCA9539A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
