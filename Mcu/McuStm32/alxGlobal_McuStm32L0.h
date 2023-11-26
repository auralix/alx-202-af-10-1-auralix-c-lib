/**
  ******************************************************************************
  * @file		alxGlobal_McuStm32L0.h
  * @brief		Auralix C Library - ALX Global MCU STM32L0 File
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
#ifndef ALX_GLOBAL_MCU_STM32L0_H
#define ALX_GLOBAL_MCU_STM32L0_H

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
#if defined(ALX_C_LIB) && defined(ALX_STM32L0)


//******************************************************************************
// Includes
//******************************************************************************
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"
#include "stm32_hal_legacy.h"
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_comp.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_crc.h"
#include "stm32l0xx_ll_crs.h"
#include "stm32l0xx_ll_dac.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_i2c.h"
#include "stm32l0xx_ll_iwdg.h"
#include "stm32l0xx_ll_lptim.h"
#include "stm32l0xx_ll_lpuart.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_rng.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_tim.h"
#include "stm32l0xx_ll_usart.h"
#include "stm32l0xx_ll_usb.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_wwdg.h"
#include "system_stm32l0xx.h"


//******************************************************************************
// Preprocessor
//******************************************************************************


//------------------------------------------------------------------------------
// ALX - MCU - DAC
//------------------------------------------------------------------------------
#define __HAL_RCC_DAC1_CLK_ENABLE		__HAL_RCC_DAC_CLK_ENABLE
#define __HAL_RCC_DAC1_CLK_DISABLE		__HAL_RCC_DAC_CLK_DISABLE
#define __HAL_RCC_DAC1_FORCE_RESET		__HAL_RCC_DAC_FORCE_RESET
#define __HAL_RCC_DAC1_RELEASE_RESET	__HAL_RCC_DAC_RELEASE_RESET


//------------------------------------------------------------------------------
// ALX - ID
//------------------------------------------------------------------------------
#define ALX_STM32_CMSIS_VERSION_MAIN	__STM32L0xx_CMSIS_VERSION_MAIN
#define ALX_STM32_CMSIS_VERSION_SUB1	__STM32L0xx_CMSIS_VERSION_SUB1
#define ALX_STM32_CMSIS_VERSION_SUB2	__STM32L0xx_CMSIS_VERSION_SUB2
#define ALX_STM32_CMSIS_VERSION_RC		__STM32L0xx_CMSIS_VERSION_RC
#define ALX_STM32_CMSIS_VERSION			__STM32L0xx_CMSIS_VERSION
#define ALX_STM32_CMSIS_VER_REQUIRED	ALX_STM32L0_CMSIS_VER_REQUIRED
#define ALX_STM32_HAL_VER_REQUIRED		ALX_STM32L0_HAL_VER_REQUIRED
#define ALX_STM32_STR "STM32L0"


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32L0)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_MCU_STM32L0_H
