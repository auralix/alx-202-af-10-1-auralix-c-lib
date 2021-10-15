/**
  ******************************************************************************
  * @file alxClk_McuLpc80x.c
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxClk_McuLpc80x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80x)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk();
static void AlxClk_PeriphGpio_Reset();
static bool AlxClk_AreClkNok(AlxClk* me);

static void AlxClk_SetFro(AlxClk* me);
//static void AlxClk_SetExtClk(AlxClk* me, bool sysOsc); // MF: Don't know how we will use it
//static void AlxClk_SetSysPll(AlxClk* me); // Doesn't work on Lpc804

#if defined(ALX_LPC80x)
static void AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me);

static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz(AlxClk* me);
static void AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5(AlxClk* me);
#endif
#if defined(ALX_LPC81x) || defined(ALX_LPC82x) || defined(ALX_LPC83x) || defined(ALX_LPC84x)
// TODO
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
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
	me->coreSysClk = 0;
	me->mainClk = 0;
	me->fro = 0;
	//me->extClk = 0; // MF: Don't know how we will use it
	//me->frg0 = 0; // MF: Don't know how we will use it

	#if defined(ALX_LPC80x)
	if		(me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)		{ AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)				{ AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz)				{ AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz_LPO_1MHz)	{ AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	#endif
	#if defined(ALX_LPC81x) || defined(ALX_LPC82x) || defined(ALX_LPC83x) || defined(ALX_LPC84x)
	// MF: NOTE - this Mcus will have the same enum
	#endif
	else																									{ ALX_CLK_ASSERT(false); return; }

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxClk_Init(AlxClk* me)
{
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
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5)				{ AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_7MHz5(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz)				{ AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_6MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5)				{ AlxClk_Init_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_4MHz5(me);}
	else																									{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// #4 Configure SysTick
	SysTick_Config(SystemCoreClock / (800000U / me->tick)); // MF: In the example it was 1000000 but then tick doesn't count right

	// #5 Check Clocks
	if (AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("ErrCheck"); return Alx_Err; }

	// #6 Set SystemCoreClock variable
	SystemCoreClock = me->coreSysClk;

	// #7 Set isInit
	me->isInit = true;

	// #8 Return OK
	return Alx_Ok;
}
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc8xx_CoreSysClk)		return me->coreSysClk; // tu variable nakurim
		if (clk == AlxClk_Clk_McuLpc8xx_MainClk)		return me->mainClk;
		if (clk == AlxClk_Clk_McuLpc8xx_Fro)			return me->fro;
		if (clk == AlxClk_Clk_McuLpc8xx_FroDiv)			return me->fro >> 1U;
		//if (clk == AlxClk_Clk_McuLpc8xx_ExtClk)		return me->extClk; // MF: Don't know how we will use it
		//if (clk == AlxClk_Clk_McuLpc8xx_Frg0)			return me->frg0; // MF: Don't know how we will use it
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


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk()
{
	CLOCK_EnableClock(kCLOCK_Gpio0);

	//CLOCK_EnableClock(kCLOCK_Gpio1); // Doesn't work on Lpc804

}
static void AlxClk_PeriphGpio_Reset()
{
	RESET_PeripheralReset(kGPIO0_RST_N_SHIFT_RSTn);

	//RESET_PeripheralReset(kGPIO1_RST_N_SHIFT_RSTn); // Doesn't work on Lpc804
}
static void AlxClk_SetFro(AlxClk* me)
{
	POWER_DisablePD(kPDRUNCFG_PD_FRO_OUT);		// Ensure FRO is on
	POWER_DisablePD(kPDRUNCFG_PD_FRO);			// Ensure FRO is on
	//CLOCK_SetFroOutClkSrc(kCLOCK_FroSrcFroOsc);	// Set FRO clock source direct from FRO oscillator   // Doesn't work on Lpc804

	CLOCK_SetMainClkSrc(kCLOCK_MainClkSrcFro);	// Select FRO for Main Clk



	/*CLOCK_Select(kFRG0_Clk_From_Fro);			// Select FRO for FRG0		// MF: We migh not need this , I don't understand this
	CLOCK_SetFRG0ClkFreq(15000000U);			// Select FRG0 freq*/
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	me->coreSysClk = CLOCK_GetCoreSysClkFreq();
	me->mainClk = CLOCK_GetMainClkFreq();
	me->fro = CLOCK_GetFroFreq();
	//me->extClk = CLOCK_GetExtClkFreq(); // MF: Don't know how we will use it
	//me->frg0 = CLOCK_GetFRG0ClkFreq(); // MF: Don't know how we will use it

	if		(me->coreSysClk != me->coreSysClk_Ctor)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	else if (me->mainClk != me->mainClk_Ctor)			{ ALX_CLK_TRACE("ErrMainClock");	return true; }
	else if (me->fro != me->fro)						{ ALX_CLK_TRACE("ErrFro");			return true; }


	if (me->config == AlxClk_Config_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default)
	{
		if		(CLOCK_GetMainClkFreq()    != 12000000U)	{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		else if (CLOCK_GetCoreSysClkFreq() != 12000000U)	{ ALX_CLK_TRACE("ErrCoreSysClock"); return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)
	{
		if		(CLOCK_GetMainClkFreq()    != 15000000U)	{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		else if (CLOCK_GetCoreSysClkFreq() != 15000000U)	{ ALX_CLK_TRACE("ErrCoreSysClock"); return true; }
	}
	else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz)
	{
		if		(CLOCK_GetMainClkFreq()    != 9000000U)	{ ALX_CLK_TRACE("ErrMainClock");		return true; }
		else if (CLOCK_GetCoreSysClkFreq() != 9000000U)	{ ALX_CLK_TRACE("ErrCoreSysClock");		return true; }
	}
	// MF: TODO treba še ostale enume naštancat

	return false;
}

static void AlxClk_Ctor_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me)
{
	me->coreSysClk_Ctor	= 12000000U;
	me->mainClk_Ctor	= 12000000U;
	me->fro_Ctor		= 24000000U;
	me->lpo_Ctor		=  1000000U;
}
static void AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(AlxClk* me)
{
	me->coreSysClk_Ctor = 15000000U;
	me->mainClk_Ctor	= 15000000U;
	me->fro_Ctor		= 30000000U;
	me->lpo_Ctor		=  1000000U;
}
static void AlxClk_Ctor_McuLpc80x_FroOsc_18MHz_Mainclk_9MHz_CoreSysClk_9MHz(AlxClk* me)
{
	me->coreSysClk_Ctor = 9000000U;
	me->mainClk_Ctor	= 9000000U;
	me->fro_Ctor		= 18000000U;
	me->lpo_Ctor		= 1000000U;
}

static void AlxClk_Init_McuLpc80x_FroOsc_24MHz_Mainclk_12MHz_CoreSysClk_12MHz_Default(AlxClk* me)
{
	AlxClk_SetFro(me);

	CLOCK_SetFroOscFreq(kCLOCK_FroOscOut24M);
	CLOCK_SetCoreSysClkDiv(1); // Divide main clk to provide the system clk to the core, memories, and the perispherals.
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

	POWER_DisablePD(kPDRUNCFG_PD_LPOSC);		// Ensure LPOSC is on
	//POWER_EnbaleLPOInDeepPowerDownMode(true);	// Doesn't work on Lpc804
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
