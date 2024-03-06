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
// Private Variables
//******************************************************************************
static AlxFs* alxFs_me = NULL;


//******************************************************************************
// Private Functions
//******************************************************************************
static int AlxFs_Lfs_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int AlxFs_Lfs_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int AlxFs_Lfs_EraseBlock(const struct lfs_config* c, lfs_block_t block);
static int AlxFs_Lfs_SyncBlock(const struct lfs_config* c);


//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxFs_Lfs_SetConfig(AlxFs* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFs_Ctor
(
	AlxFs* me
)
{
	// Private Variables
	alxFs_me = me;

	// Variables
	memset(&me->lfs, 0, sizeof(me->lfs));
	AlxFs_Lfs_SetConfig(me);

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

	// Mount
	int32_t status = lfs_mount(&me->lfs, &me->lfsConfig);
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

	// UnMount
	int32_t status = lfs_unmount(&me->lfs);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Clear isMounted
	me->isMounted = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Format(AlxFs* me)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	// isMounted -> Don't care

	// Format
	int32_t status = lfs_format(&me->lfs, &me->lfsConfig);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_Remove(AlxFs* me, const char* path)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Remove file/directory
	int32_t status = lfs_remove(&me->lfs, path);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_FileOpen(AlxFs* me, AlxFs_File* file, const char* path, int32_t flags)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Open file
	int32_t status = lfs_file_open(&me->lfs, &file->lsfFile, path, flags);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxFs_FileClose(AlxFs* me, AlxFs_File* file)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Close file
	int32_t status = lfs_file_close(&me->lfs, &file->lsfFile);
	if(status != 0) { ALX_FS_TRACE("Err"); return Alx_Err; }

	// Return
	return Alx_Ok;
}
int32_t AlxFs_FileRead(AlxFs* me, AlxFs_File* file, void* buff, uint32_t len)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Read file
	int64_t statusActualLen = lfs_file_read(&me->lfs, &file->lsfFile, buff, len);
	if(statusActualLen < 0) { ALX_FS_TRACE("Err"); return statusActualLen; }

	// Return
	return statusActualLen;
}
int32_t AlxFs_FileWrite(AlxFs* me, AlxFs_File* file, void* buff, uint32_t len)
{
	// Assert
	ALX_FS_ASSERT(me->wasCtorCalled == true);
	ALX_FS_ASSERT(me->isMounted == true);

	// Write file
	int64_t statusActualLen = lfs_file_write(&me->lfs, &file->lsfFile, buff, len);
	if(statusActualLen != len) { ALX_FS_TRACE("Err"); return statusActualLen; }

	// Return
	return statusActualLen;
}
//int32_t AlxFs_File_Sync(AlxFs* me, AlxFs_File* file)
//{
//	// Assert
//	ALX_FS_ASSERT(me->wasCtorCalled == true);
//	ALX_FS_ASSERT(me->isMounted == true);
//
//	// Close file
//	int32_t status = lfs_file_sync(&me->lfs, &file->lsfFile);
//	if(status != 0) { ALX_FS_TRACE("Err"); return status; }
//
//	// Return
//	return status;
//}


//******************************************************************************
// Private Functions
//******************************************************************************
static int AlxFs_Lfs_ReadBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
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
	uint32_t src_address = alxFs_me->lfsAddr + block * c->block_size + off;

	// Read
	memcpy(buffer, (void*)src_address, size);

	// Lock FLASH
	HAL_FLASH_Lock();

	// Return
	return 0;
}
static int AlxFs_Lfs_ProgBlock(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
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
			return -1;
		}
		#endif
		#if defined(ALX_STM32L4)
		// Local variables
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
			return -1;
		}
		#endif
	}

	// Lock FLASH
	HAL_FLASH_Lock();

	// Return
	return 0;
}
static int AlxFs_Lfs_EraseBlock(const struct lfs_config* c, lfs_block_t block)
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
	return xHAL_Status == HAL_OK ? 0 : -1;
}
static int AlxFs_Lfs_SyncBlock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return 0;
}
#ifdef LFS_THREADSAFE
static int AlxFs_Lfs_Lock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return 0;
}

static int AlxFs_Lfs_Unlock(const struct lfs_config* c)
{
	// Local variables
	(void)c;

	// Return
	return 0;
}
#endif


//******************************************************************************
// Weak Functions
//******************************************************************************
ALX_WEAK void AlxFs_Lfs_SetConfig(AlxFs* me)
{
	me->lfsConfig.read  = AlxFs_Lfs_ReadBlock;
	me->lfsConfig.prog  = AlxFs_Lfs_ProgBlock;
	me->lfsConfig.erase = AlxFs_Lfs_EraseBlock;
	me->lfsConfig.sync  = AlxFs_Lfs_SyncBlock;
	#ifdef LFS_THREADSAFE
	me->lfsConfig.lock = AlxFs_Lfs_Lock;
	me->lfsConfig.unlock = AlxFs_Lfs_Unlock;
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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LFS) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))
