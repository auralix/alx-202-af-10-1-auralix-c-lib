/**
  ******************************************************************************
  * @file		alxFsSafe.c
  * @brief		Auralix C Library - ALX File System Safe Module
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
#include "alxFsSafe.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
Alx_Status AlxFsSafe_File_ReadCopy(AlxFsSafe* me, bool isA, const char* path, void* data, uint32_t len, uint32_t* validatedCrc);
Alx_Status AlxFsSafe_File_WriteCopy(AlxFsSafe* me, bool isA, const char* path, void* data, uint32_t len, uint8_t* buff);
void AlxFsSafe_PathToPathWithSuffix(bool isA, const char* path, const char* pathWithSuffix);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFsSafe_Ctor
(
	AlxFsSafe* me,
	AlxFs* alxFs,
	uint8_t* buffA,
	uint8_t* buffB,
	uint32_t buffLen
)
{
	// Parameters
	me->alxFs = alxFs;
	me->buffA = buffA;
	me->buffB = buffB;
	me->buffLen = buffLen;

	// Variables
	AlxCrc_Ctor(&me->alxCrc, AlxCrc_Config_Ccitt);
	me->alxCrcLen = AlxCrc_GetLen(&me->alxCrc);

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFsSafe_File_Read(AlxFsSafe* me, const char* path, void* data, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FS_SAFE_ASSERT(me->wasCtorCalled == true);
	ALX_FS_SAFE_ASSERT(strlen(path) <= ALX_FS_SAFE_PATH_LEN_MAX);
	ALX_FS_SAFE_ASSERT(len <= me->buffLen - me->alxCrcLen);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	bool validA = false;
	bool validB = false;
	uint32_t crcA = 0;
	uint32_t crcB = 0;
	memset(me->buffA, 0, me->buffLen);
	memset(me->buffB, 0, me->buffLen);


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------

	// Copy A
	status = AlxFsSafe_File_ReadCopy(me, true, path, me->buffA, len, &crcA);
	if (status == Alx_Ok)
	{
		validA = true;
	}
	else
	{
		validA = false;
	}

	// Copy B
	status = AlxFsSafe_File_ReadCopy(me, false, path, me->buffB, len, &crcB);
	if (status == Alx_Ok)
	{
		validB = true;
	}
	else
	{
		validB = false;
	}


	//------------------------------------------------------------------------------
	// Check Validity
	//------------------------------------------------------------------------------
	if (validA == false && validB == false)	// Both Copy ERR
	{
		// Trace
		ALX_FS_SAFE_TRACE_INF("AlxFsSafe_File_Read - DONE: CheckValidity() 'Both Copy ERR'");

		// Status
		status = AlxSafe_BothCopyErr;
	}
	else if (validA && validB && crcA == crcB)	// Both Copy OK & CRC Same -> Use CopyA
	{
		// Trace
		ALX_FS_SAFE_TRACE_INF("AlxFsSafe_File_Read - DONE: CheckValidity() 'Both Copy OK & CRC Same -> Use CopyA'");

		// Use CopyA
		memcpy(data, me->buffA, len);

		// Status
		status = AlxSafe_BothCopyOkCrcSame_UsedCopyA;
	}
	else if (validA && validB && crcA != crcB)	// Both Copy OK & CRC Different -> Use CopyA and update CopyB with CopyA, CopyA is used because we always write CopyA first and update
	{
		// Trace
		ALX_FS_SAFE_TRACE_INF("AlxFsSafe_File_Read - DONE: CheckValidity() 'Both Copy OK & CRC Different -> Use CopyA'");

		// Update CopyB with CopyA
		status = AlxFsSafe_File_WriteCopy(me, false, path, me->buffA, len, me->buffB);
		if (status != Alx_Ok)
		{
			ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFsSafe_File_WriteCopy(B) status %d", status);
			return status;
		}

		// Use CopyA
		memcpy(data, me->buffA, len);

		// Status
		status = AlxSafe_BothCopyOkCrcDiff_UsedCopyA;
	}
	else if (validA && validB == false)	// CopyA OK, CopyB ERR -> Use CopyA and update CopyB with CopyA
	{
		// Trace
		ALX_FS_SAFE_TRACE_INF("AlxFsSafe_File_Read - DONE: CheckValidity() 'CopyA OK, CopyB ERR -> Use CopyA and update CopyB with CopyA'");

		// Update CopyB with CopyA
		status = AlxFsSafe_File_WriteCopy(me, false, path, me->buffA, len, me->buffB);
		if (status != Alx_Ok)
		{
			ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFsSafe_File_WriteCopy(B) status %d", status);
			return status;
		}

		// Use CopyA
		memcpy(data, me->buffA, len);

		// Status
		status = AlxSafe_CopyAOkCopyBErr_UsedCopyA;
	}
	else if (validA == false && validB)	// CopyA ERR, CopyB OK -> Use CopyB and update CopyA with CopyB
	{
		// Trace
		ALX_FS_SAFE_TRACE_INF("AlxFsSafe_File_Read - DONE: CheckValidity() 'CopyA ERR, CopyB OK -> Use CopyB and update CopyA with CopyB'");

		// Update CopyA with CopyB
		status = AlxFsSafe_File_WriteCopy(me, true, path, me->buffB, len, me->buffA);
		if (status != Alx_Ok)
		{
			ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFsSafe_File_WriteCopy(A) status %d", status);
			return status;
		}

		// Use CopyB
		memcpy(data, me->buffB, len);

		// Status
		status = AlxSafe_CopyAErrCopyBOk_UsedCopyB;
	}
	else
	{
		ALX_FS_SAFE_ASSERT(false);	// We should never get here
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return status;
}
Alx_Status AlxFsSafe_File_Write(AlxFsSafe* me, const char* path, void* data, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FS_SAFE_ASSERT(me->wasCtorCalled == true);
	ALX_FS_SAFE_ASSERT(strlen(path) <= ALX_FS_SAFE_PATH_LEN_MAX);
	ALX_FS_SAFE_ASSERT(len <= me->buffLen - me->alxCrcLen);


	//------------------------------------------------------------------------------
	// Local Variables
	//------------------------------------------------------------------------------
	Alx_Status status = Alx_Err;
	memset(me->buffA, 0, me->buffLen);
	memset(me->buffB, 0, me->buffLen);


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------

	// Copy A
	status = AlxFsSafe_File_WriteCopy(me, true, path, data, len, me->buffA);
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFsSafe_File_WriteCopy(A) status %d", status);
		return status;
	}

	// Copy B
	status = AlxFsSafe_File_WriteCopy(me, false, path, data, len, me->buffB);
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFsSafe_File_WriteCopy(B) status %d", status);
		return status;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
Alx_Status AlxFsSafe_File_ReadCopy(AlxFsSafe* me, bool isA, const char* path, void* data, uint32_t len, uint32_t* validatedCrc)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Path with Suffix
	char pathWithSuffix[ALX_FS_SAFE_PATH_LEN_MAX + 1 + 1] = "";	// path + suffix + termination
	AlxFsSafe_PathToPathWithSuffix(isA, path, pathWithSuffix);

	// Length
	uint32_t lenWithCrc = len + me->alxCrcLen;


	//------------------------------------------------------------------------------
	// Read
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, pathWithSuffix, "r");
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Open(%s) status %d", pathWithSuffix, status);
		return status;
	}

	// Read
	uint32_t lenWithCrcActual = 0;
	status = AlxFs_File_Read(me->alxFs, &file, data, lenWithCrc, &lenWithCrcActual);
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Read(%s) status %d lenWithCrc %u lenWithCrcActual %u", pathWithSuffix, status, lenWithCrc, lenWithCrcActual);
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Close(%s) status %d", pathWithSuffix, statusClose);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Close(%s) status %d", pathWithSuffix, status);
		// TV: TODO - Handle close error
		return status;
	}


	//------------------------------------------------------------------------------
	// Check
	//------------------------------------------------------------------------------

	// Check length
	if (lenWithCrcActual != lenWithCrc)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: CheckLen(%s) lenWithCrcActual %lu lenWithCrc %lu", pathWithSuffix, lenWithCrcActual, lenWithCrc);
		return Alx_Err;
	}

	// Check CRC
	bool isCrcOk = AlxCrc_IsOk(&me->alxCrc, data, lenWithCrc, validatedCrc);
	if (isCrcOk == false)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: CheckCrc(%s) isCrcOk %u lenWithCrc %lu", pathWithSuffix, isCrcOk, lenWithCrc);
		return Alx_Err;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
Alx_Status AlxFsSafe_File_WriteCopy(AlxFsSafe* me, bool isA, const char* path, void* data, uint32_t len, uint8_t* buff)
{
	//------------------------------------------------------------------------------
	// Prepare
	//------------------------------------------------------------------------------

	// Path with Suffix
	char pathWithSuffix[ALX_FS_SAFE_PATH_LEN_MAX + 2 + 1] = "";	// path + suffix + termination
	AlxFsSafe_PathToPathWithSuffix(isA, path, pathWithSuffix);

	// CRC
	uint16_t crc = AlxCrc_Calc(&me->alxCrc, data, len);

	// Data
	memcpy(buff, data, len);
	memcpy(&buff[len], &crc, me->alxCrcLen);

	// Length
	uint32_t lenWithCrc = len + me->alxCrcLen;


	//------------------------------------------------------------------------------
	// Write
	//------------------------------------------------------------------------------

	// Local variables
	Alx_Status status = Alx_Err;
	AlxFs_File file = {};

	// Open
	status = AlxFs_File_Open(me->alxFs, &file, pathWithSuffix, "w");
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Open(%s) status %d", pathWithSuffix, status);
		return status;
	}

	// Write
	status = AlxFs_File_Write(me->alxFs, &file, buff, lenWithCrc);
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Write(%s) status %d lenWithCrc %u", pathWithSuffix, status, lenWithCrc);
		Alx_Status statusClose = AlxFs_File_Close(me->alxFs, &file);
		if (statusClose != Alx_Ok)
		{
			ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Close(%s) status %d", pathWithSuffix, statusClose);
			// TV: TODO - Handle close error
		}
		return status;
	}

	// Close
	status = AlxFs_File_Close(me->alxFs, &file);
	if (status != Alx_Ok)
	{
		ALX_FS_SAFE_TRACE_ERR("FAIL: AlxFs_File_Close(%s) status %d", pathWithSuffix, status);
		// TV: TODO - Handle close error
		return status;
	}


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
void AlxFsSafe_PathToPathWithSuffix(bool isA, const char* path, const char* pathWithSuffix)
{
	// Find dot
	const char* dot = strrchr(path, '.');
	ALX_FS_SAFE_ASSERT(dot != NULL);

	// Set suffix
	const char* suffix = "";
	if (isA)
	{
		suffix = "A";
	}
	else
	{
		suffix = "B";
	}

	// Set prefix length
	uint32_t prefixLen = (uint32_t)(dot - path);

	// Transform
	sprintf(pathWithSuffix, "%.*s%s%s", prefixLen, path, suffix, dot);
}


#endif	// #if defined(ALX_C_LIB)
