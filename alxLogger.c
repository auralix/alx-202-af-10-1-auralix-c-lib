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
static bool AlxLogger_IsLogToReadCheck(uint64_t readId, uint64_t writeId);
static uint64_t AlxLogger_GetNumOfLogsToRead_Private(uint64_t readId, uint64_t writeId);
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
	const char* logDelim,
	AlxIoPin* do_DBG_Read,
	AlxIoPin* do_DBG_Write,
	AlxIoPin* do_DBG_StoreReadMetadata,
	AlxIoPin* do_DBG_StoreWriteMetadata
)
{
	// Parameters
	me->alxFs = alxFs;
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
Alx_Status AlxLogger_Init(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;
	uint64_t numOfLogsToReadAvailable = 0;
	char numOfLogsToSendAvailable_str[ALX_LOGGER_RESULT_STR_LEN_MAX] = "";

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
	numOfLogsToReadAvailable = AlxLogger_GetNumOfLogsToRead_Private(me->md.read.id, me->md.write.id);
	AlxGlobal_Ulltoa(numOfLogsToReadAvailable, numOfLogsToSendAvailable_str);
	ALX_LOGGER_TRACE_INF("AlxLogger - Totals");
	ALX_LOGGER_TRACE_INF("- numOfFilesTotal = %lu", me->numOfFilesTotal);
	ALX_LOGGER_TRACE_INF("- numOfLogsTotal = %lu", me->numOfLogsTotal);
	ALX_LOGGER_TRACE_INF("- numOfLogsPerDirTotal = %lu", me->numOfLogsPerDirTotal);
	ALX_LOGGER_TRACE_INF("- numOfLogsToReadAvailable = %s", numOfLogsToSendAvailable_str);

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
Alx_Status AlxLogger_Read(AlxLogger* me, char* logs, uint32_t numOfLogs, uint32_t* numOfLogsActual, bool mdUpdate, uint64_t idStart)
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
	uint32_t readLine = 0;


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
		md.read.line = (idStart % me->numOfLogsPerDirTotal) % me->numOfLogsPerFile;
		md.read.file = (idStart % me->numOfLogsPerDirTotal) / me->numOfLogsPerFile;
		md.read.dir = (idStart / me->numOfLogsPerDirTotal) % me->numOfDir;


		//------------------------------------------------------------------------------
		// Set read.pos via File Reading
		//------------------------------------------------------------------------------
		if (md.read.pos != md.read.line)
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
					ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, readLen=%u, readLenTotal=%u, readLine=%u", status, path, readLen, readLenTotal, readLine);
					Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
					if (statusClose != Alx_Ok)
					{
						ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
						// TV: TODO - Handle close error
					}
					return status;
				}

				// Increment readLine, readLenTotal
				readLine++;
				readLenTotal = readLenTotal + readLen;

				// If read.line reached
				if (readLine == md.read.line)
				{
					// Close
					status = AlxFs_File_Close(me->alxFs, &file);
					if (status != Alx_Ok)
					{
						ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, readLenTotal=%u, readLine=%u", status, path, readLenTotal, readLine);
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
		// Handle First Log & First Line
		//------------------------------------------------------------------------------
		if (logNum == 0)
		{
			// Check if log-to-read available
			if (AlxLogger_IsLogToReadCheck(md.read.id, md.write.id) == false)
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
		else if (md.read.line == 0)
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

		// read.line
		md.read.line++;
		if (md.read.line >= me->numOfLogsPerFile)
		{
			// Reset
			md.read.pos = 0;
			md.read.line = 0;

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
		// Handle Last Log & Check if Log-To-Read Available
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
		else if (AlxLogger_IsLogToReadCheck(md.read.id, md.write.id) == false)
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
Alx_Status AlxLogger_ReadFile(AlxLogger* me, const char* path, uint8_t* chunkBuff, uint32_t chunkLen, Alx_Status(*chunkRead_Callback)(void* chunkData, uint32_t chunkLenActual))
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return AlxFs_File_ReadInChunks(me->alxFs, path, chunkBuff, chunkLen, chunkRead_Callback);;
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
		numOfLogsSpacePerFileRemaining = (int64_t)me->numOfLogsPerFile - (int64_t)me->md.write.line;
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
		writeLen = AlxLogger_GetNumOfLogsEndPosition(me, (void*)logs + writeLenTotal, numOfLogsToWrite);


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

				// oldest.id - Only start handling after inital wrap around
				if (me->md.write.id >= me->numOfLogsTotal)
				{
					// Increment oldest.id by number of logs per directory
					me->md.oldest.id = me->md.oldest.id + me->numOfLogsPerDirTotal;

					// Reset
					me->md.oldest.pos = 0;
					me->md.oldest.line = 0;
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
			status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreReadWriteOldest);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
				return status;
			}
		}
		else if (me->md.write.file == 0)	// If new dir, store write & oldest
		{
			status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreWriteOldest);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, logNum=%u, writeLenTotal=%u, numOfLogs=%u", status, path, logNum, writeLenTotal, numOfLogs);
				return status;
			}
		}
		else if (me->md.write.line == 0)	// If new file, store write only
		{
			status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreWrite);
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
Alx_Status AlxLogger_GetFileSize(AlxLogger* me, const char* path, uint32_t* fileSize)
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
	status = AlxFs_File_Size(me->alxFs, &file, fileSize);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE("Err: %d, path=%s, fileSize=%u", status, path, fileSize);
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
uint64_t AlxLogger_GetNumOfLogsToReadAvailable(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return AlxLogger_GetNumOfLogsToRead_Private(me->md.read.id, me->md.write.id);
}
uint64_t AlxLogger_GetNumOfLogsToReadStored(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return AlxLogger_GetNumOfLogsToRead_Private(me->md.oldest.id, me->md.write.id);
}
Alx_Status AlxLogger_GetIdToReadOldest(AlxLogger* me, uint64_t* idToReadOldest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool isLogToReadStored = AlxLogger_IsLogToReadCheck(me->md.oldest.id, me->md.write.id);
	if (isLogToReadStored == false)
	{
		*idToReadOldest = 0;
		return Alx_Err;
	}

	// Return
	*idToReadOldest = me->md.oldest.id;
	return Alx_Ok;
}
Alx_Status AlxLogger_GetIdToReadNewest(AlxLogger* me, uint64_t* idToReadNewest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool isLogToReadStored = AlxLogger_IsLogToReadCheck(me->md.oldest.id, me->md.write.id);
	if (isLogToReadStored == false)
	{
		*idToReadNewest = 0;
		return Alx_Err;
	}

	// Return
	*idToReadNewest = me->md.write.id - 1;
	return Alx_Ok;
}
Alx_Status AlxLogger_GetFilePathToReadOldest(AlxLogger* me, char* filePathToReadOldest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool isLogToReadStored = AlxLogger_IsLogToReadCheck(me->md.oldest.id, me->md.write.id);
	if (isLogToReadStored == false)
	{
		strcpy(filePathToReadOldest, "");
		return Alx_Err;
	}

	// Prepare
	uint32_t fileToReadOldest = me->md.oldest.file;
	uint32_t dirToReadOldest = me->md.oldest.dir;

	// Return
	sprintf(filePathToReadOldest, "/%lu/%lu.csv", dirToReadOldest, fileToReadOldest);
	return Alx_Ok;
}
Alx_Status AlxLogger_GetFilePathToReadNewest(AlxLogger* me, char* filePathToReadNewest)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Check
	bool isLogToReadStored = AlxLogger_IsLogToReadCheck(me->md.oldest.id, me->md.write.id);
	if (isLogToReadStored == false)
	{
		strcpy(filePathToReadNewest, "");
		return Alx_Err;
	}

	// Prepare
	uint64_t idToReadNewest = me->md.write.id - 1;
	uint32_t fileToReadNewest = (idToReadNewest % me->numOfLogsPerDirTotal) / me->numOfLogsPerFile;
	uint32_t dirToReadNewest = (idToReadNewest / me->numOfLogsPerDirTotal) % me->numOfDir;

	// Return
	sprintf(filePathToReadNewest, "/%lu/%lu.csv", dirToReadNewest, fileToReadNewest);
	return Alx_Ok;
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
Alx_Status AlxLogger_StoreMetadata(AlxLogger* me, AlxLogger_StoreMetadata_Config config)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return AlxLogger_StoreMetadata_Private(me, config);
}
AlxMath_Data AlxLogger_GetMath_Data_ReadTime_ms(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->alxMath_Data_ReadTime_ms;
}
AlxMath_Data AlxLogger_GetMath_Data_WriteTime_ms(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	// isInit -> Don't care

	// Return
	return me->alxMath_Data_WriteTime_ms;
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
	char str[ALX_LOGGER_RESULT_STR_LEN_MAX] = "";


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

		// Load metadata
		status = AlxLogger_LoadMetadata(me);
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

		AlxGlobal_Ulltoa(me->md.read.id, str);
		ALX_LOGGER_TRACE_INF("- read.id = %s", str);
		ALX_LOGGER_TRACE_INF("- read.pos = %lu", me->md.read.pos);
		ALX_LOGGER_TRACE_INF("- read.line = %lu", me->md.read.line);
		ALX_LOGGER_TRACE_INF("- read.file = %lu", me->md.read.file);
		ALX_LOGGER_TRACE_INF("- read.dir = %lu", me->md.read.dir);

		AlxGlobal_Ulltoa(me->md.write.id, str);
		ALX_LOGGER_TRACE_INF("- write.id = %s", str);
		ALX_LOGGER_TRACE_INF("- write.pos = %lu", me->md.write.pos);
		ALX_LOGGER_TRACE_INF("- write.line = %lu", me->md.write.line);
		ALX_LOGGER_TRACE_INF("- write.file = %lu", me->md.write.file);
		ALX_LOGGER_TRACE_INF("- write.dir = %lu", me->md.write.dir);

		AlxGlobal_Ulltoa(me->md.oldest.id, str);
		ALX_LOGGER_TRACE_INF("- oldest.id = %s", str);
		ALX_LOGGER_TRACE_INF("- oldest.pos = %lu", me->md.oldest.pos);
		ALX_LOGGER_TRACE_INF("- oldest.line = %lu", me->md.oldest.line);
		ALX_LOGGER_TRACE_INF("- oldest.file = %lu", me->md.oldest.file);
		ALX_LOGGER_TRACE_INF("- oldest.dir = %lu", me->md.oldest.dir);

		ALX_LOGGER_TRACE_INF("- crc = 0x%04X", me->md.crc);

		// Repair write file
		status = AlxLogger_CheckRepairWriteFile(me);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			return status;
		}

		// Store metadata
		status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreReadWriteOldest);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d", status);
			return status;
		}

		// Trace
		ALX_LOGGER_TRACE_INF("AlxLogger - Store metadata after Check/Repair current write_file OK");
		ALX_LOGGER_TRACE_INF("- magicNumber = 0x%08lX", me->md.magicNumber);
		ALX_LOGGER_TRACE_INF("- version = %lu", me->md.version);

		ALX_LOGGER_TRACE_INF("- numOfDir = %lu", me->md.numOfDir);
		ALX_LOGGER_TRACE_INF("- numOfFilesPerDir = %lu", me->md.numOfFilesPerDir);
		ALX_LOGGER_TRACE_INF("- numOfLogsPerFile = %lu", me->md.numOfLogsPerFile);

		AlxGlobal_Ulltoa(me->md.read.id, str);
		ALX_LOGGER_TRACE_INF("- read.id = %s", str);
		ALX_LOGGER_TRACE_INF("- read.pos = %lu", me->md.read.pos);
		ALX_LOGGER_TRACE_INF("- read.line = %lu", me->md.read.line);
		ALX_LOGGER_TRACE_INF("- read.file = %lu", me->md.read.file);
		ALX_LOGGER_TRACE_INF("- read.dir = %lu", me->md.read.dir);

		AlxGlobal_Ulltoa(me->md.write.id, str);
		ALX_LOGGER_TRACE_INF("- write.id = %s", str);
		ALX_LOGGER_TRACE_INF("- write.pos = %lu", me->md.write.pos);
		ALX_LOGGER_TRACE_INF("- write.line = %lu", me->md.write.line);
		ALX_LOGGER_TRACE_INF("- write.file = %lu", me->md.write.file);
		ALX_LOGGER_TRACE_INF("- write.dir = %lu", me->md.write.dir);

		AlxGlobal_Ulltoa(me->md.oldest.id, str);
		ALX_LOGGER_TRACE_INF("- oldest.id = %s", str);
		ALX_LOGGER_TRACE_INF("- oldest.pos = %lu", me->md.oldest.pos);
		ALX_LOGGER_TRACE_INF("- oldest.line = %lu", me->md.oldest.line);
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
	status = AlxLogger_CreateDirAndFiles(me);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Store default metadata
	status = AlxLogger_StoreMetadata_Private(me, AlxLogger_StoreMetadata_Config_StoreDefault);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d", status);
		return status;
	}

	// Set metadata current
	me->md = me->mdStored;

	// Trace
	ALX_LOGGER_TRACE_INF("AlxLogger - Store default metadata OK");

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
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, ALX_LOGGER_METADATA_FILE_PATH);
		return status;
	}

	// Read
	status = AlxFs_File_Read(me->alxFs, &file, &mdTemp, sizeof(mdTemp), &lenActual);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, len=%u, lenActual=%u", status, ALX_LOGGER_METADATA_FILE_PATH, sizeof(mdTemp), lenActual);
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, ALX_LOGGER_METADATA_FILE_PATH);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, ALX_LOGGER_METADATA_FILE_PATH);
		// TV: TODO - Handle close error
		return status;
	}


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------

	// Check length
	if (lenActual != sizeof(mdTemp))
	{
		ALX_LOGGER_TRACE_WRN("Err: path=%s, lenActual=%u, lenExpected=%u", ALX_LOGGER_METADATA_FILE_PATH, lenActual, sizeof(mdTemp));
		return Alx_Err;
	}

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
static Alx_Status AlxLogger_StoreMetadata_Private(AlxLogger* me, AlxLogger_StoreMetadata_Config config)
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

	if (config == AlxLogger_StoreMetadata_Config_StoreDefault)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

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

		mdTemp.oldest.id		= 0;
		mdTemp.oldest.pos		= 0;
		mdTemp.oldest.line		= 0;
		mdTemp.oldest.file		= 0;
		mdTemp.oldest.dir		= 0;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreReadWriteOldest)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

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

		mdTemp.oldest.id		= me->md.oldest.id;
		mdTemp.oldest.pos		= me->md.oldest.pos;
		mdTemp.oldest.line		= me->md.oldest.line;
		mdTemp.oldest.file		= me->md.oldest.file;
		mdTemp.oldest.dir		= me->md.oldest.dir;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreRead)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreWriteMetadata);

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

		mdTemp.oldest.id		= me->mdStored.oldest.id;
		mdTemp.oldest.pos		= me->mdStored.oldest.pos;
		mdTemp.oldest.line		= me->mdStored.oldest.line;
		mdTemp.oldest.file		= me->mdStored.oldest.file;
		mdTemp.oldest.dir		= me->mdStored.oldest.dir;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreWrite)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

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

		mdTemp.oldest.id		= me->mdStored.oldest.id;
		mdTemp.oldest.pos		= me->mdStored.oldest.pos;
		mdTemp.oldest.line		= me->mdStored.oldest.line;
		mdTemp.oldest.file		= me->mdStored.oldest.file;
		mdTemp.oldest.dir		= me->mdStored.oldest.dir;
	}
	else if (config == AlxLogger_StoreMetadata_Config_StoreWriteOldest)
	{
		if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreReadMetadata);
		if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Set(me->do_DBG_StoreWriteMetadata);

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

		mdTemp.oldest.id		= me->md.oldest.id;
		mdTemp.oldest.pos		= me->md.oldest.pos;
		mdTemp.oldest.line		= me->md.oldest.line;
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

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, ALX_LOGGER_METADATA_FILE_PATH, "w");
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, ALX_LOGGER_METADATA_FILE_PATH);
		return status;
	}

	// Write
	status = AlxFs_File_Write(me->alxFs, &file, &mdTemp, sizeof(mdTemp));
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, len=%u", status, ALX_LOGGER_METADATA_FILE_PATH, sizeof(mdTemp));
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, ALX_LOGGER_METADATA_FILE_PATH);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, ALX_LOGGER_METADATA_FILE_PATH);
		// TV: TODO - Handle close error
		return status;
	}


	//------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------
	me->mdStored = mdTemp;


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	if(me->do_DBG_StoreReadMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreReadMetadata);
	if(me->do_DBG_StoreWriteMetadata != NULL) AlxIoPin_Reset(me->do_DBG_StoreWriteMetadata);
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
		sprintf(dirPath, "/%lu", me->numOfDirCreated);
		status = AlxFs_Dir_Make(me->alxFs, dirPath);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", status, dirPath);
			return status;
		}

		// Open dir
		status = AlxFs_Dir_Open(me->alxFs, &dir, dirPath);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", status, dirPath);
			return status;
		}

		// Create files
		for (me->numOfFilesPerDirCreated = 0; me->numOfFilesPerDirCreated < me->numOfFilesPerDir; me->numOfFilesPerDirCreated++)
		{
			// Open file
			sprintf(filePath, "%s/%lu.csv", dirPath, me->numOfFilesPerDirCreated);
			status = AlxFs_File_Open(me->alxFs, &file, filePath, "w");
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, filePath=%s", status, filePath);
				Alx_Status statusClose = AlxFs_Dir_Close(me->alxFs, &dir);
				if (statusClose != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", statusClose, dirPath);
					// TV: TODO - Handle close error
				}
				return status;
			}

			// Close file
			status = AlxFs_File_Close(me->alxFs, &file);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, filePath=%s", status, filePath);
				Alx_Status statusClose = AlxFs_Dir_Close(me->alxFs, &dir);
				if (statusClose != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", statusClose, dirPath);
					// TV: TODO - Handle close error
				}
				return status;
			}
		}

		// Close dir
		status = AlxFs_Dir_Close(me->alxFs, &dir);
		if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, dirPath=%s", status, dirPath);
			// TV: TODO - Handle close error
			return status;
		}

		// Trace
		dirFilePrepSingle_sec = AlxTimSw_Get_sec(&alxTimSw_DirFilePrepSingle);
		ALX_LOGGER_TRACE_INF("Created dir '%s' with %lu files in %lu sec", dirPath, me->numOfFilesPerDir, dirFilePrepSingle_sec);
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
static Alx_Status AlxLogger_CheckRepairWriteFile(AlxLogger* me)
{
	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	char path[ALX_LOGGER_PATH_LEN_MAX] = "";
	char log[ALX_LOGGER_LOG_LEN_MAX] = "";
	uint32_t positionNew = 0;
	uint32_t readLenActual = 0;


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_INF("AlxLogger - Check/Repair current write_file started");
	ALX_LOGGER_TRACE_INF
	(
		"Address START - dir = %lu, file = %lu, line = %lu, pos = %lu, id = %lu",
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
	sprintf(path, "/%lu/%lu.csv", me->md.write.dir, me->md.write.file);
	status = AlxFs_File_Open(me->alxFs, &file, path, "r+");
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
		return status;
	}

	// Seek
	status = AlxFs_File_Seek(me->alxFs, &file, me->md.write.pos, AlxFs_File_Seek_Origin_Set, &positionNew);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, offset=%d, positionNew=%u", status, path, me->md.write.pos, positionNew);
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
			// TV: TODO - Handle close error
		}
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
			ALX_LOGGER_TRACE_INF("Reached end of file, all logs are OK");

			// Break
			break;
		}
		else if (status == AlxFs_ErrNoDelim)
		{
			// Truncate, with this we eliminate last corrupted log
			status = AlxFs_File_Truncate(me->alxFs, &file, me->md.write.pos);
			if (status != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s, size=%u", status, path, me->md.write.pos);
				Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
				if (statusClose != Alx_Ok)
				{
					ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
					// TV: TODO - Handle close error
				}
				return status;
			}

			// Trace
			ALX_LOGGER_TRACE_INF("Found and eliminated last corrupted log - %s", log);

			// Break
			break;
		}
		else if (status != Alx_Ok)
		{
			ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
			Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
			if (statusClose != Alx_Ok)
			{
				ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", statusClose, path);
				// TV: TODO - Handle close error
			}
			return status;
		}

		// Trace
		if (me->md.write.line % 1000 == 0)
		{
			ALX_LOGGER_TRACE_FORMAT("[%lu] %s", me->md.write.line, log);
		}

		// Increment addr
		me->md.write.id++;
		me->md.write.pos = me->md.write.pos + readLenActual;
		me->md.write.line++;
	}


	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_LOGGER_TRACE_WRN("Err: %d, path=%s", status, path);
		// TV: TODO - Handle close error
		return status;
	}


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_LOGGER_TRACE_INF
	(
		"Address END - dir = %lu, file = %lu, line = %lu, pos = %lu, id = %lu",
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
static bool AlxLogger_IsLogToReadCheck(uint64_t readId, uint64_t writeId)
{
	// Get
	uint64_t numOfLogsToRead = AlxLogger_GetNumOfLogsToRead_Private(readId, writeId);

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
static uint64_t AlxLogger_GetNumOfLogsToRead_Private(uint64_t readId, uint64_t writeId)
{
	// Calculate
	ALX_LOGGER_ASSERT(writeId >= readId);
	uint64_t numOfLogsToRead = writeId - readId;

	// Return
	return numOfLogsToRead;
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
