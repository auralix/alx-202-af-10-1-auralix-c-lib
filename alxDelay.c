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
	#if defined(ALX_FREE_RTOS)
	// We should not get here	// MF: FreeRtos lowest delay unit is 1ms
	#else
	uint64_t ticksStart_ns = AlxTick_Get_ns(&alxTick);
	while ((AlxTick_Get_ns(&alxTick) - ticksStart_ns) < delay_ns);
	#endif
}
void AlxDelay_us(uint64_t delay_us)
{
	#if defined(ALX_FREE_RTOS)
	// We should not get here	// MF: FreeRtos lowest delay unit is 1ms
	#else
	AlxDelay_ns(delay_us * 1000);
	#endif
}
void AlxDelay_ms(uint64_t delay_ms)
{
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(delay_ms * 1 * configTICK_RATE_HZ / 1000);
	#else
	AlxDelay_ns(delay_ms * 1000000);
	#endif
}
void AlxDelay_sec(uint64_t delay_sec)
{
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(delay_sec * 1000 * configTICK_RATE_HZ / 1000);
	#else
	AlxDelay_ns(delay_sec * 1000000000);
	#endif
}
void AlxDelay_min(uint64_t delay_min)
{
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(delay_min * 60000 * configTICK_RATE_HZ / 1000);
	#else
	AlxDelay_ns(delay_min * 60000000000);
	#endif
}
void AlxDelay_hr(uint64_t delay_hr)
{
	#if defined(ALX_FREE_RTOS)
	vTaskDelay(delay_hr * 3600000 * configTICK_RATE_HZ / 1000);
	#else
	AlxDelay_ns(delay_hr * 3600000000000);
	#endif
}
