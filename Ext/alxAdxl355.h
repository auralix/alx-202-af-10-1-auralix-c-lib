/**
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
#include "alxTimSw.h"
#include "alxIoPin.h"
#include "alxSpi.h"
#include "alxFifo.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADXL355_FILE "alxAdxl355.h"

// Assert //
#if defined(_ALX_ADXL355_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_ADXL355_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADXL355_FILE, expr)
#elif defined(_ALX_ADXL355_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_ADXL355_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADXL355_FILE, expr)
#elif defined(_ALX_ADXL355_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_ADXL355_ASSERT(expr) ALX_ASSERT_RST(ALX_ADXL355_FILE, expr)
#else
	#define ALX_ADXL355_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_ADXL355_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_ADXL355_TRACE(...) ALX_TRACE_STD(ALX_ADXL355_FILE, __VA_ARGS__)
	#define ALX_ADXL355_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_ADXL355_TRACE(...) do{} while (false)
	#define ALX_ADXL355_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//******************************************************************************
// Address: 0x00, Reset: 0xAD, Name: DEVID_AD
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t DEVID_AD;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x00_DEVID_AD;


//******************************************************************************
// Address: 0x01, Reset: 0x1D, Name: DEVID_MST
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t DEVID_MST;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x01_DEVID_MST;


//******************************************************************************
// Address: 0x02, Reset: 0xED, Name: PARTID
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PARTID;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x02_PARTID;


//******************************************************************************
// Address: 0x03, Reset: 0x01, Name: REVID
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t REVID;
	};
	uint8_t raw;
} AlxAdxl355_RegVal_0x03_REVID;


//******************************************************************************
// Address: 0x04, Reset: 0x00, Name: Status
//******************************************************************************
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


//******************************************************************************
// Address: 0x28, Reset: 0x00, Name: Filter
//******************************************************************************
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


//******************************************************************************
// Address: 0x2A, Reset: 0x00, Name: INT_MAP
//******************************************************************************
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


//******************************************************************************
// Address: 0x2B, Reset: 0x00, Name: Sync
//******************************************************************************
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


//******************************************************************************
// Address: 0x2C, Reset: 0x81, Name: Range
//******************************************************************************
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


//******************************************************************************
// Address: 0x2D, Reset: 0x01, Name: POWER_CTL
//******************************************************************************
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


//******************************************************************************
// Address: 0x2F, Reset: 0x00, Name: Reset
//******************************************************************************
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


//******************************************************************************
// Address: 0x06, Reset: 0x00, Name: TEMP2
// Address: 0x07, Reset: 0x00, Name: TEMP1
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t TEMP2;
		uint8_t TEMP1;
	};
	uint8_t raw[2];
} AlxAdxl355_RegGroupVal_0x06_0x07_TEMP;


//******************************************************************************
// Address: 0x08, Reset: 0x00, Name: XDATA3
// Address: 0x09, Reset: 0x00, Name: XDATA2
// Address: 0x0A, Reset: 0x00, Name: XDATA1
// Address: 0x0B, Reset: 0x00, Name: YDATA3
// Address: 0x0C, Reset: 0x00, Name: YDATA2
// Address: 0x0D, Reset: 0x00, Name: YDATA1
// Address: 0x0E, Reset: 0x00, Name: ZDATA3
// Address: 0x0F, Reset: 0x00, Name: ZDATA2
// Address: 0x10, Reset: 0x00, Name: ZDATA1
//******************************************************************************
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


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x00_DEVID_AD val;
} AlxAdxl355_Reg_0x00_DEVID_AD;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x01_DEVID_MST val;
} AlxAdxl355_Reg_0x01_DEVID_MST;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x02_PARTID val;
} AlxAdxl355_Reg_0x02_PARTID;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x03_REVID val;
} AlxAdxl355_Reg_0x03_REVID;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x04_Status val;
} AlxAdxl355_Reg_0x04_Status;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x28_Filter val;
} AlxAdxl355_Reg_0x28_Filter;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x2A_INT_MAP val;
} AlxAdxl355_Reg_0x2A_INT_MAP;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x2B_Sync val;
} AlxAdxl355_Reg_0x2B_Sync;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x2C_Range val;
} AlxAdxl355_Reg_0x2C_Range;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x2D_POWER_CTL val;
} AlxAdxl355_Reg_0x2D_POWER_CTL;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegVal_0x2F_Reset val;
} AlxAdxl355_Reg_0x2F_Reset;


//******************************************************************************
// Register Group Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegGroupVal_0x06_0x07_TEMP val;
} AlxAdxl355_RegGroup_0x06_0x07_TEMP;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAdxl355_RegGroupVal_0x08_0x10_DATA val;
} AlxAdxl355_RegGroup_0x08_0x10_DATA;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxAdxl355_Reg_0x00_DEVID_AD	_0x00_DEVID_AD	;
	AlxAdxl355_Reg_0x01_DEVID_MST	_0x01_DEVID_MST	;
	AlxAdxl355_Reg_0x02_PARTID		_0x02_PARTID	;
	AlxAdxl355_Reg_0x03_REVID		_0x03_REVID		;
	AlxAdxl355_Reg_0x04_Status		_0x04_Status	;
	AlxAdxl355_Reg_0x28_Filter		_0x28_Filter	;
	AlxAdxl355_Reg_0x2A_INT_MAP		_0x2A_INT_MAP	;
	AlxAdxl355_Reg_0x2B_Sync		_0x2B_Sync		;
	AlxAdxl355_Reg_0x2C_Range		_0x2C_Range		;
	AlxAdxl355_Reg_0x2D_POWER_CTL	_0x2D_POWER_CTL	;
	AlxAdxl355_Reg_0x2F_Reset		_0x2F_Reset		;

	AlxAdxl355_RegGroup_0x06_0x07_TEMP	_0x06_0x07_TEMP	;
	AlxAdxl355_RegGroup_0x08_0x10_DATA	_0x08_0x10_DATA	;
} AlxAdxl355_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	float x_g;
	float y_g;
	float z_g;
} AlxAdxl355_Xyz_g;

typedef union
{
	struct __attribute__((packed))
	{
		int32_t x_20bit;
		int32_t y_20bit;
		int32_t z_20bit;
	};
	uint8_t raw[12];
} AlxAdxl355_Xyz_20bit;

typedef union
{
	uint16_t val;
	uint8_t raw[2];
} AlxAdxl355_Temp_12bit;

typedef struct
{
	// Objects - External
	AlxSpi* spi;

	// Objects - Internal
	AlxFifo fifo;

	// Parameters
	uint8_t spiNumOfTries;
	uint16_t spiTimeout_ms;
	uint8_t* fifoBuff;
	uint32_t fifoBuffLen;

	// Variables
	float rangeFactor;
	AlxAdxl355_Xyz_g xyz_g;
	AlxAdxl355_Xyz_20bit xyz_20bit;

	AlxAdxl355_Temp_12bit temp_12bit;
	float temp_degC;

	AlxAdxl355_Reg reg;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxAdxl355;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		spi
  * @param[in]		spiNumOfTries
  * @param[in]		spiTimeout_ms
  * @param[in]		fifoBuff
  * @param[in]		fifoBuffLen
  */
void AlxAdxl355_Ctor
(
	AlxAdxl355* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	uint8_t* fifoBuff,
	uint32_t fifoBuffLen
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_Init(AlxAdxl355* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_DeInit(AlxAdxl355* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_Enable(AlxAdxl355* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_Disable(AlxAdxl355* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		xyz_g
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetXyz_g(AlxAdxl355* me, AlxAdxl355_Xyz_g* xyz_g);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		xyz_g
  * @param[in]		len
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_GetXyzMulti_g(AlxAdxl355* me, AlxAdxl355_Xyz_g* xyz_g, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
float AlxAdxl355_GetTemp_degC(AlxAdxl355* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxAdxl355_Foreground_Handle(AlxAdxl355* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADXL355_H
