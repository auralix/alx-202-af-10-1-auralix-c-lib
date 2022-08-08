/**
  ******************************************************************************
  * @file		alxTpa3255.h
  * @brief		Auralix C Library - ALX Speaker Amplifier TPA3255 Module
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
#ifndef ALX_TPA3255_H
#define ALX_TPA3255_H

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
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_TPA3255_FILE "alxTpa3255.h"

// Assert //
#if defined(_ALX_TPA3255_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_TPA3255_ASSERT(expr) ALX_ASSERT_BKPT(ALX_TPA3255_FILE, expr)
#elif defined(_ALX_TPA3255_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_TPA3255_ASSERT(expr) ALX_ASSERT_TRACE(ALX_TPA3255_FILE, expr)
#elif defined(_ALX_TPA3255_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_TPA3255_ASSERT(expr) ALX_ASSERT_RST(ALX_TPA3255_FILE, expr)
#else
	#define ALX_TPA3255_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_TPA3255_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_TPA3255_TRACE(...) ALX_TRACE_STD(ALX_TPA3255_FILE, __VA_ARGS__)
#else
	#define ALX_TPA3255_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	Tpa3255_St_Ok = 0,
	Tpa3255_St_Error,
	Tpa3255_St_LowLevel,
	Tpa3255_St_Waiting
} AlxTpa3255_St;

typedef struct
{
	// Parameters Const
	float lowLevelTime_ms;

	// Objects - External
	AlxIoPin* do_nRESET;
	AlxIoPin* di_nFAULT;
	AlxIoPin* di_nCLIP_OTW;

	// Objects - Internal
	AlxTimSw tim; // Timer to ReEnable Ic

	// Parameters
	float waitTime_ReEnable_ms;
	float waitTime_Waitning_ms;

	// Variables
	AlxTpa3255_St state;
	bool wasErrAsserted;
	bool wasWarningAsserted;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxTpa3255;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] di_nRESET
  * @param[in] do_nFAULT
  * @param[in] do_nCLIP_OTW
  * @param[in] waitTime_ReEnable_ms
  */
void AlxTpa3255_Ctor
(
	AlxTpa3255* me,
	AlxIoPin* di_nRESET,
	AlxIoPin* do_nFAULT,
	AlxIoPin* do_nCLIP_OTW,
	float waitTime_ReEnable_ms
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_Init(AlxTpa3255* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_DeInit(AlxTpa3255* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_Handle(AlxTpa3255* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_Enable(AlxTpa3255* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_Disable(AlxTpa3255* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxTpa3255_IsErrAsserted(AlxTpa3255* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxTpa3255_IsWarningAsserted(AlxTpa3255* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxTpa3255_WasErrAsserted(AlxTpa3255* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxTpa3255_WasWarningAsserted(AlxTpa3255* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_ClearWasErrAsserted(AlxTpa3255* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxTpa3255_ClearWasWarningAsserted(AlxTpa3255* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_TPA3255_H
