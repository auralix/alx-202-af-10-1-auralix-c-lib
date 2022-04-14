//******************************************************************************
// @file alxOsKernel.h
// @brief Auralix C Library - ALX OS Kernel Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************

#ifndef ALX_OS_KERNEL_H
#define ALX_OS_KERNEL_H

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
#define ALX_OS_KERNEL_FILE "alxOsKernel.h"

// Assert //
#if defined(_ALX_OS_KERNEL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_KERNEL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_KERNEL_FILE, expr)
#elif defined(_ALX_OS_KERNEL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_KERNEL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_KERNEL_FILE, expr)
#elif defined(_ALX_OS_KERNEL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_KERNEL_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_KERNEL_FILE, expr)
#else
	#define ALX_OS_KERNEL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_KERNEL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_KERNEL_TRACE(...) ALX_TRACE_STD(ALX_OS_KERNEL_FILE, __VA_ARGS__)
#else
	#define ALX_OS_KERNEL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Info
	bool wasKernelStarted;
	bool wasCtorCalled;
} AlxOsKernel;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxOsKernel_Ctor
(
	AlxOsKernel* me
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxOsKernel_Start(AlxOsKernel* me);


#endif // #if defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif // ALX_OS_KERNEL_H
