/**
  ******************************************************************************
  * @file alxDac.h
  * @brief Auralix C Library - ALX DAC Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_DAC_H
#define ALX_DAC_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"

// AlxMcu //
#if defined(ALX_DAC_MCU)
#include "alxDac_Mcu.h"
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_DAC_FILE "alxDac"

// Assert //
#if defined(_ALX_DAC_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_DAC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_DAC_FILE, expr)
#elif defined(_ALX_DAC_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_DAC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_DAC_FILE, expr)
#elif defined(_ALX_DAC_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_DAC_ASSERT(expr) ALX_ASSERT_RST(ALX_DAC_FILE, expr)
#else
	#define ALX_DAC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_DAC_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_DAC_TRACE(...) ALX_TRACE_STD(ALX_DAC_FILE, __VA_ARGS__)
#else
	#define ALX_DAC_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_DAC_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_DAC_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_DAC_DBG_PIN(...) do{} while (false)
#endif

// Number of Submodules //
#define ALX_DAC_NUM_OF_SUBMODULES 1


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxDac_Submodule_Mcu = 0
} AlxDac_Submodule;

typedef struct
{
	// Parameters
	AlxDac_Submodule submodule;
	void* submodulePtr;

	// FuncPtr
	Alx_Status(*AlxDac_Init_FuncPtr[ALX_DAC_NUM_OF_SUBMODULES])();
	Alx_Status(*AlxDac_Init_CalibrateVref_FuncPtr[ALX_DAC_NUM_OF_SUBMODULES])();
	Alx_Status(*AlxDac_DeInit_FuncPtr[ALX_DAC_NUM_OF_SUBMODULES])();
	Alx_Status(*AlxDac_SetVoltage_V_FuncPtr[ALX_DAC_NUM_OF_SUBMODULES])();
	Alx_Status(*AlxDac_SetVoltage_V_CalibrateVref_FuncPtr[ALX_DAC_NUM_OF_SUBMODULES])();

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxDac;


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxDac_Ctor(AlxDac* me, AlxDac_Submodule submodule, void *submodulePtr);
Alx_Status AlxDac_Init(AlxDac* me);
Alx_Status AlxDac_Init_CalibrateVref(AlxDac* me, float vref_V);
Alx_Status AlxDac_DeInit(AlxDac* me);
Alx_Status AlxDac_SetVoltage_V(AlxDac* me, Alx_Ch ch, float voltage_V);
Alx_Status AlxDac_SetVoltage_V_CalibrateVref(AlxDac* me, Alx_Ch ch, float voltage_V, float vref_V);


#ifdef __cplusplus
}
#endif

#endif // ALX_DAC_H
