﻿/**
  ******************************************************************************
  * @file		alxClk_McuLpc55S6x.h
  * @brief		Auralix C Library - ALX Clock MCU LPC55S6X Module
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
#ifndef ALX_CLK_MCU_LPC55S6X_H
#define ALX_CLK_MCU_LPC55S6X_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default,
	AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz,
	AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz
} AlxClk_Config;

typedef struct
{
	// Parameters
	AlxClk_Config config;
	AlxClk_Tick tick;

	// Variables
	uint32_t systemCoreClock;
	uint32_t mainClk;
	uint32_t ahbClk;
	uint32_t froOsc_1MHz;
	uint32_t wdtOsc;

	uint32_t systemCoreClock_Ctor;
	uint32_t mainClk_Ctor;
	uint32_t ahbClk_Ctor;
	uint32_t froOsc_1MHz_Ctor;
	uint32_t wdtOsc_Ctor;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config,
	AlxClk_Tick tick
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CLK_MCU_LPC55S6X_H
