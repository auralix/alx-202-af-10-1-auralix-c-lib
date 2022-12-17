/**
  ******************************************************************************
  * @file		alxClk_McuStm32.h
  * @brief		Auralix C Library - ALX Clock MCU STM32 Module
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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(STM32F0)
	AlxClk_Config_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hsi_8MHz_Default,
	AlxClk_Config_McuStm32F0_Sysclk_8MHz_Pclk1Apb1_8MHz_Hse_8MHz,
	AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hsi_8MHz,
	AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz,
	AlxClk_Config_McuStm32F0_Sysclk_48MHz_Pclk1Apb1_48MHz_Hse_8MHz_BackupHsi_8Mhz
	#endif
	#if defined(STM32F1)
	AlxClk_Config_McuStm32F1_Sysclk_8MHz_Pclk1Apb1_8MHz_Pclk2Apb2_8MHz_Hsi_8MHz_Default,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hsi_8MHz,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz,
	AlxClk_Config_McuStm32F1_Sysclk_64MHz_Pclk1Apb1_32MHz_Pclk2Apb2_64MHz_Hse_8MHz_BackupHsi_8Mhz,
	#endif
	#if defined(STM32F4)
	AlxClk_Config_McuStm32F4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_12MHz_BackupHsi_16Mhz,
	AlxClk_Config_McuStm32F4_Sysclk_180MHz_Pclk1Apb1_45MHz_Pclk2Apb2_90MHz_Hse_25MHz,
	#endif
	#if defined(STM32G4)
	AlxClk_Config_McuStm32G4_Sysclk_16MHz_Pclk1Apb1_16MHz_Pclk2Apb2_16MHz_Hsi_16MHz_Default,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz,
	AlxClk_Config_McuStm32G4_Sysclk_170MHz_Pclk1Apb1_170MHz_Pclk2Apb2_170MHz_Hse_12MHz_BackupHsi_16Mhz,
	#endif
	#if defined(STM32L0)
	AlxClk_Config_McuStm32L0_Sysclk_2MHz1_Pclk1Apb1_2MHz1_Pclk2Apb2_2MHz1_Msi_2MHz1_Default,
	AlxClk_Config_McuStm32L0_Sysclk_32MHz_Pclk1Apb1_32MHz_Pclk2Apb2_32MHz_Hsi_16MHz,
	AlxClk_Config_McuStm32L0_Sysclk_524kHz_Pclk1Apb1_524kHz_Pclk2Apb2_524kHz_Msi_2MHz1,
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
	uint32_t pclk2Apb2;

	uint32_t systemCoreClock_Ctor;
	uint32_t sysclk_Ctor;
	uint32_t hclk_Ctor;
	uint32_t pclk1Apb1_Ctor;
	uint32_t pclk2Apb2_Ctor;
	uint32_t pclk1Apb1Tim_Ctor;
	uint32_t pclk2Apb2Tim_Ctor;
	uint32_t mainPllInputClk_Ctor;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		config
  */
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CLK_MCU_STM32_H
