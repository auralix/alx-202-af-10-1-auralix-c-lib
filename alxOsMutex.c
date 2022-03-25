/**
  ******************************************************************************
  * @file alxOsMutex.c
  * @brief Auralix C Library - ALX OS Mutex Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsMutex.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_OS)


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
	#elif defined (ALX_MBED_OS)
	// TODO
	#else
	#error "Please select OS for your application!"
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
	#elif defined (ALX_MBED_OS)
	// TODO
	#else
	#error "Please select OS for your application!"
	#endif
}
void AlxOsMutex_Unlock(AlxOsMutex* me)
{
	// Unlock Mutex
	#if defined(ALX_FREE_RTOS)
	xSemaphoreGive(me->mutex);
	#elif defined (ALX_MBED_OS)
	// TODO
	#else
	#error "Please select OS for your application!"
	#endif
}
bool AlxOsMutex_IsMutexUnlocked(AlxOsMutex* me)
{
	// Get Status
	#if defined(ALX_FREE_RTOS)
	return uxSemaphoreGetCount(me->mutex);
	#elif defined (ALX_MBED_OS)
	// TODO
	#else
	#error "Please select OS for your application!"
	#endif
}


#endif // #if defined(ALX_OS)
