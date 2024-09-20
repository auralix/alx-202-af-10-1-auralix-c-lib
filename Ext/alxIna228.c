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
static void AlxIna228_CurrentLsbFactor(AlxIna228* me);
static void AlxIna228_ShuntFactor(AlxIna228* me);
static Alx_Status AlxIna228_TraceId(AlxIna228* me);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxIna228_RegStruct_SetVal(AlxIna228* me);


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
	uint16_t i2cTimeout_ms,
	AlxIna228_RegEnum_0x00_ADCRANGE adcRange,
	float shuntRes_Ohm,
	float shuntResTemp_ppmPerDegC
)
{
	// Parameters
	me->i2c = i2c;
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;
	me->adcRange = adcRange;
	me->shuntRes_Ohm = shuntRes_Ohm;
	me->shuntResTemp_ppmPerDegC = shuntResTemp_ppmPerDegC;

	// Variables
	AlxIna228_RegStruct_SetAddr(me);
	AlxIna228_RegStruct_SetLen(me);
	AlxIna228_RegStruct_SetValToZero(me);

	// Info
	me->wasCtorCalled = true;
	me->isInitPeriph = false;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxIna228_InitPeriph(AlxIna228* me)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == false);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Init I2C
	status = AlxI2c_Init(me->i2c);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_AlxI2c_Init"); return status; }

	// Check if slave ready
	status = AlxI2c_Master_IsSlaveReady(me->i2c, me->i2cAddr, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_AlxI2c_Master_IsSlaveReady"); return status; }

	// Set isInitPeriph
	me->isInitPeriph = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_DeInitPeriph(AlxIna228* me)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	// isInit -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit I2C
	status = AlxI2c_DeInit(me->i2c);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_AlxI2c_DeInit"); return status; }

	// Reset isInitPeriph
	me->isInitPeriph = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_Init(AlxIna228* me)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == false);

	// Set needed configuration
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxIna228_RegStruct_SetToDefault(me);

	// Set Current LSB Factor
	AlxIna228_CurrentLsbFactor(me);

	// Set Shunt Factor
	AlxIna228_ShuntFactor(me);

	// Set register values - WEAK
	AlxIna228_RegStruct_SetVal(me);

	// Can read & print IDs
	AlxIna228_TraceId(me);

	// Write registers
	status = AlxIna228_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_AlxIna228_Reg_Write_All"); return status; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_DeInit(AlxIna228* me)
{
	// We can just reset IC, so after that we know all registers are set to default values

	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Set needed configuration
	Alx_Status status = Alx_Err;

	// Set register struct values to default
	AlxIna228_RegStruct_SetToDefault(me);

	// Write registers
	status = AlxIna228_Reg_Write_All(me);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_AlxIna228_Reg_Write_All"); return status; }

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetShuntVoltage_V(AlxIna228* me, float* voltage_V)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x04_VSHUNT);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x04_VSHUNT"); return status; }

	// Set Conversion shunt factor value 312.5e-9
	const float CONVERSION_SHUNT_FACTOR_312_5En9 = 312.5e-9;

	// Set Conversion shunt factorvalue 78.125e-9
	const float CONVERSION_SHUNT_FACTOR_78_125En9 = 78.125e-9;

	// Set Conversion shunt factor. Depending on (ADCRANGE) adc range: 0 or 1.
	const float CONVERSION_SHUNT_FACTOR = (me->reg._0x00_CONFIG.val.ADCRANGE == AdcRange_163_84_mV) ? CONVERSION_SHUNT_FACTOR_312_5En9 : CONVERSION_SHUNT_FACTOR_78_125En9;

	// Set
	*voltage_V = me->reg._0x04_VSHUNT.val.VSHUNT_nVoltage * CONVERSION_SHUNT_FACTOR;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetBusVoltage_V(AlxIna228* me, float* voltage_V)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x05_VBUS);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x05_VBUS"); return status; }

	// Set Conversion vbus factor
	const float CONVERSION_VBUS_FACTOR = 195.3125e-6;

	// Set
	*voltage_V = me->reg._0x05_VBUS.val.VBUS_uVoltage * CONVERSION_VBUS_FACTOR;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetTemp_degC(AlxIna228* me, float* temp_degC)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x06_DIETEMP);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x06_DIETEMP"); return status; }

	// Set Conversion vbus factor
	const float CONVERSION_TEMP_FACTOR = 7.8125;

	// Set
	*temp_degC = me->reg._0x06_DIETEMP.val.DIETEMP_mDegC * CONVERSION_TEMP_FACTOR;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetCurrent_A(AlxIna228* me, float* current_A)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x07_CURRENT);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x07_CURRENT"); return status; }

	// Set Conversion Current LSB Factor
	const float CONVERSION_CURRENT_LSB_FACTOR = me->conversionCurrentLsbFactor;

	// Set
	*current_A = CONVERSION_CURRENT_LSB_FACTOR * me->reg._0x07_CURRENT.val.CURRENT_Amperes;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetPower_W(AlxIna228* me, float* power_W)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x08_POWER);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x08_POWER"); return status; }

	// Set Conversion Power Factor
	const float CONVERSION_POWER_FACTOR = 3.2;

	// Set Conversion Current LSB Factor
	const float CONVERSION_CURRENT_LSB_FACTOR = me->conversionCurrentLsbFactor;

	// Set
	*power_W = CONVERSION_POWER_FACTOR * CONVERSION_CURRENT_LSB_FACTOR * me->reg._0x08_POWER.val.POWER_Watts;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetEnergy_J(AlxIna228* me, float* energy_J)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x09_ENERGY);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x09_ENERGY"); return status; }

	// Set Conversion Energy Factor
	const float CONVERSION_ENERGY_FACTOR = 16 * 3.2;

	// Set Conversion Current LSB Factor
	const float CONVERSION_CURRENT_LSB_FACTOR = me->conversionCurrentLsbFactor;

	// Set
	*energy_J = CONVERSION_ENERGY_FACTOR * CONVERSION_CURRENT_LSB_FACTOR * me->reg._0x09_ENERGY.val.ENERGY_Joules;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_GetCharge_C(AlxIna228* me, float* charge_C)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Read
	status = AlxIna228_Reg_Read(me, &me->reg._0x0A_CHARGE);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x0A_CHARGE"); return status; }

	// Set Conversion Current LSB Factor
	const float CONVERSION_CURRENT_LSB_FACTOR = me->conversionCurrentLsbFactor;

	// Set
	*charge_C = CONVERSION_CURRENT_LSB_FACTOR * me->reg._0x0A_CHARGE.val.CHARGE_Coulombs;

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_ResetEnergyAndCharge(AlxIna228* me)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// reset
	me->reg._0x00_CONFIG.val.RSTACC = EnergyAndChargeRegisters_Cleared;

	// Write
	status = AlxIna228_Reg_Write(me, &me->reg._0x00_CONFIG);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x00_CONFIG"); return status; }

	// set
	me->reg._0x00_CONFIG.val.RSTACC = EnergyAndChargeRegisters_NormalOperation;

	// Write
	status = AlxIna228_Reg_Write(me, &me->reg._0x00_CONFIG);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x00_CONFIG"); return status; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxIna228_ResetSystem(AlxIna228* me)
{
	// Assert
	ALX_INA228_ASSERT(me->wasCtorCalled == true);
	ALX_INA228_ASSERT(me->isInitPeriph == true);
	ALX_INA228_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// reset
	me->reg._0x00_CONFIG.val.RST = SystemReset_Enabled;

	// Write
	status = AlxIna228_Reg_Write(me, &me->reg._0x00_CONFIG);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x00_CONFIG"); return status; }

	// set
	me->reg._0x00_CONFIG.val.RST = SystemReset_Disabled;

	// Return
	return Alx_Ok;
}

//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIna228_RegStruct_SetAddr(AlxIna228* me)
{
	me->reg._0x00_CONFIG			.addr = 0x00;
	me->reg._0x01_ADC_CONFIG		.addr = 0x01;
	me->reg._0x02_SHUNT_CAL			.addr = 0x02;
	me->reg._0x03_SHUNT_TEMPCO		.addr = 0x03;
	me->reg._0x04_VSHUNT			.addr = 0x04;
	me->reg._0x05_VBUS				.addr = 0x05;
	me->reg._0x06_DIETEMP			.addr = 0x06;
	me->reg._0x07_CURRENT			.addr = 0x07;
	me->reg._0x08_POWER				.addr = 0x08;
	me->reg._0x09_ENERGY			.addr = 0x09;
	me->reg._0x0A_CHARGE			.addr = 0x0A;
	me->reg._0x0B_DIAG_ALRT			.addr = 0x0B;
	me->reg._0x0C_SOVL				.addr = 0x0C;
	me->reg._0x0D_SUVL				.addr = 0x0D;
	me->reg._0x0E_BOVL				.addr = 0x0E;
	me->reg._0x0F_BUVL				.addr = 0x0F;
	me->reg._0x10_TEMP_LIMIT		.addr = 0x10;
	me->reg._0x11_PWR_LIMIT			.addr = 0x11;
	me->reg._0x3E_MANUFACTURER_ID	.addr = 0x3E;
	me->reg._0x3F_DEVICE_ID			.addr = 0x3F;
}
static void AlxIna228_RegStruct_SetLen(AlxIna228* me)
{
	me->reg._0x00_CONFIG			.len = sizeof(me->reg._0x00_CONFIG			.val);
	me->reg._0x01_ADC_CONFIG		.len = sizeof(me->reg._0x01_ADC_CONFIG		.val);
	me->reg._0x02_SHUNT_CAL			.len = sizeof(me->reg._0x02_SHUNT_CAL		.val);
	me->reg._0x03_SHUNT_TEMPCO		.len = sizeof(me->reg._0x03_SHUNT_TEMPCO	.val);
	me->reg._0x04_VSHUNT			.len = sizeof(me->reg._0x04_VSHUNT			.val);
	me->reg._0x05_VBUS				.len = sizeof(me->reg._0x05_VBUS			.val);
	me->reg._0x06_DIETEMP			.len = sizeof(me->reg._0x06_DIETEMP			.val);
	me->reg._0x07_CURRENT			.len = sizeof(me->reg._0x07_CURRENT			.val);
	me->reg._0x08_POWER				.len = sizeof(me->reg._0x08_POWER			.val);
	me->reg._0x09_ENERGY			.len = sizeof(me->reg._0x09_ENERGY			.val);
	me->reg._0x0A_CHARGE			.len = sizeof(me->reg._0x0A_CHARGE			.val);
	me->reg._0x0B_DIAG_ALRT			.len = sizeof(me->reg._0x0B_DIAG_ALRT		.val);
	me->reg._0x0C_SOVL				.len = sizeof(me->reg._0x0C_SOVL			.val);
	me->reg._0x0D_SUVL				.len = sizeof(me->reg._0x0D_SUVL			.val);
	me->reg._0x0E_BOVL				.len = sizeof(me->reg._0x0E_BOVL			.val);
	me->reg._0x0F_BUVL				.len = sizeof(me->reg._0x0F_BUVL			.val);
	me->reg._0x10_TEMP_LIMIT		.len = sizeof(me->reg._0x10_TEMP_LIMIT		.val);
	me->reg._0x11_PWR_LIMIT			.len = sizeof(me->reg._0x11_PWR_LIMIT		.val);
	me->reg._0x3E_MANUFACTURER_ID	.len = sizeof(me->reg._0x3E_MANUFACTURER_ID	.val);
	me->reg._0x3F_DEVICE_ID			.len = sizeof(me->reg._0x3F_DEVICE_ID		.val);
}
static void AlxIna228_RegStruct_SetValToZero(AlxIna228* me)
{
	me->reg._0x00_CONFIG			.val.raw =	0x0000;
	me->reg._0x01_ADC_CONFIG		.val.raw =	0x0000;
	me->reg._0x02_SHUNT_CAL			.val.raw =	0x0000;
	me->reg._0x03_SHUNT_TEMPCO		.val.raw =	0x0000;
	memset(me->reg._0x04_VSHUNT.val.raw,		0x00, sizeof(me->reg._0x04_VSHUNT	.val.raw));
	memset(me->reg._0x05_VBUS.val.raw,			0x00, sizeof(me->reg._0x05_VBUS		.val.raw));
	me->reg._0x06_DIETEMP			.val.raw =	0x0000;
	memset(me->reg._0x07_CURRENT.val.raw,		0x00, sizeof(me->reg._0x07_CURRENT	.val.raw));
	memset(me->reg._0x08_POWER.val.raw,			0x00, sizeof(me->reg._0x08_POWER	.val.raw));
	memset(me->reg._0x09_ENERGY.val.raw,		0x00, sizeof(me->reg._0x09_ENERGY	.val.raw));
	memset(me->reg._0x0A_CHARGE.val.raw,		0x00, sizeof(me->reg._0x0A_CHARGE	.val.raw));
	me->reg._0x0B_DIAG_ALRT			.val.raw =	0x0000;
	me->reg._0x0C_SOVL				.val.raw =	0x0000;
	me->reg._0x0D_SUVL				.val.raw =	0x0000;
	me->reg._0x0E_BOVL				.val.raw =	0x0000;
	me->reg._0x0F_BUVL				.val.raw =	0x0000;
	me->reg._0x10_TEMP_LIMIT		.val.raw =	0x0000;
	me->reg._0x11_PWR_LIMIT			.val.raw =	0x0000;
	me->reg._0x3E_MANUFACTURER_ID	.val.raw =	0x0000;
	me->reg._0x3F_DEVICE_ID			.val.raw =	0x0000;
}
static void AlxIna228_RegStruct_SetToDefault(AlxIna228* me)
{
	me->reg._0x00_CONFIG.val.raw =			0x0000;
	me->reg._0x01_ADC_CONFIG.val.raw =		0xFB68;
	me->reg._0x02_SHUNT_CAL.val.raw	=		0x1000;
	me->reg._0x03_SHUNT_TEMPCO.val.raw =	0x0000;
	memset(me->reg._0x04_VSHUNT.val.raw,	0x00, sizeof(me->reg._0x04_VSHUNT	.val.raw));
	memset(me->reg._0x05_VBUS.val.raw,		0x00, sizeof(me->reg._0x05_VBUS		.val.raw));
	me->reg._0x06_DIETEMP.val.raw =			0x0000;
	memset(me->reg._0x07_CURRENT.val.raw,	0x00, sizeof(me->reg._0x07_CURRENT	.val.raw));
	memset(me->reg._0x08_POWER.val.raw,		0x00, sizeof(me->reg._0x08_POWER	.val.raw));
	memset(me->reg._0x09_ENERGY.val.raw,	0x00, sizeof(me->reg._0x09_ENERGY	.val.raw));
	memset(me->reg._0x0A_CHARGE.val.raw,	0x00, sizeof(me->reg._0x0A_CHARGE	.val.raw));
	me->reg._0x0B_DIAG_ALRT.val.raw =		0x0001;
	me->reg._0x0C_SOVL.val.raw =			0x7FFF;
	me->reg._0x0D_SUVL.val.raw =			0x8000;
	me->reg._0x0E_BOVL.val.raw =			0x7FFF;
	me->reg._0x0F_BUVL.val.raw =			0x0000;
	me->reg._0x10_TEMP_LIMIT.val.raw =		0x7FFF;
	me->reg._0x11_PWR_LIMIT.val.raw =		0xFFFF;
	me->reg._0x3E_MANUFACTURER_ID.val.raw =	0x5449;
	me->reg._0x3F_DEVICE_ID.val.raw =		0x2281;
}

static Alx_Status AlxIna228_Reg_Write(AlxIna228* me, void* reg)
{
	// Local variables
	Alx_Status status = Alx_Err;
	uint8_t regAddr = *((uint8_t*)reg);
	uint8_t regLen = *((uint8_t*)reg + sizeof(regAddr));
	uint8_t* regValPtr = (uint8_t*)reg + sizeof(regAddr) + sizeof(regLen);
	uint8_t data[regLen];

	// flipped the MSB and LSB all lenght
	for(uint32_t i = 0; i< regLen; i++)
	data[regLen-1-i] = regValPtr[i];

	// Write
	status = AlxI2c_Master_StartWriteMemStop_Multi(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, regLen, me->i2cCheckWithRead, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_AlxI2c_Master_StartWriteMemStop_Multi"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxIna228_Reg_Write_All(AlxIna228* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// Write
	status = AlxIna228_Reg_Write(me, &me->reg._0x00_CONFIG);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x00_CONFIG"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x01_ADC_CONFIG);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x01_ADC_CONFIG"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x02_SHUNT_CAL);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x02_SHUNT_CAL"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x03_SHUNT_TEMPCO);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x03_SHUNT_TEMPCO"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x0B_DIAG_ALRT);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x0B_DIAG_ALRT"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x0C_SOVL);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x0C_SOVL"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x0D_SUVL);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x0D_SUVL"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x0E_BOVL);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x0E_BOVL"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x0F_BUVL);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x0F_BUVL"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x10_TEMP_LIMIT);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x10_TEMP_LIMIT"); return status; }

	status = AlxIna228_Reg_Write(me, &me->reg._0x11_PWR_LIMIT);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x11_PWR_LIMIT"); return status; }

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
	uint8_t data[regLen];

	// Read
	status = AlxI2c_Master_StartReadMemStop(me->i2c, me->i2cAddr, regAddr, AlxI2c_Master_MemAddrLen_8bit, data, regLen, me->i2cNumOfTries, me->i2cTimeout_ms);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err"); return status; }

	// flipped the MSB and LSB all lenght
	for(uint32_t i = 0; i< regLen; i++)
	regValPtr[i] = data[regLen-1-i];

	// Return
	return Alx_Ok;
}

static void AlxIna228_CurrentLsbFactor(AlxIna228* me)
{
	// Set Shunt Resistor value
	const float SHUNT_RES_VAL = me->shuntRes_Ohm;

	// Set AdcRange_163_84_mV value
	const float ADC_RANGE_163_84_MV = 163.84e-3;

	// Set AdcRange_40_96_mV value
	const float ADC_RANGE_40_96_MV = 40.96e-3;

	// Set Maximum Expected Current [A]
	const float MAX_EXPECTED_CURRENT = (me->adcRange == AdcRange_163_84_mV) ? (ADC_RANGE_163_84_MV / SHUNT_RES_VAL) : (ADC_RANGE_40_96_MV / SHUNT_RES_VAL);

	// Set Current LSB factor
	const float CURRENT_LSB_FACTOR =  MAX_EXPECTED_CURRENT / 524288.0; // 2^19 = 524288;

	// Save
	me->conversionCurrentLsbFactor = CURRENT_LSB_FACTOR;
}
static void AlxIna228_ShuntFactor(AlxIna228* me)
{
	// Set Fixed Scaling Factor
	const float FIXED_SCALING_FACTOR = 13107.2 * 1e6;

	// Set Current LSB factor
	const float CONVERSION_CURRENT_LSB_FACTOR = me->conversionCurrentLsbFactor;

	// Set Shunt Resistor value
	const float SHUNT_RES_VAL = me->shuntRes_Ohm;

	// Set Multiplied factor, Depending on ADCRANGE
	const float MULTIPLIED_FACTOR = (me->adcRange == AdcRange_163_84_mV) ? 1.0 : 4.0;

	 // Set Shunt Cal Register Value
	const float shuntCalRegVal = FIXED_SCALING_FACTOR * CONVERSION_CURRENT_LSB_FACTOR * SHUNT_RES_VAL * MULTIPLIED_FACTOR;

	// Save
	me->conversionShuntFactor = (uint16_t)shuntCalRegVal;
}
static Alx_Status AlxIna228_TraceId(AlxIna228* me)
{
	// Local variables
	Alx_Status status = Alx_Err;

	// #1 Read ID registers
	status = AlxIna228_Reg_Read(me, &me->reg._0x3E_MANUFACTURER_ID);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x3E_MANUFACTURER_ID	"); return status;}

	status = AlxIna228_Reg_Read(me, &me->reg._0x3F_DEVICE_ID);
	if (status != Alx_Ok) { ALX_INA228_TRACE_WRN("Err_0x3F_DEVICE_ID	"); return status;}

	// #2 Trace
	ALX_INA228_TRACE_INF("");
	ALX_INA228_TRACE_INF("Auralix C Library - ALX Current Monitor INA228 Module Identification:");
	ALX_INA228_TRACE_INF("- MANUFACTURER_ID: 0x%02X", me->reg._0x3E_MANUFACTURER_ID.val.MANFID);
	ALX_INA228_TRACE_INF("- DEVICE_ID.REV_ID: 0x%02X", me->reg._0x3F_DEVICE_ID.val.REV_ID);
	ALX_INA228_TRACE_INF("- DEVICE_ID.DIEID: 0x%02X", me->reg._0x3F_DEVICE_ID.val.DIEID);
	ALX_INA228_TRACE_INF("\r\n");

	// #3 Return OK
	return Alx_Ok;
}


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxIna228_RegStruct_SetVal(AlxIna228* me)
{
	// Local variables
	(void)me;

	// Assert
	ALX_INA228_TRACE_WRN("Define 'AlxIna228_RegStruct_SetVal' function in your application.");
	ALX_INA228_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB)
