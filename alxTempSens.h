/**
  ******************************************************************************
  * @file alxTempSens.h
  * @brief Auralix C Library - ALX Temperature Sensor Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_TEMP_SENS_H
#define ALX_TEMP_SENS_H

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
#if defined(ALX_TEMP_SENS_MCU)
#include "alxTempSens_Mcu.h"
#endif

// AlxRtd //
#if defined(ALX_TEMP_SENS_RTD_VDIV)
#include "alxTempSens_RtdVdiv.h"
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TEMP_SENS_FILE "alxTempSens"

// Assert //
#if defined(_ALX_TEMP_SENS_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TEMP_SENS_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TEMP_SENS_FILE, expr)
#elif defined(_ALX_TEMP_SENS_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TEMP_SENS_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TEMP_SENS_FILE, expr)
#elif defined(_ALX_TEMP_SENS_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TEMP_SENS_ASSERT(expr) ALX_ASSERT_RST(ALX_TEMP_SENS_FILE, expr)
#else
	#define ALX_TEMP_SENS_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TEMP_SENS_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TEMP_SENS_TRACE(...) ALX_TRACE_STD(ALX_TEMP_SENS_FILE, __VA_ARGS__)
#else
	#define ALX_TEMP_SENS_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_TEMP_SENS_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_TEMP_SENS_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_TEMP_SENS_DBG_PIN(...) do{} while (false)
#endif

// Number of Submodules //
#define ALX_TEMP_SENS_NUM_OF_SUBMODULES 2


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxTempSens_Submodule_Mcu = 0,
	AlxTempSens_Submodule_Rtd_Vdiv = 1
} AlxTempSens_Submodule;

typedef struct
{
	// Parameters
	AlxTempSens_Submodule submodule;
	void* submodulePtr;

	// FuncPtr
	Alx_Status(*alxTempSens_Init_FuncPtr[ALX_TEMP_SENS_NUM_OF_SUBMODULES])();
	Alx_Status(*alxTempSens_DeInit_FuncPtr[ALX_TEMP_SENS_NUM_OF_SUBMODULES])();
	Alx_Status(*alxTempSens_GetTemp_degC_FuncPtr[ALX_TEMP_SENS_NUM_OF_SUBMODULES])();

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTempSens;


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxTempSens_Ctor(AlxTempSens* me, AlxTempSens_Submodule submodule, void* submodulePtr);
Alx_Status AlxTempSens_Init(AlxTempSens* me);
Alx_Status AlxTempSens_DeInit(AlxTempSens* me);
//float AlxTempSens_GetTemp_degC(AlxTempSens* me);
Alx_Status AlxTempSens_GetTemp_degC(AlxTempSens* me, float* temp_degC);


#ifdef __cplusplus
}
#endif

#endif // ALX_TEMP_SENS_H
