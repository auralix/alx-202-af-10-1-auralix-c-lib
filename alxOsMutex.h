/**
  ******************************************************************************
  * @file alxOsMutex.h
  * @brief Auralix C Library - ALX OS Mutex Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_OS_MUTEX_H
#define ALX_OS_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_OS)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Variables
	#if defined(ALX_FREE_RTOS)
	SemaphoreHandle_t mutex;
	#elif defined (ALX_MBED_OS)
	// TODO
	#else
	#error "Please select OS for your application!"
	#endif

	// Info
	bool wasCtorCalled;
} AlxOsMutex;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsMutex_Ctor
(
	AlxOsMutex* me
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsMutex_Lock(AlxOsMutex* me);
void AlxOsMutex_Unlock(AlxOsMutex* me);
bool AlxOsMutex_IsMutexUnlocked(AlxOsMutex* me);	// TV: Not tested


#endif // #if defined(ALX_OS)

#ifdef __cplusplus
}
#endif

#endif // ALX_OS_MUTEX_H
