/**
  ******************************************************************************
  * @file alxDbgPin.h
  * @brief Auralix C Library - ALX Debug Pin Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_DBG_PIN_H
#define ALX_DBG_PIN_H

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
#if defined(ALX_LPC80x)
#include "alxDbgPin_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxDbgPin;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_DBG_PIN_TOGGLE() do								{ AlxDbgPin_Toggle(&alxDbgPin); } while(false)


#define ALX_DBG_PIN_FILE "alxDbgPin"

// Assert //
#if defined(_ALX_DBG_PIN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_DBG_PIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_DBG_PIN_FILE, expr)
#elif defined(_ALX_DBG_PIN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_DBG_PIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_DBG_PIN_FILE, expr)
#elif defined(_ALX_DBG_PIN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_DBG_PIN_ASSERT(expr) ALX_ASSERT_RST(ALX_DBG_PIN_FILE, expr)
#else
	#define ALX_DBG_PIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_DBG_PIN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_DBG_PIN_TRACE(...) ALX_TRACE_STD(ALX_DBG_PIN_FILE, __VA_ARGS__)
#else
	#define ALX_DBG_PIN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxDbgPin_Init(AlxDbgPin* me);
void AlxDbgPin_DeInit(AlxDbgPin* me);
void AlxDbgPin_Toggle(AlxDbgPin* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_DBG_PIN_H
