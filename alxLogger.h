/**
  ******************************************************************************
  * @file		alxLogger.h
  * @brief		Auralix C Library - ALX Logger Module
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
#ifndef ALX_LOGGER_H
#define ALX_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFs.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LOGGER_FILE "alxLogger.h"

// Assert //
#if defined(ALX_LOGGER_ASSERT_BKPT_ENABLE)
	#define ALX_LOGGER_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LOGGER_FILE, expr)
#elif defined(ALX_LOGGER_ASSERT_TRACE_ENABLE)
	#define ALX_LOGGER_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LOGGER_FILE, expr)
#elif defined(ALX_LOGGER_ASSERT_RST_ENABLE)
	#define ALX_LOGGER_ASSERT(expr) ALX_ASSERT_RST(ALX_LOGGER_FILE, expr)
#else
	#define ALX_LOGGER_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_LOGGER_TRACE_ENABLE)
	#define ALX_LOGGER_TRACE(...) ALX_TRACE_STD(ALX_LOGGER_FILE, __VA_ARGS__)
	#define ALX_LOGGER_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_LOGGER_TRACE(...) do{} while (false)
	#define ALX_LOGGER_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines

	// Parameters
	AlxFs* alxFs;

	// Variables

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxLogger;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLogger_Ctor
(
	AlxLogger* me,
	AlxFs* alxFs
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLogger_Init(AlxLogger* me);
Alx_Status AlxLogger_DeInit(AlxLogger* me);
Alx_Status AlxLogger_Handle(AlxLogger* me);
Alx_Status AlxLogger_Trace_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual);
Alx_Status AlxLogger_Trace_WriteLog(AlxLogger* me, void* data, uint32_t len);
Alx_Status AlxLogger_Data_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual);
Alx_Status AlxLogger_Data_WriteLog(AlxLogger* me, void* data, uint32_t len);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LOGGER_H
