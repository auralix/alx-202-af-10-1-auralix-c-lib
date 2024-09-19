/**
  ******************************************************************************
  * @file		alxIna228.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_INA228_H
#define ALX_INA228_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_INA228_FILE "alxIna228.h"

// Assert //
#if defined(ALX_INA228_ASSERT_BKPT_ENABLE)
	#define ALX_INA228_ASSERT(expr) ALX_ASSERT_BKPT(ALX_INA228_FILE, expr)
#elif defined(ALX_INA228_ASSERT_TRACE_ENABLE)
	#define ALX_INA228_ASSERT(expr) ALX_ASSERT_TRACE(ALX_INA228_FILE, expr)
#elif defined(ALX_INA228_ASSERT_RST_ENABLE)
	#define ALX_INA228_ASSERT(expr) ALX_ASSERT_RST(ALX_INA228_FILE, expr)
#else
	#define ALX_INA228_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_INA228_TRACE_ENABLE)
	#define ALX_INA228_TRACE_WRN(...) ALX_TRACE_WRN(ALX_INA228_FILE, __VA_ARGS__)
	#define ALX_INA228_TRACE_INF(...) ALX_TRACE_INF(ALX_INA228_FILE, __VA_ARGS__)
#else
	#define ALX_INA228_TRACE_WRN(...) do{} while (false)
	#define ALX_INA228_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//******************************************************************************
// Address: 0x00, Reset: 0x00 0x00, Name: CONFIG
//******************************************************************************
typedef enum
{
	AdcRange_163_84_mV = 0,
	AdcRange_40_96_mV = 1
} AlxIna228_RegEnum_0x00_ADCRANGE;
typedef enum
{
	TempCompen_Disabled = 0,
	TempCompen_Enabled = 1
} AlxIna228_RegEnum_0x00_TEMPCOMP;
typedef enum
{
	EnergyAndChargeRegisters_NormalOperation = 0,
	EnergyAndChargeRegisters_Cleared = 1
} AlxIna228_RegEnum_0x00_RSTACC;
typedef enum
{
	SystemReset_Disabled = 0,
	SystemReset_Enabled = 1
} AlxIna228_RegEnum_0x00_RST;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3 : 4;
		AlxIna228_RegEnum_0x00_ADCRANGE ADCRANGE : 1;
		AlxIna228_RegEnum_0x00_TEMPCOMP TEMPCOMP : 1;
		uint8_t CONVDLY : 8;
		AlxIna228_RegEnum_0x00_RSTACC RSTACC : 1;
		AlxIna228_RegEnum_0x00_RST RST : 1;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x00_CONFIG;


//******************************************************************************
// Address: 0x01, Reset: 0xFB 0x68, Name: ADC_CONFIG
//******************************************************************************
typedef enum
{
	AdcSampleAvg_1 = 0,
	AdcSampleAvg_4 = 1,
	AdcSampleAvg_16 = 2,
	AdcSampleAvg_64 = 3,
	AdcSampleAvg_128 = 4,
	AdcSampleAvg_256 = 5,
	AdcSampleAvg_512 = 6,
	AdcSampleAvg_1024 = 7
} AlxIna228_RegEnum_0x01_AVG;
typedef enum
{
	ConversionTimeTemp_50_us = 0,
	ConversionTimeTemp_84_us = 1,
	ConversionTimeTemp_150_us = 2,
	ConversionTimeTemp_280_us = 3,
	ConversionTimeTemp_540_us = 4,
	ConversionTimeTemp_1052_us = 5,
	ConversionTimeTemp_2074_us = 6,
	ConversionTimeTemp_4120_us = 7,
} AlxIna228_RegEnum_0x01_VTCT;
typedef enum
{
	ConversionTimeShuntVolt_50_us = 0,
	ConversionTimeShuntVolt_84_us = 1,
	ConversionTimeShuntVolt_150_us = 2,
	ConversionTimeShuntVolt_280_us = 3,
	ConversionTimeShuntVolt_540_us = 4,
	ConversionTimeShuntVolt_1052_us = 5,
	ConversionTimeShuntVolt_2074_us = 6,
	ConversionTimeShuntVolt_4120_us = 7,
} AlxIna228_RegEnum_0x01_VSHCT;
typedef enum
{
	ConversionTimeBusVolt_50_us = 0,
	ConversionTimeBusVolt_84_us = 1,
	ConversionTimeBusVolt_150_us = 2,
	ConversionTimeBusVolt_280_us = 3,
	ConversionTimeBusVolt_540_us = 4,
	ConversionTimeBusVolt_1052_us = 5,
	ConversionTimeBusVolt_2074_us = 6,
	ConversionTimeBusVolt_4120_us = 7,
} AlxIna228_RegEnum_0x01_VBUSCT;
typedef enum
{
	Shutdown = 0,
	BusVoltage = 1,
	ShuntVoltage = 2,
	ShuntVoltage_BusVoltage = 3,
	Temp = 4,
	Temp_BusVoltage = 5,
	Temp_ShuntVoltage = 6,
	BusVoltage_ShuntVoltage_Temp = 7
} AlxIna228_RegEnum_0x01_TRIGGER_SOURCE;
typedef enum
{
	Mode_Trigger = 0,
	Mode_Continuous = 1
} AlxIna228_RegEnum_0x01_TRIGGER_MODE;
typedef union
{
	struct __attribute__((packed))
	{
		AlxIna228_RegEnum_0x01_AVG AVG : 3;
		AlxIna228_RegEnum_0x01_VTCT VTCT : 3;
		AlxIna228_RegEnum_0x01_VSHCT VSHCT : 3;
		AlxIna228_RegEnum_0x01_VBUSCT VBUSCT : 3;
		AlxIna228_RegEnum_0x01_TRIGGER_SOURCE SOURCE : 3;
		AlxIna228_RegEnum_0x01_TRIGGER_MODE MODE : 1;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x01_ADC_CONFIG;


//******************************************************************************
// Address: 0x02, Reset: 0x10 0x00, Name: SHUNT_CAL
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t SHUNT_CAL_Current_Amperes : 15;
		uint8_t unused_15 : 1;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x02_SHUNT_CAL;


//******************************************************************************
// Address: 0x03, Reset: 0x00 0x00, Name: SHUNT_TEMPCO
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t TEMPCO_ppm_DegC : 14;
		uint8_t unused_14_15 : 2;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x03_SHUNT_TEMPCO;


//******************************************************************************
// Address: 0x04, Reset: 0x00 0x00, Name: VSHUNT
//******************************************************************************
typedef union
{
    struct __attribute__((packed))
    {
		int8_t unused_0_3 : 4;
		int32_t VSHUNT_nVoltage : 20;
    };
	int8_t raw[3];
} AlxIna228_RegVal_0x04_VSHUNT;


//******************************************************************************
// Address: 0x05, Reset: 0x00 0x00, Name: VBUS
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int8_t unused_0_3 : 4;
		int32_t VBUS_uVoltage : 20;
	};
	int8_t raw[3];
} AlxIna228_RegVal_0x05_VBUS;


//******************************************************************************
// Address: 0x06, Reset: 0x00 0x00, Name: DIETEMP
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int16_t DIETEMP_mDegC;
	};
	int16_t raw;
} AlxIna228_RegVal_0x06_DIETEMP;


//******************************************************************************
// Address: 0x07, Reset: 0x00 0x00, Name: CURRENT
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int8_t unused_0_3 : 4;
		int32_t CURRENT_Amperes : 20;
	};
	int8_t raw[3];
} AlxIna228_RegVal_0x07_CURRENT;


//******************************************************************************
// Address: 0x08, Reset: 0x00 0x00, Name: POWER
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint32_t POWER_Watts : 24;
	};
	uint8_t raw[3];
} AlxIna228_RegVal_0x08_POWER;


//******************************************************************************
// Address: 0x09, Reset: 0x00 0x00, Name: ENERGY
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint64_t ENERGY_Joules : 40;
	};
	uint8_t raw[5];
} AlxIna228_RegVal_0x09_ENERGY;


//******************************************************************************
// Address: 0x0A, Reset: 0x00 0x00, Name: CHARGE
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int64_t CHARGE_Coulombs : 40;
	};
	int8_t raw[5];
} AlxIna228_RegVal_0x0A_CHARGE;


//******************************************************************************
// Address: 0x0B, Reset: 0x00 0x01, Name: DIAG_ALRT
//******************************************************************************
typedef enum
{
	MemoryChecksum_Error = 0,
	MemoryChecksum_Normal = 1
} AlxIna228_RegEnum_0x0B_MEMSTAT;
typedef enum
{
	ConversionIsNotcompleted = 0,
	ConversionIsCompleted = 1
} AlxIna228_RegEnum_0x0B_CNVRF;
typedef enum
{
	PowerOverLimitEvent_Off = 0,
	PowerOverLimitEvent_On = 1
} AlxIna228_RegEnum_0x0B_POL;
typedef enum
{
	BusUnderLimitEvent_Off = 0,
	BusUnderLimitEvent_On = 1
} AlxIna228_RegEnum_0x0B_BUSUL;
typedef enum
{
	BusOverLimitEvent_Off = 0,
	BusOverLimitEvent_On = 1
} AlxIna228_RegEnum_0x0B_BUSOL;
typedef enum
{
	UnderShuntVoltageEvent_Off = 0,
	UnderShuntVoltageEvent_On = 1
} AlxIna228_RegEnum_0x0B_SHNTUL;
typedef enum
{
	OverShuntVoltageEvent_Off = 0,
	OverShuntVoltageEvent_On = 1
} AlxIna228_RegEnum_0x0B_SHNTOL;
typedef enum
{
	OverTempEvent_Off = 0,
	OverTempEvent_On = 1
} AlxIna228_RegEnum_0x0B_TMPOL;
typedef enum
{
	ArithmeticOverflowIs_Normal = 0,
	ArithmeticOverflowIs_Error = 1
} AlxIna228_RegEnum_0x0B_MATHOF;
typedef enum
{
	HealthChargeOverflow_Normal = 0,
	HealthChargeOverflow_Error = 1
} AlxIna228_RegEnum_0x0B_CHARGEOF;
typedef enum
{
	HealthEnergyOverflow_Normal = 0,
	HealthEnergyOverflow_Error = 1
} AlxIna228_RegEnum_0x0B_ENERGYOF;
typedef enum
{
	PolarityOpenDrain_ActiveLow = 0,
	PolarityOpenDrain_ActiveHigh = 1
} AlxIna228_RegEnum_0x0B_APOL;
typedef enum
{
	AlertComparison_AveragedAdc_Off = 0,
	AlertComparison_AveragedAdc_On = 1
} AlxIna228_RegEnum_0x0B_SLOWALERT;
typedef enum
{
	ConversionReadyFlagAlert_Disabled = 0,
	ConversionReadyFlagAlert_Enabled = 1
} AlxIna228_RegEnum_0x0B_CNVR;
typedef enum
{
	TransparentMode = 0,
	LatchMode = 1
} AlxIna228_RegEnum_0x0B_ALATCH;
typedef union
{
	struct __attribute__((packed))
	{
		AlxIna228_RegEnum_0x0B_MEMSTAT MEMSTAT : 1;
		AlxIna228_RegEnum_0x0B_CNVRF CNVRF : 1;
		AlxIna228_RegEnum_0x0B_POL POL : 1;
		AlxIna228_RegEnum_0x0B_BUSUL BUSUL : 1;
		AlxIna228_RegEnum_0x0B_BUSOL BUSOL : 1;
		AlxIna228_RegEnum_0x0B_SHNTUL SHNTUL : 1;
		AlxIna228_RegEnum_0x0B_SHNTOL SHNTOL : 1;
		AlxIna228_RegEnum_0x0B_TMPOL TMPOL : 1;
		uint8_t unused_8 : 1;
		AlxIna228_RegEnum_0x0B_MATHOF MATHOF : 1;
		AlxIna228_RegEnum_0x0B_CHARGEOF CHARGEOF : 1;
		AlxIna228_RegEnum_0x0B_ENERGYOF ENERGYOF : 1;
		AlxIna228_RegEnum_0x0B_APOL APOL : 1;
		AlxIna228_RegEnum_0x0B_SLOWALERT SLOWALERT : 1;
		AlxIna228_RegEnum_0x0B_CNVR CNVR : 1;
		AlxIna228_RegEnum_0x0B_ALATCH ALATCH : 1;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x0B_DIAG_ALRT;


//******************************************************************************
// Address: 0x0C, Reset: 0x7F 0xFF, Name: SOVL
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int16_t SOVL_uVoltage;
	};
	int16_t raw;
} AlxIna228_RegVal_0x0C_SOVL;


//******************************************************************************
// Address: 0x0D, Reset: 0x80 0x00, Name: SUVL
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int16_t SUVL_uVoltage;
	};
	int16_t raw;
} AlxIna228_RegVal_0x0D_SUVL;


//******************************************************************************
// Address: 0x0E, Reset: 0x7F 0xFF, Name: BOVL
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t BOVL_mVoltage : 15;
		uint8_t unused_15 : 1;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x0E_BOVL;


//******************************************************************************
// Address: 0x0F, Reset: 0x00 0x00, Name: BUVL
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t BUVL_mVoltage : 15;
		uint8_t unused_15 : 1;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x0F_BUVL;


//******************************************************************************
// Address: 0x10, Reset: 0x7F 0xFF, Name: TEMP_LIMIT
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		int16_t TOL_mDegC;
	};
	int16_t raw;
} AlxIna228_RegVal_0x10_TEMP_LIMIT;


//******************************************************************************
// Address: 0x11, Reset: 0xFF 0xFF, Name: PWR_LIMIT
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t POL;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x11_PWR_LIMIT;


//******************************************************************************
// Address: 0x3E, Reset: 0x54 0x49, Name: MANUFACTURER_ID
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t MANFID;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x3E_MANUFACTURER_ID;


//******************************************************************************
// Address: 0x3F, Reset: 0x22 0x81, Name: DEVICE_ID
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t REV_ID : 4;
		uint16_t DIEID : 12;
	};
	uint16_t raw;
} AlxIna228_RegVal_0x3F_DEVICE_ID;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x00_CONFIG val;
} AlxIna228_Reg_0x00_CONFIG;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x01_ADC_CONFIG val;
} AlxIna228_Reg_0x01_ADC_CONFIG;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x02_SHUNT_CAL val;
} AlxIna228_Reg_0x02_SHUNT_CAL;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x03_SHUNT_TEMPCO val;
} AlxIna228_Reg_0x03_SHUNT_TEMPCO;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x04_VSHUNT val;
} AlxIna228_Reg_0x04_VSHUNT;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x05_VBUS val;
} AlxIna228_Reg_0x05_VBUS;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x06_DIETEMP val;
} AlxIna228_Reg_0x06_DIETEMP;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x07_CURRENT val;
} AlxIna228_Reg_0x07_CURRENT;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x08_POWER val;
} AlxIna228_Reg_0x08_POWER;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x09_ENERGY val;
} AlxIna228_Reg_0x09_ENERGY;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x0A_CHARGE val;
} AlxIna228_Reg_0x0A_CHARGE;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x0B_DIAG_ALRT val;
} AlxIna228_Reg_0x0B_DIAG_ALRT;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x0C_SOVL val;
} AlxIna228_Reg_0x0C_SOVL;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x0D_SUVL val;
} AlxIna228_Reg_0x0D_SUVL;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x0E_BOVL val;
} AlxIna228_Reg_0x0E_BOVL;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x0F_BUVL val;
} AlxIna228_Reg_0x0F_BUVL;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x10_TEMP_LIMIT val;
} AlxIna228_Reg_0x10_TEMP_LIMIT;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x11_PWR_LIMIT val;
} AlxIna228_Reg_0x11_PWR_LIMIT;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x3E_MANUFACTURER_ID val;
} AlxIna228_Reg_0x3E_MANUFACTURER_ID;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxIna228_RegVal_0x3F_DEVICE_ID val;
} AlxIna228_Reg_0x3F_DEVICE_ID;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxIna228_Reg_0x00_CONFIG			_0x00_CONFIG;
	AlxIna228_Reg_0x01_ADC_CONFIG		_0x01_ADC_CONFIG;
	AlxIna228_Reg_0x02_SHUNT_CAL		_0x02_SHUNT_CAL;
	AlxIna228_Reg_0x03_SHUNT_TEMPCO		_0x03_SHUNT_TEMPCO;
	AlxIna228_Reg_0x04_VSHUNT			_0x04_VSHUNT;
	AlxIna228_Reg_0x05_VBUS				_0x05_VBUS;
	AlxIna228_Reg_0x06_DIETEMP			_0x06_DIETEMP;
	AlxIna228_Reg_0x07_CURRENT			_0x07_CURRENT;
	AlxIna228_Reg_0x08_POWER			_0x08_POWER;
	AlxIna228_Reg_0x09_ENERGY			_0x09_ENERGY;
	AlxIna228_Reg_0x0A_CHARGE			_0x0A_CHARGE;
	AlxIna228_Reg_0x0B_DIAG_ALRT		_0x0B_DIAG_ALRT;
	AlxIna228_Reg_0x0C_SOVL				_0x0C_SOVL;
	AlxIna228_Reg_0x0D_SUVL				_0x0D_SUVL;
	AlxIna228_Reg_0x0E_BOVL				_0x0E_BOVL;
	AlxIna228_Reg_0x0F_BUVL				_0x0F_BUVL;
	AlxIna228_Reg_0x10_TEMP_LIMIT		_0x10_TEMP_LIMIT;
	AlxIna228_Reg_0x11_PWR_LIMIT		_0x11_PWR_LIMIT;
	AlxIna228_Reg_0x3E_MANUFACTURER_ID	_0x3E_MANUFACTURER_ID;
	AlxIna228_Reg_0x3F_DEVICE_ID		_0x3F_DEVICE_ID;
} AlxIna228_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;
	AlxIna228_RegEnum_0x00_ADCRANGE adcRange;
	float shuntRes_Ohm;
	float shuntResTemp_ppmPerDegC;

	// Variables
	AlxIna228_Reg reg;
	float conversionCurrentLsbFactor;
	uint16_t conversionShuntFactor;

	// Info
	bool isInit;
	bool isInitPeriph;
	bool wasCtorCalled;
} AlxIna228;


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
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxIna228_InitPeriph(AlxIna228* me);
Alx_Status AlxIna228_DeInitPeriph(AlxIna228* me);
Alx_Status AlxIna228_Init(AlxIna228* me);
Alx_Status AlxIna228_DeInit(AlxIna228* me);
Alx_Status AlxIna228_GetShuntVoltage_V(AlxIna228* me, float* voltage_V);
Alx_Status AlxIna228_GetBusVoltage_V(AlxIna228* me, float* voltage_V);
Alx_Status AlxIna228_GetTemp_degC(AlxIna228* me, float* temp_degC);
Alx_Status AlxIna228_GetCurrent_A(AlxIna228* me, float* current_A);
Alx_Status AlxIna228_GetPower_W(AlxIna228* me, float* power_W);
Alx_Status AlxIna228_GetEnergy_J(AlxIna228* me, float* energy_J);
Alx_Status AlxIna228_GetCharge_C(AlxIna228* me, float* charge_C);
Alx_Status AlxIna228_ResetEnergyAndCharge(AlxIna228* me);
Alx_Status AlxIna228_ResetSystem(AlxIna228* me);

#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_INA228_H
