/**
  ******************************************************************************
  * @file		alxMcp73831.h
  * @brief		Auralix C Library - ALX Battery Charger MCP73831 Module
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
#ifndef ALX_MCP73831_H
#define ALX_MCP73831_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_MCP73831_FILE "alxMcp73831.h"

// Assert //
#if defined(ALX_MCP73831_ASSERT_BKPT_ENABLE)
	#define ALX_MCP73831_ASSERT(expr) ALX_ASSERT_BKPT(ALX_MCP73831_FILE, expr)
#elif defined(ALX_MCP73831_ASSERT_TRACE_ENABLE)
	#define ALX_MCP73831_ASSERT(expr) ALX_ASSERT_TRACE(ALX_MCP73831_FILE, expr)
#elif defined(ALX_MCP73831_ASSERT_RST_ENABLE)
	#define ALX_MCP73831_ASSERT(expr) ALX_ASSERT_RST(ALX_MCP73831_FILE, expr)
#else
	#define ALX_MCP73831_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_MCP73831_TRACE_ENABLE)
	#define ALX_MCP73831_TRACE_WRN(...) ALX_TRACE_WRN(ALX_MCP73831_FILE, __VA_ARGS__)
#else
	#define ALX_MCP73831_TRACE_WRN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxMcp73831_St_Err = 0,
	AlxMcp73831_St_Shutdown = 1,
	AlxMcp73831_St_Charging = 2,
	AlxMcp73831_St_Standby = 3
} AlxMcp73831_St;
typedef struct
{
	// Parameters
	AlxIoPin* di_STAT;
	bool di_STAT_TriStateReadEnable;

	// Variables
	AlxMcp73831_St st;
	AlxIoPin_TriState di_STAT_TriStateVal;
	bool di_STAT_Val;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxMcp73831;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMcp73831_Ctor
(
	AlxMcp73831* me,
	AlxIoPin* di_STAT,
	bool di_STAT_TriStateReadEnable
);


//******************************************************************************
// Functions
//******************************************************************************
void AlxMcp73831_Init(AlxMcp73831* me);
void AlxMcp73831_DeInit(AlxMcp73831* me);
AlxMcp73831_St AlxMcp73831_GetSt(AlxMcp73831* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_MCP73831_H
