/**
  ******************************************************************************
  * @file alxFiltGlitchUint32.h
  * @brief Auralix C Library - ALX Fliter Glitch Uint32 Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_FILT_GLITCH_UINT32_H
#define ALX_FILT_GLITCH_UINT32_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTimSw.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_FILT_GLITCH_UINT32_FILE "alxFiltGlitchUint32"

// Assert //
#if defined(_ALX_FILT_GLITCH_UINT32_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FILT_GLITCH_UINT32_FILE, expr)
#elif defined(_ALX_FILT_GLITCH_UINT32_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FILT_GLITCH_UINT32_FILE, expr)
#elif defined(_ALX_FILT_GLITCH_UINT32_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) ALX_ASSERT_RST(ALX_FILT_GLITCH_UINT32_FILE, expr)
#else
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_FILT_GLITCH_UINT32_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_FILT_GLITCH_UINT32_TRACE(...) ALX_TRACE_STD(ALX_FILT_GLITCH_UINT32_FILE, __VA_ARGS__)
#else
	#define ALX_FILT_GLITCH_UINT32_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_FILT_GLITCH_UINT32_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_FILT_GLITCH_UINT32_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_FILT_GLITCH_UINT32_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint32_t valInitial;
	float stableTime_ms;

	// Objects - Internal
	AlxTimSw tim; // Timer to measure how long state is stable

	// Variables
	uint32_t valOld;
	uint32_t valFiltered;
	
	// Info
	bool wasCtorCalled;
} AlxFiltGlitchUint32;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchUint32_Ctor
(
	AlxFiltGlitchUint32* me,
	uint32_t valInitial,		// Initial output filtered value
	float stableTime_ms			// Time that val must be stable to change output state to new val
);


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxFiltGlitchUint32_Process(AlxFiltGlitchUint32* me, uint32_t valNew);


#ifdef __cplusplus
}
#endif

#endif // ALX_FILT_GLITCH_UINT32_H
