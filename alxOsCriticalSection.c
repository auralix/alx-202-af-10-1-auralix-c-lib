//******************************************************************************
// @file alxOsCriticalSection.c
// @brief Auralix C Library - ALX OS Critical Section Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************


//******************************************************************************
// Includes
//******************************************************************************
#include "alxOsCriticalSection.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_FREE_RTOS)


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsCriticalSection_Enter(void)
{
	#if defined(ALX_FREE_RTOS)
	taskENTER_CRITICAL();
	#endif
}
void AlxOsCriticalSection_Exit(void)
{
	#if defined(ALX_FREE_RTOS)
	taskEXIT_CRITICAL();
	#endif
}


#endif // #if defined(ALX_FREE_RTOS)
