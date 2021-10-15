/**
  ******************************************************************************
  * @file alxClk.h
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 5926 $
  * @date $LastChangedDate: 2021-09-03 00:31:26 +0200 (Fri, 03 Sep 2021) $
  ******************************************************************************
  */

#ifndef ALX_CLK_H
#define ALX_CLK_H

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
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxClk_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxClk_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxClk_McuLpc84.h"

#elif defined(ALX_LPC80x)
#include "alxClk_McuLpc80x.h"

#else
typedef struct { bool dummy; } AlxClk;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CLK_FILE "alxClk"

// Assert //
#if defined(_ALX_CLK_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_CLK_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CLK_FILE, expr)
#elif defined(_ALX_CLK_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_CLK_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CLK_FILE, expr)
#elif defined(_ALX_CLK_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_CLK_ASSERT(expr) ALX_ASSERT_RST(ALX_CLK_FILE, expr)
#else
	#define ALX_CLK_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_CLK_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_CLK_TRACE(...) ALX_TRACE_STD(ALX_CLK_FILE, __VA_ARGS__)
#else
	#define ALX_CLK_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_CLK_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_CLK_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_CLK_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Variables
//******************************************************************************
extern AlxClk alxClk;


//******************************************************************************
// Specific Functions
//******************************************************************************
Alx_Status AlxClk_Init(AlxClk* me);
uint32_t AlxClk_GetClk_Hz(AlxClk* me, AlxClk_Clk clk);
void AlxClk_Irq_Handle(AlxClk* me);


//******************************************************************************
// Common Functions
//******************************************************************************


#ifdef __cplusplus
}
#endif

#endif // ALX_CLK_H
