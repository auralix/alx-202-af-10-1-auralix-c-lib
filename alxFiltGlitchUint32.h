/**
  ******************************************************************************
  * @file		alxFiltGlitchUint32.h
  * @brief		Auralix C Library - ALX Fliter Glitch Uint32 Module
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
#ifndef ALX_FILT_GLITCH_UINT32_H
#define ALX_FILT_GLITCH_UINT32_H

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
#define ALX_FILT_GLITCH_UINT32_FILE "alxFiltGlitchUint32.h"

// Assert //
#if defined(_ALX_FILT_GLITCH_UINT32_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FILT_GLITCH_UINT32_FILE, expr)
#elif defined(_ALX_FILT_GLITCH_UINT32_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FILT_GLITCH_UINT32_FILE, expr)
#elif defined(_ALX_FILT_GLITCH_UINT32_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) ALX_ASSERT_RST(ALX_FILT_GLITCH_UINT32_FILE, expr)
#else
	#define ALX_FILT_GLITCH_UINT32_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_FILT_GLITCH_UINT32_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_FILT_GLITCH_UINT32_TRACE(...) ALX_TRACE_STD(ALX_FILT_GLITCH_UINT32_FILE, __VA_ARGS__)
#else
	#define ALX_FILT_GLITCH_UINT32_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint32_t valInitial;
	float stableTime_ms;

	// Objects - Internal
	AlxTimSw tim; // Timer to measure how long state is stable

	// Variables
	uint32_t valOld;
	uint32_t valFiltered;

	// Info
	bool wasCtorCalled;
} AlxFiltGlitchUint32;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] valInitial
  * @param[in] stableTime_ms
  */
void AlxFiltGlitchUint32_Ctor
(
	AlxFiltGlitchUint32* me,
	uint32_t valInitial,		// Initial output filtered value
	float stableTime_ms			// Time that val must be stable to change output state to new val
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] valNew
  */
uint32_t AlxFiltGlitchUint32_Process(AlxFiltGlitchUint32* me, uint32_t valNew);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_FILT_GLITCH_UINT32_H
