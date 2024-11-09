/**
  ******************************************************************************
  * @file		alxAds114s08.h
  * @brief		Auralix C Library - ALX Precision ADC ADS114S08 Module
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
#ifndef ALX_ADS114S08_H
#define ALX_ADS114S08_H

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
#include "alxInterpLin.h"

#include "alxTimSw.h"
#include "alxOsMutex.h"
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADS114S08_FILE "alxAds114s08.h"

// Assert //
#if defined(ALX_ADS114S08_ASSERT_BKPT_ENABLE)
	#define ALX_ADS114S08_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADS114S08_FILE, expr)
#elif defined(ALX_ADS114S08_ASSERT_TRACE_ENABLE)
	#define ALX_ADS114S08_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADS114S08_FILE, expr)
#elif defined(ALX_ADS114S08_ASSERT_RST_ENABLE)
	#define ALX_ADS114S08_ASSERT(expr) ALX_ASSERT_RST(ALX_ADS114S08_FILE, expr)
#else
	#define ALX_ADS114S08_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_ADS114S08_TRACE_ENABLE)
	#define ALX_ADS114S08_TRACE(...) ALX_TRACE_WRN(ALX_ADS114S08_FILE, __VA_ARGS__)
#else
	#define ALX_ADS114S08_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Enums
//******************************************************************************
typedef enum
{
	thermocoupleType_K,
	thermocoupleType_S
} Ads114s08_ThermocoupleType;

//******************************************************************************
// Register Values Unions
//******************************************************************************


//------------------------------------------------------------------------------
// Device ID (ID) Register
//------------------------------------------------------------------------------
typedef enum
{
	DEV_ID_Reserved0 = 0b000,
	DEV_ID_Reserved1 = 0b001,
	DEV_ID_Reserved2 = 0b010,
	DEV_ID_Reserved3 = 0b011,
	DEV_ID_ADS114S08 = 0b100,
	DEV_ID_ADS114S06 = 0b101,
	DEV_ID_Reserved6 = 0b110,
	DEV_ID_Reserved7 = 0b111
} Ads114s08_RegEnum_0x00_DEV_ID;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x00_DEV_ID DEV_ID: 3;
		uint8_t RESERVED : 5;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x00_ID;


//------------------------------------------------------------------------------
// Device Status (STATUS) Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool FL_REF_L0	: 1;
		bool FL_REF_L1	: 1;
		bool FL_N_RAILN	: 1;
		bool FL_N_RAILP	: 1;
		bool FL_P_RAILN	: 1;
		bool FL_P_RAILP	: 1;
		bool RDY		: 1;
		bool FL_POR		: 1;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x01_STATUS;


//------------------------------------------------------------------------------
// Input Multiplexer (INPMUX) Register
//------------------------------------------------------------------------------
typedef enum
{
	MUXP_AIN0       = 0b0000,
	MUXP_AIN1       = 0b0001,
	MUXP_AIN2       = 0b0010,
	MUXP_AIN3       = 0b0011,
	MUXP_AIN4       = 0b0100,
	MUXP_AIN5       = 0b0101,
	MUXP_AIN6       = 0b0110,
	MUXP_AIN7       = 0b0111,
	MUXP_AIN8       = 0b1000,
	MUXP_AIN9       = 0b1001,
	MUXP_AIN10      = 0b1010,
	MUXP_AIN11      = 0b1011,
	MUXP_AINCOM     = 0b1100,
	MUXP_Reserved13 = 0b1101,
	MUXP_Reserved14 = 0b1110,
	MUXP_Reserved   = 0b1111
} Ads114s08_RegEnum_0x02_MUXP;

typedef enum
{
	MUXN_AIN0       = 0b0000,
	MUXN_AIN1       = 0b0001,
	MUXN_AIN2       = 0b0010,
	MUXN_AIN3       = 0b0011,
	MUXN_AIN4       = 0b0100,
	MUXN_AIN5       = 0b0101,
	MUXN_AIN6       = 0b0110,
	MUXN_AIN7       = 0b0111,
	MUXN_AIN8       = 0b1000,
	MUXN_AIN9       = 0b1001,
	MUXN_AIN10      = 0b1010,
	MUXN_AIN11      = 0b1011,
	MUXN_AINCOM     = 0b1100,
	MUXN_Reserved13 = 0b1101,
	MUXN_Reserved14 = 0b1110,
	MUXN_Reserved   = 0b1111
} Ads114s08_RegEnum_0x02_MUXN;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x02_MUXN MUXN : 4;
		Ads114s08_RegEnum_0x02_MUXP MUXP : 4;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x02_INPMUX;


//------------------------------------------------------------------------------
// Gain Setting (PGA) Register
//------------------------------------------------------------------------------

typedef enum
{
	MUXP_14xtmod   = 0b000,
	MUXP_25xtmod   = 0b001,
	MUXP_64xtmod   = 0b010,
	MUXP_256xtmod  = 0b011,
	MUXP_1024xtmod = 0b100,
	MUXP_2048xtmod = 0b101,
	MUXP_4096xtmod = 0b110,
	MUXP_1xtmod    = 0b111
} Ads114s08_RegEnum_0x03_DELAY;

typedef enum
{
	PGA_EN_PgaIsDisableed	= 0b00,
	PGA_EN_PgaIsEnabled		= 0b01,
	PGA_EN_Reserved2		= 0b10,
	PGA_EN_Reserved3		= 0b11
} Ads114s08_RegEnum_0x03_PGA_EN;

typedef enum
{
	PGA_GAIN_1		= 0b000,
	PGA_GAIN_2		= 0b001,
	PGA_GAIN_4		= 0b010,
	PGA_GAIN_8		= 0b011,
	PGA_GAIN_16		= 0b100,
	PGA_GAIN_32		= 0b101,
	PGA_GAIN_64		= 0b110,
	PGA_GAIN_128	= 0b111
} Ads114s08_RegEnum_0x03_GAIN;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x03_GAIN		GAIN : 3;
		Ads114s08_RegEnum_0x03_PGA_EN	PGA_EN : 2;
		Ads114s08_RegEnum_0x03_DELAY	DELAY : 3;
	};
	uint8_t raw;
} Ads114s08_RegVal_0x03_PGA;


//------------------------------------------------------------------------------
// Data Rate (DATARATE) Register
//------------------------------------------------------------------------------
typedef enum
{
	G_CHOP_Dissabled = 0b0,
	G_CHOP_Enabled   = 0b1
} Ads114s08_RegEnum_0x04_G_CHOP;
typedef enum
{
	CLK_Internal	= 0b0,
	CLK_External	= 0b1
} Ads114s08_RegEnum_0x04_CLK;
typedef enum
{
	MODE_ContinuousConversion	= 0b0,
	MODE_SingleConversion		= 0b1
} Ads114s08_RegEnum_0x04_MODE;
typedef enum
{
	FILTER_Sinc3		= 0b0,
	FILTER_LowLatency	= 0b1
} Ads114s08_RegEnum_0x04_FILTER;
typedef enum
{
	DR_2_5sps		= 0b0000,
	DR_5sps     	= 0b0001,
	DR_10sps    	= 0b0010,
	DR_16_6sps  	= 0b0011,
	DR_20sps		= 0b0100,
	DR_50sps    	= 0b0101,
	DR_60sps    	= 0b0110,
	DR_100sps   	= 0b0111,
	DR_200sps   	= 0b1000,
	DR_400sps   	= 0b1001,
	DR_800sps   	= 0b1010,
	DR_1000sps  	= 0b1011,
	DR_2000sps  	= 0b1100,
	DR_4000sps  	= 0b1101,
	DR_4000sps15  	= 0b1110,
	DR_Reserved15 	= 0b1111
} Ads114s08_RegEnum_0x04_DR;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x04_DR		DR		: 4;
		Ads114s08_RegEnum_0x04_FILTER	FILTER	: 1;
		Ads114s08_RegEnum_0x04_MODE		MODE	: 1;
		Ads114s08_RegEnum_0x04_CLK		CLK		: 1;
		Ads114s08_RegEnum_0x04_G_CHOP	G_CHOP	: 1;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x04_DATARATE;


//------------------------------------------------------------------------------
// Reference Control (REF) Register
//------------------------------------------------------------------------------
typedef enum
{
	FL_REF_EN_Dissabled						= 0b00,
	FL_REF_EN_FL_REF_L0_MonEn				= 0b01,
	FL_REF_EN_FL_REF_L0_FL_REF_L1_MonEn		= 0b10,
	FL_REF_EN_FL_REF_L0_MonEn_PullUpEn		= 0b11
	} Ads114s08_RegEnum_0x05_FL_REF_EN;
typedef enum
{
	REFP_BUF_Enabled	= 0b0,
	REFP_BUF_Disabled	= 0b1
} Ads114s08_RegEnum_0x05_REFP_BUF;
typedef enum
{
	REFN_BUF_Enabled	= 0b0,
	REFN_BUF_Disabled	= 0b1
} Ads114s08_RegEnum_0x05_REFN_BUF;
typedef enum
{
	REFSEL_REF0_REFN0		= 0b00,
	REFSEL_REF1_REFN1		= 0b01,
	REFSEL_Internal2_5Vref	= 0b10,
	REFSEL_Reserved3		= 0b11,
} Ads114s08_RegEnum_0x05_REFSEL;
typedef enum
{
	REFCON_InternalRefOff		= 0b00,
	REFCON_InternalRefOn		= 0b01,
	REFCON_InternalRefAlwaysOn	= 0b10,
	REFCON_InternalRefReserved3	= 0b11,
} Ads114s08_RegEnum_0x05_REFCON;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x05_REFCON		REFCON		: 2;
		Ads114s08_RegEnum_0x05_REFSEL		REFSEL		: 2;
		Ads114s08_RegEnum_0x05_REFN_BUF		REFN_BUF	: 1;
		Ads114s08_RegEnum_0x05_REFP_BUF		REFP_BUF	: 1;
		Ads114s08_RegEnum_0x05_FL_REF_EN	FL_REF_EN	: 2;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x05_REF;


//------------------------------------------------------------------------------
// Excitation Current Register 1 (IDACMAG)
//------------------------------------------------------------------------------
typedef enum
{
	FL_RAIL_EN_Dissabled	= 0b0,
	FL_RAIL_EN_Enabled		= 0b1
} Ads114s08_RegEnum_0x06_FL_RAIL_EN;
typedef enum
{
	PSW_Open	= 0b0,
	PSW_Closed	= 0b1
} Ads114s08_RegEnum_0x06_PSW;
typedef enum
{
	RESERVED_Reserved0	= 0b00,
	RESERVED_Reserved1	= 0b01,
	RESERVED_Reserved2	= 0b10,
	RESERVED_Reserved3	= 0b11,
} Ads114s08_RegEnum_0x06_RESERVED;
typedef enum
{
	IMAG_Off 	= 0b0000,
	IMAG_10uA   = 0b0001,
	IMAG_50uA   = 0b0010,
	IMAG_100uA  = 0b0011,
	IMAG_250uA  = 0b0100,
	IMAG_500uA  = 0b0101,
	IMAG_750uA  = 0b0110,
	IMAG_1000uA = 0b0111,
	IMAG_1500uA = 0b1000,
	IMAG_2000uA = 0b1001,
	IMAG_Off10  = 0b1010,
	IMAG_Off11  = 0b1011,
	IMAG_Off12  = 0b1100,
	IMAG_Off13  = 0b1101,
	IMAG_Off14  = 0b1110,
	IMAG_Off15  = 0b1111
} Ads114s08_RegEnum_0x06_IMAG;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x06_IMAG			IMAG		: 4;
		Ads114s08_RegEnum_0x06_RESERVED		RESERVED	: 2;
		Ads114s08_RegEnum_0x06_PSW			PSW			: 1;
		Ads114s08_RegEnum_0x06_FL_RAIL_EN	FL_RAIL_EN	: 1;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x06_IDACMAG;


//------------------------------------------------------------------------------
// Excitation Current Register 2 (IDACMUX)
//------------------------------------------------------------------------------
typedef enum
{
	I2MUX_AIN0           = 0b0000,
	I2MUX_AIN1           = 0b0001,
	I2MUX_AIN2           = 0b0010,
	I2MUX_AIN3           = 0b0011,
	I2MUX_AIN4           = 0b0100,
	I2MUX_AIN5           = 0b0101,
	I2MUX_AIN6           = 0b0110,
	I2MUX_AIN7           = 0b0111,
	I2MUX_AIN8           = 0b1000,
	I2MUX_AIN9           = 0b1001,
	I2MUX_AIN10          = 0b1010,
	I2MUX_AIN11          = 0b1011,
	I2MUX_AINCOM         = 0b1100,
	I2MUX_Disconnected13 = 0b1101,
	I2MUX_Disconnected14 = 0b1110,
	I2MUX_Disconnected15 = 0b1111
} Ads114s08_RegEnum_0x07_I2MUX;
typedef enum
{
	I1MUX_AIN0           = 0b0000,
	I1MUX_AIN1           = 0b0001,
	I1MUX_AIN2           = 0b0010,
	I1MUX_AIN3           = 0b0011,
	I1MUX_AIN4           = 0b0100,
	I1MUX_AIN5           = 0b0101,
	I1MUX_AIN6           = 0b0110,
	I1MUX_AIN7           = 0b0111,
	I1MUX_AIN8           = 0b1000,
	I1MUX_AIN9           = 0b1001,
	I1MUX_AIN10          = 0b1010,
	I1MUX_AIN11          = 0b1011,
	I1MUX_AINCOM         = 0b1100,
	I1MUX_Disconnected13 = 0b1101,
	I1MUX_Disconnected14 = 0b1110,
	I1MUX_Disconnected15 = 0b1111
} Ads114s08_RegEnum_0x07_I1MUX;

typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x07_I1MUX	I1MUX	: 4;
		Ads114s08_RegEnum_0x07_I2MUX	I2MUX	: 4;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x07_IDACMUX;


//------------------------------------------------------------------------------
// Sensor Biasing (VBIAS) Register
//------------------------------------------------------------------------------
typedef enum
{
	VB_LEVEL_AvddAvss_div2				= 0b0,
	VB_LEVEL_AvddAvss_div12				= 0b1
} Ads114s08_RegEnum_0x08_VB_LEVEL;
typedef enum
{
	VB_AINC_VbiasDisconnectedFromAinCom	= 0b0,
	VB_AINC_VbiasConnectedToAinCom		= 0b1
} Ads114s08_RegEnum_0x08_VB_AINC;
typedef enum
{
	VB_AIN5_VbiasDisconnectedFromAin5	= 0b0,
	VB_AIN5_VbiasConnectedToAin5		= 0b1
} Ads114s08_RegEnum_0x08_VB_AIN5;
typedef enum
{
	VB_AIN4_VbiasDisconnectedFromAin4	= 0b0,
	VB_AIN4_VbiasConnectedToAin4		= 0b1
} Ads114s08_RegEnum_0x08_VB_AIN4;
typedef enum
{
	VB_AIN3_VbiasDisconnectedFromAin3	= 0b0,
	VB_AIN3_VbiasConnectedToAin3		= 0b1
} Ads114s08_RegEnum_0x08_VB_AIN3;
typedef enum
{
	VB_AIN2_VbiasDisconnectedFromAin2	= 0b0,
	VB_AIN2_VbiasConnectedToAin2		= 0b1
} Ads114s08_RegEnum_0x08_VB_AIN2;
typedef enum
{
	VB_AIN1_VbiasDisconnectedFromAin1	= 0b0,
	VB_AIN1_VbiasConnectedToAin1		= 0b1
} Ads114s08_RegEnum_0x08_VB_AIN1;
typedef enum
{
	VB_AIN0_VbiasDisconnectedFromAin0	= 0b0,
	VB_AIN0_VbiasConnectedToAin0		= 0b1
} Ads114s08_RegEnum_0x08_VB_AIN0;
typedef union
{
	struct __attribute__((packed))
	{
		Ads114s08_RegEnum_0x08_VB_AIN0	VB_AIN0	 : 1;
		Ads114s08_RegEnum_0x08_VB_AIN1	VB_AIN1	 : 1;
		Ads114s08_RegEnum_0x08_VB_AIN2	VB_AIN2	 : 1;
		Ads114s08_RegEnum_0x08_VB_AIN3	VB_AIN3	 : 1;
		Ads114s08_RegEnum_0x08_VB_AIN4	VB_AIN4	 : 1;
		Ads114s08_RegEnum_0x08_VB_AIN5	VB_AIN5	 : 1;
		Ads114s08_RegEnum_0x08_VB_AINC	VB_AINC	 : 1;
		Ads114s08_RegEnum_0x08_VB_LEVEL	VB_LEVEL : 1;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x08_VBIAS;


//------------------------------------------------------------------------------
// System Control (SYS) Register
//------------------------------------------------------------------------------
typedef enum
{
	SYS_MON_Disabled					= 0b000,
	SYS_MON_Pga_AvddAvss_Div2           = 0b001,
	SYS_MON_InternalTempSense           = 0b010,
	SYS_MON_AvddAvss_Div4               = 0b011,
	SYS_MON_Dvdd_Div4                   = 0b100,
	SYS_MON_BurnOutCurrentSrcEn_0_2uA   = 0b101,
	SYS_MON_BurnOutCurrentSrcEn_1uA     = 0b110,
	SYS_MON_BurnOutCurrentSrcEn_10uA    = 0b111
} Ads114s08_RegEnum_0x09_SYS_MON;
typedef enum
{
	CaL_SAMP_1sample = 0b00,
	CaL_SAMP_2sample = 0b01,
	CaL_SAMP_4sample = 0b10,
	CaL_SAMP_8sample = 0b11,
} Ads114s08_RegEnum_0x09_CAL_SAMP;
typedef union
{
	struct __attribute__((packed))
	{
		bool SENDSTAT_EN							: 1;
		bool CRC_EN									: 1;
		bool TIMEOUT_EN								: 1;
		Ads114s08_RegEnum_0x09_CAL_SAMP	CAL_SAMP	: 2;
		Ads114s08_RegEnum_0x09_SYS_MON	SYS_MON		: 3;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x09_SYS;


//------------------------------------------------------------------------------
// Reserved Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	reserved0 : 8;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x0A_Reserved;


//------------------------------------------------------------------------------
// Offset Calibration Register 1 (OFCAL0)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	OFC_7_0 : 8;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x0B_OFCAL0;


//------------------------------------------------------------------------------
// Offset Calibration Register 2 (OFCAL1)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	OFC_15_8 : 8;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x0C_OFCAL1;


//------------------------------------------------------------------------------
// Reserved Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	reserved0 : 8;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x0D_Reserved;


//------------------------------------------------------------------------------
// Gain Calibration Register 1 (FSCAL0)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	FSC_7_0 : 8;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x0E_FSCAL0;


//------------------------------------------------------------------------------
// Gain Calibration Register 2 (FSCAL1)
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t	FSC_15_8 : 8;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x0F_FSCAL1;


//------------------------------------------------------------------------------
// GPIO Data (GPIODAT) Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool DAT_pin0 : 1;
		bool DAT_pin1 : 1;
		bool DAT_pin2 : 1;
		bool DAT_pin3 : 1;
		bool DIR_pin0_asInput : 1;
		bool DIR_pin1_asInput : 1;
		bool DIR_pin2_asInput : 1;
		bool DIR_pin3_asInput : 1;
	};
	uint8_t raw;
	} Ads114s08_RegVal_0x10_GPIODAT;


//------------------------------------------------------------------------------
// GPIO Configuration (GPIOCON) Register
//------------------------------------------------------------------------------
typedef union
{
	struct __attribute__((packed))
	{
		bool CON_pin0_asGpio : 1;
		bool CON_pin1_asGpio : 1;
		bool CON_pin2_asGpio : 1;
		bool CON_pin3_asGpio : 1;
		bool reserved0 : 1;
		bool reserved1 : 1;
		bool reserved2 : 1;
		bool reserved3 : 1;
	};
	uint8_t raw;
} Ads114s08_RegVal_0x11_GPIOCON;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x00_ID val;
} Ads114s08_Reg_0x00_ID;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x01_STATUS val;
} Ads114s08_Reg_0x01_STATUS;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x02_INPMUX val;
} Ads114s08_Reg_0x02_INPMUX;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x03_PGA val;
} Ads114s08_Reg_0x03_PGA;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x04_DATARATE val;
} Ads114s08_Reg_0x04_DATARATE;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x05_REF val;
} Ads114s08_Reg_0x05_REF;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x06_IDACMAG val;
} Ads114s08_Reg_0x06_IDACMAG;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x07_IDACMUX val;
} Ads114s08_Reg_0x07_IDACMUX;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x08_VBIAS val;
} Ads114s08_Reg_0x08_VBIAS;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x09_SYS val;
} Ads114s08_Reg_0x09_SYS;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x0A_Reserved val;
} Ads114s08_Reg_0x0A_Reserved;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x0B_OFCAL0 val;
} Ads114s08_Reg_0x0B_OFCAL0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x0C_OFCAL1 val;
} Ads114s08_Reg_0x0C_OFCAL1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x0D_Reserved val;
} Ads114s08_Reg_0x0D_Reserved;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x0E_FSCAL0 val;
} Ads114s08_Reg_0x0E_FSCAL0;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x0F_FSCAL1 val;
} Ads114s08_Reg_0x0F_FSCAL1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x10_GPIODAT val;
} Ads114s08_Reg_0x10_GPIODAT;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	Ads114s08_RegVal_0x11_GPIOCON val;
} Ads114s08_Reg_0x11_GPIOCON;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	Ads114s08_Reg_0x00_ID			reg_0x00_ID;
	Ads114s08_Reg_0x01_STATUS		reg_0x01_STATUS;
	Ads114s08_Reg_0x02_INPMUX		reg_0x02_INPMUX;
	Ads114s08_Reg_0x03_PGA			reg_0x03_PGA;
	Ads114s08_Reg_0x04_DATARATE		reg_0x04_DATARATE;
	Ads114s08_Reg_0x05_REF			reg_0x05_REF;
	Ads114s08_Reg_0x06_IDACMAG		reg_0x06_IDACMAG;
	Ads114s08_Reg_0x07_IDACMUX		reg_0x07_IDACMUX;
	Ads114s08_Reg_0x08_VBIAS		reg_0x08_VBIAS;
	Ads114s08_Reg_0x09_SYS			reg_0x09_SYS;
	Ads114s08_Reg_0x0A_Reserved		reg_0x0A_RESERVED;
	Ads114s08_Reg_0x0B_OFCAL0		reg_0x0B_OFCAL0;
	Ads114s08_Reg_0x0C_OFCAL1		reg_0x0C_OFCAL1;
	Ads114s08_Reg_0x0D_Reserved		reg_0x0D_RESERVED;
	Ads114s08_Reg_0x0E_FSCAL0		reg_0x0E_FSCAL0;
	Ads114s08_Reg_0x0F_FSCAL1		reg_0x0F_FSCAL1;
	Ads114s08_Reg_0x10_GPIODAT		reg_0x10_GPIODAT;
	Ads114s08_Reg_0x11_GPIOCON		reg_0x11_GPIOCON;
} AlxAds114s08_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxSpi* spi;
	uint8_t spiNumOfTries;
	uint16_t spiTimeout_ms;
	AlxIoPin* di_nDRDY;
	AlxIoPin* do_START;
	AlxIoPin* do_nRST;
	uint32_t CONVERSION_TIMEOUT_ms;

	AlxInterpLin transferFun_thermocoupleS_degC_mV;
	AlxInterpLin transferFun_thermocoupleS_mV_degC;

	// Variables
	AlxAds114s08_Reg reg;

	// Timeout timer
	AlxTimSw timeoutTimer;

	// Info
	bool isInit;
	bool isInitPeriph;
	bool wasCtorCalled;
} AlxAds114s08;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxAds114s08_Ctor
(
AlxAds114s08* me,
AlxSpi* spi,
uint8_t spiNumOfTries,
uint16_t spiTimeout_ms,
AlxIoPin* di_nDRDY,
AlxIoPin* do_START,
AlxIoPin* do_nRST);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAds114s08_InitPeriph(AlxAds114s08* me);
Alx_Status AlxAds114s08_DeInitPeriph(AlxAds114s08* me);
Alx_Status AlxAds114s08_Init(AlxAds114s08* me);
Alx_Status AlxAds114s08_DeInit(AlxAds114s08* me);
Alx_Status AlxAds114s08_GetChVoltage_mV(AlxAds114s08* me, uint8_t chNum, float* chVoltage_mV, bool* isOutOfRange);
Alx_Status AlxAds114s08_GetPgaShorted_uV(AlxAds114s08* me, float* pgaShortedVoltage_uV);
Alx_Status AlxAds114s08_GetInternalTemp_degC(AlxAds114s08* me, float* internalTemp_degC);
Alx_Status AlxAds114s08_GetAvddAvss_mV(AlxAds114s08* me, float* avddAvss_mV);
Alx_Status AlxAds114s08_GetDvdd_mV(AlxAds114s08* me, float* dvdd_mV);
Alx_Status AlxAds114s08_GetDevId(AlxAds114s08* me, Ads114s08_RegEnum_0x00_DEV_ID* DEV_ID);
Alx_Status AlxAds114s08_ApplyColdJunctionCompensation_degC(AlxAds114s08* me,
Ads114s08_ThermocoupleType thermocoupleType,
float thermocoupleVoltage_mV,
float coldJunctionTemp_degC,
float* thermocoupleTemperatureCompnsated_degC);

#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_ADS114S08_H
