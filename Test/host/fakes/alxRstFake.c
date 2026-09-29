/**
  ******************************************************************************
  * @file		alxRstFake.c
  * @brief		Auralix C Library - ALX Reset Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the reset-cause module, named after the module it
  * fakes. The real module reads why the MCU last reset from its registers; a
  * PC has no reset cause to read, so the fake initialises nothing and traces
  * nothing. The constructor exists on an MCU family only and is defined under
  * the module header's own guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxRst.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4)
void AlxRst_Ctor(AlxRst* me)
{
	(void)me;
}
#endif

Alx_Status AlxRst_Init(AlxRst* me)
{
	(void)me;
	return Alx_Ok;
}

void AlxRst_Trace(AlxRst* me)
{
	(void)me;
}
