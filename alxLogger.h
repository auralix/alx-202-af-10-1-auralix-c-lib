/**
  ******************************************************************************
  * @file		alxLogger.h
  * @brief		Auralix C Library - ALX Logger Module
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
#ifndef ALX_LOGGER_H
#define ALX_LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxFs.h"
#include "alxCrc.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_LOGGER_FILE "alxLogger.h"

// Assert //
#if defined(ALX_LOGGER_ASSERT_BKPT_ENABLE)
	#define ALX_LOGGER_ASSERT(expr) ALX_ASSERT_BKPT(ALX_LOGGER_FILE, expr)
#elif defined(ALX_LOGGER_ASSERT_TRACE_ENABLE)
	#define ALX_LOGGER_ASSERT(expr) ALX_ASSERT_TRACE(ALX_LOGGER_FILE, expr)
#elif defined(ALX_LOGGER_ASSERT_RST_ENABLE)
	#define ALX_LOGGER_ASSERT(expr) ALX_ASSERT_RST(ALX_LOGGER_FILE, expr)
#else
	#define ALX_LOGGER_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_LOGGER_TRACE_ENABLE)
	#define ALX_LOGGER_TRACE(...) ALX_TRACE_STD(ALX_LOGGER_FILE, __VA_ARGS__)
	#define ALX_LOGGER_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_LOGGER_TRACE(...) do{} while (false)
	#define ALX_LOGGER_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef union
{
	struct __attribute__((packed))
	{
		uint32_t magicNumber;
		uint32_t version;

		int64_t idLogReadNext;
		int64_t idLogReadNewest;
		int64_t idLogReadOldest;
		int64_t idLogWriteNext;
		int64_t idLogWriteNewest;
		int64_t idLogWriteOldest;

		int64_t addrLineReadNext;
		int64_t addrLineReadNewest;
		int64_t addrLineReadOldest;
		int64_t addrLineWriteNext;
		int64_t addrLineWriteNewest;
		int64_t addrLineWriteOldest;

		int64_t addrFileReadNext;
		int64_t addrFileReadNewest;
		int64_t addrFileReadOldest;
		int64_t addrFileWriteNext;
		int64_t addrFileWriteNewest;
		int64_t addrFileWriteOldest;

		int64_t addrDirReadNext;
		int64_t addrDirReadNewest;
		int64_t addrDirReadOldest;
		int64_t addrDirWriteNext;
		int64_t addrDirWriteNewest;
		int64_t addrDirWriteOldest;

		uint16_t crc;
	};
	uint8_t raw[4 + 4 + 24*8 + 2];
} AlxLogger_Info;

typedef struct
{
	// Defines
	#define ALX_LOGGER_INFO_FILENAME "AlxLoggerInfo.bin"
	#define ALX_LOGGER_INFO_MAGIC_NUMBER 0x002DCA5D
	#define ALX_LOGGER_INFO_VERSION 1

	// Parameters
	AlxFs* alxFs;

	// Variables
	AlxLogger_Info info;
	AlxCrc alxCrc;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxLogger;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLogger_Ctor
(
	AlxLogger* me,
	AlxFs* alxFs
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLogger_Init(AlxLogger* me);
Alx_Status AlxLogger_DeInit(AlxLogger* me);
Alx_Status AlxLogger_Handle(AlxLogger* me);
Alx_Status AlxLogger_Trace_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual);
Alx_Status AlxLogger_Trace_WriteLog(AlxLogger* me, void* data, uint32_t len);
Alx_Status AlxLogger_Data_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual);
Alx_Status AlxLogger_Data_WriteLog(AlxLogger* me, void* data, uint32_t len);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LOGGER_H
