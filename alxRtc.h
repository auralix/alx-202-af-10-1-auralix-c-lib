﻿/**
  ******************************************************************************
  * @file		alxRtc.h
  * @brief		Auralix C Library - ALX RTC Module
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
#ifndef ALX_RTC_H
#define ALX_RTC_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxRtc_Global.h"

// AlxMcu //
#if defined(ALX_STM32F4)
#include "alxRtc_McuStm32.h"

#else
typedef struct { bool dummy; } AlxRtc;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_RTC_FILE "alxRtc.h"

// Assert //
#if defined(_ALX_RTC_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_RTC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_RTC_FILE, expr)
#elif defined(_ALX_RTC_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_RTC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_RTC_FILE, expr)
#elif defined(_ALX_RTC_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_RTC_ASSERT(expr) ALX_ASSERT_RST(ALX_RTC_FILE, expr)
#else
	#define ALX_RTC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_RTC_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_RTC_TRACE(...) ALX_TRACE_STD(ALX_RTC_FILE, __VA_ARGS__)
#else
	#define ALX_RTC_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_Init(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_DeInit(AlxRtc* me);

/**
  * @param[in,out]	me
  * @param[in]		dateTime
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_SetDateTime(AlxRtc* me, AlxRtc_DateTime dateTime);

/**
  * @brief
  * @param[in,out]	me
  * @param[out]		dateTime
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_GetDateTimeWithStatus(AlxRtc* me, AlxRtc_DateTime* dateTime);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
AlxRtc_DateTime AlxRtc_GetDateTime(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxRtc_IsDateTimeConfigured(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_ns
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_SetUnixTime_ns(AlxRtc* me, uint64_t unixTime_ns);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_us
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_SetUnixTime_us(AlxRtc* me, uint64_t unixTime_us);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_SetUnixTime_ms(AlxRtc* me, uint64_t unixTime_ms);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_sec
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_SetUnixTime_sec(AlxRtc* me, uint64_t unixTime_sec);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_ns
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_GetUnixTimeWithStatus_ns(AlxRtc* me, uint64_t* unixTime_ns);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_us
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_GetUnixTimeWithStatus_us(AlxRtc* me, uint64_t* unixTime_us);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_GetUnixTimeWithStatus_ms(AlxRtc* me, uint64_t* unixTime_ms);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		unixTime_sec
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_GetUnixTimeWithStatus_sec(AlxRtc* me, uint64_t* unixTime_sec);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxRtc_GetUnixTime_ns(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxRtc_GetUnixTime_us(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxRtc_GetUnixTime_ms(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @return
  */
uint64_t AlxRtc_GetUnixTime_sec(AlxRtc* me);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		tuneTime_ns
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_TuneTime_ns(AlxRtc* me, int64_t tuneTime_ns);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		tuneTime_us
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_TuneTime_us(AlxRtc* me, int64_t tuneTime_us);

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		tuneTime_ms
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRtc_TuneTime_ms(AlxRtc* me, int64_t tuneTime_ms);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_RTC_H
