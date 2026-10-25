/**
  ******************************************************************************
  * @file		alxTickFake.c
  * @brief		Auralix C Library - ALX Tick Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A clock that advances BY ITSELF, one step per read.
  *
  * Everywhere else in this suite the tick is real and the test moves it, which
  * is what makes a timer's boundaries exact. That cannot work for AlxDelay: the
  * delay is a busy wait that spins on the clock and never returns to the
  * caller, so nothing on the test's side would ever get the chance to advance
  * it. On a target the clock moves because an interrupt moves it while the loop
  * spins, and this fake is that arrangement written down - every read comes
  * back a step later than the one before.
  *
  * The step is the test's to choose, which is what lets a test say how coarse
  * the interrupt is relative to the delay being asked for.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxTick.h"


//******************************************************************************
// Private variables
//******************************************************************************
AlxTick alxTick = {};						// the real module's global, which AlxDelay reads through

static uint64_t alxTickFake_now_ns;
static uint64_t alxTickFake_step_ns = 1;
static uint32_t alxTickFake_reads;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxTickFake_Reset(uint64_t step_ns);
void AlxTickFake_SetNow_ns(uint64_t now_ns);
uint64_t AlxTickFake_Now_ns(void);
uint32_t AlxTickFake_Reads(void);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxTickFake_Reset(uint64_t step_ns)
{
	alxTickFake_now_ns = 0;
	alxTickFake_step_ns = step_ns;
	if (step_ns == 0) { alxTickFake_step_ns = 1; }	// a clock that never moves never returns
	alxTickFake_reads = 0;
}

void AlxTickFake_SetNow_ns(uint64_t now_ns)
{
	// Start the clock wherever a test needs it - near the 64 bit wrap, for instance.
	alxTickFake_now_ns = now_ns;
}

uint64_t AlxTickFake_Now_ns(void)
{
	// What the clock reads WITHOUT advancing it, so a test can measure the delay it just waited.
	return alxTickFake_now_ns;
}

uint32_t AlxTickFake_Reads(void)
{
	return alxTickFake_reads;
}


//******************************************************************************
// The faked module
//******************************************************************************
uint64_t AlxTick_Get_ns(AlxTick* me)
{
	(void)me;

	uint64_t now_ns = alxTickFake_now_ns;
	alxTickFake_now_ns += alxTickFake_step_ns;	// the interrupt, between one read and the next
	alxTickFake_reads++;
	return now_ns;
}
