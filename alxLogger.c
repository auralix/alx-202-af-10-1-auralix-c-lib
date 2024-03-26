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
static Alx_Status AlxLogger_Prepare(AlxLogger* me);
static Alx_Status AlxLogger_LoadMetadata(AlxLogger* me);
static Alx_Status AlxLogger_StoreMetadata_Private(AlxLogger* me, AlxLogger_StoreMetadata_Config config);
static Alx_Status AlxLogger_CreateDirAndFiles(AlxLogger* me);
static Alx_Status AlxLogger_CheckRepairWriteFile(AlxLogger* me);
static Alx_Status AlxLogger_ClearWriteDir(AlxLogger* me);
static bool AlxLogger_IsLogToReadAvailable(AlxLogger* me);
static uint32_t AlxLogger_GetNumOfLogsToReadAvailable_Private(AlxLogger* me);
static uint32_t AlxLogger_GetNumOfLogsEndPosition(AlxLogger* me, const char* logs, uint32_t numOfLogs);


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

	// Parameters - Private
	me->numOfFilesTotal = me->numOfFilesPerDir * me->numOfDir;
	me->numOfLogsTotal = me->numOfFilesTotal * me->numOfLogsPerFile;
	me->numOfLogsPerDirTotal = me->numOfFilesPerDir * me->numOfLogsPerFile;

	// Variables
	memset(&me->md, 0, sizeof(me->md));
	memset(&me->mdStored, 0, sizeof(me->mdStored));
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

	// Trace
	ALX_LOGGER_TRACE_FORMAT("\r\n");
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Init started\r\n");

	// Prepare FS
	Alx_Status status = AlxLogger_Prepare(me);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Trace
	uint32_t numOfLogsToReadAvailable = AlxLogger_GetNumOfLogsToReadAvailable_Private(me);
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Totals\r\n");
	ALX_LOGGER_TRACE_FORMAT("- numOfFilesTotal = %lu\r\n", me->numOfFilesTotal);
	ALX_LOGGER_TRACE_FORMAT("- numOfLogsTotal = %lu\r\n", me->numOfLogsTotal);
	ALX_LOGGER_TRACE_FORMAT("- numOfLogsPerDirTotal = %lu\r\n", me->numOfLogsPerDirTotal);
	ALX_LOGGER_TRACE_FORMAT("- numOfLogsToReadAvailable = %lu\r\n", numOfLogsToReadAvailable);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Format(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Format
	Alx_Status status = AlxFs_Format(me->alxFs);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Read(AlxLogger* me, char* logs, uint32_t numOfLogs, uint32_t* numOfLogsActual)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";
	uint32_t logNum = 0;
	uint32_t readLen = 0;
	uint32_t readLenTotal = 0;


	//------------------------------------------------------------------------------
	// Loop
	//------------------------------------------------------------------------------
	while (true)
	{
		//------------------------------------------------------------------------------
		// Handle First Log & First Line
		//------------------------------------------------------------------------------
		if (logNum == 0)
		{
			// Check if log-to-read available
			if (AlxLogger_IsLogToReadAvailable(me) == false)
			{
				// Return
				*numOfLogsActual = logNum;
				return AlxLogger_ErrNoReadLog;
			}

			// Open
			sprintf(filePath, "/%lu/%lu.csv", me->md.read.dir, me->md.read.file);
			status = AlxFs_File_Open(me->alxFs, &file, filePath, "r");
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}

			// Seek
			uint32_t filePositionNew = 0;
			status = AlxFs_File_Seek(me->alxFs, &file, me->md.read.pos, AlxFs_File_Seek_Origin_Set, &filePositionNew);
			if (status != Alx_Ok)
			{
				AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
				ALX_FS_TRACE("Err");
				return status;
			}
		}
		else if (me->md.read.line == 0)
		{
			// Close existing file
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}

			// Open new file
			sprintf(filePath, "/%lu/%lu.csv", me->md.read.dir, me->md.read.file);
			status = AlxFs_File_Open(me->alxFs, &file, filePath, "r");
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}
		}


		//------------------------------------------------------------------------------
		// Read Log
		//------------------------------------------------------------------------------
		status = AlxFs_File_ReadStrUntil(me->alxFs, &file, logs + readLenTotal, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLen);
		if (status != Alx_Ok)
		{
			AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}


		//------------------------------------------------------------------------------
		// Handle IDs & Addresses
		//------------------------------------------------------------------------------

		// read.id
		me->md.read.id++;

		// read.pos
		me->md.read.pos = me->md.read.pos + readLen;

		// read.line
		me->md.read.line++;
		if (me->md.read.line >= me->numOfLogsPerFile)
		{
			// Reset
			me->md.read.pos = 0;
			me->md.read.line = 0;

			// read.file
			me->md.read.file++;
			if (me->md.read.file >= me->numOfFilesPerDir)
			{
				// Reset
				me->md.read.file = 0;

				// read.dir
				me->md.read.dir++;
				if (me->md.read.dir >= me->numOfDir)
				{
					// Reset
					me->md.read.dir = 0;
				}
			}
		}


		//------------------------------------------------------------------------------
		// Increment logNum, readLenTotal
		//------------------------------------------------------------------------------
		logNum++;
		readLenTotal = readLenTotal + readLen;



		//------------------------------------------------------------------------------
		// Handle Last Log & Check if Log-To-Read Available
		//------------------------------------------------------------------------------
		if (logNum == numOfLogs)
		{
			// Close
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}

			// Break
			status = Alx_Ok;
			break;
		}
		else if (AlxLogger_IsLogToReadAvailable(me) == false)
		{
			// Close
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}

			// Break
			status =  AlxLogger_ErrNoReadLog;
			break;
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	*numOfLogsActual = logNum;
	return status;
}
Alx_Status AlxLogger_Write(AlxLogger* me, const char* logs, uint32_t numOfLogs)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char filePath[ALX_LOGGER_PATH_LEN_MAX] = "";
	uint32_t logNum = 0;
	uint32_t writeLen = 0;
	uint32_t writeLenTotal = 0;
	int64_t numOfLogsPerFileRemaining = 0;
	uint32_t numOfLogsToWrite = 0;
	bool wereOldestReadLogsDiscarded = false;


	//------------------------------------------------------------------------------
	// Loop
	//------------------------------------------------------------------------------
	while (true)
	{
		//------------------------------------------------------------------------------
		// Open
		//------------------------------------------------------------------------------
		sprintf(filePath, "/%lu/%lu.csv", me->md.write.dir, me->md.write.file);
		status = AlxFs_File_Open(me->alxFs, &file, filePath, "a");
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE("Err");
			return status;
		}


		//------------------------------------------------------------------------------
		// Prepare
		//------------------------------------------------------------------------------
		numOfLogsPerFileRemaining = (int64_t)me->numOfLogsPerFile - (int64_t)me->md.write.line;
		ALX_LOGGER_ASSERT(numOfLogsPerFileRemaining > 0);
		if (numOfLogsPerFileRemaining > numOfLogs)
		{
			numOfLogsToWrite = numOfLogs;
			writeLen = strlen(logs);
		}
		else
		{
			numOfLogsToWrite = numOfLogsPerFileRemaining;
			writeLen = AlxLogger_GetNumOfLogsEndPosition(me, (void*)logs + writeLenTotal, numOfLogsToWrite);
		}


		//------------------------------------------------------------------------------
		// Write
		//------------------------------------------------------------------------------
		status = AlxFs_File_Write(me->alxFs, &file, (void*)logs + writeLenTotal, writeLen);
		if (status != Alx_Ok)
		{
			AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}


		//------------------------------------------------------------------------------
		// Close
		//------------------------------------------------------------------------------
		status = AlxFs_File_Close(me->alxFs, &file);
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE("Err");
			return status;
		}


		//------------------------------------------------------------------------------
		// Handle IDs & Addresses
		//------------------------------------------------------------------------------

		// write.id
		me->md.write.id = me->md.write.id + numOfLogsToWrite;

		// write.pos
		me->md.write.pos = me->md.write.pos + writeLen;

		// write.line
		me->md.write.line = me->md.write.line + numOfLogsToWrite;
		if (me->md.write.line >= me->numOfLogsPerFile)
		{
			// Reset
			me->md.write.pos = 0;
			me->md.write.line = 0;

			// write.file
			me->md.write.file++;
			if (me->md.write.file >= me->numOfFilesPerDir)
			{
				// Reset
				me->md.write.file = 0;

				// write.dir
				me->md.write.dir++;
				if (me->md.write.dir >= me->numOfDir)
				{
					// Reset
					me->md.write.dir = 0;
				}

				// If needed, discard oldest logs by incrementing read.dir
				if (me->md.write.dir == me->md.read.dir)
				{
					// Increment read.id to next nearest multiple
					uint64_t remainder = me->md.read.id % me->numOfLogsPerDirTotal;
					uint64_t delta = me->numOfLogsPerDirTotal - remainder;
					me->md.read.id = me->md.read.id + delta;

					// Reset
					me->md.read.pos = 0;
					me->md.read.line = 0;
					me->md.read.file = 0;

					// read.dir
					me->md.read.dir++;
					if (me->md.read.dir >= me->numOfDir)
					{
						// Reset
						me->md.read.dir = 0;
					}

					// Set
					wereOldestReadLogsDiscarded = true;
				}

				// Clear next write dir
				status = AlxLogger_ClearWriteDir(me);
				if (status != Alx_Ok)
				{
					ALX_FS_TRACE("Err");
					return status;
				}
			}
		}


		//------------------------------------------------------------------------------
		// Store Metadata
		//------------------------------------------------------------------------------
		if (wereOldestReadLogsDiscarded)	// If oldest read logs were discared, store read & write
		{
			status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreReadWrite);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}
		}
		else if (me->md.write.line == 0)	// If new file, store write only
		{
			status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreWrite);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE("Err");
				return status;
			}
		}


		//------------------------------------------------------------------------------
		// Increment logNum, writeLenTotal
		//------------------------------------------------------------------------------
		logNum = logNum + numOfLogsToWrite;
		writeLenTotal = writeLenTotal + writeLen;


		//------------------------------------------------------------------------------
		// Handle Last Log
		//------------------------------------------------------------------------------
		if (logNum == numOfLogs)
		{
			// Break
			status = Alx_Ok;
			break;
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}
uint32_t AlxLogger_GetNumOfLogsToReadAvailable(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return AlxLogger_GetNumOfLogsToReadAvailable_Private(me);
}
Alx_Status AlxLogger_StoreMetadata(AlxLogger* me, AlxLogger_StoreMetadata_Config config)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return AlxLogger_StoreMetadata_Private(me, config);
}
AlxLogger_Metadata AlxLogger_GetMetadataCurrent(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->md;
}
AlxLogger_Metadata AlxLogger_GetMetadataStored(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->mdStored;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static Alx_Status AlxLogger_Prepare(AlxLogger* me)
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

		// Load metadata
		status = AlxLogger_LoadMetadata(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); break; }

		// Set metadata current
		me->md = me->mdStored;

		// Trace
		ALX_LOGGER_TRACE_FORMAT("AlxLogger - Load metadata OK\r\n");
		ALX_LOGGER_TRACE_FORMAT("- magicNumber = 0x%08lX\r\n", me->md.magicNumber);
		ALX_LOGGER_TRACE_FORMAT("- version = %lu\r\n", me->md.version);

		ALX_LOGGER_TRACE_FORMAT("- numOfDir = %lu\r\n", me->md.numOfDir);
		ALX_LOGGER_TRACE_FORMAT("- numOfFilesPerDir = %lu\r\n", me->md.numOfFilesPerDir);
		ALX_LOGGER_TRACE_FORMAT("- numOfLogsPerFile = %lu\r\n", me->md.numOfLogsPerFile);

		ALX_LOGGER_TRACE_FORMAT("- addrRead.id = %lu\r\n", (uint32_t)me->md.read.id);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.pos = %lu\r\n", me->md.read.pos);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.line = %lu\r\n", me->md.read.line);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.file = %lu\r\n", me->md.read.file);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.dir = %lu\r\n", me->md.read.dir);

		ALX_LOGGER_TRACE_FORMAT("- addrWrite.id = %lu\r\n", (uint32_t)me->md.write.id);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.pos = %lu\r\n", me->md.write.pos);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.line = %lu\r\n", me->md.write.line);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.file = %lu\r\n", me->md.write.file);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.dir = %lu\r\n", me->md.write.dir);

		ALX_LOGGER_TRACE_FORMAT("- crc = 0x%04X\r\n", me->md.crc);

		// Repair write file
		status = AlxLogger_CheckRepairWriteFile(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Store metadata
		status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreReadWrite);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Trace
		ALX_LOGGER_TRACE_FORMAT("AlxLogger - Store metadata after Check/Repair current write_file OK\r\n");
		ALX_LOGGER_TRACE_FORMAT("- magicNumber = 0x%08lX\r\n", me->md.magicNumber);
		ALX_LOGGER_TRACE_FORMAT("- version = %lu\r\n", me->md.version);

		ALX_LOGGER_TRACE_FORMAT("- numOfDir = %lu\r\n", me->md.numOfDir);
		ALX_LOGGER_TRACE_FORMAT("- numOfFilesPerDir = %lu\r\n", me->md.numOfFilesPerDir);
		ALX_LOGGER_TRACE_FORMAT("- numOfLogsPerFile = %lu\r\n", me->md.numOfLogsPerFile);

		ALX_LOGGER_TRACE_FORMAT("- addrRead.id = %lu\r\n", (uint32_t)me->md.read.id);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.pos = %lu\r\n", me->md.read.pos);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.line = %lu\r\n", me->md.read.line);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.file = %lu\r\n", me->md.read.file);
		ALX_LOGGER_TRACE_FORMAT("- addrRead.dir = %lu\r\n", me->md.read.dir);

		ALX_LOGGER_TRACE_FORMAT("- addrWrite.id = %lu\r\n", (uint32_t)me->md.write.id);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.pos = %lu\r\n", me->md.write.pos);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.line = %lu\r\n", me->md.write.line);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.file = %lu\r\n", me->md.write.file);
		ALX_LOGGER_TRACE_FORMAT("- addrWrite.dir = %lu\r\n", me->md.write.dir);

		ALX_LOGGER_TRACE_FORMAT("- crc = 0x%04X\r\n", me->md.crc);

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Load metadata or Check/Repair current write_file ERROR, prepare default\r\n");

	// Format
	status = AlxFs_Format(me->alxFs);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Mount
	status = AlxFs_Mount(me->alxFs);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Create directories & files
	status = AlxLogger_CreateDirAndFiles(me);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Store default metadata
	status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreDefault);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Set metadata current
	me->md = me->mdStored;

	// Trace
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Store default metadata OK\r\n");

	// Return
	return Alx_Ok;
}
static Alx_Status AlxLogger_LoadMetadata(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	AlxLogger_Metadata mdTemp = {};
	uint32_t lenActual = 0;
	uint32_t validatedCrc = 0;
	bool isCrcOk = false;


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_METADATA_FILE_PATH, "r");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Read
	status = AlxFs_File_Read(me->alxFs, &file, &mdTemp, sizeof(mdTemp), &lenActual);
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
	if (lenActual != sizeof(mdTemp))
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check CRC
	isCrcOk = AlxCrc_IsOk(&me->alxCrc, (uint8_t*)&mdTemp, sizeof(mdTemp), &validatedCrc);
	if (isCrcOk != true)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check magic number
	if (mdTemp.magicNumber != ALX_LOGGER_METADATA_MAGIC_NUMBER)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check version
	if (mdTemp.version != ALX_LOGGER_METADATA_VERSION)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of directories
	if (mdTemp.numOfDir != me->numOfDir)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of files per directory
	if (mdTemp.numOfFilesPerDir != me->numOfFilesPerDir)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

	// Check number of logs per file
	if (mdTemp.numOfLogsPerFile != me->numOfLogsPerFile)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------
	me->mdStored = mdTemp;


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_StoreMetadata_Private(AlxLogger* me, AlxLogger_StoreMetadata_Config config)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	AlxLogger_Metadata mdTemp = {};


	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------
	mdTemp.magicNumber = ALX_LOGGER_METADATA_MAGIC_NUMBER;
	mdTemp.version = ALX_LOGGER_METADATA_VERSION;

	mdTemp.numOfDir = me->numOfDir;
	mdTemp.numOfFilesPerDir = me->numOfFilesPerDir;
	mdTemp.numOfLogsPerFile = me->numOfLogsPerFile;

	if (config == AlxLogger_StoreMetadata_Config_StoreDefault)
	{
		mdTemp.read.id			= 0;
		mdTemp.read.pos			= 0;
		mdTemp.read.line		= 0;
		mdTemp.read.file		= 0;
		mdTemp.read.dir			= 0;

		mdTemp.write.id			= 0;
		mdTemp.write.pos		= 0;
		mdTemp.write.line		= 0;
		mdTemp.write.file		= 0;
		mdTemp.write.dir		= 0;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreReadWrite)
	{
		mdTemp.read.id			= me->md.read.id;
		mdTemp.read.pos			= me->md.read.pos;
		mdTemp.read.line		= me->md.read.line;
		mdTemp.read.file		= me->md.read.file;
		mdTemp.read.dir			= me->md.read.dir;

		mdTemp.write.id			= me->md.write.id;
		mdTemp.write.pos		= me->md.write.pos;
		mdTemp.write.line		= me->md.write.line;
		mdTemp.write.file		= me->md.write.file;
		mdTemp.write.dir		= me->md.write.dir;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreRead)
	{
		mdTemp.read.id			= me->md.read.id;
		mdTemp.read.pos			= me->md.read.pos;
		mdTemp.read.line		= me->md.read.line;
		mdTemp.read.file		= me->md.read.file;
		mdTemp.read.dir			= me->md.read.dir;

		mdTemp.write.id			= me->mdStored.write.id;
		mdTemp.write.pos		= me->mdStored.write.pos;
		mdTemp.write.line		= me->mdStored.write.line;
		mdTemp.write.file		= me->mdStored.write.file;
		mdTemp.write.dir		= me->mdStored.write.dir;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreWrite)
	{
		mdTemp.read.id			= me->mdStored.read.id;
		mdTemp.read.pos			= me->mdStored.read.pos;
		mdTemp.read.line		= me->mdStored.read.line;
		mdTemp.read.file		= me->mdStored.read.file;
		mdTemp.read.dir			= me->mdStored.read.dir;

		mdTemp.write.id			= me->md.write.id;
		mdTemp.write.pos		= me->md.write.pos;
		mdTemp.write.line		= me->md.write.line;
		mdTemp.write.file		= me->md.write.file;
		mdTemp.write.dir		= me->md.write.dir;
	}
	else
	{
		ALX_LOGGER_ASSERT(false);	// We should never get here
	}

	uint32_t crcLen = AlxCrc_GetLen(&me->alxCrc);
	uint32_t mdTempLenWithoutCrc = sizeof(mdTemp) - crcLen;
	mdTemp.crc = AlxCrc_Calc(&me->alxCrc, (uint8_t*)&mdTemp, mdTempLenWithoutCrc);


	//------------------------------------------------------------------------------
	// Store
	//------------------------------------------------------------------------------

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_METADATA_FILE_PATH, "w");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Write
	status = AlxFs_File_Write(me->alxFs, &file, &mdTemp, sizeof(mdTemp));
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
	// Set
	//------------------------------------------------------------------------------
	me->mdStored = mdTemp;


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_CreateDirAndFiles(AlxLogger* me)
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
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Started creating dir & files\r\n");

	// Start timer
	AlxTimSw_Start(&alxTimSw_DirFilePrepAll);

	// Create directories
	for (uint32_t i = 0; i < me->numOfDir; i++)
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
	ALX_LOGGER_TRACE_FORMAT("Created %lu dir with %lu files, total %lu files in %lu sec\r\n", me->numOfDir, me->numOfFilesPerDir, me->numOfFilesTotal, dirFilePrepAllTime_sec);
	ALX_LOGGER_TRACE_FORMAT("Each file has %lu logs, so total number of logs is %lu\r\n", me->numOfLogsPerFile, me->numOfLogsTotal);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_CheckRepairWriteFile(AlxLogger* me)
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
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_FORMAT("AlxLogger - Check/Repair current write_file started\r\n");
	ALX_LOGGER_TRACE_FORMAT
	(
		"Address START - dir = %lu, file = %lu, line = %lu, pos = %lu, id = %lu\r\n",
		me->md.write.dir,
		me->md.write.file,
		me->md.write.line,
		me->md.write.pos,
		(uint32_t)me->md.write.id
	);


	//------------------------------------------------------------------------------
	// Check/Repair
	//------------------------------------------------------------------------------

	// Open
	sprintf(filePath, "/%lu/%lu.csv", me->md.write.dir, me->md.write.file);
	status = AlxFs_File_Open(me->alxFs, &file, filePath, "r+");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}

	// Seek
	uint32_t filePositionNew = 0;
	status = AlxFs_File_Seek(me->alxFs, &file, me->md.write.pos, AlxFs_File_Seek_Origin_Set, &filePositionNew);
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Read logs until end-of-file or last corrupted log
	while (true)
	{
		// Read
		status = AlxFs_File_ReadStrUntil(me->alxFs, &file, log, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLenActual);
		if ((status == AlxFs_ErrNoDelim) && (readLenActual == 0))
		{
			// Trace
			ALX_LOGGER_TRACE_FORMAT("Reached end of file, all logs are OK\r\n");

			// Break
			break;
		}
		else if (status == AlxFs_ErrNoDelim)
		{
			// Truncate, with this we eliminate last corrupted log
			status = AlxFs_File_Truncate(me->alxFs, &file, me->md.write.pos);
			if (status != Alx_Ok)
			{
				AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
				ALX_FS_TRACE("Err");
				return status;
			}

			// Trace
			ALX_LOGGER_TRACE_FORMAT("Found and eliminated last corrupted log - %s\r\n", log);

			// Break
			break;
		}
		else if (status != Alx_Ok)
		{
			AlxFs_File_Close(me->alxFs, &file);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}

		// Trace
		ALX_LOGGER_TRACE_FORMAT("[%lu] %s", me->md.write.line, log);

		// Increment addr
		me->md.write.id++;
		me->md.write.pos = me->md.write.pos + readLenActual;
		me->md.write.line++;
	}


	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err");
		return status;
	}


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_FORMAT
	(
		"Address END - dir = %lu, file = %lu, line = %lu, pos = %lu, id = %lu\r\n",
		me->md.write.dir,
		me->md.write.file,
		me->md.write.line,
		me->md.write.pos,
		(uint32_t)me->md.write.id
	);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
static Alx_Status AlxLogger_ClearWriteDir(AlxLogger* me)
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
		sprintf(filePath, "/%lu/%lu.csv", me->md.write.dir, i);
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
static bool AlxLogger_IsLogToReadAvailable(AlxLogger* me)
{
	// Get
	uint32_t numOfLogsToRead = AlxLogger_GetNumOfLogsToReadAvailable_Private(me);

	// Return
	if (numOfLogsToRead > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
static uint32_t AlxLogger_GetNumOfLogsToReadAvailable_Private(AlxLogger* me)
{
	// Calculate
	int64_t numOfLogsToRead = (int64_t)me->md.write.id - (int64_t)me->md.read.id;
	ALX_LOGGER_ASSERT(numOfLogsToRead >= 0);

	// Return
	return (uint32_t)numOfLogsToRead;
}
static uint32_t AlxLogger_GetNumOfLogsEndPosition(AlxLogger* me, const char* logs, uint32_t numOfLogs)
{
	// Local variables
	const char* logsPtr = logs;
	uint32_t logNum = 0;
	uint32_t position = 0;

	// Loop
	while (true)
	{
		// If delimiter, increment logNum
		if (*logsPtr == *me->logDelim)
		{
			logNum++;
		}

		// Increment
		logsPtr++;
		position++;

		// If numOfLogs reached, break
		if (logNum == numOfLogs)
		{
			break;
		}
	}

	// Return
	return position;
}


#endif	// #if defined(ALX_C_LIB)
