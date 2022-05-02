/**
  ******************************************************************************
  * @file alxFiltGlitchBool.h
  * @brief Auralix C Library - ALX Fliter Glitch Bool Module
  * @version $LastChangedRevision: 5398 $
  * @date $LastChangedDate: 2021-06-11 11:19:51 +0200 (Fri, 11 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_FILT_GLITCH_BOOL_H
#define ALX_FILT_GLITCH_BOOL_H

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
#define ALX_FILT_GLITCH_BOOL_FILE "alxFiltGlitch"

// Assert //
#if defined(_ALX_FILT_GLITCH_BOOL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FILT_GLITCH_BOOL_FILE, expr)
#elif defined(_ALX_FILT_GLITCH_BOOL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FILT_GLITCH_BOOL_FILE, expr)
#elif defined(_ALX_FILT_GLITCH_BOOL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) ALX_ASSERT_RST(ALX_FILT_GLITCH_BOOL_FILE, expr)
#else
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_FILT_GLITCH_BOOL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_FILT_GLITCH_BOOL_TRACE(...) ALX_TRACE_STD(ALX_FILT_GLITCH_BOOL_FILE, __VA_ARGS__)
#else
	#define ALX_FILT_GLITCH_BOOL_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_FILT_GLITCH_BOOL_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_FILT_GLITCH_BOOL_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_FILT_GLITCH_BOOL_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint64_t stableTrueTime_us;
	uint64_t stableFalseTime_us;

	// Objects - Internal
	AlxTimSw tim; // Timer to measure how long state is stable

	// Variables
	bool valRaw;
	bool valFiltered;

	// Info
	bool wasCtorCalled;
} AlxFiltGlitchBool;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchBool_Ctor
(
	AlxFiltGlitchBool* me,
	bool valInitial,			// Initial output filtered value
	float stableTrueTime_ms,	// Time that val must be stable true to change output state to true
	float stableFalseTime_ms	// Time that val must be stable false to change output state to false
);

void AlxFiltGlitchBool_Ctor_us
(
	AlxFiltGlitchBool* me,
	bool valInitial,			// Initial output filtered value
	uint64_t stableTrueTime_us,	// Time that val must be stable true to change output state to true
	uint64_t stableFalseTime_us	// Time that val must be stable false to change output state to false
);


//******************************************************************************
// Functions
//******************************************************************************
bool AlxFiltGlitchBool_Process(AlxFiltGlitchBool* me, bool in);


#ifdef __cplusplus
}
#endif

#endif // ALX_FILT_GLITCH_BOOL_H
