/**
  ******************************************************************************
  * @file		alxClk_McuLpc80x.c
  * @brief		Auralix C Library - ALX Clock Module
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
#include "alxClk_McuLpc80x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk();
static void AlxClk_PeriphGpio_Reset();
static bool AlxClk_AreClkNok(AlxClk* me);

static void AlxClk_SetFro(AlxClk* me);
//static void AlxClk_SetExtClk(AlxClk* me, bool sysOsc);	// MF: Don't know how we will use it
//static void AlxClk_SetSysPll(AlxClk* me);					// MF: Doesn't work on Lpc804

#if defined(ALX_LPC80X)
static void AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me);

static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5(AlxClk* me);
#endif
#if defined(ALX_LPC81X) || defined(ALX_LPC82X) || defined(ALX_LPC83X) || defined(ALX_LPC84X)
// TODO
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
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
	me->systemCoreClock = 0;
	me->coreSysClk = 0;
	me->mainClk = 0;
	me->fro = 0;
	//me->extClk = 0;	// MF: Don't know how we will use it
	//me->frg0 = 0;		// MF: Don't know how we will use it

	#if defined(ALX_LPC80X)
	// MF: Ctor depends on FRO OSC, that's why only 3 different Ctors (for 30, 24 and 18MHz)
	if		(me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)		{ AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)				{ AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz)				{ AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz)	{ AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz)	{ AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz)		{ AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5)				{ AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz)				{ AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5)				{ AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(me); }
	#endif
	#if defined(ALX_LPC81X) || defined(ALX_LPC82X) || defined(ALX_LPC83X) || defined(ALX_LPC84X)
	// MF: NOTE - this Mcus will have the same enum
	#endif
	else																									{ ALX_CLK_ASSERT(false); return; }

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
ALX_WEAK Alx_Status AlxClk_Init(AlxClk* me)
{
	// Assert
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// #1 Reset GPIO Periphery
	AlxClk_PeriphGpio_Reset();

	// #2 Enable GPIO Periphery clock
	AlxClk_PeriphGpio_EnableClk();

	// #3 Init Clocks
	if		(me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)		{ AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)				{ AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz)				{ AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz)	{ AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz)	{ AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz)		{ AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5)				{ AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz)				{ AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5)				{ AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5(me);}
	else																									{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// #4 Check Clocks
	if (AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("ErrCheck"); return Alx_Err; }

	// #5 Update SystemCoreClock
	SystemCoreClockUpdate();

	// #6 Configure SysTick
	if (SysTick_Config(SystemCoreClock / (1000000U / me->tick)) == 1) { ALX_CLK_TRACE("ErrSysTickConfig"); return Alx_Err; }	// MF: In the example it was 1000000 when setting up SysTick

	// #7 Set SystemCoreClock
	me->systemCoreClock = SystemCoreClock;

	// #8 Set isInit
	me->isInit = true;

	// #9 Return OK
	return Alx_Ok;
}
ALX_WEAK uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc8xx_CoreSysClk)		return me->coreSysClk;	// MF: tu variable nakurim
		if (clk == AlxClk_Clk_McuLpc8xx_MainClk)		return me->mainClk;
		if (clk == AlxClk_Clk_McuLpc8xx_Fro)			return me->fro;
		if (clk == AlxClk_Clk_McuLpc8xx_FroDiv)			return me->fro >> 1U;
		//if (clk == AlxClk_Clk_McuLpc8xx_ExtClk)		return me->extClk;		// MF: Don't know how we will use it
		//if (clk == AlxClk_Clk_McuLpc8xx_Frg0)			return me->frg0;		// MF: Don't know how we will use it
	}

	if (clk == AlxClk_Clk_McuLpc8xx_CoreSysClk_Ctor)	return me->coreSysClk_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_MainClk_Ctor)		return me->mainClk_Ctor;
	if (clk == AlxClk_Clk_McuLpc8xx_Fro_Ctor)			return me->fro_Ctor;

	if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz ||
		me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz ||
		me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz)
	{
		if (clk == AlxClk_Clk_McuLpc8xx_LPO_Ctor)		return me->lpo_Ctor;
	}

	ALX_CLK_ASSERT(false); // We shouldn't get here
	return 0;
}
void AlxClk_Irq_Handle(AlxClk* me)
{
	// TODO
	ALX_CLK_ASSERT(false);
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk()
{
	CLOCK_EnableClock(kCLOCK_Gpio0);

	//CLOCK_EnableClock(kCLOCK_Gpio1);					// MF: Doesn't work on Lpc804
}
static void AlxClk_PeriphGpio_Reset()
{
	RESET_PeripheralReset(kGPIO0_RST_N_SHIFT_RSTn);

	//RESET_PeripheralReset(kGPIO1_RST_N_SHIFT_RSTn);	// MF: Doesn't work on Lpc804
}
static void AlxClk_SetFro(AlxClk* me)
{
	POWER_DisablePD(kPDRUNCFG_PD_FRO_OUT);				// MF: Ensures FRO is on
	POWER_DisablePD(kPDRUNCFG_PD_FRO);					// MF: Ensures FRO is on
	//CLOCK_SetFroOutClkSrc(kCLOCK_FroSrcFroOsc);		// MF: Set FRO clock source direct from FRO oscillator // Doesn't work on Lpc804

	CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcFro);			// MF: Select FRO for Main Clk

	/*CLOCK_Select(kFRG0_Clk_From_Fro);					// MF: Select FRO for FRG0		// MF: We migh not need this, I don't understand this
	CLOCK_SetFRG0ClkFreq(15000000U);					// MF: Select FRG0 freq*/
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	me->coreSysClk = CLOCK_GetCoreSysClkFreq();
	me->mainClk = CLOCK_GetMainClkFreq();
	me->fro = CLOCK_GetFroFreq();
	//me->extClk = CLOCK_GetExtClkFreq();				// MF: Don't know how we will use it
	//me->frg0 = CLOCK_GetFRG0ClkFreq();				// MF: Don't know how we will use it

	if		(me->coreSysClk != me->coreSysClk_Ctor)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	else if (me->mainClk != me->mainClk_Ctor)			{ ALX_CLK_TRACE("ErrMainClock");	return true; }
	else if (me->fro != me->fro)						{ ALX_CLK_TRACE("ErrFro");			return true; }

	if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)
	{
		if (CLOCK_GetMainClkFreq()    != 12000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 12000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)
	{
		if (CLOCK_GetMainClkFreq()    != 15000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 15000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz)
	{
		if (CLOCK_GetMainClkFreq()    != 9000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 9000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz)
	{
		if (CLOCK_GetMainClkFreq()    != 15000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 15000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
		if (CLOCK_GetLPOscFreq()      != 1000000U)		{ ALX_CLK_TRACE("ErrLowPwrOsc");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz)
	{
		if (CLOCK_GetMainClkFreq()    != 12000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 12000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
		if (CLOCK_GetLPOscFreq()      != 1000000U)		{ ALX_CLK_TRACE("ErrLowPwrOsc");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz)
	{
		if (CLOCK_GetMainClkFreq()    != 9000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 9000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
		if (CLOCK_GetLPOscFreq()      != 1000000U)		{ ALX_CLK_TRACE("ErrLowPwrOsc");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5)
	{
		if(CLOCK_GetMainClkFreq()    != 15000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 7500000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz)
	{
		if (CLOCK_GetMainClkFreq()    != 12000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 6000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5)
	{
		if (CLOCK_GetMainClkFreq()    != 9000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 4500000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}

	return false;
}
static void AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me)
{
	// me->systemCoreClock	// MF: I do not understand how this sistem with "SystemCoreClock" and "coreSysClk_Ctor" works
	me->coreSysClk_Ctor	= 12000000U;
	me->mainClk_Ctor	= 12000000U;
	me->fro_Ctor		= 24000000U;
	me->lpo_Ctor		=  1000000U;
}
static void AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me)
{
	// me->systemCoreClock	// MF: I do not understand how this sistem with "SystemCoreClock" and "coreSysClk_Ctor" works
	me->coreSysClk_Ctor = 15000000U;
	me->mainClk_Ctor	= 15000000U;
	me->fro_Ctor		= 30000000U;
	me->lpo_Ctor		=  1000000U;
}
static void AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me)
{
	// me->systemCoreClock	// MF: I do not understand how this sistem with "SystemCoreClock" and "coreSysClk_Ctor" works
	me->coreSysClk_Ctor = 9000000U;
	me->mainClk_Ctor	= 9000000U;
	me->fro_Ctor		= 18000000U;
	me->lpo_Ctor		= 1000000U;
}
static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut24M);
	CLOCK_SetCoreSysClkDiv(1);	// MF: Divide main clk to provide the system clk to the core, memories, and the perispherals.
}
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut30M);
	CLOCK_SetCoreSysClkDiv(1);
}
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut18M);
	CLOCK_SetCoreSysClkDiv(1);
}
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz(AlxClk* me)
{
	AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me);

	POWER_DisablePD(kPDRUNCFG_PD_LPOSC);		// MF: Ensure LPOSC is on
	//POWER_EnbaleLPOInDeepPowerDownMode(true);	// MF: Doesn't work on Lpc804
}
static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_LPO_1MHz(AlxClk* me)
{
	AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(me);

	POWER_DisablePD(kPDRUNCFG_PD_LPOSC);		// MF: Ensure LPOSC is on
	//POWER_EnbaleLPOInDeepPowerDownMode(true);	// MF: Doesn't work on Lpc804
}

static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz_LPO_1MHz(AlxClk* me)
{
	AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(me);

	POWER_DisablePD(kPDRUNCFG_PD_LPOSC);		// MF: Ensure LPOSC is on
	//POWER_EnbaleLPOInDeepPowerDownMode(true);	// MF: Doesn't work on Lpc804
}
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut30M);
	CLOCK_SetCoreSysClkDiv(2);
}
static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut24M);
	CLOCK_SetCoreSysClkDiv(2);
}
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut18M);
	CLOCK_SetCoreSysClkDiv(2);
}

#endif // Module Guard
