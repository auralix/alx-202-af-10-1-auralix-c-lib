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
static Alx_Status AlxLogger_Fs_LoadAndCheckInfo(AlxLogger* me);
static Alx_Status AlxLogger_Fs_StoreInfo(AlxLogger* me, bool storeDefault);
static Alx_Status AlxLogger_Fs_CreateDirAndFiles(AlxLogger* me);
static Alx_Status AlxLogger_Fs_ClearWriteDir(AlxLogger* me);
static Alx_Status AlxLogger_Fs_RepairWriteFile(AlxLogger* me);


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
)
{
	// Parameters
	me->alxFs = alxFs;
	me->numOfDir = numOfDir;
	me->numOfFilesPerDir = numOfFilesPerDir;
	me->numOfLogsPerFile = numOfLogsPerFile;
	me->logDelim = logDelim;

	// Variables
	memset(&me->info, 0, sizeof(me->info));
	AlxCrc_Ctor(&me->alxCrc, AlxCrc_Config_Ccitt);
	me->numOfFiles = me->numOfFilesPerDir * me->numOfDir;
	me->numOfLogsMax = me->numOfFiles * me->numOfLogsPerFile;
	me->numOfLogsPerDir = me->numOfFilesPerDir * me->numOfLogsPerFile;

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
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Init Started\r\n");

	// Prepare FS
	Alx_Status status = AlxLogger_Fs_Prepare(me);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Trace info
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Info\r\n");
	ALX_LOGGER_TRACE_FORMAT("- magicNumber = 0x%08lX\r\n", me->info.magicNumber);
	ALX_LOGGER_TRACE_FORMAT("- version = %lu\r\n", me->info.version);

	ALX_LOGGER_TRACE_FORMAT("- numOfDir = %lu\r\n", me->info.numOfDir);
	ALX_LOGGER_TRACE_FORMAT("- numOfFilesPerDir = %lu\r\n", me->info.numOfFilesPerDir);
	ALX_LOGGER_TRACE_FORMAT("- numOfLogsPerFile = %lu\r\n", me->info.numOfLogsPerFile);

	ALX_LOGGER_TRACE_FORMAT("- idRead = %lu\r\n", (uint32_t)me->info.idRead);
	ALX_LOGGER_TRACE_FORMAT("- addrReadPosition = %lu\r\n", me->info.addrReadPosition);
	ALX_LOGGER_TRACE_FORMAT("- addrReadLine = %lu\r\n", me->info.addrReadLine);
	ALX_LOGGER_TRACE_FORMAT("- addrReadFile = %lu\r\n", me->info.addrReadFile);
	ALX_LOGGER_TRACE_FORMAT("- addrReadDir = %lu\r\n", me->info.addrReadDir);

	ALX_LOGGER_TRACE_FORMAT("- idWrite = %lu\r\n", (uint32_t)me->info.idWrite);
	ALX_LOGGER_TRACE_FORMAT("- addrWritePosition = %lu\r\n", me->info.addrWritePosition);
	ALX_LOGGER_TRACE_FORMAT("- addrWriteLine = %lu\r\n", me->info.addrWriteLine);
	ALX_LOGGER_TRACE_FORMAT("- addrWriteFile = %lu\r\n", me->info.addrWriteFile);
	ALX_LOGGER_TRACE_FORMAT("- addrWriteDir = %lu\r\n", me->info.addrWriteDir);

	ALX_LOGGER_TRACE_FORMAT("- crc = 0x%04X\r\n", me->info.crc);
	ALX_LOGGER_TRACE_FORMAT("\r\n");

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Trace_ReadLog(AlxLogger* me, char* log, uint32_t numOfLogs)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// TV: TODO
	ALX_LOGGER_ASSERT(false);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Trace_WriteLog(AlxLogger* me, const char* log, uint32_t numOfLogs, bool appendLogDelim)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// TV: TODO
	ALX_LOGGER_ASSERT(false);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Data_ReadLog(AlxLogger* me, char* log, uint32_t numOfLogs)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);
	ALX_LOGGER_ASSERT(numOfLogs == 1);	// Only single log handling supported


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";
	uint32_t readLenActual = 0;


	//------------------------------------------------------------------------------
	// Check idLogDelta
	//------------------------------------------------------------------------------
	int64_t idLogDelta = me->info.idWrite - me->info.idRead;
	if (idLogDelta == 0)
	{
		return AlxLogger_ErrNoReadLog;
	}
	else if (idLogDelta < 0)
	{
		ALX_LOGGER_ASSERT(false);	// We should never get here
	}


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------

	// Open
	sprintf(filePath, "/%lu/%lu.csv", me->info.addrReadDir, me->info.addrReadFile);
	status = AlxFs_File_Open(me->alxFs, &file, filePath, "r");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Seek
	uint32_t filePositionNew = 0;
	status = AlxFs_File_Seek(me->alxFs, &file, me->info.addrReadPosition, AlxFs_File_Seek_Origin_Set, &filePositionNew);
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Read
	status = AlxFs_File_ReadStrUntil(me->alxFs, &file, log, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLenActual);
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Handle IDs & Addresses
	//------------------------------------------------------------------------------

	// idLog
	me->info.idRead++;

	// addrPos
	me->info.addrReadPosition = me->info.addrReadPosition + readLenActual;

	// addrLine
	me->info.addrReadLine++;
	if (me->info.addrReadLine >= me->numOfLogsPerFile)
	{
		// Reset
		me->info.addrReadPosition = 0;
		me->info.addrReadLine = 0;

		// addrFile
		me->info.addrReadFile++;
		if (me->info.addrReadFile >= me->numOfFilesPerDir)
		{
			// Reset
			me->info.addrReadFile = 0;

			// addrDir
			me->info.addrReadDir++;
			if (me->info.addrReadDir >= me->numOfDir)
			{
				// Reset
				me->info.addrReadDir = 0;
			}
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
Alx_Status AlxLogger_Data_WriteLog(AlxLogger* me, const char* log, uint32_t numOfLogs, bool appendLogDelim)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);
	ALX_LOGGER_ASSERT(numOfLogs == 1);	// Only single log handling supported


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";
	uint32_t writeLen = strlen(log);


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------

	// Open
	sprintf(filePath, "/%lu/%lu.csv", me->info.addrWriteDir, me->info.addrWriteFile);
	status = AlxFs_File_Open(me->alxFs, &file, filePath, "a");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Write log
	status = AlxFs_File_WriteStr(me->alxFs, &file, log);
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Write log delimiter
	if (appendLogDelim)
	{
		status = AlxFs_File_WriteStr(me->alxFs, &file, me->logDelim);
		if (status != Alx_Ok)
		{
			AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Handle IDs & Addresses
	//------------------------------------------------------------------------------

	// idLog
	me->info.idWrite++;

	// addrPos
	me->info.addrWritePosition = me->info.addrWritePosition + writeLen;

	// addrLine
	me->info.addrWriteLine++;
	if (me->info.addrWriteLine >= me->numOfLogsPerFile)
	{
		// Reset
		me->info.addrWritePosition = 0;
		me->info.addrWriteLine = 0;

		// addrFile
		me->info.addrWriteFile++;
		if (me->info.addrWriteFile >= me->numOfFilesPerDir)
		{
			// Reset
			me->info.addrWriteFile = 0;

			// addrDir
			me->info.addrWriteDir++;
			if (me->info.addrWriteDir >= me->numOfDir)
			{
				// Reset
				me->info.addrWriteDir = 0;
			}

			// If needed, discard oldest logs by incrementing addrDirRead
			if (me->info.addrWriteDir == me->info.addrReadDir)
			{
				// Increment idLogRead to next nearest multiple
				uint64_t remainder = me->info.idRead % me->numOfLogsPerDir;
				uint64_t delta = me->numOfLogsPerDir - remainder;
				me->info.idRead = me->info.idRead + delta;

				// Reset
				me->info.addrReadPosition = 0;
				me->info.addrReadLine = 0;
				me->info.addrReadFile = 0;

				// addrDir
				me->info.addrReadDir++;
				if (me->info.addrReadDir >= me->numOfDir)
				{
					// Reset
					me->info.addrReadDir = 0;
				}
			}

			// Clear next write dir
			status = AlxLogger_Fs_ClearWriteDir(me);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}
		}
	}


	//------------------------------------------------------------------------------
	// Store Info
	//------------------------------------------------------------------------------
	if (me->info.addrWriteLine == 0)	// Every new file
	{
		status = AlxLogger_Fs_StoreInfo(me, false);
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE("Err");
			return status;
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxLogger_Fs_Prepare(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
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

		// Load & Check info
		status = AlxLogger_Fs_LoadAndCheckInfo(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); break; }

		// Repair write file
		status = AlxLogger_Fs_RepairWriteFile(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Store info
		status = AlxLogger_Fs_StoreInfo(me, false);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

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
	status = AlxLogger_Fs_CreateDirAndFiles(me);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Store default info
	status = AlxLogger_Fs_StoreInfo(me, true);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_LoadAndCheckInfo(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t lenActual = 0;
	uint32_t validatedCrc = 0;
	bool isCrcOk = false;


	//------------------------------------------------------------------------------
	// Load
	//------------------------------------------------------------------------------

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_INFO_FILE_PATH, "r");
	if (status != Alx_Ok)
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
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------

	// Check length
	if (lenActual != sizeof(me->info))
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check CRC
	isCrcOk = AlxCrc_IsOk(&me->alxCrc, (uint8_t*)&me->info, sizeof(me->info), &validatedCrc);
	if (isCrcOk != true)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check magic number
	if (me->info.magicNumber != ALX_LOGGER_INFO_MAGIC_NUMBER)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check version
	if (me->info.version != ALX_LOGGER_INFO_VERSION)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of directories
	if (me->info.numOfDir != me->numOfDir)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of files per directory
	if (me->info.numOfFilesPerDir != me->numOfFilesPerDir)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of logs per file
	if (me->info.numOfLogsPerFile != me->numOfLogsPerFile)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_StoreInfo(AlxLogger* me, bool storeDefault)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};


	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------
	if (storeDefault)
	{
		me->info.magicNumber = ALX_LOGGER_INFO_MAGIC_NUMBER;
		me->info.version = ALX_LOGGER_INFO_VERSION;

		me->info.numOfDir = me->numOfDir;
		me->info.numOfFilesPerDir = me->numOfFilesPerDir;
		me->info.numOfLogsPerFile = me->numOfLogsPerFile;

		me->info.idRead = 0;
		me->info.idWrite = 0;

		me->info.addrReadPosition = 0;
		me->info.addrWritePosition = 0;
		me->info.addrReadLine = 0;
		me->info.addrWriteLine = 0;
		me->info.addrReadFile = 0;
		me->info.addrWriteFile = 0;
		me->info.addrReadDir = 0;
		me->info.addrWriteDir = 0;
	}

	uint32_t crcLen = AlxCrc_GetLen(&me->alxCrc);
	uint32_t infoLenWithoutCrc = sizeof(me->info) - crcLen;
	me->info.crc = AlxCrc_Calc(&me->alxCrc, (uint8_t*)&me->info, infoLenWithoutCrc);


	//------------------------------------------------------------------------------
	// Store
	//------------------------------------------------------------------------------

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_INFO_FILE_PATH, "w");
	if (status != Alx_Ok)
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
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_CreateDirAndFiles(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_Dir dir = {};
	AlxFs_File file = {};
	char dirPath[ALX_LOGGER_PATH_LEN_MAX] = "";
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";
	AlxTimSw alxTimSw_DirFilePrepSingle;
	AlxTimSw alxTimSw_DirFilePrepAll;
	AlxTimSw_Ctor(&alxTimSw_DirFilePrepSingle, false);
	AlxTimSw_Ctor(&alxTimSw_DirFilePrepAll, false);


	//------------------------------------------------------------------------------
	// Create Dir & Files
	//------------------------------------------------------------------------------

	// Trace
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Started Creating Dir & Files\r\n");

	// Start timer
	AlxTimSw_Start(&alxTimSw_DirFilePrepAll);

	// Create directories
	for (uint32_t i = 0; i < me->numOfFilesPerDir; i++)
	{
		// Start timer
		AlxTimSw_Start(&alxTimSw_DirFilePrepSingle);

		// Make dir
		sprintf(dirPath, "/%lu", i);
		status = AlxFs_Dir_Make(me->alxFs, dirPath);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Open dir
		status = AlxFs_Dir_Open(me->alxFs, &dir, dirPath);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Create files
		for (uint32_t y = 0; y < me->numOfFilesPerDir; y++)
		{
			// Open file
			sprintf(filePath, "%s/%lu.csv", dirPath, y);
			status = AlxFs_File_Open(me->alxFs, &file, filePath, "w");
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
		}

		// Close dir
		status = AlxFs_Dir_Close(me->alxFs, &dir);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Trace
		uint32_t dirFilePrepSingle_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepSingle);
		ALX_LOGGER_TRACE_FORMAT("Created dir '%s' with %lu files in %lu sec\r\n", dirPath, me->numOfFilesPerDir, dirFilePrepSingle_sec);
	}

	// Trace
	uint32_t dirFilePrepAllTime_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepAll);
	ALX_LOGGER_TRACE_FORMAT("Created %lu dir with %lu files, total %lu files in %lu sec\r\n", me->numOfDir, me->numOfFilesPerDir, me->numOfFiles, dirFilePrepAllTime_sec);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_ClearWriteDir(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";


	//------------------------------------------------------------------------------
	// Clear
	//------------------------------------------------------------------------------
	for (uint32_t i = 0; i < me->numOfFilesPerDir; i++)
	{
		// Open
		sprintf(filePath, "/%lu/%lu.csv", me->info.addrWriteDir, i);
		status = AlxFs_File_Open(me->alxFs, &file, filePath, "w");
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE("Err");
			return status;
		}

		// Close
		status = AlxFs_File_Close(me->alxFs, &file);
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE("Err");
			return status;
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_Fs_RepairWriteFile(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";
	char log[ALX_LOGGER_LOG_LEN_MAX] = "";
	uint32_t readLenActual = 0;


	//------------------------------------------------------------------------------
	// Repair
	//------------------------------------------------------------------------------

	// Open
	sprintf(filePath, "/%lu/%lu.csv", me->info.addrWriteDir, me->info.addrWriteFile);
	status = AlxFs_File_Open(me->alxFs, &file, filePath, "r+");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Read lines until end-of-file or last corrupted line
	while (true)
	{
		// Read
		status = AlxFs_File_ReadStrUntil(me->alxFs, &file, log, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLenActual);
		if ((status == AlxFs_ErrNoDelim) && (readLenActual == 0))
		{
			// Break, we reached end of file, all lines are OK
			break;
		}
		else if (status == AlxFs_ErrNoDelim)
		{
			// Truncate, with this we eliminate last corrupted line
			status = AlxFs_File_Truncate(me->alxFs, &file, me->info.addrWritePosition);
			if (status != Alx_Ok)
			{
				AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
				ALX_FS_TRACE("Err");
				return status;
			}

			// Break
			break;
		}
		else if (status != Alx_Ok)
		{
			AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}

		// Increment addr
		me->info.addrWritePosition = me->info.addrWritePosition + readLenActual;
		me->info.addrWriteLine++;
	}


	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
