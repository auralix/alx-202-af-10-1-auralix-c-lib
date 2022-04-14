//******************************************************************************
// @file alxOsDelay.c
// @brief Auralix C Library - ALX OS Delay Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************


//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsDelay.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsDelay_Ctor
(
	AlxOsDelay* me,
	AlxClk_Tick osTick
)
{
	// Parameters
	me->osTick = osTick;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsDelay_us(AlxOsDelay* me, uint64_t osDelay_us)
{
	// #1 Assert
	ALX_OS_DELAY_ASSERT(me->wasCtorCalled == true);
	ALX_OS_DELAY_ASSERT(osDelay_us >= (2 * (uint64_t)me->osTick));
	ALX_OS_DELAY_ASSERT((osDelay_us % (uint64_t)me->osTick) == 0);

	// #2 Convert to osTick
	uint64_t osDelay_osTick = osDelay_us / (uint64_t)me->osTick;

	// #3 Delay
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(osDelay_osTick);
	#endif
}
void AlxOsDelay_ms(AlxOsDelay* me, uint64_t osDelay_ms)		{ AlxOsDelay_us(me, osDelay_ms * 1000); }
void AlxOsDelay_sec(AlxOsDelay* me, uint64_t osDelay_sec)	{ AlxOsDelay_us(me, osDelay_sec * 1000000); }
void AlxOsDelay_min(AlxOsDelay* me, uint64_t osDelay_min)	{ AlxOsDelay_us(me, osDelay_min * 60000000); }
void AlxOsDelay_hr(AlxOsDelay* me, uint64_t osDelay_hr)		{ AlxOsDelay_us(me, osDelay_hr * 3600000000); }


#endif // #if defined(ALX_FREE_RTOS)
