/**
  ******************************************************************************
  * @file		alxGlobal_McuStm32G4.h
  * @brief		Auralix C Library - ALX Global MCU STM32G4 File
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
#ifndef ALX_GLOBAL_MCU_STM32G4_H
#define ALX_GLOBAL_MCU_STM32G4_H

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
#if defined(ALX_C_LIB) && defined(ALX_STM32G4)


//******************************************************************************
// Includes
//******************************************************************************
#include "stm32g4xx.h"
#include "stm32g4xx_hal.h"
#include "stm32_hal_legacy.h"
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_comp.h"
#include "stm32g4xx_ll_cordic.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_crc.h"
#include "stm32g4xx_ll_crs.h"
#include "stm32g4xx_ll_dac.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_dmamux.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_fmac.h"
#include "stm32g4xx_ll_fmc.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_hrtim.h"
#include "stm32g4xx_ll_i2c.h"
#include "stm32g4xx_ll_iwdg.h"
#include "stm32g4xx_ll_lptim.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_opamp.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_rng.h"
#include "stm32g4xx_ll_rtc.h"
#include "stm32g4xx_ll_spi.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_tim.h"
#include "stm32g4xx_ll_ucpd.h"
#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_usb.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_wwdg.h"
#include "system_stm32g4xx.h"
#include "tz_context.h"


//******************************************************************************
// Preprocessor
//******************************************************************************

//------------------------------------------------------------------------------
// ALX MCU - I2C
//------------------------------------------------------------------------------
#define ALX_I2C_MCU_STM32_CLK_100kHz 0				// TODO
#define ALX_I2C_MCU_STM32_CLK_400kHz 0x40B285C2		// _400kHz_8MHz_100nsRise_10nsFall_AnalogFiltOn_DigitalFilt0, on STM32F3 -> Used I2C timing configuration tool (AN4235) and the associated software STSW-STM32126, measured 380kHz, if analog filter is disabled we can get 400kHz..
													// 0x60400D28 _400kHz_8MHz_100nsRise_10nsFall_AnalogFiltOn_DigitalFilt0
													// 0x40B285C2 _100kHz_8MHz_100nsRise_100nsFall_AnalogFiltOn_DigitalFilt0


//------------------------------------------------------------------------------
// ALX - ID
//------------------------------------------------------------------------------
#define ALX_STM32_CMSIS_VERSION_MAIN	__STM32G4_CMSIS_VERSION_MAIN
#define ALX_STM32_CMSIS_VERSION_SUB1	__STM32G4_CMSIS_VERSION_SUB1
#define ALX_STM32_CMSIS_VERSION_SUB2	__STM32G4_CMSIS_VERSION_SUB2
#define ALX_STM32_CMSIS_VERSION_RC		__STM32G4_CMSIS_VERSION_RC
#define ALX_STM32_CMSIS_VERSION			__STM32G4_CMSIS_VERSION
#define ALX_STM32_CMSIS_VER_REQUIRED	ALX_STM32G4_CMSIS_VER_REQUIRED
#define ALX_STM32_HAL_VER_REQUIRED		ALX_STM32G4_HAL_VER_REQUIRED
#define ALX_STM32_STR "STM32G4"


#endif	// #if defined(ALX_C_LIB) && defined(ALX_STM32G4)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_MCU_STM32G4_H
