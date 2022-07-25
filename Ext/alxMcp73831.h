/**
  ******************************************************************************
  * @file		alxMcp73831.h
  * @brief		Auralix C Library - ALX Battery Charger MCP73831 Module
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
#ifndef ALX_MCP73831_H
#define ALX_MCP73831_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"
#include "alxIoPin.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MCP73831_FILE "alxMcp73831.h"

// Assert //
#if defined(_ALX_MCP73831_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_MCP73831_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MCP73831_FILE, expr)
#elif defined(_ALX_MCP73831_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_MCP73831_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MCP73831_FILE, expr)
#elif defined(_ALX_MCP73831_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_MCP73831_ASSERT(expr) ALX_ASSERT_RST(ALX_MCP73831_FILE, expr)
#else
	#define ALX_MCP73831_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_MCP73831_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_MCP73831_TRACE(...) ALX_TRACE_STD(ALX_MCP73831_FILE, __VA_ARGS__)
#else
	#define ALX_MCP73831_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	AlxIoPin* di_STAT;

	// Variables

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxMcp73831;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMcp73831_Ctor
(
	AlxMcp73831* me,
	AlxIoPin* di_STAT
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxMcp73831_Init(AlxMcp73831* me);
void AlxMcp73831_DeInit(AlxMcp73831* me);
bool AlxMcp73831_IsBatCharging(AlxMcp73831* me);
bool AlxMcp73831_IsBatFull(AlxMcp73831* me);
bool AlxMcp73831_TriState_IsBatCharging(AlxMcp73831* me);
bool AlxMcp73831_TriState_IsBatFull(AlxMcp73831* me);
bool AlxMcp73831_TriState_IsShutdown(AlxMcp73831* me);


#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_MCP73831_H
