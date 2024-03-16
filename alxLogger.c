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
static Alx_Status AlxLogger_Fs_Prepare(AlxLogger* me);
static Alx_Status AlxLogger_Fs_CheckInfo(AlxLogger* me);
static Alx_Status AlxLogger_Fs_CreateInfo(AlxLogger* me);
static Alx_Status AlxLogger_Fs_CreateDirFile(AlxLogger* me);


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
)
{
	// Parameters
	me->alxFs = alxFs;
	me->numOfDir = numOfDir;
	me->numOfFilesPerDir = numOfFilesPerDir;
	me->numOfLogsPerFile = numOfLogsPerFile;

	// Variables
	memset(&me->info, 0, sizeof(me->info));
	AlxCrc_Ctor(&me->alxCrc, AlxCrc_Config_Ccitt);
	me->numOfFiles = me->numOfFilesPerDir * me->numOfDir;
	me->numOfLogsMax = me->numOfFiles * me->numOfLogsPerFile;

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

	// Trace intro
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("Auralix C Library - ALX Logger Module:\r\n");

	// Prepare FS
	Alx_Status status = AlxLogger_Fs_Prepare(me);
	if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Trace info
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("Info:\r\n");
	ALX_LOGGER_TRACE_FORMAT("- magicNumber = 0x%08lX\r\n", me->info.magicNumber);
	ALX_LOGGER_TRACE_FORMAT("- version = %lu\r\n", me->info.version);

	ALX_LOGGER_TRACE_FORMAT("- numOfDir = %lu\r\n", me->info.numOfDir);
	ALX_LOGGER_TRACE_FORMAT("- numOfFilesPerDir = %lu\r\n", me->info.numOfFilesPerDir);
	ALX_LOGGER_TRACE_FORMAT("- numOfLogsPerFile = %lu\r\n", me->info.numOfLogsPerFile);
	ALX_LOGGER_TRACE_FORMAT("- numOfFiles = %lu\r\n", me->info.numOfFiles);
	ALX_LOGGER_TRACE_FORMAT("- numOfLogsMax = %lu\r\n", (uint32_t)me->info.numOfLogsMax);

	ALX_LOGGER_TRACE_FORMAT("- idLogReadNext = %ld\r\n", (int32_t)me->info.idLogReadNext);
	ALX_LOGGER_TRACE_FORMAT("- idLogReadNewest = %ld\r\n", (int32_t)me->info.idLogReadNewest);
	ALX_LOGGER_TRACE_FORMAT("- idLogReadOldest = %ld\r\n", (int32_t)me->info.idLogReadOldest);
	ALX_LOGGER_TRACE_FORMAT("- idLogWriteNext = %ld\r\n", (int32_t)me->info.idLogWriteNext);
	ALX_LOGGER_TRACE_FORMAT("- idLogWriteNewest = %ld\r\n", (int32_t)me->info.idLogWriteNewest);
	ALX_LOGGER_TRACE_FORMAT("- idLogWriteOldest = %ld\r\n", (int32_t)me->info.idLogWriteOldest);

	ALX_LOGGER_TRACE_FORMAT("- addrLineReadNext = %ld\r\n", (int32_t)me->info.addrLineReadNext);
	ALX_LOGGER_TRACE_FORMAT("- addrLineReadNewest = %ld\r\n", (int32_t)me->info.addrLineReadNewest);
	ALX_LOGGER_TRACE_FORMAT("- addrLineReadOldest = %ld\r\n", (int32_t)me->info.addrLineReadOldest);
	ALX_LOGGER_TRACE_FORMAT("- addrLineWriteNext = %ld\r\n", (int32_t)me->info.addrLineWriteNext);
	ALX_LOGGER_TRACE_FORMAT("- addrLineWriteNewest = %ld\r\n", (int32_t)me->info.addrLineWriteNewest);
	ALX_LOGGER_TRACE_FORMAT("- addrLineWriteOldest = %ld\r\n", (int32_t)me->info.addrLineWriteOldest);

	ALX_LOGGER_TRACE_FORMAT("- addrFileReadNext = %ld\r\n", (int32_t)me->info.addrFileReadNext);
	ALX_LOGGER_TRACE_FORMAT("- addrFileReadNewest = %ld\r\n", (int32_t)me->info.addrFileReadNewest);
	ALX_LOGGER_TRACE_FORMAT("- addrFileReadOldest = %ld\r\n", (int32_t)me->info.addrFileReadOldest);
	ALX_LOGGER_TRACE_FORMAT("- addrFileWriteNext = %ld\r\n", (int32_t)me->info.addrFileWriteNext);
	ALX_LOGGER_TRACE_FORMAT("- addrFileWriteNewest = %ld\r\n", (int32_t)me->info.addrFileWriteNewest);
	ALX_LOGGER_TRACE_FORMAT("- addrFileWriteOldest = %ld\r\n", (int32_t)me->info.addrFileWriteOldest);

	ALX_LOGGER_TRACE_FORMAT("- addrDirReadNext = %ld\r\n", (int32_t)me->info.addrDirReadNext);
	ALX_LOGGER_TRACE_FORMAT("- addrDirReadNewest = %ld\r\n", (int32_t)me->info.addrDirReadNewest);
	ALX_LOGGER_TRACE_FORMAT("- addrDirReadOldest = %ld\r\n", (int32_t)me->info.addrDirReadOldest);
	ALX_LOGGER_TRACE_FORMAT("- addrDirWriteNext = %ld\r\n", (int32_t)me->info.addrDirWriteNext);
	ALX_LOGGER_TRACE_FORMAT("- addrDirWriteNewest = %ld\r\n", (int32_t)me->info.addrDirWriteNewest);
	ALX_LOGGER_TRACE_FORMAT("- addrDirWriteOldest = %ld\r\n", (int32_t)me->info.addrDirWriteOldest);

	ALX_LOGGER_TRACE_FORMAT("- crc = 0x%04X\r\n", me->info.crc);
	ALX_LOGGER_TRACE_FORMAT("\r\n");

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
static Alx_Status AlxLogger_Fs_Prepare(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;


	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (true)
	{
		// Mount
		status = AlxFs_Mount(me->alxFs);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); break; }

		// Check info
		status = AlxLogger_Fs_CheckInfo(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); break; }

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Format
	status = AlxFs_Format(me->alxFs);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Mount
	status = AlxFs_Mount(me->alxFs);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Create directories & files
	status = AlxLogger_Fs_CreateDirFile(me);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Create info
	status = AlxLogger_Fs_CreateInfo(me);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_CheckInfo(AlxLogger* me)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t lenActual = 0;
	uint32_t validatedCrc = 0;
	bool isCrcOk = false;

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_INFO_FILENAME, "r");
	if(status != Alx_Ok)
	{
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
		return Alx_Err;
	}

	// Check CRC
	isCrcOk = AlxCrc_IsOk(&me->alxCrc, (uint8_t*)&me->info, sizeof(me->info), &validatedCrc);
	if(isCrcOk != true)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check magic number
	if(me->info.magicNumber != ALX_LOGGER_INFO_MAGIC_NUMBER)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check version
	if(me->info.version != ALX_LOGGER_INFO_VERSION)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of directories
	if(me->info.numOfDir != me->numOfDir)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of files per directory
	if(me->info.numOfFilesPerDir != me->numOfFilesPerDir)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of logs per file
	if(me->info.numOfLogsPerFile != me->numOfLogsPerFile)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of files
	if(me->info.numOfFiles != me->numOfFiles)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of logs max
	if(me->info.numOfLogsMax != me->numOfLogsMax)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_CreateInfo(AlxLogger* me)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};

	// Prepare info
	me->info.magicNumber = ALX_LOGGER_INFO_MAGIC_NUMBER;
	me->info.version = ALX_LOGGER_INFO_VERSION;

	me->info.numOfDir = me->numOfDir;
	me->info.numOfFilesPerDir = me->numOfFilesPerDir;
	me->info.numOfLogsPerFile = me->numOfLogsPerFile;
	me->info.numOfFiles = me->numOfFiles;
	me->info.numOfLogsMax = me->numOfLogsMax;

	me->info.idLogReadNext = -1;
	me->info.idLogReadNewest = -1;
	me->info.idLogReadOldest = -1;
	me->info.idLogWriteNext = 0;
	me->info.idLogWriteNewest = -1;
	me->info.idLogWriteOldest = -1;

	me->info.addrLineReadNext = -1;
	me->info.addrLineReadNewest = -1;
	me->info.addrLineReadOldest = -1;
	me->info.addrLineWriteNext = 0;
	me->info.addrLineWriteNewest = -1;
	me->info.addrLineWriteOldest = -1;

	me->info.addrFileReadNext = -1;
	me->info.addrFileReadNewest = -1;
	me->info.addrFileReadOldest = -1;
	me->info.addrFileWriteNext = 0;
	me->info.addrFileWriteNewest = -1;
	me->info.addrFileWriteOldest = -1;

	me->info.addrDirReadNext = -1;
	me->info.addrDirReadNewest = -1;
	me->info.addrDirReadOldest = -1;
	me->info.addrDirWriteNext = 0;
	me->info.addrDirWriteNewest = -1;
	me->info.addrDirWriteOldest = -1;

	uint32_t crcLen = AlxCrc_GetLen(&me->alxCrc);
	uint32_t infoLenWithoutCrc = sizeof(me->info) - crcLen;
	me->info.crc = AlxCrc_Calc(&me->alxCrc, (uint8_t*)&me->info, infoLenWithoutCrc);

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_INFO_FILENAME, "w");
	if(status != Alx_Ok)
	{
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
static Alx_Status AlxLogger_Fs_CreateDirFile(AlxLogger* me)
{
	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_Dir dir = {};
	AlxFs_File file = {};
	char dirName[16] = "";
	char fileName[16] = "";
	uint32_t dirAddr = 0;
	uint32_t fileAddr = 0;
	AlxTimSw alxTimSw_DirFilePrepSingle;
	AlxTimSw alxTimSw_DirFilePrepAll;
	AlxTimSw_Ctor(&alxTimSw_DirFilePrepSingle, false);
	AlxTimSw_Ctor(&alxTimSw_DirFilePrepAll, false);

	// Trace
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("Started creating dir & files:\r\n");

	// Start timer
	AlxTimSw_Start(&alxTimSw_DirFilePrepAll);

	// Create directories
	for(dirAddr = 0; dirAddr < me->numOfFiles; dirAddr = dirAddr + me->numOfFilesPerDir)
	{
		// Start timer
		AlxTimSw_Start(&alxTimSw_DirFilePrepSingle);

		// Make dir
		sprintf(dirName, "/Dir_%04lu", dirAddr);
		status = AlxFs_Dir_Make(me->alxFs, dirName);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Open dir
		status = AlxFs_Dir_Open(me->alxFs, &dir, dirName);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Create files
		for (uint32_t i = 0; i < me->numOfFilesPerDir; i++)
		{
			// Open file
			sprintf(fileName, "%04lu.csv", fileAddr);
			status = AlxFs_File_Open(me->alxFs, &file, fileName, "w");
			if (status != Alx_Ok)
			{
				AlxFs_Dir_Close(me->alxFs, &dir);	// Will not handle return
				ALX_FS_TRACE("Err");
				return status;
			}

			// Close file
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				AlxFs_Dir_Close(me->alxFs, &dir);	// Will not handle return
				ALX_FS_TRACE("Err");
				return status;
			}

			// Increment
			fileAddr++;
		}

		// Close dir
		status = AlxFs_Dir_Close(me->alxFs, &dir);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Trace
		uint32_t dirFilePrepSingle_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepSingle);
		ALX_LOGGER_TRACE_FORMAT("Created %s with %lu files in %lu sec\r\n", dirName, me->numOfFilesPerDir, dirFilePrepSingle_sec);
	}

	// Trace
	uint32_t dirFilePrepAllTime_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepAll);
	ALX_LOGGER_TRACE_FORMAT("Created %lu dir with %lu files, total %lu files in %lu sec\r\n", me->numOfDir, me->numOfFilesPerDir, me->numOfFiles, dirFilePrepAllTime_sec);

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
