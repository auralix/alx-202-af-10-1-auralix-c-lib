/**
  ******************************************************************************
  * @file		alxRange.h
  * @brief		Auralix C Library - ALX Range Module
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
#ifndef ALX_RANGE_H
#define ALX_RANGE_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxAssert.h"


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_RANGE_FILE "alxRange"

// Assert //
#if defined(_ALX_RANGE_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_RANGE_ASSERT(expr) ALX_ASSERT_BKPT(ALX_RANGE_FILE, expr)
#elif defined(_ALX_RANGE_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_RANGE_ASSERT(expr) ALX_ASSERT_TRACE(ALX_RANGE_FILE, expr)
#elif defined(_ALX_RANGE_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_RANGE_ASSERT(expr) ALX_ASSERT_RST(ALX_RANGE_FILE, expr)
#else
	#define ALX_RANGE_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_RANGE_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_RANGE_TRACE(...) ALX_TRACE_STD(ALX_RANGE_FILE, __VA_ARGS__)
#else
	#define ALX_RANGE_TRACE(...) do{} while (false)
#endif

// DbgPin //
#if defined(_ALX_RANGE_DBG_PIN) || defined(_ALX_DBG_PIN_ALL)
	#define ALX_RANGE_DBG_PIN(...) ALX_DBG_PIN_TOGGLE()
#else
	#define ALX_RANGE_DBG_PIN(...) do{} while (false)
#endif


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxRange_CheckUint8		(uint8_t val, uint8_t valMin, uint8_t valMax	);
Alx_Status AlxRange_CheckUint16		(uint16_t val, uint16_t valMin, uint16_t valMax	);
Alx_Status AlxRange_CheckUint32		(uint32_t val, uint32_t valMin, uint32_t valMax	);
Alx_Status AlxRange_CheckUint64		(uint64_t val, uint64_t valMin, uint64_t valMax	);
Alx_Status AlxRange_CheckInt8		(int8_t val, int8_t valMin, int8_t valMax		);
Alx_Status AlxRange_CheckInt16		(int16_t val, int16_t valMin, int16_t valMax	);
Alx_Status AlxRange_CheckInt32		(int32_t val, int32_t valMin, int32_t valMax	);
Alx_Status AlxRange_CheckInt64		(int64_t val, int64_t valMin, int64_t valMax	);
Alx_Status AlxRange_CheckFloat		(float val, float valMin, float valMax			);
Alx_Status AlxRange_CheckDouble		(double val, double valMin, double valMax		);
//Alx_Status AlxRange_CheckArr		(void* val, uint32_t valLenMax					);	// TO DO
Alx_Status AlxRange_CheckStr		(char* val, uint32_t valLenMax					);


#ifdef __cplusplus
}
#endif

#endif // #ifndef ALX_RANGE_H
