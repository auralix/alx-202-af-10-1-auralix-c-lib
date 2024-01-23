/**
  ******************************************************************************
  * @file		alxWdt_McuLpc80x.c
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
// Includes
//******************************************************************************
#include "alxWdt_McuLpc80x.h"
#include "alxWdt.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxWdt_IsClkOk(AlxWdt* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxWdt_Ctor
(
	AlxWdt* me,
	AlxWdt_Config config,
	AlxClk* clk
)
{
	// Parameters
	me->config = config;
	me->clk = clk;

	// Variables
	me->wwdtConfig.enableWwdt = true;
	me->wwdtConfig.enableWatchdogReset = true;
	me->wwdtConfig.enableWatchdogProtect = false;
	me->wwdtConfig.enableLockOscillator = true;
	me->wwdtConfig.windowValue = 0x00FFFFFF;
	me->wwdtConfig.warningValue = 0;
	if (me->config == AlxWdt_Config_McuLpc80x_WdtTimeout_4000ms_WdtClk_50kHz_WdtOsc_200kHz)
	{
		me->wwdtConfig.timeoutValue = 200000;
		me->wwdtConfig.clockFreq_Hz = 50000;
	}
	else
	{
		ALX_WDT_ASSERT(false);	// We should not get here
	}

	// Check clock
	ALX_WDT_ASSERT(AlxWdt_IsClkOk(me));

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxWdt_Init(AlxWdt* me)
{
	// Assert
	ALX_WDT_ASSERT(me->wasCtorCalled == true);
	ALX_WDT_ASSERT(me->isInit == false);

	// Init WDT
	WWDT_Init(WWDT, &me->wwdtConfig);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxWdt_Refresh(AlxWdt* me)
{
	// Assert
	ALX_WDT_ASSERT(me->wasCtorCalled == true);
	ALX_WDT_ASSERT(me->isInit == true);

	// Refresh
	WWDT_Refresh(WWDT);

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static bool AlxWdt_IsClkOk(AlxWdt* me)
{
	if (me->config == AlxWdt_Config_McuLpc80x_WdtTimeout_4000ms_WdtClk_50kHz_WdtOsc_200kHz)
	{
		if(200000 == AlxClk_GetClk_Hz(me->clk, AlxClk_Clk_McuLpc8xx_WdtOsc_Ctor))
			return true;
		else
			return false;
	}

	ALX_WDT_ASSERT(false);	// We should not get here
	return ALX_NULL;
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))
