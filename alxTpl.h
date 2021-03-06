/**
  ******************************************************************************
  * @file		alxTpl.h
  * @brief		Auralix C Library - ALX Template Module
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

#ifndef ALX_TPL_H
#define ALX_TPL_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTimSw.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TPL_FILE "alxTpl"

// Assert //
#if defined(_ALX_TPL_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TPL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TPL_FILE, expr)
#elif defined(_ALX_TPL_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TPL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TPL_FILE, expr)
#elif defined(_ALX_TPL_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TPL_ASSERT(expr) ALX_ASSERT_RST(ALX_TPL_FILE, expr)
#else
	#define ALX_TPL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TPL_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TPL_TRACE(...) ALX_TRACE_STD(ALX_TPL_FILE, __VA_ARGS__)
#else
	#define ALX_TPL_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_TPL_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_TPL_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_TPL_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters - Const
	
	// Objects - External
	
	// Objects - Internal
	AlxTimSw tim; // Timer to measure how long state is stable
	
	// Parameters
	float stableTrueTime_ms;
	float stableFalseTime_ms;

	// Variables
	bool valRaw;
	bool valFiltered;
	
	// Info
	bool wasCtorCalled;
} AlxTpl;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTpl_Ctor
(
	AlxTpl* me,
	bool valInitial,			// Initial output filtered value
	float stableTrueTime_ms,	// Time that val must be stable true to change output state to true
	float stableFalseTime_ms	// Time that val must be stable false to change output state to false
);


//******************************************************************************
// Functions
//******************************************************************************
bool AlxTpl_Process(AlxTpl* me, bool in);


#ifdef __cplusplus
}
#endif

#endif // ALX_TPL_H
