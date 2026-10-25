/**
  ******************************************************************************
  * @file		alxRtcFake.c
  * @brief		Auralix C Library - ALX RTC Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the real-time clock, named after the module it fakes,
  * for a consumer that constructs the clock but does not read it on the host.
  * It satisfies the linker for the constructor and nothing else; a consumer
  * that starts reading the date gives this fake its model. The constructor
  * exists on an MCU family only and is defined under the module header's own
  * guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxRtc.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4)
void AlxRtc_Ctor(AlxRtc* me, AlxRtc_Clk rtcClk, AlxRtc_LseDrive lseDrive)
{
	(void)me; (void)rtcClk; (void)lseDrive;
}
#endif
