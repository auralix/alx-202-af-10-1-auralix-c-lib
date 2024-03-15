/**
  ******************************************************************************
  * @file		alxLogger.c
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
// Includes
//******************************************************************************
#include "alxLogger.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxLogger_GetInfo(AlxLogger* me);
static Alx_Status AlxLogger_SetInfoToDefault(AlxLogger* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLogger_Ctor
(
	AlxLogger* me,
	AlxFs* alxFs
)
{
	// Parameters
	me->alxFs = alxFs;

	// Variables
	memset(&me->info, 0, sizeof(me->info));
	AlxCrc_Ctor(&me->alxCrc, AlxCrc_Config_Ccitt);

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLogger_Init(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Mount/Format FS
	status = AlxFs_MountFormat(me->alxFs);
	if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Get/Set info
	status = AlxLogger_GetInfo(me);
	if (status != Alx_Ok)
	{
		status = AlxLogger_SetInfoToDefault(me);
		if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }
	}

	// Trace info
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("Auralix C Library - ALX Logger Module:\r\n");
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("Info:\r\n");
	ALX_LOGGER_TRACE_FORMAT("- magicNumber = 0x%08lX\r\n", me->info.magicNumber);
	ALX_LOGGER_TRACE_FORMAT("- version = %lu\r\n", me->info.version);
	ALX_LOGGER_TRACE_FORMAT("- idLogReadOldest = %ld\r\n", (int32_t)me->info.idLogReadOldest);
	ALX_LOGGER_TRACE_FORMAT("- idLogReadNewest = %ld\r\n", (int32_t)me->info.idLogReadNewest);
	ALX_LOGGER_TRACE_FORMAT("- idLogReadNext = %ld\r\n", (int32_t)me->info.idLogReadNext);
	ALX_LOGGER_TRACE_FORMAT("- idLogWriteOldest = %ld\r\n", (int32_t)me->info.idLogWriteOldest);
	ALX_LOGGER_TRACE_FORMAT("- idLogWriteNewest = %ld\r\n", (int32_t)me->info.idLogWriteNewest);
	ALX_LOGGER_TRACE_FORMAT("- idLogWriteNext = %ld\r\n", (int32_t)me->info.idLogWriteNext);
	ALX_LOGGER_TRACE_FORMAT("- idFileReadOldest = %ld\r\n", (int32_t)me->info.idFileReadOldest);
	ALX_LOGGER_TRACE_FORMAT("- idFileReadNewest = %ld\r\n", (int32_t)me->info.idFileReadNewest);
	ALX_LOGGER_TRACE_FORMAT("- idFileReadNext = %ld\r\n", (int32_t)me->info.idFileReadNext);
	ALX_LOGGER_TRACE_FORMAT("- idFileWriteOldest = %ld\r\n", (int32_t)me->info.idFileWriteOldest);
	ALX_LOGGER_TRACE_FORMAT("- idFileWriteNewest = %ld\r\n", (int32_t)me->info.idFileWriteNewest);
	ALX_LOGGER_TRACE_FORMAT("- idFileWriteNext = %ld\r\n", (int32_t)me->info.idFileWriteNext);
	ALX_LOGGER_TRACE_FORMAT("- crc = 0x%04X\r\n", me->info.crc);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_DeInit(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Handle(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Trace_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Trace_WriteLog(AlxLogger* me, void* data, uint32_t len)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Data_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Data_WriteLog(AlxLogger* me, void* data, uint32_t len)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxLogger_GetInfo(AlxLogger* me)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t lenActual = 0;
	uint32_t validatedCrc = 0;
	bool isCrcOk = false;

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_INFO_FILENAME, "r");
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Read
	status = AlxFs_File_Read(me->alxFs, &file, &me->info, sizeof(me->info), &lenActual);
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if(status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Check length
	if(lenActual != sizeof(me->info))
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Check CRC
	isCrcOk = AlxCrc_IsOk(&me->alxCrc, (uint8_t*)&me->info, sizeof(me->info), &validatedCrc);
	if(isCrcOk != true)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Check magic number
	if(me->info.magicNumber != ALX_LOGGER_INFO_MAGIC_NUMBER)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Check version
	if(me->info.version != ALX_LOGGER_INFO_VERSION)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Return
	return Alx_Ok;
}
static Alx_Status AlxLogger_SetInfoToDefault(AlxLogger* me)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};

	// Prepare info
	me->info.magicNumber = ALX_LOGGER_INFO_MAGIC_NUMBER;
	me->info.version = ALX_LOGGER_INFO_VERSION;
	me->info.idLogReadOldest = -1;
	me->info.idLogReadNewest = -1;
	me->info.idLogReadNext = 0;
	me->info.idLogWriteOldest = -1;
	me->info.idLogWriteNewest = -1;
	me->info.idLogWriteNext = 0;
	me->info.idFileReadOldest = -1;
	me->info.idFileReadNewest = -1;
	me->info.idFileReadNext = 0;
	me->info.idFileWriteOldest = -1;
	me->info.idFileWriteNewest = -1;
	me->info.idFileWriteNext = 0;
	uint32_t crcLen = AlxCrc_GetLen(&me->alxCrc);
	uint32_t infoLenWithoutCrc = sizeof(me->info) - crcLen;
	me->info.crc = AlxCrc_Calc(&me->alxCrc, (uint8_t*)&me->info, infoLenWithoutCrc);

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_INFO_FILENAME, "w");
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Write
	status = AlxFs_File_Write(me->alxFs, &file, &me->info, sizeof(me->info));
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if(status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
