/**
  ******************************************************************************
  * @file		alxPcal6416a.c
  * @brief		Auralix C Library - ALX IO Expander PCAL6416A Module
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
#include "alxPcal6416a.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Types
//******************************************************************************
typedef enum
{
	AlxPcal6416a_Pull_None = 0,
	AlxPcal6416a_Pull_Up = 1,
	AlxPcal6416a_Pull_Down = 2
} AlxPcal6416a_Pull;


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPcal6416a_RegStruct_SetAddr(AlxPcal6416a* me);
static void AlxPcal6416a_RegStruct_SetLen(AlxPcal6416a* me);
static void AlxPcal6416a_RegStruct_SetValToZero(AlxPcal6416a* me);
static void AlxPcal6416a_RegStruct_SetValToDefault(AlxPcal6416a* me);
void AlxPcal6416a_IoPin_ConfigPull(AlxPcal6416a* me, uint8_t port, uint8_t pin, AlxPcal6416a_Pull pull);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me);
Alx_Status AlxPcal6416a_RegStruct_Write(AlxPcal6416a* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPcal6416a_Ctor
(
	AlxPcal6416a* me,
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
	AlxPcal6416a_RegStruct_SetAddr(me);
	AlxPcal6416a_RegStruct_SetLen(me);
	AlxPcal6416a_RegStruct_SetValToZero(me);

	// Info
	me->wasCtorCalled = true;
	me->isInitPeriph = false;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxPcal6416a_InitPeriph(AlxPcal6416a* me)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_DeInitPeriph(AlxPcal6416a* me)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Reset isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Init(AlxPcal6416a* me)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Set register struct values to default
	AlxPcal6416a_RegStruct_SetValToDefault(me);

	// Set register struct values - WEAK
	AlxPcal6416a_RegStruct_SetVal(me);

	// Write register struct - WEAK
	status = AlxPcal6416a_RegStruct_Write(me);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_DeInit(AlxPcal6416a* me)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == true);

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Handle(AlxPcal6416a* me, bool inPort0, bool inPort1, bool outPort0, bool outPort1)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Handle
	if (inPort0)
	{
		status = AlxPcal6416a_Reg_Read(me, &me->reg._00h_InputPort_0);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	}
	if (inPort1)
	{
		status = AlxPcal6416a_Reg_Read(me, &me->reg._01h_InputPort_1);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	}
	if (outPort0)
	{
		status = AlxPcal6416a_Reg_Write(me, &me->reg._02h_OutputPort_0);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	}
	if (outPort1)
	{
		status = AlxPcal6416a_Reg_Write(me, &me->reg._03h_OutputPort_1);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	}

	// Return
	return Alx_Ok;
}
bool AlxPcal6416a_IoPin_Read(AlxPcal6416a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCAL6416A_ASSERT((port == 0) || (port == 1));
	ALX_PCAL6416A_ASSERT((0 <= pin) && (pin <= 8));

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
		ALX_PCAL6416A_ASSERT(false);	// We should never get here
		return false;
	}
}
void AlxPcal6416a_IoPin_Write(AlxPcal6416a* me, uint8_t port, uint8_t pin, bool val)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCAL6416A_ASSERT((port == 0) || (port == 1));
	ALX_PCAL6416A_ASSERT((0 <= pin) && (pin <= 8));

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
		ALX_PCAL6416A_ASSERT(false);	// We should never get here
	}
}
void AlxPcal6416a_IoPin_Set(AlxPcal6416a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCAL6416A_ASSERT((port == 0) || (port == 1));
	ALX_PCAL6416A_ASSERT((0 <= pin) && (pin <= 8));

	// Set
	AlxPcal6416a_IoPin_Write(me, port, pin, true);
}
void AlxPcal6416a_IoPin_Reset(AlxPcal6416a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCAL6416A_ASSERT((port == 0) || (port == 1));
	ALX_PCAL6416A_ASSERT((0 <= pin) && (pin <= 8));

	// Reset
	AlxPcal6416a_IoPin_Write(me, port, pin, false);
}
void AlxPcal6416a_IoPin_Toggle(AlxPcal6416a* me, uint8_t port, uint8_t pin)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care
	ALX_PCAL6416A_ASSERT((port == 0) || (port == 1));
	ALX_PCAL6416A_ASSERT((0 <= pin) && (pin <= 8));

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
		ALX_PCAL6416A_ASSERT(false);	// We should never get here
	}
}
Alx_Status AlxPcal6416a_IoPin_Read_TriState(AlxPcal6416a* me, uint8_t port, uint8_t pin, AlxIoPin_TriState* val)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Local variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	bool valPullUp = false;
	bool valPullDown = false;


	//------------------------------------------------------------------------------
	// Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	AlxPcal6416a_IoPin_ConfigPull(me, port, pin, AlxPcal6416a_Pull_Up);

	// Write pull registers
	status = AlxPcal6416a_Reg_Write(me, &me->reg._48h_PullUpPullDownSel_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._49h_PullUpPullDownSel_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._46h_PullUpPullDownEn_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._47h_PullUpPullDownEn_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Wait
	AlxDelay_ms(1);

	// Read input port registers
	status = AlxPcal6416a_Reg_Read(me, &me->reg._00h_InputPort_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Read(me, &me->reg._01h_InputPort_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Read
	valPullUp = AlxPcal6416a_IoPin_Read(me, port, pin);

	// Disable pull
	AlxPcal6416a_IoPin_ConfigPull(me, port, pin, AlxPcal6416a_Pull_None);

	// Write pull registers
	status = AlxPcal6416a_Reg_Write(me, &me->reg._48h_PullUpPullDownSel_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._49h_PullUpPullDownSel_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._46h_PullUpPullDownEn_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._47h_PullUpPullDownEn_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	AlxPcal6416a_IoPin_ConfigPull(me, port, pin, AlxPcal6416a_Pull_Down);

	// Write pull registers
	status = AlxPcal6416a_Reg_Write(me, &me->reg._48h_PullUpPullDownSel_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._49h_PullUpPullDownSel_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._46h_PullUpPullDownEn_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._47h_PullUpPullDownEn_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Wait
	AlxDelay_ms(1);

	// Read input port registers
	status = AlxPcal6416a_Reg_Read(me, &me->reg._00h_InputPort_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Read(me, &me->reg._01h_InputPort_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Read
	valPullDown = AlxPcal6416a_IoPin_Read(me, port, pin);

	// Disable pull
	AlxPcal6416a_IoPin_ConfigPull(me, port, pin, AlxPcal6416a_Pull_None);

	// Write pull registers
	status = AlxPcal6416a_Reg_Write(me, &me->reg._48h_PullUpPullDownSel_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._49h_PullUpPullDownSel_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._46h_PullUpPullDownEn_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }
	status = AlxPcal6416a_Reg_Write(me, &me->reg._47h_PullUpPullDownEn_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }


	//------------------------------------------------------------------------------
	// Set val
	//------------------------------------------------------------------------------
	if ((valPullUp == true) && (valPullDown == false))
	{
		*val = AlxIoPin_TriState_HiZ;
	}
	else if ((valPullUp == true) && (valPullDown == true))
	{
		*val = AlxIoPin_TriState_Hi;
	}
	else if ((valPullUp == false) && (valPullDown == false))
	{
		*val = AlxIoPin_TriState_Lo;
	}
	else
	{
		*val = AlxIoPin_TriState_Undefined;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Reg_Write(AlxPcal6416a* me, void* reg)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Reg_Read(AlxPcal6416a* me, void* reg)
{
	// Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Read
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPcal6416a_RegStruct_SetAddr(AlxPcal6416a* me)
{
	me->reg._00h_InputPort_0			.addr = 0x00;
	me->reg._01h_InputPort_1			.addr = 0x01;
	me->reg._02h_OutputPort_0			.addr = 0x02;
	me->reg._03h_OutputPort_1			.addr = 0x03;
	me->reg._04h_PolarityInversion_0	.addr = 0x04;
	me->reg._05h_PolarityInversion_1	.addr = 0x05;
	me->reg._06h_Configuration_0		.addr = 0x06;
	me->reg._07h_Configuration_1		.addr = 0x07;
	me->reg._40h_OutputDriveStrength_0	.addr = 0x40;
	me->reg._41h_OutputDriveStrength_0	.addr = 0x41;
	me->reg._42h_OutputDriveStrength_1	.addr = 0x42;
	me->reg._43h_OutputDriveStrength_1	.addr = 0x43;
	me->reg._44h_InputLatch_0			.addr = 0x44;
	me->reg._45h_InputLatch_1			.addr = 0x45;
	me->reg._46h_PullUpPullDownEn_0		.addr = 0x46;
	me->reg._47h_PullUpPullDownEn_1		.addr = 0x47;
	me->reg._48h_PullUpPullDownSel_0	.addr = 0x48;
	me->reg._49h_PullUpPullDownSel_1	.addr = 0x49;
	me->reg._4Ah_IrqMask_0				.addr = 0x4A;
	me->reg._4Bh_IrqMask_1				.addr = 0x4B;
	me->reg._4Ch_IrqStatus_0			.addr = 0x4C;
	me->reg._4Dh_IrqStatus_1			.addr = 0x4D;
	me->reg._4Fh_OutputPortConfig		.addr = 0x4F;
}
static void AlxPcal6416a_RegStruct_SetLen(AlxPcal6416a* me)
{
	me->reg._00h_InputPort_0			.len = sizeof(me->reg._00h_InputPort_0				.val);
	me->reg._01h_InputPort_1			.len = sizeof(me->reg._01h_InputPort_1				.val);
	me->reg._02h_OutputPort_0			.len = sizeof(me->reg._02h_OutputPort_0				.val);
	me->reg._03h_OutputPort_1			.len = sizeof(me->reg._03h_OutputPort_1				.val);
	me->reg._04h_PolarityInversion_0	.len = sizeof(me->reg._04h_PolarityInversion_0		.val);
	me->reg._05h_PolarityInversion_1	.len = sizeof(me->reg._05h_PolarityInversion_1		.val);
	me->reg._06h_Configuration_0		.len = sizeof(me->reg._06h_Configuration_0			.val);
	me->reg._07h_Configuration_1		.len = sizeof(me->reg._07h_Configuration_1			.val);
	me->reg._40h_OutputDriveStrength_0	.len = sizeof(me->reg._40h_OutputDriveStrength_0	.val);
	me->reg._41h_OutputDriveStrength_0	.len = sizeof(me->reg._41h_OutputDriveStrength_0	.val);
	me->reg._42h_OutputDriveStrength_1	.len = sizeof(me->reg._42h_OutputDriveStrength_1	.val);
	me->reg._43h_OutputDriveStrength_1	.len = sizeof(me->reg._43h_OutputDriveStrength_1	.val);
	me->reg._44h_InputLatch_0			.len = sizeof(me->reg._44h_InputLatch_0				.val);
	me->reg._45h_InputLatch_1			.len = sizeof(me->reg._45h_InputLatch_1				.val);
	me->reg._46h_PullUpPullDownEn_0		.len = sizeof(me->reg._46h_PullUpPullDownEn_0		.val);
	me->reg._47h_PullUpPullDownEn_1		.len = sizeof(me->reg._47h_PullUpPullDownEn_1		.val);
	me->reg._48h_PullUpPullDownSel_0	.len = sizeof(me->reg._48h_PullUpPullDownSel_0		.val);
	me->reg._49h_PullUpPullDownSel_1	.len = sizeof(me->reg._49h_PullUpPullDownSel_1		.val);
	me->reg._4Ah_IrqMask_0				.len = sizeof(me->reg._4Ah_IrqMask_0				.val);
	me->reg._4Bh_IrqMask_1				.len = sizeof(me->reg._4Bh_IrqMask_1				.val);
	me->reg._4Ch_IrqStatus_0			.len = sizeof(me->reg._4Ch_IrqStatus_0				.val);
	me->reg._4Dh_IrqStatus_1			.len = sizeof(me->reg._4Dh_IrqStatus_1				.val);
	me->reg._4Fh_OutputPortConfig		.len = sizeof(me->reg._4Fh_OutputPortConfig			.val);
}
static void AlxPcal6416a_RegStruct_SetValToZero(AlxPcal6416a* me)
{
	//me->reg._00h_InputPort_0			.val.raw = 0b00000000;	// MF: Read Only Reg
	//me->reg._01h_InputPort_1			.val.raw = 0b00000000;	// MF: Read Only Reg
	me->reg._02h_OutputPort_0			.val.raw = 0b00000000;
	me->reg._03h_OutputPort_1			.val.raw = 0b00000000;
	me->reg._04h_PolarityInversion_0	.val.raw = 0b00000000;
	me->reg._05h_PolarityInversion_1	.val.raw = 0b00000000;
	me->reg._06h_Configuration_0		.val.raw = 0b00000000;
	me->reg._07h_Configuration_1		.val.raw = 0b00000000;
	me->reg._40h_OutputDriveStrength_0	.val.raw = 0b00000000;
	me->reg._41h_OutputDriveStrength_0	.val.raw = 0b00000000;
	me->reg._42h_OutputDriveStrength_1	.val.raw = 0b00000000;
	me->reg._43h_OutputDriveStrength_1	.val.raw = 0b00000000;
	me->reg._44h_InputLatch_0			.val.raw = 0b00000000;
	me->reg._45h_InputLatch_1			.val.raw = 0b00000000;
	me->reg._46h_PullUpPullDownEn_0		.val.raw = 0b00000000;
	me->reg._47h_PullUpPullDownEn_1		.val.raw = 0b00000000;
	me->reg._48h_PullUpPullDownSel_0	.val.raw = 0b00000000;
	me->reg._49h_PullUpPullDownSel_1	.val.raw = 0b00000000;
	me->reg._4Ah_IrqMask_0				.val.raw = 0b00000000;
	me->reg._4Bh_IrqMask_1				.val.raw = 0b00000000;
	//me->reg._4Ch_IrqStatus_0			.val.raw = 0b00000000;	// MF: Read Only Reg
	//me->reg._4Dh_IrqStatus_1			.val.raw = 0b00000000;	// MF: Read Only Reg
	me->reg._4Fh_OutputPortConfig		.val.raw = 0b00000000;
}
static void AlxPcal6416a_RegStruct_SetValToDefault(AlxPcal6416a* me)
{
	//me->reg._00h_InputPort_0			.val.raw = 0bXXXXXXXX;	// MF: Read Only Reg
	//me->reg._01h_InputPort_1			.val.raw = 0bXXXXXXXX;	// MF: Read Only Reg
	//me->reg._02h_OutputPort_0			.val.raw = 0b11111111;	// TV: We will handle OutputPort config outside of Init
	//me->reg._03h_OutputPort_1			.val.raw = 0b11111111;	// TV: We will handle OutputPort config outside of Init
	me->reg._04h_PolarityInversion_0	.val.raw = 0b00000000;
	me->reg._05h_PolarityInversion_1	.val.raw = 0b00000000;
	me->reg._06h_Configuration_0		.val.raw = 0b11111111;
	me->reg._07h_Configuration_1		.val.raw = 0b11111111;
	me->reg._40h_OutputDriveStrength_0	.val.raw = 0b11111111;
	me->reg._41h_OutputDriveStrength_0	.val.raw = 0b11111111;
	me->reg._42h_OutputDriveStrength_1	.val.raw = 0b11111111;
	me->reg._43h_OutputDriveStrength_1	.val.raw = 0b11111111;
	me->reg._44h_InputLatch_0			.val.raw = 0b00000000;
	me->reg._45h_InputLatch_1			.val.raw = 0b00000000;
	me->reg._46h_PullUpPullDownEn_0		.val.raw = 0b00000000;
	me->reg._47h_PullUpPullDownEn_1		.val.raw = 0b00000000;
	me->reg._48h_PullUpPullDownSel_0	.val.raw = 0b11111111;
	me->reg._49h_PullUpPullDownSel_1	.val.raw = 0b11111111;
	me->reg._4Ah_IrqMask_0				.val.raw = 0b11111111;
	me->reg._4Bh_IrqMask_1				.val.raw = 0b11111111;
	//me->reg._4Ch_IrqStatus_0			.val.raw = 0b00000000;	// MF: Read Only Reg
	//me->reg._4Dh_IrqStatus_1			.val.raw = 0b00000000;	// MF: Read Only Reg
	me->reg._4Fh_OutputPortConfig		.val.raw = 0b00000000;
}
void AlxPcal6416a_IoPin_ConfigPull(AlxPcal6416a* me, uint8_t port, uint8_t pin, AlxPcal6416a_Pull pull)
{
	// If port 0 is used
	if (port == 0)
	{
		if (pull == AlxPcal6416a_Pull_None)
		{
			// Disable pull
			me->reg._46h_PullUpPullDownEn_0.val.raw &= ~(1 << pin);
		}
		else
		{
			// Enable pull
			me->reg._46h_PullUpPullDownEn_0.val.raw |= 1 << pin;

			// Config pull
			if (pull == AlxPcal6416a_Pull_Up)
			{
				me->reg._48h_PullUpPullDownSel_0.val.raw |= 1 << pin;
			}
			else if (pull == AlxPcal6416a_Pull_Down)
			{
				me->reg._48h_PullUpPullDownSel_0.val.raw &= ~(1 << pin);
			}
			else
			{
				ALX_PCAL6416A_ASSERT(false);	// We should not get here
			}
		}
	}

	// If port 1 is used
	else if (port == 1)
	{
		if (pull == AlxPcal6416a_Pull_None)
		{
			// Disable pull
			me->reg._47h_PullUpPullDownEn_1.val.raw &= ~(1 << pin);
		}
		else
		{
			// Enable pull
			me->reg._47h_PullUpPullDownEn_1.val.raw |= 1 << pin;

			// Config pull
			if (pull == AlxPcal6416a_Pull_Up)
			{
				me->reg._49h_PullUpPullDownSel_1.val.raw |= 1 << pin;
			}
			else if (pull == AlxPcal6416a_Pull_Down)
			{
				me->reg._49h_PullUpPullDownSel_1.val.raw &= ~(1 << pin);
			}
			else
			{
				ALX_PCAL6416A_ASSERT(false);	// We should not get here
			}
		}
	}

	// Assert
	else
	{
		ALX_PCAL6416A_ASSERT(false);	// We should never get here
	}
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me)
{
	(void)me;
	ALX_PCAL6416A_TRACE("Define function!");
	ALX_PCAL6416A_ASSERT(false);
}
ALX_WEAK Alx_Status AlxPcal6416a_RegStruct_Write(AlxPcal6416a* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	//status = AlxPcal6416a_Reg_Write(me, &me->reg._00h_InputPort_0);		// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._01h_InputPort_1);		// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._02h_OutputPort_0);		// TV: We will handle OutputPort config outside of Init
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._03h_OutputPort_1);		// TV: We will handle OutputPort config outside of Init
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._04h_PolarityInversion_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._05h_PolarityInversion_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._06h_Configuration_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._07h_Configuration_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._40h_OutputDriveStrength_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._41h_OutputDriveStrength_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._42h_OutputDriveStrength_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._43h_OutputDriveStrength_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._44h_InputLatch_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._45h_InputLatch_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._46h_PullUpPullDownEn_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._47h_PullUpPullDownEn_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._48h_PullUpPullDownSel_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._49h_PullUpPullDownSel_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._4Ah_IrqMask_0);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._4Bh_IrqMask_1);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._4Ch_IrqStatus_0);		// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._4Dh_IrqStatus_1);		// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	status = AlxPcal6416a_Reg_Write(me, &me->reg._4Fh_OutputPortConfig);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
