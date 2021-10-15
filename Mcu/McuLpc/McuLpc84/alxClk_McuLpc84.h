/**
  ******************************************************************************
  * @file alxClk_McuLpc84.h
  * @brief Auralix C Library - ALX Clock Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_CLK_MCU_LPC84_H
#define ALX_CLK_MCU_LPC84_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC84)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	clock_main_clk_src_t mainClkSource;
	uint32_t clkFreq;
	uint8_t divider;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxClk_Ctor
(
	AlxClk* me,
	clock_main_clk_src_t mainClkSource,
	uint32_t clkFreq,
	uint8_t divider
)
{
	// Parameters
	me->mainClkSource = mainClkSource;
	me->clkFreq = clkFreq;
	me->divider = divider;

	// Info
	me->wasCtorCalled = true;
}


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_CLK_MCU_LPC84_H
