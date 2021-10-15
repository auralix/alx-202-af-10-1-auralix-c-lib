/**
  ******************************************************************************
  * @file alxClk_McuLpc17.c
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxClk_McuLpc17.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17)


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxClk_Init(AlxClk* me)
{
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

	me->isInit = true;
}
void AlxClk_DeInit(AlxClk* me)
{
	ALX_CLK_ASSERT(me->isInit == true);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_GPIO);

	me->isInit = false;
}


#endif
