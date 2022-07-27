/**
  ******************************************************************************
  * @file		alxClk_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX Clock MCU LPC55S6X Module
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
#include "alxClk_McuLpc55S6x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk(AlxClk* me);
static void AlxClk_PeriphGpio_Reset(AlxClk* me);
static bool AlxClk_AreClkNok(AlxClk* me);
static void AlxClk_SetupPll(AlxClk* me, pll_setup_t* pllSetup, uint32_t inputFreq, uint8_t pllN, uint8_t pllP, uint16_t pllM);

static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me);

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	AlxClk_Config config,
	AlxClk_Tick tick
)
{
	// Assert
	(void)me;
	(void)config;
	ALX_CLK_ASSERT(tick >= AlxClk_Tick_100us);

	// Parameters
	me->config = config;
	me->tick = tick;

	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)	{ AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0)	{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(me); }
	else																								{ ALX_CLK_ASSERT(false); return; }

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxClk_Init(AlxClk* me)
{
	// Assert
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #1 Reset GPIO Periphery
	AlxClk_PeriphGpio_Reset(me);

	// #2 Enable GPIO Periphery clock
	AlxClk_PeriphGpio_EnableClk(me);

	// #3 Init Clocks
	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default)	{ AlxClk_Init_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0)	{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(me); }
	else																								{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// #4 Update SystemCoreClock
	SystemCoreClockUpdate();

	// #5 Configure SysTick
	if (SysTick_Config(SystemCoreClock / (1000000U / me->tick)) == 1) { ALX_CLK_TRACE("ErrSysTickConfig"); return Alx_Err; }	// MF: In the example it was 1000000 when setting up SysTick

	// #6 Enable IOCON	// MF: it has to always be on. see: "alxWiki_McuLpc55S6x.md" for more info
	CLOCK_EnableClock(kCLOCK_Iocon);

	// #7 Check Clocks
	if (AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("ErrCheck"); return Alx_Err; }

	// #8 Set isInit
	me->isInit = true;

	// #9 Return OK
	return Alx_Ok;
}
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);
	(void)me;

	// #1 Return Clk Freq
	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc55s6x_SystemCoreClock)	{ return me->systemCoreClock; }
		if (clk == AlxClk_Clk_McuLpc55s6x_AhbClk)			{ return me->ahbClk; }
		if (clk == AlxClk_Clk_McuLpc55s6x_MainClk)			{ return me->mainClk; }
	}

	if (clk == AlxClk_Clk_McuLpc55s6x_SystemCoreClock_Ctor)	{ return me->systemCoreClock_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_AhbClk_Ctor)			{ return me->ahbClk_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_MainClk_Ctor)			{ return me->mainClk_Ctor; }

	// Assert
	ALX_CLK_ASSERT(false); // We shouldn't get here
	return 0;
}
void AlxClk_Irq_Handle(AlxClk* me)
{
	// Assert
	(void)me;

	// TODO
	ALX_CLK_ASSERT(false); // We shouldn't get here
	return;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Enable GPIO
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	CLOCK_EnableClock(kCLOCK_Gpio3);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static void AlxClk_PeriphGpio_Reset(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Reset GPIO
	RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Get Clks
	me->systemCoreClock = SystemCoreClock;
	me->ahbClk = CLOCK_GetFreq(kCLOCK_BusClk);		// MF: BusClk is "SysClk" because it is divided "MainClk"
	me->mainClk = CLOCK_GetFreq(kCLOCK_CoreSysClk);	// MF: CoreSysClk is actually "MainClk" (svinjarijo)

	// #2 Check Clks
	if		(SystemCoreClock != me->systemCoreClock_Ctor)	{ ALX_CLK_TRACE("ErrSystemCoreClock");	return true; }
	else if	(me->ahbClk  != me->ahbClk_Ctor )				{ ALX_CLK_TRACE("ErrAhbClock");			return true; }
	else if	(me->mainClk != me->mainClk_Ctor)				{ ALX_CLK_TRACE("ErrMainClock");		return true; }
	else													{ return false; }
}
static void AlxClk_SetupPll(AlxClk* me, pll_setup_t* pllSetup, uint32_t inputFreq, uint8_t pllN, uint8_t pllP, uint16_t pllM)
{
	// Assert
	(void)me;
	(void)pllSetup;
	(void)inputFreq;
	(void)pllN;
	(void)pllP;
	(void)pllM;

	// #1 Prepare Variable
	// #1.1 SELI
	uint8_t seli = 0U;
	if (pllM >= 8000U)						{ seli = 1U; }
	if ((8000U > pllM) && (pllM >= 122U))	{ seli = (8000U / pllM); }
	if (( 122U > pllM) && (pllM >= 1U))		{ seli = ((2U * (pllM / 4U)) + 3U); }
	if (seli >= 63U)						{ seli = 63U; }

	// #1.2 SELP
	uint8_t selp = ((pllM / 4U) + 1U);
	if (selp >= 31U)						{ selp = 31U; }

	// #2 Setup Pll Struct		// MF: pllM is set in "pllsscg[1]" below
	pllSetup->pllctrl		= (SYSCON_PLL0CTRL_CLKEN_MASK | SYSCON_PLL0CTRL_SELI(seli) | SYSCON_PLL0CTRL_SELP(selp));
	pllSetup->pllndec		= SYSCON_PLL0NDEC_NDIV(pllN);
	pllSetup->pllpdec		= SYSCON_PLL0PDEC_PDIV(pllP);
	pllSetup->pllsscg[0]	= 0x0U;
	pllSetup->pllsscg[1]	= (SYSCON_PLL0SSCG1_MDIV_EXT(pllM) | SYSCON_PLL0SSCG1_SEL_EXT_MASK);
	pllSetup->pllRate		= (((inputFreq / pllN) * pllM) / (2 * pllP));
	pllSetup->flags			= PLL_SETUPFLAG_WAITLOCK;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 6000000U;
	me->ahbClk_Ctor				= 6000000U;
	me->mainClk_Ctor			= 12000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 96000000U;
	me->ahbClk_Ctor				= 96000000U;
	me->mainClk_Ctor			= 96000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->mainClk_Ctor			= 150000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 150000000U;
	me->ahbClk_Ctor				= 150000000U;
	me->mainClk_Ctor			= 150000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 48000000U;
	me->ahbClk_Ctor				= 48000000U;
	me->mainClk_Ctor			= 96000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 24000000U;
	me->ahbClk_Ctor				= 24000000U;
	me->mainClk_Ctor			= 96000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Ctor Clk Freq
	me->systemCoreClock_Ctor	= 12000000U;
	me->ahbClk_Ctor				= 12000000U;
	me->mainClk_Ctor			= 96000000U;
}
static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_AhbClk_6MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #2 Attach FRO12 to MainClk
	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(96000000U);

	// #2 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #3 Attach FROHF to MainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// #4 Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(150000000U);

	// #2 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #3 Setup PLL
	// #3.1 Prepare Variable
	pll_setup_t pll0Setup;
	AlxClk_SetupPll(me, &pll0Setup, 12000000U, 6U, 1U, 150U);

	// #3.2 Attach FRO 12MHz to PLL0
	CLOCK_AttachClk(kFRO12M_to_PLL0);

	// #3.3 Setup PLL Freq
	if (CLOCK_SetPLL0Freq(&pll0Setup) != kStatus_PLL_Success) { ALX_CLK_TRACE("ErrSetUpPLL"); ALX_CLK_ASSERT(false); }	// MF: Enabling PLL happend here

	// #4 Attach PLL0 to MainClk
	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

	// #5 Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_AhbClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	// Assert
	(void)me;

	// #1 Set Up ExtClk
	if (CLOCK_SetupExtClocking(16000000U) != kStatus_Success) { ALX_CLK_TRACE("ErrSetUpExtClk"); ALX_CLK_ASSERT(false); }	// MF: Enable clk_in and set up crystal Freq. Enabling XTAL32M happens here
	ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;	// MF: Enable clk_in to system

	// #2 Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(150000000U);

	// #3 Setup PLL
	// #3.1 Prepare Variable
	pll_setup_t pll0Setup;
	AlxClk_SetupPll(me, &pll0Setup, 16000000U, 8U, 1U, 150U);

	// #3.2 Attach ExtClk to PLL0
	CLOCK_AttachClk(kEXT_CLK_to_PLL0);

	// #3.3 Setup PLL Freq
	if (CLOCK_SetPLL0Freq(&pll0Setup) != kStatus_PLL_Success) { ALX_CLK_TRACE("ErrSetUpPLL"); ALX_CLK_ASSERT(false); }	// MF: Enabling PLL happend here

	// #4 Attach PLL0 to MainClk
	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

	// #5 Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_48MHz_FroOsc_96MHz(AlxClk* me)
{
	ALX_CLK_ASSERT(false);	// TV: TODO, FUKA

	// Assert
	(void)me;

	// #1 Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(48000000U);

	// #2 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #3 Attach FROHF to MainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// #4 Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 2U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_24MHz_FroOsc_96MHz(AlxClk* me)
{
	ALX_CLK_ASSERT(false);	// TV: TODO, FUKA

	// Assert
	(void)me;

	// #1 Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(24000000U);

	// #2 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #3 Attach FROHF to MainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// #4 Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 4U, true);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_AhbClk_12MHz_FroOsc_96MHz(AlxClk* me)
{
	ALX_CLK_ASSERT(false);	// TV: TODO, FUKA

	// Assert
	(void)me;

	// #1 Set FLASH wait states for core Freq
	CLOCK_SetFLASHAccessCyclesForFreq(12000000U);

	// #2 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #3 Attach FROHF to MainClk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);

	// #4 Divide SysClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 8U, true);
}


#endif // #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
