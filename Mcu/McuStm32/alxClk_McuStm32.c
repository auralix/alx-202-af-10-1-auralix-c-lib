/**
  ******************************************************************************
  * @file		alxClk_McuStm32.c
  * @brief		Auralix C Library - ALX Clock MCU STM32 Module
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
// Includes
//******************************************************************************
#include "alxClk_McuStm32.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
#if defined(ALX_STM32F0)
static void AlxClk_Ctor_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hsi_8MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hse_8MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz(AlxClk* me);
#endif
#if defined(ALX_STM32F1)
static void AlxClk_Ctor_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(AlxClk* me);
#endif
#if defined(ALX_STM32F4)
static void AlxClk_Ctor_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz(AlxClk* me);
#endif
#if defined(ALX_STM32F7)
static void AlxClk_Ctor_McuStm32F7_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hse_25MHz(AlxClk* me);
#endif
#if defined(ALX_STM32G4)
static void AlxClk_Ctor_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(AlxClk* me);
#endif
#if defined(ALX_STM32L0)
static void AlxClk_Ctor_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1(AlxClk* me);
#endif
#if defined(ALX_STM32L4)
static void AlxClk_Ctor_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_25MHz(AlxClk* me);
#endif
#if defined(ALX_STM32U5)
static void AlxClk_Ctor_McuStm32U5_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Pclk3Apb3_4MHz_Msi_4MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuStm32U5_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Pclk3Apb3_16MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hsi_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz(AlxClk* me);
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxCan_Ctor_IsClkOk(AlxClk* me);
static void AlxClk_Periph_Gpio_EnableClk(AlxClk* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		config
  */
ALX_WEAK void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config
)
{
	// Parameters
	me->config = config;

	// Variables
	#if defined(ALX_STM32F0)
	if		(me->config == AlxClk_Config_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hsi_8MHz_Default)								{ AlxClk_Ctor_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hsi_8MHz_Default(me);						me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hse_8MHz)										{ AlxClk_Ctor_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hse_8MHz(me);								me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz)										{ AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz(me);								me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz)										{ AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz(me);								me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz_BackupHsi_8MHz)						{ AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz(me);								me->backupHsiEnable = true;		}
	#endif
	#if defined(ALX_STM32F1)
	if		(me->config == AlxClk_Config_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default)					{ AlxClk_Ctor_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default(me);		me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz)						{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(me);				me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz)						{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(me);				me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8MHz)		{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(me);				me->backupHsiEnable = true;		}
	#endif
	#if defined(ALX_STM32F4)
	if		(me->config == AlxClk_Config_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default)				{ AlxClk_Ctor_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz)					{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz)					{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16MHz)	{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(me);			me->backupHsiEnable = true;		}
	else if	(me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz)					{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz(me);			me->backupHsiEnable = false;	}
	#endif
	#if defined(ALX_STM32F7)
	if		(me->config == AlxClk_Config_McuStm32F7_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default)				{ AlxClk_Ctor_McuStm32F7_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hsi_16MHz)					{ AlxClk_Ctor_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hse_25MHz)					{ AlxClk_Ctor_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hse_25MHz(me);			me->backupHsiEnable = false;	}
	#endif
	#if defined(ALX_STM32G4)
	if		(me->config == AlxClk_Config_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default)				{ AlxClk_Ctor_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz)					{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz)					{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz_BackupHsi_16MHz)	{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(me);			me->backupHsiEnable = true;		}
	#endif
	#if defined(ALX_STM32L0)
	if		(me->config == AlxClk_Config_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default)				{ AlxClk_Ctor_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz)						{ AlxClk_Ctor_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1)					{ AlxClk_Ctor_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1(me);			me->backupHsiEnable = false;	}
	#endif
	#if defined(ALX_STM32L4)
	if		(me->config == AlxClk_Config_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default)					{ AlxClk_Ctor_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default(me);		me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz)						{ AlxClk_Ctor_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hsi_16MHz)						{ AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz)						{ AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz(me);				me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz_BackupHsi_16MHz)		{ ALX_CLK_ASSERT(false); me->backupHsiEnable = true; }	// TV: TODO
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hsi_16MHz)					{ AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hsi_16MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_25MHz)					{ AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_25MHz(me);			me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_12MHz_BackupHsi_16MHz)	{ ALX_CLK_ASSERT(false); me->backupHsiEnable = true; }	// TV: TODO
	#endif
	#if defined(ALX_STM32U5)
	if		(me->config == AlxClk_Config_McuStm32U5_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Pclk3Apb3_4MHz_Msi_4MHz_Default)						{ AlxClk_Ctor_McuStm32U5_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Pclk3Apb3_4MHz_Msi_4MHz_Default(me);		me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32U5_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Pclk3Apb3_16MHz_Hsi_16MHz)							{ AlxClk_Ctor_McuStm32U5_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Pclk3Apb3_16MHz_Hsi_16MHz(me);		me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hsi_16MHz)						{ AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hsi_16MHz(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz)						{ AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz(me);	me->backupHsiEnable = false;	}
	else if	(me->config == AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz_BackupHsi_16MHz)		{ ALX_CLK_ASSERT(false); me->backupHsiEnable = true; }	// TV: TODO
	#endif
	else																																			{ ALX_CLK_ASSERT(false); return; }	// We should not get here
	me->isBackupHsiUsed = false;

	me->systemCoreClock = 0;
	me->sysclk = 0;
	me->hclk = 0;
	me->pclk1Apb1 = 0;
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	me->pclk2Apb2 = 0;
	#endif
	#if defined(ALX_STM32U5)
	me->pclk3Apb3 = 0;
	#endif

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
ALX_WEAK Alx_Status AlxClk_Init(AlxClk* me)
{
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);
	//ALX_CLK_ASSERT(me->isInit == false);

	// If backup HSI enabled, enable CSS
	if (me->backupHsiEnable)
	{
		HAL_RCC_EnableCSS();
	}

	// Init oscillators
	if(HAL_RCC_OscConfig(&me->iosc) != HAL_OK)
	{
		// If backup HSI enabled
		if (me->backupHsiEnable)
		{
			// Clear wasCtorCalled
			me->wasCtorCalled = false;

			// Ctor
			#if defined(ALX_STM32F0)
			if (me->config == AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz_BackupHsi_8MHz)										{ AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz(me);						me->isBackupHsiUsed = true;	}
			#endif
			#if defined(ALX_STM32F1)
			if (me->config == AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8MHz)						{ AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(me);		me->isBackupHsiUsed = true;	}
			#endif
			#if defined(ALX_STM32F4)
			if (me->config == AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16MHz)						{ AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true;	}
			#endif
			#if defined(ALX_STM32F7)
			if (true)																																{ ALX_CLK_ASSERT(false); return Alx_Err; } // We should not get here
			#endif
			#if defined(ALX_STM32G4)
			if (me->config == AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz_BackupHsi_16MHz)					{ AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true;	}
			#endif
			#if defined(ALX_STM32L4)
			if (me->config == AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz_BackupHsi_16MHz)						{ AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true; }
			else if (me->config == AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_12MHz_BackupHsi_16MHz)				{ AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true; }
			#endif
			#if defined(ALX_STM32U5)
			if (me->config == AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz_BackupHsi_16MHz)	{ AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hsi_16MHz(me);	me->isBackupHsiUsed = true; }
			#endif
			else																																	{ ALX_CLK_ASSERT(false); return Alx_Err; } // We should not get here

			// Set wasCtorCalled
			me->wasCtorCalled = true;

			// Init
			if(HAL_RCC_OscConfig(&me->iosc) != HAL_OK) { ALX_CLK_TRACE("Err"); return Alx_Err; }
		}
		// Else trace & return
		else
		{
			ALX_CLK_TRACE("Err");
			return Alx_Err;
		}
	}

	// Enable GPIO periphery clocks
	AlxClk_Periph_Gpio_EnableClk(me);

	// Enable PWR periphery clock
	__HAL_RCC_PWR_CLK_ENABLE();

	// Init power regulator
	#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	if(HAL_PWREx_ControlVoltageScaling(me->pwrRegVoltageScale) != HAL_OK) { ALX_CLK_TRACE("Err"); return Alx_Err; };
	#endif
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(ALX_STM32F7)
	if (me->isPwrRegOverDrive)
	{
		if(HAL_PWREx_EnableOverDrive() != HAL_OK) { ALX_CLK_TRACE("Err"); return Alx_Err; };
	}
	#endif
	#if defined(ALX_STM32L0)
	while(__HAL_PWR_GET_FLAG(PWR_FLAG_VOS));
	__HAL_PWR_VOLTAGESCALING_CONFIG(me->pwrRegVoltageScale);
	while(__HAL_PWR_GET_FLAG(PWR_FLAG_VOS));
	#endif

	// Enable SYSCFG periphery clock
	__HAL_RCC_SYSCFG_CLK_ENABLE();

	// Init clocks
	if(HAL_RCC_ClockConfig(&me->iclk, me->flashLatency) != HAL_OK) { ALX_CLK_TRACE("Err"); return Alx_Err; }

	// Check clocks
	if(AlxCan_Ctor_IsClkOk(me) == false) { ALX_CLK_TRACE("Err"); return Alx_Err; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
ALX_WEAK Alx_Status AlxClk_DeInit(AlxClk* me)
{
	// TV: TODO
	ALX_CLK_ASSERT(false);
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		clk
  * @return
  */
ALX_WEAK uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	if(me->isInit)
	{
		if(clk == AlxClk_Clk_McuStm32_SystemCoreClock )		return me->systemCoreClock;
		if(clk == AlxClk_Clk_McuStm32_Sysclk )				return me->sysclk;
		if(clk == AlxClk_Clk_McuStm32_Hclk )				return me->hclk;
		if(clk == AlxClk_Clk_McuStm32_Pclk1Apb1 )			return me->pclk1Apb1;
		#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
		if(clk == AlxClk_Clk_McuStm32_Pclk2Apb2 )			return me->pclk2Apb2;
		#endif
		#if defined(ALX_STM32U5)
		if(clk == AlxClk_Clk_McuStm32_Pclk3Apb3 )			return me->pclk3Apb3;
		#endif
	}

	if(clk == AlxClk_Clk_McuStm32_SystemCoreClock_Ctor )	return me->systemCoreClock_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Sysclk_Ctor )				return me->sysclk_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Hclk_Ctor )				return me->hclk_Ctor;
	if(clk == AlxClk_Clk_McuStm32_Pclk1Apb1_Ctor )			return me->pclk1Apb1_Ctor;
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	if(clk == AlxClk_Clk_McuStm32_Pclk2Apb2_Ctor )			return me->pclk2Apb2_Ctor;
	#endif
	#if defined(ALX_STM32U5)
	if(clk == AlxClk_Clk_McuStm32_Pclk3Apb3_Ctor )			return me->pclk3Apb3;
	#endif
	if(clk == AlxClk_Clk_McuStm32_Pclk1Apb1Tim_Ctor )		return me->pclk1Apb1Tim_Ctor;
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	if(clk == AlxClk_Clk_McuStm32_Pclk2Apb2Tim_Ctor )		return me->pclk2Apb2Tim_Ctor;
	#endif
	if(clk == AlxClk_Clk_McuStm32_MainPllInputClk_Ctor )	return me->mainPllInputClk_Ctor;

	ALX_CLK_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxClk_Irq_Handle(AlxClk* me)
{
	(void)me;
	HAL_RCC_NMI_IRQHandler();
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
#if defined(ALX_STM32F0)
static void AlxClk_Ctor_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hsi_8MHz_Default(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.HSI14State = RCC_HSI14_OFF;
	me->iosc.HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLMUL = ALX_NULL;
	me->iosc.PLL.PREDIV = ALX_NULL;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 8000000;
	me->sysclk_Ctor = 8000000;
	me->hclk_Ctor = 8000000;
	me->pclk1Apb1_Ctor = 8000000;
	me->pclk2Apb2_Ctor = 0;
	me->pclk1Apb1Tim_Ctor = 8000000;
	me->pclk2Apb2Tim_Ctor = 0;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hse_8MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.HSI14State = RCC_HSI14_OFF;
	me->iosc.HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLMUL = ALX_NULL;
	me->iosc.PLL.PREDIV = ALX_NULL;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 8000000;
	me->sysclk_Ctor = 8000000;
	me->hclk_Ctor = 8000000;
	me->pclk1Apb1_Ctor = 8000000;
	me->pclk2Apb2_Ctor = 0;
	me->pclk1Apb1Tim_Ctor = 8000000;
	me->pclk2Apb2Tim_Ctor = 0;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.HSI14State = RCC_HSI14_OFF;
	me->iosc.HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL6;
	me->iosc.PLL.PREDIV = RCC_PREDIV_DIV1;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_1;

	// Set expected clocks
	me->systemCoreClock_Ctor = 48000000;
	me->sysclk_Ctor = 48000000;
	me->hclk_Ctor = 48000000;
	me->pclk1Apb1_Ctor = 48000000;
	me->pclk2Apb2_Ctor = 0;
	me->pclk1Apb1Tim_Ctor = 48000000;
	me->pclk2Apb2Tim_Ctor = 0;
	me->mainPllInputClk_Ctor = 8000000;
}
static void AlxClk_Ctor_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.HSI14State = RCC_HSI14_OFF;
	me->iosc.HSI14CalibrationValue = RCC_HSI14CALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL6;
	me->iosc.PLL.PREDIV = RCC_PREDIV_DIV1;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_1;

	// Set expected clocks
	me->systemCoreClock_Ctor = 48000000;
	me->sysclk_Ctor = 48000000;
	me->hclk_Ctor = 48000000;
	me->pclk1Apb1_Ctor = 48000000;
	me->pclk2Apb2_Ctor = 0;
	me->pclk1Apb1Tim_Ctor = 48000000;
	me->pclk2Apb2Tim_Ctor = 0;
	me->mainPllInputClk_Ctor = 8000000;
}
#endif
#if defined(ALX_STM32F1)
static void AlxClk_Ctor_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default(AlxClk* me)
{
	// #1 Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLMUL = 0;


	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 8000000UL;
	me->sysclk_Ctor = 8000000UL;
	me->hclk_Ctor = 8000000UL;
	me->pclk1Apb1_Ctor = 8000000UL;
	me->pclk2Apb2_Ctor = 8000000UL;
	me->pclk1Apb1Tim_Ctor = 8000000UL;
	me->pclk2Apb2Tim_Ctor = 8000000UL;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz(AlxClk* me)
{
	// #1 Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL16;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV2;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_1; // Change to 1 JK, goes to hard fault if its 0

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 64000000UL;
	me->sysclk_Ctor = 64000000UL;
	me->hclk_Ctor = 64000000UL;
	me->pclk1Apb1_Ctor = 32000000UL;
	me->pclk2Apb2_Ctor = 64000000UL;
	me->pclk1Apb1Tim_Ctor = 64000000UL;
	me->pclk2Apb2Tim_Ctor = 64000000UL;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz(AlxClk* me)
{
	// #1 Set power regulator
	me->pwrRegVoltageScale = ALX_NULL;
	me->isPwrRegOverDrive = ALX_NULL;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;	// copied from cube
	me->iosc.HSEState = RCC_HSE_ON;						// copied from cube
	me->iosc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;		// copied from cube
	me->iosc.PLL.PLLState = RCC_PLL_ON;				// same as cube and alx
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;		// same as cube and alx
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL8;				// same as cube and alx

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; // same as cube and alx
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;	// same as cube, not sure??
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;	// same as cube
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV2;	// same as cube
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;	// same as cube

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_1;		// Cube set this to 0, did not work. Googling suggested this is a bug with cube and this should larger than 0. 1 Worked fine.

	// #5 Set expected clocks
	me->systemCoreClock_Ctor =	64000000UL;		// ??
	me->sysclk_Ctor =			64000000UL;		// 64 MHz OK - same as cube
	me->hclk_Ctor =				64000000UL;		// 64 MHz OK - same as cube
	me->pclk1Apb1_Ctor =		32000000UL;		// 32 MHz OK - same as cube
	me->pclk2Apb2_Ctor =		64000000UL;		// 64 MHz OK - same as cube
	me->pclk1Apb1Tim_Ctor =		64000000UL;		// 64 MHz OK - same as cube
	me->pclk2Apb2Tim_Ctor =		64000000UL;		// 64 MHz OK - same as cube
	me->mainPllInputClk_Ctor =	8000000UL;		// 8MHz
}
#endif
#if defined(ALX_STM32F4)
static void AlxClk_Ctor_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLM = ALX_NULL;
	me->iosc.PLL.PLLN = ALX_NULL;
	me->iosc.PLL.PLLP = ALX_NULL;
	me->iosc.PLL.PLLQ = ALX_NULL;
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = ALX_NULL;
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 16000000;
	me->sysclk_Ctor = 16000000;
	me->hclk_Ctor = 16000000;
	me->pclk1Apb1_Ctor = 16000000;
	me->pclk2Apb2_Ctor = 16000000;
	me->pclk1Apb1Tim_Ctor = 16000000;
	me->pclk2Apb2Tim_Ctor = 16000000;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz(AlxClk* me)
{
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
		defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = 8;	// VCO frequency = 2MHz, it must be between 1MHz and 2MHz, recommended is 2MHz to limit PLL jitter
	me->iosc.PLL.PLLN = 180;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 15;	// TV: TODO
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;	// TV: TODO
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// Set expected clocks
	me->systemCoreClock_Ctor = 180000000;
	me->sysclk_Ctor = 180000000;
	me->hclk_Ctor = 180000000;
	me->pclk1Apb1_Ctor = 45000000;
	me->pclk2Apb2_Ctor = 90000000;
	me->pclk1Apb1Tim_Ctor = 45000000 * 2;
	me->pclk2Apb2Tim_Ctor = 90000000 * 2;
	me->mainPllInputClk_Ctor = HSI_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false);	// We shouldn't get here
	#endif
}
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz(AlxClk* me)
{
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
	defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 6;	// VCO frequency = 2MHz, it must be between 1MHz and 2MHz, recommended is 2MHz to limit PLL jitter
	me->iosc.PLL.PLLN = 180;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 15;	// TV: TODO
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;	// TV: TODO
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// Set expected clocks
	me->systemCoreClock_Ctor = 180000000;
	me->sysclk_Ctor = 180000000;
	me->hclk_Ctor = 180000000;
	me->pclk1Apb1_Ctor = 45000000;
	me->pclk2Apb2_Ctor = 90000000;
	me->pclk1Apb1Tim_Ctor = 45000000 * 2;
	me->pclk2Apb2Tim_Ctor = 90000000 * 2;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false);	// We shouldn't get here
	#endif
}
static void AlxClk_Ctor_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz(AlxClk* me)
{
	#if defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
	defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx)
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 25;	// VCO frequency = 1MHz, it must be between 1MHz and 2MHz, recommended is 2MHz to limit PLL jitter
	me->iosc.PLL.PLLN = 360;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 15;	// TV: TODO
	#if defined(STM32F410Tx) || defined(STM32F410Cx) || defined(STM32F410Rx) || defined(STM32F446xx) || defined(STM32F469xx) ||\
		defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) ||\
		defined(STM32F413xx) || defined(STM32F423xx)
	me->iosc.PLL.PLLR = 0;	// TV: TODO
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// Set expected clocks
	me->systemCoreClock_Ctor = 180000000;
	me->sysclk_Ctor = 180000000;
	me->hclk_Ctor = 180000000;
	me->pclk1Apb1_Ctor = 45000000;
	me->pclk2Apb2_Ctor = 90000000;
	me->pclk1Apb1Tim_Ctor = 45000000 * 2;
	me->pclk2Apb2Tim_Ctor = 90000000 * 2;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false);	// We shouldn't get here
	#endif
}
#endif
#if defined(ALX_STM32F7)
static void AlxClk_Ctor_McuStm32F7_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLM = ALX_NULL;
	me->iosc.PLL.PLLN = ALX_NULL;
	me->iosc.PLL.PLLP = ALX_NULL;
	me->iosc.PLL.PLLQ = ALX_NULL;
	#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
	me->iosc.PLL.PLLR = ALX_NULL;
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 16000000;
	me->sysclk_Ctor = 16000000;
	me->hclk_Ctor = 16000000;
	me->pclk1Apb1_Ctor = 16000000;
	me->pclk2Apb2_Ctor = 16000000;
	me->pclk1Apb1Tim_Ctor = 16000000;
	me->pclk2Apb2Tim_Ctor = 16000000;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hsi_16MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = 8;	// VCO frequency = 2MHz, it must be between 1MHz and 2MHz, recommended is 2MHz to limit PLL jitter
	me->iosc.PLL.PLLN = 216;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 9;	// PLLQ frequency = 48MHz, it clocks USB_OTG_FS, SDMMC, RNG - USB_OTG_FS needs exactly 48MHz to work, SDMMC and RNG need 48MHz or lower
	#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
	me->iosc.PLL.PLLR = 2;	// Default
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_7;

	// Set expected clocks
	me->systemCoreClock_Ctor = 216000000;
	me->sysclk_Ctor = 216000000;
	me->hclk_Ctor = 216000000;
	me->pclk1Apb1_Ctor = 54000000;
	me->pclk2Apb2_Ctor = 108000000;
	me->pclk1Apb1Tim_Ctor = 54000000 * 2;
	me->pclk2Apb2Tim_Ctor = 108000000 * 2;
	me->mainPllInputClk_Ctor = HSI_VALUE / me->iosc.PLL.PLLM;
}
static void AlxClk_Ctor_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hse_25MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = true;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 25;	// VCO frequency = 1MHz, it must be between 1MHz and 2MHz, recommended is 2MHz to limit PLL jitter
	me->iosc.PLL.PLLN = 432;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = 9;	// PLLQ frequency = 48MHz, it clocks USB_OTG_FS, SDMMC, RNG - USB_OTG_FS needs exactly 48MHz to work, SDMMC and RNG need 48MHz or lower
	#if defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
	me->iosc.PLL.PLLR = 2;	// Default
	#endif

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV4;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV2;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_7;

	// Set expected clocks
	me->systemCoreClock_Ctor = 216000000;
	me->sysclk_Ctor = 216000000;
	me->hclk_Ctor = 216000000;
	me->pclk1Apb1_Ctor = 54000000;
	me->pclk2Apb2_Ctor = 108000000;
	me->pclk1Apb1Tim_Ctor = 54000000 * 2;
	me->pclk2Apb2Tim_Ctor = 108000000 * 2;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
}
#endif
#if defined(ALX_STM32G4)
static void AlxClk_Ctor_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_NONE;
	me->iosc.PLL.PLLM = 0;
	me->iosc.PLL.PLLN = 0;
	me->iosc.PLL.PLLP = 0;
	me->iosc.PLL.PLLQ = 0;
	me->iosc.PLL.PLLR = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 16000000UL;
	me->sysclk_Ctor = 16000000UL;
	me->hclk_Ctor = 16000000UL;
	me->pclk1Apb1_Ctor = 16000000UL;
	me->pclk2Apb2_Ctor = 16000000UL;
	me->pclk1Apb1Tim_Ctor = 16000000UL;
	me->pclk2Apb2Tim_Ctor = 16000000UL;
}
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1_BOOST;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = RCC_PLLM_DIV4;
	me->iosc.PLL.PLLN = 85;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLQ_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLR_DIV2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 170000000UL;
	me->sysclk_Ctor = 170000000UL;
	me->hclk_Ctor = 170000000UL;
	me->pclk1Apb1_Ctor = 170000000UL;
	me->pclk2Apb2_Ctor = 170000000UL;
	me->pclk1Apb1Tim_Ctor = 170000000UL;
	me->pclk2Apb2Tim_Ctor = 170000000UL;
}
static void AlxClk_Ctor_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1_BOOST;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = RCC_PLLM_DIV3;
	me->iosc.PLL.PLLN = 85;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLQ_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLR_DIV2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 170000000UL;
	me->sysclk_Ctor = 170000000UL;
	me->hclk_Ctor = 170000000UL;
	me->pclk1Apb1_Ctor = 170000000UL;
	me->pclk2Apb2_Ctor = 170000000UL;
	me->pclk1Apb1Tim_Ctor = 170000000UL;
	me->pclk2Apb2Tim_Ctor = 170000000UL;
}
#endif
#if defined(ALX_STM32L0)
static void AlxClk_Ctor_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE3;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_3;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLMUL = 0;
	me->iosc.PLL.PLLDIV = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 2097152UL;
	me->sysclk_Ctor = 2097152UL;
	me->hclk_Ctor = 2097152UL;
	me->pclk1Apb1_Ctor = 2097152UL;
	me->pclk2Apb2_Ctor = 2097152UL;
	me->pclk1Apb1Tim_Ctor = 2097152UL;
	me->pclk2Apb2Tim_Ctor = 2097152UL;
}
static void AlxClk_Ctor_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE3;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_5;
	me->iosc.PLL.PLLState = RCC_PLL_OFF;
	me->iosc.PLL.PLLSource = 0;
	me->iosc.PLL.PLLMUL = 0;
	me->iosc.PLL.PLLDIV = 0;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 2097152UL;
	me->sysclk_Ctor = 2097152UL;
	me->hclk_Ctor = 2097152UL;
	me->pclk1Apb1_Ctor = 2097152UL;
	me->pclk2Apb2_Ctor = 2097152UL;
	me->pclk1Apb1Tim_Ctor = 2097152UL;
	me->pclk2Apb2Tim_Ctor = 2097152UL;
}
static void AlxClk_Ctor_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz(AlxClk* me)
{
	// #1 Set voltage regulator scale
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = false;

	// #2 Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_5;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLMUL = RCC_PLL_MUL4;
	me->iosc.PLL.PLLDIV = RCC_PLL_DIV2;

	// #3 Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// #4 Set flash latency
	me->flashLatency = FLASH_LATENCY_1;

	// #5 Set expected clocks
	me->systemCoreClock_Ctor = 32000000UL;
	me->sysclk_Ctor = 32000000UL;
	me->hclk_Ctor = 32000000UL;
	me->pclk1Apb1_Ctor = 32000000UL;
	me->pclk2Apb2_Ctor = 32000000UL;
	me->pclk1Apb1Tim_Ctor = 32000000UL;
	me->pclk2Apb2Tim_Ctor = 32000000UL;
}
#endif
#if defined(ALX_STM32L4)
static void AlxClk_Ctor_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLM = ALX_NULL;
	me->iosc.PLL.PLLN = ALX_NULL;
	me->iosc.PLL.PLLP = ALX_NULL;
	me->iosc.PLL.PLLQ = ALX_NULL;
	me->iosc.PLL.PLLR = ALX_NULL;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 4000000;
	me->sysclk_Ctor = 4000000;
	me->hclk_Ctor = 4000000;
	me->pclk1Apb1_Ctor = 4000000;
	me->pclk2Apb2_Ctor = 4000000;
	me->pclk1Apb1Tim_Ctor = 4000000;
	me->pclk2Apb2Tim_Ctor = 4000000;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = ALX_NULL;
	me->iosc.PLL.PLLM = ALX_NULL;
	me->iosc.PLL.PLLN = ALX_NULL;
	me->iosc.PLL.PLLP = ALX_NULL;
	me->iosc.PLL.PLLQ = ALX_NULL;
	me->iosc.PLL.PLLR = ALX_NULL;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 16000000;
	me->sysclk_Ctor = 16000000;
	me->hclk_Ctor = 16000000;
	me->pclk1Apb1_Ctor = 16000000;
	me->pclk2Apb2_Ctor = 16000000;
	me->pclk1Apb1Tim_Ctor = 16000000;
	me->pclk2Apb2Tim_Ctor = 16000000;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hsi_16MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = 2;
	me->iosc.PLL.PLLN = 20;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLP_DIV2;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// Set expected clocks
	me->systemCoreClock_Ctor = 80000000;
	me->sysclk_Ctor = 80000000;
	me->hclk_Ctor = 80000000;
	me->pclk1Apb1_Ctor = 80000000;
	me->pclk2Apb2_Ctor = 80000000;
	me->pclk1Apb1Tim_Ctor = 80000000;
	me->pclk2Apb2Tim_Ctor = 80000000;
	me->mainPllInputClk_Ctor = HSI_VALUE / me->iosc.PLL.PLLM;
}
static void AlxClk_Ctor_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 1;
	me->iosc.PLL.PLLN = 20;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLP_DIV2;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// Set expected clocks
	me->systemCoreClock_Ctor = 80000000;
	me->sysclk_Ctor = 80000000;
	me->hclk_Ctor = 80000000;
	me->pclk1Apb1_Ctor = 80000000;
	me->pclk2Apb2_Ctor = 80000000;
	me->pclk1Apb1Tim_Ctor = 80000000;
	me->pclk2Apb2Tim_Ctor = 80000000;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
}
static void AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hsi_16MHz(AlxClk* me)
{
	#if defined(PWR_CR5_R1MODE)
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1_BOOST;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = 2;
	me->iosc.PLL.PLLN = 30;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLP_DIV2;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// Set expected clocks
	me->systemCoreClock_Ctor = 120000000;
	me->sysclk_Ctor = 120000000;
	me->hclk_Ctor = 120000000;
	me->pclk1Apb1_Ctor = 120000000;
	me->pclk2Apb2_Ctor = 120000000;
	me->pclk1Apb1Tim_Ctor = 120000000;
	me->pclk2Apb2Tim_Ctor = 120000000;
	me->mainPllInputClk_Ctor = HSI_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false);
	#endif
}
static void AlxClk_Ctor_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_25MHz(AlxClk* me)
{
	#if defined(PWR_CR5_R1MODE)
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1_BOOST;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_6;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 5;
	me->iosc.PLL.PLLN = 48;
	me->iosc.PLL.PLLP = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLQ = RCC_PLLP_DIV2;
	me->iosc.PLL.PLLR = RCC_PLLP_DIV2;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_5;

	// Set expected clocks
	me->systemCoreClock_Ctor = 120000000;
	me->sysclk_Ctor = 120000000;
	me->hclk_Ctor = 120000000;
	me->pclk1Apb1_Ctor = 120000000;
	me->pclk2Apb2_Ctor = 120000000;
	me->pclk1Apb1Tim_Ctor = 120000000;
	me->pclk2Apb2Tim_Ctor = 120000000;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
	#else
	ALX_CLK_ASSERT(false);
	#endif
}
#endif
#if defined(ALX_STM32U5)
static void AlxClk_Ctor_McuStm32U5_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Pclk3Apb3_4MHz_Msi_4MHz_Default(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.LSIDiv = RCC_LSI_DIV1;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_4;
	me->iosc.MSIKClockRange = RCC_MSIKRANGE_4;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.SHSIState = RCC_SHSI_OFF;
	me->iosc.MSIKState = RCC_MSIK_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_NONE;
	me->iosc.PLL.PLLM = 0;
	me->iosc.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
	me->iosc.PLL.PLLN = 0;
	me->iosc.PLL.PLLP = 0;
	me->iosc.PLL.PLLQ = 0;
	me->iosc.PLL.PLLR = 0;
	me->iosc.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
	me->iosc.PLL.PLLFRACN = 0;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB3CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 4000000;
	me->sysclk_Ctor = 4000000;
	me->hclk_Ctor = 4000000;
	me->pclk1Apb1_Ctor = 4000000;
	me->pclk2Apb2_Ctor = 4000000;
	me->pclk3Apb3_Ctor = 4000000;
	me->pclk1Apb1Tim_Ctor = 4000000;
	me->pclk2Apb2Tim_Ctor = 4000000;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32U5_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Pclk3Apb3_16MHz_Hsi_16MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.LSIDiv = RCC_LSI_DIV1;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_4;
	me->iosc.MSIKClockRange = RCC_MSIKRANGE_4;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.SHSIState = RCC_SHSI_OFF;
	me->iosc.MSIKState = RCC_MSIK_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_NONE;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_NONE;
	me->iosc.PLL.PLLM = 0;
	me->iosc.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
	me->iosc.PLL.PLLN = 0;
	me->iosc.PLL.PLLP = 0;
	me->iosc.PLL.PLLQ = 0;
	me->iosc.PLL.PLLR = 0;
	me->iosc.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
	me->iosc.PLL.PLLFRACN = 0;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB3CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_0;

	// Set expected clocks
	me->systemCoreClock_Ctor = 16000000;
	me->sysclk_Ctor = 16000000;
	me->hclk_Ctor = 16000000;
	me->pclk1Apb1_Ctor = 16000000;
	me->pclk2Apb2_Ctor = 16000000;
	me->pclk3Apb3_Ctor = 16000000;
	me->pclk1Apb1Tim_Ctor = 16000000;
	me->pclk2Apb2Tim_Ctor = 16000000;
	me->mainPllInputClk_Ctor = 0;
}
static void AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hsi_16MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	me->iosc.HSEState = RCC_HSE_OFF;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_ON;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.LSIDiv = RCC_LSI_DIV1;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_4;
	me->iosc.MSIKClockRange = RCC_MSIKRANGE_4;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.SHSIState = RCC_SHSI_OFF;
	me->iosc.MSIKState = RCC_MSIK_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	me->iosc.PLL.PLLM = 2;
	me->iosc.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
	me->iosc.PLL.PLLN = 40;
	me->iosc.PLL.PLLP = 2;
	me->iosc.PLL.PLLQ = 2;
	me->iosc.PLL.PLLR = 2;
	me->iosc.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
	me->iosc.PLL.PLLFRACN = 0;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB3CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// Set expected clocks
	me->systemCoreClock_Ctor = 160000000;
	me->sysclk_Ctor = 160000000;
	me->hclk_Ctor = 160000000;
	me->pclk1Apb1_Ctor = 160000000;
	me->pclk2Apb2_Ctor = 160000000;
	me->pclk3Apb3_Ctor = 160000000;
	me->pclk1Apb1Tim_Ctor = 160000000;
	me->pclk2Apb2Tim_Ctor = 160000000;
	me->mainPllInputClk_Ctor = HSI_VALUE / me->iosc.PLL.PLLM;
}
static void AlxClk_Ctor_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz(AlxClk* me)
{
	// Set power regulator
	me->pwrRegVoltageScale = PWR_REGULATOR_VOLTAGE_SCALE1;
	me->isPwrRegOverDrive = ALX_NULL;

	// Set oscillators
	me->iosc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	me->iosc.HSEState = RCC_HSE_ON;
	me->iosc.LSEState = RCC_LSE_OFF;
	me->iosc.HSIState = RCC_HSI_OFF;
	me->iosc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	me->iosc.LSIState = RCC_LSI_OFF;
	me->iosc.LSIDiv = RCC_LSI_DIV1;
	me->iosc.MSIState = RCC_MSI_ON;
	me->iosc.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	me->iosc.MSIClockRange = RCC_MSIRANGE_4;
	me->iosc.MSIKClockRange = RCC_MSIKRANGE_4;
	me->iosc.HSI48State = RCC_HSI48_OFF;
	me->iosc.SHSIState = RCC_SHSI_OFF;
	me->iosc.MSIKState = RCC_MSIK_OFF;
	me->iosc.PLL.PLLState = RCC_PLL_ON;
	me->iosc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	me->iosc.PLL.PLLM = 3;
	me->iosc.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
	me->iosc.PLL.PLLN = 80;
	me->iosc.PLL.PLLP = 2;
	me->iosc.PLL.PLLQ = 2;
	me->iosc.PLL.PLLR = 2;
	me->iosc.PLL.PLLRGE = RCC_PLLVCIRANGE_0;
	me->iosc.PLL.PLLFRACN = 0;

	// Set clocks
	me->iclk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	me->iclk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	me->iclk.AHBCLKDivider = RCC_SYSCLK_DIV1;
	me->iclk.APB1CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB2CLKDivider = RCC_HCLK_DIV1;
	me->iclk.APB3CLKDivider = RCC_HCLK_DIV1;

	// Set flash latency
	me->flashLatency = FLASH_LATENCY_4;

	// Set expected clocks
	me->systemCoreClock_Ctor = 160000000;
	me->sysclk_Ctor = 160000000;
	me->hclk_Ctor = 160000000;
	me->pclk1Apb1_Ctor = 160000000;
	me->pclk2Apb2_Ctor = 160000000;
	me->pclk3Apb3_Ctor = 160000000;
	me->pclk1Apb1Tim_Ctor = 160000000;
	me->pclk2Apb2Tim_Ctor = 160000000;
	me->mainPllInputClk_Ctor = HSE_VALUE / me->iosc.PLL.PLLM;
}
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxCan_Ctor_IsClkOk(AlxClk* me)
{
	// Prepare
	me->systemCoreClock = SystemCoreClock;
	me->sysclk = HAL_RCC_GetSysClockFreq();
	me->hclk = HAL_RCC_GetHCLKFreq();
	me->pclk1Apb1 = HAL_RCC_GetPCLK1Freq();
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	me->pclk2Apb2 = HAL_RCC_GetPCLK2Freq();
	#endif
	#if defined(ALX_STM32U5)
	me->pclk3Apb3 = HAL_RCC_GetPCLK3Freq();
	#endif

	// Check
	if (SystemCoreClock != me->systemCoreClock_Ctor)	{ ALX_CLK_TRACE("Err");	return false; }
	if (me->sysclk != me->sysclk_Ctor)					{ ALX_CLK_TRACE("Err");	return false; }
	if (me->hclk != me->hclk_Ctor)						{ ALX_CLK_TRACE("Err");	return false; }
	if (me->pclk1Apb1 != me->pclk1Apb1_Ctor)			{ ALX_CLK_TRACE("Err");	return false; }
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	if (me->pclk2Apb2 != me->pclk2Apb2_Ctor)			{ ALX_CLK_TRACE("Err");	return false; }
	#endif
	#if defined(ALX_STM32U5)
	if (me->pclk3Apb3 != me->pclk3Apb3_Ctor)			{ ALX_CLK_TRACE("Err");	return false; }
	#endif

	// Return
	return true;
}
static void AlxClk_Periph_Gpio_EnableClk(AlxClk* me)
{
	#if defined(GPIOA)
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#if defined(GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#if defined(GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
	#if defined(GPIOD)
	__HAL_RCC_GPIOD_CLK_ENABLE();
	#endif
	#if defined(GPIOE)
	__HAL_RCC_GPIOE_CLK_ENABLE();
	#endif
	#if defined(GPIOF)
	__HAL_RCC_GPIOF_CLK_ENABLE();
	#endif
	#if defined(GPIOG)
	__HAL_RCC_GPIOG_CLK_ENABLE();
	#endif
	#if defined(GPIOH)
	__HAL_RCC_GPIOH_CLK_ENABLE();
	#endif
	#if defined(GPIOI)
	__HAL_RCC_GPIOI_CLK_ENABLE();
	#endif
	#if defined(GPIOJ)
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	#endif
	#if defined(GPIOK)
	__HAL_RCC_GPIOK_CLK_ENABLE();
	#endif
	#if defined(GPIOL)
	__HAL_RCC_GPIOL_CLK_ENABLE();
	#endif
	#if defined(GPIOM)
	__HAL_RCC_GPIOM_CLK_ENABLE();
	#endif
	#if defined(GPION)
	__HAL_RCC_GPION_CLK_ENABLE();
	#endif
}


//******************************************************************************
// Callback Functions
//******************************************************************************
void HAL_RCC_CSSCallback()
{
	HAL_NVIC_SystemReset();
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))
