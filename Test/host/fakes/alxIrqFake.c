/**
  ******************************************************************************
  * @file		alxIrqFake.c
  * @brief		Auralix C Library - ALX IRQ Module - PC test fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The real module masks interrupts with CMSIS intrinsics, which do not exist
  * on a PC. Nothing under test here runs concurrently, so the lock is a no-op -
  * but the calls are COUNTED, because "the module takes the lock around the
  * shared 64 bit tick" is a contract worth being able to assert rather than
  * assume, and a fake that only returns silence cannot answer it.
  ******************************************************************************
  **/

#include "alxGlobal.h"

#if defined(ALX_C_LIB)

static uint32_t alxIrqFake_lockCount = 0;
static uint32_t alxIrqFake_unlockCount = 0;
static int32_t alxIrqFake_depth = 0;
static int32_t alxIrqFake_depthMax = 0;

uint32_t AlxIrq_Lock(void);
void AlxIrq_Unlock(uint32_t key);
void AlxIrqFake_Reset(void);
uint32_t AlxIrqFake_LockCount(void);
uint32_t AlxIrqFake_UnlockCount(void);
int32_t AlxIrqFake_Depth(void);
int32_t AlxIrqFake_DepthMax(void);

uint32_t AlxIrq_Lock(void)
{
	alxIrqFake_lockCount++;
	alxIrqFake_depth++;
	if (alxIrqFake_depth > alxIrqFake_depthMax)
	{
		alxIrqFake_depthMax = alxIrqFake_depth;
	}
	return 0;
}

void AlxIrq_Unlock(uint32_t key)
{
	(void)key;
	alxIrqFake_unlockCount++;
	alxIrqFake_depth--;
}

void AlxIrqFake_Reset(void)
{
	alxIrqFake_lockCount = 0;
	alxIrqFake_unlockCount = 0;
	alxIrqFake_depth = 0;
	alxIrqFake_depthMax = 0;
}

uint32_t AlxIrqFake_LockCount(void)		{ return alxIrqFake_lockCount; }
uint32_t AlxIrqFake_UnlockCount(void)	{ return alxIrqFake_unlockCount; }
int32_t AlxIrqFake_Depth(void)			{ return alxIrqFake_depth; }
int32_t AlxIrqFake_DepthMax(void)		{ return alxIrqFake_depthMax; }

#endif	// #if defined(ALX_C_LIB)
