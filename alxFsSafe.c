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


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFsSafe_Ctor
(
	AlxFsSafe* me,
	AlxFs* alxFs
)
{
	// Parameters
	me->alxFs = alxFs;

	// Variables

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxFsSafe_File_Read(AlxFsSafe* me, const char* path, void* data, uint32_t len, uint32_t* lenActual)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FS_SAFE_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return Alx_Ok;
}
Alx_Status AlxFsSafe_File_Write(AlxFsSafe* me, const char* path, void* data, uint32_t len)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_FS_SAFE_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************


#endif	// #if defined(ALX_C_LIB)
