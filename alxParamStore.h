/**
  ******************************************************************************
  * @file		alxParamStore.h
  * @brief		Auralix C Library - ALX Parameter Store Module
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
#ifndef ALX_PARAM_STORE_H
#define ALX_PARAM_STORE_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxParamGroup.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_STORE_FILE "alxParamStore.h"

// Assert //
#if defined(_ALX_PARAM_STORE_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PARAM_STORE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_STORE_FILE, expr)
#elif defined(_ALX_PARAM_STORE_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PARAM_STORE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_STORE_FILE, expr)
#elif defined(_ALX_PARAM_STORE_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PARAM_STORE_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_STORE_FILE, expr)
#else
	#define ALX_PARAM_STORE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PARAM_STORE_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PARAM_STORE_TRACE(...) ALX_TRACE_STD(ALX_PARAM_STORE_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_STORE_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxParamStore_St_Init = 0,
	AlxParamStore_St_Err = 1,
	AlxParamStore_St_CheckingGroup1 = 2,
	AlxParamStore_St_CheckingGroup2 = 3,
	AlxParamStore_St_CheckingGroup3 = 4,
	AlxParamStore_St_CheckingGroup4 = 5,
	AlxParamStore_St_WritingGroup1 = 6,
	AlxParamStore_St_WritingGroup2 = 7,
	AlxParamStore_St_WritingGroup3 = 8,
	AlxParamStore_St_WritingGroup4 = 9
} AlxParamStore_St;

typedef struct
{
	// Parameters
	AlxParamGroup** paramGroupArr;
	uint32_t numOfParamGroups;

	// Variables
	AlxParamStore_St st;
	bool isParamGroupInitErr;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxParamStore;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramGroupArr
  * @param[in]		numOfParamGroups
  */
void AlxParamStore_Ctor
(
	AlxParamStore* me,
	AlxParamGroup** paramGroupArr,
	uint32_t numOfParamGroups
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		status
  * @param[in]		numOfParamGroups
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxParamStore_Init(AlxParamStore* me, Alx_Status* status, uint32_t numOfParamGroups);

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamStore_Handle(AlxParamStore* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxParamStore_IsErr(AlxParamStore* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_STORE_H
