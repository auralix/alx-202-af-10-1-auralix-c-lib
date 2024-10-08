/**
  ******************************************************************************
  * @file		alxCrc.h
  * @brief		Auralix C Library - ALX CRC Module
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
#ifndef ALX_CRC_H
#define ALX_CRC_H

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
#define ALX_CRC_FILE "alxCrc.h"

// Assert //
#if defined(ALX_CRC_ASSERT_BKPT_ENABLE)
	#define ALX_CRC_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CRC_FILE, expr)
#elif defined(ALX_CRC_ASSERT_TRACE_ENABLE)
	#define ALX_CRC_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CRC_FILE, expr)
#elif defined(ALX_CRC_ASSERT_RST_ENABLE)
	#define ALX_CRC_ASSERT(expr) ALX_ASSERT_RST(ALX_CRC_FILE, expr)
#else
	#define ALX_CRC_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_CRC_TRACE_ENABLE)
	#define ALX_CRC_TRACE(...) ALX_TRACE_WRN(ALX_CRC_FILE, __VA_ARGS__)
#else
	#define ALX_CRC_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxCrc_Config_Ccitt = 0,
	AlxCrc_Config_Crc16 = 1,
	AlxCrc_Config_Crc32 = 2
} AlxCrc_Config;

typedef struct
{
	// Parameters
	AlxCrc_Config config;

	// Info
	bool wasCtorCalled;
} AlxCrc;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxCrc_Ctor
(
	AlxCrc* me,
	AlxCrc_Config config
);


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxCrc_Calc(AlxCrc* me, uint8_t* data, uint32_t len);
bool AlxCrc_IsOk(AlxCrc* me, uint8_t* dataWithCrc, uint32_t lenWithCrc, uint32_t* validatedCrc);
uint32_t AlxCrc_GetLen(AlxCrc* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CRC_H
