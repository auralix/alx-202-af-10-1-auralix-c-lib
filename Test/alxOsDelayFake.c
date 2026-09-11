/**
  ******************************************************************************
  * @file		alxOsDelayFake.c
  * @brief		Auralix C Library - ALX OS Delay - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The RTOS sleep, on a host with no RTOS. It RETURNS AT ONCE and counts the
  * milliseconds it was asked for.
  *
  * Returning at once is the only thing a single-threaded test can do with a
  * sleep, and it is worth being explicit that this is a lie: a module whose
  * correctness depends on other work happening during the sleep will look
  * right here and be wrong on a device. What the count gives back is the one
  * honest question a test can still ask - how long the module INTENDED to wait.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsDelay.h"


//******************************************************************************
// Private variables
//******************************************************************************
static uint64_t alxOsDelayFake_total_ms;
static uint32_t alxOsDelayFake_calls;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxOsDelayFake_Reset(void);
uint64_t AlxOsDelayFake_Total_ms(void);
uint32_t AlxOsDelayFake_Calls(void);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxOsDelayFake_Reset(void)
{
	alxOsDelayFake_total_ms = 0;
	alxOsDelayFake_calls = 0;
}

uint64_t AlxOsDelayFake_Total_ms(void)
{
	return alxOsDelayFake_total_ms;
}

uint32_t AlxOsDelayFake_Calls(void)
{
	return alxOsDelayFake_calls;
}


//******************************************************************************
// The faked module
//******************************************************************************
void AlxOsDelay_ms(AlxOsDelay* me, uint64_t osTicks_ms)
{
	(void)me;
	alxOsDelayFake_total_ms += osTicks_ms;
	alxOsDelayFake_calls++;
}
