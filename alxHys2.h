/**
  ******************************************************************************
  * @file alxHys2.h
  * @brief Auralix C Library - ALX Hysteresis 2 Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_HYS2_H
#define ALX_HYS2_H

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
#define ALX_HYS2_FILE "alxHys2"

// Assert //
#if defined(_ALX_HYS2_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_HYS2_ASSERT(expr) ALX_ASSERT_BKPT(ALX_HYS2_FILE, expr)
#elif defined(_ALX_HYS2_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_HYS2_ASSERT(expr) ALX_ASSERT_TRACE(ALX_HYS2_FILE, expr)
#elif defined(_ALX_HYS2_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_HYS2_ASSERT(expr) ALX_ASSERT_RST(ALX_HYS2_FILE, expr)
#else
	#define ALX_HYS2_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_HYS2_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_HYS2_TRACE(...) ALX_TRACE_STD(ALX_HYS2_FILE, __VA_ARGS__)
#else
	#define ALX_HYS2_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_HYS2_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_HYS2_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_HYS2_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxHys2_StTop = 0,
	AlxHys2_StMid = 1,
	AlxHys2_StBot = 2
} AlxHys2_St;

typedef struct
{
	// Parameters
	float topHigh;
	float topLow;
	float botHigh;
	float botLow;

	// SM
	AlxHys2_St st;

	// Info
	bool wasCtorCalled;
} AlxHys2;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys2_Ctor
(
	AlxHys2* me,
	float topHigh,
	float topLow,
	float botHigh,
	float botLow
);


//******************************************************************************
// Functions
//******************************************************************************
AlxHys2_St AlxHys2_Process(AlxHys2* me, float in);


#ifdef __cplusplus
}
#endif

#endif // ALX_HYS2_H
