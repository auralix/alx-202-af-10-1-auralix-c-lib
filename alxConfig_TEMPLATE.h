/**
  ******************************************************************************
  * @file		alxConfig.h
  * @brief		Auralix Library - Configuration File
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


#ifndef ALX_CONFIG_H
#define ALX_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Configuration
//******************************************************************************

// Language C //
#define ALX_LANG_C_VER_MIN_REQUIRED 199901L // C99

// Language C Library //
#define ALX_NEWLIB_NANO
#define ALX_LANG_C_LIB_VER_MIN_REQUIRED 0x02050000 // 2.5.0

// Language C++ //
#define ALX_LANG_CPP_VER_MIN_REQUIRED 201402L // C++14

// Language C++ Library //
#define ALX_GNU_CPP_LIB
#define ALX_LANG_CPP_LIB_VER_MIN_REQUIRED 20170814

// Compiler //
#define ALX_GCC
#define ALX_COMP_VER_MIN_REQUIRED 0x07020000 // 7.2.0

// Platform - ARM CMSIS //
#define ALX_CMSIS_CORE
#define ALX_CMSIS_CORE_VER_REQUIRED 0x00050001 // 5.1
#define ALX_CMSIS_DSP

// Platform - STM32 //
//#define ALX_STM32F4
//#define ALX_STM32F4_CMSIS_VER_REQUIRED 0x02060500 // 2.6.5.0
//#define ALX_STM32F4_HAL_VER_REQUIRED 0x01070A00 // 1.7.10.0

//#define ALX_STM32G4
//#define ALX_STM32G4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32G4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32L0
//#define ALX_STM32L0_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32L0_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

// Platform - LPC8XX //
//#define ALX_LPC8XX
//#define ALX_LPC84X
//#define ALX_LPC80X

// Platform - LPC55XXX //
//#define ALX_LPC55S6X

// Assert //
#define _ALX_ASSERT_BKPT_ALL

// Trace //
#define _ALX_TRACE_ALL

// Optimize //
//#define ALX_OPTIMIZE_SIZE_ALL

// Submodules //
//#define ALX_ADC_MCU
//#define ALX_DAC_MCU
//#define ALX_TEMP_SENS_MCU
//#define ALX_TEMP_SENS_RTD_VDIV

// HW //
//#define ALX_HW_LPC_XPRESSO_55S69_C_TEST
//#define ALX_HW_LPC_XPRESSO_55S69_CPP_TEST
//#define ALX_HW_NFC_WLC_LISTENER_V3_5B_C_TEST
//#define ALX_HW_NFC_WLC_LISTENER_3_5B_CPP_TEST
//#define ALX_HW_NUCLEO_F429ZI_C_TEST
//#define ALX_HW_NUCLEO_F429ZI_CPP_TEST

// Test //
//#define ALX_TEST_GH
//#define ALX_TEST_GK
//#define ALX_TEST_JK
//#define ALX_TEST_JS
//#define ALX_TEST_MF
//#define ALX_TEST_RK
//#define ALX_TEST_TV


#ifdef __cplusplus
}
#endif

#endif // ALX_CONFIG_H
