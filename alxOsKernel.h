﻿/**
  ******************************************************************************
  * @file		alxOsKernel.h
  * @brief		Auralix C Library - ALX OS Kernel Module
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
#ifndef ALX_OS_KERNEL_H
#define ALX_OS_KERNEL_H

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
#define ALX_OS_KERNEL_MODULE "AlxOsKernel.h"

// Assert //
#if defined(ALX_OS_KERNEL_ASSERT_BKPT_ENABLE)
	#define ALX_OS_KERNEL_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_KERNEL_MODULE, expr)
#elif defined(ALX_OS_KERNEL_ASSERT_TRACE_ENABLE)
	#define ALX_OS_KERNEL_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_KERNEL_MODULE, expr)
#elif defined(ALX_OS_KERNEL_ASSERT_RST_ENABLE)
	#define ALX_OS_KERNEL_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_KERNEL_MODULE, expr)
#else
	#define ALX_OS_KERNEL_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_OS_KERNEL_TRACE_ENABLE)
	#define ALX_OS_KERNEL_TRACE(...) ALX_TRACE_WRN(ALX_OS_KERNEL_MODULE, __VA_ARGS__)
#else
	#define ALX_OS_KERNEL_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsKernel_Start(void);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_OS_KERNEL_H
