/**
  ******************************************************************************
  * @file alxTpl.h
  * @brief Auralix C Library - ALX Template Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_TPL_H
#define ALX_TPL_H

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
#define ALX_TPL_FILE "alxTpl"

// Assert //
#if defined(_ALX_TPL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TPL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TPL_FILE, expr)
#elif defined(_ALX_TPL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TPL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TPL_FILE, expr)
#elif defined(_ALX_TPL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TPL_ASSERT(expr) ALX_ASSERT_RST(ALX_TPL_FILE, expr)
#else
	#define ALX_TPL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TPL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TPL_TRACE(...) ALX_TRACE_STD(ALX_TPL_FILE, __VA_ARGS__)
#else
	#define ALX_TPL_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_TPL_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_TPL_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_TPL_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const
	
	// Objects - External
	
	// Objects - Internal
	AlxTimSw tim; // Timer to measure how long state is stable
	
	// Parameters
	float stableTrueTime_ms;
	float stableFalseTime_ms;

	// Variables
	bool valRaw;
	bool valFiltered;
	
	// Info
	bool wasCtorCalled;
} AlxTpl;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTpl_Ctor
(
	AlxTpl* me,
	bool valInitial,			// Initial output filtered value
	float stableTrueTime_ms,	// Time that val must be stable true to change output state to true
	float stableFalseTime_ms	// Time that val must be stable false to change output state to false
);


//******************************************************************************
// Functions
//******************************************************************************
bool AlxTpl_Process(AlxTpl* me, bool in);


#ifdef __cplusplus
}
#endif

#endif // ALX_TPL_H
