/**
  ******************************************************************************
  * @file		alxGlobal_McuStm32F4.h
  * @brief		Auralix C Library - ALX Global MCU STM32F4 File
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
#ifndef ALX_GLOBAL_MCU_STM32F4_H
#define ALX_GLOBAL_MCU_STM32F4_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_STM32F4)


//******************************************************************************
// Includes
//******************************************************************************
#include <stm32f4xx_hal.h>
#include <stm32_hal_legacy.h>

#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"
//#include "stm32f4xx_ll_comp.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_crc.h"
//#include "stm32f4xx_ll_crs.h"
#include "stm32f4xx_ll_dac.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_lptim.h"
//#include "stm32f4xx_ll_lpuart.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_rng.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_system.h"
//#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_usb.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_wwdg.h"


//******************************************************************************
// Preprocessor
//******************************************************************************

// AlxI2c_McuStm32
#define ALX_I2C_MCU_STM32_CLK_100kHz (AlxI2c_Clk_100kHz * 1000)
#define ALX_I2C_MCU_STM32_CLK_400kHz (AlxI2c_Clk_400kHz * 1000)

// AlxAdc_McuStm32
#define __LL_ADC_CALC_VREFANALOG_VOLTAGE(__VREFINT_ADC_DATA__,__ADC_RESOLUTION__) \
		(((uint32_t)(*VREFINT_CAL_ADDR) * VREFINT_CAL_VREF) / __LL_ADC_CONVERT_DATA_RESOLUTION((__VREFINT_ADC_DATA__),(__ADC_RESOLUTION__),LL_ADC_RESOLUTION_12B))

// AlxDac_McuStm32
#define __HAL_RCC_DAC1_CLK_ENABLE		__HAL_RCC_DAC_CLK_ENABLE
#define __HAL_RCC_DAC1_CLK_DISABLE		__HAL_RCC_DAC_CLK_DISABLE
#define __HAL_RCC_DAC1_FORCE_RESET		__HAL_RCC_DAC_FORCE_RESET
#define __HAL_RCC_DAC1_RELEASE_RESET	__HAL_RCC_DAC_RELEASE_RESET

// AlxId //
#define ALX_STM32_CMSIS_VERSION_MAIN	__STM32F4xx_CMSIS_VERSION_MAIN
#define ALX_STM32_CMSIS_VERSION_SUB1	__STM32F4xx_CMSIS_VERSION_SUB1
#define ALX_STM32_CMSIS_VERSION_SUB2	__STM32F4xx_CMSIS_VERSION_SUB2
#define ALX_STM32_CMSIS_VERSION_RC		__STM32F4xx_CMSIS_VERSION_RC
#define ALX_STM32_CMSIS_VERSION			((__STM32F4xx_CMSIS_VERSION_MAIN << 24)\
										|(__STM32F4xx_CMSIS_VERSION_SUB1 << 16)\
										|(__STM32F4xx_CMSIS_VERSION_SUB2 << 8 )\
										|(__STM32F4xx_CMSIS_VERSION_RC))
#define ALX_STM32_CMSIS_VER_REQUIRED	ALX_STM32F4_CMSIS_VER_REQUIRED
#define ALX_STM32_HAL_VER_REQUIRED		ALX_STM32F4_HAL_VER_REQUIRED
#define ALX_STM32_STR "STM32F4"

#endif

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_GLOBAL_MCU_STM32F4_H
