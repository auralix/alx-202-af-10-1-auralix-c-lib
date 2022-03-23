/**
  ******************************************************************************
  * @file alxDelay.c
  * @brief Auralix C Library - ALX Delay Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxDelay.h"


//******************************************************************************
// Functions
//******************************************************************************
void AlxDelay_ns(uint64_t delay_ns)
{
	uint64_t ticksStart_ns = AlxTick_Get_ns(&alxTick);
	while ((AlxTick_Get_ns(&alxTick) - ticksStart_ns) < delay_ns);
}
void AlxDelay_us(uint64_t delay_us)		{ AlxDelay_ns(delay_us  * 1000); }
void AlxDelay_ms(uint64_t delay_ms)		{ AlxDelay_ns(delay_ms  * 1000000); }
void AlxDelay_sec(uint64_t delay_sec)	{ AlxDelay_ns(delay_sec * 1000000000); }
void AlxDelay_min(uint64_t delay_min)	{ AlxDelay_ns(delay_min * 60000000000); }
void AlxDelay_hr(uint64_t delay_hr)		{ AlxDelay_ns(delay_hr  * 3600000000000); }
