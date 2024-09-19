/**
  ******************************************************************************
  * @file		alxNtcg103jf103ft1s.h
  * @brief		Auralix C Library - ALX NTC Thermistors NTCG103JF103FT1S Module
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
#ifndef ALX_NTCG103JF103FT1S_H
#define ALX_NTCG103JF103FT1S_H

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
#define ALX_NTCG103JF103FT1S_FILE "alxNtcg103jf103ft1s.h"

// Assert //
#if defined(ALX_NTCG103JF103FT1S_ASSERT_BKPT_ENABLE)
	#define ALX_NTCG103JF103FT1S_ASSERT(expr) ALX_ASSERT_BKPT(ALX_NTCG103JF103FT1S_FILE, expr)
#elif defined(ALX_NTCG103JF103FT1S_ASSERT_TRACE_ENABLE)
	#define ALX_NTCG103JF103FT1S_ASSERT(expr) ALX_ASSERT_TRACE(ALX_NTCG103JF103FT1S_FILE, expr)
#elif defined(ALX_NTCG103JF103FT1S_ASSERT_RST_ENABLE)
	#define ALX_NTCG103JF103FT1S_ASSERT(expr) ALX_ASSERT_RST(ALX_NTCG103JF103FT1S_FILE, expr)
#else
	#define ALX_NTCG103JF103FT1S_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_NTCG103JF103FT1S_TRACE_ENABLE)
	#define ALX_NTCG103JF103FT1S_TRACE(...) ALX_TRACE_WRN(ALX_NTCG103JF103FT1S_FILE, __VA_ARGS__)
#else
	#define ALX_NTCG103JF103FT1S_TRACE(...) do{} while (false)
#endif

#define ALX_NTCG103JF103FT1S_NUM_OF_POINTS 191


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	float xPointRes_kOhm[ALX_NTCG103JF103FT1S_NUM_OF_POINTS];
	float yPointTemp_degC[ALX_NTCG103JF103FT1S_NUM_OF_POINTS];
	uint32_t numOfPoints;
} AlxNtcg103jf103ft1s;


//******************************************************************************
// Variables
//******************************************************************************
extern const AlxNtcg103jf103ft1s alxNtcg103jf103ft1s;


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_NTCG103JF103FT1S_H
