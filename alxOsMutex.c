//******************************************************************************
// @file alxOsMutex.c
// @brief Auralix C Library - ALX OS Thread Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************


//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsMutex.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
)
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	me->mutex = xSemaphoreCreateBinary();	// MF: Mutex is created once and won't be deleted during a program
	xSemaphoreGive(me->mutex);
	#endif

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsMutex_Lock(AlxOsMutex* me)
{
	// Lock Mutex
	#if defined(ALX_FREE_RTOS)
	xSemaphoreTake(me->mutex, portMAX_DELAY);
	#endif
}
void AlxOsMutex_Unlock(AlxOsMutex* me)
{
	// Unlock Mutex
	#if defined(ALX_FREE_RTOS)
	xSemaphoreGive(me->mutex);
	#endif
}
bool AlxOsMutex_IsMutexUnlocked(AlxOsMutex* me)
{
	// Get Status
	#if defined(ALX_FREE_RTOS)
	return uxSemaphoreGetCount(me->mutex);
	#endif
}


#endif // #if defined(ALX_FREE_RTOS)
