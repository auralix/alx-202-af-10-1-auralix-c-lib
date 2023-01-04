/**
  ******************************************************************************
  * @file		alxClk_McuLpc80x.h
  * @brief		Auralix C Library - ALX Clock MCU LPC80X Module
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
#ifndef ALX_CLK_MCU_LPC80X_H
#define ALX_CLK_MCU_LPC80X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	#if defined(ALX_LPC80X)
	AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default = 0,
	AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz = 1,
	AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz = 2,
	AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz = 3,
	AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz = 4,
	AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz = 5,
	AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5 = 6,
	AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz = 7,
	AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5 = 8,
	#endif
	#if defined(ALX_LPC81X) || defined(ALX_LPC82X) || defined(ALX_LPC83X) || defined(ALX_LPC84X)
	// MF: NOTE - this Mcus will have the same enum
	#endif
} AlxClk_Config;

typedef struct
{
	// Parameters
	AlxClk_Config config;
	AlxClk_Tick tick;

	// Variables
	uint32_t systemCoreClock;
	uint32_t coreSysClk;
	uint32_t mainClk;
	uint32_t fro;
	//uint32_t extClk;	// MF: Don't know how we will use it
	//uint32_t frg0;	// MF: Don't know how we will use it

	uint32_t coreSysClk_Ctor;
	uint32_t mainClk_Ctor;
	uint32_t fro_Ctor;
	uint32_t lpo_Ctor;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		config
  * @param[in]		tick
  */
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config,
	AlxClk_Tick tick
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC80X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CLK_MCU_LPC80X_H
