/**
  ******************************************************************************
  * @file		alxConfig.h
  * @brief		Auralix C Library - ALX Configuration File
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
#ifndef ALX_CONFIG_H
#define ALX_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Configuration
//******************************************************************************


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Modules
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// ALX - Global - C/C++
//------------------------------------------------------------------------------
//#define ALX_PC
#define ALX_NEWLIB_NANO
//#define ALX_CMSIS_DSP
//#define ALX_FREE_RTOS
//#define ALX_LW_IP
//#define ALX_LFS
//#define ALX_MBED
//#define ALX_MBED_CLOUD_CLIENT


//------------------------------------------------------------------------------
// ALX - ID - C/C++
//------------------------------------------------------------------------------

// FW - Language C
#define ALX_LANG_C_VER_MIN_REQUIRED 199901L // C99

// FW - Language C Library
#define ALX_LANG_C_LIB_VER_MIN_REQUIRED 0x02050000 // 2.5.0

// FW - Language C++
#define ALX_LANG_CPP_VER_MIN_REQUIRED 201402L // C++14

// FW - Language C++ Library
#define ALX_LANG_CPP_LIB_VER_MIN_REQUIRED 20170814

// FW - Compiler
#define ALX_GCC
#define ALX_COMP_VER_MIN_REQUIRED 0x07020000 // 7.2.0

// FW - CMSIS Core
#define ALX_CMSIS_CORE
#define ALX_CMSIS_CORE_VER_REQUIRED 0x00050001 // 5.1

// HW - STM32
//#define ALX_STM32F0
//#define ALX_STM32F0_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32F0_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32F1
//#define ALX_STM32F1_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32F1_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

#define ALX_STM32F4
#define ALX_STM32F4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
#define ALX_STM32F4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32F7
//#define ALX_STM32F7_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32F7_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32G4
//#define ALX_STM32G4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32G4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32L0
//#define ALX_STM32L0_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32L0_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32L4
//#define ALX_STM32L4_CMSIS_VER_REQUIRED 0x01010100 // 1.1.1.0
//#define ALX_STM32L4_HAL_VER_REQUIRED 0x01010100 // 1.1.1.0

//#define ALX_STM32U5
//#define ALX_STM32U5_CMSIS_VER_REQUIRED 0x01030000 // 1.3.0.0
//#define ALX_STM32U5_HAL_VER_REQUIRED 0x01030000 // 1.3.0.0

// HW - LPC
//#define ALX_LPC17XX
//#define ALX_LPC55S6X
//#define ALX_LPC80X
//#define ALX_LPC84X


//------------------------------------------------------------------------------
// ALX - MCU - IO Pin IRQ - C/C++
//------------------------------------------------------------------------------
//#define ALX_IO_PIN_IRQ_HANDLERS_OFF




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// C/C++
//------------------------------------------------------------------------------
#define ALX_C_LIB
//#define ALX_CPP_LIB


//------------------------------------------------------------------------------
// Assert
//------------------------------------------------------------------------------
#define _ALX_ASSERT_BKPT_ALL
//#define _ALX_ASSERT_TRACE_ALL
//#define _ALX_ASSERT_RST_ALL


//------------------------------------------------------------------------------
// Trace
//------------------------------------------------------------------------------
#define _ALX_TRACE_ALL


//------------------------------------------------------------------------------
// Optimize Size
//------------------------------------------------------------------------------
//#define ALX_ADC_OPTIMIZE_SIZE
//#define ALX_PWM_OPTIMIZE_SIZE




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Configuration File - User
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//#include "alxConfigUsr.h"


#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CONFIG_H
