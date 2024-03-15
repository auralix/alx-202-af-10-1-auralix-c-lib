/**
  ******************************************************************************
  * @file		alxLogger.c
  * @brief		Auralix C Library - ALX Logger Module
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
#include "alxLogger.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLogger_Ctor
(
	AlxLogger* me,
	AlxFs* alxFs
)
{
	// Parameters
	me->alxFs = alxFs;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLogger_Init(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == false);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_DeInit(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Handle(AlxLogger* me)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Trace_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Trace_WriteLog(AlxLogger* me, void* data, uint32_t len)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Data_ReadLog(AlxLogger* me, void* data, uint32_t lenMax, uint32_t* lenActual)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxLogger_Data_WriteLog(AlxLogger* me, void* data, uint32_t len)
{
	// Assert
	ALX_LOGGER_ASSERT(me->wasCtorCalled == true);
	ALX_LOGGER_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
