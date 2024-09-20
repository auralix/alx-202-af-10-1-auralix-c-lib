/**
  ******************************************************************************
  * @file		alxPwr.h
  * @brief		Auralix C Library - ALX Power Module
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
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
#ifndef ALX_PWR_H
#define ALX_PWR_H

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
#include "alxFiltGlitchBool.h"
#include "alxHys2.h"
#include "alxIoPin.h"
#include "alxVdiv.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PWR_MODULE "AlxPwr.h"

// Assert //
#if defined(ALX_PWR_ASSERT_BKPT_ENABLE)
	#define ALX_PWR_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PWR_MODULE, expr)
#elif defined(ALX_PWR_ASSERT_TRACE_ENABLE)
	#define ALX_PWR_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PWR_MODULE, expr)
#elif defined(ALX_PWR_ASSERT_RST_ENABLE)
	#define ALX_PWR_ASSERT(expr) ALX_ASSERT_RST(ALX_PWR_MODULE, expr)
#else
	#define ALX_PWR_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PWR_TRACE_ENABLE)
	#define ALX_PWR_TRACE(...) ALX_TRACE_WRN(ALX_PWR_MODULE, __VA_ARGS__)
#else
	#define ALX_PWR_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float vdiv_ResHigh_kOhm;
	float vdiv_ResLow_kOhm;
	float hys2_TopHigh_V;
	float hys2_TopLow_V;
	float hys2_BotHigh_V;
	float hys2_BotLow_V;
	float filtGlitchBool_StableTrueTime_ms;
	float filtGlitchBool_StableFalseTime_ms;

	// Variables
	AlxHys2 hys2;
	AlxFiltGlitchBool filtGlitchBool;
	float adcVal_V;
	float val_V;
	AlxHys2_St hys2_St;
	bool isInRangeRaw;
	bool isInRangeFiltered;

	// Info
	bool wasCtorCalled;
} AlxPwr;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPwr_Ctor
(
	AlxPwr* me,
	float vdiv_ResHigh_kOhm,
	float vdiv_ResLow_kOhm,
	float hys2_TopHigh_V,
	float hys2_TopLow_V,
	float hys2_BotHigh_V,
	float hys2_BotLow_V,
	bool filtGlitchBool_valInitial,
	float filtGlitchBool_StableTrueTime_ms,
	float filtGlitchBool_StableFalseTime_ms
);


//******************************************************************************
// Functions
//******************************************************************************
bool AlxPwr_Process(AlxPwr* me, float adcVal_V);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PWR_H
