/**
  ******************************************************************************
  * @file alxIoPinIrq.h
  * @brief Auralix C Library - ALX IO Pin IRQ Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_IRQ_H
#define ALX_IO_PIN_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

// AlxMcu //
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxIoPinIrq_McuStm32.h"

#elif defined(ALX_LPC80x)
//#include "alxIoPinIrq_McuLpc80x.h"	// TV: Multiple definitions, need to fix...

#else
typedef struct { bool dummy; } AlxIoPinIrq;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_IO_PIN_IRQ_FILE "alxIoPinIrq"

// Assert //
#if defined(_ALX_IO_PIN_IRQ_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_IO_PIN_IRQ_ASSERT(expr) ALX_ASSERT_BKPT(ALX_IO_PIN_IRQ_FILE, expr)
#elif defined(_ALX_IO_PIN_IRQ_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_IO_PIN_IRQ_ASSERT(expr) ALX_ASSERT_TRACE(ALX_IO_PIN_IRQ_FILE, expr)
#elif defined(_ALX_IO_PIN_IRQ_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_IO_PIN_IRQ_ASSERT(expr) ALX_ASSERT_RST(ALX_IO_PIN_IRQ_FILE, expr)
#else
	#define ALX_IO_PIN_IRQ_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_IO_PIN_IRQ_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_IO_PIN_IRQ_TRACE(...) ALX_TRACE_STD(ALX_IO_PIN_IRQ_FILE, __VA_ARGS__)
#else
	#define ALX_IO_PIN_IRQ_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPinIrq_Init(AlxIoPinIrq* me);
void AlxIoPinIrq_DeInit(AlxIoPinIrq* me);


//******************************************************************************
// Common Functions
//******************************************************************************


#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_IRQ_H
