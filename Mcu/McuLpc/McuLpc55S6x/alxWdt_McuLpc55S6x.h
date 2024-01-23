/**
  ******************************************************************************
  * @file		alxWdt_McuLpc55S6x.h
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
#ifndef ALX_WDT_MCU_LPC55S6X_H
#define ALX_WDT_MCU_LPC55S6X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxWdt_Config_McuLpc55S6x_WdtTimeout_4000ms_WdtClk_250kHz_WdtOsc_1MHz	// TV: ATTENTION, WDT oscillator is generated from FRO_1MHz and has +/-15% tolerance!
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_WDT_MCU_LPC55S6X_H
