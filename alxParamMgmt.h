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
#if defined(ALX_PARAM_MGMT_ASSERT_BKPT_ENABLE)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_MGMT_FILE, expr)
#elif defined(ALX_PARAM_MGMT_ASSERT_TRACE_ENABLE)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_MGMT_FILE, expr)
#elif defined(ALX_PARAM_MGMT_ASSERT_RST_ENABLE)
	#define ALX_PARAM_MGMT_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_MGMT_FILE, expr)
#else
	#define ALX_PARAM_MGMT_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PARAM_MGMT_TRACE_ENABLE)
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
// By Index
//------------------------------------------------------------------------------
AlxParamItem_DataType	AlxParamMgmt_ByIndex_GetDataType(AlxParamMgmt* me, uint32_t index);
AlxParamItem_ParamType	AlxParamMgmt_ByIndex_GetParamType(AlxParamMgmt* me, uint32_t index);
const char*				AlxParamMgmt_ByIndex_GetKey(AlxParamMgmt* me, uint32_t index);
uint32_t				AlxParamMgmt_ByIndex_GetId(AlxParamMgmt* me, uint32_t index);
const char*				AlxParamMgmt_ByIndex_GetGroupKey(AlxParamMgmt* me, uint32_t index);
uint32_t				AlxParamMgmt_ByIndex_GetGroupId(AlxParamMgmt* me, uint32_t index);
void*					AlxParamMgmt_ByIndex_GetValPtr(AlxParamMgmt* me, uint32_t index);
uint32_t				AlxParamMgmt_ByIndex_GetValLen(AlxParamMgmt* me, uint32_t index);
Alx_Status				AlxParamMgmt_ByIndex_Get(AlxParamMgmt* me, uint32_t index, void* val, uint32_t len);	// TV: TODO
Alx_Status				AlxParamMgmt_ByIndex_Set(AlxParamMgmt* me, uint32_t index, void* val, uint32_t len);	// TV: TODO
Alx_Status				AlxParamMgmt_ByIndex_GetVal_StrFormat(AlxParamMgmt* me, uint32_t index, char* val, uint32_t maxLenWithNullTerm);
Alx_Status				AlxParamMgmt_ByIndex_SetVal_StrFormat(AlxParamMgmt* me, uint32_t index, char* val);		// TV: TODO


//------------------------------------------------------------------------------
// By Key
//------------------------------------------------------------------------------
Alx_Status			AlxParamMgmt_ByKey_SetVal_StrFormat(AlxParamMgmt* me, char* key, char* val);


//------------------------------------------------------------------------------
// By ID
//------------------------------------------------------------------------------
Alx_Status			AlxParamMgmt_ById_GetValLen(AlxParamMgmt* me, uint32_t id, uint32_t* valLen);
Alx_Status			AlxParamMgmt_ById_Get(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
Alx_Status			AlxParamMgmt_ById_Set(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);


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


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_MGMT_H
