/**
  ******************************************************************************
  * @file		alxBool.h
  * @brief		Auralix C Library - ALX Bool Module
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
#ifndef ALX_BOOL_H
#define ALX_BOOL_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFiltGlitchBool.h"
#include "alxTimSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_BOOL_FILE "alxBool.h"

// Assert //
#if defined(ALX_BOOL_ASSERT_BKPT_ENABLE)
	#define ALX_BOOL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BOOL_FILE, expr)
#elif defined(ALX_BOOL_ASSERT_TRACE_ENABLE)
	#define ALX_BOOL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BOOL_FILE, expr)
#elif defined(ALX_BOOL_ASSERT_RST_ENABLE)
	#define ALX_BOOL_ASSERT(expr) ALX_ASSERT_RST(ALX_BOOL_FILE, expr)
#else
	#define ALX_BOOL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_BOOL_TRACE_ENABLE)
	#define ALX_BOOL_TRACE(...) ALX_TRACE_WRN(ALX_BOOL_FILE, __VA_ARGS__)
#else
	#define ALX_BOOL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	bool valInitial;
	float trueShortTime_ms;
	float trueLongTime_ms;
	float falseShortTime_ms;
	float falseLongTime_ms;
	float filtGlitchBool_StableTrueTime_ms;
	float filtGlitchBool_StableFalseTime_ms;

	// Variables
	bool valRaw;
	bool valFiltered;
	AlxFiltGlitchBool alxFiltGlitchBool;

	AlxTimSw alxTimSw_True;
	float trueTime_ms;
	bool isTrue;
	bool isTrueUpToShortTime;
	bool isTrueUpToLongTime;
	bool isTrueForLongTime;
	bool wasTrue;
	bool wasTrueForShortTime;
	bool wasTrueForLongTime;

	AlxTimSw alxTimSw_False;
	float falseTime_ms;
	bool isFalse;
	bool isFalseUpToShortTime;
	bool isFalseUpToLongTime;
	bool isFalseForLongTime;
	bool wasFalse;
	bool wasFalseForShortTime;
	bool wasFalseForLongTime;

	// Info
	bool wasCtorCalled;
} AlxBool;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBool_Ctor
(
	AlxBool* me,
	bool valInitial,
	float trueShortTime_ms,
	float trueLongTime_ms,
	float falseShortTime_ms,
	float falseLongTime_ms,
	float filtGlitchBool_StableTrueTime_ms,
	float filtGlitchBool_StableFalseTime_ms
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxBool_Update(AlxBool* me, bool val);
bool AlxBool_IsTrue(AlxBool* me);
bool AlxBool_IsTrueUpToShortTime(AlxBool* me);
bool AlxBool_IsTrueUpToLongTime(AlxBool* me);
bool AlxBool_IsTrueForLongTime(AlxBool* me);
bool AlxBool_WasTrue(AlxBool* me);
bool AlxBool_WasTrueForShortTime(AlxBool* me);
bool AlxBool_WasTrueForLongTime(AlxBool* me);
void AlxBool_ClearWasTrueFlag(AlxBool* me);
void AlxBool_ClearWasTrueForShortTimeFlag(AlxBool* me);
void AlxBool_ClearWasTrueForLongTimeFlag(AlxBool* me);
bool AlxBool_IsFalse(AlxBool* me);
bool AlxBool_IsFalseUpToShortTime(AlxBool* me);
bool AlxBool_IsFalseUpToLongTime(AlxBool* me);
bool AlxBool_IsFalseForLongTime(AlxBool* me);
bool AlxBool_WasFalse(AlxBool* me);
bool AlxBool_WasFalseForShortTime(AlxBool* me);
bool AlxBool_WasFalseForLongTime(AlxBool* me);
void AlxBool_ClearWasFalseFlag(AlxBool* me);
void AlxBool_ClearWasFalseForShortTimeFlag(AlxBool* me);
void AlxBool_ClearWasFalseForLongTimeFlag(AlxBool* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BOOL_H
