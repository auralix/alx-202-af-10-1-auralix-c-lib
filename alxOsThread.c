//******************************************************************************
// @file alxOsThread.c
// @brief Auralix C Library - ALX OS Thread Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************


//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsThread.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsThread_Ctor
(
	AlxOsThread* me,
	TaskFunction_t pxTaskCode,
	const char* const pcName,
	const configSTACK_DEPTH_TYPE usStackDepth,
	void* const pvParameters,
	UBaseType_t uxPriority
)
{
	#if defined(ALX_FREE_RTOS)
	// Parameters
	me->pxTaskCode = pxTaskCode;
	me->pcName = pcName;
	me->usStackDepth = usStackDepth;
	me->pvParameters = pvParameters;
	me->uxPriority = uxPriority;

	// Variables
	me->pxCreatedTask = NULL;
	#endif

	// Info
	me->wasThreadStarted = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxOsThread_Start(AlxOsThread* me)
{
	// #1 Assert
	ALX_OS_THREAD_ASSERT(me->wasThreadStarted == false);
	ALX_OS_THREAD_ASSERT(me->wasCtorCalled == true);

	// #2 Start
	#if defined(ALX_FREE_RTOS)
	BaseType_t status = xTaskCreate
	(
		me->pxTaskCode,
		me->pcName,
		me->usStackDepth,
		me->pvParameters,
		me->uxPriority,
		me->pxCreatedTask
	);
	if (status != pdPASS) { ALX_OS_THREAD_TRACE("Err"); return Alx_Err; }
	#endif

	// #3 Return
	return Alx_Ok;
}


#endif // #if defined(ALX_FREE_RTOS)
