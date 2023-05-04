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
// Constructor
//******************************************************************************
void AlxIna228_Ctor
(
	AlxIna228* me,
	uint32_t* param
)
{
	// Parameters
	me->param = param;

	// Variables
	me->var = 0;

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


#endif	// #if defined(ALX_C_LIB)
