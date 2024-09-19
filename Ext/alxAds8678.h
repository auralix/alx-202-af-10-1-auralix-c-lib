/**
  ******************************************************************************
  * @file		alxAds8678.h
  * @brief		Auralix C Library - ALX Precision ADC ADS8678 Module
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
#ifndef ALX_ADS8678_H
#define ALX_ADS8678_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADS8678_FILE "alxAds8678.h"

// Assert //
#if defined(ALX_ADS8678_ASSERT_BKPT_ENABLE)
	#define ALX_ADS8678_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADS8678_FILE, expr)
#elif defined(ALX_ADS8678_ASSERT_TRACE_ENABLE)
	#define ALX_ADS8678_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADS8678_FILE, expr)
#elif defined(ALX_ADS8678_ASSERT_RST_ENABLE)
	#define ALX_ADS8678_ASSERT(expr) ALX_ASSERT_RST(ALX_ADS8678_FILE, expr)
#else
	#define ALX_ADS8678_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_ADS8678_TRACE_ENABLE)
	#define ALX_ADS8678_TRACE(...) ALX_TRACE_WRN(ALX_ADS8678_FILE, __VA_ARGS__)
#else
	#define ALX_ADS8678_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************


//------------------------------------------------------------------------------
// Auto-Scan Sequence Enable Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool CH0_EN : 1;
		bool CH1_EN : 1;
		bool CH2_EN : 1;
		bool CH3_EN : 1;
		bool CH4_EN : 1;
		bool CH5_EN : 1;
		bool CH6_EN : 1;
		bool CH7_EN : 1;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x01_AutoScanSequenceEnable;


//------------------------------------------------------------------------------
// Channel Power Down Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool CH0_PD : 1;
		bool CH1_PD : 1;
		bool CH2_PD : 1;
		bool CH3_PD : 1;
		bool CH4_PD : 1;
		bool CH5_PD : 1;
		bool CH6_PD : 1;
		bool CH7_PD : 1;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x02_ChannelPowerDown;


//------------------------------------------------------------------------------
// Feature Select Register
//------------------------------------------------------------------------------
typedef enum
{
	SDO_Conv = 0b000,
	SDO_Conv_ChAddr = 0b001,
	SDO_Conv_ChAddr_DevAddr = 0b010,
	SDO_Conv_ChAddr_DevAddr_InRange = 0b011
} AlxAds8678_RegEnum_0x03_SDO;
typedef enum
{
	DEV_ID0 = 0b00,
	DEV_ID1 = 0b01,
	DEV_ID2 = 0b10,
	DEV_ID3 = 0b11
} AlxAds8678_RegEnum_0x03_DEV;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x03_SDO SDO : 3;
		uint8_t unused_3 : 1;
		bool ALARM_EN : 1;
		uint8_t unused_5 : 1;
		AlxAds8678_RegEnum_0x03_DEV DEV : 2;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x03_FeatureSelect;


//------------------------------------------------------------------------------
// Channel Input Range Register
//------------------------------------------------------------------------------
typedef enum
{
	Range_CHn_PM_2V5xVref = 0b0000,			// PM_2V5xVref = +/-2.5V * Vref
	Range_CHn_PM_1V25xVref = 0b0001,
	Range_CHn_PM_0V625xVref = 0b0010,
	Range_CHn_PM_0V3125xVref = 0b0011,
	Range_CHn_PM_0V15625xVref = 0b1011,
	Range_CHn_0V_2V5xVref = 0b0101,			// 0V_2V5xVref = 0-2.5V * Vref
	Range_CHn_0V_1V25xVref = 0b0110,
	Range_CHn_0V_0V625xVref = 0b0111,
	Range_CHn_0V_0V3125xVref = 0b1111
} AlxAds8678_RegEnum_0x05_0x0C_Range_CHn;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x05_Channel0InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x06_Channel1InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x07_Channel2InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x08_Channel3InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x09_Channel4InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x0A_Channel5InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x0B_Channel6InputRange;
typedef union
{
	struct __attribute__((packed))
	{
		AlxAds8678_RegEnum_0x05_0x0C_Range_CHn Range_CHn : 4;
		uint8_t unused_4_7 : 4;
	};
	uint8_t raw;
} AlxAds8678_RegVal_0x0C_Channel7InputRange;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x01_AutoScanSequenceEnable val;
} AlxAds8678_Reg_0x01_AutoScanSequenceEnable;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x02_ChannelPowerDown val;
} AlxAds8678_Reg_0x02_ChannelPowerDown;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x03_FeatureSelect val;
} AlxAds8678_Reg_0x03_FeatureSelect;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x05_Channel0InputRange val;
} AlxAds8678_Reg_0x05_Channel0InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x06_Channel1InputRange val;
} AlxAds8678_Reg_0x06_Channel1InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x07_Channel2InputRange val;
} AlxAds8678_Reg_0x07_Channel2InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x08_Channel3InputRange val;
} AlxAds8678_Reg_0x08_Channel3InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x09_Channel4InputRange val;
} AlxAds8678_Reg_0x09_Channel4InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x0A_Channel5InputRange val;
} AlxAds8678_Reg_0x0A_Channel5InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x0B_Channel6InputRange val;
} AlxAds8678_Reg_0x0B_Channel6InputRange;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxAds8678_RegVal_0x0C_Channel7InputRange val;
} AlxAds8678_Reg_0x0C_Channel7InputRange;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxAds8678_Reg_0x01_AutoScanSequenceEnable	_0x01_AutoScanSequenceEnable;
	AlxAds8678_Reg_0x02_ChannelPowerDown		_0x02_ChannelPowerDown;
	AlxAds8678_Reg_0x03_FeatureSelect			_0x03_FeatureSelect;
	AlxAds8678_Reg_0x05_Channel0InputRange		_0x05_Channel0InputRange;
	AlxAds8678_Reg_0x06_Channel1InputRange		_0x06_Channel1InputRange;
	AlxAds8678_Reg_0x07_Channel2InputRange		_0x07_Channel2InputRange;
	AlxAds8678_Reg_0x08_Channel3InputRange		_0x08_Channel3InputRange;
	AlxAds8678_Reg_0x09_Channel4InputRange		_0x09_Channel4InputRange;
	AlxAds8678_Reg_0x0A_Channel5InputRange		_0x0A_Channel5InputRange;
	AlxAds8678_Reg_0x0B_Channel6InputRange		_0x0B_Channel6InputRange;
	AlxAds8678_Reg_0x0C_Channel7InputRange		_0x0C_Channel7InputRange;
} AlxAds8678_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	#define ALX_ADS8678_NUM_OF_CH 8
	float chVoltage_V[ALX_ADS8678_NUM_OF_CH];
	bool isEnabled[ALX_ADS8678_NUM_OF_CH];
} AlxAds8678_ChVoltageAll_V;
typedef struct
{
	// Parameters
	AlxSpi* spi;
	uint8_t spiNumOfTries;
	uint16_t spiTimeout_ms;
	AlxIoPin* do_nRST;

	// Variables
	AlxAds8678_Reg reg;

	// Info
	bool isInit;
	bool isInitPeriph;
	bool wasCtorCalled;
} AlxAds8678;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAds8678_Ctor
(
	AlxAds8678* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms,
	AlxIoPin* do_nRST
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAds8678_InitPeriph(AlxAds8678* me);
Alx_Status AlxAds8678_DeInitPeriph(AlxAds8678* me);
Alx_Status AlxAds8678_Init(AlxAds8678* me);
Alx_Status AlxAds8678_DeInit(AlxAds8678* me);
Alx_Status AlxAds8678_GetChVoltageAll_V(AlxAds8678* me, AlxAds8678_ChVoltageAll_V* chVoltageAll_V);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADS8678_H
