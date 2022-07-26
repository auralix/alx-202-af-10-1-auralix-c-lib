/**
  ******************************************************************************
  * @file		alxTempSens_RtdVdiv.h
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
// Include Guard
//******************************************************************************
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
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TEMP_SENS_RTD_VDIV_FILE "alxTempSens_RtdVdiv.h"

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


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	Alx_Ch chAdc_Vin;
	Alx_Ch chAdc_Vout;
	bool isResRtdLow;
	float resOther_kOhm;

	// Objects - External
	AlxAdc* adc;
	AlxInterpLin *interpLin;

	// Info
	bool wasCtorCalled;
} AlxTempSensRtdVdiv;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in] me
  */
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

/**
  * @brief
  * @param[in] me
  */
Alx_Status AlxTempSensRtdVdiv_Init(AlxTempSensRtdVdiv* me);

/**
  * @brief
  * @param[in] me
  */
Alx_Status AlxTempSensRtdVdiv_DeInit(AlxTempSensRtdVdiv* me);

/**
  * @brief
  * @param[in] me
  */
Alx_Status AlxTempSensRtdVdiv_GetTemp_degC(AlxTempSensRtdVdiv* me, float* temp_degC);


#endif // #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_TEMP_SENS_RTD_VDIV_H
