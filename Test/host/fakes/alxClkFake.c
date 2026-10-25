/**
  ******************************************************************************
  * @file		alxClkFake.c
  * @brief		Auralix C Library - ALX Clock Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the clock module, named after the module it fakes. The
  * real module configures the MCU's clock tree; a PC has none, so the fake
  * constructs and initialises nothing and says it succeeded. On an MCU family
  * the module's own constructor exists and is defined here under the same
  * guard the module header uses.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxClk.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F0) || defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
void AlxClk_Ctor(AlxClk* me, AlxClk_Config config)
{
	(void)me; (void)config;
}
#endif

Alx_Status AlxClk_Init(AlxClk* me)
{
	(void)me;
	return Alx_Ok;
}
