/**
  ******************************************************************************
  * @file		alxWdt_McuLpc80x.h
  * @brief		Auralix C Library - ALX Watchdog Timer MCU LPC80X Module
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
#ifndef ALX_WDT_MCU_LPC80X_H
#define ALX_WDT_MCU_LPC80X_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxWdt_Config_McuLpc80x_WdtTimeout_4000ms_WdtClk_50kHz_WdtOsc_200kHz	// TV: ATTENTION, WDT oscillator has +/-40% tolerance!
} AlxWdt_Config;

typedef struct
{
	// Parameters
	AlxWdt_Config config;
	AlxClk* clk;

	// Variables
	wwdt_config_t wwdtConfig;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxWdt;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxWdt_Ctor
(
	AlxWdt* me,
	AlxWdt_Config config,
	AlxClk* clk
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_WDT_MCU_LPC80X_H
