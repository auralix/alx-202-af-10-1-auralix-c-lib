/**
  ******************************************************************************
  * @file		alxWdt_McuStm32.h
  * @brief		Auralix C Library - ALX Watchdog Timer MCU STM32 Module
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
#ifndef ALX_WDT_MCU_STM32_H
#define ALX_WDT_MCU_STM32_H

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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxWdt_Config_McuStm32_WdtTimeout_512ms_WdtClk_8kHz_Lsi_32kHz,
	AlxWdt_Config_McuStm32_WdtTimeout_1024ms_WdtClk_4kHz_Lsi_32kHz,
	AlxWdt_Config_McuStm32_WdtTimeout_2048ms_WdtClk_2kHz_Lsi_32kHz,
	AlxWdt_Config_McuStm32_WdtTimeout_4096ms_WdtClk_1kHz_Lsi_32kHz,
	AlxWdt_Config_McuStm32_WdtTimeout_8192ms_WdtClk_500Hz_Lsi_32kHz,
	AlxWdt_Config_McuStm32_WdtTimeout_16384ms_WdtClk_250Hz_Lsi_32kHz,
	AlxWdt_Config_McuStm32_WdtTimeout_32768ms_WdtClk_125Hz_Lsi_32kHz
} AlxWdt_Config;

typedef struct
{
	// Parameters
	AlxWdt_Config config;

	// Variables
	IWDG_HandleTypeDef hiwdg;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxWdt;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxWdt_Ctor
(
	AlxWdt* me,
	AlxWdt_Config config
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_WDT_MCU_STM32_H
