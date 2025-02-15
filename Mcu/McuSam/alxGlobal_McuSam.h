/**
  ******************************************************************************
  * @file		alxGlobal_McuSam.h
  * @brief		Auralix C Library - ALX Global MCU SAM File
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
#ifndef ALX_GLOBAL_MCU_SAM_H
#define ALX_GLOBAL_MCU_SAM_H

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
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Includes
//******************************************************************************
#include <peripheral_clk_config.h>
#include <hpl_sercom_config.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>
#include <hal_init.h>
#include <hal_adc_sync.h>
#include <hal_gpio.h>
#include <hal_usart_sync.h>


//******************************************************************************
// Preprocessor
//******************************************************************************

//------------------------------------------------------------------------------
// ALX - MCU - SerialPort & Trace
//------------------------------------------------------------------------------
#ifndef CONF_GCLK_SERCOM0_CORE_SRC
	#define CONF_GCLK_SERCOM0_CORE_SRC 0
#endif
#ifndef CONF_GCLK_SERCOM1_CORE_SRC
	#define CONF_GCLK_SERCOM1_CORE_SRC 0
#endif
#ifndef CONF_GCLK_SERCOM2_CORE_SRC
	#define CONF_GCLK_SERCOM2_CORE_SRC 0
#endif
#ifndef CONF_GCLK_SERCOM3_CORE_SRC
	#define CONF_GCLK_SERCOM3_CORE_SRC 0
#endif
#ifndef CONF_GCLK_SERCOM4_CORE_SRC
	#define CONF_GCLK_SERCOM4_CORE_SRC 0
#endif
#ifndef CONF_GCLK_SERCOM5_CORE_SRC
	#define CONF_GCLK_SERCOM5_CORE_SRC 0
#endif


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_MCU_SAM_H
