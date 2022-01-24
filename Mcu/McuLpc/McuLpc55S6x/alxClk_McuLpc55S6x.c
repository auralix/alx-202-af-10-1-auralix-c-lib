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
static void AlxClk_SetupPll(AlxClk* me, pll_setup_t* pllSetup, uint32_t inputFreq, uint8_t pllN, uint8_t pllP, uint16_t pllM);

static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me);

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me);


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
	ALX_CLK_ASSERT(tick >= AlxClk_Tick_100us);

	// Parameters
	me->config = config;
	me->tick = tick;

	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default)	{ AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0)	{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz)			{ AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz(me); }
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

	// #1 Reset GPIO Periphery
	AlxClk_PeriphGpio_Reset();

	// #2 Enable GPIO Periphery clock
	AlxClk_PeriphGpio_EnableClk();

	// #3 Init Clocks
	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default)	{ AlxClk_Init_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0)	{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz)			{ AlxClk_Init_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz(me); }
	else																								{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// #4 Check Clocks
	if (AlxClk_AreClkNok(me)) { ALX_CLK_TRACE("ErrCheck"); return Alx_Err; }

	// #5 Update SystemCoreClock
	SystemCoreClockUpdate();

	// #6 Configure SysTick
	if (SysTick_Config(SystemCoreClock / (1000000U / me->tick)) == 1) { ALX_CLK_TRACE("ErrSysTickConfig"); return Alx_Err; }	// MF: In the example it was 1000000 when setting up SysTick

	// #7 Set SystemCoreClock
	me->systemCoreClock = SystemCoreClock;

	// #8 Enable IOCON	// MF: it has to always be on. see: "alxWiki_McuLpc55S6x.md" for more info
	CLOCK_EnableClock(kCLOCK_Iocon);

	// #9 Set isInit
	me->isInit = true;

	// #10 Return OK
	return Alx_Ok;
}
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk)
{
	// Assert
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	// #1 Return Clk Freq
	if (me->isInit)
	{
		if (clk == AlxClk_Clk_McuLpc55s6x_CoreSysClk)	{ return me->coreSysClk; }
		if (clk == AlxClk_Clk_McuLpc55s6x_MainClk)		{ return me->mainClk; }
		if (clk == AlxClk_Clk_McuLpc55s6x_Fro)			{ return me->fro; }
	}

	if (clk == AlxClk_Clk_McuLpc55s6x_CoreSysClk_Ctor)	{ return me->coreSysClk_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_MainClk_Ctor)		{ return me->mainClk_Ctor; }
	if (clk == AlxClk_Clk_McuLpc55s6x_Fro_Ctor)			{ return me->fro_Ctor; }

	// Assert
	ALX_CLK_ASSERT(false); // We shouldn't get here
	return 0;
}
void AlxClk_Irq_Handle(AlxClk* me)
{
	(void)me;

	// TODO
	ALX_CLK_ASSERT(false); // We shouldn't get here
	return;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk()
{
	// #1 Enable GPIO
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	CLOCK_EnableClock(kCLOCK_Gpio3);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static void AlxClk_PeriphGpio_Reset()
{
	// #1 Reset GPIO
	RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	// #1 Get Clks
	me->coreSysClk = CLOCK_GetFreq(kCLOCK_BusClk);	//MF: BusClk is "SysClk" because it is divided "MainClk"
	me->mainClk = CLOCK_GetFreq(kCLOCK_CoreSysClk);	//MF: CoreSysClk is actually "MainClk" (svinjarijo)
	me->fro = me->fro_Ctor;							// MF: Don't Know if we need it

	// #2 Check Clks
	if		(me->coreSysClk != me->coreSysClk_Ctor)	{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	else if (me->mainClk != me->mainClk_Ctor)		{ ALX_CLK_TRACE("ErrMainClock");	return true; }
	else											{ return false; }
}
static void AlxClk_SetupPll(AlxClk* me, pll_setup_t* pllSetup, uint32_t inputFreq, uint8_t pllN, uint8_t pllP, uint16_t pllM)
{
	(void)me;

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

static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	// #1 Set Ctor Clk Freq
	me->coreSysClk_Ctor	= 6000000U;
	me->mainClk_Ctor	= 12000000U;
	me->fro_Ctor		= 12000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	// #1 Set Ctor Clk Freq
	me->coreSysClk_Ctor	= 96000000U;
	me->mainClk_Ctor	= 96000000U;
	me->fro_Ctor		= 96000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me)
{
	// #1 Set Ctor Clk Freq
	me->coreSysClk_Ctor	= 150000000U;
	me->mainClk_Ctor	= 150000000U;
	me->fro_Ctor		= 12000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	// #1 Set Ctor Clk Freq
	me->coreSysClk_Ctor	= 150000000U;
	me->mainClk_Ctor	= 150000000U;
	me->fro_Ctor		= 12000000U;
}

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_SysClk_6MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	(void)me;

	// #1 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #2 Attach FRO12 to MainClk
	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);
}
static void AlxClk_Init_McuLpc55S6x_MainClk_96MHz_SysClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
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
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me)
{
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
static void AlxClk_Init_McuLpc55S6x_MainClk_150MHz_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	// #1 Set Up ExtClk
	if (CLOCK_SetupExtClocking(16000000U) != kStatus_Success) { ALX_CLK_TRACE("ErrSetUpExtClk"); ALX_CLK_ASSERT(false); }	// MF: Enable clk_in and set up crystal Freq. Enabling XTAL32M happens here
	SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK;					// MF: Enable clk_in from XTAL32M clock
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


#endif // Module Guard
