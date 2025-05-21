/**
  ******************************************************************************
  * @file		alxA352.h
  * @brief		Auralix C Library - ALX Accelerometer Epson A352 Module
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
#ifndef ALX_A352_H
#define ALX_A352_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxSpi.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_A352_FILE "alxA352.h"

// Assert //
#if defined(ALX_A352_ASSERT_BKPT_ENABLE)
#define ALX_A352_ASSERT(expr) ALX_ASSERT_BKPT(ALX_A352_FILE, expr)
#elif defined(ALX_A352_ASSERT_TRACE_ENABLE)
#define ALX_A352_ASSERT(expr) ALX_ASSERT_TRACE(ALX_A352_FILE, expr)
#elif defined(ALX_A352_ASSERT_RST_ENABLE)
#define ALX_A352_ASSERT(expr) ALX_ASSERT_RST(ALX_A352_FILE, expr)
#else
#define ALX_A352_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_A352_TRACE_ENABLE)
#define ALX_A352_TRACE_WRN(...) ALX_TRACE_WRN(ALX_A352_FILE, __VA_ARGS__)
#define ALX_A352_TRACE_INF(...) ALX_TRACE_INF(ALX_A352_FILE, __VA_ARGS__)
#else
#define ALX_A352_TRACE_WRN(...) do{} while (false)
#define ALX_A352_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint16_t BURST_CMD   : 8;
		uint16_t unused_8_15 : 8;
	};
	uint16_t raw;
} AlxA352_RegVal_BURST;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t unused_0_7   : 8;
		uint16_t MODE_CMD     : 2;
		uint16_t MODE_STAT    : 2;
		uint16_t unused_12_15 : 4;
	};
	uint16_t raw;
} AlxA352_RegVal_MODE_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t FLASH_BU_ERR : 1;
		uint16_t ACC_ERR_ALL  : 1;
		uint16_t FLASH_ERR    : 1;
		uint16_t UART_OVF     : 1;
		uint16_t SPI_OVF      : 1;
		uint16_t HARD_ERR     : 3;
		uint16_t VDD_ERR      : 1;
		uint16_t TEMP_ERR     : 1;
		uint16_t SENS_ERR     : 2;
		uint16_t ACC_Z_ERR    : 1;
		uint16_t ACC_Y_ERR    : 1;
		uint16_t ACC_X_ERR    : 1;
		uint16_t ACC_VS_ERR   : 1;
	};
	uint16_t raw;
} AlxA352_RegVal_DIAG_STAT;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t EA           : 1;
		uint16_t ALIASI_ERR   : 1;
		uint16_t ZALARM_ERR   : 1;
		uint16_t YALARM_ERR   : 1;
		uint16_t XALARM_ERR   : 1;
		uint16_t unused_5_8   : 4;
		uint16_t ND_ZACCL     : 1;
		uint16_t ND_YACCL     : 1;
		uint16_t ND_XACCL     : 1;
		uint16_t unused_12_14 : 3;
		uint16_t ND_TEMP      : 1;
	};
	uint16_t raw;
} AlxA352_RegVal_FLAG;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t COUNT;
	};
	uint16_t raw;
} AlxA352_RegVal_COUNT;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t TEMP_HIGH;
		uint16_t TEMP_LOW;
	};
	uint16_t raw[2];
} AlxA352_RegVal_TEMP;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t XACCL_HIGH;
		uint16_t XACCL_LOW;
		uint16_t YACCL_HIGH;
		uint16_t YACCL_LOW;
		uint16_t ZACCL_HIGH;
		uint16_t ZACCL_LOW;
	};
	uint16_t raw[6];
} AlxA352_RegVal_ACCL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t unused_0_1   : 2;
		uint16_t TEMP_STABIL  : 1;
		uint16_t unused_3     : 1;
		uint16_t MESMOD_SEL   : 1;
		uint16_t OUTPUT_SEL_Z : 1;
		uint16_t OUTPUT_SEL_Y : 1;
		uint16_t OUTPUT_SEL_X : 1;
		uint16_t unused_8     : 1;
		uint16_t ND_EN_ZACCL  : 1;
		uint16_t ND_EN_YACCL  : 1;
		uint16_t ND_EN_XACCL  : 1;
		uint16_t unused_12_14 : 3;
		uint16_t ND_EN_TEMP   : 1;
	};
	uint16_t raw;
} AlxA352_RegVal_SIG_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t unused_0    : 1;
		uint16_t DRDY_POL    : 1;
		uint16_t DRDY_ON     : 1;
		uint16_t unused_3_4  : 2;
		uint16_t EXT_POL     : 1;
		uint16_t EXT_SEL     : 1;
		uint16_t unused_7    : 1;
		uint16_t VDD_TEST    : 1;
		uint16_t TEMP_TEST   : 1;
		uint16_t ACC_TEST    : 1;
		uint16_t FLASH_TEST  : 1;
		uint16_t X_SENS_TEST : 1;
		uint16_t Y_SENS_TEST : 1;
		uint16_t Z_SENS_TEST : 1;
		uint16_t unused_15   : 1;
	};
	uint16_t raw;
} AlxA352_RegVal_MSC_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t unused_0_7   : 8;
		uint16_t DOUT_RATE    : 4;
		uint16_t unused_12_15 : 4;
	};
	uint16_t raw;
} AlxA352_RegVal_SMPL_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t FILTER_SEL  :  4;
		uint16_t unused_4    :  1;
		uint16_t FILTER_STAT :  1;
		uint16_t unused_6_15 : 10;
	};
	uint16_t raw;
} AlxA352_RegVal_FILTER_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t unused_0_1   : 2;
		uint16_t FLASH_RST    : 1;
		uint16_t FLASH_BACKUP : 1;
		uint16_t unused_4_6   : 3;
		uint16_t SOFT_RST     : 1;
		uint16_t unused_8_9   : 2;
		uint16_t NOT_READY    : 1;
		uint16_t unused_11    : 1;
		uint16_t MESMOD_STAT  : 1;
		uint16_t unused_13_15 : 3;
	};
	uint16_t raw;
} AlxA352_RegVal_GLOB_CMD;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t CHKSM_OUT    : 1;
		uint16_t COUNT_OUT    : 1;
		uint16_t unused_2_7   : 6;
		uint16_t ACCZ_OUT     : 1;
		uint16_t ACCY_OUT     : 1;
		uint16_t ACCX_OUT     : 1;
		uint16_t unused_11_13 : 3;
		uint16_t TEMP_OUT     : 1;
		uint16_t FLAG_OUT     : 1;
	};
	uint16_t raw;
} AlxA352_RegVal_BURST_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t FILT_EN     :  1;
		uint16_t FILT_SEL    :  1;
		uint16_t unused_2_15 : 14;
	};
	uint16_t raw;
} AlxA352_RegVal_LONGFILT_CTRL;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t TAP_SIZE    :  4;
		uint16_t unused_4_15 : 12;
	};
	uint16_t raw;
} AlxA352_RegVal_LONGFILT_TAP;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t XOFFSET_HIGH;
	};
	uint16_t raw;
} AlxA352_RegVal_XOFFSET_HIGH;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t XOFFSET_LOW;
	};
	uint16_t raw;
} AlxA352_RegVal_XOFFSET_LOW;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t YOFFSET_HIGH;
	};
	uint16_t raw;
} AlxA352_RegVal_YOFFSET_HIGH;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t YOFFSET_LOW;
	};
	uint16_t raw;
} AlxA352_RegVal_YOFFSET_LOW;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t ZOFFSET_HIGH;
	};
	uint16_t raw;
} AlxA352_RegVal_ZOFFSET_HIGH;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t ZOFFSET_LOW;
	};
	uint16_t raw;
} AlxA352_RegVal_ZOFFSET_LOW;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t PROD_ID1;
		uint16_t PROD_ID2;
		uint16_t PROD_ID3;
		uint16_t PROD_ID4;
	};
	uint16_t raw[4];
} AlxA352_RegVal_PROD_ID;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t VERSION;
	};
	uint16_t raw;
} AlxA352_RegVal_VERSION;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t SERIAL_NUM1;
		uint16_t SERIAL_NUM2;
		uint16_t SERIAL_NUM3;
		uint16_t SERIAL_NUM4;
	};
	uint16_t raw[4];
} AlxA352_RegVal_SERIAL_NUM;

typedef union
{
	struct __attribute__((packed))
	{
		uint16_t WINDOW_ID;
	};
	uint16_t raw;
} AlxA352_RegVal_WIN_CTRL;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t high : 1;
		uint8_t low  : 1;
		uint8_t unused : 6;
	};
	uint8_t raw;
} AlxA352_RegWriteMeta;

typedef struct __attribute__((packed))
{
	uint8_t addr;
	uint8_t win;
	uint8_t len;
	AlxA352_RegWriteMeta w;
} AlxA352_RegMeta;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_BURST val;
} AlxA352_Reg_BURST;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_MODE_CTRL val;
} AlxA352_Reg_MODE_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_DIAG_STAT val;
} AlxA352_Reg_DIAG_STAT;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_FLAG val;
} AlxA352_Reg_FLAG;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_COUNT val;
} AlxA352_Reg_COUNT;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_TEMP val;
} AlxA352_Reg_TEMP;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_ACCL val;
} AlxA352_Reg_ACCL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_SIG_CTRL val;
} AlxA352_Reg_SIG_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_MSC_CTRL val;
} AlxA352_Reg_MSC_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_SMPL_CTRL val;
} AlxA352_Reg_SMPL_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_FILTER_CTRL val;
} AlxA352_Reg_FILTER_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_GLOB_CMD val;
} AlxA352_Reg_GLOB_CMD;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_BURST_CTRL val;
} AlxA352_Reg_BURST_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_LONGFILT_CTRL val;
} AlxA352_Reg_LONGFILT_CTRL;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_LONGFILT_TAP val;
} AlxA352_Reg_LONGFILT_TAP;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_XOFFSET_HIGH val;
} AlxA352_Reg_XOFFSET_HIGH;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_XOFFSET_LOW val;
} AlxA352_Reg_XOFFSET_LOW;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_YOFFSET_HIGH val;
} AlxA352_Reg_YOFFSET_HIGH;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_YOFFSET_LOW val;
} AlxA352_Reg_YOFFSET_LOW;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_ZOFFSET_HIGH val;
} AlxA352_Reg_ZOFFSET_HIGH;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_ZOFFSET_LOW val;
} AlxA352_Reg_ZOFFSET_LOW;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_PROD_ID val;
} AlxA352_Reg_PROD_ID;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_VERSION val;
} AlxA352_Reg_VERSION;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_SERIAL_NUM val;
} AlxA352_Reg_SERIAL_NUM;

typedef struct __attribute__((packed))
{
	AlxA352_RegMeta meta;
	AlxA352_RegVal_WIN_CTRL val;
} AlxA352_Reg_WIN_CTRL;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct __attribute__((packed))
{
	AlxA352_Reg_BURST BURST;
	AlxA352_Reg_MODE_CTRL MODE_CTRL;
	AlxA352_Reg_DIAG_STAT DIAG_STAT;
	AlxA352_Reg_FLAG FLAG;
	AlxA352_Reg_COUNT COUNT;
	AlxA352_Reg_TEMP TEMP;
	AlxA352_Reg_ACCL ACCL;
	AlxA352_Reg_SIG_CTRL SIG_CTRL;
	AlxA352_Reg_MSC_CTRL MSC_CTRL;
	AlxA352_Reg_SMPL_CTRL SMPL_CTRL;
	AlxA352_Reg_FILTER_CTRL FILTER_CTRL;
	AlxA352_Reg_GLOB_CMD GLOB_CMD;
	AlxA352_Reg_BURST_CTRL BURST_CTRL;
	AlxA352_Reg_LONGFILT_CTRL LONGFILT_CTRL;
	AlxA352_Reg_LONGFILT_TAP LONGFILT_TAP;
	AlxA352_Reg_XOFFSET_HIGH XOFFSET_HIGH;
	AlxA352_Reg_XOFFSET_LOW XOFFSET_LOW;
	AlxA352_Reg_YOFFSET_HIGH YOFFSET_HIGH;
	AlxA352_Reg_YOFFSET_LOW YOFFSET_LOW;
	AlxA352_Reg_ZOFFSET_HIGH ZOFFSET_HIGH;
	AlxA352_Reg_ZOFFSET_LOW ZOFFSET_LOW;
	AlxA352_Reg_PROD_ID PROD_ID;
	AlxA352_Reg_VERSION VERSION;
	AlxA352_Reg_SERIAL_NUM SERIAL_NUM;
	AlxA352_Reg_WIN_CTRL WIN_CTRL;
} AlxA352_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxSpi* spi;
	uint8_t spiNumOfTries;
	uint16_t spiTimeout_ms;

	// Variables
	AlxA352_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxA352;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxA352_Ctor(
	AlxA352* me,
	AlxSpi* spi,
	uint8_t spiNumOfTries,
	uint16_t spiTimeout_ms);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxA352_Init(AlxA352* me);
Alx_Status AlxA352_DeInit(AlxA352* me);
Alx_Status AlxA352_Configure(AlxA352* me);
Alx_Status AlxA352_GetProdId(AlxA352* me);
Alx_Status AlxA352_GetFwVersion(AlxA352 *me);
Alx_Status AlxA352_GetSerialNum(AlxA352 *me);
Alx_Status AlxA352_GetAccData(AlxA352* me);
Alx_Status AlxA352_GetTempData(AlxA352* me);

#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADXL355_H

