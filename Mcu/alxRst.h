﻿/**
  ******************************************************************************
  * @file		alxRst.h
  * @brief		Auralix C Library - ALX Reset Module
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
#ifndef ALX_RST_H
#define ALX_RST_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"

#if defined(ALX_STM32F7) || defined(ALX_STM32L4)
#include "alxRst_McuStm32.h"

#else
typedef struct { bool dummy; } AlxRst;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_RST_FILE "alxRst.h"

// Assert //
#if defined(ALX_RST_ASSERT_BKPT_ENABLE)
	#define ALX_RST_ASSERT(expr) ALX_ASSERT_BKPT(ALX_RST_FILE, expr)
#elif defined(ALX_RST_ASSERT_TRACE_ENABLE)
	#define ALX_RST_ASSERT(expr) ALX_ASSERT_TRACE(ALX_RST_FILE, expr)
#elif defined(ALX_RST_ASSERT_RST_ENABLE)
	#define ALX_RST_ASSERT(expr) ALX_ASSERT_RST(ALX_RST_FILE, expr)
#else
	#define ALX_RST_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_RST_TRACE_ENABLE)
	#define ALX_RST_TRACE_WRN(...) ALX_TRACE_WRN(ALX_RST_FILE, __VA_ARGS__)
	#define ALX_RST_TRACE_INF(...) ALX_TRACE_INF(ALX_RST_FILE, __VA_ARGS__)
#else
	#define ALX_RST_TRACE_WRN(...) do{} while (false)
	#define ALX_RST_TRACE_INF(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRst_Init(AlxRst* me);
void AlxRst_Trace(AlxRst* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_RST_H
