/**
  ******************************************************************************
  * @file		alxParamMgmt.h
  * @brief		Auralix C Library - ALX Parameter Management Module
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

#ifndef ALX_PARAM_MGMT_H
#define ALX_PARAM_MGMT_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxParamItem.h"


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

// DbgPin //
#if defined(_ALX_PARAM_MGMT_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_PARAM_MGMT_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_PARAM_MGMT_DBG_PIN(...) do{} while (false)
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

	// Variables

	// Info
	bool wasCtorCalled;
} AlxParamMgmt;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamMgmt_Ctor
(
	AlxParamMgmt* me,
	AlxParamItem** paramItemArr,
	uint8_t numOfParamItems
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxParamMgmt_GetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len);
void AlxParamMgmt_GetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
void AlxParamMgmt_SetByName(AlxParamMgmt* me, const char* name, void* val, uint32_t len);
void AlxParamMgmt_SetById(AlxParamMgmt* me, uint32_t id, void* val, uint32_t len);
void AlxParamMgmt_SetToDefByName(AlxParamMgmt* me, const char* name, uint32_t len);
void AlxParamMgmt_SetToDefById(AlxParamMgmt* me, uint32_t id, uint32_t len);
void AlxParamMgmt_SetToDefGroup(AlxParamMgmt* me, uint8_t groupId);
void AlxParamMgmt_SetToDefAll(AlxParamMgmt* me);


#ifdef __cplusplus
}
#endif

#endif // ALX_PARAM_MGMT_H
