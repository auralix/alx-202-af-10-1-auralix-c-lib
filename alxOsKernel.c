//******************************************************************************
// @file alxOsKernel.c
// @brief Auralix C Library - ALX OS Kernel Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************

//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsKernel.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_FREE_RTOS)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsKernel_Ctor
(
	AlxOsKernel* me
)
{
	// Info
	me->wasKernelStarted = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxOsKernel_Start(AlxOsKernel* me)
{
	// #1 Assert
	ALX_OS_KERNEL_ASSERT(me->wasKernelStarted == false);
	ALX_OS_KERNEL_ASSERT(me->wasCtorCalled == true);

	// #2 Start
	#if defined(ALX_FREE_RTOS)
	vTaskStartScheduler();
	#endif

	// #3 Set wasThreadStarted
	me->wasKernelStarted = true;

	// #4 Return
	return Alx_Ok;
}


#endif // #if defined(ALX_FREE_RTOS)
