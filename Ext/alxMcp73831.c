/**
  ******************************************************************************
  * @file		alxMcp73831.c
  * @brief		Auralix C Library - ALX Battery Charger MCP73831 Module
  * @copyright	Copyright (C) 2020-2022 Auralix d.o.o. All rights reserved.
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
#include <alxMcp73831.h>


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMcp73831_Ctor
(
	AlxMcp73831* me,
	AlxIoPin* di_STAT
)
{
	// Parameters
	me->di_STAT = di_STAT;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxMcp73831_Init(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == false);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Init GPIO
	AlxIoPin_Init(me->di_STAT);

	// #3 Set isInit
	me->isInit = true;
}
void AlxMcp73831_DeInit(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 DeInit GPIO
	AlxIoPin_DeInit(me->di_STAT);

	// #3 Reset isInit
	me->isInit = false;
}
bool AlxMcp73831_IsBatCharging(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	if (AlxIoPin_Read(me->di_STAT) == false)
		return true;
	else
		return false;
}
bool AlxMcp73831_IsBatFull(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	if (AlxIoPin_Read(me->di_STAT) == true)
		return true;
	else
		return false;
}
bool AlxMcp73831_TriState_IsBatCharging(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	if (AlxIoPin_Read_TriState(me->di_STAT) == AlxIoPin_TriState_Lo)
		return true;
	else
		return false;
}
bool AlxMcp73831_TriState_IsBatFull(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	if (AlxIoPin_Read_TriState(me->di_STAT) == AlxIoPin_TriState_Hi)
		return true;
	else
		return false;
}
bool AlxMcp73831_TriState_IsShutdown(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	if (AlxIoPin_Read_TriState(me->di_STAT) == AlxIoPin_TriState_HiZ)
		return true;
	else
		return false;
}
