/**
  ******************************************************************************
  * @file		alxPcal6416a.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_PCAL6416A_H
#define ALX_PCAL6416A_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxI2c.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PCAL6416A_FILE "alxPcal6416a.h"

// Assert //
#if defined(_ALX_PCAL6416A_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PCAL6416A_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PCAL6416A_FILE, expr)
#elif defined(_ALX_PCAL6416A_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PCAL6416A_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PCAL6416A_FILE, expr)
#elif defined(_ALX_PCAL6416A_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PCAL6416A_ASSERT(expr) ALX_ASSERT_RST(ALX_PCAL6416A_FILE, expr)
#else
	#define ALX_PCAL6416A_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if (defined(_ALX_PCAL6416A_TRACE) || defined(_ALX_TRACE_ALL)) && !defined(ALX_PCAL6416A_TRACE_OFF)
	#define ALX_PCAL6416A_TRACE(...) ALX_TRACE_STD(ALX_PCAL6416A_FILE, __VA_ARGS__)
#else
	#define ALX_PCAL6416A_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************

//------------------------------------------------------------------------------
// Input port register pair (00h, 01h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_00h_InputPort_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_01h_InputPort_1;

//------------------------------------------------------------------------------
// Output port register pair (02h, 03h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_02h_OutputPort_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_03h_OutputPort_1;

//------------------------------------------------------------------------------
// Polarity inversion register pair (04h, 05h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_04h_PolarityInversion_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_05h_PolarityInversion_1;

//------------------------------------------------------------------------------
// Configuration register pair (06h, 07h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_06h_Configuration_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_07h_Configuration_1;

//------------------------------------------------------------------------------
// Output drive strength register pairs (40h, 41h, 42h, 43h)
//------------------------------------------------------------------------------
typedef enum
{
	OutputDriveLevel_0p25 = 0b00,
	OutputDriveLevel_0p50 = 0b01,
	OutputDriveLevel_0p75 = 0b10,
	OutputDriveLevel_1p00 = 0b11
} AlxPcal6416a_OutputDriveLevel;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPcal6416a_OutputDriveLevel P0_0 : 2;
		AlxPcal6416a_OutputDriveLevel P0_1 : 2;
		AlxPcal6416a_OutputDriveLevel P0_2 : 2;
		AlxPcal6416a_OutputDriveLevel P0_3 : 2;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_40h_OutputDriveStrength_0;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPcal6416a_OutputDriveLevel P0_4 : 2;
		AlxPcal6416a_OutputDriveLevel P0_5 : 2;
		AlxPcal6416a_OutputDriveLevel P0_6 : 2;
		AlxPcal6416a_OutputDriveLevel P0_7 : 2;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_41h_OutputDriveStrength_0;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPcal6416a_OutputDriveLevel P1_0 : 2;
		AlxPcal6416a_OutputDriveLevel P1_1 : 2;
		AlxPcal6416a_OutputDriveLevel P1_2 : 2;
		AlxPcal6416a_OutputDriveLevel P1_3 : 2;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_42h_OutputDriveStrength_1;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPcal6416a_OutputDriveLevel P1_4 : 2;
		AlxPcal6416a_OutputDriveLevel P1_5 : 2;
		AlxPcal6416a_OutputDriveLevel P1_6 : 2;
		AlxPcal6416a_OutputDriveLevel P1_7 : 2;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_43h_OutputDriveStrength_1;

//------------------------------------------------------------------------------
// Input latch register pair (44h, 45h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_44h_InputLatch_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_45h_InputLatch_1;

//------------------------------------------------------------------------------
// Pull-up/pull-down enable register pair (46h, 47h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_46h_PullUpPullDownEn_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_47h_PullUpPullDownEn_1;

//------------------------------------------------------------------------------
// Pull-up/pull-down selection register pair (48h, 49h)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_48h_PullUpPullDownSel_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_49h_PullUpPullDownSel_1;

//------------------------------------------------------------------------------
// Interrupt mask register pair (4Ah, 4Bh)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_4Ah_IrqMask_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_4Bh_IrqMask_1;

//------------------------------------------------------------------------------
// Interrupt status register pair (4Ch, 4Dh)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool P0_0 : 1;
		bool P0_1 : 1;
		bool P0_2 : 1;
		bool P0_3 : 1;
		bool P0_4 : 1;
		bool P0_5 : 1;
		bool P0_6 : 1;
		bool P0_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_4Ch_IrqStatus_0;
typedef union
{
	struct __attribute__((packed))
	{
		bool P1_0 : 1;
		bool P1_1 : 1;
		bool P1_2 : 1;
		bool P1_3 : 1;
		bool P1_4 : 1;
		bool P1_5 : 1;
		bool P1_6 : 1;
		bool P1_7 : 1;
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_4Dh_IrqStatus_1;

//------------------------------------------------------------------------------
// Output port configuration register (4Fh)
//------------------------------------------------------------------------------
typedef enum
{
	OpenDrainEn_Disabled = 0b0,
	OpenDrainEn_Enabled  = 0b1,
} AlxPcal6416a_4Fh_OpenDrainEn;
typedef union
{
	struct __attribute__((packed))
	{
		AlxPcal6416a_4Fh_OpenDrainEn ODEN0 : 1;
		AlxPcal6416a_4Fh_OpenDrainEn ODEN1 : 1;
		uint8_t unused_2_7 : 6; // bits 2..7 are unused
	};
	uint8_t raw;
} AlxPcal6416a_RegVal_4Fh_OutputPortConfig;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_00h_InputPort_0 val;
} AlxPcal6416a_Reg_00h_InputPort_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_01h_InputPort_1 val;
} AlxPcal6416a_Reg_01h_InputPort_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_02h_OutputPort_0 val;
} AlxPcal6416a_Reg_02h_OutputPort_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_03h_OutputPort_1 val;
} AlxPcal6416a_Reg_03h_OutputPort_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_04h_PolarityInversion_0 val;
} AlxPcal6416a_Reg_04h_PolarityInversion_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_05h_PolarityInversion_1 val;
} AlxPcal6416a_Reg_05h_PolarityInversion_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_06h_Configuration_0 val;
} AlxPcal6416a_Reg_06h_Configuration_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_07h_Configuration_1 val;
} AlxPcal6416a_Reg_07h_Configuration_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_40h_OutputDriveStrength_0 val;
} AlxPcal6416a_Reg_40h_OutputDriveStrength_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_41h_OutputDriveStrength_0 val;
} AlxPcal6416a_Reg_41h_OutputDriveStrength_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_42h_OutputDriveStrength_1 val;
} AlxPcal6416a_Reg_42h_OutputDriveStrength_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_43h_OutputDriveStrength_1 val;
} AlxPcal6416a_Reg_43h_OutputDriveStrength_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_44h_InputLatch_0 val;
} AlxPcal6416a_Reg_44h_InputLatch_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_45h_InputLatch_1 val;
} AlxPcal6416a_Reg_45h_InputLatch_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_46h_PullUpPullDownEn_0 val;
} AlxPcal6416a_Reg_46h_PullUpPullDownEn_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_47h_PullUpPullDownEn_1 val;
} AlxPcal6416a_Reg_47h_PullUpPullDownEn_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_48h_PullUpPullDownSel_0 val;
} AlxPcal6416a_Reg_48h_PullUpPullDownSel_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_49h_PullUpPullDownSel_1 val;
} AlxPcal6416a_Reg_49h_PullUpPullDownSel_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_4Ah_IrqMask_0 val;
} AlxPcal6416a_Reg_4Ah_IrqMask_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_4Bh_IrqMask_1 val;
} AlxPcal6416a_Reg_4Bh_IrqMask_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_4Ch_IrqStatus_0 val;
} AlxPcal6416a_Reg_4Ch_IrqStatus_0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_4Dh_IrqStatus_1 val;
} AlxPcal6416a_Reg_4Dh_IrqStatus_1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxPcal6416a_RegVal_4Fh_OutputPortConfig val;
} AlxPcal6416a_Reg_4Fh_OutputPortConfig;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxPcal6416a_Reg_00h_InputPort_0 _00h_InputPort_0;
	AlxPcal6416a_Reg_01h_InputPort_1 _01h_InputPort_1;
	AlxPcal6416a_Reg_02h_OutputPort_0 _02h_OutputPort_0;
	AlxPcal6416a_Reg_03h_OutputPort_1 _03h_OutputPort_1;
	AlxPcal6416a_Reg_04h_PolarityInversion_0 _04h_PolarityInversion_0;
	AlxPcal6416a_Reg_05h_PolarityInversion_1 _05h_PolarityInversion_1;
	AlxPcal6416a_Reg_06h_Configuration_0 _06h_Configuration_0;
	AlxPcal6416a_Reg_07h_Configuration_1 _07h_Configuration_1;
	AlxPcal6416a_Reg_40h_OutputDriveStrength_0 _40h_OutputDriveStrength_0;
	AlxPcal6416a_Reg_41h_OutputDriveStrength_0 _41h_OutputDriveStrength_0;
	AlxPcal6416a_Reg_42h_OutputDriveStrength_1 _42h_OutputDriveStrength_1;
	AlxPcal6416a_Reg_43h_OutputDriveStrength_1 _43h_OutputDriveStrength_1;
	AlxPcal6416a_Reg_44h_InputLatch_0 _44h_InputLatch_0;
	AlxPcal6416a_Reg_45h_InputLatch_1 _45h_InputLatch_1;
	AlxPcal6416a_Reg_46h_PullUpPullDownEn_0 _46h_PullUpPullDownEn_0;
	AlxPcal6416a_Reg_47h_PullUpPullDownEn_1 _47h_PullUpPullDownEn_1;
	AlxPcal6416a_Reg_48h_PullUpPullDownSel_0 _48h_PullUpPullDownSel_0;
	AlxPcal6416a_Reg_49h_PullUpPullDownSel_1 _49h_PullUpPullDownSel_1;
	AlxPcal6416a_Reg_4Ah_IrqMask_0 _4Ah_IrqMask_0;
	AlxPcal6416a_Reg_4Bh_IrqMask_1 _4Bh_IrqMask_1;
	AlxPcal6416a_Reg_4Ch_IrqStatus_0 _4Ch_IrqStatus_0;
	AlxPcal6416a_Reg_4Dh_IrqStatus_1 _4Dh_IrqStatus_1;
	AlxPcal6416a_Reg_4Fh_OutputPortConfig _4Fh_OutputPortConfig;
} AlxPcal6416a_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxPcal6416a_P0_0 = 0b0000,
	AlxPcal6416a_P0_1 = 0b0001,
	AlxPcal6416a_P0_2 = 0b0010,
	AlxPcal6416a_P0_3 = 0b0011,
	AlxPcal6416a_P0_4 = 0b0100,
	AlxPcal6416a_P0_5 = 0b0101,
	AlxPcal6416a_P0_6 = 0b0110,
	AlxPcal6416a_P0_7 = 0b0111,
	AlxPcal6416a_P1_0 = 0b1000,
	AlxPcal6416a_P1_1 = 0b1001,
	AlxPcal6416a_P1_2 = 0b1010,
	AlxPcal6416a_P1_3 = 0b1011,
	AlxPcal6416a_P1_4 = 0b1100,
	AlxPcal6416a_P1_5 = 0b1101,
	AlxPcal6416a_P1_6 = 0b1110,
	AlxPcal6416a_P1_7 = 0b1111
} AlxPcal6416a_PortPin;

typedef enum
{
	AlxPcal6416a_PullUp = 0b00,
	AlxPcal6416a_PullDown = 0b01,
	AlxPcal6416a_Inact = 0b10,
} AlxPcal6416a_Mode;

typedef struct
{
	// Parameters
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxPcal6416a_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInitPeriph;
	bool isInit;
} AlxPcal6416a;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] i2c
  * @param[in] i2cAddr
  * @param[in] i2cCheckWithRead
  * @param[in] i2cNumOfTries
  * @param[in] i2cTimeout_ms
  */
void AlxPcal6416a_Ctor
(
	AlxPcal6416a* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPcal6416a_InitPeriph(AlxPcal6416a* me);

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPcal6416a_DeInitPeriph(AlxPcal6416a* me);

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPcal6416a_Init(AlxPcal6416a* me);

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxPcal6416a_DeInit(AlxPcal6416a* me);

/**
  * @brief
  * @param[in] me
  * @param[in] inputPort0
  * @param[in] inputPort1
  * @param[in] outputPort0
  * @param[in] outputPort1
  */
Alx_Status AlxPcal6416a_Handle(AlxPcal6416a* me, bool inputPort0, bool inputPort1, bool outputPort0, bool outputPort1);

/**
  * @brief
  * @param[in] me
  * @param[in] pin
  */
bool AlxPcal6416a_IoPin_Read(AlxPcal6416a* me, AlxPcal6416a_PortPin pin);

/**
  * @brief
  * @param[in] me
  * @param[in] pin
  * @param[in] val
  */
void AlxPcal6416a_IoPin_Write(AlxPcal6416a* me, AlxPcal6416a_PortPin pin, bool val);

/**
  * @brief
  * @param[in,out] me
  * @param[in] pin
  */
void AlxPcal6416a_IoPin_Set(AlxPcal6416a* me, AlxPcal6416a_PortPin pin);

/**
  * @brief
  * @param[in,out] me
  * @param[in] pin
  */
void AlxPcal6416a_IoPin_Reset(AlxPcal6416a* me, AlxPcal6416a_PortPin pin);

/**
  * @brief
  * @param[in,out] me
  * @param[in] pin
  */
void AlxPcal6416a_IoPin_Toggle(AlxPcal6416a* me, AlxPcal6416a_PortPin pin);

/**
  * @brief
  * @param[in] me
  * @param[in] reg
  */
Alx_Status AlxPcal6416a_Reg_Write(AlxPcal6416a* me, void* reg);

/**
  * @brief
  * @param[in] me
  * @param[out] reg
  */
Alx_Status AlxPcal6416a_Reg_Read(AlxPcal6416a* me, void* reg);


#endif // #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_PCAL6416A_H




















//void AlxPcal6416a_IoPin_SetMode(AlxPcal6416a* me, AlxPcal6416a_PortPin pin, AlxPcal6416a_Mode mode);	// MF: Set PullUp or PulDown
