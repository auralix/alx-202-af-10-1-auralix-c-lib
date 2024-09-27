/**
  ******************************************************************************
  * @file		alxFs.h
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
// Include Guard
//******************************************************************************
#ifndef ALX_FS_H
#define ALX_FS_H

#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"
#include "alxMmc.h"
#if defined(ALX_LFS)
#include "lfs.h"
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_FS_MODULE "AlxFs.h"

// Assert //
#if defined(ALX_FS_ASSERT_BKPT_ENABLE)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FS_MODULE, expr)
#elif defined(ALX_FS_ASSERT_TRACE_ENABLE)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FS_MODULE, expr)
#elif defined(ALX_FS_ASSERT_RST_ENABLE)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_RST(ALX_FS_MODULE, expr)
#else
	#define ALX_FS_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_FS_TRACE_ENABLE)
	#ifndef TV_TEST
	#define ALX_FS_TRACE(...) ALX_TRACE_WRN(ALX_FS_MODULE, __VA_ARGS__)
	#else
	#define ALX_FS_TRACE(...) ALX_TRACE_WRN(ALX_FS_MODULE, __VA_ARGS__); if(me->config == AlxFs_Config_Lfs_Mmc) AlxAssert_Rst("file", 0, "fun")
	#endif
	#define ALX_FS_TRACE_FORMAT(...) ALX_TRACE_FORMAT(__VA_ARGS__)
#else
	#define ALX_FS_TRACE(...) do{} while (false)
	#define ALX_FS_TRACE_FORMAT(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxFs_Config_Undefined,
	#if defined(ALX_FATFS)
	AlxFs_Config_Fatfs_Mmc,
	#endif
	#if defined(ALX_LFS)
	AlxFs_Config_Lfs_FlashInt,
	AlxFs_Config_Lfs_Mmc
	#endif
} AlxFs_Config;

typedef enum
{
	AlxFs_File_Seek_Origin_Set,
	AlxFs_File_Seek_Origin_Cur,
	AlxFs_File_Seek_Origin_End
} AlxFs_File_Seek_Origin;

typedef struct
{
	#if defined(ALX_FATFS)
	FIL fatfsFile;
	#endif
	#if defined(ALX_LFS)
	lfs_file_t lfsFile;
	#endif
	bool dummy;
} AlxFs_File;

typedef struct
{
	#if defined(ALX_FATFS)
	DIR fatfsDir;
	#endif
	#if defined(ALX_LFS)
	lfs_dir_t lfsDir;
	#endif
	bool dummy;
} AlxFs_Dir;

typedef struct
{
	#if defined(ALX_FATFS)
	FILINFO fatfsInfo;
	#endif
	#if defined(ALX_LFS)
	struct lfs_info lfsInfo;
	#endif
	bool dummy;
} AlxFs_Info;

typedef struct
{
	// Defines
	#define ALX_FS_BUFF_LEN 256
	#if defined(ALX_FATFS)
	#define ALX_FS_FATFS_PATH "0:/"
	#define ALX_FS_FATFS_MKFS_BUFF_LEN 512
	#endif

	// Parameters
	AlxFs_Config config;
	AlxMmc* alxMmc;
	AlxIoPin* do_DBG_ReadBlock;
	AlxIoPin* do_DBG_WriteBlock;
	AlxIoPin* do_DBG_EraseBlock;
	AlxIoPin* do_DBG_SyncBlock;

	// Variables
	#if defined(ALX_FATFS)
	FATFS fatfs;
	MKFS_PARM fatfsMkfsOpt;
	uint8_t fatfsMkfsBuff[ALX_FS_FATFS_MKFS_BUFF_LEN];
	#endif
	#if defined(ALX_LFS)
	lfs_t lfs;
	struct lfs_config lfsConfig;
	uint32_t lfsAddr;
	#endif

	// Info
	bool wasCtorCalled;
	bool isMounted;
} AlxFs;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFs_Ctor
(
	AlxFs* me,
	AlxFs_Config config,
	AlxMmc* alxMmc,
	AlxIoPin* do_DBG_ReadBlock,
	AlxIoPin* do_DBG_WriteBlock,
	AlxIoPin* do_DBG_EraseBlock,
	AlxIoPin* do_DBG_SyncBlock
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFs_Mount(AlxFs* me);
Alx_Status AlxFs_UnMount(AlxFs* me);
Alx_Status AlxFs_MountFormat(AlxFs* me);
Alx_Status AlxFs_Format(AlxFs* me);
Alx_Status AlxFs_Remove(AlxFs* me, const char* path);
Alx_Status AlxFs_Rename(AlxFs* me, const char* pathOld, const char* pathNew);
Alx_Status AlxFs_File_Open(AlxFs* me, AlxFs_File* file, const char* path, const char* mode);
Alx_Status AlxFs_File_Close(AlxFs* me, AlxFs_File* file);
Alx_Status AlxFs_File_Read(AlxFs* me, AlxFs_File* file, void* data, uint32_t len, uint32_t* lenActual);
Alx_Status AlxFs_File_ReadStrUntil(AlxFs* me, AlxFs_File* file, char* str, const char* delim, uint32_t len, uint32_t* lenActual);
Alx_Status AlxFs_File_Write(AlxFs* me, AlxFs_File* file, void* data, uint32_t len);
Alx_Status AlxFs_File_WriteStr(AlxFs* me, AlxFs_File* file, const char* str);
Alx_Status AlxFs_File_Sync(AlxFs* me, AlxFs_File* file);
Alx_Status AlxFs_File_Seek(AlxFs* me, AlxFs_File* file, int32_t offset, AlxFs_File_Seek_Origin origin, uint32_t* positionNew);
Alx_Status AlxFs_File_Tell(AlxFs* me, AlxFs_File* file, uint32_t* position);
Alx_Status AlxFs_File_Rewind(AlxFs* me, AlxFs_File* file);
Alx_Status AlxFs_File_Size(AlxFs* me, AlxFs_File* file, uint32_t* size);
Alx_Status AlxFs_File_Truncate(AlxFs* me, AlxFs_File* file, uint32_t size);
Alx_Status AlxFs_File_ReadInChunks(AlxFs* me, const char* path, uint8_t* chunkBuff, uint32_t chunkLen, Alx_Status(*chunkRead_Callback)(void* chunkData, uint32_t chunkLenActual));
Alx_Status AlxFs_File_Trace(AlxFs* me, const char* path);
Alx_Status AlxFs_Dir_Make(AlxFs* me, const char* path);
Alx_Status AlxFs_Dir_Open(AlxFs* me, AlxFs_Dir* dir, const char* path);
Alx_Status AlxFs_Dir_Close(AlxFs* me, AlxFs_Dir* dir);
Alx_Status AlxFs_Dir_Read(AlxFs* me, AlxFs_Dir* dir, AlxFs_Info* info);
Alx_Status AlxFs_Dir_Trace(AlxFs* me, const char* path, bool fileTrace);


#endif	// #if defined(ALX_C_LIB)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FS_H
