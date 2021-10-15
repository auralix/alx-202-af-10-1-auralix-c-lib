/**
  ******************************************************************************
  * @file alxTick.c
  * @brief Auralix C Library - ALX Tick Module
  * @version $LastChangedRevision: 5497 $
  * @date $LastChangedDate: 2021-07-01 20:08:07 +0200 (Thu, 01 Jul 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTick.h"


//******************************************************************************
// Variables
//******************************************************************************
volatile AlxTick alxTick = {0};


//******************************************************************************
// Functions
//******************************************************************************
uint64_t AlxTick_Get_ns(volatile AlxTick* me)
{
	volatile uint64_t tick_ns = 0;

	AlxGlobal_DisableIrq();		// Start of critical section
	tick_ns = me->tick_ns;		// Shared resource tick variable is used by:
								// Background - GetTick (gets current tick value)
								// Foreground - IrqHandler (incremnets tick value)
	AlxGlobal_EnableIrq();		// End of critical section

	return tick_ns;
}
uint64_t AlxTick_Get_us(volatile AlxTick* me)	{ return AlxTick_Get_ns(me) / 1000; }
uint64_t AlxTick_Get_ms(volatile AlxTick* me)	{ return AlxTick_Get_ns(me) / 1000000; }
uint64_t AlxTick_Get_sec(volatile AlxTick* me)	{ return AlxTick_Get_ns(me) / 1000000000; }
uint64_t AlxTick_Get_min(volatile AlxTick* me)	{ return AlxTick_Get_ns(me) / 60000000000; }
uint64_t AlxTick_Get_hr(volatile AlxTick* me)	{ return AlxTick_Get_ns(me) / 3600000000000; }
void AlxTick_Inc_ns(volatile AlxTick* me)		{ me->tick_ns = me->tick_ns + 1; }
void AlxTick_Inc_us(volatile AlxTick* me)		{ me->tick_ns = me->tick_ns + 1000; }
void AlxTick_Inc_ms(volatile AlxTick* me)		{ me->tick_ns = me->tick_ns + 1000000; }
void AlxTick_Inc_sec(volatile AlxTick* me)		{ me->tick_ns = me->tick_ns + 1000000000; }
void AlxTick_Inc_min(volatile AlxTick* me)		{ me->tick_ns = me->tick_ns + 60000000000; }
void AlxTick_Inc_hr(volatile AlxTick* me)		{ me->tick_ns = me->tick_ns + 3600000000000; }
void AlxTick_IncRange_ns(volatile AlxTick* me, uint64_t ticks_ns)	{ me->tick_ns = me->tick_ns + ticks_ns; }
void AlxTick_IncRange_us(volatile AlxTick* me, uint64_t ticks_us)	{ me->tick_ns = me->tick_ns + (ticks_us * 1000); }
void AlxTick_IncRange_ms(volatile AlxTick* me, uint64_t ticks_ms)	{ me->tick_ns = me->tick_ns + (ticks_ms * 1000000); }
void AlxTick_IncRange_sec(volatile AlxTick* me, uint64_t ticks_sec)	{ me->tick_ns = me->tick_ns + (ticks_sec * 1000000000); }
void AlxTick_IncRange_min(volatile AlxTick* me, uint64_t ticks_min)	{ me->tick_ns = me->tick_ns + (ticks_min * 60000000000); }
void AlxTick_IncRange_hr(volatile AlxTick* me, uint64_t ticks_hr)	{ me->tick_ns = me->tick_ns + (ticks_hr * 3600000000000); }
void AlxTick_Reset(volatile AlxTick* me)
{
	AlxGlobal_DisableIrq();	// Start of critical section
	me->tick_ns = 0;		// Shared resource tick variable is used by:
							// Background - GetTick (gets current tick value)
							// Foreground - IrqHandler (incremnets tick value)
	AlxGlobal_EnableIrq();	// End of critical section
}
