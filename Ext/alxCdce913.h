/**
  ******************************************************************************
  * @file		alxCdce913.h
  * @brief		Auralix C Library - ALX CDCE913 PLL Module
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
#ifndef ALX_CDCE913_H
#define ALX_CDCE913_H

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
#include "alxDac.h"
#include "alxIoPin.h"

//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32L4)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CDCE913_FILE "alxCdce913.h"

// Assert //
#if defined(ALX_CDCE913_ASSERT_BKPT_ENABLE)
#define ALX_CDCE913_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CDCE913_FILE, expr)
#elif defined(ALX_CDCE913_ASSERT_TRACE_ENABLE)
#define ALX_CDCE913_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CDCE913_FILE, expr)
#elif defined(ALX_CDCE913_ASSERT_RST_ENABLE)
#define ALX_CDCE913_ASSERT(expr) ALX_ASSERT_RST(ALX_CDCE913_FILE, expr)
#else
#define ALX_CDCE913_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_CDCE913_TRACE_ENABLE)
#define ALX_CDCE913_TRACE_WRN(...) ALX_TRACE_WRN(ALX_CDCE913_FILE, __VA_ARGS__)
#define ALX_CDCE913_TRACE_INF(...) ALX_TRACE_INF(ALX_CDCE913_FILE, __VA_ARGS__)
#else
#define ALX_CDCE913_TRACE_WRN(...) do{} while (false)
#define ALX_CDCE913_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Register Enums
//******************************************************************************
typedef enum
{
	DEV_ID_Cdce913 = 0,
	DEV_ID_Cdcel913 = 1
} AlxCdce913_RegEnum_DEV_ID;

typedef enum
{
	EEPROM_PROG_Complete = 0,
	EEPROM_PROG_Programing = 1
} AlxCdce913_RegEnum_EEPROM_PROG;

typedef enum
{
	EEPROM_LOCK_NotLocked = 0,
	EEPROM_LOCK_Locked = 1
} AlxCdce913_RegEnum_EEPROM_LOCK;

typedef enum
{
	PWDN_DeviceActive = 0,
	PWDN_DevicePoweredDown = 1
} AlxCdce913_RegEnum_PWDN;

typedef enum
{
	INCLK_Xtal = 0,
	INCLK_Vcxo = 1,
	INCLK_Lvcmos = 2
} AlxCdce913_RegEnum_INCLK;

typedef enum
{
	Y1_CLK_Input = 0,
	Y1_CLK_Pll = 1
} AlxCdce913_RegEnum_Y1_CLK;

typedef enum
{
	SPICON_Spi = 0,
	SPICON_Con = 1
} AlxCdce913_RegEnum_SPICON;

typedef enum
{
	Y1_ST_DevicePowerDown = 0,
	Y1_ST_DisabledHiZ = 1,
	Y1_ST_DisabledLow = 2,
	Y1_ST_Enabled = 3
} AlxCdce913_RegEnum_Y1_ST;

typedef enum
{
	Y1_X_State0 = 0,
	Y1_X_State1 = 1
} AlxCdce913_RegEnum_Y1_X;

typedef enum
{
	SSC_MOD_AMOUNT_Off = 0,
	SSC_MOD_AMOUNT_0p25 = 1,
	SSC_MOD_AMOUNT_0p50 = 2,
	SSC_MOD_AMOUNT_0p75 = 3,
	SSC_MOD_AMOUNT_1p00 = 4,
	SSC_MOD_AMOUNT_1p25 = 5,
	SSC_MOD_AMOUNT_1p50 = 6,
	SSC_MOD_AMOUNT_2p00 = 7
} AlxCdce913_RegEnum_SSC_MOD_AMOUNT;

typedef enum
{
	PLL1_FREQ_SEL_Pll1_0 = 0,
	PLL1_FREQ_SEL_Pll1_1 = 1,
} AlxCdce913_RegEnum_PLL1_FREQ_SEL;

typedef enum
{
	MUX1_Pll = 0,
	MUX1_PllBypass = 1
} AlxCdce913_RegEnum_MUX1;

typedef enum
{
	Y2_MUX_Pdiv1 = 0,
	Y2_MUX_Pdiv2 = 1
} AlxCdce913_RegEnum_Y2_MUX;

typedef enum
{
	Y3_MUX_Pdiv1 = 0,
	Y3_MUX_Pdiv2 = 1,
	Y3_MUX_Pdiv3 = 2
} AlxCdce913_RegEnum_Y3_MUX;

typedef enum
{
	Y2Y3_ST_DevicePowerDown = 0,
	Y2Y3_ST_DisabledHiZ = 1,
	Y2Y3_ST_DisabledLow = 2,
	Y2Y3_ST_Enabled = 3
} AlxCdce913_RegEnum_Y2Y3_ST;

typedef enum
{
	Y2Y3_X_State0 = 0,
	Y2Y3_X_State1 = 1
} AlxCdce913_RegEnum_Y2Y3_X;

typedef enum
{
	PLL1_SSC_SEL_Down = 0,
	PLL1_SSC_SEL_Center = 1
} AlxCdce913_RegEnum_PLL1_SSC_SEL;

typedef enum
{
	VCO1_X_RANGE_Min125Mhz = 0,
	VCO1_X_RANGE_125Mhz150Mhz = 0,
	VCO1_X_RANGE_150Mhz175Mhz = 0,
	VCO1_X_RANGE_175MhzMax = 0,
} AlxCdce913_RegEnum_VCO1_X_RANGE;


//******************************************************************************
// Register Values Unions
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VEN_ID : 4;
		uint8_t REV_ID : 3;
		uint8_t DEV_ID : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_ID;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t TARGET_ADR : 2;
		uint8_t INCLK : 2;
		uint8_t PWDN : 1;
		uint8_t EELOCK : 1;
		uint8_t EEPIP : 1;
		uint8_t unused_7 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_DEV_CTRL0;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PDIV1_8_9 : 2;
		uint8_t Y1_ST0 : 2;
		uint8_t Y1_ST1 : 2;
		uint8_t SPICON : 1;
		uint8_t M1 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_DEV_CTRL1;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PDIV1_0_7;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PDIV1_LSB;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Y1_0 : 1;
		uint8_t Y1_1 : 1;
		uint8_t Y1_2 : 1;
		uint8_t Y1_3 : 1;
		uint8_t Y1_4 : 1;
		uint8_t Y1_5 : 1;
		uint8_t Y1_6 : 1;
		uint8_t Y1_7 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_CON_Y1;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_2 : 3;
		uint8_t XCSEL : 5;
	};
	uint8_t raw;
} AlxCdce913_RegVal_XCSEL;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t EEWRITE : 1;
		uint8_t BCOUNT : 7;
	};
	uint8_t raw;
} AlxCdce913_RegVal_DEV_CTRL2;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t SSC1_5_1_2 : 2;
		uint8_t SSC1_6 : 3;
		uint8_t SSC1_7 : 3;
	};
	uint8_t raw;
} AlxCdce913_RegVal_CON_SSC0;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t SSC1_2_2 : 1;
		uint8_t SSC1_3 : 3;
		uint8_t SSC1_4 : 3;
		uint8_t SSC1_5_0 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_CON_SSC1;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t SSC1_0 : 3;
		uint8_t SSC1_1 : 3;
		uint8_t SSC1_2_0_1 : 2;
	};
	uint8_t raw;
} AlxCdce913_RegVal_CON_SSC2;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t FS1_0 : 1;
		uint8_t FS1_1 : 1;
		uint8_t FS1_2 : 1;
		uint8_t FS1_3 : 1;
		uint8_t FS1_4 : 1;
		uint8_t FS1_5 : 1;
		uint8_t FS1_6 : 1;
		uint8_t FS1_7 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_CON_FS;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Y2Y3_ST0 : 2;
		uint8_t Y2Y3_ST1 : 2;
		uint8_t M3 : 2;
		uint8_t M2 : 1;
		uint8_t MUX1 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_DEV_CTRL3;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Y2Y31_0 : 1;
		uint8_t Y2Y31_1 : 1;
		uint8_t Y2Y31_2 : 1;
		uint8_t Y2Y31_3 : 1;
		uint8_t Y2Y31_4 : 1;
		uint8_t Y2Y31_5 : 1;
		uint8_t Y2Y31_6 : 1;
		uint8_t Y2Y31_7 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_CON_Y2Y3;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PDIV2 : 7;
		uint8_t SSC1DC : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_DEV_CTRL4;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PDIV3 : 7;
		uint8_t unused_7 : 1;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PDIV3;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PLL1_0N_4_11;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_0_CFG0;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PLL1_0R_5_8 : 4;
		uint8_t PLL1_0N_0_3 : 4;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_0_CFG1;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PLL1_0Q_3_5 : 3;
		uint8_t PLL1_0R_0_4 : 5;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_0_CFG2;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VCO1_0_RANGE : 2;
		uint8_t PLL1_0P : 3;
		uint8_t PLL1_0Q_0_2 : 3;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_0_CFG3;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PLL1_1N_4_11;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_1_CFG0;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PLL1_1R_5_8 : 4;
		uint8_t PLL1_1N_0_3 : 4;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_1_CFG1;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PLL1_1Q_3_5 : 3;
		uint8_t PLL1_1R_0_4 : 5;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_1_CFG2;

typedef union
{
	struct __attribute__((packed))
	{
		uint8_t VCO1_1_RANGE : 2;
		uint8_t PLL1_1P : 3;
		uint8_t PLL1_1Q_0_2 : 3;
	};
	uint8_t raw;
} AlxCdce913_RegVal_PLL1_1_CFG3;


//******************************************************************************
// Register Structures
//******************************************************************************
#define _ALX_CDCE913_REG(name)	typedef struct						\
								{									\
									uint8_t addr;					\
									AlxCdce913_RegVal_##name val;	\
								} AlxCdce913_Reg_##name

_ALX_CDCE913_REG(ID);
_ALX_CDCE913_REG(DEV_CTRL0);
_ALX_CDCE913_REG(DEV_CTRL1);
_ALX_CDCE913_REG(PDIV1_LSB);
_ALX_CDCE913_REG(CON_Y1);
_ALX_CDCE913_REG(XCSEL);
_ALX_CDCE913_REG(DEV_CTRL2);
_ALX_CDCE913_REG(CON_SSC0);
_ALX_CDCE913_REG(CON_SSC1);
_ALX_CDCE913_REG(CON_SSC2);
_ALX_CDCE913_REG(CON_FS);
_ALX_CDCE913_REG(DEV_CTRL3);
_ALX_CDCE913_REG(CON_Y2Y3);
_ALX_CDCE913_REG(DEV_CTRL4);
_ALX_CDCE913_REG(PDIV3);
_ALX_CDCE913_REG(PLL1_0_CFG0);
_ALX_CDCE913_REG(PLL1_0_CFG1);
_ALX_CDCE913_REG(PLL1_0_CFG2);
_ALX_CDCE913_REG(PLL1_0_CFG3);
_ALX_CDCE913_REG(PLL1_1_CFG0);
_ALX_CDCE913_REG(PLL1_1_CFG1);
_ALX_CDCE913_REG(PLL1_1_CFG2);
_ALX_CDCE913_REG(PLL1_1_CFG3);


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxCdce913_Reg_ID ID;
	AlxCdce913_Reg_DEV_CTRL0 DEV_CTRL0;
	AlxCdce913_Reg_DEV_CTRL1 DEV_CTRL1;
	AlxCdce913_Reg_PDIV1_LSB PDIV1_LSB;
	AlxCdce913_Reg_CON_Y1 CON_Y1;
	AlxCdce913_Reg_XCSEL XCSEL;
	AlxCdce913_Reg_DEV_CTRL2 DEV_CTRL2;
	AlxCdce913_Reg_CON_SSC0 CON_SSC0;
	AlxCdce913_Reg_CON_SSC1 CON_SSC1;
	AlxCdce913_Reg_CON_SSC2 CON_SSC2;
	AlxCdce913_Reg_CON_FS CON_FS;
	AlxCdce913_Reg_DEV_CTRL3 DEV_CTRL3;
	AlxCdce913_Reg_CON_Y2Y3 CON_Y2Y3;
	AlxCdce913_Reg_DEV_CTRL4 DEV_CTRL4;
	AlxCdce913_Reg_PDIV3 PDIV3;
	AlxCdce913_Reg_PLL1_0_CFG0 PLL1_0_CFG0;
	AlxCdce913_Reg_PLL1_0_CFG1 PLL1_0_CFG1;
	AlxCdce913_Reg_PLL1_0_CFG2 PLL1_0_CFG2;
	AlxCdce913_Reg_PLL1_0_CFG3 PLL1_0_CFG3;
	AlxCdce913_Reg_PLL1_1_CFG0 PLL1_1_CFG0;
	AlxCdce913_Reg_PLL1_1_CFG1 PLL1_1_CFG1;
	AlxCdce913_Reg_PLL1_1_CFG2 PLL1_1_CFG2;
	AlxCdce913_Reg_PLL1_1_CFG3 PLL1_1_CFG3;
} AlxCdce913_Reg;

//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	ALX_CDCE913_OUTPUT_Y1_1024KHZ0,
	ALX_CDCE913_OUTPUT_Y1_819KHZ2,
} AlxCdce913_OutputY1;

typedef struct
{
	// Parameters
	AlxI2c* i2c;
	AlxDac* dac;
	float dacReference_V;
	Alx_Ch dacChannel;
	AlxIoPin* pllEnable;

	// Variables
	AlxCdce913_Reg reg;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;
	float xtalFrequency_MHz;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxCdce913;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCdce913_Ctor
(
	AlxCdce913* me,
	AlxI2c* i2c,
	AlxDac* dac,
	float dacReference_V,
	Alx_Ch dacChannel,
	AlxIoPin* pllEnable
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxCdce913_Init
(
	AlxCdce913* me,
	float xtalFrequency,
	AlxCdce913_RegEnum_INCLK xtalSource,
	uint8_t xtalCapacitance_pF
);
Alx_Status AlxCdce913_DeInit(AlxCdce913* me);
Alx_Status AlxCdce913_SetY1OutputFreq(AlxCdce913* me, AlxCdce913_OutputY1 freq);
Alx_Status AlxCdce913_SetVctrl(AlxCdce913* me, float vctrl_V);
Alx_Status AlxCdce913_EnableY1(AlxCdce913* me);
Alx_Status AlxCdce913_DisableY1(AlxCdce913* me);

#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L4)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CDCE913_H
