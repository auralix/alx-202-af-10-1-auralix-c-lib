/**
  ******************************************************************************
  * @file alxTempSens_RtdVdiv.c
  * @brief Auralix C Library - ALX Temperature Sensor - Resistor Temperature Detector Voltage Divider Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTempSens_RtdVdiv.h"
#include "alxInterpLin.h"
#include "alxAdc.h"
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
)
{
	// Ctor
	
	// Parameters - Const
	me->chAdc_Vin = chAdc_Vin;
	me->chAdc_Vout = chAdc_Vout;
	me->isResRtdLow = isResRtdLow;
	me->resOther_kOhm = resOther_kOhm;
		
	// Objects - External
	me->adc = adc;
	me->interpLin = interpLin;
	
	// Variables

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxTempSensRtdVdiv_Init(AlxTempSensRtdVdiv* me)
{
	ALX_TEMP_SENS_RTD_VDIV_ASSERT(false);
}
Alx_Status AlxTempSensRtdVdiv_DeInit(AlxTempSensRtdVdiv* me)
{
	ALX_TEMP_SENS_RTD_VDIV_ASSERT(false);
}
Alx_Status AlxTempSensRtdVdiv_GetTemp_degC(AlxTempSensRtdVdiv* me, float* temp_degC) 
{
	ALX_TEMP_SENS_RTD_VDIV_ASSERT(me->wasCtorCalled == true);
	
	// Measure Vin and Vout
	float vIn_V = AlxAdc_GetVoltage_V(me->adc, me->chAdc_Vin);
	float vOut_V = AlxAdc_GetVoltage_V(me->adc, me->chAdc_Vout);
	float rtdRes_kOhm = 0;	
	float rtdTemp_degC = 0;	
	
	// Get temperature sensor resistance
	if (me->isResRtdLow) rtdRes_kOhm = AlxVdiv_GetResLow_kOhm(vIn_V, vOut_V, me->resOther_kOhm);
	else rtdRes_kOhm = AlxVdiv_GetResHigh_kOhm(vIn_V, vOut_V, me->resOther_kOhm);
	
	// Get degC through Linear Interpoaltion
	return AlxInterpLin_GetY_WithStatus(me->interpLin, rtdRes_kOhm, temp_degC);
	
	return rtdTemp_degC;
}

