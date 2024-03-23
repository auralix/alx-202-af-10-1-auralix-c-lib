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
typedef enum
{
	AlxLogger_StoreMetadata_Config_StoreDefault,
	AlxLogger_StoreMetadata_Config_StoreCurrent,
	AlxLogger_StoreMetadata_Config_StoreRead,
	AlxLogger_StoreMetadata_Config_StoreWrite
} AlxLogger_StoreMetadata_Config;

typedef struct
{
	uint64_t id;
	uint32_t position;
	uint32_t line;
	uint32_t file;
	uint32_t dir;
} AlxLogger_Metadata_ReadWrite;

typedef struct
{
	uint32_t magicNumber;
	uint32_t version;

	uint32_t numOfDir;
	uint32_t numOfFilesPerDir;
	uint32_t numOfLogsPerFile;

	AlxLogger_Metadata_ReadWrite read;
	AlxLogger_Metadata_ReadWrite write;

	uint16_t crc;
} AlxLogger_Metadata;

typedef struct
{
	// Defines
	#define ALX_LOGGER_PATH_LEN_MAX 128
	#define ALX_LOGGER_LOG_LEN_MAX 256
	#define ALX_LOGGER_METADATA_FILE_PATH "/AlxLoggerMetadata.bin"
	#define ALX_LOGGER_METADATA_MAGIC_NUMBER 0x002DCA5D
	#define ALX_LOGGER_METADATA_VERSION 1

	// Parameters
	AlxFs* alxFs;
	uint32_t numOfDir;
	uint32_t numOfFilesPerDir;
	uint32_t numOfLogsPerFile;
	const char* logDelim;

	// Parameters - Private
	uint32_t numOfFiles;
	uint64_t numOfLogsPerDir;

	// Variables
	AlxLogger_Metadata md;
	AlxLogger_Metadata mdStored;
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
	AlxFs* alxFs,
	uint32_t numOfDir,
	uint32_t numOfFilesPerDir,
	uint32_t numOfLogsPerFile,
	const char* logDelim
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLogger_Init(AlxLogger* me);
Alx_Status AlxLogger_Read(AlxLogger* me, char* logs, uint32_t numOfLogs, uint32_t* numOfLogsActual);
Alx_Status AlxLogger_Write(AlxLogger* me, const char* logs, uint32_t numOfLogs);
Alx_Status AlxLogger_StoreMetadata(AlxLogger* me, AlxLogger_StoreMetadata_Config config);
AlxLogger_Metadata AlxLogger_GetMetadataCurrent(AlxLogger* me);
AlxLogger_Metadata AlxLogger_GetMetadataStored(AlxLogger* me);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_LOGGER_H
