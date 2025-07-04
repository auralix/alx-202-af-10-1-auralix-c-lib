﻿/**
  ******************************************************************************
  * @file		alxAdxl355.h
  * @brief		Auralix C Library - ALX Accelerometer ADXL355 Module
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
#ifndef ALX_ADXL355_H
#define ALX_ADXL355_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxAccelerometer.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32L4)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADXL355_FILE "alxAdxl355.h"

// Assert //
#if defined(ALX_ADXL355_ASSERT_BKPT_ENABLE)
	#define ALX_ADXL355_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADXL355_FILE, expr)
#elif defined(ALX_ADXL355_ASSERT_TRACE_ENABLE)
	#define ALX_ADXL355_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADXL355_FILE, expr)
#elif defined(ALX_ADXL355_ASSERT_RST_ENABLE)
	#define ALX_ADXL355_ASSERT(expr) ALX_ASSERT_RST(ALX_ADXL355_FILE, expr)
#else
	#define ALX_ADXL355_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_ADXL355_TRACE_ENABLE)
	#define ALX_ADXL355_TRACE_WRN(...) ALX_TRACE_WRN(ALX_ADXL355_FILE, __VA_ARGS__)
	#define ALX_ADXL355_TRACE_INF(...) ALX_TRACE_INF(ALX_ADXL355_FILE, __VA_ARGS__)
#else
	#define ALX_ADXL355_TRACE_WRN(...) do{} while (false)
	#define ALX_ADXL355_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//------------------------------------------------------------------------------
// ANALOG DEVICES ID REGISTER
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t DEVID_AD;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x00_DEVID_AD;


//------------------------------------------------------------------------------
// ANALOG DEVICES MEMS ID REGISTER
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t DEVID_MST;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x01_DEVID_MST;


//------------------------------------------------------------------------------
// DEVICE ID REGISTER
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PARTID;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x02_PARTID;


//------------------------------------------------------------------------------
// PRODUCT REVISION ID REGISTER
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t REVID;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x03_REVID;


//------------------------------------------------------------------------------
// STATUS REGISTER
//------------------------------------------------------------------------------
typedef enum
{
	DATA_RDY_False	= 0b0,
	DATA_RDY_True	= 0b1
} AlxAdxl355_RegEnum_0x04_DATA_RDY;

typedef enum
{
	FIFO_FULL_False	= 0b0,
	FIFO_FULL_True	= 0b1
} AlxAdxl355_RegEnum_0x04_FIFO_FULL;

typedef enum
{
	FIFO_OVR_False	= 0b0,
	FIFO_OVR_True	= 0b1
} AlxAdxl355_RegEnum_0x04_FIFO_OVR;

typedef enum
{
	Activity_Detected		= 0b0,
	Activity_NotDetected	= 0b1
} AlxAdxl355_RegEnum_0x04_Activity;

typedef enum
{
	NVM_BUSY_False	= 0b0,
	NVM_BUSY_True	= 0b1
} AlxAdxl355_RegEnum_0x04_NVM_BUSY;

typedef union
{
	struct __attribute__((packed))
	{
		AlxAdxl355_RegEnum_0x04_DATA_RDY DATA_RDY : 1;
		AlxAdxl355_RegEnum_0x04_FIFO_FULL FIFO_FULL : 1;
		AlxAdxl355_RegEnum_0x04_FIFO_OVR FIFO_OVR : 1;
		AlxAdxl355_RegEnum_0x04_Activity Activity : 1;
		AlxAdxl355_RegEnum_0x04_NVM_BUSY NVM_BUSY : 1;
		uint8_t unused_5_7 : 3;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x04_Status;


//------------------------------------------------------------------------------
// FILTER SETTINGS REGISTER
//------------------------------------------------------------------------------
typedef enum
{
	ODR_LPF_4000Hz_1000Hz		= 0b0000,
	ODR_LPF_2000Hz_500Hz		= 0b0001,
	ODR_LPF_1000Hz_250Hz		= 0b0010,
	ODR_LPF_500Hz_125Hz			= 0b0011,
	ODR_LPF_250Hz_62Hz5Hz		= 0b0100,
	ODR_LPF_125Hz_31Hz25Hz		= 0b0101,
	ODR_LPF_62Hz5_15Hz625Hz		= 0b0110,
	ODR_LPF_31Hz25_7Hz813Hz		= 0b0111,
	ODR_LPF_15Hz625_3Hz906Hz	= 0b1000,
	ODR_LPF_7Hz813_1Hz953Hz		= 0b1001,
	ODR_LPF_3Hz906_0Hz977Hz		= 0b1010
} Adxl355_RegEnum_0x28_ODR_LPF;
typedef enum
{
	HPF_CORNER_NoFilter	= 0b000,
	HPF_CORNER_24p7		= 0b001,
	HPF_CORNER_6p2084	= 0b010,
	HPF_CORNER_1p5545	= 0b011,
	HPF_CORNER_0p3862	= 0b100,
	HPF_CORNER_0p0954	= 0b101,
	HPF_CORNER_0p0238	= 0b110
} Adxl355_RegEnum_0x28_HPF_CORNER;
typedef union
{
	struct __attribute__((packed))
	{
		Adxl355_RegEnum_0x28_ODR_LPF ODR_LPF : 4;
		Adxl355_RegEnum_0x28_HPF_CORNER HPF_CORNER : 3;
		uint8_t unused_7 : 1;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x28_Filter;


//------------------------------------------------------------------------------
// FIFO SAMPLES REGISTER
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t FIFO_SAMPLES : 7;
		uint8_t unused_7 : 1;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x29_FIFO_SAMPLES;


//------------------------------------------------------------------------------
// INTERRUPT PIN (INTX) FUNCTION MAP REGISTER
//------------------------------------------------------------------------------
typedef enum
{
	RDY_EN1_Disable		= 0b0,
	RDY_EN1_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_RDY_EN1;

typedef enum
{
	FULL_EN1_Disable	= 0b0,
	FULL_EN1_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_FULL_EN1;

typedef enum
{
	OVR_EN1_Disable		= 0b0,
	OVR_EN1_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_OVR_EN1;

typedef enum
{
	ACT_EN1_Disable		= 0b0,
	ACT_EN1_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_ACT_EN1;

typedef enum
{
	RDY_EN2_Disable		= 0b0,
	RDY_EN2_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_RDY_EN2;

typedef enum
{
	FULL_EN2_Disable	= 0b0,
	FULL_EN2_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_FULL_EN2;

typedef enum
{
	OVR_EN2_Disable		= 0b0,
	OVR_EN2_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_OVR_EN2;

typedef enum
{
	ACT_EN2_Disable		= 0b0,
	ACT_EN2_Enable		= 0b1
} AlxAdxl355_RegEnum_0x2A_ACT_EN2;

typedef union
{
	struct __attribute__((packed))
	{
		AlxAdxl355_RegEnum_0x2A_RDY_EN1 RDY_EN1 : 1;
		AlxAdxl355_RegEnum_0x2A_FULL_EN1 FULL_EN1 : 1;
		AlxAdxl355_RegEnum_0x2A_OVR_EN1 OVR_EN1 : 1;
		AlxAdxl355_RegEnum_0x2A_ACT_EN1 ACT_EN1 : 1;

		AlxAdxl355_RegEnum_0x2A_RDY_EN2 RDY_EN2 : 1;
		AlxAdxl355_RegEnum_0x2A_FULL_EN2 FULL_EN2 : 1;
		AlxAdxl355_RegEnum_0x2A_OVR_EN2 OVR_EN2 : 1;
		AlxAdxl355_RegEnum_0x2A_ACT_EN2 ACT_EN2 : 1;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x2A_INT_MAP;


//------------------------------------------------------------------------------
// DATA SYNCHRONIZATION REGISTER
//------------------------------------------------------------------------------
typedef enum
{
	EXT_SYNC_InternalSync						= 0b00,
	EXT_SYNC_ExternalSync_NoInterpolationFilter	= 0b01,
	EXT_SYNC_ExternalSync_InterpolationFilter	= 0b10,
	EXT_SYNC_Reserved							= 0b11
} AlxAdxl355_RegEnum_0x2B_EXT_SYNC;

typedef enum
{
	EXT_CLK_Disable	= 0b0,
	EXT_CLK_Enable	= 0b1
} AlxAdxl355_RegEnum_0x2B_EXT_CLK;

typedef union
{
	struct __attribute__((packed))
	{
		AlxAdxl355_RegEnum_0x2B_EXT_SYNC EXT_SYNC : 2;
		AlxAdxl355_RegEnum_0x2B_EXT_CLK EXT_CLK : 1;
		uint8_t unused_3_7 : 5;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x2B_Sync;


//------------------------------------------------------------------------------
// I2C SPEED, INTERRUPT POLARITY, AND RANGE REGISTER
//------------------------------------------------------------------------------
typedef enum
{
	Range_Reserved	= 0b00,
	Range_2g048		= 0b01,
	Range_4g096		= 0b10,
	Range_8g192		= 0b11,
} AlxAdxl355_RegEnum_0x2C_Range;

typedef enum
{
	INT_POL_INT1_INT2_ActiveLow		= 0b0,
	INT_POL_INT1_INT2_ActiveHigh	= 0b1
} AlxAdxl355_RegEnum_0x2C_INT_POL;

typedef enum
{
	I2C_HS_FastMode			= 0b0,
	I2C_HS_HighSpeedMode	= 0b1
} AlxAdxl355_RegEnum_0x2C_I2C_HS;

typedef union
{
	struct __attribute__((packed))
	{
		AlxAdxl355_RegEnum_0x2C_Range Range : 2;
		uint8_t unused_3_7 : 4;
		AlxAdxl355_RegEnum_0x2C_INT_POL INT_POL : 1;
		AlxAdxl355_RegEnum_0x2C_I2C_HS I2C_HS : 1;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x2C_Range;


//------------------------------------------------------------------------------
// POWER CONTROL REGISTER
//------------------------------------------------------------------------------
typedef enum
{
	Standby_StandbyMode		= 0b1,
	Standby_MeasurementMode	= 0b0,
} AlxAdxl355_RegEnum_0x2D_Standby;

typedef enum
{
	TEMP_OFF_TempProcessEnabled		= 0b0,
	TEMP_OFF_TempProcessDisabled	= 0b1
} AlxAdxl355_RegEnum_0x2D_TEMP_OFF;

typedef enum
{
	DRDY_OFF_DRDY_NormalOperation	= 0b0,
	DRDY_OFF_DRDY_ForceToZero		= 0b1
} AlxAdxl355_RegEnum_0x2D_DRDY_OFF;

typedef union
{
	struct __attribute__((packed))
	{
		AlxAdxl355_RegEnum_0x2D_Standby Standby : 1;
		AlxAdxl355_RegEnum_0x2D_TEMP_OFF TEMP_OFF : 1;
		AlxAdxl355_RegEnum_0x2D_DRDY_OFF DRDY_OFF : 1;
		uint8_t unused_3_7 : 5;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x2D_POWER_CTL;


//------------------------------------------------------------------------------
// RESET REGISTER
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Reset;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x2F_Reset;


//******************************************************************************
// Register Group Values Unions
//******************************************************************************


//------------------------------------------------------------------------------
// TEMPERATURE DATA REGISTER GROUP
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t TEMP2;
		uint8_t TEMP1;
	};
	uint8_t raw[2];
} AlxAdxl355_RegGroupVal_0x06_0x07_TEMP;


//------------------------------------------------------------------------------
// X-AXIS / Y-AXIS / Z-AXIS DATA REGISTER GROUP
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t XDATA3;
		uint8_t XDATA2;
		uint8_t XDATA1;

		uint8_t YDATA3;
		uint8_t YDATA2;
		uint8_t YDATA1;

		uint8_t ZDATA3;
		uint8_t ZDATA2;
		uint8_t ZDATA1;
	};
	uint8_t raw[9];
} AlxAdxl355_RegGroupVal_0x08_0x10_DATA;


//------------------------------------------------------------------------------
// FIFO ACCESS REGISTER GROUP
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t XDATA3;
		uint8_t XDATA2;
		uint8_t XDATA1_Xmarker : 1;
		uint8_t XDATA1_EmptyIndicator : 1;
		uint8_t XDATA1_unused_2_3 : 2;
		uint8_t XDATA1 : 4;

		uint8_t YDATA3;
		uint8_t YDATA2;
		uint8_t YDATA1_Xmarker : 1;
		uint8_t YDATA1_EmptyIndicator : 1;
		uint8_t YDATA1_unused_2_3 : 2;
		uint8_t YDATA1 : 4;

		uint8_t ZDATA3;
		uint8_t ZDATA2;
		uint8_t ZDATA1_Xmarker : 1;
		uint8_t ZDATA1_EmptyIndicator : 1;
		uint8_t ZDATA1_unused_2_3 : 2;
		uint8_t ZDATA1 : 4;
	} xyz_20bit[32];
	uint8_t raw[288];
} AlxAdxl355_RegGroupVal_0x11_FIFO_DATA;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x00_DEVID_AD val;
} AlxAdxl355_Reg_0x00_DEVID_AD;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x01_DEVID_MST val;
} AlxAdxl355_Reg_0x01_DEVID_MST;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x02_PARTID val;
} AlxAdxl355_Reg_0x02_PARTID;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x03_REVID val;
} AlxAdxl355_Reg_0x03_REVID;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x04_Status val;
} AlxAdxl355_Reg_0x04_Status;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x28_Filter val;
} AlxAdxl355_Reg_0x28_Filter;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x29_FIFO_SAMPLES val;
} AlxAdxl355_Reg_0x29_FIFO_SAMPLES;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x2A_INT_MAP val;
} AlxAdxl355_Reg_0x2A_INT_MAP;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x2B_Sync val;
} AlxAdxl355_Reg_0x2B_Sync;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x2C_Range val;
} AlxAdxl355_Reg_0x2C_Range;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x2D_POWER_CTL val;
} AlxAdxl355_Reg_0x2D_POWER_CTL;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegVal_0x2F_Reset val;
} AlxAdxl355_Reg_0x2F_Reset;


//******************************************************************************
// Register Group Structures
//******************************************************************************
typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegGroupVal_0x06_0x07_TEMP val;
} AlxAdxl355_RegGroup_0x06_0x07_TEMP;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegGroupVal_0x08_0x10_DATA val;
} AlxAdxl355_RegGroup_0x08_0x10_DATA;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint16_t len;
	AlxAdxl355_RegGroupVal_0x11_FIFO_DATA val;
} AlxAdxl355_RegGroup_0x11_FIFO_DATA;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct __attribute__((packed))
{
	AlxAdxl355_Reg_0x00_DEVID_AD		_0x00_DEVID_AD;
	AlxAdxl355_Reg_0x01_DEVID_MST		_0x01_DEVID_MST;
	AlxAdxl355_Reg_0x02_PARTID			_0x02_PARTID;
	AlxAdxl355_Reg_0x03_REVID			_0x03_REVID;
	AlxAdxl355_Reg_0x04_Status			_0x04_Status;
	AlxAdxl355_Reg_0x28_Filter			_0x28_Filter;
	AlxAdxl355_Reg_0x29_FIFO_SAMPLES	_0x29_FIFO_SAMPLES;
	AlxAdxl355_Reg_0x2A_INT_MAP			_0x2A_INT_MAP;
	AlxAdxl355_Reg_0x2B_Sync			_0x2B_Sync;
	AlxAdxl355_Reg_0x2C_Range			_0x2C_Range;
	AlxAdxl355_Reg_0x2D_POWER_CTL		_0x2D_POWER_CTL;
	AlxAdxl355_Reg_0x2F_Reset			_0x2F_Reset;

	AlxAdxl355_RegGroup_0x06_0x07_TEMP	_0x06_0x07_TEMP;
	AlxAdxl355_RegGroup_0x08_0x10_DATA	_0x08_0x10_DATA;
	AlxAdxl355_RegGroup_0x11_FIFO_DATA	_0x11_FIFO_DATA;
} AlxAdxl355_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct __attribute__((packed))
{
	float x_g;
	float y_g;
	float z_g;
} AlxAdxl355_Xyz_g;

typedef struct
{
	// Parameters
	AlxSpi* spi;
	uint8_t spiNumOfTries;
	uint16_t spiTimeout_ms;

	// Variables
	AlxAdxl355_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxAdxl355;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAdxl355_Ctor
(
	AlxAdxl355* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAdxl355_Init(AlxAdxl355* me, float sampleRate);
Alx_Status AlxAdxl355_DeInit(AlxAdxl355* me);
Alx_Status AlxAdxl355_Enable(AlxAdxl355* me);
Alx_Status AlxAdxl355_Disable(AlxAdxl355* me);
Alx_Status AlxAdxl355_GetData(AlxAdxl355* me, AccDataPoint* data, uint8_t len);
uint8_t AlxAdxl355_GetFifoLen(AlxAdxl355* me);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L4)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADXL355_H
