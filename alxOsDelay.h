//******************************************************************************
// @file alxOsDelay.h
// @brief Auralix C Library - ALX OS Delay Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************

#ifndef ALX_OS_DELAY_H
#define ALX_OS_DELAY_H

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
#if defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_DELAY_FILE "alxOsDelay.h"

// Assert //
#if defined(_ALX_OS_DELAY_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_DELAY_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_DELAY_FILE, expr)
#elif defined(_ALX_OS_DELAY_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_DELAY_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_DELAY_FILE, expr)
#elif defined(_ALX_OS_DELAY_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_DELAY_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_DELAY_FILE, expr)
#else
	#define ALX_OS_DELAY_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_DELAY_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_DELAY_TRACE(...) ALX_TRACE_STD(ALX_OS_DELAY_FILE, __VA_ARGS__)
#else
	#define ALX_OS_DELAY_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxClk_Tick osTick;
	bool approxDisable;

	// Info
	bool wasCtorCalled;
} AlxOsDelay;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxOsDelay alxOsDelay;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsDelay_Ctor
(
	AlxOsDelay* me,
	AlxClk_Tick osTick,
	bool approxDisable
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsDelay_us(AlxOsDelay* me, uint64_t osDelay_us);
void AlxOsDelay_ms(AlxOsDelay* me, uint64_t osDelay_ms);
void AlxOsDelay_sec(AlxOsDelay* me, uint64_t osDelay_sec);
void AlxOsDelay_min(AlxOsDelay* me, uint64_t osDelay_min);
void AlxOsDelay_hr(AlxOsDelay* me, uint64_t osDelay_hr);


#endif // #if defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif // ALX_OS_DELAY_H
