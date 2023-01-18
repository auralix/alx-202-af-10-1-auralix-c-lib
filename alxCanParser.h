/**
  ******************************************************************************
  * @file		alxCanParser.h
  * @brief		Auralix C Library - ALX CAN Parser Module
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
#ifndef ALX_CAN_PARSER_H
#define ALX_CAN_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxCan.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_CAN_PARSER_FILE "alxCanParser.h"

// Assert //
#if defined(_ALX_CAN_PARSER_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_CAN_PARSER_ASSERT(expr) ALX_ASSERT_BKPT(ALX_CAN_PARSER_FILE, expr)
#elif defined(_ALX_CAN_PARSER_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_CAN_PARSER_ASSERT(expr) ALX_ASSERT_TRACE(ALX_CAN_PARSER_FILE, expr)
#elif defined(_ALX_CAN_PARSER_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_CAN_PARSER_ASSERT(expr) ALX_ASSERT_RST(ALX_CAN_PARSER_FILE, expr)
#else
	#define ALX_CAN_PARSER_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_CAN_PARSER_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_CAN_PARSER_TRACE(...) ALX_TRACE_STD(ALX_CAN_PARSER_FILE, __VA_ARGS__)
#else
	#define ALX_CAN_PARSER_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	Big		= 0,	// most significant byte is placed at the byte with the lowest address	// Renamed from: can_endian_big
	Little	= 1		// least significant byte is placed at the byte with the lowest address // Renamed from: can_endian_little
} AlxCanParser_Endian;


//******************************************************************************
// Functions
//******************************************************************************
void AlxCanParser_SetBit(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset, bool value);
void AlxCanParser_SetUint8(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t value);
void AlxCanParser_SetInt8(AlxCan_Msg* msg, uint8_t byteOffset, int8_t value);
void AlxCanParser_SetUint16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, uint16_t value);
void AlxCanParser_SetInt16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, int16_t value);
void AlxCanParser_SetUint32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, uint32_t value);
void AlxCanParser_SetInt32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, int32_t value);
void AlxCanParser_SetUint64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, uint64_t value);
void AlxCanParser_SetInt64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, int64_t value);
void AlxCanParser_SetFloat(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, float value);
void AlxCanParser_SetDouble(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset, double value);
void AlxCanParser_SetEnum(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset, uint8_t nOfBits, uint8_t value);
bool AlxCanParser_GetBit(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset);
uint8_t AlxCanParser_GetUint8(AlxCan_Msg* msg, uint8_t byteOffset);
int8_t AlxCanParser_GetInt8(AlxCan_Msg* msg, uint8_t byteOffset);
uint16_t AlxCanParser_GetUint16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
int16_t AlxCanParser_GetInt16(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
uint32_t AlxCanParser_GetUint32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
int32_t AlxCanParser_GetInt32(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
uint64_t AlxCanParser_GetUint64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
int64_t AlxCanParser_GetInt64(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
float AlxCanParser_GetFloat(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
double AlxCanParser_GetDouble(AlxCan_Msg* msg, AlxCanParser_Endian endian, uint8_t byteOffset);
uint8_t AlxCanParser_GetEnum(AlxCan_Msg* msg, uint8_t byteOffset, uint8_t bitOffset, uint8_t nOfBits);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CAN_PARSER_H
