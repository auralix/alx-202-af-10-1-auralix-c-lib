/**
  ******************************************************************************
  * @file		alxVdiv.h
  * @brief		Auralix C Library - ALX Voltage Divider Module
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
#ifndef ALX_VDIV_H
#define ALX_VDIV_H

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
#define ALX_VDIV_FILE "alxVdiv.h"

// Assert //
#if defined(ALX_VDIV_ASSERT_BKPT_ENABLE)
	#define ALX_VDIV_ASSERT(expr) ALX_ASSERT_BKPT(ALX_VDIV_FILE, expr)
#elif defined(ALX_VDIV_ASSERT_TRACE_ENABLE)
	#define ALX_VDIV_ASSERT(expr) ALX_ASSERT_TRACE(ALX_VDIV_FILE, expr)
#elif defined(ALX_VDIV_ASSERT_RST_ENABLE)
	#define ALX_VDIV_ASSERT(expr) ALX_ASSERT_RST(ALX_VDIV_FILE, expr)
#else
	#define ALX_VDIV_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_VDIV_TRACE_ENABLE)
	#define ALX_VDIV_TRACE_WRN(...) ALX_TRACE_WRN(ALX_VDIV_FILE, __VA_ARGS__)
#else
	#define ALX_VDIV_TRACE_WRN(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
// Five quantities, two flavours, and the difference between them is not taste.
//
// FLOAT is the primary API. Its units follow the schematic - volts and kilohms - so a reader types
// the numbers the drawing already shows. Single precision carries about seven significant digits,
// far more than any ADC placed in front of it, so here the unit is a READABILITY choice and not a
// capability one: to work in another unit, scale the RESULT. On a float that is exact and loses
// nothing. Current needs no scale factor at all, because V / kOhm is mA.
//
// FIXED POINT is for parts with no FPU - this library supports Cortex-M0, M0+ and M3 families
// where every float operation is a software call. Here the unit IS the design, because an integer
// cannot carry a fraction: the scale is the resolution. Do NOT rescale one of these at the call
// site. That truncates, and truncating a current reading to whole milliamps is a defect this
// module has already caused in a product.
//
// Which is why the two current functions take a finer voltage than their neighbours do: mV / ohm
// would yield whole milliamps and throw the fraction away, while uV / ohm yields microamps and
// keeps it. The asymmetry is forced by arithmetic rather than left over from an edit.
//******************************************************************************
float AlxVdiv_GetVout_V(float vin_V, float resHigh_kOhm, float resLow_kOhm);
float AlxVdiv_GetVin_V(float vout_V, float resHigh_kOhm, float resLow_kOhm);
float AlxVdiv_GetResHigh_kOhm(float vin_V, float vout_V, float resLow_kOhm);
float AlxVdiv_GetResLow_kOhm(float vin_V, float vout_V, float resHigh_kOhm);
float AlxVdiv_GetCurrent_mA(float vout_V, float resLow_kOhm);
uint32_t AlxVdiv_GetVout_mV(uint32_t vin_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetVin_mV(uint32_t vout_mV, uint32_t resHigh_ohm, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetResHigh_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resLow_ohm);
uint32_t AlxVdiv_GetResLow_ohm(uint32_t vin_mV, uint32_t vout_mV, uint32_t resHigh_ohm);
uint32_t AlxVdiv_GetCurrent_uA(uint32_t vout_uV, uint32_t resLow_ohm);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_VDIV_H
