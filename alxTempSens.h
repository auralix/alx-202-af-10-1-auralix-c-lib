/**
  ******************************************************************************
  * @file		alxTempSens.h
  * @brief		Auralix C Library - ALX Temperature Sensor Module
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
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TEMP_SENS_FILE "alxTempSens.h"

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
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] submodule
  * @param[in] submodulePtr
  */
void AlxTempSens_Ctor
(
	AlxTempSens* me,
	AlxTempSens_Submodule submodule,
	void* submodulePtr
);


//******************************************************************************
// Specific Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] me
  */
Alx_Status AlxTempSens_Init(AlxTempSens* me);

/**
  * @brief
  * @param[in,out] me
  */
Alx_Status AlxTempSens_DeInit(AlxTempSens* me);

/**
  * @brief
  * @param[in] me
  */
//float AlxTempSens_GetTemp_degC(AlxTempSens* me);

/**
  * @brief
  * @param[in] me
  * @param[out] temp_degC
  */
Alx_Status AlxTempSens_GetTemp_degC(AlxTempSens* me, float* temp_degC);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_TEMP_SENS_H
