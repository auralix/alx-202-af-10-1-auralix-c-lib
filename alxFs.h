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
#if defined(ALX_LFS)
#include "lfs.h"
#else
typedef struct { bool dummy; } AlxFs;
#endif


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LFS) && (defined(ALX_STM32F4) || defined(ALX_STM32F7))


//******************************************************************************
// Preprocessor
//******************************************************************************
#define ALX_FS_FILE "alxFs.h"

// Assert //
#if defined(_ALX_FS_ASSERT_BKPT) || defined(_ALX_ASSERT_BKPT_ALL)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_BKPT(ALX_FS_FILE, expr)
#elif defined(_ALX_FS_ASSERT_TRACE) || defined(_ALX_ASSERT_TRACE_ALL)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_TRACE(ALX_FS_FILE, expr)
#elif defined(_ALX_FS_ASSERT_RST) || defined(_ALX_ASSERT_RST_ALL)
	#define ALX_FS_ASSERT(expr) ALX_ASSERT_RST(ALX_FS_FILE, expr)
#else
	#define ALX_FS_ASSERT(expr) do{} while (false)
#endif

// Trace //
#if defined(_ALX_FS_TRACE) || defined(_ALX_TRACE_ALL)
	#define ALX_FS_TRACE(...) ALX_TRACE_STD(ALX_FS_FILE, __VA_ARGS__)
#else
	#define ALX_FS_TRACE(...) do{} while (false)
#endif


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	lfs_file_t lsfFile;
} AlxFs_File;

typedef struct
{
	// Variables
	lfs_t lfs;
	uint32_t lfsAddr;
	struct lfs_config lfsConfig;

	// Info
	bool wasCtorCalled;
	bool isMounted;
} AlxFs;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFs_Ctor
(
	AlxFs* me
);


//******************************************************************************
// Functions
//******************************************************************************
int32_t AlxFs_Mount(AlxFs* me);
int32_t AlxFs_UnMount(AlxFs* me);
int32_t AlxFs_Format(AlxFs* me);
int32_t AlxFs_Remove(AlxFs* me, const char* path);
int32_t AlxFs_FileOpen(AlxFs* me, AlxFs_File* file, const char* path, int32_t flags);
int32_t AlxFs_FileClose(AlxFs* me, AlxFs_File* file);
int32_t AlxFs_FileRead(AlxFs* me, AlxFs_File* file, void* buff, uint32_t len);
int32_t AlxFs_FileWrite(AlxFs* me, AlxFs_File* file, void* buff, uint32_t len);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LFS) && (defined(ALX_STM32F4) || defined(ALX_STM32F7))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_FS_H
