/**
  ******************************************************************************
  * @file alxTick.h
  * @brief Auralix C Library - ALX Tick Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_TICK_H
#define ALX_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Internal
	volatile uint64_t tick_ns;

	// Info
	bool wasCtorCalled;
} AlxTick;


//******************************************************************************
// Variables
//******************************************************************************
extern volatile AlxTick alxTick;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxTick_Ctor
(
	volatile AlxTick* me
)
{
	// Internal
	me->tick_ns = 0;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
uint64_t AlxTick_Get_ns(volatile AlxTick* me);
uint64_t AlxTick_Get_us(volatile AlxTick* me);
uint64_t AlxTick_Get_ms(volatile AlxTick* me);
uint64_t AlxTick_Get_sec(volatile AlxTick* me);
uint64_t AlxTick_Get_min(volatile AlxTick* me);
uint64_t AlxTick_Get_hr(volatile AlxTick* me);
void AlxTick_Inc_ns(volatile AlxTick* me);
void AlxTick_Inc_us(volatile AlxTick* me);
void AlxTick_Inc_ms(volatile AlxTick* me);
void AlxTick_Inc_sec(volatile AlxTick* me);
void AlxTick_Inc_min(volatile AlxTick* me);
void AlxTick_Inc_hr(volatile AlxTick* me);
void AlxTick_IncRange_ns(volatile AlxTick* me, uint64_t ticks_ns);
void AlxTick_IncRange_us(volatile AlxTick* me, uint64_t ticks_us);
void AlxTick_IncRange_ms(volatile AlxTick* me, uint64_t ticks_ms);
void AlxTick_IncRange_sec(volatile AlxTick* me, uint64_t ticks_sec);
void AlxTick_IncRange_min(volatile AlxTick* me, uint64_t ticks_min);
void AlxTick_IncRange_hr(volatile AlxTick* me, uint64_t ticks_hr);
void AlxTick_Reset(volatile AlxTick* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_TICK_H
