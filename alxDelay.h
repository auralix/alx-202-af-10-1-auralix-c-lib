﻿/**
  ******************************************************************************
  * @file		alxDelay.h
  * @brief		Auralix C Library - ALX Delay Module
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
#ifndef ALX_DELAY_H
#define ALX_DELAY_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTick.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_DELAY_FILE "alxDelay.h"

// Assert //
#if defined(_ALX_DELAY_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_DELAY_ASSERT(expr) ALX_ASSERT_BKPT(ALX_DELAY_FILE, expr)
#elif defined(_ALX_DELAY_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_DELAY_ASSERT(expr) ALX_ASSERT_TRACE(ALX_DELAY_FILE, expr)
#elif defined(_ALX_DELAY_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_DELAY_ASSERT(expr) ALX_ASSERT_RST(ALX_DELAY_FILE, expr)
#else
	#define ALX_DELAY_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_DELAY_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_DELAY_TRACE(...) ALX_TRACE_STD(ALX_DELAY_FILE, __VA_ARGS__)
#else
	#define ALX_DELAY_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in] delay_ns
  */
void AlxDelay_ns(uint64_t delay_ns);

/**
  * @brief
  * @param[in] delay_us
  */
void AlxDelay_us(uint64_t delay_us);

/**
  * @brief
  * @param[in] delay_ms
  */
void AlxDelay_ms(uint64_t delay_ms);

/**
  * @brief
  * @param[in] delay_sec
  */
void AlxDelay_sec( uint64_t delay_sec);

/**
  * @brief
  * @param[in] delay_min
  */
void AlxDelay_min(uint64_t delay_min);

/**
  * @brief
  * @param[in] delay_hr
  */
void AlxDelay_hr(uint64_t delay_hr);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_DELAY_H
