/**
  ******************************************************************************
  * @file		alxSocket.h
  * @brief		Auralix C Library - ALX Socket Module
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
#ifndef ALX_SOCKET_H
#define ALX_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_SOCKET_FILE "alxSocket.h"

// Assert //
#if defined(ALX_SOCKET_ASSERT_BKPT_ENABLE)
	#define ALX_SOCKET_ASSERT(expr) ALX_ASSERT_BKPT(ALX_SOCKET_FILE, expr)
#elif defined(ALX_SOCKET_ASSERT_TRACE_ENABLE)
	#define ALX_SOCKET_ASSERT(expr) ALX_ASSERT_TRACE(ALX_SOCKET_FILE, expr)
#elif defined(ALX_SOCKET_ASSERT_RST_ENABLE)
	#define ALX_SOCKET_ASSERT(expr) ALX_ASSERT_RST(ALX_SOCKET_FILE, expr)
#else
	#define ALX_SOCKET_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_SOCKET_TRACE_ENABLE)
	#define ALX_SOCKET_TRACE(...) ALX_TRACE_STD(ALX_SOCKET_FILE, __VA_ARGS__)
	#define ALX_SOCKET_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_SOCKET_TRACE(...) do{} while (false)
	#define ALX_SOCKET_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Defines

	// Parameters

	// Variables

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxSocket;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxSocket_Ctor
(
	AlxSocket* me
);


//******************************************************************************
// Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_SOCKET_H
