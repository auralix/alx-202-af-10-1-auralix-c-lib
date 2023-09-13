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
	// Parameters
	AlxParamItem* paramItemArr;
	uint32_t numOfParamItems;

	// Info
	bool wasCtorCalled;
} AlxParamMgmt;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamMgmt_Ctor
(
	AlxParamMgmt* me,
	AlxParamItem* paramItemArr,
	uint32_t numOfParamItems
);


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
uint32_t AlxParamMgmt_GetNumOfParamItems(AlxParamMgmt* me);


//------------------------------------------------------------------------------
// Get & Set By Index
//------------------------------------------------------------------------------
const char* AlxParamMgmt_ByIndex_GetKey(AlxParamMgmt* me, uint32_t index);
uint32_t AlxParamMgmt_ByIndex_GetGroupId(AlxParamMgmt* me, uint32_t index);
AlxParamItem_Type AlxParamMgmt_ByIndex_GetType(AlxParamMgmt* me, uint32_t index);
Alx_Status AlxParamMgmt_ByIndex_GetVal_StrFormat(AlxParamMgmt* me, uint32_t index, char* val, uint32_t maxLenWithNullTerm);


//------------------------------------------------------------------------------
// Get & Set By Key
//------------------------------------------------------------------------------
Alx_Status AlxParamMgmt_ByKey_SetVal_StrFormat(AlxParamMgmt* me, char* key, char* val);


//------------------------------------------------------------------------------
// Get & Set By ID
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Set Default
//------------------------------------------------------------------------------
void AlxParamMgmt_SetValToDef_Group(AlxParamMgmt* me, uint32_t groupId);
void AlxParamMgmt_SetValToDef_All(AlxParamMgmt* me);


//------------------------------------------------------------------------------
// Load & Store
//------------------------------------------------------------------------------
Alx_Status AlxParamMgmt_LoadVal_Group(AlxParamMgmt* me, uint32_t groupId);
Alx_Status AlxParamMgmt_LoadVal_All(AlxParamMgmt* me);
Alx_Status AlxParamMgmt_StoreVal_Group(AlxParamMgmt* me, uint32_t groupId);
Alx_Status AlxParamMgmt_StoreVal_All(AlxParamMgmt* me);


//------------------------------------------------------------------------------
// TODO
//------------------------------------------------------------------------------
Alx_Status AlxParamMgmt_ByKey_GetId(AlxParamMgmt* me, const char* key, uint32_t* id);
Alx_Status AlxParamMgmt_ByKey_GetGroupId(AlxParamMgmt* me, const char* key, uint32_t* groupId);
Alx_Status AlxParamMgmt_ByKey_GetType(AlxParamMgmt* me, const char* key, AlxParamItem_Type* type);
Alx_Status AlxParamMgmt_ByKey_GetValPtr(AlxParamMgmt* me, const char* key, void* valPtr);
Alx_Status AlxParamMgmt_ByKey_GetValLen(AlxParamMgmt* me, const char* key, uint32_t* valLen);
Alx_Status AlxParamMgmt_ById_GetKey(AlxParamMgmt* me, uint32_t id, char* key);
Alx_Status AlxParamMgmt_GetByKey(AlxParamMgmt* me, const char* key, void* val, uint32_t len);
Alx_Status AlxParamMgmt_GetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
Alx_Status AlxParamMgmt_SetByKey(AlxParamMgmt* me, const char* key, void* val, uint32_t len);
Alx_Status AlxParamMgmt_SetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
Alx_Status AlxParamMgmt_SetToDefByKey(AlxParamMgmt* me, const char* key, uint32_t len);
Alx_Status AlxParamMgmt_SetToDefById(AlxParamMgmt* me, uint32_t id, uint32_t len);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_MGMT_H
