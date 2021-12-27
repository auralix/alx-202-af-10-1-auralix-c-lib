/**
  ******************************************************************************
  * @file alxIoPin.h
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 5232 $
  * @date $LastChangedDate: 2021-05-26 12:42:22 +0200 (Wed, 26 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_H
#define ALX_IO_PIN_H

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
#if defined(ALX_STM32F1) ||defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxIoPin_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxIoPin_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxIoPin_McuLpc84.h"

#elif defined(ALX_LPC80X)
#include "alxIoPin_McuLpc80x.h"

#elif defined(ALX_LPC55S6X)
#include "alxIoPin_McuLpc55S6x.h"

#else
typedef struct { bool dummy; } AlxIoPin;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_IO_PIN_FILE "alxIoPin"

// Assert //
#if defined(_ALX_IO_PIN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_IO_PIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_IO_PIN_FILE, expr)
#elif defined(_ALX_IO_PIN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_IO_PIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_IO_PIN_FILE, expr)
#elif defined(_ALX_IO_PIN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_IO_PIN_ASSERT(expr) ALX_ASSERT_RST(ALX_IO_PIN_FILE, expr)
#else
	#define ALX_IO_PIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_IO_PIN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_IO_PIN_TRACE(...) ALX_TRACE_STD(ALX_IO_PIN_FILE, __VA_ARGS__)
#else
	#define ALX_IO_PIN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPin_Init(AlxIoPin* me);
void AlxIoPin_DeInit(AlxIoPin* me);
bool AlxIoPin_Read(AlxIoPin* me);
void AlxIoPin_Write(AlxIoPin* me, bool val);
void AlxIoPin_Set(AlxIoPin* me);
void AlxIoPin_Reset(AlxIoPin* me);
void AlxIoPin_Toggle(AlxIoPin* me);
void AlxIoPin_Lock(AlxIoPin* me);
void AlxIoPin_IrqHandler(AlxIoPin* me);
void AlxIoPin_Config_PullNone(AlxIoPin* me);
void AlxIoPin_Config_PullUp(AlxIoPin* me);
void AlxIoPin_Config_PullDown(AlxIoPin* me);
void AlxIoPin_Config_AssertOn(AlxIoPin* me);
void AlxIoPin_Config_AssertOff(AlxIoPin* me);


//******************************************************************************
// Common Functions
//******************************************************************************


#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_H