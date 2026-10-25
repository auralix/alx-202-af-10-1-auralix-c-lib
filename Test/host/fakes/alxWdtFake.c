/**
  ******************************************************************************
  * @file		alxWdtFake.c
  * @brief		Auralix C Library - ALX Watchdog Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the watchdog, named after the module it fakes. Nothing
  * bites on a PC, so the fake never resets anything - but it COUNTS the
  * refreshes, because a product refreshes once per pass of its main loop, and
  * that count is how a test measures the loop's rate against the time it
  * advanced. The constructor exists on an MCU family only and is defined
  * under the module header's own guard.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxWdt.h"


//******************************************************************************
// Private Variables
//******************************************************************************
static uint32_t alxWdtFake_refreshCount;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxWdtFake_Reset(void);
uint32_t AlxWdtFake_RefreshCount(void);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxWdtFake_Reset(void)
{
	alxWdtFake_refreshCount = 0;
}

uint32_t AlxWdtFake_RefreshCount(void)
{
	return alxWdtFake_refreshCount;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
void AlxWdt_Ctor(AlxWdt* me, AlxWdt_Config config)
{
	(void)me; (void)config;
}
#endif

Alx_Status AlxWdt_Init(AlxWdt* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxWdt_Refresh(AlxWdt* me)
{
	(void)me;
	alxWdtFake_refreshCount++;
	return Alx_Ok;
}
