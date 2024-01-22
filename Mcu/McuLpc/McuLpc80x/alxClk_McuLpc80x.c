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


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
#if defined(ALX_LPC80X)
#endif
#if defined(ALX_LPC84X)
static void AlxClk_Ctor_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz(AlxClk* me);

static void AlxClk_Init_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz(AlxClk* me);
static void AlxClk_Init_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz(AlxClk* me);
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxClk_IsClkOk(AlxClk* me);
static void AlxClk_Periph_Gpio_Reset(void);
static void AlxClk_Periph_Gpio_EnableClk(void);


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
	#if defined(ALX_LPC80X)
	if		(me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)	{ ALX_CLK_ASSERT(false); return;												}
	else if	(me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)			{ ALX_CLK_ASSERT(false); return;												}
	#endif
	#if defined(ALX_LPC84X)
	if		(me->config == AlxClk_Config_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default)		{ AlxClk_Ctor_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default(me);	}
	else if	(me->config == AlxClk_Config_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz)				{ AlxClk_Ctor_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz(me);			}
	else if	(me->config == AlxClk_Config_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz)				{ AlxClk_Ctor_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz(me);			}
	#endif
	else																								{ ALX_CLK_ASSERT(false); return;												}	// We should not get here

	me->systemCoreClock = 0;
	me->mainClk = 0;
	me->ahbClk = 0;
	me->froOsc = 0;
	me->extClk = 0;

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
	AlxClk_Periph_Gpio_Reset();

	// Enable GPIO periphery clocks
	AlxClk_Periph_Gpio_EnableClk();

	// Init
	#if defined(ALX_LPC80X)
	if		(me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)	{ ALX_CLK_ASSERT(false); return;												}
	else if	(me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)			{ ALX_CLK_ASSERT(false); return;												}
	#endif
	#if defined(ALX_LPC84X)
	if		(me->config == AlxClk_Config_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default)		{ AlxClk_Init_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default(me);	}
	else if	(me->config == AlxClk_Config_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz)				{ AlxClk_Init_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz(me);			}
	else if	(me->config == AlxClk_Config_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz)				{ AlxClk_Init_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz(me);			}
	#endif
	else																								{ ALX_CLK_ASSERT(false); return Alx_Err;										}	// We should not get here

	// Update SystemCoreClock
	SystemCoreClockUpdate();

	// Configure SysTick
	if (SysTick_Config(SystemCoreClock / (1000000U / me->tick)) == 1) { ALX_CLK_TRACE("Err"); return Alx_Err; }	// MF: In the example it was 1000000 when setting up SysTick

	// Check clocks
	if (AlxClk_IsClkOk(me) == false) { ALX_CLK_TRACE("Err"); return Alx_Err; }

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
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// Return
	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc8xx_SystemCoreClock)	return me->systemCoreClock;
		if (clk == AlxClk_Clk_McuLpc8xx_MainClk)			return me->mainClk;
		if (clk == AlxClk_Clk_McuLpc8xx_AhbClk)				return me->ahbClk;
		if (clk == AlxClk_Clk_McuLpc8xx_FroOsc)				return me->froOsc;
		if (clk == AlxClk_Clk_McuLpc8xx_ExtClk)				return me->extClk;
	}

	if (clk == AlxClk_Clk_McuLpc8xx_SystemCoreClock_Ctor)	return me->systemCoreClock_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_MainClk_Ctor)			return me->mainClk_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_AhbClk_Ctor)			return me->ahbClk_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_FroOsc_Ctor)			return me->froOsc_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_ExtClk_Ctor)			return me->extClk_Ctor;

	// Assert
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


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
#if defined(ALX_LPC80X)
#endif
#if defined(ALX_LPC84X)
static void AlxClk_Ctor_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 12000000U;
	me->mainClk_Ctor			= 12000000U;
	me->ahbClk_Ctor				= 12000000U;
	me->froOsc_Ctor				= 12000000U;
	me->extClk_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 30000000U;
	me->mainClk_Ctor			= 30000000U;
	me->ahbClk_Ctor				= 30000000U;
	me->froOsc_Ctor				= 30000000U;
	me->extClk_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 30000000U;
	me->mainClk_Ctor			= 30000000U;
	me->ahbClk_Ctor				= 30000000U;
	me->froOsc_Ctor				= 24000000U;
	me->extClk_Ctor				= 12000000U;
}

static void AlxClk_Init_McuLpc84x_MainClk_12MHz_AhbClk_12MHz_FroOsc_24MHz_Default(AlxClk* me)
{
	(void)me;
	ALX_CLK_ASSERT(false);	// TV: TODO
}
static void AlxClk_Init_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_FroOsc_30MHz(AlxClk* me)
{
	(void)me;

	POWER_DisablePD(kPDRUNCFG_PD_FRO_OUT);		// MF: Ensures FRO is on
	POWER_DisablePD(kPDRUNCFG_PD_FRO);			// MF: Ensures FRO is on
	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut30M);	// MF: Set up FRO frequency
	CLOCK_SetFroOutClkSrc(kCLOCK_FroSrcFroOsc);	// MF: Set FRO clock source direct from FRO oscillator // Doesn't work on Lpc804

	CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcFro);	// MF: Select FRO for Main clock
	CLOCK_SetCoreSysClkDiv(1);					// MF: Divide main clock to provide the system AHB clock to the core, memories and the peripherals
}
static void AlxClk_Init_McuLpc84x_Mainclk_30MHz_AhbClk_30MHz_ExtClk_12MHz(AlxClk* me)
{
	(void)me;
	ALX_CLK_ASSERT(false);	// TV: TODO
}
#endif


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxClk_IsClkOk(AlxClk* me)
{
	// Prepare
	me->systemCoreClock = SystemCoreClock;
	me->mainClk = CLOCK_GetMainClkFreq();
	me->ahbClk = CLOCK_GetCoreSysClkFreq();
	me->froOsc = CLOCK_GetFroFreq();
	me->extClk = CLOCK_GetExtClkFreq();

	// Check
	if (SystemCoreClock != me->systemCoreClock_Ctor)	{ ALX_CLK_TRACE("Err");	return false; }
	if (me->mainClk != me->mainClk_Ctor)				{ ALX_CLK_TRACE("Err");	return false; }
	if (me->ahbClk != me->ahbClk_Ctor)					{ ALX_CLK_TRACE("Err");	return false; }
	if (me->froOsc != me->froOsc_Ctor)					{ ALX_CLK_TRACE("Err");	return false; }
	if (me->extClk != me->extClk_Ctor)					{ ALX_CLK_TRACE("Err");	return false; }

	// Return
	return true;
}
static void AlxClk_Periph_Gpio_Reset(void)
{
	RESET_PeripheralReset(kGPIO0_RST_N_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_N_SHIFT_RSTn);
}
static void AlxClk_Periph_Gpio_EnableClk(void)
{
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_LPC80X) || defined(ALX_LPC84X))
