/**
  ******************************************************************************
  * @file		alxIna228.c
  * @brief		Auralix C Library - ALX Current Monitor INA228 Module
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
#include "alxIna228.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIna228_RegStruct_SetAddr(AlxIna228* me);
static void AlxIna228_RegStruct_SetLen(AlxIna228* me);
static void AlxIna228_RegStruct_SetValToZero(AlxIna228* me);
static void AlxIna228_RegStruct_SetToDefault(AlxIna228* me);
static Alx_Status AlxIna228_Reg_Write(AlxIna228* me, void* reg);
static Alx_Status AlxIna228_Reg_Write_All(AlxIna228* me);
static Alx_Status AlxIna228_Reg_Read(AlxIna228* me, void* reg);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxVeml6040_RegStruct_SetVal(AlxIna228* me);




//******************************************************************************
// Constructor
//******************************************************************************
void AlxIna228_Ctor
(
	AlxIna228* me,
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
Alx_Status AlxIna228_InitPeriph(AlxIna228* me)
{
	// I2C Init
}
Alx_Status AlxIna228_DeInitPeriph(AlxIna228* me)
{
	// I2C DeInit
}
Alx_Status AlxIna228_Init(AlxIna228* me)
{
	// Set needed configuration
	//0h CONFIG Configuration 16 Go
	//1h ADC_CONFIG ADC Configuration 16 Go
	//2h SHUNT_CAL Shunt Calibration 16 Go
	//3h SHUNT_TEMPCO Shunt Temperature Coefficient 16 Go

	// Set limits -> for start this will all be default, we will not use this
	// Bh DIAG_ALRT Diagnostic Flags and Alert 16 Go
	//Ch SOVL Shunt Overvoltage Threshold 16 Go
	//Dh SUVL Shunt Undervoltage Threshold 16 Go
	//Eh BOVL Bus Overvoltage Threshold 16 Go
	//Fh BUVL Bus Undervoltage Threshold 16 Go
	//10h TEMP_LIMIT Temperature Over-Limit Threshold 16 Go
	//11h PWR_LIMIT Power Over-Limit Threshold 16 Go

	// Can read & print IDs
	//3Eh MANUFACTURER_ID Manufacturer ID 16 Go
	//3Fh DEVICE_ID Device ID 16 Go
}
Alx_Status AlxIna228_DeInit(AlxIna228* me)
{
	// We can just reset IC, so after that we know all registers are set to default values
}
Alx_Status AlxIna228_GetShuntVoltage_V(AlxIna228* me, float* voltage_V)
{
	//4h VSHUNT Shunt Voltage Measurement 24 Go
	// Differential voltage measured across the shunt output. Two's complement value.
	// Return Volts (positive or negative, depending in which direction is voltage applied on shunt)
	// Also take into account how is IC configured:
	// 312.5 nV/LSB when ADCRANGE = 0
	// 78.125 nV/LSB when ADCRANGE = 1
}
Alx_Status AlxIna228_GetBusVoltage_V(AlxIna228* me, float* voltage_V)
{
	//5h VBUS Bus Voltage Measurement 24 Go
	// Bus voltage output. Two's complement value, however always positive.
	// Return Volts (always positive value)
	// Conversion factor: 195.3125 uV/LSB
}
Alx_Status AlxIna228_GetTemp_degC(AlxIna228* me, float* temp_degC)
{
	//6h DIETEMP Temperature Measurement 16 Go
	// Internal die temperature measurement. Two's complement value.
	// return DegreeCelsius (positive or negative)
	// Conversion factor: 7.8125 mDegC/LSB
}
Alx_Status AlxIna228_GetCurrent_A(AlxIna228* me, float* current_A)
{
	//7h CURRENT Current Result 24 Go
}
Alx_Status AlxIna228_GetPower_W(AlxIna228* me, float* power_W)
{
	//8h POWER Power Result 24 Go
}
Alx_Status AlxIna228_GetEnergy_J(AlxIna228* me, float* energy_J)
{
	//9h ENERGY Energy Result 40 Go
}
Alx_Status AlxIna228_GetCharge_C(AlxIna228* me, float* charge_C)
{
	//Ah CHARGE Charge Result 40 Go
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIna228_RegStruct_SetAddr(AlxIna228* me)
{
	me->reg._0x00_CONF		.addr = 0x00;
	me->reg._0x08_R_DATA	.addr = 0x08;
	me->reg._0x09_G_DATA	.addr = 0x09;
	me->reg._0x0A_B_DATA	.addr = 0x0A;
	me->reg._0x0B_W_DATA	.addr = 0x0B;
}
static void AlxIna228_RegStruct_SetLen(AlxIna228* me)
{
	me->reg._0x00_CONF		.len = sizeof(me->reg._0x00_CONF	.val);
	me->reg._0x08_R_DATA	.len = sizeof(me->reg._0x08_R_DATA	.val);
	me->reg._0x09_G_DATA	.len = sizeof(me->reg._0x09_G_DATA	.val);
	me->reg._0x0A_B_DATA	.len = sizeof(me->reg._0x0A_B_DATA	.val);
	me->reg._0x0B_W_DATA	.len = sizeof(me->reg._0x0B_W_DATA	.val);
}
static void AlxIna228_RegStruct_SetValToZero(AlxIna228* me)
{
	me->reg._0x00_CONF		.val.raw = 0x0000;
	me->reg._0x08_R_DATA	.val.raw = 0x0000;
	me->reg._0x09_G_DATA	.val.raw = 0x0000;
	me->reg._0x0A_B_DATA	.val.raw = 0x0000;
	me->reg._0x0B_W_DATA	.val.raw = 0x0000;
}
static void AlxIna228_RegStruct_SetToDefault(AlxIna228* me)
{
	me->reg._0x00_CONF		.val.raw = 0x0001;
	me->reg._0x08_R_DATA	.val.raw = 0x0000;
	me->reg._0x09_G_DATA	.val.raw = 0x0000;
	me->reg._0x0A_B_DATA	.val.raw = 0x0000;
	me->reg._0x0B_W_DATA	.val.raw = 0x0000;
}

static Alx_Status AlxIna228_Reg_Write(AlxIna228* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_INA228_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxIna228_Reg_Write_All(AlxIna228* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	status = AlxIna228_Reg_Write(me, &me->reg._0x00_CONF	);
	if (status != Alx_Ok) { ALX_INA228_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxIna228_Reg_Read(AlxIna228* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);

	// Read
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, regValPtr, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_INA228_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxVeml6040_RegStruct_SetVal(AlxIna228* me)
{
	// Local variables
	(void)me;

	// Assert
	ALX_INA228_TRACE("Err");
	ALX_INA228_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
