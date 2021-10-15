/**
  ******************************************************************************
  * @file alxAdc_Mcu.h
  * @brief Auralix C Library - ALX DAC Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_DAC_MCU_H
#define ALX_DAC_MCU_H

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
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
#include "alxDac_McuStm32.h"

#else
typedef struct {} AlxDac_Mcu;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_DAC_MCU_FILE "alxDac_Mcu"

// Assert //
#if defined(_ALX_DAC_MCU_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_DAC_MCU_ASSERT(expr) ALX_ASSERT_BKPT(ALX_DAC_MCU_FILE, expr)
#elif defined(_ALX_DAC_MCU_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_DAC_MCU_ASSERT(expr) ALX_ASSERT_TRACE(ALX_DAC_MCU_FILE, expr)
#elif defined(_ALX_DAC_MCU_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_DAC_MCU_ASSERT(expr) ALX_ASSERT_RST(ALX_DAC_MCU_FILE, expr)
#else
	#define ALX_DAC_MCU_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_DAC_MCU_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_DAC_MCU_TRACE(...) ALX_TRACE_STD(ALX_DAC_MCU_FILE, __VA_ARGS__)
#else
	#define ALX_DAC_MCU_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxDacMcu_Init(AlxDac_Mcu* me);
Alx_Status AlxDacMcu_Init_CalibrateVref(AlxDac_Mcu* me, float* vref_V);
Alx_Status AlxDacMcu_DeInit(AlxDac_Mcu* me);
Alx_Status AlxDacMcu_SetVoltage_V(AlxDac_Mcu* me, Alx_Ch* ch, float* voltage_V);
Alx_Status AlxDacMcu_SetVoltage_V_CalibrateVref(AlxDac_Mcu* me, Alx_Ch* ch, float* voltage_V, float* vref_V);


#ifdef __cplusplus
}
#endif

#endif // ALX_DAC_MCU_H
