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
#if defined(ALX_C_LIB) && defined(ALX_LFS) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Flash - Internal
//------------------------------------------------------------------------------
static void AlxFs_Lfs_FlashInt_Ctor(AlxFs* me);
static int AlxFs_Lfs_FlashInt_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_FlashInt_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_FlashInt_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_FlashInt_SyncBlock(const struct lfs_config* c);
static int AlxFs_Lfs_FlashInt_Lock(const struct lfs_config* c);
static int AlxFs_Lfs_FlashInt_Unlock(const struct lfs_config* c);


//------------------------------------------------------------------------------
// MMC
//------------------------------------------------------------------------------
static void AlxFs_Lfs_Mmc_Ctor(AlxFs* me);
static int AlxFs_Lfs_Mmc_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_Mmc_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_Mmc_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_Mmc_SyncBlock(const struct lfs_config* c);
static int AlxFs_Lfs_Mmc_Lock(const struct lfs_config* c);
static int AlxFs_Lfs_Mmc_Unlock(const struct lfs_config* c);


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
	if (me->config == AlxFs_Config_Lfs_FlashInt)
	{
		AlxFs_Lfs_FlashInt_Ctor(me);
	}
	else if	(me->config == AlxFs_Config_Lfs_Mmc)
	{
		AlxFs_Lfs_Mmc_Ctor(me);
	}
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
	int status = lfs_mount(&me->lfs, &me->lfsConfig);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

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
	int status = lfs_unmount(&me->lfs);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

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
		if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

		// Mount
		status = AlxFs_Mount(me);
		if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }
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
	int status = lfs_format(&me->lfs, &me->lfsConfig);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Remove(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_remove(&me->lfs, path);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Rename(AlxFs* me, const char* pathOld, const char* pathNew)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_rename(&me->lfs, pathOld, pathNew);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Open(AlxFs* me, AlxFs_File* file, const char* path, const char* mode)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Set mode flags
	int modeFlags = 0;
	if (0 == strcmp(mode, "r"))
	{
		modeFlags = LFS_O_RDONLY;
	}
	else if (0 == strcmp(mode, "w"))
	{
		modeFlags = LFS_O_WRONLY | LFS_O_CREAT;
	}
	else if (0 == strcmp(mode, "a"))
	{
		modeFlags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
	}
	else
	{
		ALX_FS_ASSERT(false);	// We should never get here
	}

	// Do
	int status = lfs_file_open(&me->lfs, &file->lfsFile, path, modeFlags);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Close(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_file_close(&me->lfs, &file->lfsFile);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Read(AlxFs* me, AlxFs_File* file, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	lfs_ssize_t statusLenActual = lfs_file_read(&me->lfs, &file->lfsFile, data, (lfs_size_t)lenMax);
	if(statusLenActual < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	*lenActual = (uint32_t)statusLenActual;
	return Alx_Ok;
}
Alx_Status AlxFs_File_ReadStrUntil(AlxFs* me, AlxFs_File* file, char* str, const char* delim, uint32_t lenMax, uint32_t* lenActual)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);
	ALX_FS_ASSERT(strlen(delim) == 1);	// Only single char delim supported

	// Local variables
	uint32_t i = 0;

	// Loop
	for (i = 0; i < (lenMax - 1); i++)
	{
		// Local variables
		char ch = 0;
		uint32_t readLenActual = 0;

		// Read char
		Alx_Status status = AlxFs_File_Read(me, file, &ch, 1, &readLenActual);
		if (status != Alx_Ok)			// If error
		{
			return Alx_Err;
		}
		else if (readLenActual == 0)	// If end-of-file
		{
			break;
		}
		else if (readLenActual != 1)	// Assert
		{
			ALX_FS_ASSERT(false);		// We should never get here
		}

		// Store char
		str[i] = ch;

		// Check if char is delim
		if (ch == *delim)
		{
			i++;	// Increment i to include delim in str[i]
			break;
		}
	}

	// Null-terminate string
	str[i] = '\0';

	// Return
	*lenActual = i;
	return Alx_Ok;
}
Alx_Status AlxFs_File_Write(AlxFs* me, AlxFs_File* file, void* data, uint32_t len)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	lfs_ssize_t statusLenActual = lfs_file_write(&me->lfs, &file->lfsFile, data, (lfs_size_t)len);
	if(statusLenActual != (lfs_ssize_t)len) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_WriteStr(AlxFs* me, AlxFs_File* file, char* str)
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
	int status = lfs_file_sync(&me->lfs, &file->lfsFile);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Seek(AlxFs* me, AlxFs_File* file, uint32_t offset, AlxFs_File_Seek_Origin origin, uint32_t* filePositionNew)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	lfs_soff_t statusFilePositionNew = lfs_file_seek(&me->lfs, &file->lfsFile, (lfs_soff_t)offset, (int)origin);
	if(statusFilePositionNew < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	*filePositionNew = (uint32_t)statusFilePositionNew;
	return Alx_Ok;
}
Alx_Status AlxFs_File_Tell(AlxFs* me, AlxFs_File* file, uint32_t* filePositionCurrent)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	lfs_soff_t statusFilePositionCurrent = lfs_file_tell(&me->lfs, &file->lfsFile);
	if(statusFilePositionCurrent < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	*filePositionCurrent = (uint32_t)statusFilePositionCurrent;
	return Alx_Ok;
}
Alx_Status AlxFs_File_Size(AlxFs* me, AlxFs_File* file, uint32_t* fileSize)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	lfs_soff_t statusFileSize = lfs_file_size(&me->lfs, &file->lfsFile);
	if(statusFileSize < 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	*fileSize = (uint32_t)statusFileSize;
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
	uint8_t buff[ALX_FS_BUFF_LEN+1] = {};	// Add +1 for string null-terminator
	uint32_t lenActual = 0;

	// Open
	status = AlxFs_File_Open(me, &file, path, "r");
	if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Get fileSize
	status = AlxFs_File_Size(me, &file, &fileSize);
	if(status != Alx_Ok)
	{
		AlxFs_File_Close(me, &file);	// Will not handle return
		ALX_FS_TRACE("Err");
		return status;
	}

	// Loop
	while (1)
	{
		// Read
		status = AlxFs_File_Read(me, &file, buff, ALX_FS_BUFF_LEN, &lenActual);
		if(status != Alx_Ok)
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
	if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Make(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_mkdir(&me->lfs, path);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Open(AlxFs* me, AlxFs_Dir* dir, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_dir_open(&me->lfs, &dir->lfsDir, path);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Close(AlxFs* me, AlxFs_Dir* dir)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_dir_close(&me->lfs, &dir->lfsDir);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Read(AlxFs* me, AlxFs_Dir* dir, AlxFs_Info* info)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	int status = lfs_dir_read(&me->lfs, &dir->lfsDir, &info->lfsInfo);
	if(status == 0)
	{
		return AlxFs_DirEnd;
	}
	else if(status != 1)
	{
		ALX_FS_TRACE("Err");
		return Alx_Err;
	}

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

	// Open
	status = AlxFs_Dir_Open(me, &dir, path);
	if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Loop
	while (1)
	{
		// Read
		status = AlxFs_Dir_Read(me, &dir, &info);
		if (status == AlxFs_DirEnd)
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
				if(status != Alx_Ok)
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

	// Close
	status = AlxFs_Dir_Close(me, &dir);
	if(status != Alx_Ok) { ALX_FS_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Flash - Internal
//------------------------------------------------------------------------------
static void AlxFs_Lfs_FlashInt_Ctor(AlxFs* me)
{
	memset(&me->lfs, 0, sizeof(me->lfs));

	me->lfsConfig.context = me;

	me->lfsConfig.read  = AlxFs_Lfs_FlashInt_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_FlashInt_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_FlashInt_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_FlashInt_SyncBlock;
	me->lfsConfig.lock = AlxFs_Lfs_FlashInt_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_FlashInt_Unlock;

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
	for(uint32_t i_row = 0; i_row < n_rows; i_row++)
	{
		#if defined(ALX_STM32F4) || defined(ALX_STM32F7)
		// Local variables
		AlxFs* alxFs_me = (AlxFs*)c->context;
		uint32_t block_base_addr = alxFs_me->lfsAddr + block * c->block_size;
		uint32_t dest_address = block_base_addr + off + i_row * 4;		// Multiply by 4 because we write 4 bytes at the time
		uint32_t* src_address_ptr = (uint32_t*)(buffer + i_row * 4);	// Multiply by 4 because we write 4 bytes at the time

		// Program
		HAL_StatusTypeDef xHAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dest_address, *src_address_ptr);
		if(xHAL_Status != HAL_OK)
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
		if(xHAL_Status != HAL_OK)
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


//------------------------------------------------------------------------------
// MMC
//------------------------------------------------------------------------------
static void AlxFs_Lfs_Mmc_Ctor(AlxFs* me)
{
	memset(&me->lfs, 0, sizeof(me->lfs));

	me->lfsConfig.context = me;

	me->lfsConfig.read  = AlxFs_Lfs_Mmc_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_Mmc_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_Mmc_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_Mmc_SyncBlock;
	me->lfsConfig.lock = AlxFs_Lfs_Mmc_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_Mmc_Unlock;

	me->lfsConfig.read_size = 512;
	me->lfsConfig.prog_size = 512;
	me->lfsConfig.block_size = 512;
	me->lfsConfig.block_count = 62160896;
	me->lfsConfig.block_cycles = -1;	// -1 means wear-leveling disabled
	me->lfsConfig.cache_size = 512;
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
	Alx_Status status = AlxMmc_ReadBlock(alxFs_me->alxMmc, 1, block, (uint8_t*)buffer, 512, 1, 10);
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
	Alx_Status status = AlxMmc_WriteBlock(alxFs_me->alxMmc, 1, block, (uint8_t*)buffer, 512, 1, 10);
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LFS) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))
