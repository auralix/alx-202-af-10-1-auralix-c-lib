/**
  ******************************************************************************
  * @file		alxFs.c
  * @brief		Auralix C Library - ALX File System Module
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
#include "alxFs.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Flash - Internal
//------------------------------------------------------------------------------
#if defined(ALX_LFS)
static void AlxFs_Lfs_FlashInt_Ctor(AlxFs* me);
static int AlxFs_Lfs_FlashInt_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_FlashInt_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_FlashInt_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_FlashInt_SyncBlock(const struct lfs_config* c);
static int AlxFs_Lfs_FlashInt_Lock(const struct lfs_config* c);
static int AlxFs_Lfs_FlashInt_Unlock(const struct lfs_config* c);
#endif


//------------------------------------------------------------------------------
// MMC
//------------------------------------------------------------------------------
#if defined(ALX_LFS) && defined(ALX_STM32L4)
static void AlxFs_Lfs_Mmc_Ctor(AlxFs* me);
static int AlxFs_Lfs_Mmc_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_Mmc_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_Mmc_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_Mmc_SyncBlock(const struct lfs_config* c);
static int AlxFs_Lfs_Mmc_Lock(const struct lfs_config* c);
static int AlxFs_Lfs_Mmc_Unlock(const struct lfs_config* c);
#endif


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFs_Ctor
(
	AlxFs* me,
	AlxFs_Config config,
	AlxMmc* alxMmc
)
{
	// Parameters
	me->config = config;
	me->alxMmc = alxMmc;

	// Variables
	if (me->config == AlxFs_Config_Undefined)
	{
	}
	#if defined(ALX_LFS)
	else if (me->config == AlxFs_Config_Lfs_FlashInt)
	{
		AlxFs_Lfs_FlashInt_Ctor(me);
	}
	#endif
	#if defined(ALX_LFS) && defined(ALX_STM32L4)
	else if	(me->config == AlxFs_Config_Lfs_Mmc)
	{
		AlxFs_Lfs_Mmc_Ctor(me);
	}
	#endif
	else
	{
		ALX_FS_ASSERT(false);	// We should never get here
	}

	// Info
	me->wasCtorCalled = true;
	me->isMounted = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFs_Mount(AlxFs* me)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	// isMounted -> Don't care

	// Do
	#if defined(ALX_LFS)
	int status = lfs_mount(&me->lfs, &me->lfsConfig);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Set isMounted
	me->isMounted = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_UnMount(AlxFs* me)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	// isMounted -> Don't care

	// Do
	#if defined(ALX_LFS)
	int status = lfs_unmount(&me->lfs);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Clear isMounted
	me->isMounted = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_MountFormat(AlxFs* me)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	// isMounted -> Don't care

	// Local variables
	Alx_Status status = Alx_Err;

	// Try to mount, if error, then format & mount
	status = AlxFs_Mount(me);
	if (status != Alx_Ok)
	{
		// Format
		status = AlxFs_Format(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Mount
		status = AlxFs_Mount(me);
		if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Format(AlxFs* me)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	// isMounted -> Don't care

	// Do
	#if defined(ALX_LFS)
	int status = lfs_format(&me->lfs, &me->lfsConfig);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Remove(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_remove(&me->lfs, path);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Rename(AlxFs* me, const char* pathOld, const char* pathNew)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_rename(&me->lfs, pathOld, pathNew);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Open(AlxFs* me, AlxFs_File* file, const char* path, const char* mode)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Prepare modeConfig
	int modeConfig = 0;
	if (0 == strcmp(mode, "r"))
	{
		#if defined(ALX_LFS)
		modeConfig = LFS_O_RDONLY;
		#endif
	}
	else if (0 == strcmp(mode, "w"))
	{
		#if defined(ALX_LFS)
		modeConfig = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
		#endif
	}
	else if (0 == strcmp(mode, "a"))
	{
		#if defined(ALX_LFS)
		modeConfig = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
		#endif
	}
	else if (0 == strcmp(mode, "r+"))
	{
		#if defined(ALX_LFS)
		modeConfig = LFS_O_RDWR;
		#endif
	}
	else if (0 == strcmp(mode, "w+"))
	{
		#if defined(ALX_LFS)
		modeConfig = LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC;
		#endif
	}
	else if (0 == strcmp(mode, "a+"))
	{
		#if defined(ALX_LFS)
		modeConfig = LFS_O_RDWR | LFS_O_CREAT | LFS_O_APPEND;
		#endif
	}
	else
	{
		ALX_FS_ASSERT(false);	// We should never get here
	}

	// Do
	#if defined(ALX_LFS)
	int status = lfs_file_open(&me->lfs, &file->lfsFile, path, modeConfig);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Close(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_file_close(&me->lfs, &file->lfsFile);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Read(AlxFs* me, AlxFs_File* file, void* data, uint32_t len, uint32_t* lenActual)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	lfs_ssize_t statusLenActual = lfs_file_read(&me->lfs, &file->lfsFile, data, (lfs_size_t)len);
	if (statusLenActual < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }
	*lenActual = (uint32_t)statusLenActual;
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_ReadStrUntil(AlxFs* me, AlxFs_File* file, char* str, const char* delim, uint32_t len, uint32_t* lenActual)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);
	ALX_FS_ASSERT(strlen(delim) == 1);	// Only single char delim supported

	// Local variables
	Alx_Status status = AlxFs_ErrNoDelim;
	uint32_t i = 0;

	// Loop
	for (i = 0; i < (len - 1); i++)
	{
		// Local variables
		char ch = 0;
		uint32_t readLenActual = 0;

		// Read char
		Alx_Status statusRead = AlxFs_File_Read(me, file, &ch, 1, &readLenActual);
		if ((statusRead == Alx_Ok) && (readLenActual == 0))
		{
			// Break, we reached end of file, status already AlxFs_ErrNoDelim
			break;
		}
		if (statusRead != Alx_Ok)
		{
			// Break, error occured, trace, change status to Alx_Err
			ALX_FS_TRACE("Err");
			status = Alx_Err;
			break;
		}
		else if (readLenActual != 1)
		{
			ALX_FS_ASSERT(false);		// We should never get here
		}

		// Store char
		str[i] = ch;

		// Check if char is delim
		if (ch == *delim)
		{
			// Break, we found delimiter, increment i for correct null-termination, change status to Alx_Ok
			i++;
			status = Alx_Ok;
			break;
		}
	}

	// Null-terminate string
	str[i] = '\0';

	// Return
	*lenActual = i;
	return status;
}
Alx_Status AlxFs_File_Write(AlxFs* me, AlxFs_File* file, void* data, uint32_t len)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	lfs_ssize_t statusLenActual = lfs_file_write(&me->lfs, &file->lfsFile, data, (lfs_size_t)len);
	if (statusLenActual != (lfs_ssize_t)len) { ALX_FS_TRACE("Err"); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_WriteStr(AlxFs* me, AlxFs_File* file, const char* str)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Return
	return AlxFs_File_Write(me, file, (void*)str, strlen(str));
}
Alx_Status AlxFs_File_Sync(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_file_sync(&me->lfs, &file->lfsFile);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Seek(AlxFs* me, AlxFs_File* file, int32_t offset, AlxFs_File_Seek_Origin origin, uint32_t* positionNew)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Prepare originConfig
	int originConfig = 0;
	if (origin == AlxFs_File_Seek_Origin_Set)
	{
		#if defined(ALX_LFS)
		originConfig = LFS_SEEK_SET;
		#endif
	}
	else if(origin == AlxFs_File_Seek_Origin_Cur)
	{
		#if defined(ALX_LFS)
		originConfig = LFS_SEEK_CUR;
		#endif
	}
	else if(origin == AlxFs_File_Seek_Origin_End)
	{
		#if defined(ALX_LFS)
		originConfig = LFS_SEEK_END;
		#endif
	}
	else
	{
		ALX_FS_ASSERT(false);	// We should never get here
	}

	// Do
	#if defined(ALX_LFS)
	lfs_soff_t statusPositionNew = lfs_file_seek(&me->lfs, &file->lfsFile, (lfs_soff_t)offset, originConfig);
	if (statusPositionNew < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }
	*positionNew = (uint32_t)statusPositionNew;
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Tell(AlxFs* me, AlxFs_File* file, uint32_t* position)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	lfs_soff_t statusPosition = lfs_file_tell(&me->lfs, &file->lfsFile);
	if (statusPosition < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }
	*position = (uint32_t)statusPosition;
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Rewind(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_file_rewind(&me->lfs, &file->lfsFile);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Size(AlxFs* me, AlxFs_File* file, uint32_t* size)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	lfs_soff_t statusSize = lfs_file_size(&me->lfs, &file->lfsFile);
	if (statusSize < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }
	*size = (uint32_t)statusSize;
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Truncate(AlxFs* me, AlxFs_File* file, uint32_t size)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_file_truncate(&me->lfs, &file->lfsFile, (lfs_off_t)size);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Trace(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t fileSize = 0;
	uint32_t fileSizeTraced = 0;
	uint8_t buff[ALX_FS_BUFF_LEN] = {};
	uint32_t lenActual = 0;

	// Open
	status = AlxFs_File_Open(me, &file, path, "r");
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Get fileSize
	status = AlxFs_File_Size(me, &file, &fileSize);
	if (status != Alx_Ok)
	{
		AlxFs_File_Close(me, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Loop
	while (true)
	{
		// Read
		memset(buff, 0, sizeof(buff));
		status = AlxFs_File_Read(me, &file, buff, sizeof(buff)-1, &lenActual);	// Subtract -1 for string null-terminator
		if (status != Alx_Ok)
		{
			AlxFs_File_Close(me, &file);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}

		// Trace
		ALX_FS_TRACE_FORMAT("%s", buff);

		// Increment fileSizeTraced
		fileSizeTraced = fileSizeTraced + lenActual;

		// If done, break
		if (fileSizeTraced >= fileSize)
		{
			break;
		}
	}

	// Close
	status = AlxFs_File_Close(me, &file);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Make(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_mkdir(&me->lfs, path);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Open(AlxFs* me, AlxFs_Dir* dir, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_dir_open(&me->lfs, &dir->lfsDir, path);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Close(AlxFs* me, AlxFs_Dir* dir)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_dir_close(&me->lfs, &dir->lfsDir);
	if (status != 0) { ALX_FS_TRACE("Err: %d", status); return Alx_Err; }
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Read(AlxFs* me, AlxFs_Dir* dir, AlxFs_Info* info)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_LFS)
	int status = lfs_dir_read(&me->lfs, &dir->lfsDir, &info->lfsInfo);
	if (status == 0)
	{
		return AlxFs_EndOfDir;
	}
	else if (status != 1)
	{
		ALX_FS_TRACE("Err: %d", status);
		return Alx_Err;
	}
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Trace(AlxFs* me, const char* path, bool fileTrace)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_Dir dir = {};
	AlxFs_Info info = {};
	char buff[ALX_FS_BUFF_LEN] = {};

	// Trace
	ALX_FS_TRACE_FORMAT("AlxFs - Dir '%s' Trace\r\n", path);

	// Open
	status = AlxFs_Dir_Open(me, &dir, path);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Loop
	#if defined(ALX_LFS)
	while (true)
	{
		// Read
		status = AlxFs_Dir_Read(me, &dir, &info);
		if (status == AlxFs_EndOfDir)
		{
			break;
		}
		else if (status != Alx_Ok)
		{
			AlxFs_Dir_Close(me, &dir);	// Will not handle return
			ALX_FS_TRACE("Err");
			return status;
		}

		// Trace
		if (info.lfsInfo.type == LFS_TYPE_REG)
		{
			// Prepare
			sprintf(buff, "FILE - %s - %lu B\r\n", info.lfsInfo.name, info.lfsInfo.size);

			// Trace
			ALX_FS_TRACE_FORMAT("%s", buff);

			// If fileTrace enabled
			if (fileTrace)
			{
				// Prepare
				sprintf(buff, "%s/%s", path, info.lfsInfo.name);

				// Trace
				status = AlxFs_File_Trace(me, buff);
				if (status != Alx_Ok)
				{
					AlxFs_Dir_Close(me, &dir);	// Will not handle return
					ALX_FS_TRACE("Err");
					return status;
				}
			}
		}
		else if (info.lfsInfo.type == LFS_TYPE_DIR)
		{
			// Prepare
			sprintf(buff, "DIR - %s\r\n", info.lfsInfo.name);

			// Trace
			ALX_FS_TRACE_FORMAT("%s", buff);
		}
		else
		{
			ALX_FS_ASSERT(false);	// We should never get here
		}
	}
	#endif

	// Close
	status = AlxFs_Dir_Close(me, &dir);
	if (status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Trace
	ALX_FS_TRACE_FORMAT("\r\n");

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Flash - Internal
//------------------------------------------------------------------------------
#if defined(ALX_LFS)
static void AlxFs_Lfs_FlashInt_Ctor(AlxFs* me)
{
	memset(&me->lfs, 0, sizeof(me->lfs));

	me->lfsConfig.context = me;

	me->lfsConfig.read  = AlxFs_Lfs_FlashInt_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_FlashInt_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_FlashInt_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_FlashInt_SyncBlock;
	#if defined(LFS_THREADSAFE)
	me->lfsConfig.lock = AlxFs_Lfs_FlashInt_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_FlashInt_Unlock;
	#endif

	#if defined(ALX_STM32F4)
	me->lfsAddr = 0x08100000;
	me->lfsConfig.read_size = 1;
	me->lfsConfig.prog_size = 4;
	me->lfsConfig.block_size = 16 * 1024;
	me->lfsConfig.block_count = 4;
	me->lfsConfig.block_cycles = -1;	// -1 means wear-leveling disabled
	me->lfsConfig.cache_size = 16;
	me->lfsConfig.lookahead_size = 8;
	#endif

	#if defined(ALX_STM32F7)
	me->lfsAddr = 0x08180000;
	me->lfsConfig.read_size = 1;
	me->lfsConfig.prog_size = 4;
	me->lfsConfig.block_size = 256 * 1024;
	me->lfsConfig.block_count = 2;
	me->lfsConfig.block_cycles = -1;	// -1 means wear-leveling disabled
	me->lfsConfig.cache_size = 16;
	me->lfsConfig.lookahead_size = 8;
	#endif

	#if defined(ALX_STM32L4)
	me->lfsAddr = 0x08100000;
	me->lfsConfig.read_size = 1;
	me->lfsConfig.prog_size = 8;
	me->lfsConfig.block_size = 4 * 1024;
	me->lfsConfig.block_count = 32;
	me->lfsConfig.block_cycles = -1;	// -1 means wear-leveling disabled
	me->lfsConfig.cache_size = 16;
	me->lfsConfig.lookahead_size = 8;
	#endif
}
static int AlxFs_Lfs_FlashInt_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	// Unlock FLASH
	HAL_FLASH_Unlock();

	// Clear all FLASH flags
	#if defined(ALX_STM32F4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_BSY
	);
	#endif
	#if defined(ALX_STM32F7)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_ERSERR |
		FLASH_FLAG_BSY
	);
	#endif
	#if defined(ALX_STM32L4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_PROGERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_SIZERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_MISERR |
		FLASH_FLAG_FASTERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_OPTVERR |
		FLASH_FLAG_BSY |
		FLASH_FLAG_ECCC |
		FLASH_FLAG_ECCD
	);
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
	}
	#endif

	// Set addrSrc
	AlxFs* alxFs_me = (AlxFs*)c->context;
	uint32_t src_address = alxFs_me->lfsAddr + block * c->block_size + off;

	// Read
	memcpy(buffer, (void*)src_address, size);

	// Lock FLASH
	HAL_FLASH_Lock();

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_FlashInt_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	// Unlock FLASH
	HAL_FLASH_Unlock();

	// Clear all FLASH flags
	#if defined(ALX_STM32F4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_BSY
	);
	#endif
	#if defined(ALX_STM32F7)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_ERSERR |
		FLASH_FLAG_BSY
	);
	#endif
	#if defined(ALX_STM32L4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_PROGERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_SIZERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_MISERR |
		FLASH_FLAG_FASTERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_OPTVERR |
		FLASH_FLAG_BSY |
		FLASH_FLAG_ECCC |
		FLASH_FLAG_ECCD
	);
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
	}
	#endif

	// Loop
	uint32_t n_rows = size / c->prog_size;
	for (uint32_t i_row = 0; i_row < n_rows; i_row++)
	{
		#if defined(ALX_STM32F4) || defined(ALX_STM32F7)
		// Local variables
		AlxFs* alxFs_me = (AlxFs*)c->context;
		uint32_t block_base_addr = alxFs_me->lfsAddr + block * c->block_size;
		uint32_t dest_address = block_base_addr + off + i_row * 4;		// Multiply by 4 because we write 4 bytes at the time
		uint32_t* src_address_ptr = (uint32_t*)(buffer + i_row * 4);	// Multiply by 4 because we write 4 bytes at the time

		// Program
		HAL_StatusTypeDef xHAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dest_address, *src_address_ptr);
		if (xHAL_Status != HAL_OK)
		{
			// Lock FLASH
			HAL_FLASH_Lock();

			// Return
			return LFS_ERR_IO;
		}
		#endif
		#if defined(ALX_STM32L4)
		// Local variables
		AlxFs* alxFs_me = (AlxFs*)c->context;
		uint32_t block_base_addr = alxFs_me->lfsAddr + block * c->block_size;
		uint32_t dest_address = block_base_addr + off + i_row * 8;		// Multiply by 8 because we write 8 bytes at the time
		uint64_t* src_address_ptr = (uint64_t*)(buffer + i_row * 8);	// Multiply by 8 because we write 8 bytes at the time

		// Program
		HAL_StatusTypeDef xHAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, dest_address, *src_address_ptr);
		if (xHAL_Status != HAL_OK)
		{
			// Lock FLASH
			HAL_FLASH_Lock();

			// Return
			return LFS_ERR_IO;
		}
		#endif
	}

	// Lock FLASH
	HAL_FLASH_Lock();

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_FlashInt_EraseBlock(const struct lfs_config* c, lfs_block_t block)
{
	// Unlock FLASH
	HAL_FLASH_Unlock();

	// Clear all FLASH flags
	#if defined(ALX_STM32F4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_BSY
	);
	#endif
	#if defined(ALX_STM32F7)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_ERSERR |
		FLASH_FLAG_BSY
	);
	#endif
	#if defined(ALX_STM32L4)
	__HAL_FLASH_CLEAR_FLAG
	(
		FLASH_FLAG_EOP |
		FLASH_FLAG_OPERR |
		FLASH_FLAG_PROGERR |
		FLASH_FLAG_WRPERR |
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_SIZERR |
		FLASH_FLAG_PGSERR |
		FLASH_FLAG_MISERR |
		FLASH_FLAG_FASTERR |
		FLASH_FLAG_RDERR |
		FLASH_FLAG_OPTVERR |
		FLASH_FLAG_BSY |
		FLASH_FLAG_ECCC |
		FLASH_FLAG_ECCD
	);
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
	{
		__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
	}
	#endif

	// Erase
	#if defined(ALX_STM32F4)
	uint32_t ulPageError = 0;
	FLASH_EraseInitTypeDef xErase_Config = {};
	xErase_Config.TypeErase = FLASH_TYPEERASE_SECTORS;
	xErase_Config.Banks = ALX_NULL;
	xErase_Config.Sector = FLASH_SECTOR_12 + block;
	xErase_Config.NbSectors = 1;
	xErase_Config.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	HAL_StatusTypeDef xHAL_Status = HAL_FLASHEx_Erase(&xErase_Config, &ulPageError);
	#endif
	#if defined(ALX_STM32F7)
	uint32_t ulPageError = 0;
	FLASH_EraseInitTypeDef xErase_Config = {};
	xErase_Config.TypeErase = FLASH_TYPEERASE_SECTORS;
	xErase_Config.Banks = ALX_NULL;
	xErase_Config.Sector = FLASH_SECTOR_10 + block;
	xErase_Config.NbSectors = 1;
	xErase_Config.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	HAL_StatusTypeDef xHAL_Status = HAL_FLASHEx_Erase(&xErase_Config, &ulPageError);
	#endif
	#if defined(ALX_STM32L4)
	uint32_t ulPageError = 0;
	FLASH_EraseInitTypeDef xErase_Config = {};
	xErase_Config.TypeErase = FLASH_TYPEERASE_PAGES;
	xErase_Config.Banks = FLASH_BANK_2;
	xErase_Config.Page = block;
	xErase_Config.NbPages = 1;
	HAL_StatusTypeDef xHAL_Status = HAL_FLASHEx_Erase(&xErase_Config, &ulPageError);
	#endif

	// Lock FLASH
	HAL_FLASH_Lock();

	// Return
	return xHAL_Status == HAL_OK ? LFS_ERR_OK : LFS_ERR_IO;
}
static int AlxFs_Lfs_FlashInt_SyncBlock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return LFS_ERR_OK;
}
#if defined(LFS_THREADSAFE)
static int AlxFs_Lfs_FlashInt_Lock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_FlashInt_Unlock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return LFS_ERR_OK;
}
#endif
#endif


//------------------------------------------------------------------------------
// MMC
//------------------------------------------------------------------------------
#if defined(ALX_LFS) && defined(ALX_STM32L4)
static void AlxFs_Lfs_Mmc_Ctor(AlxFs* me)
{
	memset(&me->lfs, 0, sizeof(me->lfs));

	me->lfsConfig.context = me;

	me->lfsConfig.read  = AlxFs_Lfs_Mmc_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_Mmc_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_Mmc_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_Mmc_SyncBlock;
	#if defined(LFS_THREADSAFE)
	me->lfsConfig.lock = AlxFs_Lfs_Mmc_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_Mmc_Unlock;
	#endif

	#define ALX_FS_MULTIPLY 8
	me->lfsConfig.read_size = 512*ALX_FS_MULTIPLY;
	me->lfsConfig.prog_size = 512*ALX_FS_MULTIPLY;
	me->lfsConfig.block_size = 512*ALX_FS_MULTIPLY;
	me->lfsConfig.block_count = 62160896;	// 62160896 * 512 bytes = ~32GB
	me->lfsConfig.block_cycles = -1;	// -1 means wear-leveling disabled
	me->lfsConfig.cache_size = 512*ALX_FS_MULTIPLY;
	me->lfsConfig.lookahead_size = 32;
}
static int AlxFs_Lfs_Mmc_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	// Local variables
	(void)c;
	(void)off;
	(void)size;

	// Read
	AlxFs* alxFs_me = (AlxFs*)c->context;
	uint32_t numOfBlocks = size / 512;
	uint32_t blockAddr = (block * c->block_size + off) / 512;
//	LL_GPIO_SetOutputPin(GPIOC, GPIO_PIN_3);	// di_ADC_nDRDY - DBG9
	Alx_Status status = AlxMmc_ReadBlock(alxFs_me->alxMmc, numOfBlocks, blockAddr, (uint8_t*)buffer, size, 1, 1000);
//	LL_GPIO_ResetOutputPin(GPIOC, GPIO_PIN_3);	// di_ADC_nDRDY - DBG9
	if (status != Alx_Ok)
	{
		return LFS_ERR_IO;
	}

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	// Local variables
	(void)c;
	(void)off;
	(void)size;

	// Write
	AlxFs* alxFs_me = (AlxFs*)c->context;
	uint32_t numOfBlocks = size / 512;
	uint32_t blockAddr = (block * c->block_size + off) / 512;
//	LL_GPIO_SetOutputPin(GPIOF, GPIO_PIN_14);	// do_ADC_I2C_SCL - DBG10
	Alx_Status status = AlxMmc_WriteBlock(alxFs_me->alxMmc, numOfBlocks, blockAddr, (uint8_t*)buffer, size, 1, 1000);
//	LL_GPIO_ResetOutputPin(GPIOF, GPIO_PIN_14);	// do_ADC_I2C_SCL - DBG10
	if (status != Alx_Ok)
	{
		return LFS_ERR_IO;
	}

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_EraseBlock(const struct lfs_config* c, lfs_block_t block)
{
	// Local variables
	(void)c;
	(void)block;

//	LL_GPIO_SetOutputPin(GPIOF, GPIO_PIN_15);	// io_ADC_I2C_SDA - DBG11
//	LL_GPIO_ResetOutputPin(GPIOF, GPIO_PIN_15);	// io_ADC_I2C_SDA - DBG11

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_SyncBlock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return LFS_ERR_OK;
}
#if defined(LFS_THREADSAFE)
static int AlxFs_Lfs_Mmc_Lock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_Unlock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return LFS_ERR_OK;
}
#endif
#endif


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))
