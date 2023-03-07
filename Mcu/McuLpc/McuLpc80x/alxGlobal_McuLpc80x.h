/**
  ******************************************************************************
  * @file		alxGlobal_McuLpc80x.h
  * @brief		Auralix C Library - ALX Global MCU LPC80X Module
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
#ifndef ALX_GLOBAL_MCU_LPC80X_H
#define ALX_GLOBAL_MCU_LPC80X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC80X)


//******************************************************************************
// Includes
//******************************************************************************
#include "fsl_acomp.h"
#include "fsl_adc.h"
#include "fsl_capt.h"
#include "fsl_clock.h"
#include "fsl_common.h"
#include "fsl_crc.h"
#include "fsl_ctimer.h"
#include "fsl_dac.h"
//#include "fsl_dma.h"						// MF: Doesn't work on Lpc804
#include "fsl_gpio.h"
#include "fsl_i2c.h"
#include "fsl_iap.h"
//#include "fsl_inputmux.h"					// MF: Doesn't work on Lpc804
//#include "fsl_inputmux_connections.h"		// MF: Doesn't work on Lpc804
#include "fsl_iocon.h"
#include "fsl_mrt.h"
#include "fsl_pint.h"
#include "fsl_power.h"
#include "fsl_reset.h"
//#include "fsl_sctimer.h"					// MF: Doesn't work on Lpc804
#include "fsl_spi.h"
#include "fsl_swm.h"
#include "fsl_swm_connections.h"
#include "fsl_syscon.h"
#include "fsl_syscon_connections.h"
#include "fsl_usart.h"
#include "fsl_wkt.h"
#include "fsl_wwdt.h"


//******************************************************************************
// IoPin Functions
//******************************************************************************
uint8_t AlxPROTECTED_IoPin_GetIoconPortPinIndex(uint8_t pin, uint8_t port);
void AlxPROTECTED_IoPin_SetIoconMode(uint8_t pin, uint8_t port, uint32_t mode);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_MCU_LPC80X_H
