/**
  ******************************************************************************
  * @file		alxBq24600.h
  * @brief		Auralix C Library - ALX Battery Charger BQ24600 Module
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
#ifndef ALX_BQ24600_H
#define ALX_BQ24600_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxTimSw.h"
#include "alxIoPin.h"
#include "alxFiltGlitchBool.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_BQ24600_FILE "alxBq24600.h"

// Assert //
#if defined(_ALX_BQ24600_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_BQ24600_ASSERT(expr) ALX_ASSERT_BKPT(ALX_BQ24600_FILE, expr)
#elif defined(_ALX_BQ24600_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_BQ24600_ASSERT(expr) ALX_ASSERT_TRACE(ALX_BQ24600_FILE, expr)
#elif defined(_ALX_BQ24600_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_BQ24600_ASSERT(expr) ALX_ASSERT_RST(ALX_BQ24600_FILE, expr)
#else
	#define ALX_BQ24600_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_BQ24600_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_BQ24600_TRACE(...) ALX_TRACE_STD(ALX_BQ24600_FILE, __VA_ARGS__)
#else
	#define ALX_BQ24600_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	Bq24600Status_Undef = 0,
	Bq24600Status_Charging,
	Bq24600Status_BattFull,
	Bq24600Status_Sleep,
	Bq24600Status_Error,
} Bq24600_status;

typedef struct
{
	// Parameters - Const
	uint32_t STATUS_DETECTION_WINDOW_TIME_ms;		// time window width for mode detection [ms]
	uint32_t ERROR_STATUS_DETECTION_TRANSITIONS;	// minimum number of STAT pin transitions in one detection window to recognize state as Error
	uint32_t STATUS_BLINK_HALFPERIOD_TIME_MIN_ms;	// minimum duration of half period on STAT pin when it's blinking [ms]
	uint32_t STATUS_BLINK_HALFPERIOD_TIME_MAX_ms;	// maximum duration of half period on STAT pin when it's blinking [ms] -> used for stable state detection

	// Objects - External
	AlxIoPin* do_EN;
	AlxIoPin* di_nPG;
	AlxIoPin* di_STAT;

	// Objects - Internal
	AlxTimSw timStatHalfperiod;			// timer for time STAT pin half period duration measurement
	AlxTimSw timWindow;					// timer for time window for mode detection
	AlxTimSw timStableStatus;			// timer for detection of stable status

	// Variables
	Bq24600_status statusRaw;			// unstable charger IC state, contains current status seen by IO pins, which might not be a stable state
	Bq24600_status status;				// charger IC state
	bool oldStatPinState;				// old STAT pin state, used for STAT pin transition detection
	uint32_t nOfStatPinTransitions;		// number of STAT pin transitions in current time window
	bool isStatBlink;					// is blinking detected on status pin [true->yes]

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxBq24600;


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  * @param[in] do_EN
  * @param[in] di_nPG
  * @param[in] di_STAT
  */
void AlxBq24600_Ctor
(
	AlxBq24600* me,
	AlxIoPin* do_EN,
	AlxIoPin* di_nPG,
	AlxIoPin* di_STAT
);


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxBq24600_Init(AlxBq24600* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBq24600_DeInit(AlxBq24600* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBq24600_Handle(AlxBq24600* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBq24600_Enable(AlxBq24600* me);

/**
  * @brief
  * @param[in,out] me
  */
void AlxBq24600_Disable(AlxBq24600* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxBq24600_IsBatCharging(AlxBq24600* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxBq24600_IsBatFull(AlxBq24600* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxBq24600_IsSleep(AlxBq24600* me);

/**
  * @brief
  * @param[in] me
  */
bool AlxBq24600_IsErr(AlxBq24600* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_BQ24600_H
