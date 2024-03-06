/**
  ******************************************************************************
  * @file		alxParamKvStore.c
  * @brief		Auralix C Library - ALX Parameter Key-Value Store Module
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
#include "alxParamKvStore.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxParamKvStore_Ctor
(
	AlxParamKvStore* me,
	AlxFs* fs
)
{
	// Parameters
	me->fs = fs;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxParamKvStore_Init(AlxParamKvStore* me)
{
	// Assert
	ALX_PARAM_KV_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_KV_STORE_ASSERT(me->isInit == false);

	// Try to mount FS, if error, then format & mount
	Alx_Status status = AlxFs_Mount(me->fs);
	if (status != Alx_Ok)
	{
		// Format FS
		status = AlxFs_Format(me->fs);
		if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

		// Mount FS
		status = AlxFs_Mount(me->fs);
		if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }
	}

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxParamKvStore_DeInit(AlxParamKvStore* me)
{
	// Assert
	ALX_PARAM_KV_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_KV_STORE_ASSERT(me->isInit == true);

	// UnMount FS
	Alx_Status status = AlxFs_UnMount(me->fs);
	if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxParamKvStore_Get(AlxParamKvStore* me, const char* key, void* buff, uint32_t len, uint32_t* actualLen)
{
	// Assert
	ALX_PARAM_KV_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_KV_STORE_ASSERT(me->isInit == true);

	// Local variables
	AlxFs_File file;
	Alx_Status status = Alx_Err;
	int64_t statusActualLen = -1;

	// Open File
	status = AlxFs_File_Open(me->fs, &file, key, "r");
	if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

	// Read File
	statusActualLen = AlxFs_File_Read(me->fs, &file, buff, len);
	if(statusActualLen < 0) { ALX_PARAM_KV_STORE_TRACE("Err"); return Alx_Err; }

	// Close File
	status = AlxFs_File_Close(me->fs, &file);
	if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

	// Return
	*actualLen = (uint32_t)statusActualLen;
	return Alx_Ok;
}
Alx_Status AlxParamKvStore_Set(AlxParamKvStore* me, const char* key, void* buff, uint32_t len)
{
	// Assert
	ALX_PARAM_KV_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_KV_STORE_ASSERT(me->isInit == true);

	// Local variables
	AlxFs_File file;
	Alx_Status status = Alx_Err;
	int64_t statusActualLen = -1;

	// Open File
	status = AlxFs_File_Open(me->fs, &file, key, "w");
	if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

	// Write File
	statusActualLen = AlxFs_File_Write(me->fs, &file, buff, len);
	if(statusActualLen != len) { ALX_PARAM_KV_STORE_TRACE("Err"); return Alx_Err; }

	// Close File
	status = AlxFs_File_Close(me->fs, &file);
	if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}
Alx_Status AlxParamKvStore_Remove(AlxParamKvStore* me, const char* key)
{
	// Assert
	ALX_PARAM_KV_STORE_ASSERT(me->wasCtorCalled == true);
	ALX_PARAM_KV_STORE_ASSERT(me->isInit == true);

	// Remove FS
	Alx_Status status = AlxFs_Remove(me->fs, key);
	if(status != Alx_Ok) { ALX_PARAM_KV_STORE_TRACE("Err"); return status; }

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
