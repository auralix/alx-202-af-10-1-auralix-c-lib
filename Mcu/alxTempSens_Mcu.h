/**
  ******************************************************************************
  * @file alxTempSens_Mcu.h
  * @brief Auralix C Library - ALX Temperature Sensor Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_TEMP_SENS_MCU_H
#define ALX_TEMP_SENS_MCU_H

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
#if defined(ALX_TEMP_SENS_MCU_DUMMY)
#include "alxTempSens_Dummy.h"

#else
typedef struct {} AlxTempSens_Mcu;
#endif


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TEMP_SENS_MCU_FILE "alxTempSens_Mcu"

// Assert //
#if defined(_ALX_TEMP_SENS_MCU_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TEMP_SENS_MCU_FILE, expr)
#elif defined(_ALX_TEMP_SENS_MCU_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TEMP_SENS_MCU_FILE, expr)
#elif defined(_ALX_TEMP_SENS_MCU_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) ALX_ASSERT_RST(ALX_TEMP_SENS_MCU_FILE, expr)
#else
	#define ALX_TEMP_SENS_MCU_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TEMP_SENS_MCU_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TEMP_SENS_MCU_TRACE(...) ALX_TRACE_STD(ALX_TEMP_SENS_MCU_FILE, __VA_ARGS__)
#else
	#define ALX_TEMP_SENS_MCU_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTempSensMcu_Init(AlxTempSens_Mcu* me);
Alx_Status AlxTempSensMcu_DeInit(AlxTempSens_Mcu* me);
float AlxTempSensMcu_GetTemp_degC(AlxTempSens_Mcu* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_TEMP_SENS_MCU_H
