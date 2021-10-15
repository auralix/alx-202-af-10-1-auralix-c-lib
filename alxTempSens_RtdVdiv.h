/**
  ******************************************************************************
  * @file alxTempSens_RtdVdiv.h
  * @brief Auralix C Library - ALX Temperature Sensor - Resistor Temperature Detector Voltage Divider Module
  * @version $LastChangedRevision: 4937 $
  * @date $LastChangedDate: 2021-05-02 22:05:40 +0200 (Sun, 02 May 2021) $
  ******************************************************************************
  */

#ifndef ALX_TEMP_SENS_RTD_VDIV_H
#define ALX_TEMP_SENS_RTD_VDIV_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxAdc.h"
#include "alxVdiv.h"	
#include "alxInterpLin.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TEMP_SENS_RTD_VDIV_FILE "alxTempSens_RtdVdiv"

// Assert //
#if defined(_ALX_TEMP_SENS_RTD_VDIV_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TEMP_SENS_RTD_VDIV_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TEMP_SENS_RTD_VDIV_FILE, expr)
#elif defined(_ALX_TEMP_SENS_RTD_VDIV_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TEMP_SENS_RTD_VDIV_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TEMP_SENS_RTD_VDIV_FILE, expr)
#elif defined(_ALX_TEMP_SENS_RTD_VDIV_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TEMP_SENS_RTD_VDIV_ASSERT(expr) ALX_ASSERT_RST(ALX_TEMP_SENS_RTD_VDIV_FILE, expr)
#else
	#define ALX_TEMP_SENS_RTD_VDIV_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TEMP_SENS_RTD_VDIV_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TEMP_SENS_RTD_VDIV_TRACE(...) ALX_TRACE_STD(ALX_TEMP_SENS_RTD_VDIV_FILE, __VA_ARGS__)
#else
	#define ALX_TEMP_SENS_RTD_VDIV_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_TEMP_SENS_RTD_VDIV_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_TEMP_SENS_RTD_VDIV_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_TEMP_SENS_RTD_VDIV_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const
	Alx_Ch chAdc_Vin;
	Alx_Ch chAdc_Vout;
	bool isResRtdLow;
	float resOther_kOhm;

	// Objects - External
	AlxAdc* adc;
	AlxInterpLin *interpLin;
	
	// Objects - Internal
	
	// Variables
	
	// Info
	bool wasCtorCalled; 
} AlxTempSensRtdVdiv;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTempSensRtdVdiv_Ctor
(
	AlxTempSensRtdVdiv* me,
	AlxAdc* adc,
	AlxInterpLin *interpLin,
	Alx_Ch chAdc_Vin,
	Alx_Ch chAdc_Vout,
	bool isResRtdLow,
	float resOther_kOhm
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTempSensRtdVdiv_Init(AlxTempSensRtdVdiv* me);
Alx_Status AlxTempSensRtdVdiv_DeInit(AlxTempSensRtdVdiv* me);
Alx_Status AlxTempSensRtdVdiv_GetTemp_degC(AlxTempSensRtdVdiv* me, float* temp_degC);


#ifdef __cplusplus
}
#endif

#endif // ALX_TEMP_SENS_RTD_VDIV_H
