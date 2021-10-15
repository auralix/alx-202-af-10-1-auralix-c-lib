/**
  ******************************************************************************
  * @file alxBtn.h
  * @brief Auralix C Library - ALX Button Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_BTN_H
#define ALX_BTN_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTimSw.h"	
#include "alxFiltGlitchBool.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_BTN_FILE "alxBtn"

// Assert //
#if defined(_ALX_BTN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_BTN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BTN_FILE, expr)
#elif defined(_ALX_BTN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_BTN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BTN_FILE, expr)
#elif defined(_ALX_BTN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_BTN_ASSERT(expr) ALX_ASSERT_RST(ALX_BTN_FILE, expr)
#else
	#define ALX_BTN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_BTN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_BTN_TRACE(...) ALX_TRACE_STD(ALX_BTN_FILE, __VA_ARGS__)
#else
	#define ALX_BTN_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_BTN_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_BTN_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_BTN_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float longTime_ms;
	float filterTime;

	// Objects - Internal
	AlxTimSw timPressed;	// Timer for PressedShort
	AlxFiltGlitchBool filtGlitchBool; 

	// Variables
	bool isPressedOld;
	bool isPressed;
	bool wasPressed;
	bool wasReleased;
	bool isPressedShort;
	bool wasPressedShort;
	bool isPressedLong;
	bool wasPressedLong;
	
	// Info
	bool wasCtorCalled;
} AlxBtn;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBtn_Ctor
(
	AlxBtn* me,
	bool valInitial,	// Initial output filtered value
	float longTime_ms,	// min time of long button press [ms]
	float debounceTime	// time of stable button status to change it's state [ms]
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxBtn_Handle(AlxBtn* me, bool in);
bool AlxBtn_IsPressed(AlxBtn* me); 			// True -> Button is currently pressed
bool AlxBtn_WasPressed(AlxBtn* me);			
bool AlxBtn_WasReleased(AlxBtn* me);
bool AlxBtn_IsPressedShort(AlxBtn* me);
bool AlxBtn_WasPressedShort(AlxBtn* me);
bool AlxBtn_IsPressedLong(AlxBtn* me);
bool AlxBtn_WasPressedLong(AlxBtn* me);
void AlxBtn_ClearWasPressed(AlxBtn* me);
void AlxBtn_ClearWasReleased(AlxBtn* me);
void AlxBtn_ClearWasPressedShort(AlxBtn* me);
void AlxBtn_ClearWasPressedLong(AlxBtn* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_BTN_H
