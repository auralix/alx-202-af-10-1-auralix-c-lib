/**
  ******************************************************************************
  * @file alxInterpLin.h
  * @brief Auralix C Library - ALX Linear Interpolation Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_INTERP_LIN_H
#define ALX_INTERP_LIN_H

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
#define ALX_INTERP_LIN_FILE "alxInterpLin"

// Assert //
#if defined(_ALX_INTERP_LIN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_INTERP_LIN_FILE, expr)
#elif defined(_ALX_INTERP_LIN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_INTERP_LIN_FILE, expr)
#elif defined(_ALX_INTERP_LIN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_INTERP_LIN_ASSERT(expr) ALX_ASSERT_RST(ALX_INTERP_LIN_FILE, expr)
#else
	#define ALX_INTERP_LIN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_INTERP_LIN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_INTERP_LIN_TRACE(...) ALX_TRACE_STD(ALX_INTERP_LIN_FILE, __VA_ARGS__)
#else
	#define ALX_INTERP_LIN_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_INTERP_LIN_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_INTERP_LIN_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_INTERP_LIN_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float* xPointArr;
	float* yPointArr;
	uint32_t numOfArrPoints;
	bool isXpointArrRising;
	
	// Objects - Internal
	
	// Variables;
	
	// Info
	bool wasCtorCalled;
	
} AlxInterpLin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxInterpLin_Ctor
(
	AlxInterpLin *me,
	float *xPointArr,			// Points to 0th element of the X arr.
	float *yPointArr,			// Points to 0th element of the Y arr.
	uint32_t numOfArrPoints,	// Length of the arrays
	bool isisXpointArrRising	// True if X[n+1] > X[n]
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxInterpLin_GetY_WithStatus(AlxInterpLin* me, float x, float* y);
float AlxInterpLin_GetY(AlxInterpLin* me, float x); 


#ifdef __cplusplus
}
#endif

#endif // ALX_INTERP_LIN_H
