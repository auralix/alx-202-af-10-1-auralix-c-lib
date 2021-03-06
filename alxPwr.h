/**
  ******************************************************************************
  * @file		alxPwr.h
  * @brief		Auralix C Library - ALX Power Module
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

#ifndef ALX_PWR_H
#define ALX_PWR_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxAdc.h"
#include "alxFiltGlitchBool.h"
#include "alxHys2.h"
#include "alxIoPin.h"
#include "alxVdiv.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PWR_FILE "alxPwr.h"

// Assert //
#if defined(_ALX_PWR_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PWR_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PWR_FILE, expr)
#elif defined(_ALX_PWR_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PWR_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PWR_FILE, expr)
#elif defined(_ALX_PWR_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PWR_ASSERT(expr) ALX_ASSERT_RST(ALX_PWR_FILE, expr)
#else
	#define ALX_PWR_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PWR_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PWR_TRACE(...) ALX_TRACE_STD(ALX_PWR_FILE, __VA_ARGS__)
#else
	#define ALX_PWR_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxAdc* adc;
	Alx_Ch chAdc;
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
	bool isGoodRaw;
	bool isGoodFiltered;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxPwr;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPwr_Ctor
(
	AlxPwr* me,
	AlxAdc* adc,
	Alx_Ch chAdc,
	float vdiv_ResHigh_kOhm,
	float vdiv_ResLow_kOhm,
	float hys2_TopHigh_V,
	float hys2_TopLow_V,
	float hys2_BotHigh_V,
	float hys2_BotLow_V,
	float filtGlitchBool_StableTrueTime_ms,
	float filtGlitchBool_StableFalseTime_ms
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxPwr_Init(AlxPwr* me);
void AlxPwr_DeInit(AlxPwr* me);
void AlxPwr_Handle(AlxPwr* me);
bool AlxPwr_IsGood(AlxPwr* me);
bool AlxPwr_IsNotGood(AlxPwr* me);
bool AlxPwr_IsErr(AlxPwr* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_PWR_H
