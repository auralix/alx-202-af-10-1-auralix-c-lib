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
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Variables
//******************************************************************************
#if defined(ALX_FATFS)
static AlxFs* alxFs_Fatfs_Mmc_Usb_me = NULL;
#endif


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Fatfs_Mmc & Fatfs_Usb
//------------------------------------------------------------------------------
#if defined(ALX_FATFS)
static void AlxFs_Fatfs_Mmc_Usb_Ctor(AlxFs* me);
#endif


//------------------------------------------------------------------------------
// Lfs_FlashInt
//------------------------------------------------------------------------------
#if defined(ALX_LFS)
static void AlxFs_Lfs_FlashInt_Ctor(AlxFs* me);
static int AlxFs_Lfs_FlashInt_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_FlashInt_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_FlashInt_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_FlashInt_SyncBlock(const struct lfs_config* c);
#if defined(LFS_THREADSAFE)
static int AlxFs_Lfs_FlashInt_Lock(const struct lfs_config* c);
static int AlxFs_Lfs_FlashInt_Unlock(const struct lfs_config* c);
#endif
#endif


//------------------------------------------------------------------------------
// Lfs_Mmc
//------------------------------------------------------------------------------
#if defined(ALX_LFS)
static void AlxFs_Lfs_Mmc_Ctor(AlxFs* me);
static int AlxFs_Lfs_Mmc_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_Mmc_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_Mmc_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_Mmc_SyncBlock(const struct lfs_config* c);
#if defined(LFS_THREADSAFE)
static int AlxFs_Lfs_Mmc_Lock(const struct lfs_config* c);
static int AlxFs_Lfs_Mmc_Unlock(const struct lfs_config* c);
#endif
#endif


//------------------------------------------------------------------------------
// Callback Functions
//------------------------------------------------------------------------------
static Alx_Status AlxFs_File_Trace_ChunkRead_Callback(void* ctx, void* chunkData, uint32_t chunkLenActual);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFs_Ctor
(
	AlxFs* me,
	AlxFs_Config config,
	AlxMmc* alxMmc,
	AlxUsb* alxUsb,
	AlxIoPin* do_DBG_ReadBlock,
	AlxIoPin* do_DBG_WriteBlock,
	AlxIoPin* do_DBG_EraseBlock,
	AlxIoPin* do_DBG_SyncBlock
)
{
	// Parameters
	me->config = config;
	me->alxMmc = alxMmc;
	me->alxUsb = alxUsb;
	me->do_DBG_ReadBlock = do_DBG_ReadBlock;
	me->do_DBG_WriteBlock = do_DBG_WriteBlock;
	me->do_DBG_EraseBlock = do_DBG_EraseBlock;
	me->do_DBG_SyncBlock = do_DBG_SyncBlock;

	// Ctor
	if (me->config == AlxFs_Config_Undefined)
	{
	}
	#if defined(ALX_FATFS)
	else if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		AlxFs_Fatfs_Mmc_Usb_Ctor(me);
	}
	#endif
	#if defined(ALX_LFS)
	else if (me->config == AlxFs_Config_Lfs_FlashInt)
	{
		AlxFs_Lfs_FlashInt_Ctor(me);
	}
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

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_mount(&me->fatfs, ALX_FS_FATFS_PATH, 1);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		ALX_FS_ASSERT(me->lfsConfig.block_size % me->lfsConfig.read_size == 0);
		ALX_FS_ASSERT(me->lfsConfig.block_size % me->lfsConfig.prog_size == 0);
		ALX_FS_ASSERT(me->lfsConfig.cache_size % me->lfsConfig.read_size == 0);
		ALX_FS_ASSERT(me->lfsConfig.cache_size % me->lfsConfig.prog_size == 0);
		ALX_FS_ASSERT(me->lfsConfig.block_size % me->lfsConfig.cache_size == 0);
		ALX_FS_ASSERT(me->lfsConfig.lookahead_size % 8 == 0);

		status = lfs_mount(&me->lfs, &me->lfsConfig);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

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

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_unmount(ALX_FS_FATFS_PATH);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_unmount(&me->lfs);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

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
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE_WRN("Err: %d", status);
			return status;
		}

		// Mount
		status = AlxFs_Mount(me);
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE_WRN("Err: %d", status);
			return status;
		}
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Format(AlxFs* me)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	// isMounted -> Don't care

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_mkfs(ALX_FS_FATFS_PATH, &me->fatfsMkfsOpt, me->fatfsMkfsBuff, sizeof(me->fatfsMkfsBuff));
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_format(&me->lfs, &me->lfsConfig);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Remove(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_unlink(path);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_remove(&me->lfs, path);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Rename(AlxFs* me, const char* pathOld, const char* pathNew)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_rename(pathOld, pathNew);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_rename(&me->lfs, pathOld, pathNew);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d, pathOld=%s, pathNew=%s", status, pathOld, pathNew);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Open(AlxFs* me, AlxFs_File* file, const char* path, const char* mode)
{
	// https://en.cppreference.com/w/c/io/fopen
	// http://elm-chan.org/fsw/ff/doc/open.html

	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		// Local variables
		uint8_t fatfsMode = 0;

		// Prepare
		if (0 == strcmp(mode, "r"))
		{
			fatfsMode = FA_READ;
		}
		else if (0 == strcmp(mode, "w"))
		{
			fatfsMode = FA_WRITE | FA_CREATE_ALWAYS;
		}
		else if (0 == strcmp(mode, "a"))
		{
			fatfsMode = FA_WRITE | FA_OPEN_APPEND;
		}
		else if (0 == strcmp(mode, "r+"))
		{
			fatfsMode = FA_READ | FA_WRITE;
		}
		else if (0 == strcmp(mode, "w+"))
		{
			fatfsMode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS;
		}
		else if (0 == strcmp(mode, "a+"))
		{
			fatfsMode = FA_READ | FA_WRITE | FA_OPEN_APPEND;
		}
		else
		{
			ALX_FS_ASSERT(false);	// We should never get here
		}

		// Do
		status = f_open(&file->fatfsFile, path, fatfsMode);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		// Local variables
		int32_t lfsMode = 0;

		// Prepare
		if (0 == strcmp(mode, "r"))
		{
			lfsMode = LFS_O_RDONLY;
		}
		else if (0 == strcmp(mode, "w"))
		{
			lfsMode = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
		}
		else if (0 == strcmp(mode, "a"))
		{
			lfsMode = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
		}
		else if (0 == strcmp(mode, "r+"))
		{
			lfsMode = LFS_O_RDWR;
		}
		else if (0 == strcmp(mode, "w+"))
		{
			lfsMode = LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC;
		}
		else if (0 == strcmp(mode, "a+"))
		{
			lfsMode = LFS_O_RDWR | LFS_O_CREAT | LFS_O_APPEND;
		}
		else
		{
			ALX_FS_ASSERT(false);	// We should never get here
		}

		// Do
		status = lfs_file_open(&me->lfs, &file->lfsFile, path, lfsMode);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s, mode=%s", status, path, mode);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Close(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_close(&file->fatfsFile);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_file_close(&me->lfs, &file->lfsFile);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Read(AlxFs* me, AlxFs_File* file, void* data, uint32_t len, uint32_t* lenActual)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		unsigned int _lenActual = 0;
		int32_t status = f_read(&file->fatfsFile, data, len, &_lenActual);
		if (status != 0)
		{
			ALX_FS_TRACE_WRN("Err: %d, lenActual=%d, len=%u", status, _lenActual, len);
			return Alx_Err;
		}
		*lenActual = (uint32_t)_lenActual;
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		int32_t _lenActual = lfs_file_read(&me->lfs, &file->lfsFile, data, len);
		if (_lenActual < 0)
		{
			ALX_FS_TRACE_WRN("Err: lenActual=%d, len=%u", _lenActual, len);
			return Alx_Err;
		}
		*lenActual = (uint32_t)_lenActual;
	}
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
	Alx_Status statusRead = Alx_Err;
	uint32_t _lenActual = 0;
	char ch __attribute__((aligned(4))) = 0;
	uint32_t readLenActual = 0;

	// Loop
	for (_lenActual = 0; _lenActual < (len - 1); _lenActual++)
	{
		// Read char
		statusRead = AlxFs_File_Read(me, file, &ch, 1, &readLenActual);
		if ((statusRead == Alx_Ok) && (readLenActual == 0))
		{
			// Break, we reached end of file, status already AlxFs_ErrNoDelim
			break;
		}
		else if (statusRead != Alx_Ok)
		{
			// Break, error occured, trace, change status to Alx_Err
			ALX_FS_TRACE_WRN("Err: %d, ch=%d, readLenActual=%u, lenActual=%u, len=%u, delim=%s", statusRead, ch, readLenActual, _lenActual, len, delim);
			status = Alx_Err;
			break;
		}
		else if (readLenActual != 1)
		{
			ALX_FS_ASSERT(false);		// We should never get here
		}

		// Store char
		str[_lenActual] = ch;

		// Check if char is delim
		if (ch == *delim)
		{
			// Break, we found delimiter, increment i for correct null-termination, change status to Alx_Ok
			_lenActual++;
			status = Alx_Ok;
			break;
		}
	}

	// Null-terminate string
	str[_lenActual] = '\0';

	// Return
	*lenActual = _lenActual;
	return status;
}
Alx_Status AlxFs_File_Write(AlxFs* me, AlxFs_File* file, void* data, uint32_t len)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		unsigned int lenActual = 0;
		int32_t status = f_write(&file->fatfsFile, data, len, &lenActual);
		if ((status != 0) || (lenActual != len))
		{
			ALX_FS_TRACE_WRN("Err: %d, lenActual=%d, len=%u", status, lenActual, len);
			return Alx_Err;
		}
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		int32_t lenActual = lfs_file_write(&me->lfs, &file->lfsFile, data, len);
		if (lenActual != (int32_t)len)
		{
			ALX_FS_TRACE_WRN("Err: lenActual=%d, len=%u", lenActual, len);
			return Alx_Err;
		}
	}
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

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_sync(&file->fatfsFile);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_file_sync(&me->lfs, &file->lfsFile);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Seek(AlxFs* me, AlxFs_File* file, int32_t offset, AlxFs_File_Seek_Origin origin, uint32_t* positionNew)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		// Local variables
		int32_t fatfsPositionNew = 0;

		// Prepare
		if (origin == AlxFs_File_Seek_Origin_Set)
		{
			fatfsPositionNew = offset;
		}
		else if(origin == AlxFs_File_Seek_Origin_Cur)
		{
			fatfsPositionNew = f_tell(&file->fatfsFile) + offset;
		}
		else if(origin == AlxFs_File_Seek_Origin_End)
		{
			fatfsPositionNew = f_size(&file->fatfsFile) + offset;
		}
		else
		{
			ALX_FS_ASSERT(false);	// We should never get here
		}

		// Do
		int32_t status = f_lseek(&file->fatfsFile, fatfsPositionNew);
		if (status != 0)
		{
			ALX_FS_TRACE_WRN("Err: %d, fatfsPositionNew=%d, offset=%d, origin=%d", status, fatfsPositionNew, offset, origin);
			return Alx_Err;
		}

		// Return
		*positionNew = (uint32_t)fatfsPositionNew;
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		// Local variables
		int lfsOrigin = 0;

		// Prepare
		if (origin == AlxFs_File_Seek_Origin_Set)
		{
			lfsOrigin = LFS_SEEK_SET;
		}
		else if(origin == AlxFs_File_Seek_Origin_Cur)
		{
			lfsOrigin = LFS_SEEK_CUR;
		}
		else if(origin == AlxFs_File_Seek_Origin_End)
		{
			lfsOrigin = LFS_SEEK_END;
		}
		else
		{
			ALX_FS_ASSERT(false);	// We should never get here
		}

		// Do
		int32_t lfsPositionNew = lfs_file_seek(&me->lfs, &file->lfsFile, (lfs_soff_t)offset, lfsOrigin);
		if (lfsPositionNew < 0)
		{
			ALX_FS_TRACE_WRN("Err: lfsPositionNew=%d, offset=%d, origin=%d", lfsPositionNew, offset, origin);
			return Alx_Err;
		}

		// Return
		*positionNew = (uint32_t)lfsPositionNew;
	}
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Tell(AlxFs* me, AlxFs_File* file, uint32_t* position)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t _position = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		_position = f_tell(&file->fatfsFile);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		_position = lfs_file_tell(&me->lfs, &file->lfsFile);
	}
	#endif

	// Trace
	if (_position < 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", _position);
		return Alx_Err;
	}

	// Return
	*position = (uint32_t)_position;
	return Alx_Ok;
}
Alx_Status AlxFs_File_Rewind(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_rewind(&file->fatfsFile);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_file_rewind(&me->lfs, &file->lfsFile);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_Size(AlxFs* me, AlxFs_File* file, uint32_t* size)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t _size = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		_size = f_size(&file->fatfsFile);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		_size = lfs_file_size(&me->lfs, &file->lfsFile);
	}
	#endif

	// Trace
	if (_size < 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", _size);
		return Alx_Err;
	}

	// Return
	*size = (uint32_t)_size;
	return Alx_Ok;
}
Alx_Status AlxFs_File_Truncate(AlxFs* me, AlxFs_File* file, uint32_t size)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		// Move file pointer to the new end of the file, with this we can expand or shrink file
		status = f_lseek(&file->fatfsFile, size);

		// If f_lseek OK, truncate file at the current file pointer
		if (status == 0)
		{
			status = f_truncate(&file->fatfsFile);
		}
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_file_truncate(&me->lfs, &file->lfsFile, size);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d, size=%u", status, size);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_File_ReadInChunks(AlxFs* me, const char* path, uint8_t* chunkBuff, uint32_t chunkLen, Alx_Status(*chunkRead_Callback)(void* ctx, void* chunkData, uint32_t chunkLenActual), void* chunkRead_Callback_Ctx, uint32_t* readLen, AlxOsMutex* alxOsMutex)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};
	uint32_t fileSize = 0;
	uint32_t fileSizeRead = 0;
	uint32_t chunkLenActual = 0;

	// Open
	if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
	status = AlxFs_File_Open(me, &file, path, "r");
	if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
		return status;
	}

	// Get fileSize
	if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
	status = AlxFs_File_Size(me, &file, &fileSize);
	if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s, fileSize=%u", status, path, fileSize);
		if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
		Alx_Status statusClose = AlxFs_File_Close(me, &file);
		if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
		if (statusClose != Alx_Ok)
		{
			ALX_FS_TRACE_WRN("Err: %d, path=%s", statusClose, path);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Loop
	while (true)
	{
		// Read
		memset(chunkBuff, 0, chunkLen);
		if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
		status = AlxFs_File_Read(me, &file, chunkBuff, chunkLen, &chunkLenActual);
		if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE_WRN("Err: %d, path=%s, fileSize=%u, chunkLen=%u, chunkLenActual=%u", status, path, fileSize, chunkLen, chunkLenActual);
			if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
			Alx_Status statusClose = AlxFs_File_Close(me, &file);
			if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
			if (statusClose != Alx_Ok)
			{
				ALX_FS_TRACE_WRN("Err: %d, path=%s", statusClose, path);
				// TV: TODO - Handle close error
			}
			return status;
		}

		// Callback
		status = chunkRead_Callback(chunkRead_Callback_Ctx, chunkBuff, chunkLenActual);
		if (status != Alx_Ok)
		{
			ALX_FS_TRACE_WRN("Err: %d, path=%s, fileSize=%u, chunkLenActual=%u", status, path, fileSize, chunkLenActual);
			if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
			Alx_Status statusClose = AlxFs_File_Close(me, &file);
			if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
			if (statusClose != Alx_Ok)
			{
				ALX_FS_TRACE_WRN("Err: %d, path=%s", statusClose, path);
				// TV: TODO - Handle close error
			}
			return status;
		}

		// Increment fileSizeRead
		fileSizeRead = fileSizeRead + chunkLenActual;

		// If done, break
		if (fileSizeRead >= fileSize)
		{
			break;
		}
	}

	// Close
	if (alxOsMutex != NULL) AlxOsMutex_Lock(alxOsMutex);
	status = AlxFs_File_Close(me, &file);
	if (alxOsMutex != NULL) AlxOsMutex_Unlock(alxOsMutex);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
		// TV: TODO - Handle close error
		return status;
	}

	// Return
	*readLen = fileSizeRead;
	return Alx_Ok;
}
Alx_Status AlxFs_File_Trace(AlxFs* me, const char* path, bool bin)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	uint8_t chunkBuff[ALX_FS_BUFF_LEN] = {};

	// Trace
	ALX_FS_TRACE_FORMAT("AlxFs - File '%s' Trace\r\n", path);

	// Read
	uint32_t readLen = 0;
	Alx_Status status = AlxFs_File_ReadInChunks(me, path, chunkBuff, sizeof(chunkBuff), AlxFs_File_Trace_ChunkRead_Callback, &bin, &readLen, NULL);

	// Trace
	ALX_FS_TRACE_FORMAT("\r\n");

	// Return
	return status;
}
Alx_Status AlxFs_Dir_Make(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_mkdir(path);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_mkdir(&me->lfs, path);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Open(AlxFs* me, AlxFs_Dir* dir, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_opendir(&dir->fatfsDir, path);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_dir_open(&me->lfs, &dir->lfsDir, path);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Close(AlxFs* me, AlxFs_Dir* dir)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_closedir(&dir->fatfsDir);
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_dir_close(&me->lfs, &dir->lfsDir);
	}
	#endif

	// Trace
	if (status != 0)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Read(AlxFs* me, AlxFs_Dir* dir, AlxFs_Info* info)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Local variables
	int32_t status = -1;

	// Do
	#if defined(ALX_FATFS)
	if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
	{
		status = f_readdir(&dir->fatfsDir, &info->fatfsInfo);
		if (status != 0)
		{
			ALX_FS_TRACE_WRN("Err: %d", status);
			return Alx_Err;
		}
		else if (strcmp(info->fatfsInfo.fname, "") == 0)
		{
			return AlxFs_EndOfDir;
		}
	}
	#endif
	#if defined(ALX_LFS)
	if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
	{
		status = lfs_dir_read(&me->lfs, &dir->lfsDir, &info->lfsInfo);
		if (status == 0)
		{
			return AlxFs_EndOfDir;
		}
		else if (status != 1)
		{
			ALX_FS_TRACE_WRN("Err: %d", status);
			return Alx_Err;
		}
	}
	#endif

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Dir_Trace(AlxFs* me, const char* path, bool fileTrace, bool fileBin)
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
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s, fileTrace=%u", status, path, fileTrace);
		return status;
	}

	// Loop
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
			ALX_FS_TRACE_WRN("Err: %d, path=%s, fileTrace=%u", status, path, fileTrace);
			Alx_Status statusClose = AlxFs_Dir_Close(me, &dir);
			if (statusClose != Alx_Ok)
			{
				ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
				// TV: TODO - Handle close error
			}
			return status;
		}

		// Prepare
		bool isFile = false;
		#if defined(ALX_FATFS)
		if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
		{
			if (info.fatfsInfo.fattrib & AM_DIR)
			{
				isFile = false;
			}
			else
			{
				isFile = true;
			}
		}
		#endif
		#if defined(ALX_LFS)
		if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
		{
			if (info.lfsInfo.type == LFS_TYPE_REG)
			{
				isFile = true;
			}
			else if (info.lfsInfo.type == LFS_TYPE_DIR)
			{
				isFile = false;
			}
			else
			{
				ALX_FS_ASSERT(false);	// We should never get here
			}
		}
		#endif

		// Trace
		if (isFile)
		{
			// Prepare
			#if defined(ALX_FATFS)
			if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
			{
				sprintf(buff, "FILE - %s - %lu B\r\n", info.fatfsInfo.fname, info.fatfsInfo.fsize);
			}
			#endif
			#if defined(ALX_LFS)
			if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
			{
				sprintf(buff, "FILE - %s - %lu B\r\n", info.lfsInfo.name, info.lfsInfo.size);
			}
			#endif

			// Trace
			ALX_FS_TRACE_FORMAT("%s", buff);

			// If fileTrace enabled
			if (fileTrace)
			{
				// Prepare
				#if defined(ALX_FATFS)
				if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
				{
					sprintf(buff, "%s/%s", path, info.fatfsInfo.fname);
				}
				#endif
				#if defined(ALX_LFS)
				if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
				{
					sprintf(buff, "%s/%s", path, info.lfsInfo.name);
				}
				#endif

				// Trace
				status = AlxFs_File_Trace(me, buff, fileBin);
				if (status != Alx_Ok)
				{
					ALX_FS_TRACE_WRN("Err: %d, path=%s, fileTrace=%u", status, path, fileTrace);
					Alx_Status statusClose = AlxFs_Dir_Close(me, &dir);
					if (statusClose != Alx_Ok)
					{
						ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
						// TV: TODO - Handle close error
					}
					return status;
				}
			}
		}
		else
		{
			// Prepare
			#if defined(ALX_FATFS)
			if (me->config == AlxFs_Config_Fatfs_Mmc || me->config == AlxFs_Config_Fatfs_Usb)
			{
				sprintf(buff, "DIR - %s\r\n", info.fatfsInfo.fname);
			}
			#endif
			#if defined(ALX_LFS)
			if (me->config == AlxFs_Config_Lfs_FlashInt || me->config == AlxFs_Config_Lfs_Mmc)
			{
				sprintf(buff, "DIR - %s\r\n", info.lfsInfo.name);
			}
			#endif

			// Trace
			ALX_FS_TRACE_FORMAT("%s", buff);
		}
	}

	// Close
	status = AlxFs_Dir_Close(me, &dir);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d, path=%s", status, path);
		// TV: TODO - Handle close error
		return status;
	}

	// Trace
	ALX_FS_TRACE_FORMAT("\r\n");

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************


//------------------------------------------------------------------------------
// Fatfs_Mmc & Fatfs_Usb
//------------------------------------------------------------------------------
#if defined(ALX_FATFS)
static void AlxFs_Fatfs_Mmc_Usb_Ctor(AlxFs* me)
{
	//------------------------------------------------------------------------------
	// Private Variables
	//------------------------------------------------------------------------------
	alxFs_Fatfs_Mmc_Usb_me = me;


	//------------------------------------------------------------------------------
	// Variables
	//------------------------------------------------------------------------------
	memset(&me->fatfs, 0, sizeof(me->fatfs));
	me->fatfsMkfsOpt.fmt = FM_FAT32;
	me->fatfsMkfsOpt.n_fat = 1;
	me->fatfsMkfsOpt.align = 0;
	me->fatfsMkfsOpt.n_root = ALX_NULL;
	me->fatfsMkfsOpt.au_size = 0;
	memset(&me->fatfsMkfsBuff, 0, sizeof(me->fatfsMkfsBuff));
}
DSTATUS disk_initialize (BYTE pdrv)
{
	(void)pdrv;

	return RES_OK;
}
DSTATUS disk_status (BYTE pdrv)
{
	(void)pdrv;

	return RES_OK;
}
DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	// Prepare
	(void)pdrv;
	Alx_Status status = Alx_Err;

	// Read
	if(alxFs_Fatfs_Mmc_Usb_me->do_DBG_ReadBlock != NULL) AlxIoPin_Set(alxFs_Fatfs_Mmc_Usb_me->do_DBG_ReadBlock);
	#if defined(ALX_STM32L4)
	status = AlxMmc_ReadBlock(alxFs_Fatfs_Mmc_Usb_me->alxMmc, count, sector, buff, count * 512, 3, 100);
	#endif
	if(alxFs_Fatfs_Mmc_Usb_me->do_DBG_ReadBlock != NULL) AlxIoPin_Reset(alxFs_Fatfs_Mmc_Usb_me->do_DBG_ReadBlock);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return RES_ERROR;
	}

	// Return
	return RES_OK;
}
DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	// Prepare
	(void)pdrv;
	Alx_Status status = Alx_Err;

	// Write
	if(alxFs_Fatfs_Mmc_Usb_me->do_DBG_WriteBlock != NULL) AlxIoPin_Set(alxFs_Fatfs_Mmc_Usb_me->do_DBG_WriteBlock);
	#if defined(ALX_STM32L4)
	status = AlxMmc_WriteBlock(alxFs_Fatfs_Mmc_Usb_me->alxMmc, count, sector, (uint8_t*)buff, count * 512, 3, 100);
	#endif
	if(alxFs_Fatfs_Mmc_Usb_me->do_DBG_WriteBlock != NULL) AlxIoPin_Reset(alxFs_Fatfs_Mmc_Usb_me->do_DBG_WriteBlock);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return RES_ERROR;
	}

	return RES_OK;
}
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	// Prepare
	(void)pdrv;
	Alx_Status status = Alx_Err;

	// Get
	switch (cmd)
	{
		case CTRL_SYNC:
		{
			if(alxFs_Fatfs_Mmc_Usb_me->do_DBG_SyncBlock != NULL) AlxIoPin_Set(alxFs_Fatfs_Mmc_Usb_me->do_DBG_SyncBlock);
			#if defined(ALX_STM32L4)
			status = AlxMmc_WaitForTransferState(alxFs_Fatfs_Mmc_Usb_me->alxMmc);
			#endif
			if(alxFs_Fatfs_Mmc_Usb_me->do_DBG_SyncBlock != NULL) AlxIoPin_Reset(alxFs_Fatfs_Mmc_Usb_me->do_DBG_SyncBlock);
			if (status != Alx_Ok)
			{
				ALX_FS_TRACE_WRN("Err: %d", status);
				return RES_NOTRDY;
			}

			return RES_OK;
		}
		case GET_SECTOR_COUNT:
		{
			*(LBA_t*)buff = 62160896;
			return RES_OK;
		}
		case GET_SECTOR_SIZE:
		{
			*(WORD*)buff = 512;
			return RES_OK;
		}
		case GET_BLOCK_SIZE:
		{
			*(DWORD*)buff = 1;	// TV: Seems like this shall be the same as erasable block size, but we can use simplification approach and set it to 1
			return RES_OK;
		}
		default:
		{
			return RES_PARERR;
		}
	}
}
#endif


//------------------------------------------------------------------------------
// Lfs_FlashInt
//------------------------------------------------------------------------------
#if defined(ALX_LFS)
static void AlxFs_Lfs_FlashInt_Ctor(AlxFs* me)
{
	//------------------------------------------------------------------------------
	// General
	//------------------------------------------------------------------------------
	memset(&me->lfs, 0, sizeof(me->lfs));
	me->lfsConfig.context = me;


	//------------------------------------------------------------------------------
	// Functions
	//------------------------------------------------------------------------------
	me->lfsConfig.read  = AlxFs_Lfs_FlashInt_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_FlashInt_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_FlashInt_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_FlashInt_SyncBlock;
	#if defined(LFS_THREADSAFE)
	me->lfsConfig.lock = AlxFs_Lfs_FlashInt_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_FlashInt_Unlock;
	#endif


	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	#if defined(ALX_STM32F4) || defined(ALX_STM32F7)
	me->lfsAddr = 0x08100000;
	me->lfsConfig.read_size = 1;
	me->lfsConfig.prog_size = 4;
	me->lfsConfig.block_size = 16 * 1024;
	me->lfsConfig.block_count = 4;
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
	AlxFs* me = (AlxFs*)c->context;
	uint32_t src_address = me->lfsAddr + block * c->block_size + off;

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
		AlxFs* me = (AlxFs*)c->context;
		uint32_t block_base_addr = me->lfsAddr + block * c->block_size;
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
		AlxFs* me = (AlxFs*)c->context;
		uint32_t block_base_addr = me->lfsAddr + block * c->block_size;
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
// Lfs_Mmc
//------------------------------------------------------------------------------
#if defined(ALX_LFS)
static void AlxFs_Lfs_Mmc_Ctor(AlxFs* me)
{
	//------------------------------------------------------------------------------
	// General
	//------------------------------------------------------------------------------
	memset(&me->lfs, 0, sizeof(me->lfs));
	me->lfsConfig.context = me;


	//------------------------------------------------------------------------------
	// Functions
	//------------------------------------------------------------------------------
	me->lfsConfig.read  = AlxFs_Lfs_Mmc_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_Mmc_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_Mmc_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_Mmc_SyncBlock;
	#if defined(LFS_THREADSAFE)
	me->lfsConfig.lock = AlxFs_Lfs_Mmc_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_Mmc_Unlock;
	#endif


	//------------------------------------------------------------------------------
	// Parameters
	//------------------------------------------------------------------------------
	#define ALX_FS_MULTIPLY 16
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
	// Prepare
	Alx_Status status = Alx_Err;
	AlxFs* me = (AlxFs*)c->context;
	uint32_t numOfBlocks = size / 512;
	uint32_t blockAddr = (block * c->block_size + off) / 512;

	// Read
	if(me->do_DBG_ReadBlock != NULL) AlxIoPin_Set(me->do_DBG_ReadBlock);
	#if defined(ALX_STM32L4)
	status = AlxMmc_ReadBlock(me->alxMmc, numOfBlocks, blockAddr, (uint8_t*)buffer, size, 3, 100);
	#endif
	if(me->do_DBG_ReadBlock != NULL) AlxIoPin_Reset(me->do_DBG_ReadBlock);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return LFS_ERR_IO;
	}

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	// Prepare
	Alx_Status status = Alx_Err;
	AlxFs* me = (AlxFs*)c->context;
	uint32_t numOfBlocks = size / 512;
	uint32_t blockAddr = (block * c->block_size + off) / 512;

	// Write
	if(me->do_DBG_WriteBlock != NULL) AlxIoPin_Set(me->do_DBG_WriteBlock);
	#if defined(ALX_STM32L4)
	status = AlxMmc_WriteBlock(me->alxMmc, numOfBlocks, blockAddr, (uint8_t*)buffer, size, 3, 100);
	#endif
	if(me->do_DBG_WriteBlock != NULL) AlxIoPin_Reset(me->do_DBG_WriteBlock);
	if (status != Alx_Ok)
	{
		ALX_FS_TRACE_WRN("Err: %d", status);
		return LFS_ERR_IO;
	}

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_EraseBlock(const struct lfs_config* c, lfs_block_t block)
{
	// Prepare
	AlxFs* me = (AlxFs*)c->context;
	(void)block;

	// Toggle
	if(me->do_DBG_EraseBlock != NULL) AlxIoPin_Set(me->do_DBG_EraseBlock);
	if(me->do_DBG_EraseBlock != NULL) AlxIoPin_Reset(me->do_DBG_EraseBlock);

	// Return
	return LFS_ERR_OK;
}
static int AlxFs_Lfs_Mmc_SyncBlock(const struct lfs_config* c)
{
	// Prepare
	AlxFs* me = (AlxFs*)c->context;

	// Toggle
	if(me->do_DBG_SyncBlock != NULL) AlxIoPin_Set(me->do_DBG_SyncBlock);
	if(me->do_DBG_SyncBlock != NULL) AlxIoPin_Reset(me->do_DBG_SyncBlock);

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


//------------------------------------------------------------------------------
// Callback Functions
//------------------------------------------------------------------------------
static Alx_Status AlxFs_File_Trace_ChunkRead_Callback(void* ctx, void* chunkData, uint32_t chunkLenActual)
{
	// Prepare
	bool bin = *(bool*)ctx;

	// Trace
	if (bin)
	{
		for (uint32_t i = 0; i < chunkLenActual; i = i + 64)
		{
			// Prepare line length
			uint32_t lineLen = 0;
			if (chunkLenActual - i > 64)
			{
				lineLen = 64;
			}
			else
			{
				lineLen = chunkLenActual - i;
			}

			// Trace data
			for (uint32_t j = 0; j < lineLen; j++)
			{
				ALX_FS_TRACE_FORMAT("%02X ", ((uint8_t*)chunkData)[i + j]);
			}

			// Trace new line
			ALX_FS_TRACE_FORMAT("\r\n");
		}
	}
	else
	{
		ALX_FS_TRACE_FORMAT("%s", chunkData);
	}

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
