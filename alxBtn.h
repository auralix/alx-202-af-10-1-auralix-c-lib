/**
  ******************************************************************************
  * @file		alxBtn.h
  * @brief		Auralix C Library - ALX Button Module
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
#ifndef ALX_BTN_H
#define ALX_BTN_H

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
#include "alxFiltGlitchBool.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_BTN_FILE "alxBtn.h"

// Assert //
#if defined(_ALX_BTN_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_BTN_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BTN_FILE, expr)
#elif defined(_ALX_BTN_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_BTN_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BTN_FILE, expr)
#elif defined(_ALX_BTN_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_BTN_ASSERT(expr) ALX_ASSERT_RST(ALX_BTN_FILE, expr)
#else
	#define ALX_BTN_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_BTN_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_BTN_TRACE(...) ALX_TRACE_STD(ALX_BTN_FILE, __VA_ARGS__)
#else
	#define ALX_BTN_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	float longTime_ms;
	float filterTime;

	// Objects - Internal
	AlxTimSw timPressed;	// Timer for PressedShort
	AlxFiltGlitchBool filtGlitchBool;

	// Variables
	bool isPressedOld;
	bool isPressed;
	bool wasPressed;
	bool wasReleased;
	bool isPressedShort;
	bool wasPressedShort;
	bool isPressedLong;
	bool wasPressedLong;

	// Info
	bool wasCtorCalled;
} AlxBtn;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] valInitial
  * @param[in] longTime_ms
  * @param[in] debounceTime
  */
void AlxBtn_Ctor
(
	AlxBtn* me,
	bool valInitial,	// Initial output filtered value
	float longTime_ms,	// min time of long button press [ms]
	float debounceTime	// time of stable button status to change it's state [ms]
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] in
  */
void AlxBtn_Handle(AlxBtn* me, bool in);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_IsPressed(AlxBtn* me);	// True -> Button is currently pressed

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_WasPressed(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_WasReleased(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_IsPressedShort(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_WasPressedShort(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_IsPressedLong(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
bool AlxBtn_WasPressedLong(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBtn_ClearWasPressed(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBtn_ClearWasReleased(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBtn_ClearWasPressedShort(AlxBtn* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBtn_ClearWasPressedLong(AlxBtn* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BTN_H
