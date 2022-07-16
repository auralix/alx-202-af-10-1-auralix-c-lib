/**
  ******************************************************************************
  * @file		alxClk_McuLpc84x.c
  * @brief		Auralix C Library - ALX Clock MCU LPC84X Module
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

//******************************************************************************
// Includes
//******************************************************************************
#include "alxClk_McuLpc84x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC84X)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_SetFro(AlxClk* me);
static void AlxClk_SetExtClk(AlxClk* me, bool sysOsc);
//static void AlxClk_SetWdtOsc(AlxClk* me); // TODO
//static void AlxClk_SetFroDiv(AlxClk* me);
//static void AlxClk_SetSysPll(AlxClk* me);


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxClk_Init(AlxClk* me)
{
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Enable GPIO clock
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);

	// #3 Set up clk Source
	if(me->mainClkSource == kCLOCK_MainClkSrcFro)		{ AlxClk_SetFro(me); }
	if (me->mainClkSource == kCLOCK_MainClkSrcExtClk)	{ AlxClk_SetExtClk(me, true); }	// 2nd argument shouldn be hardcoded!!!
	//if (me->mainClkSource == kCLOCK_MainClkSrcWdtOsc) // TODO
	//if (me->mainClkSource == kCLOCK_MainClkSrcFroDiv)
	//if (me->mainClkSource == kCLOCK_MainClkSrcSysPll)

	// #4 Configure the main clock and system clock
	CLOCK_SetMainClkSrc(me->mainClkSource); 		// Select main clock in MAINCLKSEL and update MAINCLKUEN
	CLOCK_SetCoreSysClkDiv(me->divider); 		// Divide main clk to provide the system clk to the core, memories, and the perispherals.
	SystemCoreClock = (me->clkFreq * 1000U);  	// Set SystemCoreClock variable.
	
	// #5 Enable LPO
	POWER_EnbaleLPO(true);
	//POWER_EnbaleLPOInDeepPowerDownMode(true);
}
void AlxClk_DeInit(AlxClk* me)
{
	ALX_CLK_ASSERT(me->isInit == true);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// #2 Enable GPIO clock
	CLOCK_DisableClock(kCLOCK_Gpio0);
	CLOCK_DisableClock(kCLOCK_Gpio1);

	me->isInit = false;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_SetFro(AlxClk* me)
{
	POWER_DisablePD(kPDRUNCFG_PD_FRO_OUT);    	// Ensure FRO is on
	POWER_DisablePD(kPDRUNCFG_PD_FRO);    		// Ensure FRO is on
	CLOCK_SetFroOscFreq(me->clkFreq);    		// Set up FRO freq
	CLOCK_SetFroOutClkSrc(kCLOCK_FroSrcFroOsc); 	// Set FRO clock source direct from FRO oscillator
}
static void AlxClk_SetExtClk(AlxClk* me, bool sysOsc)
{
	if (sysOsc)
	{
		POWER_DisablePD(kPDRUNCFG_PD_SYSOSC);    // Ensure Crystal Oscillator is on
		CLOCK_Select(kEXT_Clk_From_SysOsc);    	// Select system oscillator as an external clockk
	}
	else
	{
		CLOCK_Select(kEXT_Clk_From_ClkIn); 		// Select clk_in (direct from external IO) as an external clk
	}
}


#endif
