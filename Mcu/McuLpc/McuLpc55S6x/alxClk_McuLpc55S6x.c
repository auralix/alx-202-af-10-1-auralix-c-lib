/**
  ******************************************************************************
  * @file alxClk_McuLpc55S6x.c
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxClk_McuLpc55S6x.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk();
static void AlxClk_PeriphGpio_Reset();
static bool AlxClk_AreClkNok(AlxClk* me);

static void AlxClk_Ctor_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me);

static void AlxClk_Init_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me);

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

	if		(me->config == AlxClk_Config_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default)		{ AlxClk_Ctor_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default(me); }
	//else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)				{ AlxClk_Ctor_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else																					{ ALX_CLK_ASSERT(false); return; }

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
Alx_Status AlxClk_Init(AlxClk* me)
{
	// Assert
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// #1 Reset GPIO Periphery
	AlxClk_PeriphGpio_Reset();

	// #2 Enable GPIO Periphery clock
	AlxClk_PeriphGpio_EnableClk();

	// #3 Init Clocks
	if		(me->config == AlxClk_Config_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default)		{ AlxClk_Init_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default(me); }
	//else if (me->config == AlxClk_Config_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz)				{ AlxClk_Init_McuLpc80x_FroOsc_30MHz_Mainclk_15MHz_CoreSysClk_15MHz(me); }
	else																					{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// #4 Check Clocks
	//if (AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("ErrCheck"); return Alx_Err; }

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
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);


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
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);
	CLOCK_EnableClock(kCLOCK_Gpio3);
}
static void AlxClk_PeriphGpio_Reset()
{
	RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);	// MF: I'm not sure this works on Lpc55S6x
	RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);	// MF: I'm not sure this works on Lpc55S6x
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	me->coreSysClk = CLOCK_GetCoreSysClkFreq();
	//me->mainClk = CLOCK_GetMainClkFreq();
	//me->fro = CLOCK_GetFroFreq();

	if		(me->coreSysClk != me->coreSysClk_Ctor)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	//else if (me->mainClk != me->mainClk_Ctor)			{ ALX_CLK_TRACE("ErrMainClock");	return true; }
	//else if (me->fro != me->fro)						{ ALX_CLK_TRACE("ErrFro");			return true; }

	if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default)
	{
		//if (CLOCK_GetMainClkFreq()    != 12000000U)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
		if (CLOCK_GetCoreSysClkFreq() != 12000000U)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	}
}

static void AlxClk_Ctor_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	me->coreSysClk_Ctor = 12000000U;
	me->mainClk_Ctor	= 12000000U;
	me->fro_Ctor		= 12000000U;
}

static void AlxClk_Init_McuLpc55S6x_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	//AlxClk_SetFro(me);
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);	// MF: Ensures FRO is on

	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                /*!< Switch to FRO 12MHz first to ensure we can change the clock setting */
}

#endif // Module Guard
