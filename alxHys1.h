/**
  ******************************************************************************
  * @file alxHys1.h
  * @brief Auralix C Library - ALX Hysteresis 1 Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HYS1_H
#define ALX_HYS1_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_HYS1_FILE "alxHys1"

// Assert //
#if defined(_ALX_HYS1_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_HYS1_ASSERT(expr) ALX_ASSERT_BKPT(ALX_HYS1_FILE, expr)
#elif defined(_ALX_HYS1_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_HYS1_ASSERT(expr) ALX_ASSERT_TRACE(ALX_HYS1_FILE, expr)
#elif defined(_ALX_HYS1_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_HYS1_ASSERT(expr) ALX_ASSERT_RST(ALX_HYS1_FILE, expr)
#else
	#define ALX_HYS1_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_HYS1_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_HYS1_TRACE(...) ALX_TRACE_STD(ALX_HYS1_FILE, __VA_ARGS__)
#else
	#define ALX_HYS1_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_HYS1_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_HYS1_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_HYS1_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxHys1_StHigh = 0,
	AlxHys1_StLow = 1
} AlxHys1_St;

typedef struct
{
	// Parameters
	float high; // Threshold for transition to Primary state
	float low; // Threshold for transition to Secondary state

	// SM
	AlxHys1_St st;

	// Info
	bool wasCtorCalled;
} AlxHys1;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys1_Ctor
(
	AlxHys1* me,
	float high,
	float low
);


//******************************************************************************
// Functions
//******************************************************************************
AlxHys1_St AlxHys1_Process(AlxHys1* me, float in);


#ifdef __cplusplus
}
#endif

#endif // ALX_HYS1_H
