/**
  ******************************************************************************
  * @file		alxTempSens_RtdVdiv.c
  * @brief		Auralix C Library - ALX Temperature Sensor - Resistor Temperature Detector Voltage Divider Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
  *
  * @section License
  *
  * SPDX-License-Identifier: GPL-3.0-or-later
  *
  * This file is part of Auralix C Library.
  *
  * Auralix C Library is free software: you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation, either version 3
  * of the License, or (at your option) any later version.
  *
  * Auralix C Library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Auralix C Library. If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTempSens_RtdVdiv.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


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


#endif // #if defined(ALX_C_LIB)
