/**
  ******************************************************************************
  * @file		alxMcp73831.c
  * @brief		Auralix C Library - ALX Battery Charger MCP73831 Module
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
#include "alxMcp73831.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		di_STAT
  * @param[in]		di_STAT_TriState
  */
void AlxMcp73831_Ctor
(
	AlxMcp73831* me,
	AlxIoPin* di_STAT,
	bool di_STAT_TriStateReadEnable
)
{
	// Parameters
	me->di_STAT = di_STAT;
	me->di_STAT_TriStateReadEnable = di_STAT_TriStateReadEnable;

	// Variables
	me->st = AlxMcp73831_St_Err;
	me->di_STAT_TriStateVal = AlxIoPin_TriState_Undefined;
	me->di_STAT_Val = true;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxMcp73831_Init(AlxMcp73831* me)
{
	// Assert
	ALX_MCP73831_ASSERT(me->isInit == false);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// Init GPIO
	AlxIoPin_Init(me->di_STAT);

	// Set isInit
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxMcp73831_DeInit(AlxMcp73831* me)
{
	// Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// DeInit GPIO
	AlxIoPin_DeInit(me->di_STAT);

	// Reset isInit
	me->isInit = false;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			AlxMcp73831_St_Err
  * @retval			AlxMcp73831_St_Shutdown
  * @retval			AlxMcp73831_St_Charging
  * @retval			AlxMcp73831_St_Standby
  */
AlxMcp73831_St AlxMcp73831_GetSt(AlxMcp73831* me)
{
	// Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// Read GPIO
	if (me->di_STAT_TriStateReadEnable)
	{
		// If tri-state logic read
		me->di_STAT_TriStateVal = AlxIoPin_Read_TriState(me->di_STAT);

		// Handle result
		if (me->di_STAT_TriStateVal == AlxIoPin_TriState_Lo)
		{
			me->st = AlxMcp73831_St_Charging;
		}
		else if (me->di_STAT_TriStateVal == AlxIoPin_TriState_Hi)
		{
			me->st = AlxMcp73831_St_Standby;
		}
		else if (me->di_STAT_TriStateVal == AlxIoPin_TriState_HiZ)
		{
			me->st = AlxMcp73831_St_Shutdown;
		}
		else if (me->di_STAT_TriStateVal == AlxIoPin_TriState_Undefined)
		{
			me->st = AlxMcp73831_St_Err;
		}
		else
		{
			ALX_MCP73831_ASSERT(false);	// We should never get here
		}
	}
	else
	{
		// If binary logic read
		me->di_STAT_Val = AlxIoPin_Read(me->di_STAT);

		// Handle result
		if (me->di_STAT_Val)
		{
			me->st = AlxMcp73831_St_Standby;
		}
		else
		{
			me->st = AlxMcp73831_St_Charging;
		}
	}

	// Return
	return me->st;
}


#endif	// #if defined(ALX_C_LIB)
