/**
  ******************************************************************************
  * @file		alxClk_McuLpc55S6x.c
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
// Includes
//******************************************************************************
#include "alxClk_McuLpc55S6x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me);

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me);

static void AlxClk_SetupPll(AlxClk* me, pll_setup_t* pllSetup, uint32_t inputFreq, uint8_t pllN, uint8_t pllP, uint16_t pllM);


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxClk_IsClkOk(AlxClk* me);
static void AlxClk_Periph_Gpio_EnableClk(AlxClk* me);
static void AlxClk_Periph_Gpio_DisableClk(AlxClk* me);
static void AlxClk_Periph_Gpio_Reset(AlxClk* me);


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
)
{
	// Assert
	ALX_CLK_ASSERT(tick >= AlxClk_Tick_100us);

	// Parameters
	me->config = config;
	me->tick = tick;

	// Variables
	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)					{ AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz)	{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz)							{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz)	{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz(me); }
	else																												{ ALX_CLK_ASSERT(false); return; }

	me->systemCoreClock = 0;
	me->mainClk = 0;
	me->ahbClk = 0;
	me->froOsc_1MHz = 0;
	me->wdtOsc = 0;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
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
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_CLK_ASSERT(me->wasCtorCalled == true);
	ALX_CLK_ASSERT(me->isInit == false);


	//------------------------------------------------------------------------------
	// Init GPIO Clocks
	//------------------------------------------------------------------------------

	// Enable GPIO clock
	AlxClk_Periph_Gpio_EnableClk(me);


	//------------------------------------------------------------------------------
	// Init MCU Clocks
	//------------------------------------------------------------------------------

	// Init
	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)					{ AlxClk_Init_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz)	{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz)							{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz)	{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz(me); }
	else																												{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// Update SystemCoreClock
	SystemCoreClockUpdate();

	// Configure SysTick - MF: In the example it was 1000000 when setting up SysTick
	if (SysTick_Config(SystemCoreClock / (1000000U / me->tick)) == 1)
	{
		ALX_CLK_TRACE_ERR("Err");
		return Alx_Err;
	}

	// Enable IOCON	// MF: it has to always be on. see: "alxWiki_McuLpc55S6x.md" for more info
	CLOCK_EnableClock(kCLOCK_Iocon);

	// Check clocks
	if (AlxClk_IsClkOk(me) == false)
	{
		ALX_CLK_TRACE_ERR("Err");
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

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
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_CLK_ASSERT(me->wasCtorCalled == true);
	ALX_CLK_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Init MCU Default Clocks
	//------------------------------------------------------------------------------

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Attach FRO12 to MainClk
	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);

	// Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 2U, true);

	// Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(6000000U);


	//------------------------------------------------------------------------------
	// DeInit PLL
	//------------------------------------------------------------------------------

	// Disable PLL clock
	CLOCK_AttachClk(kNONE_to_PLL0);

	// Disable PLL power
	POWER_EnablePD(kPDRUNCFG_PD_PLL0);
	POWER_EnablePD(kPDRUNCFG_PD_PLL0_SSCG);


	//------------------------------------------------------------------------------
	// DeInit GPIO Clocks
	//------------------------------------------------------------------------------

	// Disable GPIO clock
	AlxClk_Periph_Gpio_DisableClk(me);

	// Disable IOCON clock
	CLOCK_DisableClock(kCLOCK_Iocon);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------

	// Reset isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		clk
  * @return
  */
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// Return Clk Freq
	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc55s6x_SystemCoreClock)	{ return me->systemCoreClock; }
		if (clk == AlxClk_Clk_McuLpc55s6x_MainClk)			{ return me->mainClk; }
		if (clk == AlxClk_Clk_McuLpc55s6x_AhbClk)			{ return me->ahbClk; }
		if (clk == AlxClk_Clk_McuLpc55s6x_FroOsc_1MHz)		{ return me->froOsc_1MHz; }
		if (clk == AlxClk_Clk_McuLpc55s6x_WdtOsc)			{ return me->wdtOsc; }
	}

	if (clk == AlxClk_Clk_McuLpc55s6x_SystemCoreClock_Ctor)	{ return me->systemCoreClock_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_MainClk_Ctor)			{ return me->mainClk_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_AhbClk_Ctor)			{ return me->ahbClk_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_FroOsc_1MHz_Ctor)		{ return me->froOsc_1MHz_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_WdtOsc_Ctor)			{ return me->wdtOsc_Ctor; }

	// Assert
	ALX_CLK_ASSERT(false);	// We shouldn't get here
	return 0;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxClk_Irq_Handle(AlxClk* me)
{
	// Assert
	(void)me;

	// TV: TODO
	ALX_CLK_ASSERT(false);	// We shouldn't get here
	return;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Specific
//------------------------------------------------------------------------------
static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 6000000U;
	me->mainClk_Ctor			= 12000000U;
	me->ahbClk_Ctor				= 6000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 12000000U;
	me->mainClk_Ctor			= 96000000U;
	me->ahbClk_Ctor				= 12000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 24000000U;
	me->mainClk_Ctor			= 96000000U;
	me->ahbClk_Ctor				= 24000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 48000000U;
	me->mainClk_Ctor			= 96000000U;
	me->ahbClk_Ctor				= 48000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 96000000U;
	me->mainClk_Ctor			= 96000000U;
	me->ahbClk_Ctor				= 96000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 150000000U;
	me->mainClk_Ctor			= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 150000000U;
	me->mainClk_Ctor			= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->froOsc_1MHz_Ctor		= 1000000U;
	me->wdtOsc_Ctor				= 1000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 150000000U;
	me->mainClk_Ctor			= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 150000000U;
	me->mainClk_Ctor			= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->froOsc_1MHz_Ctor		= 0U;
	me->wdtOsc_Ctor				= 0U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me)
{
	me->systemCoreClock_Ctor	= 150000000U;
	me->mainClk_Ctor			= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->froOsc_1MHz_Ctor		= 1000000U;
	me->wdtOsc_Ctor				= 1000000U;
}

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	// Void
	(void)me;

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(6000000U);

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Attach to mainClk
	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 2U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me)
{
	ALX_CLK_ASSERT(false);	// TV: TODO, FUKA

	// Void
	(void)me;

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(12000000U);

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Attach to mainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 8U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me)
{
	ALX_CLK_ASSERT(false);	// TV: TODO, FUKA

	// Void
	(void)me;

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(24000000U);

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Attach to mainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 4U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me)
{
	ALX_CLK_ASSERT(false);	// TV: TODO, FUKA

	// Void
	(void)me;

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(48000000U);

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Attach to mainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 2U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	// Void
	(void)me;

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(96000000U);

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Attach to mainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(AlxClk* me)
{
	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(150000000U);

	// Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// Setup PLL
	pll_setup_t pll0Setup;
	AlxClk_SetupPll(me, &pll0Setup, 12000000U, 6U, 1U, 150U);

	// Attach to PLL
	CLOCK_AttachClk(kFRO12M_to_PLL0);

	// Enable PLL
	ALX_CLK_ASSERT(CLOCK_SetPLL0Freq(&pll0Setup) == kStatus_PLL_Success);	// MF: Enabling PLL happend here

	// Attach to mainClk
	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me)
{
	AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz(me);

	// Enable FRO 1MHz
	POWER_DisablePD(kPDRUNCFG_PD_FRO1M);
	SYSCON->CLOCK_CTRL |= (1 << 6);

	// Enable WDT Oscillator 1MHz
	CLOCK_SetClkDiv(kCLOCK_DivWdtClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	// Enable external clock
	ALX_CLK_ASSERT(CLOCK_SetupExtClocking(16000000U) == kStatus_Success);	// MF: Enable clk_in and set up crystal Freq. Enabling XTAL32M happens here
	ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;	// MF: Enable clk_in to system

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(150000000U);

	// Setup PLL
	pll_setup_t pll0Setup;
	AlxClk_SetupPll(me, &pll0Setup, 16000000U, 8U, 1U, 150U);

	// Attach to PLL
	CLOCK_AttachClk(kEXT_CLK_to_PLL0);

	// Enable PLL
	ALX_CLK_ASSERT(CLOCK_SetPLL0Freq(&pll0Setup) == kStatus_PLL_Success);	// MF: Enabling PLL happend here

	// Attach to mainClk
	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(AlxClk* me)
{
	// Enable external clock
	ALX_CLK_ASSERT(CLOCK_SetupExtClocking(25000000U) == kStatus_Success);	// MF: Enable clk_in and set up crystal Freq. Enabling XTAL32M happens here
	ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;	// MF: Enable clk_in to system

	// Set FLASH wait states
	CLOCK_SetFLASHAccessCyclesForFreq(150000000U);

	// Setup PLL
	pll_setup_t pll0Setup;
	AlxClk_SetupPll(me, &pll0Setup, 25000000U, 25U, 1U, 300U);

	// Attach to PLL
	CLOCK_AttachClk(kEXT_CLK_to_PLL0);

	// Enable PLL
	ALX_CLK_ASSERT(CLOCK_SetPLL0Freq(&pll0Setup) == kStatus_PLL_Success);	// MF: Enabling PLL happend here

	// Attach to mainClk
	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

	// Divide ahbClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz_FroOsc_1MHz_WdtOsc_1MHz(AlxClk* me)
{
	AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_25MHz(me);

	// Enable FRO 1MHz
	POWER_DisablePD(kPDRUNCFG_PD_FRO1M);
	SYSCON->CLOCK_CTRL |= (1 << 6);

	// Enable WDT Oscillator 1MHz
	CLOCK_SetClkDiv(kCLOCK_DivWdtClk, 1U, true);
}

static void AlxClk_SetupPll(AlxClk* me, pll_setup_t* pllSetup, uint32_t inputFreq, uint8_t pllN, uint8_t pllP, uint16_t pllM)
{
	// Void
	(void)me;

	// SELI
	uint8_t seli = 0U;
	if (pllM >= 8000U)						{ seli = 1U; }
	if ((8000U > pllM) && (pllM >= 122U))	{ seli = (8000U / pllM); }
	if (( 122U > pllM) && (pllM >= 1U))		{ seli = ((2U * (pllM / 4U)) + 3U); }
	if (seli >= 63U)						{ seli = 63U; }

	// SELP
	uint8_t selp = ((pllM / 4U) + 1U);
	if (selp >= 31U)						{ selp = 31U; }

	// Setup PLL struct		// MF: pllM is set in "pllsscg[1]" below
	pllSetup->pllctrl		= (SYSCON_PLL0CTRL_CLKEN_MASK | SYSCON_PLL0CTRL_SELI(seli) | SYSCON_PLL0CTRL_SELP(selp));
	pllSetup->pllndec		= SYSCON_PLL0NDEC_NDIV(pllN);
	pllSetup->pllpdec		= SYSCON_PLL0PDEC_PDIV(pllP);
	pllSetup->pllsscg[0]	= 0x0U;
	pllSetup->pllsscg[1]	= (SYSCON_PLL0SSCG1_MDIV_EXT(pllM) | SYSCON_PLL0SSCG1_SEL_EXT_MASK);
	pllSetup->pllRate		= (((inputFreq / pllN) * pllM) / (2 * pllP));
	pllSetup->flags			= PLL_SETUPFLAG_WAITLOCK;
}


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
static bool AlxClk_IsClkOk(AlxClk* me)
{
	// Prepare
	me->systemCoreClock = SystemCoreClock;
	me->mainClk = CLOCK_GetFreq(kCLOCK_CoreSysClk);	// MF: CoreSysClk is actually "MainClk" (svinjarijo)
	me->ahbClk = CLOCK_GetFreq(kCLOCK_BusClk);		// MF: BusClk is "SysClk" because it is divided "MainClk"
	me->froOsc_1MHz = me->froOsc_1MHz_Ctor;			// TV: Can't check
	me->wdtOsc = CLOCK_GetWdtClkFreq();

	// Check
	if (SystemCoreClock != me->systemCoreClock_Ctor)
	{
		ALX_CLK_TRACE_ERR("Err");
		return false;
	}
	if (me->ahbClk  != me->ahbClk_Ctor )
	{
		ALX_CLK_TRACE_ERR("Err");
		return false;
	}
	if (me->mainClk != me->mainClk_Ctor)
	{
		ALX_CLK_TRACE_ERR("Err");
		return false;
	}
	if (me->froOsc_1MHz != me->froOsc_1MHz_Ctor)
	{
		ALX_CLK_TRACE_ERR("Err");
		return false;
	}
	if (me->wdtOsc != me->wdtOsc_Ctor)
	{
		ALX_CLK_TRACE_ERR("Err");
		return false;
	}

	// Return
	return true;
}
static void AlxClk_Periph_Gpio_EnableClk(AlxClk* me)
{
	// Void
	(void)me;

	// Enable
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	CLOCK_EnableClock(kCLOCK_Gpio3);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static void AlxClk_Periph_Gpio_DisableClk(AlxClk* me)
{
	// Void
	(void)me;

	// Disable
	CLOCK_DisableClock(kCLOCK_Gpio0);
	CLOCK_DisableClock(kCLOCK_Gpio1);
	CLOCK_DisableClock(kCLOCK_Gpio2);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	CLOCK_DisableClock(kCLOCK_Gpio3);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static void AlxClk_Periph_Gpio_Reset(AlxClk* me)
{
	// Void
	(void)me;

	// Reset
	RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
