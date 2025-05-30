/**
  ******************************************************************************
  * @file		alxFiltGlitchBool.h
  * @brief		Auralix C Library - ALX Fliter Glitch Bool Module
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
#ifndef ALX_FILT_GLITCH_BOOL_H
#define ALX_FILT_GLITCH_BOOL_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTimSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_FILT_GLITCH_BOOL_FILE "alxFiltGlitch.h"

// Assert //
#if defined(ALX_FILT_GLITCH_BOOL_ASSERT_BKPT_ENABLE)
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FILT_GLITCH_BOOL_FILE, expr)
#elif defined(ALX_FILT_GLITCH_BOOL_ASSERT_TRACE_ENABLE)
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FILT_GLITCH_BOOL_FILE, expr)
#elif defined(ALX_FILT_GLITCH_BOOL_ASSERT_RST_ENABLE)
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) ALX_ASSERT_RST(ALX_FILT_GLITCH_BOOL_FILE, expr)
#else
	#define ALX_FILT_GLITCH_BOOL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_FILT_GLITCH_BOOL_TRACE_ENABLE)
	#define ALX_FILT_GLITCH_BOOL_TRACE(...) ALX_TRACE_WRN(ALX_FILT_GLITCH_BOOL_FILE, __VA_ARGS__)
#else
	#define ALX_FILT_GLITCH_BOOL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	bool valInitial;
	uint64_t stableTrueTime_us;
	uint64_t stableFalseTime_us;

	// Variables
	bool valRaw;
	bool valFiltered;
	AlxTimSw tim;

	// Info
	bool wasCtorCalled;
} AlxFiltGlitchBool;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchBool_Ctor
(
	AlxFiltGlitchBool* me,
	bool valInitial,			// Initial output filtered value
	float stableTrueTime_ms,	// Time that val must be stable true to change output state to true
	float stableFalseTime_ms	// Time that val must be stable false to change output state to false
);
void AlxFiltGlitchBool_Ctor_us
(
	AlxFiltGlitchBool* me,
	bool valInitial,			// Initial output filtered value
	uint64_t stableTrueTime_us,	// Time that val must be stable true to change output state to true
	uint64_t stableFalseTime_us	// Time that val must be stable false to change output state to false
);


//******************************************************************************
// Functions
//******************************************************************************
bool AlxFiltGlitchBool_Process(AlxFiltGlitchBool* me, bool in);
void AlxFiltGlitchBool_Reset(AlxFiltGlitchBool* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FILT_GLITCH_BOOL_H
