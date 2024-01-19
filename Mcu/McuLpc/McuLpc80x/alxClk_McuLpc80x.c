/**
  ******************************************************************************
  * @file		alxClk_McuLpc80x.c
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
// Includes
//******************************************************************************
#include "alxClk_McuLpc80x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk(AlxClk* me);
static void AlxClk_PeriphGpio_Reset(AlxClk* me);
static bool AlxClk_AreClkNok(AlxClk* me);
static void AlxClk_Ctor_McuLpc84x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc84x_FroOsc_30MHz_Mainclk_30MHz_CoreSysClk_30MHz(AlxClk* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		config
  * @param[in]		tick
  */
ALX_WEAK void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config,
	AlxClk_Tick tick
)
{
	// Assert
	ALX_CLK_ASSERT(tick >= AlxClk_Tick_100us);

	// Parameters
	me->config = config;
	me->tick = tick;

	// Variables
	me->coreSysClk = 0;
	me->mainClk = 0;
	me->fro = 0;

	#if defined(ALX_LPC80X)
	if		(me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)		{ ALX_CLK_ASSERT(false); return;											}
	else if	(me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)				{ ALX_CLK_ASSERT(false); return;											}
	#endif
	#if defined(ALX_LPC84X)
	if		(me->config == AlxClk_Config_McuLpc84x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)		{ ALX_CLK_ASSERT(false); return;											}
	else if	(me->config == AlxClk_Config_McuLpc84x_FroOsc_30MHz_Mainclk_30MHz_CoreSysClk_30MHz)				{ AlxClk_Ctor_McuLpc84x_FroOsc_30MHz_Mainclk_30MHz_CoreSysClk_30MHz(me);	}
	#endif
	else																									{ ALX_CLK_ASSERT(false); return;											}
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
ALX_WEAK Alx_Status AlxClk_Init(AlxClk* me)
{
	// Assert
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// Reset GPIO periphery
	AlxClk_PeriphGpio_Reset(me);

	// Enable GPIO periphery clocks
	AlxClk_PeriphGpio_EnableClk(me);

	// Init
	POWER_DisablePD(kPDRUNCFG_PD_FRO_OUT);		// MF: Ensures FRO is on
	POWER_DisablePD(kPDRUNCFG_PD_FRO);			// MF: Ensures FRO is on
	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut30M);	// MF: Set up FRO freq
	CLOCK_SetFroOutClkSrc(kCLOCK_FroSrcFroOsc);	// MF: Set FRO clock source direct from FRO oscillator // Doesn't work on Lpc804
	CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcFro);	// MF: Select FRO for Main Clk
	CLOCK_SetCoreSysClkDiv(1);					// MF: Divide main clk to provide the system clk to the core, memories, and the peripherals

	// Check clocks
	if (AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("Err"); return Alx_Err; }

	// Update SystemCoreClock
	SystemCoreClockUpdate();

	// Configure SysTick
	if (SysTick_Config(SystemCoreClock / (1000000U / me->tick)) == 1) { ALX_CLK_TRACE("Err"); return Alx_Err; }	// MF: In the example it was 1000000 when setting up SysTick

	// Set SystemCoreClock
	me->systemCoreClock = SystemCoreClock;

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
ALX_WEAK Alx_Status AlxClk_DeInit(AlxClk* me)
{
	// TV: TODO
	ALX_CLK_ASSERT(false);
	return Alx_Err;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		clk
  * @return
  */
ALX_WEAK uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc8xx_CoreSysClk)		return me->coreSysClk;
		if (clk == AlxClk_Clk_McuLpc8xx_MainClk)		return me->mainClk;
		if (clk == AlxClk_Clk_McuLpc8xx_Fro)			return me->fro;
	}

	if (clk == AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor)	return me->coreSysClk_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_MainClk_Ctor)		return me->mainClk_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_Fro_Ctor)			return me->fro_Ctor;

	ALX_CLK_ASSERT(false);	// We should not get here
	return ALX_NULL;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxClk_Irq_Handle(AlxClk* me)
{
	// TV: TODO
	ALX_CLK_ASSERT(false);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk(AlxClk* me)
{
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);
}
static void AlxClk_PeriphGpio_Reset(AlxClk* me)
{
	RESET_PeripheralReset(kGPIO0_RST_N_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_N_SHIFT_RSTn);
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	me->coreSysClk = CLOCK_GetCoreSysClkFreq();
	me->mainClk = CLOCK_GetMainClkFreq();
	me->fro = CLOCK_GetFroFreq();

	if		(me->coreSysClk != me->coreSysClk_Ctor)		{ ALX_CLK_TRACE("Err");	return true; }
	else if (me->mainClk != me->mainClk_Ctor)			{ ALX_CLK_TRACE("Err");	return true; }
	else if (me->fro != me->fro_Ctor)					{ ALX_CLK_TRACE("Err");	return true; }

	return false;
}
static void AlxClk_Ctor_McuLpc84x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me)
{
	me->coreSysClk_Ctor			= 12000000U;
	me->mainClk_Ctor			= 12000000U;
	me->fro_Ctor				= 24000000U;
}
static void AlxClk_Ctor_McuLpc84x_FroOsc_30MHz_Mainclk_30MHz_CoreSysClk_30MHz(AlxClk* me)
{
	me->coreSysClk_Ctor			= 30000000U;
	me->mainClk_Ctor			= 30000000U;
	me->fro_Ctor				= 30000000U;
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))
