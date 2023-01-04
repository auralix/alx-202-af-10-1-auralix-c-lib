/**
  ******************************************************************************
  * @file		alxParamMgmt.h
  * @brief		Auralix C Library - ALX Parameter Management Module
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
#ifndef ALX_PARAM_MGMT_H
#define ALX_PARAM_MGMT_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxParamItem.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_MGMT_FILE "alxParamMgmt.h"

// Assert //
#if defined(_ALX_PARAM_MGMT_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_MGMT_FILE, expr)
#elif defined(_ALX_PARAM_MGMT_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_MGMT_FILE, expr)
#elif defined(_ALX_PARAM_MGMT_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_MGMT_FILE, expr)
#else
	#define ALX_PARAM_MGMT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_PARAM_MGMT_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_PARAM_MGMT_TRACE(...) ALX_TRACE_STD(ALX_PARAM_MGMT_FILE, __VA_ARGS__)
#else
	#define ALX_PARAM_MGMT_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxParamItem** paramItemArr;

	// Parameters
	uint32_t numOfParamItems;

	// Info
	bool wasCtorCalled;
} AlxParamMgmt;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		paramItemArr
  * @param[in]		numOfParamItems
  */
void AlxParamMgmt_Ctor
(
	AlxParamMgmt* me,
	AlxParamItem** paramItemArr,
	uint8_t numOfParamItems
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		name
  * @param[out]		val
  * @param[in]		len
  */
void AlxParamMgmt_GetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		id
  * @param[out]		val
  * @param[in]		len
  */
void AlxParamMgmt_GetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		name
  * @param[in]		val
  * @param[in]		len
  */
void AlxParamMgmt_SetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		id
  * @param[in]		val
  * @param[in]		len
  */
void AlxParamMgmt_SetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		name
  * @param[in]		len
  */
void AlxParamMgmt_SetToDefByName(AlxParamMgmt* me, const char* name, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		id
  * @param[in]		len
  */
void AlxParamMgmt_SetToDefById(AlxParamMgmt* me, uint32_t id, uint32_t len);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		groupId
  */
void AlxParamMgmt_SetToDefGroup(AlxParamMgmt* me, uint8_t groupId);

/**
  * @brief
  * @param[in,out]	me
  */
void AlxParamMgmt_SetToDefAll(AlxParamMgmt* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_MGMT_H
