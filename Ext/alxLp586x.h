/**
  ******************************************************************************
  * @file		alxLp586x.h
  * @brief		Auralix C Library - ALX LED Driver LP586x Module
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
#ifndef ALX_LP586x_H
#define ALX_LP586x_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxI2c.h"
#include "alxDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LP586x_FILE "alxLp586x.h"

// Assert //
#if defined(ALX_LP586x_ASSERT_BKPT_ENABLE)
	#define ALX_LP586x_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LP586x_FILE, expr)
#elif defined(ALX_LP586x_ASSERT_TRACE_ENABLE)
	#define ALX_LP586x_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LP586x_FILE, expr)
#elif defined(ALX_LP586x_ASSERT_RST_ENABLE)
	#define ALX_LP586x_ASSERT(expr) ALX_ASSERT_RST(ALX_LP586x_FILE, expr)
#else
	#define ALX_LP586x_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_LP586x_TRACE_ENABLE)
	#define ALX_LP586x_TRACE_WRN(...) ALX_TRACE_WRN(ALX_LP586x_FILE, __VA_ARGS__)
	#define ALX_LP586x_TRACE_ERR(...) ALX_TRACE_ERR(ALX_LP586x_FILE, __VA_ARGS__)
#else
	#define ALX_LP586x_TRACE_WRN(...) do{} while (false)
	#define ALX_LP586x_TRACE_ERR(...) do{} while (false)
#endif


//******************************************************************************
// Register Values Unions
//******************************************************************************
	typedef union
	{
		struct __attribute__((packed))
		{
			bool _0 : 1;
			bool _1 : 1;
			bool _2 : 1;
			bool _3 : 1;
			bool _4 : 1;
			bool _5 : 1;
			bool _6 : 1;
			bool _7 : 1;
		};
		uint8_t raw;
	} AlxLp586x_RegVal_Port;


//******************************************************************************
// Address: 0x00, Reset: 0x00, Name: Chip_en
//******************************************************************************
typedef enum
{
	Chip_Disabled = 0,
	Chip_Enabled  = 1
} AlxLp586x_RegEnum_0x00_Chip_En;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_6 : 7;
		AlxLp586x_RegEnum_0x00_Chip_En Chip_En : 1;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x00_Chip_En;


//******************************************************************************
// Address: 0x01, Reset: 0x5E , Name: Dev_initial
//******************************************************************************
typedef enum
{
	Data_Ref_Mode_Off	= 0,
	Data_Ref_Mode_1		= 1,	// PWM RESOLUTION : 8 Bits, Data update instantly, No EXTERNAL VSYNC
	Data_Ref_Mode_2		= 2,	// PWM RESOLUTION : 8 Bits, Data update by frame, With EXTERNAL VSYNC
	Data_Ref_Mode_3		= 3		// PWM RESOLUTION : 8 Bits, Data update by frame, With EXTERNAL VSYNC
} AlxLp586x_RegEnum_0x01_Data_Ref_Mode;
typedef enum
{
	PWM_Fre_62k5Hz = 0,		// 62,5kHz
	PWM_Fre_125kHz = 1		// 125kHz
} AlxLp586x_RegEnum_0x01_PWM_Fre;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0 : 1;
		uint8_t Max_Line_Num : 4;
		AlxLp586x_RegEnum_0x01_Data_Ref_Mode Ref_Mode : 2;
		AlxLp586x_RegEnum_0x01_PWM_Fre PWM_Fre : 1;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x01_Dev_initial;


//******************************************************************************
// Address: 0x02, Reset: 0x00 , Name: Dev_config1
//******************************************************************************
typedef enum
{
	SW_BLK_1us		= 0, // switch blank time: 1μs
	SW_BLK_0u5s		= 1	// switch blank time: 0,5μs
} AlxLp586x_RegEnum_0x02_SW_BLK;
typedef enum
{
	ExponentialScale	= 0,	// human-eye-friendly dimming curve
	LinearScale			= 1
} AlxLp586x_RegEnum_0x02_PWM_Scale_Mode;
typedef enum
{
	Phase_Shift_0ns		= 0,
	Phase_Shift_125ns	= 1
} AlxLp586x_RegEnum_0x02_PWM_Phase_Shift;
typedef enum
{
	CS_ON_Shift_Off = 0,
	CS_ON_Shift_On	= 1
} AlxLp586x_RegEnum_0x02_CS_ON_Shift ;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0_3 : 4;
		AlxLp586x_RegEnum_0x02_SW_BLK SW_BLK : 1;
		AlxLp586x_RegEnum_0x02_PWM_Scale_Mode PWM_Scale_Mode : 1;
		AlxLp586x_RegEnum_0x02_PWM_Phase_Shift PWM_Phase_Shift : 1;
		AlxLp586x_RegEnum_0x02_CS_ON_Shift CS_ON_Shift : 1;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x02_Dev_config1;


//******************************************************************************
// Address: 0x03, Reset: 0x00 , Name: Dev_config2
//******************************************************************************
typedef enum
{
	LOD_removal_Off = 0,
	LOD_removal_On	= 1
} AlxLp586x_RegEnum_0x03_LOD_removal; // turns off the current sink of the open channel
typedef enum
{
	LSD_removal_Off = 0,
	LSD_removal_On	= 1
} AlxLp586x_RegEnum_0x03_LSD_removal; // turns off the upside deghosting functio
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Comp_Group3 : 2; // Compensation_group 3: CS2, CS5, CS8, CS11, CS14, CS17
		uint8_t Comp_Group2 : 2; // Compensation_group 2: CS1, CS4, CS7, CS10, CS13, CS16
		uint8_t Comp_Group1 : 2; // Compensation_group 1 : CS0, CS3, CS6, CS9, CS12, CS15
		AlxLp586x_RegEnum_0x03_LOD_removal LOD_removal : 1;
		AlxLp586x_RegEnum_0x03_LSD_removal LSD_removal : 1;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x03_Dev_config2;


//******************************************************************************
// Address: 0x04, Reset: 0x47 , Name: Dev_config3
//******************************************************************************
typedef enum
{
	Maximum_Current_7m5A	= 0,
	Maximum_Current_12m5A	= 1,
	Maximum_Current_25mA	= 2,
	Maximum_Current_37m5A	= 3, // Default
	Maximum_Current_50mA	= 4,
	Maximum_Current_75mA	= 5,
	Maximum_Current_100mA	= 6
} AlxLp586x_RegEnum_0x04_Maximum_Current;
typedef enum
{
	Up_Deghost_Disnabled	= 0,
	Up_Deghost_Enabled		= 1
} AlxLp586x_RegEnum_0x04_Up_Deghost_enable;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t Down_Deghost : 2;
		uint8_t Up_Deghost : 2;
		AlxLp586x_RegEnum_0x04_Maximum_Current Maximum_Current : 3;
		AlxLp586x_RegEnum_0x04_Up_Deghost_enable Up_Deghost_enable : 1;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x04_Dev_config3;


//******************************************************************************
// Address: 0x05, Reset: 0xFF , Name: Global_bri
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PWM_Global : 8;
	};
	uint8_t raw;
	} AlxLp586x_RegVal_0x05_Global_bri;


//******************************************************************************
// Address: 0x06, Reset: 0xFF , Name: Group0_bri
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PWM_Group1 : 8;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x06_Group0_bri;


//******************************************************************************
// Address: 0x07, Reset: 0xFF , Name: Group1_bri
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PWM_Group2 : 8;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x07_Group1_bri;


//******************************************************************************
// Address: 0x08, Reset: 0xFF , Name: Group2_bri
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t PWM_Group3 : 8;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x08_Group2_bri;


//******************************************************************************
// Address: 0x09, Reset: 0x40 , Name: R_current_set - CC_Group1
//******************************************************************************
typedef enum
{
	CC_Group1_0pct   = 0  ,
	CC_Group1_0pct8  = 1  ,
	CC_Group1_1pct6  = 2  ,
	CC_Group1_2pct4  = 3  ,
	CC_Group1_3pct1  = 4  ,
	CC_Group1_3pct9  = 5  ,
	CC_Group1_4pct7  = 6  ,
	CC_Group1_5pct5  = 7  ,
	CC_Group1_6pct3  = 8  ,
	CC_Group1_7pct1  = 9  ,
	CC_Group1_7pct9  = 10 ,
	CC_Group1_8pct7  = 11 ,
	CC_Group1_9pct4  = 12 ,
	CC_Group1_10pct2 = 13 ,
	CC_Group1_11pct0 = 14 ,
	CC_Group1_11pct8 = 15 ,
	CC_Group1_12pct6 = 16 ,
	CC_Group1_13pct4 = 17 ,
	CC_Group1_14pct2 = 18 ,
	CC_Group1_15pct0 = 19 ,
	CC_Group1_15pct7 = 20 ,
	CC_Group1_16pct5 = 21 ,
	CC_Group1_17pct3 = 22 ,
	CC_Group1_18pct1 = 23 ,
	CC_Group1_18pct9 = 24 ,
	CC_Group1_19pct7 = 25 ,
	CC_Group1_20pct5 = 26 ,
	CC_Group1_21pct3 = 27 ,
	CC_Group1_22pct0 = 28 ,
	CC_Group1_22pct8 = 29 ,
	CC_Group1_23pct6 = 30 ,
	CC_Group1_24pct4 = 31 ,
	CC_Group1_25pct2 = 32 ,
	CC_Group1_26pct0 = 33 ,
	CC_Group1_26pct8 = 34 ,
	CC_Group1_27pct6 = 35 ,
	CC_Group1_28pct3 = 36 ,
	CC_Group1_29pct1 = 37 ,
	CC_Group1_29pct9 = 38 ,
	CC_Group1_30pct7 = 39 ,
	CC_Group1_31pct5 = 40 ,
	CC_Group1_32pct3 = 41 ,
	CC_Group1_33pct1 = 42 ,
	CC_Group1_33pct9 = 43 ,
	CC_Group1_34pct6 = 44 ,
	CC_Group1_35pct4 = 45 ,
	CC_Group1_36pct2 = 46 ,
	CC_Group1_37pct0 = 47 ,
	CC_Group1_37pct8 = 48 ,
	CC_Group1_38pct6 = 49 ,
	CC_Group1_39pct4 = 50 ,
	CC_Group1_40pct2 = 51 ,
	CC_Group1_40pct9 = 52 ,
	CC_Group1_41pct7 = 53 ,
	CC_Group1_42pct5 = 54 ,
	CC_Group1_43pct3 = 55 ,
	CC_Group1_44pct1 = 56 ,
	CC_Group1_44pct9 = 57 ,
	CC_Group1_45pct7 = 58 ,
	CC_Group1_46pct5 = 59 ,
	CC_Group1_47pct2 = 60 ,
	CC_Group1_48pct0 = 61 ,
	CC_Group1_48pct8 = 62 ,
	CC_Group1_49pct6 = 63 ,
	CC_Group1_50pct4 = 64 ,
	CC_Group1_51pct2 = 65 ,
	CC_Group1_52pct0 = 66 ,
	CC_Group1_52pct8 = 67 ,
	CC_Group1_53pct5 = 68 ,
	CC_Group1_54pct3 = 69 ,
	CC_Group1_55pct1 = 70 ,
	CC_Group1_55pct9 = 71 ,
	CC_Group1_56pct7 = 72 ,
	CC_Group1_57pct5 = 73 ,
	CC_Group1_58pct3 = 74 ,
	CC_Group1_59pct1 = 75 ,
	CC_Group1_59pct8 = 76 ,
	CC_Group1_60pct6 = 77 ,
	CC_Group1_61pct4 = 78 ,
	CC_Group1_62pct2 = 79 ,
	CC_Group1_63pct0 = 80 ,
	CC_Group1_63pct8 = 81 ,
	CC_Group1_64pct6 = 82 ,
	CC_Group1_65pct4 = 83 ,
	CC_Group1_66pct1 = 84 ,
	CC_Group1_66pct9 = 85 ,
	CC_Group1_67pct7 = 86 ,
	CC_Group1_68pct5 = 87 ,
	CC_Group1_69pct3 = 88 ,
	CC_Group1_70pct1 = 89 ,
	CC_Group1_70pct9 = 90 ,
	CC_Group1_71pct7 = 91 ,
	CC_Group1_72pct4 = 92 ,
	CC_Group1_73pct2 = 93 ,
	CC_Group1_74pct0 = 94 ,
	CC_Group1_74pct8 = 95 ,
	CC_Group1_75pct6 = 96 ,
	CC_Group1_76pct4 = 97 ,
	CC_Group1_77pct2 = 98 ,
	CC_Group1_78pct0 = 99 ,
	CC_Group1_78pct7 = 100,
	CC_Group1_79pct5 = 101,
	CC_Group1_80pct3 = 102,
	CC_Group1_81pct1 = 103,
	CC_Group1_81pct9 = 104,
	CC_Group1_82pct7 = 105,
	CC_Group1_83pct5 = 106,
	CC_Group1_84pct3 = 107,
	CC_Group1_85pct0 = 108,
	CC_Group1_85pct8 = 109,
	CC_Group1_86pct6 = 110,
	CC_Group1_87pct4 = 111,
	CC_Group1_88pct2 = 112,
	CC_Group1_89pct0 = 113,
	CC_Group1_89pct8 = 114,
	CC_Group1_90pct6 = 115,
	CC_Group1_91pct3 = 116,
	CC_Group1_92pct1 = 117,
	CC_Group1_92pct9 = 118,
	CC_Group1_93pct7 = 119,
	CC_Group1_94pct5 = 120,
	CC_Group1_95pct3 = 121,
	CC_Group1_96pct1 = 122,
	CC_Group1_96pct9 = 123,
	CC_Group1_97pct6 = 124,
	CC_Group1_98pct4 = 125,
	CC_Group1_99pct2 = 126,
	CC_Group1_100pct = 127
} AlxLp586x_RegEnum_0x09_CC_Group1;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0 : 1;
		AlxLp586x_RegEnum_0x09_CC_Group1 CC_Group1 : 7;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x09_R_current_set_CC_Group1;


//******************************************************************************
// Address: 0x0A, Reset: 0x40 , Name: G_current_set - CC_Group2
//******************************************************************************
typedef enum
{
	CC_Group2_0pct   = 0  ,
	CC_Group2_0pct8  = 1  ,
	CC_Group2_1pct6  = 2  ,
	CC_Group2_2pct4  = 3  ,
	CC_Group2_3pct1  = 4  ,
	CC_Group2_3pct9  = 5  ,
	CC_Group2_4pct7  = 6  ,
	CC_Group2_5pct5  = 7  ,
	CC_Group2_6pct3  = 8  ,
	CC_Group2_7pct1  = 9  ,
	CC_Group2_7pct9  = 10 ,
	CC_Group2_8pct7  = 11 ,
	CC_Group2_9pct4  = 12 ,
	CC_Group2_10pct2 = 13 ,
	CC_Group2_11pct0 = 14 ,
	CC_Group2_11pct8 = 15 ,
	CC_Group2_12pct6 = 16 ,
	CC_Group2_13pct4 = 17 ,
	CC_Group2_14pct2 = 18 ,
	CC_Group2_15pct0 = 19 ,
	CC_Group2_15pct7 = 20 ,
	CC_Group2_16pct5 = 21 ,
	CC_Group2_17pct3 = 22 ,
	CC_Group2_18pct1 = 23 ,
	CC_Group2_18pct9 = 24 ,
	CC_Group2_19pct7 = 25 ,
	CC_Group2_20pct5 = 26 ,
	CC_Group2_21pct3 = 27 ,
	CC_Group2_22pct0 = 28 ,
	CC_Group2_22pct8 = 29 ,
	CC_Group2_23pct6 = 30 ,
	CC_Group2_24pct4 = 31 ,
	CC_Group2_25pct2 = 32 ,
	CC_Group2_26pct0 = 33 ,
	CC_Group2_26pct8 = 34 ,
	CC_Group2_27pct6 = 35 ,
	CC_Group2_28pct3 = 36 ,
	CC_Group2_29pct1 = 37 ,
	CC_Group2_29pct9 = 38 ,
	CC_Group2_30pct7 = 39 ,
	CC_Group2_31pct5 = 40 ,
	CC_Group2_32pct3 = 41 ,
	CC_Group2_33pct1 = 42 ,
	CC_Group2_33pct9 = 43 ,
	CC_Group2_34pct6 = 44 ,
	CC_Group2_35pct4 = 45 ,
	CC_Group2_36pct2 = 46 ,
	CC_Group2_37pct0 = 47 ,
	CC_Group2_37pct8 = 48 ,
	CC_Group2_38pct6 = 49 ,
	CC_Group2_39pct4 = 50 ,
	CC_Group2_40pct2 = 51 ,
	CC_Group2_40pct9 = 52 ,
	CC_Group2_41pct7 = 53 ,
	CC_Group2_42pct5 = 54 ,
	CC_Group2_43pct3 = 55 ,
	CC_Group2_44pct1 = 56 ,
	CC_Group2_44pct9 = 57 ,
	CC_Group2_45pct7 = 58 ,
	CC_Group2_46pct5 = 59 ,
	CC_Group2_47pct2 = 60 ,
	CC_Group2_48pct0 = 61 ,
	CC_Group2_48pct8 = 62 ,
	CC_Group2_49pct6 = 63 ,
	CC_Group2_50pct4 = 64 ,
	CC_Group2_51pct2 = 65 ,
	CC_Group2_52pct0 = 66 ,
	CC_Group2_52pct8 = 67 ,
	CC_Group2_53pct5 = 68 ,
	CC_Group2_54pct3 = 69 ,
	CC_Group2_55pct1 = 70 ,
	CC_Group2_55pct9 = 71 ,
	CC_Group2_56pct7 = 72 ,
	CC_Group2_57pct5 = 73 ,
	CC_Group2_58pct3 = 74 ,
	CC_Group2_59pct1 = 75 ,
	CC_Group2_59pct8 = 76 ,
	CC_Group2_60pct6 = 77 ,
	CC_Group2_61pct4 = 78 ,
	CC_Group2_62pct2 = 79 ,
	CC_Group2_63pct0 = 80 ,
	CC_Group2_63pct8 = 81 ,
	CC_Group2_64pct6 = 82 ,
	CC_Group2_65pct4 = 83 ,
	CC_Group2_66pct1 = 84 ,
	CC_Group2_66pct9 = 85 ,
	CC_Group2_67pct7 = 86 ,
	CC_Group2_68pct5 = 87 ,
	CC_Group2_69pct3 = 88 ,
	CC_Group2_70pct1 = 89 ,
	CC_Group2_70pct9 = 90 ,
	CC_Group2_71pct7 = 91 ,
	CC_Group2_72pct4 = 92 ,
	CC_Group2_73pct2 = 93 ,
	CC_Group2_74pct0 = 94 ,
	CC_Group2_74pct8 = 95 ,
	CC_Group2_75pct6 = 96 ,
	CC_Group2_76pct4 = 97 ,
	CC_Group2_77pct2 = 98 ,
	CC_Group2_78pct0 = 99 ,
	CC_Group2_78pct7 = 100,
	CC_Group2_79pct5 = 101,
	CC_Group2_80pct3 = 102,
	CC_Group2_81pct1 = 103,
	CC_Group2_81pct9 = 104,
	CC_Group2_82pct7 = 105,
	CC_Group2_83pct5 = 106,
	CC_Group2_84pct3 = 107,
	CC_Group2_85pct0 = 108,
	CC_Group2_85pct8 = 109,
	CC_Group2_86pct6 = 110,
	CC_Group2_87pct4 = 111,
	CC_Group2_88pct2 = 112,
	CC_Group2_89pct0 = 113,
	CC_Group2_89pct8 = 114,
	CC_Group2_90pct6 = 115,
	CC_Group2_91pct3 = 116,
	CC_Group2_92pct1 = 117,
	CC_Group2_92pct9 = 118,
	CC_Group2_93pct7 = 119,
	CC_Group2_94pct5 = 120,
	CC_Group2_95pct3 = 121,
	CC_Group2_96pct1 = 122,
	CC_Group2_96pct9 = 123,
	CC_Group2_97pct6 = 124,
	CC_Group2_98pct4 = 125,
	CC_Group2_99pct2 = 126,
	CC_Group2_100pct = 127
} AlxLp586x_RegEnum_0x0A_CC_Group2;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0 : 1;
		AlxLp586x_RegEnum_0x0A_CC_Group2 CC_Group2 : 7;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x0A_G_current_set_CC_Group2;


//******************************************************************************
// Address: 0x0B, Reset: 0x40 , Name: B_current_set - CC_Group3
//******************************************************************************
typedef enum
{
	CC_Group3_0pct   = 0  ,
	CC_Group3_0pct8  = 1  ,
	CC_Group3_1pct6  = 2  ,
	CC_Group3_2pct4  = 3  ,
	CC_Group3_3pct1  = 4  ,
	CC_Group3_3pct9  = 5  ,
	CC_Group3_4pct7  = 6  ,
	CC_Group3_5pct5  = 7  ,
	CC_Group3_6pct3  = 8  ,
	CC_Group3_7pct1  = 9  ,
	CC_Group3_7pct9  = 10 ,
	CC_Group3_8pct7  = 11 ,
	CC_Group3_9pct4  = 12 ,
	CC_Group3_10pct2 = 13 ,
	CC_Group3_11pct0 = 14 ,
	CC_Group3_11pct8 = 15 ,
	CC_Group3_12pct6 = 16 ,
	CC_Group3_13pct4 = 17 ,
	CC_Group3_14pct2 = 18 ,
	CC_Group3_15pct0 = 19 ,
	CC_Group3_15pct7 = 20 ,
	CC_Group3_16pct5 = 21 ,
	CC_Group3_17pct3 = 22 ,
	CC_Group3_18pct1 = 23 ,
	CC_Group3_18pct9 = 24 ,
	CC_Group3_19pct7 = 25 ,
	CC_Group3_20pct5 = 26 ,
	CC_Group3_21pct3 = 27 ,
	CC_Group3_22pct0 = 28 ,
	CC_Group3_22pct8 = 29 ,
	CC_Group3_23pct6 = 30 ,
	CC_Group3_24pct4 = 31 ,
	CC_Group3_25pct2 = 32 ,
	CC_Group3_26pct0 = 33 ,
	CC_Group3_26pct8 = 34 ,
	CC_Group3_27pct6 = 35 ,
	CC_Group3_28pct3 = 36 ,
	CC_Group3_29pct1 = 37 ,
	CC_Group3_29pct9 = 38 ,
	CC_Group3_30pct7 = 39 ,
	CC_Group3_31pct5 = 40 ,
	CC_Group3_32pct3 = 41 ,
	CC_Group3_33pct1 = 42 ,
	CC_Group3_33pct9 = 43 ,
	CC_Group3_34pct6 = 44 ,
	CC_Group3_35pct4 = 45 ,
	CC_Group3_36pct2 = 46 ,
	CC_Group3_37pct0 = 47 ,
	CC_Group3_37pct8 = 48 ,
	CC_Group3_38pct6 = 49 ,
	CC_Group3_39pct4 = 50 ,
	CC_Group3_40pct2 = 51 ,
	CC_Group3_40pct9 = 52 ,
	CC_Group3_41pct7 = 53 ,
	CC_Group3_42pct5 = 54 ,
	CC_Group3_43pct3 = 55 ,
	CC_Group3_44pct1 = 56 ,
	CC_Group3_44pct9 = 57 ,
	CC_Group3_45pct7 = 58 ,
	CC_Group3_46pct5 = 59 ,
	CC_Group3_47pct2 = 60 ,
	CC_Group3_48pct0 = 61 ,
	CC_Group3_48pct8 = 62 ,
	CC_Group3_49pct6 = 63 ,
	CC_Group3_50pct4 = 64 ,
	CC_Group3_51pct2 = 65 ,
	CC_Group3_52pct0 = 66 ,
	CC_Group3_52pct8 = 67 ,
	CC_Group3_53pct5 = 68 ,
	CC_Group3_54pct3 = 69 ,
	CC_Group3_55pct1 = 70 ,
	CC_Group3_55pct9 = 71 ,
	CC_Group3_56pct7 = 72 ,
	CC_Group3_57pct5 = 73 ,
	CC_Group3_58pct3 = 74 ,
	CC_Group3_59pct1 = 75 ,
	CC_Group3_59pct8 = 76 ,
	CC_Group3_60pct6 = 77 ,
	CC_Group3_61pct4 = 78 ,
	CC_Group3_62pct2 = 79 ,
	CC_Group3_63pct0 = 80 ,
	CC_Group3_63pct8 = 81 ,
	CC_Group3_64pct6 = 82 ,
	CC_Group3_65pct4 = 83 ,
	CC_Group3_66pct1 = 84 ,
	CC_Group3_66pct9 = 85 ,
	CC_Group3_67pct7 = 86 ,
	CC_Group3_68pct5 = 87 ,
	CC_Group3_69pct3 = 88 ,
	CC_Group3_70pct1 = 89 ,
	CC_Group3_70pct9 = 90 ,
	CC_Group3_71pct7 = 91 ,
	CC_Group3_72pct4 = 92 ,
	CC_Group3_73pct2 = 93 ,
	CC_Group3_74pct0 = 94 ,
	CC_Group3_74pct8 = 95 ,
	CC_Group3_75pct6 = 96 ,
	CC_Group3_76pct4 = 97 ,
	CC_Group3_77pct2 = 98 ,
	CC_Group3_78pct0 = 99 ,
	CC_Group3_78pct7 = 100,
	CC_Group3_79pct5 = 101,
	CC_Group3_80pct3 = 102,
	CC_Group3_81pct1 = 103,
	CC_Group3_81pct9 = 104,
	CC_Group3_82pct7 = 105,
	CC_Group3_83pct5 = 106,
	CC_Group3_84pct3 = 107,
	CC_Group3_85pct0 = 108,
	CC_Group3_85pct8 = 109,
	CC_Group3_86pct6 = 110,
	CC_Group3_87pct4 = 111,
	CC_Group3_88pct2 = 112,
	CC_Group3_89pct0 = 113,
	CC_Group3_89pct8 = 114,
	CC_Group3_90pct6 = 115,
	CC_Group3_91pct3 = 116,
	CC_Group3_92pct1 = 117,
	CC_Group3_92pct9 = 118,
	CC_Group3_93pct7 = 119,
	CC_Group3_94pct5 = 120,
	CC_Group3_95pct3 = 121,
	CC_Group3_96pct1 = 122,
	CC_Group3_96pct9 = 123,
	CC_Group3_97pct6 = 124,
	CC_Group3_98pct4 = 125,
	CC_Group3_99pct2 = 126,
	CC_Group3_100pct = 127
} AlxLp586x_RegEnum_0x0B_CC_Group3;
typedef union
{
	struct __attribute__((packed))
	{
		uint8_t unused_0 : 1;
		AlxLp586x_RegEnum_0x0B_CC_Group3 CC_Group3 : 7;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x0B_B_current_set_CC_Group3;


//******************************************************************************
// Address: 0x0C, Reset: 0x00 , Name: Dot_grp_sel0
//******************************************************************************
typedef enum
{
	L0_CS3_No_Group_member	= 0,
	L0_CS3_Comp_Group1		= 1,
	L0_CS3_Comp_Group2		= 2,
	L0_CS3_Comp_Group3		= 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS3_Group;
typedef enum
{
	L0_CS2_No_Group_member	= 0,
	L0_CS2_Comp_Group1		= 1,
	L0_CS2_Comp_Group2		= 2,
	L0_CS2_Comp_Group3		= 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS2_Group;
	typedef enum
{
	L0_CS1_No_Group_member	= 0,
	L0_CS1_Comp_Group1		= 1,
	L0_CS1_Comp_Group2		= 2,
	L0_CS1_Comp_Group3		= 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS1_Group;
typedef enum
{
	L0_CS0_No_Group_member	= 0,
	L0_CS0_Comp_Group1		= 1,
	L0_CS0_Comp_Group2		= 2,
	L0_CS0_Comp_Group3		= 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS0_Group;
typedef union
{
	struct __attribute__((packed))
	{
		AlxLp586x_RegEnum_0x0C_Dot_L0_CS3_Group Dot_L0_CS3_Group : 2;
		AlxLp586x_RegEnum_0x0C_Dot_L0_CS2_Group Dot_L0_CS2_Group : 2;
		AlxLp586x_RegEnum_0x0C_Dot_L0_CS1_Group Dot_L0_CS1_Group : 2;
		AlxLp586x_RegEnum_0x0C_Dot_L0_CS0_Group Dot_L0_CS0_Group : 2;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0x0C_Dot_grp_sel0;


//******************************************************************************
// Address: 0x0D, Reset: 0x00 , Name: Dot_grp_sel1
//******************************************************************************
typedef enum
{
    L0_CS7_No_Group_member	= 0,
    L0_CS7_Comp_Group1      = 1,
    L0_CS7_Comp_Group2      = 2,
    L0_CS7_Comp_Group3      = 3
} AlxLp586x_RegEnum_0x0D_Dot_L0_CS7_Group;
typedef enum
{
    L0_CS6_No_Group_member	= 0,
    L0_CS6_Comp_Group1      = 1,
    L0_CS6_Comp_Group2      = 2,
    L0_CS6_Comp_Group3      = 3
} AlxLp586x_RegEnum_0x0D_Dot_L0_CS6_Group;
typedef enum
{
    L0_CS5_No_Group_member	= 0,
    L0_CS5_Comp_Group1      = 1,
    L0_CS5_Comp_Group2      = 2,
    L0_CS5_Comp_Group3      = 3
} AlxLp586x_RegEnum_0x0D_Dot_L0_CS5_Group;
typedef enum
{
    L0_CS4_No_Group_member	= 0,
    L0_CS4_Comp_Group1      = 1,
    L0_CS4_Comp_Group2      = 2,
    L0_CS4_Comp_Group3      = 3
} AlxLp586x_RegEnum_0x0D_Dot_L0_CS4_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x0D_Dot_L0_CS7_Group Dot_L0_CS7_Group : 2;
        AlxLp586x_RegEnum_0x0D_Dot_L0_CS6_Group Dot_L0_CS6_Group : 2;
        AlxLp586x_RegEnum_0x0D_Dot_L0_CS5_Group Dot_L0_CS5_Group : 2;
        AlxLp586x_RegEnum_0x0D_Dot_L0_CS4_Group Dot_L0_CS4_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x0D_Dot_grp_sel1;


//******************************************************************************
// Address: 0x0E, Reset: 0x00 , Name: Dot_grp_sel2
//******************************************************************************
typedef enum
{
    L0_CS11_No_Group_member = 0,
    L0_CS11_Comp_Group1     = 1,
    L0_CS11_Comp_Group2     = 2,
    L0_CS11_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0E_Dot_L0_CS11_Group;
typedef enum
{
    L0_CS10_No_Group_member = 0,
    L0_CS10_Comp_Group1     = 1,
    L0_CS10_Comp_Group2     = 2,
    L0_CS10_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0E_Dot_L0_CS10_Group;
typedef enum
{
    L0_CS9_No_Group_member = 0,
    L0_CS9_Comp_Group1     = 1,
    L0_CS9_Comp_Group2     = 2,
    L0_CS9_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0E_Dot_L0_CS9_Group;
typedef enum
{
    L0_CS8_No_Group_member = 0,
    L0_CS8_Comp_Group1     = 1,
    L0_CS8_Comp_Group2     = 2,
    L0_CS8_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0E_Dot_L0_CS8_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x0E_Dot_L0_CS11_Group Dot_L0_CS11_Group : 2;
        AlxLp586x_RegEnum_0x0E_Dot_L0_CS10_Group Dot_L0_CS10_Group : 2;
        AlxLp586x_RegEnum_0x0E_Dot_L0_CS9_Group  Dot_L0_CS9_Group  : 2;
        AlxLp586x_RegEnum_0x0E_Dot_L0_CS8_Group  Dot_L0_CS8_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x0E_Dot_grp_sel2;


//******************************************************************************
// Address: 0x0F, Reset: 0x00 , Name: Dot_grp_sel3
//******************************************************************************
typedef enum
{
    L0_CS15_No_Group_member = 0,
    L0_CS15_Comp_Group1     = 1,
    L0_CS15_Comp_Group2     = 2,
    L0_CS15_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0F_Dot_L0_CS15_Group;
typedef enum
{
    L0_CS14_No_Group_member = 0,
    L0_CS14_Comp_Group1     = 1,
    L0_CS14_Comp_Group2     = 2,
    L0_CS14_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0F_Dot_L0_CS14_Group;
typedef enum
{
    L0_CS13_No_Group_member = 0,
    L0_CS13_Comp_Group1     = 1,
    L0_CS13_Comp_Group2     = 2,
    L0_CS13_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0F_Dot_L0_CS13_Group;
typedef enum
{
    L0_CS12_No_Group_member = 0,
    L0_CS12_Comp_Group1     = 1,
    L0_CS12_Comp_Group2     = 2,
    L0_CS12_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x0F_Dot_L0_CS12_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x0F_Dot_L0_CS15_Group Dot_L0_CS15_Group : 2;
        AlxLp586x_RegEnum_0x0F_Dot_L0_CS14_Group Dot_L0_CS14_Group : 2;
        AlxLp586x_RegEnum_0x0F_Dot_L0_CS13_Group Dot_L0_CS13_Group : 2;
        AlxLp586x_RegEnum_0x0F_Dot_L0_CS12_Group Dot_L0_CS12_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x0F_Dot_grp_sel3;


//******************************************************************************
// Address: 0x10, Reset: 0x00 , Name: Dot_grp_sel4
//******************************************************************************
typedef enum
{
    L0_CS17_No_Group_member = 0,
    L0_CS17_Comp_Group1     = 1,
    L0_CS17_Comp_Group2     = 2,
    L0_CS17_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x10_Dot_L0_CS17_Group;
typedef enum
{
    L0_CS16_No_Group_member = 0,
    L0_CS16_Comp_Group1     = 1,
    L0_CS16_Comp_Group2     = 2,
    L0_CS16_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x10_Dot_L0_CS16_Group;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_3											: 4;
        AlxLp586x_RegEnum_0x10_Dot_L0_CS17_Group Dot_L0_CS17_Group	: 2;
        AlxLp586x_RegEnum_0x10_Dot_L0_CS16_Group Dot_L0_CS16_Group	: 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x10_Dot_grp_sel4;


//******************************************************************************
// Address: 0x11, Reset: 0x00 , Name: Dot_grp_sel5
//******************************************************************************
typedef enum
{
    L1_CS3_No_Group_member = 0,
    L1_CS3_Comp_Group1     = 1,
    L1_CS3_Comp_Group2     = 2,
    L1_CS3_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x11_Dot_L1_CS3_Group;
typedef enum
{
    L1_CS2_No_Group_member = 0,
    L1_CS2_Comp_Group1     = 1,
    L1_CS2_Comp_Group2     = 2,
    L1_CS2_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x11_Dot_L1_CS2_Group;
typedef enum
{
    L1_CS1_No_Group_member = 0,
    L1_CS1_Comp_Group1     = 1,
    L1_CS1_Comp_Group2     = 2,
    L1_CS1_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x11_Dot_L1_CS1_Group;
typedef enum
{
    L1_CS0_No_Group_member = 0,
    L1_CS0_Comp_Group1     = 1,
    L1_CS0_Comp_Group2     = 2,
    L1_CS0_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x11_Dot_L1_CS0_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x11_Dot_L1_CS3_Group Dot_L1_CS3_Group : 2;
        AlxLp586x_RegEnum_0x11_Dot_L1_CS2_Group Dot_L1_CS2_Group : 2;
        AlxLp586x_RegEnum_0x11_Dot_L1_CS1_Group Dot_L1_CS1_Group : 2;
        AlxLp586x_RegEnum_0x11_Dot_L1_CS0_Group Dot_L1_CS0_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x11_Dot_grp_sel5;


//******************************************************************************
// Address: 0x12, Reset: 0x00 , Name: Dot_grp_sel6
//******************************************************************************
typedef enum
{
    L1_CS7_No_Group_member = 0,
    L1_CS7_Comp_Group1     = 1,
    L1_CS7_Comp_Group2     = 2,
    L1_CS7_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x12_Dot_L1_CS7_Group;
typedef enum
{
    L1_CS6_No_Group_member = 0,
    L1_CS6_Comp_Group1     = 1,
    L1_CS6_Comp_Group2     = 2,
    L1_CS6_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x12_Dot_L1_CS6_Group;
typedef enum
{
    L1_CS5_No_Group_member = 0,
    L1_CS5_Comp_Group1     = 1,
    L1_CS5_Comp_Group2     = 2,
    L1_CS5_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x12_Dot_L1_CS5_Group;
typedef enum
{
    L1_CS4_No_Group_member = 0,
    L1_CS4_Comp_Group1     = 1,
    L1_CS4_Comp_Group2     = 2,
    L1_CS4_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x12_Dot_L1_CS4_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x12_Dot_L1_CS7_Group Dot_L1_CS7_Group : 2;
        AlxLp586x_RegEnum_0x12_Dot_L1_CS6_Group Dot_L1_CS6_Group : 2;
        AlxLp586x_RegEnum_0x12_Dot_L1_CS5_Group Dot_L1_CS5_Group : 2;
        AlxLp586x_RegEnum_0x12_Dot_L1_CS4_Group Dot_L1_CS4_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x12_Dot_grp_sel6;


//******************************************************************************
// Address: 0x13, Reset: 0x00 , Name: Dot_grp_sel7
//******************************************************************************
typedef enum
{
    L1_CS11_No_Group_member = 0,
    L1_CS11_Comp_Group1     = 1,
    L1_CS11_Comp_Group2     = 2,
    L1_CS11_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x13_Dot_L1_CS11_Group;
typedef enum
{
    L1_CS10_No_Group_member = 0,
    L1_CS10_Comp_Group1     = 1,
    L1_CS10_Comp_Group2     = 2,
    L1_CS10_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x13_Dot_L1_CS10_Group;
typedef enum
{
    L1_CS9_No_Group_member = 0,
    L1_CS9_Comp_Group1     = 1,
    L1_CS9_Comp_Group2     = 2,
    L1_CS9_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x13_Dot_L1_CS9_Group;
typedef enum
{
    L1_CS8_No_Group_member = 0,
    L1_CS8_Comp_Group1     = 1,
    L1_CS8_Comp_Group2     = 2,
    L1_CS8_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x13_Dot_L1_CS8_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x13_Dot_L1_CS11_Group Dot_L1_CS11_Group : 2;
        AlxLp586x_RegEnum_0x13_Dot_L1_CS10_Group Dot_L1_CS10_Group : 2;
        AlxLp586x_RegEnum_0x13_Dot_L1_CS9_Group  Dot_L1_CS9_Group  : 2;
        AlxLp586x_RegEnum_0x13_Dot_L1_CS8_Group  Dot_L1_CS8_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x13_Dot_grp_sel7;


//******************************************************************************
// Address: 0x14, Reset: 0x00 , Name: Dot_grp_sel8
//******************************************************************************
typedef enum
{
    L1_CS15_No_Group_member = 0,
    L1_CS15_Comp_Group1     = 1,
    L1_CS15_Comp_Group2     = 2,
    L1_CS15_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x14_Dot_L1_CS15_Group;
typedef enum
{
    L1_CS14_No_Group_member = 0,
    L1_CS14_Comp_Group1     = 1,
    L1_CS14_Comp_Group2     = 2,
    L1_CS14_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x14_Dot_L1_CS14_Group;
typedef enum
{
    L1_CS13_No_Group_member = 0,
    L1_CS13_Comp_Group1     = 1,
    L1_CS13_Comp_Group2     = 2,
    L1_CS13_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x14_Dot_L1_CS13_Group;
typedef enum
{
    L1_CS12_No_Group_member = 0,
    L1_CS12_Comp_Group1     = 1,
    L1_CS12_Comp_Group2     = 2,
    L1_CS12_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x14_Dot_L1_CS12_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x14_Dot_L1_CS15_Group Dot_L1_CS15_Group : 2;
        AlxLp586x_RegEnum_0x14_Dot_L1_CS14_Group Dot_L1_CS14_Group : 2;
        AlxLp586x_RegEnum_0x14_Dot_L1_CS13_Group Dot_L1_CS13_Group : 2;
        AlxLp586x_RegEnum_0x14_Dot_L1_CS12_Group Dot_L1_CS12_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x14_Dot_grp_sel8;


//******************************************************************************
// Address: 0x15, Reset: 0x00 , Name: Dot_grp_sel9
//******************************************************************************
typedef enum
{
    L1_CS17_No_Group_member = 0,
    L1_CS17_Comp_Group1     = 1,
    L1_CS17_Comp_Group2     = 2,
    L1_CS17_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x15_Dot_L1_CS17_Group;
typedef enum
{
    L1_CS16_No_Group_member = 0,
    L1_CS16_Comp_Group1     = 1,
    L1_CS16_Comp_Group2     = 2,
    L1_CS16_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x15_Dot_L1_CS16_Group;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_3											: 4;
        AlxLp586x_RegEnum_0x15_Dot_L1_CS17_Group Dot_L1_CS17_Group	: 2;
        AlxLp586x_RegEnum_0x15_Dot_L1_CS16_Group Dot_L1_CS16_Group	: 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x15_Dot_grp_sel9;


//******************************************************************************
// Address: 0x16, Reset: 0x00 , Name: Dot_grp_sel10
//******************************************************************************
typedef enum
{
    L2_CS3_No_Group_member = 0,
    L2_CS3_Comp_Group1     = 1,
    L2_CS3_Comp_Group2     = 2,
    L2_CS3_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x16_Dot_L2_CS3_Group;
typedef enum
{
    L2_CS2_No_Group_member = 0,
    L2_CS2_Comp_Group1     = 1,
    L2_CS2_Comp_Group2     = 2,
    L2_CS2_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x16_Dot_L2_CS2_Group;
typedef enum
{
    L2_CS1_No_Group_member = 0,
    L2_CS1_Comp_Group1     = 1,
    L2_CS1_Comp_Group2     = 2,
    L2_CS1_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x16_Dot_L2_CS1_Group;
typedef enum
{
    L2_CS0_No_Group_member = 0,
    L2_CS0_Comp_Group1     = 1,
    L2_CS0_Comp_Group2     = 2,
    L2_CS0_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x16_Dot_L2_CS0_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x16_Dot_L2_CS3_Group Dot_L2_CS3_Group : 2;
        AlxLp586x_RegEnum_0x16_Dot_L2_CS2_Group Dot_L2_CS2_Group : 2;
        AlxLp586x_RegEnum_0x16_Dot_L2_CS1_Group Dot_L2_CS1_Group : 2;
        AlxLp586x_RegEnum_0x16_Dot_L2_CS0_Group Dot_L2_CS0_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x16_Dot_grp_sel10;


//******************************************************************************
// Address: 0x17, Reset: 0x00 , Name: Dot_grp_sel11
//******************************************************************************
typedef enum
{
    L2_CS7_No_Group_member = 0,
    L2_CS7_Comp_Group1     = 1,
    L2_CS7_Comp_Group2     = 2,
    L2_CS7_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x17_Dot_L2_CS7_Group;
typedef enum
{
    L2_CS6_No_Group_member = 0,
    L2_CS6_Comp_Group1     = 1,
    L2_CS6_Comp_Group2     = 2,
    L2_CS6_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x17_Dot_L2_CS6_Group;
typedef enum
{
    L2_CS5_No_Group_member = 0,
    L2_CS5_Comp_Group1     = 1,
    L2_CS5_Comp_Group2     = 2,
    L2_CS5_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x17_Dot_L2_CS5_Group;
typedef enum
{
    L2_CS4_No_Group_member = 0,
    L2_CS4_Comp_Group1     = 1,
    L2_CS4_Comp_Group2     = 2,
    L2_CS4_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x17_Dot_L2_CS4_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x17_Dot_L2_CS7_Group Dot_L2_CS7_Group : 2;
        AlxLp586x_RegEnum_0x17_Dot_L2_CS6_Group Dot_L2_CS6_Group : 2;
        AlxLp586x_RegEnum_0x17_Dot_L2_CS5_Group Dot_L2_CS5_Group : 2;
        AlxLp586x_RegEnum_0x17_Dot_L2_CS4_Group Dot_L2_CS4_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x17_Dot_grp_sel11;


//******************************************************************************
// Address: 0x18, Reset: 0x00 , Name: Dot_grp_sel12
//******************************************************************************
typedef enum
{
    L2_CS11_No_Group_member = 0,
    L2_CS11_Comp_Group1     = 1,
    L2_CS11_Comp_Group2     = 2,
    L2_CS11_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x18_Dot_L2_CS11_Group;
typedef enum
{
    L2_CS10_No_Group_member = 0,
    L2_CS10_Comp_Group1     = 1,
    L2_CS10_Comp_Group2     = 2,
    L2_CS10_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x18_Dot_L2_CS10_Group;
typedef enum
{
    L2_CS9_No_Group_member = 0,
    L2_CS9_Comp_Group1     = 1,
    L2_CS9_Comp_Group2     = 2,
    L2_CS9_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x18_Dot_L2_CS9_Group;
typedef enum
{
    L2_CS8_No_Group_member = 0,
    L2_CS8_Comp_Group1     = 1,
    L2_CS8_Comp_Group2     = 2,
    L2_CS8_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x18_Dot_L2_CS8_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x18_Dot_L2_CS11_Group Dot_L2_CS11_Group : 2;
        AlxLp586x_RegEnum_0x18_Dot_L2_CS10_Group Dot_L2_CS10_Group : 2;
        AlxLp586x_RegEnum_0x18_Dot_L2_CS9_Group  Dot_L2_CS9_Group  : 2;
        AlxLp586x_RegEnum_0x18_Dot_L2_CS8_Group  Dot_L2_CS8_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x18_Dot_grp_sel12;


//******************************************************************************
// Address: 0x19, Reset: 0x00 , Name: Dot_grp_sel13
//******************************************************************************
typedef enum
{
    L2_CS15_No_Group_member = 0,
    L2_CS15_Comp_Group1     = 1,
    L2_CS15_Comp_Group2     = 2,
    L2_CS15_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x19_Dot_L2_CS15_Group;
typedef enum
{
    L2_CS14_No_Group_member = 0,
    L2_CS14_Comp_Group1     = 1,
    L2_CS14_Comp_Group2     = 2,
    L2_CS14_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x19_Dot_L2_CS14_Group;
typedef enum
{
    L2_CS13_No_Group_member = 0,
    L2_CS13_Comp_Group1     = 1,
    L2_CS13_Comp_Group2     = 2,
    L2_CS13_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x19_Dot_L2_CS13_Group;
typedef enum
{
    L2_CS12_No_Group_member = 0,
    L2_CS12_Comp_Group1     = 1,
    L2_CS12_Comp_Group2     = 2,
    L2_CS12_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x19_Dot_L2_CS12_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x19_Dot_L2_CS15_Group Dot_L2_CS15_Group : 2;
        AlxLp586x_RegEnum_0x19_Dot_L2_CS14_Group Dot_L2_CS14_Group : 2;
        AlxLp586x_RegEnum_0x19_Dot_L2_CS13_Group Dot_L2_CS13_Group : 2;
        AlxLp586x_RegEnum_0x19_Dot_L2_CS12_Group Dot_L2_CS12_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x19_Dot_grp_sel13;


//******************************************************************************
// Address: 0x1A, Reset: 0x00 , Name: Dot_grp_sel14
//******************************************************************************
typedef enum
{
    L2_CS17_No_Group_member = 0,
    L2_CS17_Comp_Group1     = 1,
    L2_CS17_Comp_Group2     = 2,
    L2_CS17_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1A_Dot_L2_CS17_Group;
typedef enum
{
    L2_CS16_No_Group_member = 0,
    L2_CS16_Comp_Group1     = 1,
    L2_CS16_Comp_Group2     = 2,
    L2_CS16_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1A_Dot_L2_CS16_Group;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_3											: 4;
        AlxLp586x_RegEnum_0x1A_Dot_L2_CS17_Group Dot_L2_CS17_Group	: 2;
        AlxLp586x_RegEnum_0x1A_Dot_L2_CS16_Group Dot_L2_CS16_Group	: 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x1A_Dot_grp_sel14;


//******************************************************************************
// Address: 0x1B, Reset: 0x00 , Name: Dot_grp_sel15
//******************************************************************************
typedef enum
{
    L3_CS3_No_Group_member = 0,
    L3_CS3_Comp_Group1     = 1,
    L3_CS3_Comp_Group2     = 2,
    L3_CS3_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1B_Dot_L3_CS3_Group;
typedef enum
{
    L3_CS2_No_Group_member = 0,
    L3_CS2_Comp_Group1     = 1,
    L3_CS2_Comp_Group2     = 2,
    L3_CS2_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1B_Dot_L3_CS2_Group;
typedef enum
{
    L3_CS1_No_Group_member = 0,
    L3_CS1_Comp_Group1     = 1,
    L3_CS1_Comp_Group2     = 2,
    L3_CS1_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1B_Dot_L3_CS1_Group;
typedef enum
{
    L3_CS0_No_Group_member = 0,
    L3_CS0_Comp_Group1     = 1,
    L3_CS0_Comp_Group2     = 2,
    L3_CS0_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1B_Dot_L3_CS0_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x1B_Dot_L3_CS3_Group Dot_L3_CS3_Group : 2;
        AlxLp586x_RegEnum_0x1B_Dot_L3_CS2_Group Dot_L3_CS2_Group : 2;
        AlxLp586x_RegEnum_0x1B_Dot_L3_CS1_Group Dot_L3_CS1_Group : 2;
        AlxLp586x_RegEnum_0x1B_Dot_L3_CS0_Group Dot_L3_CS0_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x1B_Dot_grp_sel15;


//******************************************************************************
// Address: 0x1C, Reset: 0x00 , Name: Dot_grp_sel16
//******************************************************************************
typedef enum
{
    L3_CS7_No_Group_member = 0,
    L3_CS7_Comp_Group1     = 1,
    L3_CS7_Comp_Group2     = 2,
    L3_CS7_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1C_Dot_L3_CS7_Group;
typedef enum
{
    L3_CS6_No_Group_member = 0,
    L3_CS6_Comp_Group1     = 1,
    L3_CS6_Comp_Group2     = 2,
    L3_CS6_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1C_Dot_L3_CS6_Group;
typedef enum
{
    L3_CS5_No_Group_member = 0,
    L3_CS5_Comp_Group1     = 1,
    L3_CS5_Comp_Group2     = 2,
    L3_CS5_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1C_Dot_L3_CS5_Group;
typedef enum
{
    L3_CS4_No_Group_member = 0,
    L3_CS4_Comp_Group1     = 1,
    L3_CS4_Comp_Group2     = 2,
    L3_CS4_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1C_Dot_L3_CS4_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x1C_Dot_L3_CS7_Group Dot_L3_CS7_Group : 2;
        AlxLp586x_RegEnum_0x1C_Dot_L3_CS6_Group Dot_L3_CS6_Group : 2;
        AlxLp586x_RegEnum_0x1C_Dot_L3_CS5_Group Dot_L3_CS5_Group : 2;
        AlxLp586x_RegEnum_0x1C_Dot_L3_CS4_Group Dot_L3_CS4_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x1C_Dot_grp_sel16;


//******************************************************************************
// Address: 0x1D, Reset: 0x00 , Name: Dot_grp_sel17
//******************************************************************************
typedef enum
{
    L3_CS11_No_Group_member = 0,
    L3_CS11_Comp_Group1     = 1,
    L3_CS11_Comp_Group2     = 2,
    L3_CS11_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1D_Dot_L3_CS11_Group;
typedef enum
{
    L3_CS10_No_Group_member = 0,
    L3_CS10_Comp_Group1     = 1,
    L3_CS10_Comp_Group2     = 2,
    L3_CS10_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1D_Dot_L3_CS10_Group;
typedef enum
{
    L3_CS9_No_Group_member = 0,
    L3_CS9_Comp_Group1     = 1,
    L3_CS9_Comp_Group2     = 2,
    L3_CS9_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1D_Dot_L3_CS9_Group;
typedef enum
{
    L3_CS8_No_Group_member = 0,
    L3_CS8_Comp_Group1     = 1,
    L3_CS8_Comp_Group2     = 2,
    L3_CS8_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1D_Dot_L3_CS8_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x1D_Dot_L3_CS11_Group Dot_L3_CS11_Group : 2;
        AlxLp586x_RegEnum_0x1D_Dot_L3_CS10_Group Dot_L3_CS10_Group : 2;
        AlxLp586x_RegEnum_0x1D_Dot_L3_CS9_Group  Dot_L3_CS9_Group  : 2;
        AlxLp586x_RegEnum_0x1D_Dot_L3_CS8_Group  Dot_L3_CS8_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x1D_Dot_grp_sel17;


//******************************************************************************
// Address: 0x1E, Reset: 0x00 , Name: Dot_grp_sel18
//******************************************************************************
typedef enum
{
    L3_CS15_No_Group_member = 0,
    L3_CS15_Comp_Group1     = 1,
    L3_CS15_Comp_Group2     = 2,
    L3_CS15_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1E_Dot_L3_CS15_Group;
typedef enum
{
    L3_CS14_No_Group_member = 0,
    L3_CS14_Comp_Group1     = 1,
    L3_CS14_Comp_Group2     = 2,
    L3_CS14_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1E_Dot_L3_CS14_Group;
typedef enum
{
    L3_CS13_No_Group_member = 0,
    L3_CS13_Comp_Group1     = 1,
    L3_CS13_Comp_Group2     = 2,
    L3_CS13_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1E_Dot_L3_CS13_Group;
typedef enum
{
    L3_CS12_No_Group_member = 0,
    L3_CS12_Comp_Group1     = 1,
    L3_CS12_Comp_Group2     = 2,
    L3_CS12_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1E_Dot_L3_CS12_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x1E_Dot_L3_CS15_Group Dot_L3_CS15_Group : 2;
        AlxLp586x_RegEnum_0x1E_Dot_L3_CS14_Group Dot_L3_CS14_Group : 2;
        AlxLp586x_RegEnum_0x1E_Dot_L3_CS13_Group Dot_L3_CS13_Group : 2;
        AlxLp586x_RegEnum_0x1E_Dot_L3_CS12_Group Dot_L3_CS12_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x1E_Dot_grp_sel18;


//******************************************************************************
// Address: 0x1F, Reset: 0x00 , Name: Dot_grp_sel19
//******************************************************************************
typedef enum
{
    L3_CS17_No_Group_member = 0,
    L3_CS17_Comp_Group1     = 1,
    L3_CS17_Comp_Group2     = 2,
    L3_CS17_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1F_Dot_L3_CS17_Group;
typedef enum
{
    L3_CS16_No_Group_member = 0,
    L3_CS16_Comp_Group1     = 1,
    L3_CS16_Comp_Group2     = 2,
    L3_CS16_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x1F_Dot_L3_CS16_Group;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_3                                          : 4;
        AlxLp586x_RegEnum_0x1F_Dot_L3_CS17_Group Dot_L3_CS17_Group  : 2;
        AlxLp586x_RegEnum_0x1F_Dot_L3_CS16_Group Dot_L3_CS16_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x1F_Dot_grp_sel19;


//******************************************************************************
// Address: 0x20, Reset: 0x00 , Name: Dot_grp_sel20
//******************************************************************************
typedef enum
{
    L4_CS3_No_Group_member = 0,
    L4_CS3_Comp_Group1     = 1,
    L4_CS3_Comp_Group2     = 2,
    L4_CS3_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x20_Dot_L4_CS3_Group;
typedef enum
{
    L4_CS2_No_Group_member = 0,
    L4_CS2_Comp_Group1     = 1,
    L4_CS2_Comp_Group2     = 2,
    L4_CS2_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x20_Dot_L4_CS2_Group;
typedef enum
{
    L4_CS1_No_Group_member = 0,
    L4_CS1_Comp_Group1     = 1,
    L4_CS1_Comp_Group2     = 2,
    L4_CS1_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x20_Dot_L4_CS1_Group;
typedef enum
{
    L4_CS0_No_Group_member = 0,
    L4_CS0_Comp_Group1     = 1,
    L4_CS0_Comp_Group2     = 2,
    L4_CS0_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x20_Dot_L4_CS0_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x20_Dot_L4_CS3_Group Dot_L4_CS3_Group : 2;
        AlxLp586x_RegEnum_0x20_Dot_L4_CS2_Group Dot_L4_CS2_Group : 2;
        AlxLp586x_RegEnum_0x20_Dot_L4_CS1_Group Dot_L4_CS1_Group : 2;
        AlxLp586x_RegEnum_0x20_Dot_L4_CS0_Group Dot_L4_CS0_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x20_Dot_grp_sel20;


//******************************************************************************
// Address: 0x21, Reset: 0x00 , Name: Dot_grp_sel21
//******************************************************************************
typedef enum
{
    L4_CS7_No_Group_member = 0,
    L4_CS7_Comp_Group1     = 1,
    L4_CS7_Comp_Group2     = 2,
    L4_CS7_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x21_Dot_L4_CS7_Group;
typedef enum
{
    L4_CS6_No_Group_member = 0,
    L4_CS6_Comp_Group1     = 1,
    L4_CS6_Comp_Group2     = 2,
    L4_CS6_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x21_Dot_L4_CS6_Group;
typedef enum
{
    L4_CS5_No_Group_member = 0,
    L4_CS5_Comp_Group1     = 1,
    L4_CS5_Comp_Group2     = 2,
    L4_CS5_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x21_Dot_L4_CS5_Group;
typedef enum
{
    L4_CS4_No_Group_member = 0,
    L4_CS4_Comp_Group1     = 1,
    L4_CS4_Comp_Group2     = 2,
    L4_CS4_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x21_Dot_L4_CS4_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x21_Dot_L4_CS7_Group Dot_L4_CS7_Group : 2;
        AlxLp586x_RegEnum_0x21_Dot_L4_CS6_Group Dot_L4_CS6_Group : 2;
        AlxLp586x_RegEnum_0x21_Dot_L4_CS5_Group Dot_L4_CS5_Group : 2;
        AlxLp586x_RegEnum_0x21_Dot_L4_CS4_Group Dot_L4_CS4_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x21_Dot_grp_sel21;


//******************************************************************************
// Address: 0x22, Reset: 0x00 , Name: Dot_grp_sel22
//******************************************************************************
typedef enum
{
    L4_CS11_No_Group_member = 0,
    L4_CS11_Comp_Group1     = 1,
    L4_CS11_Comp_Group2     = 2,
    L4_CS11_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x22_Dot_L4_CS11_Group;
typedef enum
{
    L4_CS10_No_Group_member = 0,
    L4_CS10_Comp_Group1     = 1,
    L4_CS10_Comp_Group2     = 2,
    L4_CS10_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x22_Dot_L4_CS10_Group;
typedef enum
{
    L4_CS9_No_Group_member = 0,
    L4_CS9_Comp_Group1     = 1,
    L4_CS9_Comp_Group2     = 2,
    L4_CS9_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x22_Dot_L4_CS9_Group;
typedef enum
{
    L4_CS8_No_Group_member = 0,
    L4_CS8_Comp_Group1     = 1,
    L4_CS8_Comp_Group2     = 2,
    L4_CS8_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x22_Dot_L4_CS8_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x22_Dot_L4_CS11_Group Dot_L4_CS11_Group : 2;
        AlxLp586x_RegEnum_0x22_Dot_L4_CS10_Group Dot_L4_CS10_Group : 2;
        AlxLp586x_RegEnum_0x22_Dot_L4_CS9_Group  Dot_L4_CS9_Group  : 2;
        AlxLp586x_RegEnum_0x22_Dot_L4_CS8_Group  Dot_L4_CS8_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x22_Dot_grp_sel22;


//******************************************************************************
// Address: 0x23, Reset: 0x00 , Name: Dot_grp_sel23
//******************************************************************************
typedef enum
{
    L4_CS15_No_Group_member = 0,
    L4_CS15_Comp_Group1     = 1,
    L4_CS15_Comp_Group2     = 2,
    L4_CS15_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x23_Dot_L4_CS15_Group;
typedef enum
{
    L4_CS14_No_Group_member = 0,
    L4_CS14_Comp_Group1     = 1,
    L4_CS14_Comp_Group2     = 2,
    L4_CS14_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x23_Dot_L4_CS14_Group;
typedef enum
{
    L4_CS13_No_Group_member = 0,
    L4_CS13_Comp_Group1     = 1,
    L4_CS13_Comp_Group2     = 2,
    L4_CS13_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x23_Dot_L4_CS13_Group;
typedef enum
{
    L4_CS12_No_Group_member = 0,
    L4_CS12_Comp_Group1     = 1,
    L4_CS12_Comp_Group2     = 2,
    L4_CS12_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x23_Dot_L4_CS12_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x23_Dot_L4_CS15_Group Dot_L4_CS15_Group : 2;
        AlxLp586x_RegEnum_0x23_Dot_L4_CS14_Group Dot_L4_CS14_Group : 2;
        AlxLp586x_RegEnum_0x23_Dot_L4_CS13_Group Dot_L4_CS13_Group : 2;
        AlxLp586x_RegEnum_0x23_Dot_L4_CS12_Group Dot_L4_CS12_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x23_Dot_grp_sel23;


//******************************************************************************
// Address: 0x24, Reset: 0x00 , Name: Dot_grp_sel24
//******************************************************************************
typedef enum
{
    L4_CS17_No_Group_member = 0,
    L4_CS17_Comp_Group1     = 1,
    L4_CS17_Comp_Group2     = 2,
    L4_CS17_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x24_Dot_L4_CS17_Group;
typedef enum
{
    L4_CS16_No_Group_member = 0,
    L4_CS16_Comp_Group1     = 1,
    L4_CS16_Comp_Group2     = 2,
    L4_CS16_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x24_Dot_L4_CS16_Group;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_3                                          : 4;
        AlxLp586x_RegEnum_0x24_Dot_L4_CS17_Group Dot_L4_CS17_Group  : 2;
        AlxLp586x_RegEnum_0x24_Dot_L4_CS16_Group Dot_L4_CS16_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x24_Dot_grp_sel24;


//******************************************************************************
// Address: 0x25, Reset: 0x00 , Name: Dot_grp_sel25
//******************************************************************************
typedef enum
{
    L5_CS3_No_Group_member = 0,
    L5_CS3_Comp_Group1     = 1,
    L5_CS3_Comp_Group2     = 2,
    L5_CS3_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x25_Dot_L5_CS3_Group;
typedef enum
{
    L5_CS2_No_Group_member = 0,
    L5_CS2_Comp_Group1     = 1,
    L5_CS2_Comp_Group2     = 2,
    L5_CS2_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x25_Dot_L5_CS2_Group;
typedef enum
{
    L5_CS1_No_Group_member = 0,
    L5_CS1_Comp_Group1     = 1,
    L5_CS1_Comp_Group2     = 2,
    L5_CS1_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x25_Dot_L5_CS1_Group;
typedef enum
{
    L5_CS0_No_Group_member = 0,
    L5_CS0_Comp_Group1     = 1,
    L5_CS0_Comp_Group2     = 2,
    L5_CS0_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x25_Dot_L5_CS0_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x25_Dot_L5_CS3_Group Dot_L5_CS3_Group : 2;
        AlxLp586x_RegEnum_0x25_Dot_L5_CS2_Group Dot_L5_CS2_Group : 2;
        AlxLp586x_RegEnum_0x25_Dot_L5_CS1_Group Dot_L5_CS1_Group : 2;
        AlxLp586x_RegEnum_0x25_Dot_L5_CS0_Group Dot_L5_CS0_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x25_Dot_grp_sel25;


//******************************************************************************
// Address: 0x26, Reset: 0x00 , Name: Dot_grp_sel26
//******************************************************************************
typedef enum
{
    L5_CS7_No_Group_member = 0,
    L5_CS7_Comp_Group1     = 1,
    L5_CS7_Comp_Group2     = 2,
    L5_CS7_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x26_Dot_L5_CS7_Group;
typedef enum
{
    L5_CS6_No_Group_member = 0,
    L5_CS6_Comp_Group1     = 1,
    L5_CS6_Comp_Group2     = 2,
    L5_CS6_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x26_Dot_L5_CS6_Group;
typedef enum
{
    L5_CS5_No_Group_member = 0,
    L5_CS5_Comp_Group1     = 1,
    L5_CS5_Comp_Group2     = 2,
    L5_CS5_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x26_Dot_L5_CS5_Group;
typedef enum
{
    L5_CS4_No_Group_member = 0,
    L5_CS4_Comp_Group1     = 1,
    L5_CS4_Comp_Group2     = 2,
    L5_CS4_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x26_Dot_L5_CS4_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x26_Dot_L5_CS7_Group Dot_L5_CS7_Group : 2;
        AlxLp586x_RegEnum_0x26_Dot_L5_CS6_Group Dot_L5_CS6_Group : 2;
        AlxLp586x_RegEnum_0x26_Dot_L5_CS5_Group Dot_L5_CS5_Group : 2;
        AlxLp586x_RegEnum_0x26_Dot_L5_CS4_Group Dot_L5_CS4_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x26_Dot_grp_sel26;


//******************************************************************************
// Address: 0x27, Reset: 0x00 , Name: Dot_grp_sel27
//******************************************************************************
typedef enum
{
    L5_CS11_No_Group_member = 0,
    L5_CS11_Comp_Group1     = 1,
    L5_CS11_Comp_Group2     = 2,
    L5_CS11_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x27_Dot_L5_CS11_Group;
typedef enum
{
    L5_CS10_No_Group_member = 0,
    L5_CS10_Comp_Group1     = 1,
    L5_CS10_Comp_Group2     = 2,
    L5_CS10_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x27_Dot_L5_CS10_Group;
typedef enum
{
    L5_CS9_No_Group_member = 0,
    L5_CS9_Comp_Group1     = 1,
    L5_CS9_Comp_Group2     = 2,
    L5_CS9_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x27_Dot_L5_CS9_Group;
typedef enum
{
    L5_CS8_No_Group_member = 0,
    L5_CS8_Comp_Group1     = 1,
    L5_CS8_Comp_Group2     = 2,
    L5_CS8_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x27_Dot_L5_CS8_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x27_Dot_L5_CS11_Group Dot_L5_CS11_Group : 2;
        AlxLp586x_RegEnum_0x27_Dot_L5_CS10_Group Dot_L5_CS10_Group : 2;
        AlxLp586x_RegEnum_0x27_Dot_L5_CS9_Group  Dot_L5_CS9_Group  : 2;
        AlxLp586x_RegEnum_0x27_Dot_L5_CS8_Group  Dot_L5_CS8_Group  : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x27_Dot_grp_sel27;


//******************************************************************************
// Address: 0x28, Reset: 0x00 , Name: Dot_grp_sel28
//******************************************************************************
typedef enum
{
    L5_CS15_No_Group_member = 0,
    L5_CS15_Comp_Group1     = 1,
    L5_CS15_Comp_Group2     = 2,
    L5_CS15_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x28_Dot_L5_CS15_Group;
typedef enum
{
    L5_CS14_No_Group_member = 0,
    L5_CS14_Comp_Group1     = 1,
    L5_CS14_Comp_Group2     = 2,
    L5_CS14_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x28_Dot_L5_CS14_Group;
typedef enum
{
    L5_CS13_No_Group_member = 0,
    L5_CS13_Comp_Group1     = 1,
    L5_CS13_Comp_Group2     = 2,
    L5_CS13_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x28_Dot_L5_CS13_Group;
typedef enum
{
    L5_CS12_No_Group_member = 0,
    L5_CS12_Comp_Group1     = 1,
    L5_CS12_Comp_Group2     = 2,
    L5_CS12_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x28_Dot_L5_CS12_Group;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x28_Dot_L5_CS15_Group Dot_L5_CS15_Group : 2;
        AlxLp586x_RegEnum_0x28_Dot_L5_CS14_Group Dot_L5_CS14_Group : 2;
        AlxLp586x_RegEnum_0x28_Dot_L5_CS13_Group Dot_L5_CS13_Group : 2;
        AlxLp586x_RegEnum_0x28_Dot_L5_CS12_Group Dot_L5_CS12_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x28_Dot_grp_sel28;


//******************************************************************************
// Address: 0x29, Reset: 0x00 , Name: Dot_grp_sel29
//******************************************************************************
typedef enum
{
    L5_CS17_No_Group_member = 0,
    L5_CS17_Comp_Group1     = 1,
    L5_CS17_Comp_Group2     = 2,
    L5_CS17_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x29_Dot_L5_CS17_Group;
typedef enum
{
    L5_CS16_No_Group_member = 0,
    L5_CS16_Comp_Group1     = 1,
    L5_CS16_Comp_Group2     = 2,
    L5_CS16_Comp_Group3     = 3
} AlxLp586x_RegEnum_0x29_Dot_L5_CS16_Group;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_3                                          : 4;
        AlxLp586x_RegEnum_0x29_Dot_L5_CS17_Group  Dot_L5_CS17_Group : 2;
        AlxLp586x_RegEnum_0x29_Dot_L5_CS16_Group  Dot_L5_CS16_Group : 2;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x29_Dot_grp_sel29;


//******************************************************************************
// Address: 0x43, Reset: 0xFF , Name: Dot_onoff0
//******************************************************************************
typedef enum
{
    L0_CS7_Off = 0,
    L0_CS7_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS7_OnOff;
typedef enum
{
    L0_CS6_Off = 0,
    L0_CS6_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS6_OnOff;
typedef enum
{
    L0_CS5_Off = 0,
    L0_CS5_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS5_OnOff;
typedef enum
{
    L0_CS4_Off = 0,
    L0_CS4_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS4_OnOff;
typedef enum
{
    L0_CS3_Off = 0,
    L0_CS3_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS3_OnOff;
typedef enum
{
    L0_CS2_Off = 0,
    L0_CS2_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS2_OnOff;
typedef enum
{
    L0_CS1_Off = 0,
    L0_CS1_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS1_OnOff;
typedef enum
{
    L0_CS0_Off = 0,
    L0_CS0_On  = 1
} AlxLp586x_RegEnum_0x43_Dot_L0_CS0_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x43_Dot_L0_CS7_OnOff Dot_L0_CS7_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS6_OnOff Dot_L0_CS6_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS5_OnOff Dot_L0_CS5_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS4_OnOff Dot_L0_CS4_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS3_OnOff Dot_L0_CS3_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS2_OnOff Dot_L0_CS2_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS1_OnOff Dot_L0_CS1_OnOff : 1;
        AlxLp586x_RegEnum_0x43_Dot_L0_CS0_OnOff Dot_L0_CS0_OnOff : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x43_Dot_onoff0;


//******************************************************************************
// Address: 0x44, Reset: 0xFF , Name: Dot_onoff1
//******************************************************************************
typedef enum
{
    L0_CS15_Off = 0,
    L0_CS15_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS15_OnOff;
typedef enum
{
    L0_CS14_Off = 0,
    L0_CS14_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS14_OnOff;
typedef enum
{
    L0_CS13_Off = 0,
    L0_CS13_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS13_OnOff;
typedef enum
{
    L0_CS12_Off = 0,
    L0_CS12_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS12_OnOff;
typedef enum
{
    L0_CS11_Off = 0,
    L0_CS11_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS11_OnOff;
typedef enum
{
    L0_CS10_Off = 0,
    L0_CS10_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS10_OnOff;
typedef enum
{
    L0_CS9_Off = 0,
    L0_CS9_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS9_OnOff;
typedef enum
{
    L0_CS8_Off = 0,
    L0_CS8_On  = 1
} AlxLp586x_RegEnum_0x44_Dot_L0_CS8_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x44_Dot_L0_CS15_OnOff Dot_L0_CS15_OnOff : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS14_OnOff Dot_L0_CS14_OnOff : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS13_OnOff Dot_L0_CS13_OnOff : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS12_OnOff Dot_L0_CS12_OnOff : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS11_OnOff Dot_L0_CS11_OnOff : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS10_OnOff Dot_L0_CS10_OnOff : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS9_OnOff  Dot_L0_CS9_OnOff  : 1;
        AlxLp586x_RegEnum_0x44_Dot_L0_CS8_OnOff  Dot_L0_CS8_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x44_Dot_onoff1;


//******************************************************************************
// Address: 0x45, Reset: 0x03 , Name: Dot_onoff2
//******************************************************************************
typedef enum
{
    L0_CS17_Off = 0,
    L0_CS17_On  = 1
} AlxLp586x_RegEnum_0x45_Dot_L0_CS17_OnOff;
typedef enum
{
    L0_CS16_Off = 0,
    L0_CS16_On  = 1
} AlxLp586x_RegEnum_0x45_Dot_L0_CS16_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_5                                          : 6;
        AlxLp586x_RegEnum_0x45_Dot_L0_CS17_OnOff Dot_L0_CS17_OnOff  : 1;
        AlxLp586x_RegEnum_0x45_Dot_L0_CS16_OnOff Dot_L0_CS16_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x45_Dot_onoff2;


//******************************************************************************
// Address: 0x46, Reset: 0xFF , Name: Dot_onoff3
//******************************************************************************
typedef enum
{
    L1_CS7_Off = 0,
    L1_CS7_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS7_OnOff;
typedef enum
{
    L1_CS6_Off = 0,
    L1_CS6_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS6_OnOff;
typedef enum
{
    L1_CS5_Off = 0,
    L1_CS5_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS5_OnOff;
typedef enum
{
    L1_CS4_Off = 0,
    L1_CS4_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS4_OnOff;
typedef enum
{
    L1_CS3_Off = 0,
    L1_CS3_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS3_OnOff;
typedef enum
{
    L1_CS2_Off = 0,
    L1_CS2_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS2_OnOff;
typedef enum
{
    L1_CS1_Off = 0,
    L1_CS1_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS1_OnOff;
typedef enum
{
    L1_CS0_Off = 0,
    L1_CS0_On  = 1
} AlxLp586x_RegEnum_0x46_Dot_L1_CS0_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x46_Dot_L1_CS7_OnOff Dot_L1_CS7_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS6_OnOff Dot_L1_CS6_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS5_OnOff Dot_L1_CS5_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS4_OnOff Dot_L1_CS4_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS3_OnOff Dot_L1_CS3_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS2_OnOff Dot_L1_CS2_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS1_OnOff Dot_L1_CS1_OnOff : 1;
        AlxLp586x_RegEnum_0x46_Dot_L1_CS0_OnOff Dot_L1_CS0_OnOff : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x46_Dot_onoff3;


//******************************************************************************
// Address: 0x47, Reset: 0xFF , Name: Dot_onoff4
//******************************************************************************
typedef enum
{
    L1_CS15_Off = 0,
    L1_CS15_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS15_OnOff;
typedef enum
{
    L1_CS14_Off = 0,
    L1_CS14_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS14_OnOff;
typedef enum
{
    L1_CS13_Off = 0,
    L1_CS13_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS13_OnOff;
typedef enum
{
    L1_CS12_Off = 0,
    L1_CS12_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS12_OnOff;
typedef enum
{
    L1_CS11_Off = 0,
    L1_CS11_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS11_OnOff;
typedef enum
{
    L1_CS10_Off = 0,
    L1_CS10_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS10_OnOff;
typedef enum
{
    L1_CS9_Off = 0,
    L1_CS9_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS9_OnOff;
typedef enum
{
    L1_CS8_Off = 0,
    L1_CS8_On  = 1
} AlxLp586x_RegEnum_0x47_Dot_L1_CS8_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x47_Dot_L1_CS15_OnOff Dot_L1_CS15_OnOff : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS14_OnOff Dot_L1_CS14_OnOff : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS13_OnOff Dot_L1_CS13_OnOff : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS12_OnOff Dot_L1_CS12_OnOff : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS11_OnOff Dot_L1_CS11_OnOff : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS10_OnOff Dot_L1_CS10_OnOff : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS9_OnOff  Dot_L1_CS9_OnOff  : 1;
        AlxLp586x_RegEnum_0x47_Dot_L1_CS8_OnOff  Dot_L1_CS8_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x47_Dot_onoff4;


//******************************************************************************
// Address: 0x48, Reset: 0x03 , Name: Dot_onoff5
//******************************************************************************
typedef enum
{
    L1_CS17_Off = 0,
    L1_CS17_On  = 1
} AlxLp586x_RegEnum_0x48_Dot_L1_CS17_OnOff;
typedef enum
{
    L1_CS16_Off = 0,
    L1_CS16_On  = 1
} AlxLp586x_RegEnum_0x48_Dot_L1_CS16_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                          : 6;
        AlxLp586x_RegEnum_0x48_Dot_L1_CS17_OnOff Dot_L1_CS17_OnOff  : 1;
        AlxLp586x_RegEnum_0x48_Dot_L1_CS16_OnOff Dot_L1_CS16_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x48_Dot_onoff5;


//******************************************************************************
// Address: 0x49, Reset: 0xFF , Name: Dot_onoff6
//******************************************************************************
typedef enum
{
    L2_CS7_Off = 0,
    L2_CS7_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS7_OnOff;
typedef enum
{
    L2_CS6_Off = 0,
    L2_CS6_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS6_OnOff;
typedef enum
{
    L2_CS5_Off = 0,
    L2_CS5_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS5_OnOff;
typedef enum
{
    L2_CS4_Off = 0,
    L2_CS4_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS4_OnOff;
typedef enum
{
    L2_CS3_Off = 0,
    L2_CS3_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS3_OnOff;
typedef enum
{
    L2_CS2_Off = 0,
    L2_CS2_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS2_OnOff;
typedef enum
{
    L2_CS1_Off = 0,
    L2_CS1_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS1_OnOff;
typedef enum
{
    L2_CS0_Off = 0,
    L2_CS0_On  = 1
} AlxLp586x_RegEnum_0x49_Dot_L2_CS0_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x49_Dot_L2_CS7_OnOff Dot_L2_CS7_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS6_OnOff Dot_L2_CS6_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS5_OnOff Dot_L2_CS5_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS4_OnOff Dot_L2_CS4_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS3_OnOff Dot_L2_CS3_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS2_OnOff Dot_L2_CS2_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS1_OnOff Dot_L2_CS1_OnOff : 1;
        AlxLp586x_RegEnum_0x49_Dot_L2_CS0_OnOff Dot_L2_CS0_OnOff : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x49_Dot_onoff6;


//******************************************************************************
// Address: 0x4A, Reset: 0xFF , Name: Dot_onoff7
//******************************************************************************
typedef enum
{
    L2_CS15_Off = 0,
    L2_CS15_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS15_OnOff;
typedef enum
{
    L2_CS14_Off = 0,
    L2_CS14_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS14_OnOff;
typedef enum
{
    L2_CS13_Off = 0,
    L2_CS13_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS13_OnOff;
typedef enum
{
    L2_CS12_Off = 0,
    L2_CS12_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS12_OnOff;
typedef enum
{
    L2_CS11_Off = 0,
    L2_CS11_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS11_OnOff;
typedef enum
{
    L2_CS10_Off = 0,
    L2_CS10_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS10_OnOff;
typedef enum
{
    L2_CS9_Off = 0,
    L2_CS9_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS9_OnOff;
typedef enum
{
    L2_CS8_Off = 0,
    L2_CS8_On  = 1
} AlxLp586x_RegEnum_0x4A_Dot_L2_CS8_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS15_OnOff Dot_L2_CS15_OnOff : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS14_OnOff Dot_L2_CS14_OnOff : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS13_OnOff Dot_L2_CS13_OnOff : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS12_OnOff Dot_L2_CS12_OnOff : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS11_OnOff Dot_L2_CS11_OnOff : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS10_OnOff Dot_L2_CS10_OnOff : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS9_OnOff  Dot_L2_CS9_OnOff  : 1;
        AlxLp586x_RegEnum_0x4A_Dot_L2_CS8_OnOff  Dot_L2_CS8_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x4A_Dot_onoff7;


//******************************************************************************
// Address: 0x4B, Reset: 0x03 , Name: Dot_onoff8
//******************************************************************************
typedef enum
{
    L2_CS17_Off = 0,
    L2_CS17_On  = 1
} AlxLp586x_RegEnum_0x4B_Dot_L2_CS17_OnOff;
typedef enum
{
    L2_CS16_Off = 0,
    L2_CS16_On  = 1
} AlxLp586x_RegEnum_0x4B_Dot_L2_CS16_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                          : 6;
        AlxLp586x_RegEnum_0x4B_Dot_L2_CS17_OnOff Dot_L2_CS17_OnOff  : 1;
        AlxLp586x_RegEnum_0x4B_Dot_L2_CS16_OnOff Dot_L2_CS16_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x4B_Dot_onoff8;


//******************************************************************************
// Address: 0x4C, Reset: 0xFF , Name: Dot_onoff9
//******************************************************************************
typedef enum
{
    L3_CS7_Off = 0,
    L3_CS7_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS7_OnOff;
typedef enum
{
    L3_CS6_Off = 0,
    L3_CS6_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS6_OnOff;
typedef enum
{
    L3_CS5_Off = 0,
    L3_CS5_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS5_OnOff;
typedef enum
{
    L3_CS4_Off = 0,
    L3_CS4_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS4_OnOff;
typedef enum
{
    L3_CS3_Off = 0,
    L3_CS3_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS3_OnOff;
typedef enum
{
    L3_CS2_Off = 0,
    L3_CS2_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS2_OnOff;
typedef enum
{
    L3_CS1_Off = 0,
    L3_CS1_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS1_OnOff;
typedef enum
{
    L3_CS0_Off = 0,
    L3_CS0_On  = 1
} AlxLp586x_RegEnum_0x4C_Dot_L3_CS0_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS7_OnOff Dot_L3_CS7_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS6_OnOff Dot_L3_CS6_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS5_OnOff Dot_L3_CS5_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS4_OnOff Dot_L3_CS4_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS3_OnOff Dot_L3_CS3_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS2_OnOff Dot_L3_CS2_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS1_OnOff Dot_L3_CS1_OnOff : 1;
        AlxLp586x_RegEnum_0x4C_Dot_L3_CS0_OnOff Dot_L3_CS0_OnOff : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x4C_Dot_onoff9;


//******************************************************************************
// Address: 0x4D, Reset: 0xFF , Name: Dot_onoff10
//******************************************************************************
typedef enum
{
    L3_CS15_Off = 0,
    L3_CS15_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS15_OnOff;
typedef enum
{
    L3_CS14_Off = 0,
    L3_CS14_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS14_OnOff;
typedef enum
{
    L3_CS13_Off = 0,
    L3_CS13_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS13_OnOff;
typedef enum
{
    L3_CS12_Off = 0,
    L3_CS12_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS12_OnOff;
typedef enum
{
    L3_CS11_Off = 0,
    L3_CS11_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS11_OnOff;
typedef enum
{
    L3_CS10_Off = 0,
    L3_CS10_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS10_OnOff;
typedef enum
{
    L3_CS9_Off = 0,
    L3_CS9_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS9_OnOff;
typedef enum
{
    L3_CS8_Off = 0,
    L3_CS8_On  = 1
} AlxLp586x_RegEnum_0x4D_Dot_L3_CS8_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS15_OnOff Dot_L3_CS15_OnOff : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS14_OnOff Dot_L3_CS14_OnOff : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS13_OnOff Dot_L3_CS13_OnOff : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS12_OnOff Dot_L3_CS12_OnOff : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS11_OnOff Dot_L3_CS11_OnOff : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS10_OnOff Dot_L3_CS10_OnOff : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS9_OnOff  Dot_L3_CS9_OnOff  : 1;
        AlxLp586x_RegEnum_0x4D_Dot_L3_CS8_OnOff  Dot_L3_CS8_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x4D_Dot_onoff10;


//******************************************************************************
// Address: 0x4E, Reset: 0x03 , Name: Dot_onoff11
//******************************************************************************
typedef enum
{
    L3_CS17_Off = 0,
    L3_CS17_On  = 1
} AlxLp586x_RegEnum_0x4E_Dot_L3_CS17_OnOff;
typedef enum
{
    L3_CS16_Off = 0,
    L3_CS16_On  = 1
} AlxLp586x_RegEnum_0x4E_Dot_L3_CS16_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                          : 6;
        AlxLp586x_RegEnum_0x4E_Dot_L3_CS17_OnOff Dot_L3_CS17_OnOff  : 1;
        AlxLp586x_RegEnum_0x4E_Dot_L3_CS16_OnOff Dot_L3_CS16_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x4E_Dot_onoff11;


//******************************************************************************
// Address: 0x4F, Reset: 0xFF , Name: Dot_onoff12
//******************************************************************************
typedef enum
{
    L4_CS7_Off = 0,
    L4_CS7_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS7_OnOff;
typedef enum
{
    L4_CS6_Off = 0,
    L4_CS6_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS6_OnOff;
typedef enum
{
    L4_CS5_Off = 0,
    L4_CS5_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS5_OnOff;
typedef enum
{
    L4_CS4_Off = 0,
    L4_CS4_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS4_OnOff;
typedef enum
{
    L4_CS3_Off = 0,
    L4_CS3_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS3_OnOff;
typedef enum
{
    L4_CS2_Off = 0,
    L4_CS2_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS2_OnOff;
typedef enum
{
    L4_CS1_Off = 0,
    L4_CS1_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS1_OnOff;
typedef enum
{
    L4_CS0_Off = 0,
    L4_CS0_On  = 1
} AlxLp586x_RegEnum_0x4F_Dot_L4_CS0_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS7_OnOff Dot_L4_CS7_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS6_OnOff Dot_L4_CS6_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS5_OnOff Dot_L4_CS5_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS4_OnOff Dot_L4_CS4_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS3_OnOff Dot_L4_CS3_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS2_OnOff Dot_L4_CS2_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS1_OnOff Dot_L4_CS1_OnOff : 1;
        AlxLp586x_RegEnum_0x4F_Dot_L4_CS0_OnOff Dot_L4_CS0_OnOff : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x4F_Dot_onoff12;


//******************************************************************************
// Address: 0x50, Reset: 0xFF , Name: Dot_onoff13
//******************************************************************************
typedef enum
{
    L4_CS15_Off = 0,
    L4_CS15_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS15_OnOff;
typedef enum
{
    L4_CS14_Off = 0,
    L4_CS14_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS14_OnOff;
typedef enum
{
    L4_CS13_Off = 0,
    L4_CS13_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS13_OnOff;
typedef enum
{
    L4_CS12_Off = 0,
    L4_CS12_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS12_OnOff;
typedef enum
{
    L4_CS11_Off = 0,
    L4_CS11_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS11_OnOff;
typedef enum
{
    L4_CS10_Off = 0,
    L4_CS10_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS10_OnOff;
typedef enum
{
    L4_CS9_Off = 0,
    L4_CS9_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS9_OnOff;
typedef enum
{
    L4_CS8_Off = 0,
    L4_CS8_On  = 1
} AlxLp586x_RegEnum_0x50_Dot_L4_CS8_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x50_Dot_L4_CS15_OnOff Dot_L4_CS15_OnOff : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS14_OnOff Dot_L4_CS14_OnOff : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS13_OnOff Dot_L4_CS13_OnOff : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS12_OnOff Dot_L4_CS12_OnOff : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS11_OnOff Dot_L4_CS11_OnOff : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS10_OnOff Dot_L4_CS10_OnOff : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS9_OnOff  Dot_L4_CS9_OnOff  : 1;
        AlxLp586x_RegEnum_0x50_Dot_L4_CS8_OnOff  Dot_L4_CS8_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x50_Dot_onoff13;


//******************************************************************************
// Address: 0x51, Reset: 0x03 , Name: Dot_onoff14
//******************************************************************************
typedef enum
{
    L4_CS17_Off = 0,
    L4_CS17_On  = 1
} AlxLp586x_RegEnum_0x51_Dot_L4_CS17_OnOff;
typedef enum
{
    L4_CS16_Off = 0,
    L4_CS16_On  = 1
} AlxLp586x_RegEnum_0x51_Dot_L4_CS16_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                          : 6;
        AlxLp586x_RegEnum_0x51_Dot_L4_CS17_OnOff Dot_L4_CS17_OnOff  : 1;
        AlxLp586x_RegEnum_0x51_Dot_L4_CS16_OnOff Dot_L4_CS16_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x51_Dot_onoff14;


//******************************************************************************
// Address: 0x52, Reset: 0xFF , Name: Dot_onoff15
//******************************************************************************
typedef enum
{
    L5_CS7_Off = 0,
    L5_CS7_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS7_OnOff;
typedef enum
{
    L5_CS6_Off = 0,
    L5_CS6_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS6_OnOff;
typedef enum
{
    L5_CS5_Off = 0,
    L5_CS5_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS5_OnOff;
typedef enum
{
    L5_CS4_Off = 0,
    L5_CS4_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS4_OnOff;
typedef enum
{
    L5_CS3_Off = 0,
    L5_CS3_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS3_OnOff;
typedef enum
{
    L5_CS2_Off = 0,
    L5_CS2_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS2_OnOff;
typedef enum
{
    L5_CS1_Off = 0,
    L5_CS1_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS1_OnOff;
typedef enum
{
    L5_CS0_Off = 0,
    L5_CS0_On  = 1
} AlxLp586x_RegEnum_0x52_Dot_L5_CS0_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x52_Dot_L5_CS7_OnOff Dot_L5_CS7_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS6_OnOff Dot_L5_CS6_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS5_OnOff Dot_L5_CS5_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS4_OnOff Dot_L5_CS4_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS3_OnOff Dot_L5_CS3_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS2_OnOff Dot_L5_CS2_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS1_OnOff Dot_L5_CS1_OnOff : 1;
        AlxLp586x_RegEnum_0x52_Dot_L5_CS0_OnOff Dot_L5_CS0_OnOff : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x52_Dot_onoff15;


//******************************************************************************
// Address: 0x53, Reset: 0xFF , Name: Dot_onoff16
//******************************************************************************
typedef enum
{
    L5_CS15_Off = 0,
    L5_CS15_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS15_OnOff;
typedef enum
{
    L5_CS14_Off = 0,
    L5_CS14_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS14_OnOff;
typedef enum
{
    L5_CS13_Off = 0,
    L5_CS13_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS13_OnOff;
typedef enum
{
    L5_CS12_Off = 0,
    L5_CS12_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS12_OnOff;
typedef enum
{
    L5_CS11_Off = 0,
    L5_CS11_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS11_OnOff;
typedef enum
{
    L5_CS10_Off = 0,
    L5_CS10_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS10_OnOff;
typedef enum
{
    L5_CS9_Off = 0,
    L5_CS9_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS9_OnOff;
typedef enum
{
    L5_CS8_Off = 0,
    L5_CS8_On  = 1
} AlxLp586x_RegEnum_0x53_Dot_L5_CS8_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x53_Dot_L5_CS15_OnOff Dot_L5_CS15_OnOff : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS14_OnOff Dot_L5_CS14_OnOff : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS13_OnOff Dot_L5_CS13_OnOff : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS12_OnOff Dot_L5_CS12_OnOff : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS11_OnOff Dot_L5_CS11_OnOff : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS10_OnOff Dot_L5_CS10_OnOff : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS9_OnOff  Dot_L5_CS9_OnOff  : 1;
        AlxLp586x_RegEnum_0x53_Dot_L5_CS8_OnOff  Dot_L5_CS8_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x53_Dot_onoff16;


//******************************************************************************
// Address: 0x54, Reset: 0x03 , Name: Dot_onoff17
//******************************************************************************
typedef enum
{
    L5_CS17_Off = 0,
    L5_CS17_On  = 1
} AlxLp586x_RegEnum_0x54_Dot_L5_CS17_OnOff;
typedef enum
{
    L5_CS16_Off = 0,
    L5_CS16_On  = 1
} AlxLp586x_RegEnum_0x54_Dot_L5_CS16_OnOff;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_5                                          : 6;
        AlxLp586x_RegEnum_0x54_Dot_L5_CS17_OnOff Dot_L5_CS17_OnOff  : 1;
        AlxLp586x_RegEnum_0x54_Dot_L5_CS16_OnOff Dot_L5_CS16_OnOff  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x54_Dot_onoff17;


//******************************************************************************
// Address: 0x64, Reset: 0x00 , Name: Fault_state
//******************************************************************************
typedef enum
{
    Global_LOD_Normal = 0,
    Global_LOD_Fault  = 1
} AlxLp586x_RegEnum_0x64_Global_LOD;
typedef enum
{
    Global_LSD_Normal = 0,
    Global_LSD_Fault  = 1
} AlxLp586x_RegEnum_0x64_Global_LSD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                              : 6;
        AlxLp586x_RegEnum_0x64_Global_LOD Global_LOD    : 1;
        AlxLp586x_RegEnum_0x64_Global_LSD Global_LSD    : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x64_Fault_state;


//******************************************************************************
// Address: 0x65, Reset: 0x00 , Name: Dot_lod0
//******************************************************************************
typedef enum
{
    L0_CS7_No_LOD = 0,
    L0_CS7_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS7_LOD;
typedef enum
{
    L0_CS6_No_LOD = 0,
    L0_CS6_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS6_LOD;
typedef enum
{
    L0_CS5_No_LOD = 0,
    L0_CS5_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS5_LOD;
typedef enum
{
    L0_CS4_No_LOD = 0,
    L0_CS4_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS4_LOD;
typedef enum
{
    L0_CS3_No_LOD = 0,
    L0_CS3_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS3_LOD;
typedef enum
{
    L0_CS2_No_LOD = 0,
    L0_CS2_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS2_LOD;
typedef enum
{
    L0_CS1_No_LOD = 0,
    L0_CS1_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS1_LOD;
typedef enum
{
    L0_CS0_No_LOD = 0,
    L0_CS0_LOD    = 1
} AlxLp586x_RegEnum_0x65_Dot_L0_CS0_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x65_Dot_L0_CS7_LOD Dot_L0_CS7_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS6_LOD Dot_L0_CS6_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS5_LOD Dot_L0_CS5_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS4_LOD Dot_L0_CS4_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS3_LOD Dot_L0_CS3_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS2_LOD Dot_L0_CS2_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS1_LOD Dot_L0_CS1_LOD : 1;
        AlxLp586x_RegEnum_0x65_Dot_L0_CS0_LOD Dot_L0_CS0_LOD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x65_Dot_lod0;


//******************************************************************************
// Address: 0x66, Reset: 0x00 , Name: Dot_lod1
//******************************************************************************
typedef enum
{
    L0_CS15_No_LOD = 0,
    L0_CS15_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS15_LOD;
typedef enum
{
    L0_CS14_No_LOD = 0,
    L0_CS14_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS14_LOD;
typedef enum
{
    L0_CS13_No_LOD = 0,
    L0_CS13_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS13_LOD;
typedef enum
{
    L0_CS12_No_LOD = 0,
    L0_CS12_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS12_LOD;
typedef enum
{
    L0_CS11_No_LOD = 0,
    L0_CS11_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS11_LOD;
typedef enum
{
    L0_CS10_No_LOD = 0,
    L0_CS10_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS10_LOD;
typedef enum
{
    L0_CS9_No_LOD = 0,
    L0_CS9_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS9_LOD;
typedef enum
{
    L0_CS8_No_LOD = 0,
    L0_CS8_LOD    = 1
} AlxLp586x_RegEnum_0x66_Dot_L0_CS8_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x66_Dot_L0_CS15_LOD Dot_L0_CS15_LOD : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS14_LOD Dot_L0_CS14_LOD : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS13_LOD Dot_L0_CS13_LOD : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS12_LOD Dot_L0_CS12_LOD : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS11_LOD Dot_L0_CS11_LOD : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS10_LOD Dot_L0_CS10_LOD : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS9_LOD  Dot_L0_CS9_LOD  : 1;
        AlxLp586x_RegEnum_0x66_Dot_L0_CS8_LOD  Dot_L0_CS8_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x66_Dot_lod1;


//******************************************************************************
// Address: 0x67, Reset: 0x00 , Name: Dot_lod2
//******************************************************************************
typedef enum
{
    L0_CS17_No_LOD = 0,
    L0_CS17_LOD    = 1
} AlxLp586x_RegEnum_0x67_Dot_L0_CS17_LOD;
typedef enum
{
    L0_CS16_No_LOD = 0,
    L0_CS16_LOD    = 1
} AlxLp586x_RegEnum_0x67_Dot_L0_CS16_LOD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x67_Dot_L0_CS17_LOD Dot_L0_CS17_LOD  : 1;
        AlxLp586x_RegEnum_0x67_Dot_L0_CS16_LOD Dot_L0_CS16_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x67_Dot_lod2;


//******************************************************************************
// Address: 0x68, Reset: 0x00 , Name: Dot_lod3
//******************************************************************************
typedef enum
{
    L1_CS7_No_LOD = 0,
    L1_CS7_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS7_LOD;
typedef enum
{
    L1_CS6_No_LOD = 0,
    L1_CS6_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS6_LOD;
typedef enum
{
    L1_CS5_No_LOD = 0,
    L1_CS5_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS5_LOD;
typedef enum
{
    L1_CS4_No_LOD = 0,
    L1_CS4_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS4_LOD;
typedef enum
{
    L1_CS3_No_LOD = 0,
    L1_CS3_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS3_LOD;
typedef enum
{
    L1_CS2_No_LOD = 0,
    L1_CS2_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS2_LOD;
typedef enum
{
    L1_CS1_No_LOD = 0,
    L1_CS1_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS1_LOD;
typedef enum
{
    L1_CS0_No_LOD = 0,
    L1_CS0_LOD    = 1
} AlxLp586x_RegEnum_0x68_Dot_L1_CS0_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x68_Dot_L1_CS7_LOD Dot_L1_CS7_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS6_LOD Dot_L1_CS6_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS5_LOD Dot_L1_CS5_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS4_LOD Dot_L1_CS4_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS3_LOD Dot_L1_CS3_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS2_LOD Dot_L1_CS2_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS1_LOD Dot_L1_CS1_LOD : 1;
        AlxLp586x_RegEnum_0x68_Dot_L1_CS0_LOD Dot_L1_CS0_LOD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x68_Dot_lod3;


//******************************************************************************
// Address: 0x69, Reset: 0x00 , Name: Dot_lod4
//******************************************************************************
typedef enum
{
    L1_CS15_No_LOD = 0,
    L1_CS15_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS15_LOD;
typedef enum
{
    L1_CS14_No_LOD = 0,
    L1_CS14_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS14_LOD;
typedef enum
{
    L1_CS13_No_LOD = 0,
    L1_CS13_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS13_LOD;
typedef enum
{
    L1_CS12_No_LOD = 0,
    L1_CS12_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS12_LOD;
typedef enum
{
    L1_CS11_No_LOD = 0,
    L1_CS11_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS11_LOD;
typedef enum
{
    L1_CS10_No_LOD = 0,
    L1_CS10_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS10_LOD;
typedef enum
{
    L1_CS9_No_LOD = 0,
    L1_CS9_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS9_LOD;
typedef enum
{
    L1_CS8_No_LOD = 0,
    L1_CS8_LOD    = 1
} AlxLp586x_RegEnum_0x69_Dot_L1_CS8_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x69_Dot_L1_CS15_LOD Dot_L1_CS15_LOD : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS14_LOD Dot_L1_CS14_LOD : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS13_LOD Dot_L1_CS13_LOD : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS12_LOD Dot_L1_CS12_LOD : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS11_LOD Dot_L1_CS11_LOD : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS10_LOD Dot_L1_CS10_LOD : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS9_LOD  Dot_L1_CS9_LOD  : 1;
        AlxLp586x_RegEnum_0x69_Dot_L1_CS8_LOD  Dot_L1_CS8_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x69_Dot_lod4;


//******************************************************************************
// Address: 0x6A, Reset: 0x00 , Name: Dot_lod5
//******************************************************************************
typedef enum
{
    L1_CS17_No_LOD = 0,
    L1_CS17_LOD    = 1
} AlxLp586x_RegEnum_0x6A_Dot_L1_CS17_LOD;
typedef enum
{
    L1_CS16_No_LOD = 0,
    L1_CS16_LOD    = 1
} AlxLp586x_RegEnum_0x6A_Dot_L1_CS16_LOD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x6A_Dot_L1_CS17_LOD Dot_L1_CS17_LOD  : 1;
        AlxLp586x_RegEnum_0x6A_Dot_L1_CS16_LOD Dot_L1_CS16_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x6A_Dot_lod5;


//******************************************************************************
// Address: 0x6B, Reset: 0x00 , Name: Dot_lod6
//******************************************************************************
typedef enum
{
    L2_CS7_No_LOD = 0,
    L2_CS7_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS7_LOD;
typedef enum
{
    L2_CS6_No_LOD = 0,
    L2_CS6_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS6_LOD;
typedef enum
{
    L2_CS5_No_LOD = 0,
    L2_CS5_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS5_LOD;
typedef enum
{
    L2_CS4_No_LOD = 0,
    L2_CS4_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS4_LOD;
typedef enum
{
    L2_CS3_No_LOD = 0,
    L2_CS3_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS3_LOD;
typedef enum
{
    L2_CS2_No_LOD = 0,
    L2_CS2_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS2_LOD;
typedef enum
{
    L2_CS1_No_LOD = 0,
    L2_CS1_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS1_LOD;
typedef enum
{
    L2_CS0_No_LOD = 0,
    L2_CS0_LOD    = 1
} AlxLp586x_RegEnum_0x6B_Dot_L2_CS0_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS7_LOD Dot_L2_CS7_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS6_LOD Dot_L2_CS6_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS5_LOD Dot_L2_CS5_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS4_LOD Dot_L2_CS4_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS3_LOD Dot_L2_CS3_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS2_LOD Dot_L2_CS2_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS1_LOD Dot_L2_CS1_LOD : 1;
        AlxLp586x_RegEnum_0x6B_Dot_L2_CS0_LOD Dot_L2_CS0_LOD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x6B_Dot_lod6;


//******************************************************************************
// Address: 0x6C, Reset: 0x00 , Name: Dot_lod7
//******************************************************************************
typedef enum
{
    L2_CS15_No_LOD = 0,
    L2_CS15_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS15_LOD;
typedef enum
{
    L2_CS14_No_LOD = 0,
    L2_CS14_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS14_LOD;
typedef enum
{
    L2_CS13_No_LOD = 0,
    L2_CS13_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS13_LOD;
typedef enum
{
    L2_CS12_No_LOD = 0,
    L2_CS12_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS12_LOD;
typedef enum
{
    L2_CS11_No_LOD = 0,
    L2_CS11_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS11_LOD;
typedef enum
{
    L2_CS10_No_LOD = 0,
    L2_CS10_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS10_LOD;
typedef enum
{
    L2_CS9_No_LOD = 0,
    L2_CS9_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS9_LOD;
typedef enum
{
    L2_CS8_No_LOD = 0,
    L2_CS8_LOD    = 1
} AlxLp586x_RegEnum_0x6C_Dot_L2_CS8_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS15_LOD Dot_L2_CS15_LOD : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS14_LOD Dot_L2_CS14_LOD : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS13_LOD Dot_L2_CS13_LOD : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS12_LOD Dot_L2_CS12_LOD : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS11_LOD Dot_L2_CS11_LOD : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS10_LOD Dot_L2_CS10_LOD : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS9_LOD  Dot_L2_CS9_LOD  : 1;
        AlxLp586x_RegEnum_0x6C_Dot_L2_CS8_LOD  Dot_L2_CS8_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x6C_Dot_lod7;


//******************************************************************************
// Address: 0x6D, Reset: 0x00 , Name: Dot_lod8
//******************************************************************************
typedef enum
{
    L2_CS17_No_LOD = 0,
    L2_CS17_LOD    = 1
} AlxLp586x_RegEnum_0x6D_Dot_L2_CS17_LOD;
typedef enum
{
    L2_CS16_No_LOD = 0,
    L2_CS16_LOD    = 1
} AlxLp586x_RegEnum_0x6D_Dot_L2_CS16_LOD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x6D_Dot_L2_CS17_LOD Dot_L2_CS17_LOD  : 1;
        AlxLp586x_RegEnum_0x6D_Dot_L2_CS16_LOD Dot_L2_CS16_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x6D_Dot_lod8;


//******************************************************************************
// Address: 0x6E, Reset: 0x00 , Name: Dot_lod9
//******************************************************************************
typedef enum
{
    L3_CS7_No_LOD = 0,
    L3_CS7_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS7_LOD;
typedef enum
{
    L3_CS6_No_LOD = 0,
    L3_CS6_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS6_LOD;
typedef enum
{
    L3_CS5_No_LOD = 0,
    L3_CS5_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS5_LOD;
typedef enum
{
    L3_CS4_No_LOD = 0,
    L3_CS4_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS4_LOD;
typedef enum
{
    L3_CS3_No_LOD = 0,
    L3_CS3_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS3_LOD;
typedef enum
{
    L3_CS2_No_LOD = 0,
    L3_CS2_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS2_LOD;
typedef enum
{
    L3_CS1_No_LOD = 0,
    L3_CS1_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS1_LOD;
typedef enum
{
    L3_CS0_No_LOD = 0,
    L3_CS0_LOD    = 1
} AlxLp586x_RegEnum_0x6E_Dot_L3_CS0_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS7_LOD Dot_L3_CS7_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS6_LOD Dot_L3_CS6_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS5_LOD Dot_L3_CS5_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS4_LOD Dot_L3_CS4_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS3_LOD Dot_L3_CS3_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS2_LOD Dot_L3_CS2_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS1_LOD Dot_L3_CS1_LOD : 1;
        AlxLp586x_RegEnum_0x6E_Dot_L3_CS0_LOD Dot_L3_CS0_LOD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x6E_Dot_lod9;


//******************************************************************************
// Address: 0x6F, Reset: 0x00 , Name: Dot_lod10
//******************************************************************************
typedef enum
{
    L3_CS15_No_LOD = 0,
    L3_CS15_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS15_LOD;
typedef enum
{
    L3_CS14_No_LOD = 0,
    L3_CS14_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS14_LOD;
typedef enum
{
    L3_CS13_No_LOD = 0,
    L3_CS13_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS13_LOD;
typedef enum
{
    L3_CS12_No_LOD = 0,
    L3_CS12_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS12_LOD;
typedef enum
{
    L3_CS11_No_LOD = 0,
    L3_CS11_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS11_LOD;
typedef enum
{
    L3_CS10_No_LOD = 0,
    L3_CS10_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS10_LOD;
typedef enum
{
    L3_CS9_No_LOD = 0,
    L3_CS9_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS9_LOD;
typedef enum
{
    L3_CS8_No_LOD = 0,
    L3_CS8_LOD    = 1
} AlxLp586x_RegEnum_0x6F_Dot_L3_CS8_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS15_LOD Dot_L3_CS15_LOD : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS14_LOD Dot_L3_CS14_LOD : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS13_LOD Dot_L3_CS13_LOD : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS12_LOD Dot_L3_CS12_LOD : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS11_LOD Dot_L3_CS11_LOD : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS10_LOD Dot_L3_CS10_LOD : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS9_LOD  Dot_L3_CS9_LOD  : 1;
        AlxLp586x_RegEnum_0x6F_Dot_L3_CS8_LOD  Dot_L3_CS8_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x6F_Dot_lod10;


//******************************************************************************
// Address: 0x70, Reset: 0x00 , Name: Dot_lod11
//******************************************************************************
typedef enum
{
    L3_CS17_No_LOD = 0,
    L3_CS17_LOD    = 1
} AlxLp586x_RegEnum_0x70_Dot_L3_CS17_LOD;
typedef enum
{
    L3_CS16_No_LOD = 0,
    L3_CS16_LOD    = 1
} AlxLp586x_RegEnum_0x70_Dot_L3_CS16_LOD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x70_Dot_L3_CS17_LOD Dot_L3_CS17_LOD  : 1;
        AlxLp586x_RegEnum_0x70_Dot_L3_CS16_LOD Dot_L3_CS16_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x70_Dot_lod11;


//******************************************************************************
// Address: 0x71, Reset: 0x00 , Name: Dot_lod12
//******************************************************************************
typedef enum
{
    L4_CS7_No_LOD = 0,
    L4_CS7_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS7_LOD;
typedef enum
{
    L4_CS6_No_LOD = 0,
    L4_CS6_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS6_LOD;
typedef enum
{
    L4_CS5_No_LOD = 0,
    L4_CS5_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS5_LOD;
typedef enum
{
    L4_CS4_No_LOD = 0,
    L4_CS4_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS4_LOD;
typedef enum
{
    L4_CS3_No_LOD = 0,
    L4_CS3_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS3_LOD;
typedef enum
{
    L4_CS2_No_LOD = 0,
    L4_CS2_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS2_LOD;
typedef enum
{
    L4_CS1_No_LOD = 0,
    L4_CS1_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS1_LOD;
typedef enum
{
    L4_CS0_No_LOD = 0,
    L4_CS0_LOD    = 1
} AlxLp586x_RegEnum_0x71_Dot_L4_CS0_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x71_Dot_L4_CS7_LOD Dot_L4_CS7_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS6_LOD Dot_L4_CS6_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS5_LOD Dot_L4_CS5_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS4_LOD Dot_L4_CS4_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS3_LOD Dot_L4_CS3_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS2_LOD Dot_L4_CS2_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS1_LOD Dot_L4_CS1_LOD : 1;
        AlxLp586x_RegEnum_0x71_Dot_L4_CS0_LOD Dot_L4_CS0_LOD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x71_Dot_lod12;


//******************************************************************************
// Address: 0x72, Reset: 0x00 , Name: Dot_lod13
//******************************************************************************
typedef enum
{
    L4_CS15_No_LOD = 0,
    L4_CS15_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS15_LOD;
typedef enum
{
    L4_CS14_No_LOD = 0,
    L4_CS14_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS14_LOD;
typedef enum
{
    L4_CS13_No_LOD = 0,
    L4_CS13_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS13_LOD;
typedef enum
{
    L4_CS12_No_LOD = 0,
    L4_CS12_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS12_LOD;
typedef enum
{
    L4_CS11_No_LOD = 0,
    L4_CS11_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS11_LOD;
typedef enum
{
    L4_CS10_No_LOD = 0,
    L4_CS10_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS10_LOD;
typedef enum
{
    L4_CS9_No_LOD = 0,
    L4_CS9_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS9_LOD;
typedef enum
{
    L4_CS8_No_LOD = 0,
    L4_CS8_LOD    = 1
} AlxLp586x_RegEnum_0x72_Dot_L4_CS8_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x72_Dot_L4_CS15_LOD Dot_L4_CS15_LOD : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS14_LOD Dot_L4_CS14_LOD : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS13_LOD Dot_L4_CS13_LOD : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS12_LOD Dot_L4_CS12_LOD : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS11_LOD Dot_L4_CS11_LOD : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS10_LOD Dot_L4_CS10_LOD : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS9_LOD  Dot_L4_CS9_LOD  : 1;
        AlxLp586x_RegEnum_0x72_Dot_L4_CS8_LOD  Dot_L4_CS8_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x72_Dot_lod13;


//******************************************************************************
// Address: 0x73, Reset: 0x00 , Name: Dot_lod14
//******************************************************************************
typedef enum
{
    L4_CS17_No_LOD = 0,
    L4_CS17_LOD    = 1
} AlxLp586x_RegEnum_0x73_Dot_L4_CS17_LOD;
typedef enum
{
    L4_CS16_No_LOD = 0,
    L4_CS16_LOD    = 1
} AlxLp586x_RegEnum_0x73_Dot_L4_CS16_LOD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x73_Dot_L4_CS17_LOD Dot_L4_CS17_LOD  : 1;
        AlxLp586x_RegEnum_0x73_Dot_L4_CS16_LOD Dot_L4_CS16_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x73_Dot_lod14;


//******************************************************************************
// Address: 0x74, Reset: 0x00 , Name: Dot_lod15
//******************************************************************************
typedef enum
{
    L5_CS7_No_LOD = 0,
    L5_CS7_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS7_LOD;
typedef enum
{
    L5_CS6_No_LOD = 0,
    L5_CS6_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS6_LOD;
typedef enum
{
    L5_CS5_No_LOD = 0,
    L5_CS5_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS5_LOD;
typedef enum
{
    L5_CS4_No_LOD = 0,
    L5_CS4_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS4_LOD;
typedef enum
{
    L5_CS3_No_LOD = 0,
    L5_CS3_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS3_LOD;
typedef enum
{
    L5_CS2_No_LOD = 0,
    L5_CS2_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS2_LOD;
typedef enum
{
    L5_CS1_No_LOD = 0,
    L5_CS1_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS1_LOD;
typedef enum
{
    L5_CS0_No_LOD = 0,
    L5_CS0_LOD    = 1
} AlxLp586x_RegEnum_0x74_Dot_L5_CS0_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x74_Dot_L5_CS7_LOD Dot_L5_CS7_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS6_LOD Dot_L5_CS6_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS5_LOD Dot_L5_CS5_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS4_LOD Dot_L5_CS4_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS3_LOD Dot_L5_CS3_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS2_LOD Dot_L5_CS2_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS1_LOD Dot_L5_CS1_LOD : 1;
        AlxLp586x_RegEnum_0x74_Dot_L5_CS0_LOD Dot_L5_CS0_LOD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x74_Dot_lod15;


//******************************************************************************
// Address: 0x75, Reset: 0x00 , Name: Dot_lod16
//******************************************************************************
typedef enum
{
    L5_CS15_No_LOD = 0,
    L5_CS15_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS15_LOD;
typedef enum
{
    L5_CS14_No_LOD = 0,
    L5_CS14_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS14_LOD;
typedef enum
{
    L5_CS13_No_LOD = 0,
    L5_CS13_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS13_LOD;
typedef enum
{
    L5_CS12_No_LOD = 0,
    L5_CS12_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS12_LOD;
typedef enum
{
    L5_CS11_No_LOD = 0,
    L5_CS11_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS11_LOD;
typedef enum
{
    L5_CS10_No_LOD = 0,
    L5_CS10_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS10_LOD;
typedef enum
{
    L5_CS9_No_LOD = 0,
    L5_CS9_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS9_LOD;
typedef enum
{
    L5_CS8_No_LOD = 0,
    L5_CS8_LOD    = 1
} AlxLp586x_RegEnum_0x75_Dot_L5_CS8_LOD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x75_Dot_L5_CS15_LOD Dot_L5_CS15_LOD : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS14_LOD Dot_L5_CS14_LOD : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS13_LOD Dot_L5_CS13_LOD : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS12_LOD Dot_L5_CS12_LOD : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS11_LOD Dot_L5_CS11_LOD : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS10_LOD Dot_L5_CS10_LOD : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS9_LOD  Dot_L5_CS9_LOD  : 1;
        AlxLp586x_RegEnum_0x75_Dot_L5_CS8_LOD  Dot_L5_CS8_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x75_Dot_lod16;


//******************************************************************************
// Address: 0x76, Reset: 0x00 , Name: Dot_lod17
//******************************************************************************
typedef enum
{
    L5_CS17_No_LOD = 0,
    L5_CS17_LOD    = 1
} AlxLp586x_RegEnum_0x76_Dot_L5_CS17_LOD;
typedef enum
{
    L5_CS16_No_LOD = 0,
    L5_CS16_LOD    = 1
} AlxLp586x_RegEnum_0x76_Dot_L5_CS16_LOD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x76_Dot_L5_CS17_LOD Dot_L5_CS17_LOD  : 1;
        AlxLp586x_RegEnum_0x76_Dot_L5_CS16_LOD Dot_L5_CS16_LOD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x76_Dot_lod17;


//******************************************************************************
// Address: 0x86, Reset: 0x00 , Name: Dot_lsd0
//******************************************************************************
typedef enum
{
    L0_CS7_No_LSD = 0,
    L0_CS7_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS7_LSD;
typedef enum
{
    L0_CS6_No_LSD = 0,
    L0_CS6_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS6_LSD;
typedef enum
{
    L0_CS5_No_LSD = 0,
    L0_CS5_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS5_LSD;
typedef enum
{
    L0_CS4_No_LSD = 0,
    L0_CS4_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS4_LSD;
typedef enum
{
    L0_CS3_No_LSD = 0,
    L0_CS3_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS3_LSD;
typedef enum
{
    L0_CS2_No_LSD = 0,
    L0_CS2_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS2_LSD;
typedef enum
{
    L0_CS1_No_LSD = 0,
    L0_CS1_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS1_LSD;
typedef enum
{
    L0_CS0_No_LSD = 0,
    L0_CS0_LSD    = 1
} AlxLp586x_RegEnum_0x86_Dot_L0_CS0_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x86_Dot_L0_CS7_LSD Dot_L0_CS7_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS6_LSD Dot_L0_CS6_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS5_LSD Dot_L0_CS5_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS4_LSD Dot_L0_CS4_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS3_LSD Dot_L0_CS3_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS2_LSD Dot_L0_CS2_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS1_LSD Dot_L0_CS1_LSD : 1;
        AlxLp586x_RegEnum_0x86_Dot_L0_CS0_LSD Dot_L0_CS0_LSD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x86_Dot_lsd0;


//******************************************************************************
// Address: 0x87, Reset: 0x00 , Name: Dot_lsd1
//******************************************************************************
typedef enum
{
    L0_CS15_No_LSD = 0,
    L0_CS15_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS15_LSD;
typedef enum
{
    L0_CS14_No_LSD = 0,
    L0_CS14_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS14_LSD;
typedef enum
{
    L0_CS13_No_LSD = 0,
    L0_CS13_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS13_LSD;
typedef enum
{
    L0_CS12_No_LSD = 0,
    L0_CS12_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS12_LSD;
typedef enum
{
    L0_CS11_No_LSD = 0,
    L0_CS11_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS11_LSD;
typedef enum
{
    L0_CS10_No_LSD = 0,
    L0_CS10_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS10_LSD;
typedef enum
{
    L0_CS9_No_LSD = 0,
    L0_CS9_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS9_LSD;
typedef enum
{
    L0_CS8_No_LSD = 0,
    L0_CS8_LSD    = 1
} AlxLp586x_RegEnum_0x87_Dot_L0_CS8_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x87_Dot_L0_CS15_LSD Dot_L0_CS15_LSD : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS14_LSD Dot_L0_CS14_LSD : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS13_LSD Dot_L0_CS13_LSD : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS12_LSD Dot_L0_CS12_LSD : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS11_LSD Dot_L0_CS11_LSD : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS10_LSD Dot_L0_CS10_LSD : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS9_LSD  Dot_L0_CS9_LSD  : 1;
        AlxLp586x_RegEnum_0x87_Dot_L0_CS8_LSD  Dot_L0_CS8_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x87_Dot_lsd1;


//******************************************************************************
// Address: 0x88, Reset: 0x00 , Name: Dot_lsd2
//******************************************************************************
typedef enum
{
    L0_CS17_No_LSD = 0,
    L0_CS17_LSD    = 1
} AlxLp586x_RegEnum_0x88_Dot_L0_CS17_LSD;
typedef enum
{
    L0_CS16_No_LSD = 0,
    L0_CS16_LSD    = 1
} AlxLp586x_RegEnum_0x88_Dot_L0_CS16_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x88_Dot_L0_CS17_LSD Dot_L0_CS17_LSD  : 1;
        AlxLp586x_RegEnum_0x88_Dot_L0_CS16_LSD Dot_L0_CS16_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x88_Dot_lsd2;


//******************************************************************************
// Address: 0x89, Reset: 0x00 , Name: Dot_lsd3
//******************************************************************************
typedef enum
{
    L1_CS7_No_LSD = 0,
    L1_CS7_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS7_LSD;
typedef enum
{
    L1_CS6_No_LSD = 0,
    L1_CS6_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS6_LSD;
typedef enum
{
    L1_CS5_No_LSD = 0,
    L1_CS5_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS5_LSD;
typedef enum
{
    L1_CS4_No_LSD = 0,
    L1_CS4_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS4_LSD;
typedef enum
{
    L1_CS3_No_LSD = 0,
    L1_CS3_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS3_LSD;
typedef enum
{
    L1_CS2_No_LSD = 0,
    L1_CS2_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS2_LSD;
typedef enum
{
    L1_CS1_No_LSD = 0,
    L1_CS1_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS1_LSD;
typedef enum
{
    L1_CS0_No_LSD = 0,
    L1_CS0_LSD    = 1
} AlxLp586x_RegEnum_0x89_Dot_L1_CS0_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x89_Dot_L1_CS7_LSD Dot_L1_CS7_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS6_LSD Dot_L1_CS6_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS5_LSD Dot_L1_CS5_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS4_LSD Dot_L1_CS4_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS3_LSD Dot_L1_CS3_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS2_LSD Dot_L1_CS2_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS1_LSD Dot_L1_CS1_LSD : 1;
        AlxLp586x_RegEnum_0x89_Dot_L1_CS0_LSD Dot_L1_CS0_LSD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x89_Dot_lsd3;


//******************************************************************************
// Address: 0x8A, Reset: 0x00 , Name: Dot_lsd4
//******************************************************************************
typedef enum
{
    L1_CS15_No_LSD = 0,
    L1_CS15_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS15_LSD;
typedef enum
{
    L1_CS14_No_LSD = 0,
    L1_CS14_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS14_LSD;
typedef enum
{
    L1_CS13_No_LSD = 0,
    L1_CS13_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS13_LSD;
typedef enum
{
    L1_CS12_No_LSD = 0,
    L1_CS12_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS12_LSD;
typedef enum
{
    L1_CS11_No_LSD = 0,
    L1_CS11_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS11_LSD;
typedef enum
{
    L1_CS10_No_LSD = 0,
    L1_CS10_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS10_LSD;
typedef enum
{
    L1_CS9_No_LSD = 0,
    L1_CS9_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS9_LSD;
typedef enum
{
    L1_CS8_No_LSD = 0,
    L1_CS8_LSD    = 1
} AlxLp586x_RegEnum_0x8A_Dot_L1_CS8_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS15_LSD Dot_L1_CS15_LSD : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS14_LSD Dot_L1_CS14_LSD : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS13_LSD Dot_L1_CS13_LSD : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS12_LSD Dot_L1_CS12_LSD : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS11_LSD Dot_L1_CS11_LSD : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS10_LSD Dot_L1_CS10_LSD : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS9_LSD  Dot_L1_CS9_LSD  : 1;
        AlxLp586x_RegEnum_0x8A_Dot_L1_CS8_LSD  Dot_L1_CS8_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x8A_Dot_lsd4;


//******************************************************************************
// Address: 0x8B, Reset: 0x00 , Name: Dot_lsd5
//******************************************************************************
typedef enum
{
    L1_CS17_No_LSD = 0,
    L1_CS17_LSD    = 1
} AlxLp586x_RegEnum_0x8B_Dot_L1_CS17_LSD;
typedef enum
{
    L1_CS16_No_LSD = 0,
    L1_CS16_LSD    = 1
} AlxLp586x_RegEnum_0x8B_Dot_L1_CS16_LSD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x8B_Dot_L1_CS17_LSD Dot_L1_CS17_LSD  : 1;
        AlxLp586x_RegEnum_0x8B_Dot_L1_CS16_LSD Dot_L1_CS16_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x8B_Dot_lsd5;


//******************************************************************************
// Address: 0x8C, Reset: 0x00 , Name: Dot_lsd6
//******************************************************************************
typedef enum
{
    L2_CS7_No_LSD = 0,
    L2_CS7_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS7_LSD;
typedef enum
{
    L2_CS6_No_LSD = 0,
    L2_CS6_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS6_LSD;
typedef enum
{
    L2_CS5_No_LSD = 0,
    L2_CS5_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS5_LSD;
typedef enum
{
    L2_CS4_No_LSD = 0,
    L2_CS4_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS4_LSD;
typedef enum
{
    L2_CS3_No_LSD = 0,
    L2_CS3_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS3_LSD;
typedef enum
{
    L2_CS2_No_LSD = 0,
    L2_CS2_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS2_LSD;
typedef enum
{
    L2_CS1_No_LSD = 0,
    L2_CS1_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS1_LSD;
typedef enum
{
    L2_CS0_No_LSD = 0,
    L2_CS0_LSD    = 1
} AlxLp586x_RegEnum_0x8C_Dot_L2_CS0_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS7_LSD Dot_L2_CS7_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS6_LSD Dot_L2_CS6_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS5_LSD Dot_L2_CS5_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS4_LSD Dot_L2_CS4_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS3_LSD Dot_L2_CS3_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS2_LSD Dot_L2_CS2_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS1_LSD Dot_L2_CS1_LSD : 1;
        AlxLp586x_RegEnum_0x8C_Dot_L2_CS0_LSD Dot_L2_CS0_LSD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x8C_Dot_lsd6;


//******************************************************************************
// Address: 0x8D, Reset: 0x00 , Name: Dot_lsd7
//******************************************************************************
typedef enum
{
    L2_CS15_No_LSD = 0,
    L2_CS15_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS15_LSD;
typedef enum
{
    L2_CS14_No_LSD = 0,
    L2_CS14_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS14_LSD;
typedef enum
{
    L2_CS13_No_LSD = 0,
    L2_CS13_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS13_LSD;
typedef enum
{
    L2_CS12_No_LSD = 0,
    L2_CS12_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS12_LSD;
typedef enum
{
    L2_CS11_No_LSD = 0,
    L2_CS11_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS11_LSD;
typedef enum
{
    L2_CS10_No_LSD = 0,
    L2_CS10_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS10_LSD;
typedef enum
{
    L2_CS9_No_LSD = 0,
    L2_CS9_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS9_LSD;
typedef enum
{
    L2_CS8_No_LSD = 0,
    L2_CS8_LSD    = 1
} AlxLp586x_RegEnum_0x8D_Dot_L2_CS8_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS15_LSD Dot_L2_CS15_LSD : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS14_LSD Dot_L2_CS14_LSD : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS13_LSD Dot_L2_CS13_LSD : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS12_LSD Dot_L2_CS12_LSD : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS11_LSD Dot_L2_CS11_LSD : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS10_LSD Dot_L2_CS10_LSD : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS9_LSD  Dot_L2_CS9_LSD  : 1;
        AlxLp586x_RegEnum_0x8D_Dot_L2_CS8_LSD  Dot_L2_CS8_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x8D_Dot_lsd7;


//******************************************************************************
// Address: 0x8E, Reset: 0x00 , Name: Dot_lsd8
//******************************************************************************
typedef enum
{
    L2_CS17_No_LSD = 0,
    L2_CS17_LSD    = 1
} AlxLp586x_RegEnum_0x8E_Dot_L2_CS17_LSD;
typedef enum
{
    L2_CS16_No_LSD = 0,
    L2_CS16_LSD    = 1
} AlxLp586x_RegEnum_0x8E_Dot_L2_CS16_LSD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x8E_Dot_L2_CS17_LSD Dot_L2_CS17_LSD  : 1;
        AlxLp586x_RegEnum_0x8E_Dot_L2_CS16_LSD Dot_L2_CS16_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x8E_Dot_lsd8;


//******************************************************************************
// Address: 0x8F, Reset: 0x00 , Name: Dot_lsd9
//******************************************************************************
typedef enum
{
    L3_CS7_No_LSD = 0,
    L3_CS7_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS7_LSD;
typedef enum
{
    L3_CS6_No_LSD = 0,
    L3_CS6_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS6_LSD;
typedef enum
{
    L3_CS5_No_LSD = 0,
    L3_CS5_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS5_LSD;
typedef enum
{
    L3_CS4_No_LSD = 0,
    L3_CS4_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS4_LSD;
typedef enum
{
    L3_CS3_No_LSD = 0,
    L3_CS3_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS3_LSD;
typedef enum
{
    L3_CS2_No_LSD = 0,
    L3_CS2_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS2_LSD;
typedef enum
{
    L3_CS1_No_LSD = 0,
    L3_CS1_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS1_LSD;
typedef enum
{
    L3_CS0_No_LSD = 0,
    L3_CS0_LSD    = 1
} AlxLp586x_RegEnum_0x8F_Dot_L3_CS0_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS7_LSD Dot_L3_CS7_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS6_LSD Dot_L3_CS6_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS5_LSD Dot_L3_CS5_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS4_LSD Dot_L3_CS4_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS3_LSD Dot_L3_CS3_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS2_LSD Dot_L3_CS2_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS1_LSD Dot_L3_CS1_LSD : 1;
        AlxLp586x_RegEnum_0x8F_Dot_L3_CS0_LSD Dot_L3_CS0_LSD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x8F_Dot_lsd9;


//******************************************************************************
// Address: 0x90, Reset: 0x00 , Name: Dot_lsd10
//******************************************************************************
typedef enum
{
    L3_CS15_No_LSD = 0,
    L3_CS15_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS15_LSD;
typedef enum
{
    L3_CS14_No_LSD = 0,
    L3_CS14_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS14_LSD;
typedef enum
{
    L3_CS13_No_LSD = 0,
    L3_CS13_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS13_LSD;
typedef enum
{
    L3_CS12_No_LSD = 0,
    L3_CS12_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS12_LSD;
typedef enum
{
    L3_CS11_No_LSD = 0,
    L3_CS11_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS11_LSD;
typedef enum
{
    L3_CS10_No_LSD = 0,
    L3_CS10_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS10_LSD;
typedef enum
{
    L3_CS9_No_LSD = 0,
    L3_CS9_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS9_LSD;
typedef enum
{
    L3_CS8_No_LSD = 0,
    L3_CS8_LSD    = 1
} AlxLp586x_RegEnum_0x90_Dot_L3_CS8_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x90_Dot_L3_CS15_LSD Dot_L3_CS15_LSD : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS14_LSD Dot_L3_CS14_LSD : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS13_LSD Dot_L3_CS13_LSD : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS12_LSD Dot_L3_CS12_LSD : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS11_LSD Dot_L3_CS11_LSD : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS10_LSD Dot_L3_CS10_LSD : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS9_LSD  Dot_L3_CS9_LSD  : 1;
        AlxLp586x_RegEnum_0x90_Dot_L3_CS8_LSD  Dot_L3_CS8_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x90_Dot_lsd10;


//******************************************************************************
// Address: 0x91, Reset: 0x00 , Name: Dot_lsd11
//******************************************************************************
typedef enum
{
    L3_CS17_No_LSD = 0,
    L3_CS17_LSD    = 1
} AlxLp586x_RegEnum_0x91_Dot_L3_CS17_LSD;
typedef enum
{
    L3_CS16_No_LSD = 0,
    L3_CS16_LSD    = 1
} AlxLp586x_RegEnum_0x91_Dot_L3_CS16_LSD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x91_Dot_L3_CS17_LSD Dot_L3_CS17_LSD  : 1;
        AlxLp586x_RegEnum_0x91_Dot_L3_CS16_LSD Dot_L3_CS16_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x91_Dot_lsd11;


//******************************************************************************
// Address: 0x92, Reset: 0x00 , Name: Dot_lsd12
//******************************************************************************
typedef enum
{
    L4_CS7_No_LSD = 0,
    L4_CS7_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS7_LSD;
typedef enum
{
    L4_CS6_No_LSD = 0,
    L4_CS6_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS6_LSD;
typedef enum
{
    L4_CS5_No_LSD = 0,
    L4_CS5_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS5_LSD;
typedef enum
{
    L4_CS4_No_LSD = 0,
    L4_CS4_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS4_LSD;
typedef enum
{
    L4_CS3_No_LSD = 0,
    L4_CS3_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS3_LSD;
typedef enum
{
    L4_CS2_No_LSD = 0,
    L4_CS2_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS2_LSD;
typedef enum
{
    L4_CS1_No_LSD = 0,
    L4_CS1_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS1_LSD;
typedef enum
{
    L4_CS0_No_LSD = 0,
    L4_CS0_LSD    = 1
} AlxLp586x_RegEnum_0x92_Dot_L4_CS0_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x92_Dot_L4_CS7_LSD Dot_L4_CS7_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS6_LSD Dot_L4_CS6_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS5_LSD Dot_L4_CS5_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS4_LSD Dot_L4_CS4_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS3_LSD Dot_L4_CS3_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS2_LSD Dot_L4_CS2_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS1_LSD Dot_L4_CS1_LSD : 1;
        AlxLp586x_RegEnum_0x92_Dot_L4_CS0_LSD Dot_L4_CS0_LSD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x92_Dot_lsd12;


//******************************************************************************
// Address: 0x93, Reset: 0x00 , Name: Dot_lsd13
//******************************************************************************
typedef enum
{
    L4_CS15_No_LSD = 0,
    L4_CS15_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS15_LSD;
typedef enum
{
    L4_CS14_No_LSD = 0,
    L4_CS14_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS14_LSD;
typedef enum
{
    L4_CS13_No_LSD = 0,
    L4_CS13_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS13_LSD;
typedef enum
{
    L4_CS12_No_LSD = 0,
    L4_CS12_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS12_LSD;
typedef enum
{
    L4_CS11_No_LSD = 0,
    L4_CS11_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS11_LSD;
typedef enum
{
    L4_CS10_No_LSD = 0,
    L4_CS10_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS10_LSD;
typedef enum
{
    L4_CS9_No_LSD = 0,
    L4_CS9_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS9_LSD;
typedef enum
{
    L4_CS8_No_LSD = 0,
    L4_CS8_LSD    = 1
} AlxLp586x_RegEnum_0x93_Dot_L4_CS8_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x93_Dot_L4_CS15_LSD Dot_L4_CS15_LSD : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS14_LSD Dot_L4_CS14_LSD : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS13_LSD Dot_L4_CS13_LSD : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS12_LSD Dot_L4_CS12_LSD : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS11_LSD Dot_L4_CS11_LSD : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS10_LSD Dot_L4_CS10_LSD : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS9_LSD  Dot_L4_CS9_LSD  : 1;
        AlxLp586x_RegEnum_0x93_Dot_L4_CS8_LSD  Dot_L4_CS8_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x93_Dot_lsd13;


//******************************************************************************
// Address: 0x94, Reset: 0x00 , Name: Dot_lsd14
//******************************************************************************
typedef enum
{
    L4_CS17_No_LSD = 0,
    L4_CS17_LSD    = 1
} AlxLp586x_RegEnum_0x94_Dot_L4_CS17_LSD;
typedef enum
{
    L4_CS16_No_LSD = 0,
    L4_CS16_LSD    = 1
} AlxLp586x_RegEnum_0x94_Dot_L4_CS16_LSD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x94_Dot_L4_CS17_LSD Dot_L4_CS17_LSD  : 1;
        AlxLp586x_RegEnum_0x94_Dot_L4_CS16_LSD Dot_L4_CS16_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x94_Dot_lsd14;


//******************************************************************************
// Address: 0x95, Reset: 0x00 , Name: Dot_lsd15
//******************************************************************************
typedef enum
{
    L5_CS7_No_LSD = 0,
    L5_CS7_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS7_LSD;
typedef enum
{
    L5_CS6_No_LSD = 0,
    L5_CS6_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS6_LSD;
typedef enum
{
    L5_CS5_No_LSD = 0,
    L5_CS5_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS5_LSD;
typedef enum
{
    L5_CS4_No_LSD = 0,
    L5_CS4_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS4_LSD;
typedef enum
{
    L5_CS3_No_LSD = 0,
    L5_CS3_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS3_LSD;
typedef enum
{
    L5_CS2_No_LSD = 0,
    L5_CS2_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS2_LSD;
typedef enum
{
    L5_CS1_No_LSD = 0,
    L5_CS1_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS1_LSD;
typedef enum
{
    L5_CS0_No_LSD = 0,
    L5_CS0_LSD    = 1
} AlxLp586x_RegEnum_0x95_Dot_L5_CS0_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x95_Dot_L5_CS7_LSD Dot_L5_CS7_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS6_LSD Dot_L5_CS6_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS5_LSD Dot_L5_CS5_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS4_LSD Dot_L5_CS4_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS3_LSD Dot_L5_CS3_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS2_LSD Dot_L5_CS2_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS1_LSD Dot_L5_CS1_LSD : 1;
        AlxLp586x_RegEnum_0x95_Dot_L5_CS0_LSD Dot_L5_CS0_LSD : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x95_Dot_lsd15;


//******************************************************************************
// Address: 0x96, Reset: 0x00 , Name: Dot_lsd16
//******************************************************************************
typedef enum
{
    L5_CS15_No_LSD = 0,
    L5_CS15_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS15_LSD;
typedef enum
{
    L5_CS14_No_LSD = 0,
    L5_CS14_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS14_LSD;
typedef enum
{
    L5_CS13_No_LSD = 0,
    L5_CS13_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS13_LSD;
typedef enum
{
    L5_CS12_No_LSD = 0,
    L5_CS12_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS12_LSD;
typedef enum
{
    L5_CS11_No_LSD = 0,
    L5_CS11_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS11_LSD;
typedef enum
{
    L5_CS10_No_LSD = 0,
    L5_CS10_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS10_LSD;
typedef enum
{
    L5_CS9_No_LSD = 0,
    L5_CS9_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS9_LSD;
typedef enum
{
    L5_CS8_No_LSD = 0,
    L5_CS8_LSD    = 1
} AlxLp586x_RegEnum_0x96_Dot_L5_CS8_LSD;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0x96_Dot_L5_CS15_LSD Dot_L5_CS15_LSD : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS14_LSD Dot_L5_CS14_LSD : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS13_LSD Dot_L5_CS13_LSD : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS12_LSD Dot_L5_CS12_LSD : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS11_LSD Dot_L5_CS11_LSD : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS10_LSD Dot_L5_CS10_LSD : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS9_LSD  Dot_L5_CS9_LSD  : 1;
        AlxLp586x_RegEnum_0x96_Dot_L5_CS8_LSD  Dot_L5_CS8_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x96_Dot_lsd16;


//******************************************************************************
// Address: 0x97, Reset: 0x00 , Name: Dot_lsd17
//******************************************************************************
typedef enum
{
    L5_CS17_No_LSD = 0,
    L5_CS17_LSD    = 1
} AlxLp586x_RegEnum_0x97_Dot_L5_CS17_LSD;
typedef enum
{
    L5_CS16_No_LSD = 0,
    L5_CS16_LSD    = 1
} AlxLp586x_RegEnum_0x97_Dot_L5_CS16_LSD;
typedef union
{
    struct __attribute__((packed))
    {
	    uint8_t unused_0_5                                      : 6;
        AlxLp586x_RegEnum_0x97_Dot_L5_CS17_LSD Dot_L5_CS17_LSD  : 1;
        AlxLp586x_RegEnum_0x97_Dot_L5_CS16_LSD Dot_L5_CS16_LSD  : 1;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0x97_Dot_lsd17;


//******************************************************************************
// Address: 0xA7, Reset: 0x00 , Name: LOD_clear
//******************************************************************************
typedef enum
{
	LOD_Clear_None = 0x0, // Leave all LOD flags unchanged
	LOD_Clear_All = 0xF   // Clear all 4 open-fault bits at once
} AlxLp586x_RegEnum_0xA7_LOD_Clear;
typedef union
{
	struct __attribute__((packed))
	{
		AlxLp586x_RegEnum_0xA7_LOD_Clear LOD_clear  : 4;
		uint8_t unused_4_7                          : 4;
	};
	uint8_t raw;
} AlxLp586x_RegVal_0xA7_LOD_clear;


//******************************************************************************
// Address: 0xA8, Reset: 0x00 , Name: LSD_clear
//******************************************************************************
typedef enum
{
    LSD_Clear_None = 0x0, // Leave all LSD flags unchanged
    LSD_Clear_All  = 0xF  // Clear all 4 short-fault bits at once
} AlxLp586x_RegEnum_0xA8_LSD_Clear;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0xA8_LSD_Clear LSD_clear : 4;
        uint8_t                         unused_4_7 : 4;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0xA8_LSD_clear;


//******************************************************************************
// Address: 0xA9, Reset: 0x00 , Name: Reset
//******************************************************************************
typedef enum
{
    Reset_None = 0x00,
    Reset_Exec = 0xFF     // Write 0xFF to trigger a software reset
} AlxLp586x_RegEnum_0xA9_Reset;
typedef union
{
    struct __attribute__((packed))
    {
        AlxLp586x_RegEnum_0xA9_Reset Reset : 8;
    };
    uint8_t raw;
} AlxLp586x_RegVal_0xA9_Reset;


//******************************************************************************
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x00_Chip_En val;
} AlxLp586x_Reg_0x00_Chip_En;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x01_Dev_initial val;
} AlxLp586x_Reg_0x01_Dev_initial;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x02_Dev_config1 val;
} AlxLp586x_Reg_0x02_Dev_config1;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x03_Dev_config2 val;
} AlxLp586x_Reg_0x03_Dev_config2;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x04_Dev_config3 val;
} AlxLp586x_Reg_0x04_Dev_config3;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x05_Global_bri val;
} AlxLp586x_Reg_0x05_Global_bri;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    AlxLp586x_RegVal_0x64_Fault_state    val;
} AlxLp586x_Reg_0x64_Fault_state;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    AlxLp586x_RegVal_0xA7_LOD_clear      val;
} AlxLp586x_Reg_0xA7_LOD_clear;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    AlxLp586x_RegVal_0xA8_LSD_clear      val;
} AlxLp586x_Reg_0xA8_LSD_clear;
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    AlxLp586x_RegVal_0xA9_Reset          val;
} AlxLp586x_Reg_0xA9_Reset;


//------------------------------------------------------------------------------
// 0x06–0x08: Group brightness registers (Group0_bri…Group2_bri)
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	union {
		AlxLp586x_RegVal_0x06_Group0_bri  g0;
		AlxLp586x_RegVal_0x07_Group1_bri  g1;
		AlxLp586x_RegVal_0x08_Group2_bri  g2;
		uint8_t raw; // fallback so we can memset the whole byte
	} val;
} AlxLp586x_Reg_GroupBri;


//------------------------------------------------------------------------------
// 0x09–0x0B: 3-groups CC current-set registers
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr;
	uint8_t len;
	union
	{
		AlxLp586x_RegVal_0x09_R_current_set_CC_Group1  g1;
		AlxLp586x_RegVal_0x0A_G_current_set_CC_Group2  g2;
		AlxLp586x_RegVal_0x0B_B_current_set_CC_Group3  g3;
		uint8_t raw; // fallback so we can memset the whole byte
	} val;
} AlxLp586x_Reg_CurrentSetCC;


//------------------------------------------------------------------------------
// Dot group-select registers (0x0C…0x29)
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    union
    {
    AlxLp586x_RegVal_0x0C_Dot_grp_sel0   rg0;   // 0Ch:  L0-CS3…CS0
    AlxLp586x_RegVal_0x0D_Dot_grp_sel1   rg1;   // 0Dh:  L0-CS7…CS4
    AlxLp586x_RegVal_0x0E_Dot_grp_sel2   rg2;   // 0Eh:  L0-CS11…CS8
    AlxLp586x_RegVal_0x0F_Dot_grp_sel3   rg3;   // 0Fh:  L0-CS15…CS12
    AlxLp586x_RegVal_0x10_Dot_grp_sel4   rg4;   // 10h:  L0-CS17/16
    AlxLp586x_RegVal_0x11_Dot_grp_sel5   rg5;   // 11h:  L1-CS3…CS0
    AlxLp586x_RegVal_0x12_Dot_grp_sel6   rg6;   // 12h:  L1-CS7…CS4
    AlxLp586x_RegVal_0x13_Dot_grp_sel7   rg7;   // 13h:  L1-CS11…CS8
    AlxLp586x_RegVal_0x14_Dot_grp_sel8   rg8;   // 14h:  L1-CS15…CS12
    AlxLp586x_RegVal_0x15_Dot_grp_sel9   rg9;   // 15h:  L1-CS17/16
    AlxLp586x_RegVal_0x16_Dot_grp_sel10  rg10;  // 16h:  L2-CS3…CS0
    AlxLp586x_RegVal_0x17_Dot_grp_sel11  rg11;  // 17h:  L2-CS7…CS4
    AlxLp586x_RegVal_0x18_Dot_grp_sel12  rg12;  // 18h:  L2-CS11…CS8
    AlxLp586x_RegVal_0x19_Dot_grp_sel13  rg13;  // 19h:  L2-CS15…CS12
    AlxLp586x_RegVal_0x1A_Dot_grp_sel14  rg14;  // 1Ah:  L2-CS17/16
    AlxLp586x_RegVal_0x1B_Dot_grp_sel15  rg15;  // 1Bh:  L3-CS3…CS0
    AlxLp586x_RegVal_0x1C_Dot_grp_sel16  rg16;  // 1Ch:  L3-CS7…CS4
    AlxLp586x_RegVal_0x1D_Dot_grp_sel17  rg17;  // 1Dh:  L3-CS11…CS8
    AlxLp586x_RegVal_0x1E_Dot_grp_sel18  rg18;  // 1Eh:  L3-CS15…CS12
    AlxLp586x_RegVal_0x1F_Dot_grp_sel19  rg19;  // 1Fh:  L3-CS17/16
    AlxLp586x_RegVal_0x20_Dot_grp_sel20  rg20;  // 20h:  L4-CS3…CS0
    AlxLp586x_RegVal_0x21_Dot_grp_sel21  rg21;  // 21h:  L4-CS7…CS4
    AlxLp586x_RegVal_0x22_Dot_grp_sel22  rg22;  // 22h:  L4-CS11…CS8
    AlxLp586x_RegVal_0x23_Dot_grp_sel23  rg23;  // 23h:  L4-CS15…CS12
    AlxLp586x_RegVal_0x24_Dot_grp_sel24  rg24;  // 24h:  L4-CS17/16
    AlxLp586x_RegVal_0x25_Dot_grp_sel25  rg25;  // 25h:  L5-CS3…CS0
    AlxLp586x_RegVal_0x26_Dot_grp_sel26  rg26;  // 26h:  L5-CS7…CS4
    AlxLp586x_RegVal_0x27_Dot_grp_sel27  rg27;  // 27h:  L5-CS11…CS8
    AlxLp586x_RegVal_0x28_Dot_grp_sel28  rg28;  // 28h:  L5-CS15…CS12
    AlxLp586x_RegVal_0x29_Dot_grp_sel29  rg29;  // 29h:  L5-CS17/16
	uint8_t raw; // fallback so we can memset the whole byte
    } val;
} AlxLp586x_Reg_DotGrpSel;


//------------------------------------------------------------------------------
// Dot on/off registers (0x43…0x54)
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;

    // each entry is one of the union types you already generated:
    //   0  → AlxLp586x_RegVal_0x43_Dot_onoff0
    //   1  → AlxLp586x_RegVal_0x44_Dot_onoff1
    //   …
    //  17  → AlxLp586x_RegVal_0x54_Dot_onoff17
    union
    {
        AlxLp586x_RegVal_0x43_Dot_onoff0   onoff0;
        AlxLp586x_RegVal_0x44_Dot_onoff1   onoff1;
        AlxLp586x_RegVal_0x45_Dot_onoff2   onoff2;
        AlxLp586x_RegVal_0x46_Dot_onoff3   onoff3;
        AlxLp586x_RegVal_0x47_Dot_onoff4   onoff4;
        AlxLp586x_RegVal_0x48_Dot_onoff5   onoff5;
        AlxLp586x_RegVal_0x49_Dot_onoff6   onoff6;
        AlxLp586x_RegVal_0x4A_Dot_onoff7   onoff7;
        AlxLp586x_RegVal_0x4B_Dot_onoff8   onoff8;
        AlxLp586x_RegVal_0x4C_Dot_onoff9   onoff9;
        AlxLp586x_RegVal_0x4D_Dot_onoff10  onoff10;
        AlxLp586x_RegVal_0x4E_Dot_onoff11  onoff11;
        AlxLp586x_RegVal_0x4F_Dot_onoff12  onoff12;
        AlxLp586x_RegVal_0x50_Dot_onoff13  onoff13;
        AlxLp586x_RegVal_0x51_Dot_onoff14  onoff14;
        AlxLp586x_RegVal_0x52_Dot_onoff15  onoff15;
        AlxLp586x_RegVal_0x53_Dot_onoff16  onoff16;
        AlxLp586x_RegVal_0x54_Dot_onoff17  onoff17;
	    uint8_t raw; // fallback so we can memset the whole byte
    } val;
} AlxLp586x_Reg_DotOnOff;


//------------------------------------------------------------------------------
// 0x65–0x76: Dot LOD status registers (L0…L5, CS0…CS7)
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    union
    {
        AlxLp586x_RegVal_0x65_Dot_lod0   lod0;
        AlxLp586x_RegVal_0x66_Dot_lod1   lod1;
        AlxLp586x_RegVal_0x67_Dot_lod2   lod2;
        AlxLp586x_RegVal_0x68_Dot_lod3   lod3;
        AlxLp586x_RegVal_0x69_Dot_lod4   lod4;
        AlxLp586x_RegVal_0x6A_Dot_lod5   lod5;
        AlxLp586x_RegVal_0x6B_Dot_lod6   lod6;
        AlxLp586x_RegVal_0x6C_Dot_lod7   lod7;
        AlxLp586x_RegVal_0x6D_Dot_lod8   lod8;
        AlxLp586x_RegVal_0x6E_Dot_lod9   lod9;
        AlxLp586x_RegVal_0x6F_Dot_lod10  lod10;
        AlxLp586x_RegVal_0x70_Dot_lod11  lod11;
        AlxLp586x_RegVal_0x71_Dot_lod12  lod12;
        AlxLp586x_RegVal_0x72_Dot_lod13  lod13;
        AlxLp586x_RegVal_0x73_Dot_lod14  lod14;
        AlxLp586x_RegVal_0x74_Dot_lod15  lod15;
        AlxLp586x_RegVal_0x75_Dot_lod16  lod16;
        AlxLp586x_RegVal_0x76_Dot_lod17  lod17;
	    uint8_t raw; // fallback so we can memset the whole byte
    } val;
} AlxLp586x_Reg_DotLod;


//------------------------------------------------------------------------------
// 0x86–0x97: Dot LSD status registers (L0…L5, CS0…CS7)
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;
    uint8_t len;
    union
    {
        AlxLp586x_RegVal_0x86_Dot_lsd0   lsd0;
        AlxLp586x_RegVal_0x87_Dot_lsd1   lsd1;
        AlxLp586x_RegVal_0x88_Dot_lsd2   lsd2;
        AlxLp586x_RegVal_0x89_Dot_lsd3   lsd3;
        AlxLp586x_RegVal_0x8A_Dot_lsd4   lsd4;
        AlxLp586x_RegVal_0x8B_Dot_lsd5   lsd5;
        AlxLp586x_RegVal_0x8C_Dot_lsd6   lsd6;
        AlxLp586x_RegVal_0x8D_Dot_lsd7   lsd7;
        AlxLp586x_RegVal_0x8E_Dot_lsd8   lsd8;
        AlxLp586x_RegVal_0x8F_Dot_lsd9   lsd9;
        AlxLp586x_RegVal_0x90_Dot_lsd10  lsd10;
        AlxLp586x_RegVal_0x91_Dot_lsd11  lsd11;
        AlxLp586x_RegVal_0x92_Dot_lsd12  lsd12;
        AlxLp586x_RegVal_0x93_Dot_lsd13  lsd13;
        AlxLp586x_RegVal_0x94_Dot_lsd14  lsd14;
        AlxLp586x_RegVal_0x95_Dot_lsd15  lsd15;
        AlxLp586x_RegVal_0x96_Dot_lsd16  lsd16;
        AlxLp586x_RegVal_0x97_Dot_lsd17  lsd17;
	    uint8_t raw; // fallback so we can memset the whole byte
    } val;
} AlxLp586x_Reg_DotLsd;


//------------------------------------------------------------------------------
// 0x100–0x16B: Dot current registers (DC0…DC107)
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
    uint8_t addr;     // Always 1 byte
    uint8_t  len;     // Always 1 byte
    union
    {
        uint8_t  dc0;    // L0–CS0
        uint8_t  dc1;    // L0–CS1
        uint8_t  dc2;    // L0–CS2
        uint8_t  dc3;    // L0–CS3
        uint8_t  dc4;    // L0–CS4
        uint8_t  dc5;    // L0–CS5
        uint8_t  dc6;    // L0–CS6
        uint8_t  dc7;    // L0–CS7
        uint8_t  dc8;    // L0–CS8
        uint8_t  dc9;    // L0–CS9
        uint8_t  dc10;   // L0–CS10
        uint8_t  dc11;   // L0–CS11
        uint8_t  dc12;   // L0–CS12
        uint8_t  dc13;   // L0–CS13
        uint8_t  dc14;   // L0–CS14
        uint8_t  dc15;   // L0–CS15
        uint8_t  dc16;   // L0–CS16
        uint8_t  dc17;   // L0–CS17
        uint8_t  dc18;   // L1–CS0
        uint8_t  dc19;   // L1–CS1
        uint8_t  dc20;   // L1–CS2
        uint8_t  dc21;   // L1–CS3
        uint8_t  dc22;   // L1–CS4
        uint8_t  dc23;   // L1–CS5
        uint8_t  dc24;   // L1–CS6
        uint8_t  dc25;   // L1–CS7
        uint8_t  dc26;   // L1–CS8
        uint8_t  dc27;   // L1–CS9
        uint8_t  dc28;   // L1–CS10
        uint8_t  dc29;   // L1–CS11
        uint8_t  dc30;   // L1–CS12
        uint8_t  dc31;   // L1–CS13
        uint8_t  dc32;   // L1–CS14
        uint8_t  dc33;   // L1–CS15
        uint8_t  dc34;   // L1–CS16
        uint8_t  dc35;   // L1–CS17
        uint8_t  dc36;   // L2–CS0
        uint8_t  dc37;   // L2–CS1
        uint8_t  dc38;   // L2–CS2
        uint8_t  dc39;   // L2–CS3
        uint8_t  dc40;   // L2–CS4
        uint8_t  dc41;   // L2–CS5
        uint8_t  dc42;   // L2–CS6
        uint8_t  dc43;   // L2–CS7
        uint8_t  dc44;   // L2–CS8
        uint8_t  dc45;   // L2–CS9
        uint8_t  dc46;   // L2–CS10
        uint8_t  dc47;   // L2–CS11
        uint8_t  dc48;   // L2–CS12
        uint8_t  dc49;   // L2–CS13
        uint8_t  dc50;   // L2–CS14
        uint8_t  dc51;   // L2–CS15
        uint8_t  dc52;   // L2–CS16
        uint8_t  dc53;   // L2–CS17
        uint8_t  dc54;   // L3–CS0
        uint8_t  dc55;   // L3–CS1
        uint8_t  dc56;   // L3–CS2
        uint8_t  dc57;   // L3–CS3
        uint8_t  dc58;   // L3–CS4
        uint8_t  dc59;   // L3–CS5
        uint8_t  dc60;   // L3–CS6
        uint8_t  dc61;   // L3–CS7
        uint8_t  dc62;   // L3–CS8
        uint8_t  dc63;   // L3–CS9
        uint8_t  dc64;   // L3–CS10
        uint8_t  dc65;   // L3–CS11
        uint8_t  dc66;   // L3–CS12
        uint8_t  dc67;   // L3–CS13
        uint8_t  dc68;   // L3–CS14
        uint8_t  dc69;   // L3–CS15
        uint8_t  dc70;   // L3–CS16
        uint8_t  dc71;   // L3–CS17
        uint8_t  dc72;   // L4–CS0
        uint8_t  dc73;   // L4–CS1
        uint8_t  dc74;   // L4–CS2
        uint8_t  dc75;   // L4–CS3
        uint8_t  dc76;   // L4–CS4
        uint8_t  dc77;   // L4–CS5
        uint8_t  dc78;   // L4–CS6
        uint8_t  dc79;   // L4–CS7
        uint8_t  dc80;   // L4–CS8
        uint8_t  dc81;   // L4–CS9
        uint8_t  dc82;   // L4–CS10
        uint8_t  dc83;   // L4–CS11
        uint8_t  dc84;   // L4–CS12
        uint8_t  dc85;   // L4–CS13
        uint8_t  dc86;   // L4–CS14
        uint8_t  dc87;   // L4–CS15
        uint8_t  dc88;   // L4–CS16
        uint8_t  dc89;   // L4–CS17
        uint8_t  dc90;   // L5–CS0
        uint8_t  dc91;   // L5–CS1
        uint8_t  dc92;   // L5–CS2
        uint8_t  dc93;   // L5–CS3
        uint8_t  dc94;   // L5–CS4
        uint8_t  dc95;   // L5–CS5
        uint8_t  dc96;   // L5–CS6
        uint8_t  dc97;   // L5–CS7
        uint8_t  dc98;   // L5–CS8
        uint8_t  dc99;   // L5–CS9
        uint8_t  dc100;  // L5–CS10
        uint8_t  dc101;  // L5–CS11
        uint8_t  dc102;  // L5–CS12
        uint8_t  dc103;  // L5–CS13
        uint8_t  dc104;  // L5–CS14
        uint8_t  dc105;  // L5–CS15
        uint8_t  dc106;  // L5–CS16
        uint8_t  dc107;  // L5–CS17
	    uint8_t raw; // fallback so we can memset the whole byte
    } val;
} AlxLp586x_Reg_DotCurr;


//------------------------------------------------------------------------------
// 0x200–0x2D7: Dot current registers (pwm_bri0…pwm_bri215 )
//------------------------------------------------------------------------------
typedef struct
{
	uint8_t i2cAddr_Offset; // Added because addresses ≥ 0x100
	uint8_t addr; // Always 1 byte
	uint8_t  len; // Always 1 byte
	union
	{
		uint8_t pwm_bri0;
		uint8_t pwm_bri1;
        uint8_t pwm_bri2;
        uint8_t pwm_bri3;
        uint8_t pwm_bri4;
        uint8_t pwm_bri5;
        uint8_t pwm_bri6;
        uint8_t pwm_bri7;
        uint8_t pwm_bri8;
        uint8_t pwm_bri9;
        uint8_t pwm_bri10;
        uint8_t pwm_bri11;
        uint8_t pwm_bri12;
        uint8_t pwm_bri13;
        uint8_t pwm_bri14;
        uint8_t pwm_bri15;
        uint8_t pwm_bri16;
        uint8_t pwm_bri17;
        uint8_t pwm_bri18;
        uint8_t pwm_bri19;
        uint8_t pwm_bri20;
        uint8_t pwm_bri21;
        uint8_t pwm_bri22;
        uint8_t pwm_bri23;
        uint8_t pwm_bri24;
        uint8_t pwm_bri25;
        uint8_t pwm_bri26;
        uint8_t pwm_bri27;
        uint8_t pwm_bri28;
        uint8_t pwm_bri29;
        uint8_t pwm_bri30;
        uint8_t pwm_bri31;
        uint8_t pwm_bri32;
        uint8_t pwm_bri33;
        uint8_t pwm_bri34;
        uint8_t pwm_bri35;
        uint8_t pwm_bri36;
        uint8_t pwm_bri37;
        uint8_t pwm_bri38;
        uint8_t pwm_bri39;
        uint8_t pwm_bri40;
        uint8_t pwm_bri41;
        uint8_t pwm_bri42;
        uint8_t pwm_bri43;
        uint8_t pwm_bri44;
        uint8_t pwm_bri45;
        uint8_t pwm_bri46;
        uint8_t pwm_bri47;
        uint8_t pwm_bri48;
        uint8_t pwm_bri49;
        uint8_t pwm_bri50;
        uint8_t pwm_bri51;
        uint8_t pwm_bri52;
        uint8_t pwm_bri53;
        uint8_t pwm_bri54;
        uint8_t pwm_bri55;
        uint8_t pwm_bri56;
        uint8_t pwm_bri57;
        uint8_t pwm_bri58;
        uint8_t pwm_bri59;
        uint8_t pwm_bri60;
        uint8_t pwm_bri61;
        uint8_t pwm_bri62;
        uint8_t pwm_bri63;
        uint8_t pwm_bri64;
        uint8_t pwm_bri65;
        uint8_t pwm_bri66;
        uint8_t pwm_bri67;
        uint8_t pwm_bri68;
        uint8_t pwm_bri69;
        uint8_t pwm_bri70;
        uint8_t pwm_bri71;
        uint8_t pwm_bri72;
        uint8_t pwm_bri73;
        uint8_t pwm_bri74;
        uint8_t pwm_bri75;
        uint8_t pwm_bri76;
        uint8_t pwm_bri77;
        uint8_t pwm_bri78;
        uint8_t pwm_bri79;
        uint8_t pwm_bri80;
        uint8_t pwm_bri81;
        uint8_t pwm_bri82;
        uint8_t pwm_bri83;
        uint8_t pwm_bri84;
        uint8_t pwm_bri85;
        uint8_t pwm_bri86;
        uint8_t pwm_bri87;
        uint8_t pwm_bri88;
        uint8_t pwm_bri89;
        uint8_t pwm_bri90;
        uint8_t pwm_bri91;
        uint8_t pwm_bri92;
        uint8_t pwm_bri93;
        uint8_t pwm_bri94;
        uint8_t pwm_bri95;
        uint8_t pwm_bri96;
        uint8_t pwm_bri97;
        uint8_t pwm_bri98;
        uint8_t pwm_bri99;
        uint8_t pwm_bri100;
        uint8_t pwm_bri101;
        uint8_t pwm_bri102;
        uint8_t pwm_bri103;
        uint8_t pwm_bri104;
        uint8_t pwm_bri105;
        uint8_t pwm_bri106;
        uint8_t pwm_bri107;
        uint8_t pwm_bri108;
        uint8_t pwm_bri109;
        uint8_t pwm_bri110;
        uint8_t pwm_bri111;
        uint8_t pwm_bri112;
        uint8_t pwm_bri113;
        uint8_t pwm_bri114;
        uint8_t pwm_bri115;
        uint8_t pwm_bri116;
        uint8_t pwm_bri117;
        uint8_t pwm_bri118;
        uint8_t pwm_bri119;
        uint8_t pwm_bri120;
        uint8_t pwm_bri121;
        uint8_t pwm_bri122;
        uint8_t pwm_bri123;
        uint8_t pwm_bri124;
        uint8_t pwm_bri125;
        uint8_t pwm_bri126;
        uint8_t pwm_bri127;
        uint8_t pwm_bri128;
        uint8_t pwm_bri129;
        uint8_t pwm_bri130;
        uint8_t pwm_bri131;
        uint8_t pwm_bri132;
        uint8_t pwm_bri133;
        uint8_t pwm_bri134;
        uint8_t pwm_bri135;
        uint8_t pwm_bri136;
        uint8_t pwm_bri137;
        uint8_t pwm_bri138;
        uint8_t pwm_bri139;
        uint8_t pwm_bri140;
        uint8_t pwm_bri141;
        uint8_t pwm_bri142;
        uint8_t pwm_bri143;
        uint8_t pwm_bri144;
        uint8_t pwm_bri145;
        uint8_t pwm_bri146;
        uint8_t pwm_bri147;
        uint8_t pwm_bri148;
        uint8_t pwm_bri149;
        uint8_t pwm_bri150;
        uint8_t pwm_bri151;
        uint8_t pwm_bri152;
        uint8_t pwm_bri153;
        uint8_t pwm_bri154;
        uint8_t pwm_bri155;
        uint8_t pwm_bri156;
        uint8_t pwm_bri157;
        uint8_t pwm_bri158;
        uint8_t pwm_bri159;
        uint8_t pwm_bri160;
        uint8_t pwm_bri161;
        uint8_t pwm_bri162;
        uint8_t pwm_bri163;
        uint8_t pwm_bri164;
        uint8_t pwm_bri165;
        uint8_t pwm_bri166;
        uint8_t pwm_bri167;
        uint8_t pwm_bri168;
        uint8_t pwm_bri169;
        uint8_t pwm_bri170;
        uint8_t pwm_bri171;
        uint8_t pwm_bri172;
        uint8_t pwm_bri173;
        uint8_t pwm_bri174;
        uint8_t pwm_bri175;
        uint8_t pwm_bri176;
        uint8_t pwm_bri177;
        uint8_t pwm_bri178;
        uint8_t pwm_bri179;
        uint8_t pwm_bri180;
        uint8_t pwm_bri181;
        uint8_t pwm_bri182;
        uint8_t pwm_bri183;
        uint8_t pwm_bri184;
        uint8_t pwm_bri185;
        uint8_t pwm_bri186;
        uint8_t pwm_bri187;
        uint8_t pwm_bri188;
        uint8_t pwm_bri189;
        uint8_t pwm_bri190;
        uint8_t pwm_bri191;
        uint8_t pwm_bri192;
        uint8_t pwm_bri193;
        uint8_t pwm_bri194;
        uint8_t pwm_bri195;
        uint8_t pwm_bri196;
        uint8_t pwm_bri197;
        uint8_t pwm_bri198;
        uint8_t pwm_bri199;
        uint8_t pwm_bri200;
        uint8_t pwm_bri201;
        uint8_t pwm_bri202;
        uint8_t pwm_bri203;
        uint8_t pwm_bri204;
        uint8_t pwm_bri205;
        uint8_t pwm_bri206;
        uint8_t pwm_bri207;
        uint8_t pwm_bri208;
        uint8_t pwm_bri209;
        uint8_t pwm_bri210;
        uint8_t pwm_bri211;
        uint8_t pwm_bri212;
        uint8_t pwm_bri213;
        uint8_t pwm_bri214;
		uint8_t pwm_bri215;
		uint8_t raw; // Fallback so we can memset the whole byte
	} val;
} AlxLp586x_Reg_PwmBri;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxLp586x_Reg_0x00_Chip_En      _0x00_Chip_En;
	AlxLp586x_Reg_0x01_Dev_initial  _0x01_Dev_initial;
	AlxLp586x_Reg_0x02_Dev_config1  _0x02_Dev_config1;
	AlxLp586x_Reg_0x03_Dev_config2  _0x03_Dev_config2;
	AlxLp586x_Reg_0x04_Dev_config3  _0x04_Dev_config3;
	AlxLp586x_Reg_0x05_Global_bri   _0x05_Global_bri;

	// 0x06–0x08: Group brightness registers (Group0_bri…Group2_bri)
	AlxLp586x_Reg_GroupBri          _group_bri[3];

	// 0x09…0x0B: CC-current-set
	AlxLp586x_Reg_CurrentSetCC      _current_set_cc[3];

	// 0x0C–0x29: Dot group-select for L0…L5, CS0…CS17
	AlxLp586x_Reg_DotGrpSel         _dot_grp_sel[30];

	// 0x43–0x54: Dot on/off registers (L0…L5, CS0…CS7)
    AlxLp586x_Reg_DotOnOff          _dot_onoff[18];

	// 0x65–0x76: Dot Lod open load errors registers (L0…L5, CS0…CS7)
	AlxLp586x_Reg_DotLod            _dot_lod[18];

	// 0x86–0x97: Dot Lsd short-cut load errors registers (L0…L5, CS0…CS7)
	AlxLp586x_Reg_DotLsd            _dot_lsd[18];

	// 0x64: Fault_state
    AlxLp586x_Reg_0x64_Fault_state  _0x64_Fault_state;

	// 0xA7–0xA9: Clears and reset registers
    AlxLp586x_Reg_0xA7_LOD_clear    _0xA7_LOD_clear;
    AlxLp586x_Reg_0xA8_LSD_clear    _0xA8_LSD_clear;
    AlxLp586x_Reg_0xA9_Reset        _0xA9_Reset;

	// 0x100–0x16B: Dot-current (DC0…DC107)
	AlxLp586x_Reg_DotCurr           _dot_curr[108];

	// 0x200–0x2D7: Dot current registers (pwm_bri0…pwm_bri215 )
	AlxLp586x_Reg_PwmBri            _pwm_bri[216];

} AlxLp586x_Reg;


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxIoPin* do_LED_DRV_EN;
    AlxIoPin* do_LED_DRV_SYNC;
	AlxI2c* i2c;
	uint8_t i2cAddr;
	bool i2cCheckWithRead;
	uint8_t i2cNumOfTries;
	uint16_t i2cTimeout_ms;

	// Variables
	AlxLp586x_Reg reg;

	// Info
	bool wasCtorCalled;
	bool isInitPeriph;
	bool isInit;
} AlxLp586x;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLp586x_Ctor
(
	AlxLp586x* me,
	AlxIoPin* do_LED_DRV_EN,
	AlxIoPin* do_LED_DRV_SYNC,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLp586x_InitPeriph(AlxLp586x* me);
Alx_Status AlxLp586x_DeInitPeriph(AlxLp586x* me);
Alx_Status AlxLp586x_Init(AlxLp586x* me);
Alx_Status AlxLp586x_DeInit(AlxLp586x* me);
Alx_Status AlxLp586x_Handle(AlxLp586x* me);
Alx_Status AlxLp586x_Reg_Write(AlxLp586x* me, void* reg);
void AlxLp586x_Led_Write(AlxLp586x* me, uint8_t ledNum, bool val);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LP586x_H
