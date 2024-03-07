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
#include "alxMmc.h"
#if defined(ALX_LFS)
#include "lfs.h"
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_FS_FILE "alxFs.h"

// Assert //
#if defined(ALX_FS_ASSERT_BKPT_ENABLE)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FS_FILE, expr)
#elif defined(ALX_FS_ASSERT_TRACE_ENABLE)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FS_FILE, expr)
#elif defined(ALX_FS_ASSERT_RST_ENABLE)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_RST(ALX_FS_FILE, expr)
#else
	#define ALX_FS_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(ALX_FS_TRACE_ENABLE)
	#define ALX_FS_TRACE(...) ALX_TRACE_STD(ALX_FS_FILE, __VA_ARGS__)
#else
	#define ALX_FS_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef enum
{
	AlxFs_Config_Lfs_FlashInt,
	AlxFs_Config_Lfs_Mmc
} AlxFs_Config;

typedef struct
{
	#if defined(ALX_LFS)
	lfs_file_t lsfFile;
	#endif
	bool dummy;
} AlxFs_File;

typedef struct
{
	// Parameters
	AlxFs_Config config;
	AlxMmc* alxMmc;

	// Variables
	#if defined(ALX_LFS)
	lfs_t lfs;
	struct lfs_config lfsConfig;
	#endif
	uint32_t lfsAddr;

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
	AlxMmc* alxMmc
);


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFs_Mount(AlxFs* me);
Alx_Status AlxFs_UnMount(AlxFs* me);
Alx_Status AlxFs_Format(AlxFs* me);
Alx_Status AlxFs_Remove(AlxFs* me, const char* path);
Alx_Status AlxFs_File_Open(AlxFs* me, AlxFs_File* file, const char* path, const char* mode);
Alx_Status AlxFs_File_Close(AlxFs* me, AlxFs_File* file);
int32_t AlxFs_File_Read(AlxFs* me, AlxFs_File* file, void* buff, uint32_t len);
int32_t AlxFs_File_Write(AlxFs* me, AlxFs_File* file, void* buff, uint32_t len);
Alx_Status AlxFs_File_Sync(AlxFs* me, AlxFs_File* file);
int32_t AlxFs_File_Seek(AlxFs* me, AlxFs_File* file, int32_t offset, int32_t whence);
int32_t AlxFs_File_Tell(AlxFs* me, AlxFs_File* file);
int32_t AlxFs_File_Size(AlxFs* me, AlxFs_File* file);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FS_H
