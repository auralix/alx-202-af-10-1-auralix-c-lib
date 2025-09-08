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


//------------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------------
static Alx_Status AlxLogger_Prepare(AlxLogger* me);
static Alx_Status AlxLogger_CreateDirAndFiles
(
	AlxLogger* me,
	char* dirPathBuffer,
	uint32_t dirPathBufferSize,
	char* filePathBuffer,
	uint32_t filePathBufferSize
);
static Alx_Status AlxLogger_CheckRepairReadFile(AlxLogger* me);
static Alx_Status AlxLogger_SyncLogWriteFile
(
	AlxLogger* me,
	AlxFs_File *file,
	uint32_t maxReadLen,
	char* buffer,
	uint32_t bufferSize
);
static Alx_Status AlxLogger_CheckRepairWriteFile
(
	AlxLogger* me,
	char* logBuffer,
	uint32_t logBufferSize,
	char* pathBuffer,
	uint32_t pathBufferSize
);
static Alx_Status AlxLogger_ClearWriteDir(AlxLogger* me);


//------------------------------------------------------------------------------
// Metadata
//------------------------------------------------------------------------------
static Alx_Status AlxLogger_Metadata_Load(AlxLogger* me);
static Alx_Status AlxLogger_Metadata_Store_Private(AlxLogger* me, AlxLogger_Metadata_StoreConfig config);


//------------------------------------------------------------------------------
// Status
//------------------------------------------------------------------------------
static bool AlxLogger_Log_AreLogsAvailable(uint64_t idStart, uint64_t idEnd);
static uint64_t AlxLogger_Log_GetNumOfLogs(uint64_t idStart, uint64_t idEnd);
static uint32_t AlxLogger_Log_GetLogsEndPosition(AlxLogger* me, const char* logs, uint32_t numOfLogs);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLogger_Ctor
(
	AlxLogger* me,
	AlxFs* alxFs,
	AlxFsSafe* alxFsSafe,
	uint32_t numOfDir,
	uint32_t numOfFilesPerDir,
	uint32_t numOfLogsPerFile,
	const char* logDelim,
	AlxIoPin* do_DBG_Read,
	AlxIoPin* do_DBG_Write,
	AlxIoPin* do_DBG_StoreReadMetadata,
	AlxIoPin* do_DBG_StoreWriteMetadata
)
{
	// Parameters
	me->alxFs = alxFs;
	me->alxFsSafe = alxFsSafe;
	me->numOfDir = numOfDir;
	me->numOfFilesPerDir = numOfFilesPerDir;
	me->numOfLogsPerFile = numOfLogsPerFile;
	me->logDelim = logDelim;
	me->do_DBG_Read = do_DBG_Read;
	me->do_DBG_Write = do_DBG_Write;
	me->do_DBG_StoreReadMetadata = do_DBG_StoreReadMetadata;
	me->do_DBG_StoreWriteMetadata = do_DBG_StoreWriteMetadata;

	// Parameters - Private
	me->numOfFilesTotal = me->numOfFilesPerDir * me->numOfDir;
	me->numOfLogsTotal = me->numOfFilesTotal * me->numOfLogsPerFile;
	me->numOfLogsPerDirTotal = me->numOfFilesPerDir * me->numOfLogsPerFile;

	// Variables
	memset(&me->md, 0, sizeof(me->md));
	memset(&me->mdStored, 0, sizeof(me->mdStored));
	AlxCrc_Ctor(&me->alxCrc, AlxCrc_Config_Ccitt);
	me->numOfDirCreated = 0;
	me->numOfFilesPerDirCreated = 0;
	AlxMath_Ctor(&me->alxMath_ReadTime_ms);
	AlxMath_Ctor(&me->alxMath_WriteTime_ms);
	memset(&me->alxMath_Data_ReadTime_ms, 0, sizeof(me->alxMath_Data_ReadTime_ms));
	memset(&me->alxMath_Data_WriteTime_ms, 0, sizeof(me->alxMath_Data_WriteTime_ms));

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// General
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------------
Alx_Status AlxLogger_Init(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;
	uint64_t numOfLogsToProcess = 0;
	char numOfLogsToProcess_str[ALX_LOGGER_RESULT_STR_LEN_MAX] = "";

	// Trace
	ALX_LOGGER_TRACE_INF("");
	ALX_LOGGER_TRACE_INF("AlxLogger - Init started");

	// Prepare FS
	status = AlxLogger_Prepare(me);
	if (status != Alx_Ok)
	{
		// Trace
		ALX_LOGGER_TRACE_WRN("Err: %d, Prepare FATAL ERROR, format will occur", status);

		// Format
		status = AlxFs_Format(me->alxFs);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, Format FATAL ERROR, all we can do now is just return..", status);
		}

		// Return
		return Alx_Err;
	}

	// Trace
	numOfLogsToProcess = AlxLogger_Log_GetNumOfLogs(me->md.read.id, me->md.write.id);
	AlxGlobal_Ulltoa(numOfLogsToProcess, numOfLogsToProcess_str);
	ALX_LOGGER_TRACE_INF("AlxLogger - Totals");
	ALX_LOGGER_TRACE_INF("- numOfFilesTotal = %lu", me->numOfFilesTotal);
	ALX_LOGGER_TRACE_INF("- numOfLogsTotal = %lu", me->numOfLogsTotal);
	ALX_LOGGER_TRACE_INF("- numOfLogsPerDirTotal = %lu", me->numOfLogsPerDirTotal);
	ALX_LOGGER_TRACE_INF("- numOfLogsToProcess = %s", numOfLogsToProcess_str);

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

	// Local variables
	Alx_Status status = Alx_Err;

	// Format
	status = AlxFs_Format(me->alxFs);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Return
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// Metadata
//------------------------------------------------------------------------------
AlxLogger_Metadata AlxLogger_Metadata_GetCurrent(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->md;
}
AlxLogger_Metadata AlxLogger_Metadata_GetStored(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->mdStored;
}
Alx_Status AlxLogger_Metadata_Store(AlxLogger* me, AlxLogger_Metadata_StoreConfig config)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return AlxLogger_Metadata_Store_Private(me, config);
}


//------------------------------------------------------------------------------
// Debug
//------------------------------------------------------------------------------
AlxMath_Data AlxLogger_Debug_GetReadTime_ms(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->alxMath_Data_ReadTime_ms;
}
AlxMath_Data AlxLogger_Debug_GetWriteTime_ms(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->alxMath_Data_WriteTime_ms;
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Log Layer
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Read/Write
//------------------------------------------------------------------------------
Alx_Status AlxLogger_Log_Read(AlxLogger* me, char* logs, uint32_t numOfLogs, uint32_t* numOfLogsActual, bool mdUpdate, uint64_t idStart)
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
	char path[ALX_LOGGER_PATH_LEN_MAX] = "";
	uint32_t logNum = 0;
	uint32_t readLen = 0;
	uint32_t readLenTotal = 0;
	uint32_t positionNew = 0;
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);
	uint32_t readTime_ms = 0;
	AlxLogger_Metadata md = {};
	char log[ALX_LOGGER_LOG_LEN_MAX] = "";
	uint32_t readLog = 0;


	//------------------------------------------------------------------------------
	// Local md
	//------------------------------------------------------------------------------
	md = me->md;
	if (mdUpdate == false)
	{
		//------------------------------------------------------------------------------
		// Set md.read
		//------------------------------------------------------------------------------
		md.read.id = idStart;
		md.read.pos = 0;
		md.read.log = (idStart % me->numOfLogsPerDirTotal) % me->numOfLogsPerFile;
		md.read.file = (idStart % me->numOfLogsPerDirTotal) / me->numOfLogsPerFile;
		md.read.dir = (idStart / me->numOfLogsPerDirTotal) % me->numOfDir;


		//------------------------------------------------------------------------------
		// Set read.pos via File Reading
		//------------------------------------------------------------------------------
		if (md.read.pos != md.read.log)
		{
			// Open
			sprintf(path, "/%lu/%lu.csv", md.read.dir, md.read.file);
			status = AlxFs_File_Open(me->alxFs, &file, path, "r");
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
				return status;
			}

			// Read
			while (true)
			{
				// Read
				status = AlxFs_File_ReadStrUntil(me->alxFs, &file, log, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLen);
				if (status != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, readLen=%u, readLenTotal=%u, readLog=%u", status, path, readLen, readLenTotal, readLog);
					Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
					if (statusClose != Alx_Ok)
					{
						ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
						// TV: TODO - Handle close error
					}
					return status;
				}

				// Increment readLog, readLenTotal
				readLog++;
				readLenTotal = readLenTotal + readLen;

				// If read.log reached
				if (readLog == md.read.log)
				{
					// Close
					status = AlxFs_File_Close(me->alxFs, &file);
					if (status != Alx_Ok)
					{
						ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, readLenTotal=%u, readLog=%u", status, path, readLenTotal, readLog);
						// TV: TODO - Handle close error
						return status;
					}

					// Update
					md.read.pos = readLenTotal;

					// Break
					break;
				}
			}
		}
	}


	//------------------------------------------------------------------------------
	// Loop
	//------------------------------------------------------------------------------
	readLenTotal = 0;
	AlxTimSw_Start(&alxTimSw);
	if(me->do_DBG_Read != NULL) AlxIoPin_Set(me->do_DBG_Read);
	while (true)
	{
		//------------------------------------------------------------------------------
		// Handle First Log & First Log in File
		//------------------------------------------------------------------------------
		if (logNum == 0)
		{
			// Check if log-to-process available
			if (AlxLogger_Log_AreLogsAvailable(md.read.id, md.write.id) == false)
			{
				// Update
				if (mdUpdate)
				{
					me->md = md;
				}

				// Break
				status = AlxLogger_ErrNoReadLog;
				break;
			}

			// Open
			sprintf(path, "/%lu/%lu.csv", md.read.dir, md.read.file);
			status = AlxFs_File_Open(me->alxFs, &file, path, "r");
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, logNum, readLenTotal, numOfLogs);
				return status;
			}

			// Seek
			status = AlxFs_File_Seek(me->alxFs, &file, md.read.pos, AlxFs_File_Seek_Origin_Set, &positionNew);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, offset=%d, positionNew=%u, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, md.read.pos, positionNew, logNum, readLenTotal, numOfLogs);
				Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
				if (statusClose != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
					// TV: TODO - Handle close error
				}
				return status;
			}
		}
		else if (md.read.log == 0)
		{
			// Close existing file
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, logNum, readLenTotal, numOfLogs);
				// TV: TODO - Handle close error
				return status;
			}

			// Open new file
			sprintf(path, "/%lu/%lu.csv", md.read.dir, md.read.file);
			status = AlxFs_File_Open(me->alxFs, &file, path, "r");
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, logNum, readLenTotal, numOfLogs);
				return status;
			}
		}


		//------------------------------------------------------------------------------
		// Read Log
		//------------------------------------------------------------------------------
		status = AlxFs_File_ReadStrUntil(me->alxFs, &file, logs + readLenTotal, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLen);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, readLen=%u, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, readLen, logNum, readLenTotal, numOfLogs);
			Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
			if (statusClose != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
				// TV: TODO - Handle close error
			}
			return status;
		}


		//------------------------------------------------------------------------------
		// Handle IDs & Addresses
		//------------------------------------------------------------------------------

		// read.id
		md.read.id++;

		// read.pos
		md.read.pos = md.read.pos + readLen;

		// read.log
		md.read.log++;
		if (md.read.log >= me->numOfLogsPerFile)
		{
			// Reset
			md.read.pos = 0;
			md.read.log = 0;

			// read.file
			md.read.file++;
			if (md.read.file >= me->numOfFilesPerDir)
			{
				// Reset
				md.read.file = 0;

				// read.dir
				md.read.dir++;
				if (md.read.dir >= me->numOfDir)
				{
					// Reset
					md.read.dir = 0;
				}
			}
		}


		//------------------------------------------------------------------------------
		// Increment logNum, readLenTotal
		//------------------------------------------------------------------------------
		logNum++;
		readLenTotal = readLenTotal + readLen;


		//------------------------------------------------------------------------------
		// Handle Last Log & Check if Log-To-Process Available
		//------------------------------------------------------------------------------
		if (logNum == numOfLogs)
		{
			// Close
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, logNum, readLenTotal, numOfLogs);
				// TV: TODO - Handle close error
				return status;
			}

			// Update
			if (mdUpdate)
			{
				me->md = md;
			}

			// Break
			status = Alx_Ok;
			break;
		}
		else if (AlxLogger_Log_AreLogsAvailable(md.read.id, md.write.id) == false)
		{
			ALX_LOGGER_TRACE_WRN("No more logs available (read.id %u, write.id %u, logNum %u, numOfLogs %u)",
				md.read.id, md.write.id, logNum, numOfLogs);

			// Close
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, readLenTotal=%u, numOfLogs=%u", status, path, logNum, readLenTotal, numOfLogs);
				// TV: TODO - Handle close error
				return status;
			}

			// Update
			if (mdUpdate)
			{
				me->md = md;
			}

			// Break
			status =  AlxLogger_ErrNoReadLog;
			break;
		}
	}
	if(me->do_DBG_Read != NULL) AlxIoPin_Reset(me->do_DBG_Read);
	readTime_ms = AlxTimSw_Get_ms(&alxTimSw);
	me->alxMath_Data_ReadTime_ms = AlxMath_Process(&me->alxMath_ReadTime_ms, readTime_ms);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	*numOfLogsActual = logNum;
	return status;
}
Alx_Status AlxLogger_Log_Write(AlxLogger* me, const char* logs, uint32_t numOfLogs)
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
	char path[ALX_LOGGER_PATH_LEN_MAX] = "";
	uint32_t logNum = 0;
	uint32_t writeLen = 0;
	uint32_t writeLenTotal = 0;
	int64_t numOfLogsToWriteRemaining = 0;
	int64_t numOfLogsSpacePerFileRemaining = 0;
	uint32_t numOfLogsToWrite = 0;
	bool wereOldestReadLogsDiscarded = false;
	AlxTimSw alxTimSw;
	AlxTimSw_Ctor(&alxTimSw, false);
	uint32_t writeTime_ms = 0;


	//------------------------------------------------------------------------------
	// Loop
	//------------------------------------------------------------------------------
	AlxTimSw_Start(&alxTimSw);
	if(me->do_DBG_Write != NULL) AlxIoPin_Set(me->do_DBG_Write);
	while (true)
	{
		//------------------------------------------------------------------------------
		// Open
		//------------------------------------------------------------------------------
		sprintf(path, "/%lu/%lu.csv", me->md.write.dir, me->md.write.file);
		status = AlxFs_File_Open(me->alxFs, &file, path, "a");
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
			return status;
		}


		//------------------------------------------------------------------------------
		// Prepare
		//------------------------------------------------------------------------------

		// Set numOfLogsToWriteRemaining
		numOfLogsToWriteRemaining = (int64_t)numOfLogs - (int64_t)logNum;
		ALX_LOGGER_ASSERT(numOfLogsToWriteRemaining > 0);

		// Set numOfLogsSpacePerFileRemaining
		numOfLogsSpacePerFileRemaining = (int64_t)me->numOfLogsPerFile - (int64_t)me->md.write.log;
		ALX_LOGGER_ASSERT(numOfLogsSpacePerFileRemaining > 0);

		// If enough space in current file, just write all remaining logs to current file, Else write as many as possible logs in current file
		if (numOfLogsSpacePerFileRemaining >= numOfLogsToWriteRemaining)
		{
			numOfLogsToWrite = numOfLogsToWriteRemaining;
		}
		else
		{
			numOfLogsToWrite = numOfLogsSpacePerFileRemaining;
		}

		// Set writeLen
		writeLen = AlxLogger_Log_GetLogsEndPosition(me, (void*)logs + writeLenTotal, numOfLogsToWrite);


		//------------------------------------------------------------------------------
		// Write
		//------------------------------------------------------------------------------
		status = AlxFs_File_Write(me->alxFs, &file, (void*)logs + writeLenTotal, writeLen);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, writeLen=%u, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, writeLen, logNum, writeLenTotal, numOfLogs);
			Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
			if (statusClose != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
				// TV: TODO - Handle close error
			}
			return status;
		}


		//------------------------------------------------------------------------------
		// Close
		//------------------------------------------------------------------------------
		status = AlxFs_File_Close(me->alxFs, &file);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
			// TV: TODO - Handle close error
			return status;
		}


		//------------------------------------------------------------------------------
		// Handle IDs & Addresses
		//------------------------------------------------------------------------------

		// write.id
		me->md.write.id = me->md.write.id + numOfLogsToWrite;

		// write.pos
		me->md.write.pos = me->md.write.pos + writeLen;

		// write.log
		me->md.write.log = me->md.write.log + numOfLogsToWrite;
		if (me->md.write.log >= me->numOfLogsPerFile)
		{
			// Reset
			me->md.write.pos = 0;
			me->md.write.log = 0;

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
					me->md.read.log = 0;
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

				// oldest.id - Only start handling after inital wrap around
				if (me->md.write.id >= me->numOfLogsTotal)
				{
					// Increment oldest.id by number of logs per directory
					me->md.oldest.id = me->md.oldest.id + me->numOfLogsPerDirTotal;

					// Reset
					me->md.oldest.pos = 0;
					me->md.oldest.log = 0;
					me->md.oldest.file = 0;

					// oldest.dir
					me->md.oldest.dir++;
					if (me->md.oldest.dir >= me->numOfDir)
					{
						// Reset
						me->md.oldest.dir = 0;
					}
				}

				// Clear next write dir
				status = AlxLogger_ClearWriteDir(me);
				if (status != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
					return status;
				}
			}
		}


		//------------------------------------------------------------------------------
		// Store Metadata
		//------------------------------------------------------------------------------
		if (wereOldestReadLogsDiscarded)	// If oldest read logs were discared, store read & write & oldest
		{
			ALX_LOGGER_TRACE_INF("Storing metadata because oldest logs were discarded");
			status = AlxLogger_Metadata_Store_Private(me, AlxLogger_Metadata_StoreConfig_ReadWriteOldest);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
				return status;
			}
		}
		else if (me->md.write.log == 0)	// If new file, store write only, if new dir store write & oldest
		{
			ALX_LOGGER_TRACE_INF("Storing metadata because of new write %s", me->md.write.file == 0 ? "dir" : "file");
			AlxLogger_Metadata_StoreConfig config = me->md.write.file == 0
				? AlxLogger_Metadata_StoreConfig_WriteOldest : AlxLogger_Metadata_StoreConfig_Write;
			status = AlxLogger_Metadata_Store_Private(me, config);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
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
	if(me->do_DBG_Write != NULL) AlxIoPin_Reset(me->do_DBG_Write);
	writeTime_ms = AlxTimSw_Get_ms(&alxTimSw);
	me->alxMath_Data_WriteTime_ms = AlxMath_Process(&me->alxMath_WriteTime_ms, writeTime_ms);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}


//------------------------------------------------------------------------------
// Status
//------------------------------------------------------------------------------
uint64_t AlxLogger_Log_GetNumOfLogsStored(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Get
	uint64_t numOfLogsStored = AlxLogger_Log_GetNumOfLogs(me->md.oldest.id, me->md.write.id);

	// Return
	return numOfLogsStored;
}
uint64_t AlxLogger_Log_GetNumOfLogsToProcess(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Get
	uint64_t numOfLogsToProcess = AlxLogger_Log_GetNumOfLogs(me->md.read.id, me->md.write.id);

	// Return
	return numOfLogsToProcess;
}
Alx_Status AlxLogger_Log_GetIdStoredOldest(AlxLogger* me, uint64_t* idStoredOldest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool areLogsAvailable = AlxLogger_Log_AreLogsAvailable(me->md.oldest.id, me->md.write.id);
	if (areLogsAvailable == false)
	{
		*idStoredOldest = 0;
		return Alx_Err;
	}

	// Return
	*idStoredOldest = me->md.oldest.id;
	return Alx_Ok;
}
Alx_Status AlxLogger_Log_GetIdStoredNewest(AlxLogger* me, uint64_t* idStoredNewest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool areLogsAvailable = AlxLogger_Log_AreLogsAvailable(me->md.oldest.id, me->md.write.id);
	if (areLogsAvailable == false)
	{
		*idStoredNewest = 0;
		return Alx_Err;
	}

	// Return
	*idStoredNewest = me->md.write.id - 1;
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// Management
//------------------------------------------------------------------------------
Alx_Status AlxLogger_Log_DiscardLogsToProcess(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Set metadata current read IDs to write IDs (discard logs-to-process)
	me->md.read.id = me->mdStored.write.id;
	me->md.read.pos = me->mdStored.write.pos;
	me->md.read.log = me->mdStored.write.log;
	me->md.read.file = me->mdStored.write.file;
	me->md.read.dir = me->mdStored.write.dir;

	ALX_LOGGER_TRACE_INF("Discarding logs by moving read pointer to write pointer location");

	// Return
	return AlxLogger_Metadata_Store_Private(me, AlxLogger_Metadata_StoreConfig_ReadWriteOldest);
}




//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// File Layer
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Read/Write
//------------------------------------------------------------------------------
Alx_Status AlxLogger_File_Read(AlxLogger* me, const char* path, uint8_t* chunkBuff, uint32_t chunkLen, Alx_Status(*chunkRead_Callback)(void* ctx, void* chunkData, uint32_t chunkLenActual), void* chunkRead_Callback_Ctx, uint32_t* readLen, AlxOsMutex* alxOsMutex)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return AlxFs_File_ReadInChunks(me->alxFs, path, chunkBuff, chunkLen, chunkRead_Callback, chunkRead_Callback_Ctx, readLen, alxOsMutex);
}
Alx_Status AlxLogger_File_ReadFirstLog(AlxLogger* me, const char* path, char* log)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t readLen = 0;

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, path, "r");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err: %d, path=%s", status, path);
		return status;
	}

	// Read
	status = AlxFs_File_ReadStrUntil(me->alxFs, &file, log, me->logDelim, ALX_LOGGER_LOG_LEN_MAX, &readLen);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, readLen=%u", status, path, readLen);
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err: %d, path=%s", status, path);
		// TV: TODO - Handle close error
		return status;
	}

	// Return
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// Status
//------------------------------------------------------------------------------
Alx_Status AlxLogger_File_GetSize(AlxLogger* me, const char* path, uint32_t* size)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Local Variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, path, "r");
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err: %d, path=%s", status, path);
		return status;
	}

	// Get fileSize
	status = AlxFs_File_Size(me->alxFs, &file, size);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err: %d, path=%s, fileSize=%u", status, path, size);
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_FS_TRACE("Err: %d, path=%s", statusClose, path);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err: %d, path=%s", status, path);
		// TV: TODO - Handle close error
		return status;
	}

	// Return
	return Alx_Ok;
}
uint32_t AlxLogger_File_GetNumOfFilesToProcess(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Get
	uint64_t numOfLogsToProcess = AlxLogger_Log_GetNumOfLogs(me->md.read.id, me->md.write.id);

	// Set
	uint64_t numOfFilesToProcess = numOfLogsToProcess / me->numOfLogsPerFile;

	// Return
	return numOfFilesToProcess;
}
Alx_Status AlxLogger_File_GetPathStoredOldest(AlxLogger* me, char* pathStoredOldest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool areLogsAvailable = AlxLogger_Log_AreLogsAvailable(me->md.oldest.id, me->md.write.id);
	if (areLogsAvailable == false)
	{
		strcpy(pathStoredOldest, "");
		return Alx_Err;
	}

	// Prepare
	uint32_t fileOldest = me->md.oldest.file;
	uint32_t dirOldest = me->md.oldest.dir;

	// Return
	sprintf(pathStoredOldest, "/%lu/%lu.csv", dirOldest, fileOldest);
	return Alx_Ok;
}
Alx_Status AlxLogger_File_GetPathStoredNewest(AlxLogger* me, char* pathStoredNewest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool areLogsAvailable = AlxLogger_Log_AreLogsAvailable(me->md.oldest.id, me->md.write.id);
	if (areLogsAvailable == false)
	{
		strcpy(pathStoredNewest, "");
		return Alx_Err;
	}

	// Prepare
	uint64_t idNewest = me->md.write.id - 1;
	uint32_t fileNewest = (idNewest % me->numOfLogsPerDirTotal) / me->numOfLogsPerFile;
	uint32_t dirNewest = (idNewest / me->numOfLogsPerDirTotal) % me->numOfDir;

	// Return
	sprintf(pathStoredNewest, "/%lu/%lu.csv", dirNewest, fileNewest);
	return Alx_Ok;
}
Alx_Status AlxLogger_File_GetPathToProcessOldest(AlxLogger* me, char* pathToProcessOldest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool areLogsAvailable = AlxLogger_Log_AreLogsAvailable(me->md.read.id, me->md.write.id);
	if (areLogsAvailable == false)
	{
		strcpy(pathToProcessOldest, "");
		return Alx_Err;
	}

	// Prepare
	uint32_t fileOldest = me->md.read.file;
	uint32_t dirOldest = me->md.read.dir;

	// Return
	sprintf(pathToProcessOldest, "/%lu/%lu.csv", dirOldest, fileOldest);
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// Management
//------------------------------------------------------------------------------
Alx_Status AlxLogger_File_RewindFilesToProcess(AlxLogger* me, uint32_t numOfFiles)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);
	ALX_LOGGER_ASSERT(numOfFiles == 0);		// Currently only supported rewind of existing file

	// Rewind
	me->md.read.id = me->md.read.id - me->md.read.log;
	me->md.read.pos = 0;
	me->md.read.log = 0;
	// me->md.read.file	// Don't need to change
	// me->md.read.dir	// Don't need to change

	// Return
	return AlxLogger_Metadata_Store_Private(me, AlxLogger_Metadata_StoreConfig_Read);
}
Alx_Status AlxLogger_File_ForwardFilesToProcess(AlxLogger* me, uint32_t numOfFiles)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);
	ALX_LOGGER_ASSERT(numOfFiles == 1);		// Currently only supported forwarding for 1 file


	//------------------------------------------------------------------------------
	// Forward
	//------------------------------------------------------------------------------

	// read.id
	me->md.read.id += (me->md.numOfLogsPerFile - me->md.read.log);

	// read.pos
	me->md.read.pos = 0;

	// read.log
	me->md.read.log = 0;

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


	//------------------------------------------------------------------------------
	// Store Metadata
	//------------------------------------------------------------------------------
	Alx_Status status = AlxLogger_Metadata_Store_Private(me, AlxLogger_Metadata_StoreConfig_Read);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}




//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------------
static Alx_Status AlxLogger_Prepare(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	// Optimization to avoid called functions creating large buffers on stack
	static char buffer[ALX_LOGGER_LOG_LEN_MAX] = "";
	static char pathBuffer[ALX_LOGGER_PATH_LEN_MAX] = "";
	Alx_Status status = Alx_Err;

	//------------------------------------------------------------------------------
	// Try
	//------------------------------------------------------------------------------
	while (true)
	{
		// Mount
		status = AlxFs_Mount(me->alxFs);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			break;
		}

		// Trace
		ALX_LOGGER_TRACE_VRB("AlxLogger - Trace before AlxLogger_Metadata_Load()");
		#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
		AlxFs_Dir_Trace(me->alxFs, "/", false, false);
		AlxFs_File_Trace(me->alxFs, "/md.bin", true);
		AlxFs_File_Trace(me->alxFs, "/mdB.bin", true);
		#endif

		// Load metadata
		status = AlxLogger_Metadata_Load(me);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			break;
		}

		// Set metadata current
		me->md = me->mdStored;

		// Trace
		ALX_LOGGER_TRACE_INF("AlxLogger - Load metadata OK");
		ALX_LOGGER_TRACE_INF("- magicNumber = 0x%08lX", me->md.magicNumber);
		ALX_LOGGER_TRACE_INF("- version = %lu", me->md.version);

		ALX_LOGGER_TRACE_INF("- numOfDir = %lu", me->md.numOfDir);
		ALX_LOGGER_TRACE_INF("- numOfFilesPerDir = %lu", me->md.numOfFilesPerDir);
		ALX_LOGGER_TRACE_INF("- numOfLogsPerFile = %lu", me->md.numOfLogsPerFile);

		AlxGlobal_Ulltoa(me->md.read.id, buffer);
		ALX_LOGGER_TRACE_INF("- read.id = %s", buffer);
		ALX_LOGGER_TRACE_INF("- read.pos = %lu", me->md.read.pos);
		ALX_LOGGER_TRACE_INF("- read.log = %lu", me->md.read.log);
		ALX_LOGGER_TRACE_INF("- read.file = %lu", me->md.read.file);
		ALX_LOGGER_TRACE_INF("- read.dir = %lu", me->md.read.dir);

		AlxGlobal_Ulltoa(me->md.write.id, buffer);
		ALX_LOGGER_TRACE_INF("- write.id = %s", buffer);
		ALX_LOGGER_TRACE_INF("- write.pos = %lu", me->md.write.pos);
		ALX_LOGGER_TRACE_INF("- write.log = %lu", me->md.write.log);
		ALX_LOGGER_TRACE_INF("- write.file = %lu", me->md.write.file);
		ALX_LOGGER_TRACE_INF("- write.dir = %lu", me->md.write.dir);

		AlxGlobal_Ulltoa(me->md.oldest.id, buffer);
		ALX_LOGGER_TRACE_INF("- oldest.id = %s", buffer);
		ALX_LOGGER_TRACE_INF("- oldest.pos = %lu", me->md.oldest.pos);
		ALX_LOGGER_TRACE_INF("- oldest.log = %lu", me->md.oldest.log);
		ALX_LOGGER_TRACE_INF("- oldest.file = %lu", me->md.oldest.file);
		ALX_LOGGER_TRACE_INF("- oldest.dir = %lu", me->md.oldest.dir);

		ALX_LOGGER_TRACE_INF("- crc = 0x%04X", me->md.crc);

		// Repair write file
		status = AlxLogger_CheckRepairWriteFile(
			me,
			buffer,
			ALX_ARR_LEN(buffer),
			pathBuffer,
			ALX_ARR_LEN(pathBuffer));
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			return status;
		}

		// Repair read file
		status = AlxLogger_CheckRepairReadFile(me);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			return status;
		}

		// Store metadata
		status = AlxLogger_Metadata_Store_Private(me, AlxLogger_Metadata_StoreConfig_ReadWriteOldest);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			return status;
		}

		// Trace
		ALX_LOGGER_TRACE_VRB("AlxLogger - Trace after AlxLogger_Metadata_StoreConfig_ReadWriteOldest()");
		#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
		AlxFs_Dir_Trace(me->alxFs, "/", false, false);
		AlxFs_File_Trace(me->alxFs, "/md.bin", true);
		AlxFs_File_Trace(me->alxFs, "/mdB.bin", true);
		#endif

		// Trace
		ALX_LOGGER_TRACE_INF("AlxLogger - Store metadata after Check/Repair current write_file and read_file OK");
		ALX_LOGGER_TRACE_INF("- magicNumber = 0x%08lX", me->md.magicNumber);
		ALX_LOGGER_TRACE_INF("- version = %lu", me->md.version);

		ALX_LOGGER_TRACE_INF("- numOfDir = %lu", me->md.numOfDir);
		ALX_LOGGER_TRACE_INF("- numOfFilesPerDir = %lu", me->md.numOfFilesPerDir);
		ALX_LOGGER_TRACE_INF("- numOfLogsPerFile = %lu", me->md.numOfLogsPerFile);

		AlxGlobal_Ulltoa(me->md.read.id, buffer);
		ALX_LOGGER_TRACE_INF("- read.id = %s", buffer);
		ALX_LOGGER_TRACE_INF("- read.pos = %lu", me->md.read.pos);
		ALX_LOGGER_TRACE_INF("- read.log = %lu", me->md.read.log);
		ALX_LOGGER_TRACE_INF("- read.file = %lu", me->md.read.file);
		ALX_LOGGER_TRACE_INF("- read.dir = %lu", me->md.read.dir);

		AlxGlobal_Ulltoa(me->md.write.id, buffer);
		ALX_LOGGER_TRACE_INF("- write.id = %s", buffer);
		ALX_LOGGER_TRACE_INF("- write.pos = %lu", me->md.write.pos);
		ALX_LOGGER_TRACE_INF("- write.log = %lu", me->md.write.log);
		ALX_LOGGER_TRACE_INF("- write.file = %lu", me->md.write.file);
		ALX_LOGGER_TRACE_INF("- write.dir = %lu", me->md.write.dir);

		AlxGlobal_Ulltoa(me->md.oldest.id, buffer);
		ALX_LOGGER_TRACE_INF("- oldest.id = %s", buffer);
		ALX_LOGGER_TRACE_INF("- oldest.pos = %lu", me->md.oldest.pos);
		ALX_LOGGER_TRACE_INF("- oldest.log = %lu", me->md.oldest.log);
		ALX_LOGGER_TRACE_INF("- oldest.file = %lu", me->md.oldest.file);
		ALX_LOGGER_TRACE_INF("- oldest.dir = %lu", me->md.oldest.dir);

		ALX_LOGGER_TRACE_INF("- crc = 0x%04X", me->md.crc);

		// Return
		return Alx_Ok;
	}


	//------------------------------------------------------------------------------
	// Catch
	//------------------------------------------------------------------------------

	// Trace
	ALX_LOGGER_TRACE_INF("AlxLogger - Load metadata or Check/Repair current write_file ERROR, prepare default");

	// Format
	status = AlxFs_Format(me->alxFs);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Mount
	status = AlxFs_Mount(me->alxFs);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Create directories & files
	status = AlxLogger_CreateDirAndFiles(
		me,
		buffer,
		ALX_ARR_LEN(buffer),
		pathBuffer,
		ALX_ARR_LEN(pathBuffer));
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Store default metadata
	status = AlxLogger_Metadata_Store_Private(me, AlxLogger_Metadata_StoreConfig_Default);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Trace
	ALX_LOGGER_TRACE_VRB("AlxLogger - Trace after AlxLogger_Metadata_StoreConfig_Default()");
	#if ALX_TRACE_LEVEL >= ALX_TRACE_LEVEL_VRB
	AlxFs_Dir_Trace(me->alxFs, "/", false, false);
	AlxFs_File_Trace(me->alxFs, "/md.bin", true);
	AlxFs_File_Trace(me->alxFs, "/mdB.bin", true);
	#endif

	// Set metadata current
	me->md = me->mdStored;

	// Trace
	ALX_LOGGER_TRACE_INF("AlxLogger - Store default metadata OK");

	// Return
	return Alx_Ok;
}

static Alx_Status AlxLogger_CreateDirAndFiles
(
	AlxLogger* me,
	char* dirPathBuffer,
	uint32_t dirPathBufferSize,
	char* filePathBuffer,
	uint32_t filePathBufferSize
)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_Dir dir = {};
	AlxFs_File file = {};
	AlxTimSw alxTimSw_DirFilePrepSingle;
	AlxTimSw alxTimSw_DirFilePrepAll;
	AlxTimSw_Ctor(&alxTimSw_DirFilePrepSingle, false);
	AlxTimSw_Ctor(&alxTimSw_DirFilePrepAll, false);
	uint32_t dirFilePrepSingle_sec = 0;
	uint32_t dirFilePrepAllTime_sec = 0;


	//------------------------------------------------------------------------------
	// Create Dir & Files
	//------------------------------------------------------------------------------

	// Trace
	ALX_LOGGER_TRACE_INF("");
	ALX_LOGGER_TRACE_INF("AlxLogger - Started creating dir & files");

	// Start timer
	AlxTimSw_Start(&alxTimSw_DirFilePrepAll);

	// Create directories
	for (me->numOfDirCreated = 0; me->numOfDirCreated < me->numOfDir; me->numOfDirCreated++)
	{
		// Start timer
		AlxTimSw_Start(&alxTimSw_DirFilePrepSingle);

		// Make dir
		sprintf(dirPathBuffer, "/%lu", me->numOfDirCreated);
		status = AlxFs_Dir_Make(me->alxFs, dirPathBuffer);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", status, dirPathBuffer);
			return status;
		}

		// Open dir
		status = AlxFs_Dir_Open(me->alxFs, &dir, dirPathBuffer);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", status, dirPathBuffer);
			return status;
		}

		// Create files
		for (me->numOfFilesPerDirCreated = 0; me->numOfFilesPerDirCreated < me->numOfFilesPerDir; me->numOfFilesPerDirCreated++)
		{
			// Open file
			sprintf(filePathBuffer, "%s/%lu.csv", dirPathBuffer, me->numOfFilesPerDirCreated);
			status = AlxFs_File_Open(me->alxFs, &file, filePathBuffer, "w");
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, filePath=%s", status, filePathBuffer);
				Alx_Status statusClose = AlxFs_Dir_Close(me->alxFs, &dir);
				if (statusClose != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", statusClose, dirPathBuffer);
					// TV: TODO - Handle close error
				}
				return status;
			}

			// Close file
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, filePath=%s", status, filePathBuffer);
				Alx_Status statusClose = AlxFs_Dir_Close(me->alxFs, &dir);
				if (statusClose != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", statusClose, dirPathBuffer);
					// TV: TODO - Handle close error
				}
				return status;
			}
		}

		// Close dir
		status = AlxFs_Dir_Close(me->alxFs, &dir);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", status, dirPathBuffer);
			// TV: TODO - Handle close error
			return status;
		}

		// Trace
		dirFilePrepSingle_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepSingle);
		ALX_LOGGER_TRACE_INF("Created dir '%s' with %lu files in %lu sec", dirPathBuffer, me->numOfFilesPerDir, dirFilePrepSingle_sec);
	}

	// Trace
	dirFilePrepAllTime_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepAll);
	ALX_LOGGER_TRACE_INF("Created %lu dir with %lu files, total %lu files in %lu sec", me->numOfDir, me->numOfFilesPerDir, me->numOfFilesTotal, dirFilePrepAllTime_sec);
	ALX_LOGGER_TRACE_INF("Each file has %lu logs, so total number of logs is %lu", me->numOfLogsPerFile, me->numOfLogsTotal);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}

static Alx_Status AlxLogger_CheckRepairReadFile(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t positionNew = 0;
	uint32_t readLenActual = 0;


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_INF("AlxLogger - Check/Repair current read_file started");
	ALX_LOGGER_TRACE_INF
	(
		"Address START - dir = %lu, file = %lu, log = %lu, pos = %lu, id = %lu",
		me->md.read.dir,
		me->md.read.file,
		me->md.read.log,
		me->md.read.pos,
		(uint32_t)me->md.read.id
	);


	//------------------------------------------------------------------------------
	// Check/Repair
	//------------------------------------------------------------------------------
	// Read pointer could be newer than write (if read metadata was stored after write).
	// In this case they must be synchronized otherwise read.pos could point into a middle of log
	if ((me->md.read.dir == me->md.write.dir) &&
		(me->md.read.file == me->md.write.file) &&
		((me->md.read.pos > me->md.write.pos) ||
		 (me->md.read.log > me->md.write.log) ||
		 (me->md.read.id > me->md.write.id)))
	{
		me->md.read.pos = me->md.write.pos;
		me->md.read.log = me->md.write.log;
		me->md.read.id = me->md.write.id;
		ALX_LOGGER_TRACE_INF("Read metadata was newer than write");
	}

	// Ensure read.id is consistent with read.dir, .file and .log
	uint64_t loggerRotations = (me->md.oldest.id + (me->numOfLogsTotal - 1)) / me->numOfLogsTotal;
	if (loggerRotations != 0)
	{
		if ((me->md.oldest.id % me->numOfLogsTotal) == 0)
		{
			loggerRotations++;
		}
		if (me->md.read.dir >= me->md.oldest.dir)
		{
			// This will happen if write is already wrapped but read is not - read has one fewer logger rotation
			loggerRotations--;
		}
	}
	uint64_t expectedId = me->md.read.dir * me->md.numOfFilesPerDir * me->md.numOfLogsPerFile
		+ me->md.read.file * me->md.numOfLogsPerFile
		+ me->md.read.log;
	expectedId += (loggerRotations * me->numOfLogsTotal);

	if (expectedId != me->md.read.id)
	{
		ALX_LOGGER_TRACE_WRN("Read metadata missmatch, rewinding read file");
		expectedId -= me->md.read.log;
		me->md.read.log = 0;
		me->md.read.pos = 0;
		me->md.read.id = expectedId;
	}

	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_INF
	(
		"Address END - dir = %lu, file = %lu, log = %lu, pos = %lu, id = %lu",
		me->md.read.dir,
		me->md.read.file,
		me->md.read.log,
		me->md.read.pos,
		(uint32_t)me->md.read.id
	);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}

static Alx_Status AlxLogger_SyncLogWriteFile
(
	AlxLogger* me,
	AlxFs_File *file,
	uint32_t maxReadLen,
	char* buffer,
	uint32_t bufferSize
)
{
	uint32_t positionNew = 0;
	uint32_t readLenActual = 0;
	uint32_t offset = 0;
	uint32_t logCount = 0;
	uint32_t lastDelimPos = 0;
	uint32_t lastDelimPosTemp = 0;
	uint32_t logDelimSize = strlen(me->logDelim);

	// File is open when we get here, seek to beginning
	Alx_Status status = AlxFs_File_Seek(me->alxFs, file, 0, AlxFs_File_Seek_Origin_Set, &positionNew);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Sync: initial seek failed (err: %d, offset=%d, positionNew=%u)", status, 0, positionNew);
		return Alx_Err;
	}

	status = Alx_Ok;
	while (true)
	{
		// This would be more elegant with AlxFs_File_ReadStrUntil but much slower
		status = AlxFs_File_Read(me->alxFs, file, buffer, bufferSize, &readLenActual);
		if ((status == Alx_Ok) && (readLenActual != 0))
		{
			for (uint32_t i = 0; i < readLenActual; i++)
			{
				if (!strncmp(&buffer[i], me->logDelim, logDelimSize))
				{
					logCount++;
					lastDelimPosTemp = offset + i + logDelimSize - 1;
					if (lastDelimPosTemp >= maxReadLen)
					{
						logCount--;
						break;
					}
					else
					{
						lastDelimPos = lastDelimPosTemp;
					}
				}
			}
			offset += readLenActual;
			if (offset > maxReadLen)
			{
				break;
			}
		}
		else if ((status == Alx_Ok) && (readLenActual == 0))
		{
			break;
		}
		else
		{
			ALX_LOGGER_TRACE_WRN("Sync: read failed (err: %d, readLenActual: %u)", status, readLenActual);
			break;
		}
	}

	ALX_LOGGER_TRACE_INF("Sync: file read done (numLogs: %u, last delimiter position: %u)", logCount, lastDelimPos);
	if (logCount < me->md.write.log)
	{
		me->md.write.id -= (me->md.write.log - logCount);
	}
	else
	{
		me->md.write.id += (logCount - me->md.write.log);
	}

	me->md.write.pos = (logCount > 0) ? (lastDelimPos + 1) : (0);
	me->md.write.log = logCount;

	status = AlxFs_File_Truncate(me->alxFs, file, me->md.write.pos);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_INF("Sync: failed to truncate file (%u)", status);
	}

	return status;
}

static Alx_Status AlxLogger_CheckRepairWriteFile
(
	AlxLogger* me,
	char* logBuffer,
	uint32_t logBufferSize,
	char* pathBuffer,
	uint32_t pathBufferSize
)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t positionNew = 0;
	uint32_t readLenActual = 0;


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_INF("AlxLogger - Check/Repair current write_file started");
	ALX_LOGGER_TRACE_INF
	(
		"Address START - dir = %lu, file = %lu, log = %lu, pos = %lu, id = %lu",
		me->md.write.dir,
		me->md.write.file,
		me->md.write.log,
		me->md.write.pos,
		(uint32_t)me->md.write.id
	);


	//------------------------------------------------------------------------------
	// Check/Repair
	//------------------------------------------------------------------------------

	// Open
	sprintf(pathBuffer, "/%lu/%lu.csv", me->md.write.dir, me->md.write.file);
	status = AlxFs_File_Open(me->alxFs, &file, pathBuffer, "r+");
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, pathBuffer);
		return status;
	}

	while (true)
	{
		uint32_t fileSize = 0;
		status = AlxFs_File_Size(me->alxFs, &file, &fileSize);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_INF("Failed to get file size (%u)", status);
			break;
		}
		ALX_LOGGER_TRACE_INF("File size: %u, write.pos: %u", fileSize, me->md.write.pos);
		if (fileSize > me->md.write.pos)
		{
			ALX_LOGGER_TRACE_INF("File size is larger than indicated in metadata, synchronizing...");
			status = AlxLogger_SyncLogWriteFile(me, &file, MIN(fileSize, me->md.write.pos), logBuffer, logBufferSize);
			if (status != Alx_Ok)
			{
				break;
			}
		}
		else if (fileSize < me->md.write.pos)
		{
			ALX_LOGGER_TRACE_WRN("File size is smaller than indicated in metadata, synchronizing...");
			status = AlxLogger_SyncLogWriteFile(me, &file, MIN(fileSize, me->md.write.pos), logBuffer, logBufferSize);
			if (status != Alx_Ok)
			{
				break;
			}
		}

		break;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, pathBuffer);
		// TV: TODO - Handle close error
		return status;
	}

	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_INF
	(
		"Address END - dir = %lu, file = %lu, log = %lu, pos = %lu, id = %lu",
		me->md.write.dir,
		me->md.write.file,
		me->md.write.log,
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
	char path[ALX_LOGGER_PATH_LEN_MAX] = "";


	//------------------------------------------------------------------------------
	// Clear
	//------------------------------------------------------------------------------
	for (uint32_t i = 0; i < me->numOfFilesPerDir; i++)
	{
		// Open
		sprintf(path, "/%lu/%lu.csv", me->md.write.dir, i);
		status = AlxFs_File_Open(me->alxFs, &file, path, "w");
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
			return status;
		}

		// Close
		status = AlxFs_File_Close(me->alxFs, &file);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
			// TV: TODO - Handle close error
			return status;
		}
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// Metadata
//------------------------------------------------------------------------------
static Alx_Status AlxLogger_Metadata_Load(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	AlxLogger_Metadata mdTemp = {};
	uint32_t validatedCrc = 0;
	bool isCrcOk = false;


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------
	status = AlxFsSafe_File_Read(me->alxFsSafe, ALX_LOGGER_METADATA_FILE_PATH, &mdTemp, sizeof(mdTemp));
	if
	(
		status != AlxSafe_BothCopyErr_OrigOk_UseOrig &&
		status != AlxSafe_BothCopyOkCrcSame_OrigDontCare_UseCopyA &&
		status != AlxSafe_BothCopyOkCrcDiff_OrigDontCare_UseCopyA &&
		status != AlxSafe_CopyAOkCopyBErr_OrigDontCare_UseCopyA &&
		status != AlxSafe_CopyAErrCopyBOk_OrigDontCare_UseCopyB
	)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, len=%u", status, ALX_LOGGER_METADATA_FILE_PATH, sizeof(mdTemp));
		return status;
	}


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------

	// Check CRC
	isCrcOk = AlxCrc_IsOk(&me->alxCrc, (uint8_t*)&mdTemp, sizeof(mdTemp), &validatedCrc);
	if (isCrcOk != true)
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, isCrcOk=%u, lenWithCrc=%u, validatedCrc=0x%X", ALX_LOGGER_METADATA_FILE_PATH, isCrcOk, sizeof(mdTemp), validatedCrc);
		return Alx_Err;
	}

	// Check magic number
	if (mdTemp.magicNumber != ALX_LOGGER_METADATA_MAGIC_NUMBER)
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, magicNumberActual=0x%X, magicNumberExpected=0x%X", ALX_LOGGER_METADATA_FILE_PATH, mdTemp.magicNumber, ALX_LOGGER_METADATA_MAGIC_NUMBER);
		return Alx_Err;
	}

	// Check version
	if (mdTemp.version != ALX_LOGGER_METADATA_VERSION)
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, versionActual=%u, versionExpected=%u", ALX_LOGGER_METADATA_FILE_PATH, mdTemp.version, ALX_LOGGER_METADATA_VERSION);
		return Alx_Err;
	}

	// Check number of directories
	if (mdTemp.numOfDir != me->numOfDir)
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, numOfDirActual=%u, numOfDirExpected=%u", ALX_LOGGER_METADATA_FILE_PATH, mdTemp.numOfDir, me->numOfDir);
		return Alx_Err;
	}

	// Check number of files per directory
	if (mdTemp.numOfFilesPerDir != me->numOfFilesPerDir)
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, numOfFilesPerDirActual=%u, numOfFilesPerDirExpected=%u", ALX_LOGGER_METADATA_FILE_PATH, mdTemp.numOfFilesPerDir, me->numOfFilesPerDir);
		return Alx_Err;
	}

	// Check number of logs per file
	if (mdTemp.numOfLogsPerFile != me->numOfLogsPerFile)
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, numOfLogsPerFileActual=%u, numOfLogsPerFileExpected=%u", ALX_LOGGER_METADATA_FILE_PATH, mdTemp.numOfLogsPerFile, me->numOfLogsPerFile);
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
static Alx_Status AlxLogger_Metadata_Store_Private(AlxLogger* me, AlxLogger_Metadata_StoreConfig config)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	AlxLogger_Metadata mdTemp = {};
	uint32_t crcLen = 0;
	uint32_t mdTempLenWithoutCrc = 0;


	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------
	mdTemp.magicNumber = ALX_LOGGER_METADATA_MAGIC_NUMBER;
	mdTemp.version = ALX_LOGGER_METADATA_VERSION;

	mdTemp.numOfDir = me->numOfDir;
	mdTemp.numOfFilesPerDir = me->numOfFilesPerDir;
	mdTemp.numOfLogsPerFile = me->numOfLogsPerFile;

	if (config == AlxLogger_Metadata_StoreConfig_Default)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

		mdTemp.read.id			= 0;
		mdTemp.read.pos			= 0;
		mdTemp.read.log			= 0;
		mdTemp.read.file		= 0;
		mdTemp.read.dir			= 0;

		mdTemp.write.id			= 0;
		mdTemp.write.pos		= 0;
		mdTemp.write.log		= 0;
		mdTemp.write.file		= 0;
		mdTemp.write.dir		= 0;

		mdTemp.oldest.id		= 0;
		mdTemp.oldest.pos		= 0;
		mdTemp.oldest.log		= 0;
		mdTemp.oldest.file		= 0;
		mdTemp.oldest.dir		= 0;
	}
	else if (config == AlxLogger_Metadata_StoreConfig_ReadWriteOldest)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

		mdTemp.read.id			= me->md.read.id;
		mdTemp.read.pos			= me->md.read.pos;
		mdTemp.read.log			= me->md.read.log;
		mdTemp.read.file		= me->md.read.file;
		mdTemp.read.dir			= me->md.read.dir;

		mdTemp.write.id			= me->md.write.id;
		mdTemp.write.pos		= me->md.write.pos;
		mdTemp.write.log		= me->md.write.log;
		mdTemp.write.file		= me->md.write.file;
		mdTemp.write.dir		= me->md.write.dir;

		mdTemp.oldest.id		= me->md.oldest.id;
		mdTemp.oldest.pos		= me->md.oldest.pos;
		mdTemp.oldest.log		= me->md.oldest.log;
		mdTemp.oldest.file		= me->md.oldest.file;
		mdTemp.oldest.dir		= me->md.oldest.dir;
	}
	else if (config == AlxLogger_Metadata_StoreConfig_Read)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreWriteMetadata);

		mdTemp.read.id			= me->md.read.id;
		mdTemp.read.pos			= me->md.read.pos;
		mdTemp.read.log			= me->md.read.log;
		mdTemp.read.file		= me->md.read.file;
		mdTemp.read.dir			= me->md.read.dir;

		mdTemp.write.id			= me->mdStored.write.id;
		mdTemp.write.pos		= me->mdStored.write.pos;
		mdTemp.write.log		= me->mdStored.write.log;
		mdTemp.write.file		= me->mdStored.write.file;
		mdTemp.write.dir		= me->mdStored.write.dir;

		mdTemp.oldest.id		= me->mdStored.oldest.id;
		mdTemp.oldest.pos		= me->mdStored.oldest.pos;
		mdTemp.oldest.log		= me->mdStored.oldest.log;
		mdTemp.oldest.file		= me->mdStored.oldest.file;
		mdTemp.oldest.dir		= me->mdStored.oldest.dir;
	}
	else if (config == AlxLogger_Metadata_StoreConfig_Write)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

		mdTemp.read.id			= me->mdStored.read.id;
		mdTemp.read.pos			= me->mdStored.read.pos;
		mdTemp.read.log			= me->mdStored.read.log;
		mdTemp.read.file		= me->mdStored.read.file;
		mdTemp.read.dir			= me->mdStored.read.dir;

		mdTemp.write.id			= me->md.write.id;
		mdTemp.write.pos		= me->md.write.pos;
		mdTemp.write.log		= me->md.write.log;
		mdTemp.write.file		= me->md.write.file;
		mdTemp.write.dir		= me->md.write.dir;

		mdTemp.oldest.id		= me->mdStored.oldest.id;
		mdTemp.oldest.pos		= me->mdStored.oldest.pos;
		mdTemp.oldest.log		= me->mdStored.oldest.log;
		mdTemp.oldest.file		= me->mdStored.oldest.file;
		mdTemp.oldest.dir		= me->mdStored.oldest.dir;
	}
	else if (config == AlxLogger_Metadata_StoreConfig_WriteOldest)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

		mdTemp.read.id			= me->mdStored.read.id;
		mdTemp.read.pos			= me->mdStored.read.pos;
		mdTemp.read.log			= me->mdStored.read.log;
		mdTemp.read.file		= me->mdStored.read.file;
		mdTemp.read.dir			= me->mdStored.read.dir;

		mdTemp.write.id			= me->md.write.id;
		mdTemp.write.pos		= me->md.write.pos;
		mdTemp.write.log		= me->md.write.log;
		mdTemp.write.file		= me->md.write.file;
		mdTemp.write.dir		= me->md.write.dir;

		mdTemp.oldest.id		= me->md.oldest.id;
		mdTemp.oldest.pos		= me->md.oldest.pos;
		mdTemp.oldest.log		= me->md.oldest.log;
		mdTemp.oldest.file		= me->md.oldest.file;
		mdTemp.oldest.dir		= me->md.oldest.dir;
	}
	else
	{
		ALX_LOGGER_ASSERT(false);	// We should never get here
	}

	crcLen = AlxCrc_GetLen(&me->alxCrc);
	mdTempLenWithoutCrc = sizeof(mdTemp) - crcLen;
	mdTemp.crc = AlxCrc_Calc(&me->alxCrc, (uint8_t*)&mdTemp, mdTempLenWithoutCrc);


	//------------------------------------------------------------------------------
	// Store
	//------------------------------------------------------------------------------
	status = AlxFsSafe_File_Write(me->alxFsSafe, ALX_LOGGER_METADATA_FILE_PATH, &mdTemp, sizeof(mdTemp));
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, len=%u", status, ALX_LOGGER_METADATA_FILE_PATH, sizeof(mdTemp));
		return status;
	}


	//------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------
	me->mdStored = mdTemp;

	#if defined(ALX_LOGGER_DEBUG)
	char path[ALX_LOGGER_PATH_LEN_MAX] = "";
	sprintf(path, "/%lu/%lu.csv", me->md.write.dir, me->md.write.file);
	status = AlxFs_File_Open(me->alxFs, &file, path, "r+");
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
		return status;
	}
	uint32_t fileSize = 0;
	AlxFs_File_Size(me->alxFs, &file, &fileSize);
	if (fileSize != me->md.write.pos)
	{
		ALX_LOGGER_TRACE_INF("File size does not equal metadata position (%u, %u)", fileSize, me->md.write.pos);
	}
	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
		// TV: TODO - Handle close error
		return status;
	}
	#endif

	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreReadMetadata);
	if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreWriteMetadata);
	return Alx_Ok;
}


//------------------------------------------------------------------------------
// Status
//------------------------------------------------------------------------------
static bool AlxLogger_Log_AreLogsAvailable(uint64_t idStart, uint64_t idEnd)
{
	// Get
	uint64_t numOfLogs = AlxLogger_Log_GetNumOfLogs(idStart, idEnd);

	// Return
	if (numOfLogs > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
static uint64_t AlxLogger_Log_GetNumOfLogs(uint64_t idStart, uint64_t idEnd)
{
	// Calculate
	ALX_LOGGER_ASSERT(idEnd >= idStart);
	uint64_t numOfLogs = idEnd - idStart;

	// Return
	return numOfLogs;
}
static uint32_t AlxLogger_Log_GetLogsEndPosition(AlxLogger* me, const char* logs, uint32_t numOfLogs)
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
