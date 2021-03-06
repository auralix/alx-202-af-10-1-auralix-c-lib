/**
  ******************************************************************************
  * @file		alxVdiv.h
  * @brief		Auralix C Library - ALX Voltage Divider Module
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

#ifndef ALX_V_DIV_H
#define ALX_V_DIV_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"
#include "alxTimSw.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_V_DIV_FILE "alxVdiv"

// Assert //
#if defined(_ALX_V_DIV_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_V_DIV_ASSERT(expr) ALX_ASSERT_BKPT(ALX_V_DIV_FILE, expr)
#elif defined(_ALX_V_DIV_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_V_DIV_ASSERT(expr) ALX_ASSERT_TRACE(ALX_V_DIV_FILE, expr)
#elif defined(_ALX_V_DIV_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_V_DIV_ASSERT(expr) ALX_ASSERT_RST(ALX_V_DIV_FILE, expr)
#else
	#define ALX_V_DIV_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_V_DIV_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_V_DIV_TRACE(...) ALX_TRACE_STD(ALX_V_DIV_FILE, __VA_ARGS__)
#else
	#define ALX_V_DIV_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_V_DIV_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_V_DIV_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_V_DIV_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************

//******************************************************************************
// Constructor
//******************************************************************************

//******************************************************************************
// Functions
//******************************************************************************
float AlxVdiv_GetVout_V(float vin_V, float resHigh_kOhm, float resLow_kOhm);
float AlxVdiv_GetVin_V(float vout_V, float resHigh_kOhm, float resLow_kOhm);
float AlxVdiv_GetResHigh_kOhm(float vin_V, float vout_V, float resLow_kOhm);
float AlxVdiv_GetResLow_kOhm(float vin_V, float vout_V, float resHigh_kOhm);
uint32_t AlxVdiv_GetVout_mV(uint32_t vin_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetVin_mV(uint32_t vout_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetResHigh_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetResLow_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resHigh_ohm);
uint32_t AlxVdiv_GetCurrent_uA(uint32_t vout_uV, uint32_t resLow_ohm);


#ifdef __cplusplus
}
#endif

#endif // ALX_V_DIV_H
