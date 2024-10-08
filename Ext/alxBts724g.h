﻿/**
  ******************************************************************************
  * @file		alxBts724g.h
  * @brief		Auralix C Library - ALX High Side Power Switch BTS724G Module
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
#ifndef ALX_BTS724G_H
#define ALX_BTS724G_H

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
#include "alxFiltGlitchBool.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_BTS724G_FILE "alxBts724g.h"

// Assert //
#if defined(ALX_BTS724G_ASSERT_BKPT_ENABLE)
	#define ALX_BTS724G_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BTS724G_FILE, expr)
#elif defined(ALX_BTS724G_ASSERT_TRACE_ENABLE)
	#define ALX_BTS724G_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BTS724G_FILE, expr)
#elif defined(ALX_BTS724G_ASSERT_RST_ENABLE)
	#define ALX_BTS724G_ASSERT(expr) ALX_ASSERT_RST(ALX_BTS724G_FILE, expr)
#else
	#define ALX_BTS724G_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_BTS724G_TRACE_ENABLE)
	#define ALX_BTS724G_TRACE(...) ALX_TRACE_WRN(ALX_BTS724G_FILE, __VA_ARGS__)
#else
	#define ALX_BTS724G_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const
	uint32_t stableTrueTime_openLoad_ms;
	uint32_t stableFalseTime_openLoad_ms;
	uint32_t stableTrueTime_overTemp_ms;
	uint32_t stableFalseTime_overTemp_ms;

	// Objects - External
	AlxIoPin* do_HS_IN;
	AlxIoPin* di_HS_ST;

	// Objects - Internal
	AlxFiltGlitchBool filtGlitch_openLoad;
	AlxFiltGlitchBool filtGlitch_overTemp;

	// Variables
	bool isOpenLoadDetected;
	bool isOverTempDetected;
	bool wasOpenLoadDetected;
	bool wasOverTempDetected;
	bool isOutSet;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxBts724g;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBts724g_Ctor
(
	AlxBts724g* me,
	AlxIoPin* do_HS_IN,
	AlxIoPin* di_HS_ST
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxBts724g_Init(AlxBts724g* me);
void AlxBts724g_DeInit(AlxBts724g* me);
void AlxBts724g_Handle(AlxBts724g* me);
void AlxBts724g_SetOut(AlxBts724g* me);
void AlxBts724g_ResetOut(AlxBts724g* me);
void AlxBts724g_WriteOut(AlxBts724g* me, bool state);
bool AlxBts724g_IsOpenLoadDetected(AlxBts724g* me);
bool AlxBts724g_IsOverTempDetected(AlxBts724g* me);
bool AlxBts724g_WasOpenLoadDetected(AlxBts724g* me);
bool AlxBts724g_WasOverTempDetected(AlxBts724g* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BTS724G_H
