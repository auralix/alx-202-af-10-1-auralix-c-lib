/**
  ******************************************************************************
  * @file alxTpa3255.h
  * @brief Auralix C Library - ALX Speaker Amplifier TPA3255 Module
  * @version $LastChangedRevision: 4590 $
  * @date $LastChangedDate: 2021-04-04 03:50:47 +0200 (Sun, 04 Apr 2021) $
  ******************************************************************************
  */

#ifndef ALX_TPA3255_H
#define ALX_TPA3255_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTimSw.h"
#include "alxIoPin.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TPA3255_FILE "alxTpa3255"

// Assert //
#if defined(_ALX_TPA3255_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TPA3255_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TPA3255_FILE, expr)
#elif defined(_ALX_TPA3255_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TPA3255_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TPA3255_FILE, expr)
#elif defined(_ALX_TPA3255_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TPA3255_ASSERT(expr) ALX_ASSERT_RST(ALX_TPA3255_FILE, expr)
#else
	#define ALX_TPA3255_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TPA3255_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TPA3255_TRACE(...) ALX_TRACE_STD(ALX_TPA3255_FILE, __VA_ARGS__)
#else
	#define ALX_TPA3255_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	Tpa3255_St_Ok = 0,
	Tpa3255_St_Error,
	Tpa3255_St_LowLevel,
	Tpa3255_St_Waiting
} AlxTpa3255_status;

typedef struct
{
	// Parameters Const
	float lowLevelTime_ms;
	
	// Objects - External
	AlxIoPin* do_nRESET;
	AlxIoPin* di_nFAULT;
	AlxIoPin* di_nCLIP_OTW;
	
	// Objects - Internal
	AlxTimSw tim; // Timer to ReEnable Ic
	
	// Parameters
	float waitTime_ReEnable_ms;
	float waitTime_Waitning_ms;

	// Variables
	AlxTpa3255_status state;
	bool wasErrAsserted;
	bool wasWarningAsserted;
	
	// Info
	bool isInit;
	bool wasCtorCalled;	
} AlxTpa3255;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTpa3255_Ctor
(
	AlxTpa3255* me,
	AlxIoPin* di_nRESET,
	AlxIoPin* do_nFAULT,
	AlxIoPin* do_nCLIP_OTW,
	float waitTime_ReEnable_ms
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxTpa3255_Init(AlxTpa3255* me);
void AlxTpa3255_DeInit(AlxTpa3255* me);
void AlxTpa3255_Handle(AlxTpa3255* me);
void AlxTpa3255_Enable(AlxTpa3255* me);
void AlxTpa3255_Disable(AlxTpa3255* me);
bool AlxTpa3255_IsErrAsserted(AlxTpa3255* me);
bool AlxTpa3255_IsWarningAsserted(AlxTpa3255* me);
bool AlxTpa3255_WasErrAsserted(AlxTpa3255* me);
bool AlxTpa3255_WasWarningAsserted(AlxTpa3255* me);
void AlxTpa3255_ClearWasErrAsserted(AlxTpa3255* me);
void AlxTpa3255_ClearWasWarningAsserted(AlxTpa3255* me);

#ifdef __cplusplus
}
#endif

#endif // ALX_TPA3255_H
