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

static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me);
static void AlxClk_Ctor_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me);

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me);
static void AlxClk_Init_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me);

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

	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default)		{ AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz)				{ AlxClk_Ctor_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0)		{ AlxClk_Ctor_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz)			{ AlxClk_Ctor_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz(me); }

	//else if TODO
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
	if		(me->config == AlxClk_Config_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default)		{ AlxClk_Init_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz)				{ AlxClk_Init_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0)		{ AlxClk_Init_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0(me); }
	else if (me->config == AlxClk_Config_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz)			{ AlxClk_Init_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz(me); }
	else																					{ ALX_CLK_ASSERT(false); return Alx_Err; }

	// #5 Update SystemCoreClock
	//SystemCoreClockUpdate();

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
	// TODO
	ALX_CLK_ASSERT(false); // We shouldn't get here
	return 0;
}
void AlxClk_Irq_Handle(AlxClk* me)
{
	// TODO
	ALX_CLK_ASSERT(false); // We shouldn't get here
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxClk_PeriphGpio_EnableClk()
{
	CLOCK_EnableClock(kCLOCK_Gpio0);
	CLOCK_EnableClock(kCLOCK_Gpio1);
	CLOCK_EnableClock(kCLOCK_Gpio2);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	CLOCK_EnableClock(kCLOCK_Gpio3);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static void AlxClk_PeriphGpio_Reset()
{
	RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO2_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO2 works on Lpc55S6x
	RESET_PeripheralReset(kGPIO3_RST_SHIFT_RSTn);	// MF: I'm not sure GPIO3 works on Lpc55S6x
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	// #1 Get Clks
	me->coreSysClk = CLOCK_GetFreq(kCLOCK_BusClk);		//MF: BusClk is "SysClk" because it is divided "MainClk"
	me->mainClk = CLOCK_GetFreq(kCLOCK_CoreSysClk);		//MF: CoreSysClk is actually "MainClk" (svinjarijo)

	// #2 Check Clks
	if		(me->coreSysClk != me->coreSysClk_Ctor)		{ ALX_CLK_TRACE("ErrCoreSysClock");	return true; }
	else if (me->mainClk != me->mainClk_Ctor)			{ ALX_CLK_TRACE("ErrMainClock");	return true; }
	else												{ return false;}
}

static void AlxClk_Ctor_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	me->coreSysClk_Ctor = 12000000U;
	me->mainClk_Ctor	= 12000000U;
	me->fro_Ctor		= 12000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	me->coreSysClk_Ctor = 48000000U;	// MF: Zato kr fuka
	me->mainClk_Ctor	= 96000000U;
	me->fro_Ctor		= 96000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me)
{
	me->coreSysClk_Ctor = 75000000U;
	me->mainClk_Ctor	= 150000000U;
	me->fro_Ctor		= 12000000U;
}
static void AlxClk_Ctor_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	me->coreSysClk_Ctor = 150000000U;
	me->mainClk_Ctor	= 150000000U;
	me->fro_Ctor		= 12000000U;
}

static void AlxClk_Init_McuLpc55S6x_MainClk_12MHz_SysClk_12MHz_FroOsc_12MHz_Default(AlxClk* me)
{
	// #1 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #2 Select Main Clk
	CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);

	// #3 Divide MainClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1, true);	// MF: Divide mainClk with 1 do get SysClk the same as MainClk
}
static void AlxClk_Init_McuLpc55S6x_SysClk_96MHz_FroOsc_96MHz(AlxClk* me)
{
	// #1 Enable FRO
	POWER_DisablePD(kPDRUNCFG_PD_FRO192M);

	// #3 Divide MainClk
	//CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);	// MF: Fuka, ne dela

	// #2 Select Main Clk
	CLOCK_AttachClk(kFRO_HF_to_MAIN_CLK);
}
static void AlxClk_Init_McuLpc55S6x_SysClk_150MHz_FroOsc_12MHz_Pll0(AlxClk* me)
{
	// #1 Setup PLL
	// #1.1 Prepare Variable
	uint8_t pllN = 12U;
	uint8_t pllP = 1U;
	uint16_t pllM = 150U;

	// SELP
	uint8_t selp = ((pllM / 4U) + 1U);
	if (selp >= 31U)	{ selp = 31U; }

	// SELI
	uint8_t seli = 0U;
	if (pllM >= 8000U)						{ seli = 1U; }
	if ((8000U > pllM) && (pllM >= 122U))	{ seli = 8000U / pllM; }
	if (( 122U > pllM) && (pllM >= 1U))		{ seli = ((2U * (pllM / 4U)) + 3U); }

	if (seli >= 63U)						{ seli = 63U; }

	pll_setup_t pllSetup;
	pllSetup.pllctrl = SYSCON_PLL1CTRL_CLKEN_MASK;// | SYSCON_PLL1CTRL_SELP(selp) | SYSCON_PLL1CTRL_SELI(seli);
	pllSetup.pllndec = SYSCON_PLL1NDEC_NDIV(pllN);
	pllSetup.pllpdec = SYSCON_PLL1PDEC_PDIV(pllP);
	pllSetup.pllmdec = SYSCON_PLL1MDEC_MDIV(pllM);
	//pllSetup.pllsscg[0] = 0;
	//pllSetup.pllsscg[1] = 0;
	pllSetup.pllRate = (((12000000 / pllN) * pllM) / 2 * pllP);//45646433u;//150000000U;
	pllSetup.flags = PLL_SETUPFLAG_WAITLOCK;

	// #1.2 Setup Freq
	if (CLOCK_SetPLL1Freq(&pllSetup) != kStatus_PLL_Success) { ALX_CLK_TRACE("ErrSetUpPLL"); ALX_CLK_ASSERT(false); }	// MF: Enabling PLL happend here

	// #2 Select the Pll Clk
	CLOCK_AttachClk(kFRO12M_to_PLL1);

	// #3 Select the Main Clk
	CLOCK_AttachClk(kPLL1_to_MAIN_CLK);
	
	CLOCK_AttachClk(kMAIN_CLK_to_CLKOUT);

	// #4 Divide Clk
	//CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1, true);	// MF: Fuka, ne dela 
}
static void AlxClk_Init_McuLpc55S6x_SysClk_150MHz_ExtOsc_16MHz(AlxClk* me)
{
	// #1 Set Up ExtClk
	if (CLOCK_SetupExtClocking(16000000U) != kStatus_Success)				// MF: Enable clk_in and set up crystal Freq. Enabling XTAL32M happens here
	{
		ALX_CLK_TRACE("ErrSetUpExtClk");
		ALX_CLK_ASSERT(false);
	}
	SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK;					// MF: Enable clk_in from XTAL32M clock
	ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;	// MF: Enable clk_in to system

	// #2 TODO
	//POWER_SetVoltageForFreq(150000000U);			// MF: This Func only have declaration (but not definition)(it was usend in the example)
	CLOCK_SetFLASHAccessCyclesForFreq(150000000U);	// MF: Set FLASH wait states for core

	// #3 Setup PLL
	POWER_DisablePD(kPDRUNCFG_PD_PLL0);
	POWER_DisablePD(kPDRUNCFG_PD_PLL0_SSCG);
	CLOCK_AttachClk(kEXT_CLK_to_PLL0);
	const pll_setup_t pll0Setup =
	{
		.pllctrl = SYSCON_PLL0CTRL_CLKEN_MASK | SYSCON_PLL0CTRL_SELI(53U) | SYSCON_PLL0CTRL_SELP(31U),
		.pllndec = SYSCON_PLL0NDEC_NDIV(8U),
		.pllpdec = SYSCON_PLL0PDEC_PDIV(1U),
		.pllsscg = { 0x0U, (SYSCON_PLL0SSCG1_MDIV_EXT(150U) | SYSCON_PLL0SSCG1_SEL_EXT_MASK) },
		.pllRate = 150000000U,
		.flags   = PLL_SETUPFLAG_WAITLOCK
	};
	CLOCK_SetPLL0Freq(&pll0Setup);

	// #4 Select Main Clk
	CLOCK_AttachClk(kPLL0_to_MAIN_CLK);

	// #5 Divide MainClk
	CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, true);
}

#endif // Module Guard
