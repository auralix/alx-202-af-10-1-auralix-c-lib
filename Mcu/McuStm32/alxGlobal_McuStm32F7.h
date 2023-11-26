/**
  ******************************************************************************
  * @file		alxGlobal_McuStm32F7.h
  * @brief		Auralix C Library - ALX Global MCU STM32F7 File
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
#ifndef ALX_GLOBAL_MCU_STM32F7_H
#define ALX_GLOBAL_MCU_STM32F7_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_STM32F7)


//******************************************************************************
// Includes
//******************************************************************************
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32_hal_legacy.h"
#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_crc.h"
#include "stm32f7xx_ll_dac.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_dma2d.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_fmc.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_i2c.h"
#include "stm32f7xx_ll_iwdg.h"
#include "stm32f7xx_ll_lptim.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_rng.h"
#include "stm32f7xx_ll_rtc.h"
#include "stm32f7xx_ll_sdmmc.h"
#include "stm32f7xx_ll_spi.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_tim.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_usb.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_wwdg.h"
#include "system_stm32f7xx.h"


//******************************************************************************
// Preprocessor
//******************************************************************************


//------------------------------------------------------------------------------
// ALX - MCU - ADC
//------------------------------------------------------------------------------
#define __HAL_RCC_ADC1_FORCE_RESET		__HAL_RCC_ADC_FORCE_RESET
#define __HAL_RCC_ADC2_FORCE_RESET		__HAL_RCC_ADC_FORCE_RESET
#define __HAL_RCC_ADC3_FORCE_RESET		__HAL_RCC_ADC_FORCE_RESET

#define __HAL_RCC_ADC1_RELEASE_RESET	__HAL_RCC_ADC_RELEASE_RESET
#define __HAL_RCC_ADC2_RELEASE_RESET	__HAL_RCC_ADC_RELEASE_RESET
#define __HAL_RCC_ADC3_RELEASE_RESET	__HAL_RCC_ADC_RELEASE_RESET


//------------------------------------------------------------------------------
// ALX - ID
//------------------------------------------------------------------------------
#define ALX_STM32_CMSIS_VERSION_MAIN	__STM32F7_CMSIS_VERSION_MAIN
#define ALX_STM32_CMSIS_VERSION_SUB1	__STM32F7_CMSIS_VERSION_SUB1
#define ALX_STM32_CMSIS_VERSION_SUB2	__STM32F7_CMSIS_VERSION_SUB2
#define ALX_STM32_CMSIS_VERSION_RC		__STM32F7_CMSIS_VERSION_RC
#define ALX_STM32_CMSIS_VERSION			__STM32F7_CMSIS_VERSION
#define ALX_STM32_CMSIS_VER_REQUIRED	ALX_STM32F7_CMSIS_VER_REQUIRED
#define ALX_STM32_HAL_VER_REQUIRED		ALX_STM32F7_HAL_VER_REQUIRED
#define ALX_STM32_STR "STM32F7"


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32F7)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_MCU_STM32F7_H
