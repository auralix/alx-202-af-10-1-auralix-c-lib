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
typedef struct
{
	uint32_t magicNumber;
	uint32_t version;

	uint32_t numOfDir;
	uint32_t numOfFilesPerDir;
	uint32_t numOfLogsPerFile;

	uint64_t idLogRead;
	uint64_t idLogWrite;

	uint32_t addrPosRead;
	uint32_t addrPosWrite;
	uint32_t addrLineRead;
	uint32_t addrLineWrite;
	uint32_t addrFileRead;
	uint32_t addrFileWrite;
	uint32_t addrDirRead;
	uint32_t addrDirWrite;

	uint16_t crc;
} AlxLogger_Info;

typedef struct
{
	// Defines
	#define ALX_LOGGER_PATH_LEN_MAX 128
	#define ALX_LOGGER_INFO_FILE_PATH "/AlxLoggerInfo.bin"
	#define ALX_LOGGER_INFO_MAGIC_NUMBER 0x002DCA5D
	#define ALX_LOGGER_INFO_VERSION 1
	#define ALX_LOGGER_LOG_LEN_MAX 512

	// Parameters
	AlxFs* alxFs;
	uint32_t numOfDir;
	uint32_t numOfFilesPerDir;
	uint32_t numOfLogsPerFile;

	// Variables
	AlxLogger_Info info;
	AlxCrc alxCrc;
	uint32_t numOfFiles;
	uint64_t numOfLogsMax;
	uint64_t numOfLogsPerDir;

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
	AlxFs* alxFs,
	uint32_t numOfDir,
	uint32_t numOfFilesPerDir,
	uint32_t numOfLogsPerFile
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLogger_Init(AlxLogger* me);
Alx_Status AlxLogger_DeInit(AlxLogger* me);
Alx_Status AlxLogger_Handle(AlxLogger* me);
Alx_Status AlxLogger_Trace_ReadLog(AlxLogger* me, char* str);
Alx_Status AlxLogger_Trace_WriteLog(AlxLogger* me, char* str);
Alx_Status AlxLogger_Data_ReadLog(AlxLogger* me, char* str);
Alx_Status AlxLogger_Data_WriteLog(AlxLogger* me, char* str);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LOGGER_H
