/**
  ******************************************************************************
  * @file		alxGlobal_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX Global MCU LPC55S6X File
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
#ifndef ALX_GLOBAL_MCU_LPC55S6X_H
#define ALX_GLOBAL_MCU_LPC55S6X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Includes
//******************************************************************************
#include "fsl_anactrl.h"
#include "fsl_casper.h"
#include "fsl_clock.h"
#include "fsl_cmp.h"
#include "fsl_common.h"
#include "fsl_common_arm.h"
#include "fsl_crc.h"
#include "fsl_ctimer.h"
#include "fsl_dma.h"
#include "fsl_flexcomm.h"
#include "fsl_fro_calib.h"	// TV: LPC55S66 doesn't support this module, LPC55S69 does
#include "fsl_gint.h"
#include "fsl_gpio.h"
#include "fsl_hashcrypt.h"
#include "fsl_i2c.h"
#include "fsl_i2c_dma.h"
#include "fsl_i2s.h"
#include "fsl_i2s_dma.h"
#include "fsl_iap.h"
#include "fsl_iap_ffr.h"
#include "fsl_iap_kbp.h"
#include "fsl_iap_skboot_authenticate.h"
#include "fsl_inputmux.h"
#include "fsl_inputmux_connections.h"
#include "fsl_iocon.h"
#include "fsl_lpadc.h"
#include "fsl_mailbox.h"
#include "fsl_mrt.h"
#include "fsl_ostimer.h"
#include "fsl_pint.h"
#include "fsl_plu.h"
#include "fsl_powerquad.h"
#include "fsl_powerquad_data.h"
#include "fsl_power.h"
//#include "fsl_prince.h"	// MF: error: redeclaration of enumerator 'kSECURE_FALSE'
#include "fsl_puf.h"
#include "fsl_reset.h"
#include "fsl_rng.h"
#include "fsl_rtc.h"
#include "fsl_sctimer.h"
#include "fsl_sdif.h"
#include "fsl_spi.h"
#include "fsl_spi_dma.h"
#include "fsl_sysctl.h"
#include "fsl_usart.h"
#include "fsl_usart_dma.h"
#include "fsl_utick.h"
#include "fsl_wwdt.h"


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_GLOBAL_MCU_LPC55S6X_H
