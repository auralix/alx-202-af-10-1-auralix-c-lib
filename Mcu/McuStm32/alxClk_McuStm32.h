﻿/**
  ******************************************************************************
  * @file		alxClk_McuStm32.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_CLK_MCU_STM32_H
#define ALX_CLK_MCU_STM32_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_STM32F0)
	AlxClk_Config_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hsi_8MHz_Default,
	AlxClk_Config_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hse_8MHz,
	AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz,
	AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz,
	AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz_BackupHsi_8MHz
	#endif
	#if defined(ALX_STM32F1)
	AlxClk_Config_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8MHz,
	#endif
	#if defined(ALX_STM32F4)
	AlxClk_Config_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16MHz,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz,
	#endif
	#if defined(ALX_STM32F7)
	AlxClk_Config_McuStm32F7_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default,
	AlxClk_Config_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hse_12MHz,
	AlxClk_Config_McuStm32F7_Sysclk_216MHz_Pclk1Apb1_54MHz_Pclk2Apb2_108MHz_Hse_25MHz,
	#endif
	#if defined(ALX_STM32G4)
	AlxClk_Config_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz_BackupHsi_16MHz,
	#endif
	#if defined(ALX_STM32L0)
	AlxClk_Config_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default,
	AlxClk_Config_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1,
	#endif
	#if defined(ALX_STM32L4)
	AlxClk_Config_McuStm32L4_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Msi_4MHz_Default,
	AlxClk_Config_McuStm32L4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz,
	AlxClk_Config_McuStm32L4_Sysclk_80MHz_Pclk1Apb1_80MHz_Pclk2Apb2_80MHz_Hse_8MHz_BackupHsi_16MHz,
	AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_25MHz,
	AlxClk_Config_McuStm32L4_Sysclk_120MHz_Pclk1Apb1_120MHz_Pclk2Apb2_120MHz_Hse_12MHz_BackupHsi_16MHz,
	#endif
	#if defined(ALX_STM32U5)
	AlxClk_Config_McuStm32U5_Sysclk_4MHz_Pclk1Apb1_4MHz_Pclk2Apb2_4MHz_Pclk3Apb3_4MHz_Msi_4MHz_Default,
	AlxClk_Config_McuStm32U5_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Pclk3Apb3_16MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz,
	AlxClk_Config_McuStm32U5_Sysclk_160MHz_Pclk1Apb1_160MHz_Pclk2Apb2_160MHz_Pclk3Apb3_160MHz_Hse_12MHz_BackupHsi_16MHz
	#endif
} AlxClk_Config;

typedef struct
{
	// Parameters
	AlxClk_Config config;

	// Variables
	uint32_t pwrRegVoltageScale;
	bool isPwrRegOverDrive;
	RCC_OscInitTypeDef iosc;
	RCC_ClkInitTypeDef iclk;
	uint32_t flashLatency;
	bool backupHsiEnable;
	bool isBackupHsiUsed;

	uint32_t systemCoreClock;
	uint32_t sysclk;
	uint32_t hclk;
	uint32_t pclk1Apb1;
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	uint32_t pclk2Apb2;
	#endif
	#if defined(ALX_STM32U5)
	uint32_t pclk3Apb3;
	#endif

	uint32_t systemCoreClock_Ctor;
	uint32_t sysclk_Ctor;
	uint32_t hclk_Ctor;
	uint32_t pclk1Apb1_Ctor;
	#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	uint32_t pclk2Apb2_Ctor;
	#endif
	#if defined(ALX_STM32U5)
	uint32_t pclk3Apb3_Ctor;
	#endif
	uint32_t pclk1Apb1Tim_Ctor;
	uint32_t pclk2Apb2Tim_Ctor;
	uint32_t mainPllInputClk_Ctor;
	#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
	uint32_t pllP_Ctor;
	uint32_t pllQ_Ctor;
	#endif

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CLK_MCU_STM32_H
