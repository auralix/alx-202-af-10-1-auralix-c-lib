/**
  ******************************************************************************
  * @file		alxFsSafe.h
  * @brief		Auralix C Library - ALX File System Safe Module
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
#ifndef ALX_FS_SAFE_H
#define ALX_FS_SAFE_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************rp
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
#define ALX_FS_SAFE_FILE "alxFsSafe.h"

// Assert //
#if defined(ALX_FS_SAFE_ASSERT_BKPT_ENABLE)
	#define ALX_FS_SAFE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FS_SAFE_FILE, expr)
#elif defined(ALX_FS_SAFE_ASSERT_TRACE_ENABLE)
	#define ALX_FS_SAFE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FS_SAFE_FILE, expr)
#elif defined(ALX_FS_SAFE_ASSERT_RST_ENABLE)
	#define ALX_FS_SAFE_ASSERT(expr) ALX_ASSERT_RST(ALX_FS_SAFE_FILE, expr)
#else
	#define ALX_FS_SAFE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_FS_SAFE_TRACE_ENABLE)
	#define ALX_FS_SAFE_TRACE_ERR(...) ALX_TRACE_ERR(ALX_FS_SAFE_FILE, __VA_ARGS__)
	#define ALX_FS_SAFE_TRACE_WRN(...) ALX_TRACE_WRN(ALX_FS_SAFE_FILE, __VA_ARGS__)
	#define ALX_FS_SAFE_TRACE_DBG(...) ALX_TRACE_DBG(ALX_FS_SAFE_FILE, __VA_ARGS__)
#else
	#define ALX_FS_SAFE_TRACE_ERR(...) do{} while (false)
	#define ALX_FS_SAFE_TRACE_WRN(...) do{} while (false)
	#define ALX_FS_SAFE_TRACE_DBG(...) do{} while (false)
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
} AlxFsSafe;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFsSafe_Ctor
(
	AlxFsSafe* me,
	AlxFs* alxFs
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFsSafe_File_Read(AlxFsSafe* me, const char* path, void* data, uint32_t len, uint32_t* lenActual);
Alx_Status AlxFsSafe_File_Write(AlxFsSafe* me, const char* path, void* data, uint32_t len);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FS_SAFE_H
