/**
  ******************************************************************************
  * @file		alxPcal6416a.c
  * @brief		Auralix C Library - ALX IO Expander PCAL6416A Module
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
#include "alxPcal6416a.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPcal6416a_RegStruct_SetAddr(AlxPcal6416a* me);
static void AlxPcal6416a_RegStruct_SetLen(AlxPcal6416a* me);
static void AlxPcal6416a_RegStruct_SetValToZero(AlxPcal6416a* me);
static void AlxPcal6416a_RegStruct_SetValToDefault(AlxPcal6416a* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me);
Alx_Status AlxPcal6416a_Reg_WriteAll(AlxPcal6416a* me);


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
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// #4 Set isInitPeriph
	me->isInitPeriph = true;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_DeInitPeriph(AlxPcal6416a* me)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// #4 Reset isInitPeriph
	me->isInitPeriph = false;

	// #5 Return OK
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Init(AlxPcal6416a* me)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == false);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Check if slave is ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// #4 Set register struct values to default
	AlxPcal6416a_RegStruct_SetValToDefault(me);

	// #5 Set registers values - WEAK
	AlxPcal6416a_RegStruct_SetVal(me);

	// #6 Write all registers
	status = AlxPcal6416a_Reg_WriteAll(me);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status; }

	// #7 Set isInit
	me->isInit = true;

	// #8 Return OK
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_DeInit(AlxPcal6416a* me)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == true);

	// #2 DO NOTHING

	// #3 Reset isInit
	me->isInit = false;

	// #4 Return OK
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Handle(AlxPcal6416a* me, bool inputPort0, bool inputPort1, bool outputPort0, bool outputPort1)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	ALX_PCAL6416A_ASSERT(me->isInitPeriph == true);
	ALX_PCAL6416A_ASSERT(me->isInit == true);

	// #2 Prepare variables
	Alx_Status status = Alx_Err;

	// #3 Handle
	if (inputPort0)
	{
		status = AlxPcal6416a_Reg_Read(me, &me->reg._00h_InputPort_0);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status;}
	}
	if (inputPort1)
	{
		status = AlxPcal6416a_Reg_Read(me, &me->reg._01h_InputPort_1);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status;}
	}
	if (outputPort0)
	{
		status = AlxPcal6416a_Reg_Write(me, &me->reg._02h_OutputPort_0);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status;}
	}
	if (outputPort1)
	{
		status = AlxPcal6416a_Reg_Write(me, &me->reg._03h_OutputPort_1);
		if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status;}
	}

	// #4 Return OK
	return Alx_Ok;
}
bool AlxPcal6416a_IoPin_Read(AlxPcal6416a* me, AlxPcal6416a_PortPin pin)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care

	// #2.1 Read if Port0 is used
	if (!(pin & (1 << 3)))
	{
		if (me->reg._00h_InputPort_0.val.raw & (1U << pin))	{ return true; }
		else												{ return false; }
	}

	// #2.2 Read if Port1 is used
	if (pin & (1 << 3))
	{
		if (me->reg._01h_InputPort_1.val.raw & (1U << pin))	{ return true; }
		else												{ return false; }
	}

	// #3 Asset
	ALX_PCAL6416A_ASSERT(false); // We sould not get here
	return false;
}
void AlxPcal6416a_IoPin_Write(AlxPcal6416a* me, AlxPcal6416a_PortPin pin, bool val)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care

	// #2.1 Write if Port0 is used
	if (!(pin & (1 << 3)))
	{
		if (val == true)	{ me->reg._02h_OutputPort_0.val.raw |=  (1U << pin); return; }
		else				{ me->reg._02h_OutputPort_0.val.raw &= ~(1U << pin); return; }
	}

	// #2.2 Write if Port1 is used
	if (pin & (1 << 3))
	{
		if (val == true)	{ me->reg._03h_OutputPort_1.val.raw |=  (1U << pin); return; }
		else				{ me->reg._03h_OutputPort_1.val.raw &= ~(1U << pin); return; }
	}

	// #3 Asset
	ALX_PCAL6416A_ASSERT(false); // We sould not get here
}
void AlxPcal6416a_IoPin_Set(AlxPcal6416a* me, AlxPcal6416a_PortPin pin)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care

	// #2.1 Write if Port0 is used
	if (!(pin & (1 << 3)))	{ me->reg._02h_OutputPort_0.val.raw |= (1U << pin); return; }

	// #2.2 Write if Port1 is used
	if (pin & (1 << 3))		{ me->reg._03h_OutputPort_1.val.raw |= (1U << pin); return; }

	// #3 Asset
	ALX_PCAL6416A_ASSERT(false); // We sould not get here
}
void AlxPcal6416a_IoPin_Reset(AlxPcal6416a* me, AlxPcal6416a_PortPin pin)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care

	// #2.1 Write if Port0 is used
	if (!(pin & (1 << 3)))	{ me->reg._02h_OutputPort_0.val.raw &= ~(1U << pin); return; }

	// #2.2 Write if Port1 is used
	if (pin & (1 << 3))		{ me->reg._03h_OutputPort_1.val.raw &= ~(1U << pin); return; }

	// #3 Asset
	ALX_PCAL6416A_ASSERT(false); // We sould not get here
}
void AlxPcal6416a_IoPin_Toggle(AlxPcal6416a* me, AlxPcal6416a_PortPin pin)
{
	// #1 Assert
	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
	// isInitPeriph -> Don't care
	// isInit -> Don't care

	// #2.1 Write if Port0 is used
	if (!(pin & (1 << 3)))	{ me->reg._02h_OutputPort_0.val.raw ^= (1U << pin); return; }

	// #2.2 Write if Port1 is used
	if (pin & (1 << 3))		{ me->reg._03h_OutputPort_1.val.raw ^= (1U << pin); return; }

	// #3 Asset
	ALX_PCAL6416A_ASSERT(false);	// We sould not get here
}
Alx_Status AlxPcal6416a_Reg_Write(AlxPcal6416a* me, void* reg)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #2 Write address and data
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err"); return status;}

	// #3 Return OK
	return Alx_Ok;
}
Alx_Status AlxPcal6416a_Reg_Read(AlxPcal6416a* me, void* reg)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// #2 Write address and Read data
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if(status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err") ; return status ; }

	// #3 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPcal6416a_RegStruct_SetAddr(AlxPcal6416a* me)
{
	// #1 Set Address
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
	// #1 Set Lenght
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
	// #1 Set values to zero
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
	// #1 Set values to default
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


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK Alx_Status AlxPcal6416a_Reg_WriteAll(AlxPcal6416a* me)
{
	// #1 Prepare variables
	Alx_Status status = Alx_Err;

	// #2 Write Registers
	//status = AlxPcal6416a_Reg_Write(me, &me->reg._00h_InputPort_0			);							// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_00h_InputPort_0			"); return status;}

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._01h_InputPort_1			);							// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_01h_InputPort_1			"); return status;}

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._02h_OutputPort_0			);						// TV: We will handle OutputPort config outside of Init
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_02h_OutputPort_0			"); return status;}

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._03h_OutputPort_1			);						// TV: We will handle OutputPort config outside of Init
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_03h_OutputPort_1			"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._04h_PolarityInversion_0	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_04h_PolarityInversion_0	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._05h_PolarityInversion_1	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_05h_PolarityInversion_1	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._06h_Configuration_0		);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_06h_Configuration_0		"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._07h_Configuration_1		);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_00h_InputPort_0			"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._40h_OutputDriveStrength_0	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_40h_OutputDriveStrength_0	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._41h_OutputDriveStrength_0	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_41h_OutputDriveStrength_0	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._42h_OutputDriveStrength_1	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_42h_OutputDriveStrength_1	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._43h_OutputDriveStrength_1	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_43h_OutputDriveStrength_1	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._44h_InputLatch_0			);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_44h_InputLatch_0			"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._45h_InputLatch_1			);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_45h_InputLatch_1			"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._46h_PullUpPullDownEn_0	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_46h_PullUpPullDownEn_0		"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._47h_PullUpPullDownEn_1	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_47h_PullUpPullDownEn_1		"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._48h_PullUpPullDownSel_0	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_48h_PullUpPullDownSel_0	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._49h_PullUpPullDownSel_1	);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_49h_PullUpPullDownSel_1	"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._4Ah_IrqMask_0				);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_4Ah_IrqMask_0				"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._4Bh_IrqMask_1				);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_4Bh_IrqMask_1				"); return status;}

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._4Ch_IrqStatus_0			);							// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_4Ch_IrqStatus_0			"); return status;}

	//status = AlxPcal6416a_Reg_Write(me, &me->reg._4Dh_IrqStatus_1			);							// MF: Read Only Reg
	//if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_4Dh_IrqStatus_1			"); return status;}

	status = AlxPcal6416a_Reg_Write(me, &me->reg._4Fh_OutputPortConfig		);
	if (status != Alx_Ok) { ALX_PCAL6416A_TRACE("Err_4Fh_OutputPortConfig		"); return status;}

	// #3 Return Ok
	return Alx_Ok;
}
ALX_WEAK void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me)
{
	(void)me;
	ALX_PCAL6416A_TRACE("Define function!");
	ALX_PCAL6416A_ASSERT(false);
}


#endif // #if defined(ALX_C_LIB)



















//void AlxPcal6416a_IoPin_SetMode(AlxPcal6416a* me, AlxPcal6416a_PortPin pin, AlxPcal6416a_Mode mode)
//{
//	// #1 Assert
//	ALX_PCAL6416A_ASSERT(me->isInit == true);
//	ALX_PCAL6416A_ASSERT(me->wasCtorCalled == true);
//
//	// #2.1 Write if Port0 is used
//	if (!(pin & (1 << 3)))
//	{
//		if (mode == AlxPcal6416a_Inact)
//		{
//			me->reg._46h_PullUpPullDownEn_0.val.raw &= ~(1U << pin);	// MF: Disables Pull-up/pull-down
//			return;
//		}
//		else
//		{
//			me->reg._46h_PullUpPullDownEn_0.val.raw |= 1U << pin;		// MF: Enables Pull-up/pull-down
//
//			if (mode == AlxPcal6416a_PullUp)	{ me->reg._48h_PullUpPullDownSel_0.val.raw |=  (1U << pin);	return; }	// MF: Sets PullUp
//			else								{ me->reg._48h_PullUpPullDownSel_0.val.raw &= ~(1U << pin);	return; }	// MF: Sets PullDown
//		}
//	}
//
//	// #2.2 Write if Port1 is used
//	if (pin & (1 << 3))
//	{
//		if (mode == AlxPcal6416a_Inact)
//		{
//			me->reg._47h_PullUpPullDownEn_1.val.raw &= ~(1U << pin); // MF: Disables Pull-up/pull-down
//			return;
//		}
//		else
//		{
//			me->reg._47h_PullUpPullDownEn_1.val.raw |= (1U << pin); // MF: Enables Pull-up/pull-down
//
//			if (mode == AlxPcal6416a_PullUp)	{ me->reg._49h_PullUpPullDownSel_1.val.raw |=  (1U << pin);	return;}	// MF: Sets PullUp
//			else								{ me->reg._49h_PullUpPullDownSel_1.val.raw &= ~(1U << pin);	return;}	// MF: Sets PullDown
//		}
//	}
//
//	// #3 Asset
//	ALX_PCAL6416A_ASSERT(false); // We sould not get here
//}
