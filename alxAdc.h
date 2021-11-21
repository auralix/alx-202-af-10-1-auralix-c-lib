/**
  ******************************************************************************
  * @file alxAdc.h
  * @brief Auralix C Library - ALX ADC Module
  * @version $LastChangedRevision: 5268 $
  * @date $LastChangedDate: 2021-05-28 19:25:15 +0200 (Fri, 28 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_ADC_H
#define ALX_ADC_H

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
#if defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxAdc_McuStm32.h"

#elif defined(ALX_LPC1769)
#include "alxAdc_McuLpc17.h"

#elif defined(ALX_LPC845)
#include "alxAdc_McuLpc84.h"

#elif defined(ALX_LPC80x_UNDEFINED)	// TV: AlxAdcMcu module doen't exists anymore, need to fix this
#include "alxAdc_McuLpc80x.h"

#else
typedef struct {} AlxAdc;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_ADC_FILE "alxAdc"

// Assert //
#if defined(_ALX_ADC_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_ADC_FILE, expr)
#elif defined(_ALX_ADC_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_ADC_FILE, expr)
#elif defined(_ALX_ADC_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_ADC_ASSERT(expr) ALX_ASSERT_RST(ALX_ADC_FILE, expr)
#else
	#define ALX_ADC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_ADC_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_ADC_TRACE(...) ALX_TRACE_STD(ALX_ADC_FILE, __VA_ARGS__)
#else
	#define ALX_ADC_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxAdc_Init(AlxAdc* me);
Alx_Status AlxAdc_DeInit(AlxAdc* me);
float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch);
uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch);
float AlxAdc_TempSens_GetTemp_degC(AlxAdc* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_ADC_H
