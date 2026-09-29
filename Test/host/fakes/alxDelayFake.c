/**
  ******************************************************************************
  * @file		alxDelayFake.c
  * @brief		Auralix C Library - ALX Delay Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the blocking delay, named after the module it fakes. A
  * delay returns at once: on a PC time is what the test advances, so a delay
  * that waited would only slow the suite down. The calls are COUNTED, because
  * how often a module waits is part of what it does - a status signal that
  * blinks a pin pauses between each write, and a test can assert the pauses.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxDelay.h"


//******************************************************************************
// Private Variables
//******************************************************************************
static uint32_t alxDelayFake_msCount;
static uint32_t alxDelayFake_usCount;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxDelayFake_Reset(void);
uint32_t AlxDelayFake_MsCount(void);
uint32_t AlxDelayFake_UsCount(void);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxDelayFake_Reset(void)
{
	alxDelayFake_msCount = 0;
	alxDelayFake_usCount = 0;
}

uint32_t AlxDelayFake_MsCount(void)
{
	return alxDelayFake_msCount;
}

uint32_t AlxDelayFake_UsCount(void)
{
	return alxDelayFake_usCount;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxDelay_ms(uint64_t delay_ms)
{
	(void)delay_ms;
	alxDelayFake_msCount++;
}

void AlxDelay_us(uint64_t delay_us)
{
	(void)delay_us;
	alxDelayFake_usCount++;
}
