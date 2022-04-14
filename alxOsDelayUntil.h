//******************************************************************************
// @file alxOsDelayUntil.h
// @brief Auralix C Library - ALX OS Delay Until Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************

#ifndef ALX_OS_DELAY_UNTIL_H
#define ALX_OS_DELAY_UNTIL_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_FREE_RTOS)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_DELAY_UNTIL_FILE "alxOsDelayUntil.h"

// Assert //
#if defined(_ALX_OS_DELAY_UNTIL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_DELAY_UNTIL_FILE, expr)
#elif defined(_ALX_OS_DELAY_UNTIL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_DELAY_UNTIL_FILE, expr)
#elif defined(_ALX_OS_DELAY_UNTIL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_DELAY_UNTIL_FILE, expr)
#else
	#define ALX_OS_DELAY_UNTIL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_DELAY_UNTIL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_DELAY_UNTIL_TRACE(...) ALX_TRACE_STD(ALX_OS_DELAY_UNTIL_FILE, __VA_ARGS__)
#else
	#define ALX_OS_DELAY_UNTIL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxClk_Tick osTick;

	// Variables
	#if defined(ALX_FREE_RTOS)
	TickType_t xLastWakeTime;
	#endif

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxOsDelayUntil;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsDelayUntil_Ctor
(
	AlxOsDelayUntil* me,
	AlxClk_Tick osTick
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsDelayUntil_Init(AlxOsDelayUntil* me);
void AlxOsDelayUntil_us(AlxOsDelayUntil* me, uint64_t osDelayUntilIncrement_us);
void AlxOsDelayUntil_ms(AlxOsDelayUntil* me, uint64_t osTimeIncrement_ms);
void AlxOsDelayUntil_sec(AlxOsDelayUntil* me, uint64_t osTimeIncrement_sec);
void AlxOsDelayUntil_min(AlxOsDelayUntil* me, uint64_t osTimeIncrement_min);
void AlxOsDelayUntil_hr(AlxOsDelayUntil* me, uint64_t osTimeIncrement_hr);


#endif // #if defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif // ALX_OS_DELAY_UNTIL_H
