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
	Data_Ref_Mode_Off = 0,
	Data_Ref_Mode_1 = 1,	// PWM RESOLUTION : 8 Bits, Data update instantly, No EXTERNAL VSYNC
	Data_Ref_Mode_2 = 2,	// PWM RESOLUTION : 8 Bits, Data update by frame, With EXTERNAL VSYNC
	Data_Ref_Mode_3 = 3		// PWM RESOLUTION : 8 Bits, Data update by frame, With EXTERNAL VSYNC
} AlxLp586x_RegEnum_0x01_Data_Ref_Mode;
typedef enum
{
	PWM_Fre_125kHz = 0,		// 125kHz
	PWM_Fre_62k5Hz = 1		// 62,5kHz
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
	SW_BLK_1us = 0, // switch blank time: 1μs
	SW_BLK_0u5s = 1	// switch blank time: 0,5μs
} AlxLp586x_RegEnum_0x02_SW_BLK;
typedef enum
{
	ExponentialScale = 0,	// human-eye-friendly dimming curve
	LinearScale = 1
} AlxLp586x_RegEnum_0x02_PWM_Scale_Mode;
typedef enum
{
	Phase_Shift_0ns = 0,
	Phase_Shift_125ns = 1
} AlxLp586x_RegEnum_0x02_PWM_Phase_Shift;
typedef enum
{
	CS_ON_Shift_Off = 0,
	CS_ON_Shift_On = 1
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
	LOD_removal_On = 1
} AlxLp586x_RegEnum_0x03_LOD_removal; // turns off the current sink of the open channel
typedef enum
{
	LSD_removal_Off = 0,
	LSD_removal_On = 1
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
	Maximum_Current_7m5A = 0,
	Maximum_Current_12m5A = 1,
	Maximum_Current_25mA = 2,
	Maximum_Current_37m5A = 3, // Default
	Maximum_Current_50mA = 4,
	Maximum_Current_75mA = 5,
	Maximum_Current_100mA = 6
} AlxLp586x_RegEnum_0x04_Maximum_Current;
typedef enum
{
	Up_Deghost_Disnabled = 0,
	Up_Deghost_Enabled = 1
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
	L0_CS3_No_Group_member = 0,
	L0_CS3_Comp_Group1 = 1,
	L0_CS3_Comp_Group2 = 2,
	L0_CS3_Comp_Group3 = 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS3_Group;
typedef enum
{
	L0_CS2_No_Group_member = 0,
	L0_CS2_Comp_Group1 = 1,
	L0_CS2_Comp_Group2 = 2,
	L0_CS2_Comp_Group3 = 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS2_Group;
	typedef enum
{
	L0_CS1_No_Group_member = 0,
	L0_CS1_Comp_Group1 = 1,
	L0_CS1_Comp_Group2 = 2,
	L0_CS1_Comp_Group3 = 3
} AlxLp586x_RegEnum_0x0C_Dot_L0_CS1_Group;
typedef enum
{
	L0_CS0_No_Group_member = 0,
	L0_CS0_Comp_Group1 = 1,
	L0_CS0_Comp_Group2 = 2,
	L0_CS0_Comp_Group3 = 3
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
// Register Structures
//******************************************************************************
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x00_Chip_En val;
} AlxLp586x_Reg_0x00_Chip_En;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x01_Dev_initial val;
} AlxLp586x_Reg_0x01_Dev_initial;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x02_Dev_config1 val;
} AlxLp586x_Reg_0x02_Dev_config1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x03_Dev_config2 val;
} AlxLp586x_Reg_0x03_Dev_config2;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x04_Dev_config3 val;
} AlxLp586x_Reg_0x04_Dev_config3;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x05_Global_bri val;
} AlxLp586x_Reg_0x05_Global_bri;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x06_Group0_bri val;
} AlxLp586x_Reg_0x06_Group0_bri;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x07_Group1_bri val;
} AlxLp586x_Reg_0x07_Group1_bri;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x08_Group2_bri val;
} AlxLp586x_Reg_0x08_Group2_bri;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x09_R_current_set_CC_Group1 val;
} AlxLp586x_Reg_0x09_R_current_set_CC_Group1;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x0A_G_current_set_CC_Group2 val;
} AlxLp586x_Reg_0x0A_G_current_set_CC_Group2;
typedef struct
{
	uint8_t addr;
	uint8_t len;
	AlxLp586x_RegVal_0x0B_B_current_set_CC_Group3 val;
} AlxLp586x_Reg_0x0B_B_current_set_CC_Group3;


//******************************************************************************
// Main Register Structure
//******************************************************************************
typedef struct
{
	AlxLp586x_Reg_0x00_Chip_En _0x00_Chip_En;
	AlxLp586x_Reg_0x01_Dev_initial _Dev_initial;
	AlxLp586x_Reg_0x02_Dev_config1 _Dev_config1;
	AlxLp586x_Reg_0x03_Dev_config2 _Dev_config2;
	AlxLp586x_Reg_0x04_Dev_config3 _Dev_config3;
	AlxLp586x_Reg_0x05_Global_bri _Global_bri;
	AlxLp586x_Reg_0x06_Group0_bri _Group0_bri;
	AlxLp586x_Reg_0x07_Group1_bri _0x07_Group1_bri;
	AlxLp586x_Reg_0x08_Group2_bri _0x08_Group2_bri;
	AlxLp586x_Reg_0x09_R_current_set_CC_Group1 _0x09_R_current_set_CC_Group1;
	AlxLp586x_Reg_0x0A_G_current_set_CC_Group2 _0x0A_G_current_set_CC_Group2;
	AlxLp586x_Reg_0x0B_B_current_set_CC_Group3 _0x0B_B_current_set_CC_Group3;
} AlxLp586x_Reg;


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
void AlxLp586x_Led_Write(AlxLp586x* me, uint8_t ledNum, bool val);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LP586x_H
