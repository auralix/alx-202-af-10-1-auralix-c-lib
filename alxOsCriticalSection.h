/**
  ******************************************************************************
  * @file		alxOsCriticalSection.h
  * @brief		Auralix C Library - ALX OS Critical Section Module
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

#ifndef ALX_OS_CRITICAL_SECTION_H
#define ALX_OS_CRITICAL_SECTION_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_OS) && defined(ALX_FREE_RTOS)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_OS_CRITICAL_SECTION_FILE "alxOsCriticalSection.h"

// Assert //
#if defined(_ALX_OS_CRITICAL_SECTION_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_OS_CRITICAL_SECTION_ASSERT(expr) ALX_ASSERT_BKPT(ALX_OS_CRITICAL_SECTION_FILE, expr)
#elif defined(_ALX_OS_CRITICAL_SECTION_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_OS_CRITICAL_SECTION_ASSERT(expr) ALX_ASSERT_TRACE(ALX_OS_CRITICAL_SECTION_FILE, expr)
#elif defined(_ALX_OS_CRITICAL_SECTION_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_OS_CRITICAL_SECTION_ASSERT(expr) ALX_ASSERT_RST(ALX_OS_CRITICAL_SECTION_FILE, expr)
#else
	#define ALX_OS_CRITICAL_SECTION_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_OS_CRITICAL_SECTION_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_OS_CRITICAL_SECTION_TRACE(...) ALX_TRACE_STD(ALX_OS_CRITICAL_SECTION_FILE, __VA_ARGS__)
#else
	#define ALX_OS_CRITICAL_SECTION_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
void AlxOsCriticalSection_Enter(void);
void AlxOsCriticalSection_Exit(void);


#endif // #if defined(ALX_FREE_RTOS)

#ifdef __cplusplus
}
#endif

#endif // ALX_OS_CRITICAL_SECTION_H
