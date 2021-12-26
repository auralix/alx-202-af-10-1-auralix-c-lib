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
	CLOCK_EnableClock(kCLOCK_Gpio2);	// MF: I'm not sure this works on Lpc55S6x
	CLOCK_EnableClock(kCLOCK_Gpio3);	// MF: I'm not sure this works on Lpc55S6x
}
static void AlxClk_PeriphGpio_Reset()
{
	RESET_PeripheralReset(kGPIO0_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kGPIO1_RST_SHIFT_RSTn);
}
static bool AlxClk_AreClkNok(AlxClk* me)
{
	me->coreSysClk = CLOCK_GetCoreSysClkFreq();
	me->mainClk = CLOCK_GetMainClkFreq();
	me->fro = CLOCK_GetFroFreq();
}

#endif // Module Guard
