/**
  ******************************************************************************
  * @file		alxParamGroup.h
  * @brief		Auralix C Library - ALX Parameter Group Module
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
#ifndef ALX_PARAM_GROUP_H
#define ALX_PARAM_GROUP_H

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
#include "alxMemSafe.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_PARAM_GROUP_MODULE "AlxParamGroup.h"

// Assert //
#if defined(ALX_PARAM_GROUP_ASSERT_BKPT_ENABLE)
	#define ALX_PARAM_GROUP_ASSERT(expr) ALX_ASSERT_BKPT(ALX_PARAM_GROUP_MODULE, expr)
#elif defined(ALX_PARAM_GROUP_ASSERT_TRACE_ENABLE)
	#define ALX_PARAM_GROUP_ASSERT(expr) ALX_ASSERT_TRACE(ALX_PARAM_GROUP_MODULE, expr)
#elif defined(ALX_PARAM_GROUP_ASSERT_RST_ENABLE)
	#define ALX_PARAM_GROUP_ASSERT(expr) ALX_ASSERT_RST(ALX_PARAM_GROUP_MODULE, expr)
#else
	#define ALX_PARAM_GROUP_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_PARAM_GROUP_TRACE_ENABLE)
	#define ALX_PARAM_GROUP_TRACE(...) ALX_TRACE_WRN(ALX_PARAM_GROUP_MODULE, __VA_ARGS__)
#else
	#define ALX_PARAM_GROUP_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxMemSafe* memSafe;
	const char* name;
	uint32_t len;
	uint8_t* valBuff;
	uint8_t* valStoredBuff;
	uint8_t* valToStoreBuff;
	AlxParamItem** paramItemArr;
	uint32_t numOfParamItems;
	uint8_t initNumOfTries;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxParamGroup;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamGroup_Ctor
(
	AlxParamGroup* me,
	AlxMemSafe* memSafe,
	const char* name,
	uint32_t len,
	uint8_t* valBuff,
	uint8_t* valStoredBuff,
	uint8_t* valToStoreBuff,
	AlxParamItem** paramItemArr,
	uint32_t numOfParamItems,
	uint8_t initNumOfTries
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxParamGroup_Init(AlxParamGroup* me);
void AlxParamGroup_Write(AlxParamGroup* me);
bool AlxParamGroup_IsWriteDone(AlxParamGroup* me);
bool AlxParamGroup_IsWriteErr(AlxParamGroup* me);
bool AlxParamGroup_IsValStoredBuffDiff(AlxParamGroup* me);
void AlxParamGroup_ValBuffToValToStoreBuff(AlxParamGroup* me);
void AlxParamGroup_ValToStoreBuffToValStoredBuff(AlxParamGroup* me);
void AlxParamGroup_ParamItemsValToValBuff(AlxParamGroup* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_PARAM_GROUP_H
