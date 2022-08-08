/**
  ******************************************************************************
  * @file		alxTpa3255.c
  * @brief		Auralix C Library - ALX Speaker Amplifier TPA3255 Module
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
#include "alxTpa3255.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTpa3255_Ctor
(
	AlxTpa3255* me,
	AlxIoPin* do_nRESET,
	AlxIoPin* di_nFAULT,
	AlxIoPin* di_nCLIP_OTW,
	float waitTime_ReEnable_ms
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, false);

	// Parameters Const
	me->lowLevelTime_ms = 2;

	// Objects - External
	me->do_nRESET = do_nRESET;
	me->di_nFAULT = di_nFAULT;
	me->di_nCLIP_OTW = di_nCLIP_OTW;

	// Parameters
	me->waitTime_ReEnable_ms = waitTime_ReEnable_ms;
	me->waitTime_Waitning_ms = 2;

	// Variables
	me->state = Tpa3255_St_Ok;
	me->wasErrAsserted = false;
	me->wasWarningAsserted = false;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxTpa3255_Init(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == false);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->do_nRESET);
	AlxIoPin_Init(me->di_nFAULT);
	AlxIoPin_Init(me->di_nCLIP_OTW);

	// #2 Set isInit
	me->isInit = true;
}
void AlxTpa3255_DeInit(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_DeInit(me->do_nRESET);
	AlxIoPin_DeInit(me->di_nFAULT);
	AlxIoPin_DeInit(me->di_nCLIP_OTW);

	// #2 Reset isInit
	me->isInit = false;
}
void AlxTpa3255_Handle(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	if (me->waitTime_ReEnable_ms > 0) // functionality is enabled
	{
		// #1 Ok state
		if(me->state == Tpa3255_St_Ok)
		{
			// Transition
			if (AlxTpa3255_IsErrAsserted(me))
			{
				AlxTimSw_Start(&me->tim); // restart timer timReEnable
				me->state = Tpa3255_St_Error; // set Error
			}
			// Do
			else
			{
				AlxTpa3255_Enable(me);
			}
		}
		// #2 Error state
		else if (me->state == Tpa3255_St_Error)
		{
			// Transition
			if (AlxTimSw_IsTimeout_ms(&me->tim, me->waitTime_ReEnable_ms))
			{
				AlxTimSw_Start(&me->tim); // restart timer timlowLevel
				me->state = Tpa3255_St_LowLevel; // set LowLevel
			}
			// Do
			else
			{
				// Nothing
			}
		}
		// #3 LowLevel state
		else if (me->state == Tpa3255_St_LowLevel)
		{
			// Transition
			if(AlxTimSw_IsTimeout_ms(&me->tim, me->lowLevelTime_ms))
			{
				AlxTimSw_Start(&me->tim); // restart timer Waiting
				me->state = Tpa3255_St_Waiting; // set Waiting
			}
			// Do
			else
			{
				AlxTpa3255_Disable(me);
			}
		}
		// #4 Waiting state
		else if(me->state == Tpa3255_St_Waiting)
		{
			// Transition
			if(AlxTimSw_IsTimeout_ms(&me->tim, me->waitTime_Waitning_ms))
			{
				me->state = Tpa3255_St_Ok; // set Ok
			}
			// Do
			else
			{
				AlxTpa3255_Enable(me);
			}
		}
		// Procesing WAS
		if(AlxTpa3255_IsErrAsserted(me))
		{
			me->wasErrAsserted = true;
		}
		if (AlxTpa3255_IsWarningAsserted(me))
		{
			me->wasWarningAsserted = true;
		}
	}
	else // functionality is disabled -> not try to ReEnable IC
	{
		AlxIoPin_Write(me->do_nRESET, true);
	}
}
void AlxTpa3255_Enable(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Write(me->do_nRESET, true);
}
void AlxTpa3255_Disable(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Write(me->do_nRESET, false);
}
bool AlxTpa3255_IsErrAsserted(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	return (AlxIoPin_Read(me->di_nFAULT)) ? false : true;
}
bool AlxTpa3255_IsWarningAsserted(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	return (AlxIoPin_Read(me->di_nCLIP_OTW)) ? false : true;
}
bool AlxTpa3255_WasErrAsserted(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	return me->wasErrAsserted;
}
bool AlxTpa3255_WasWarningAsserted(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	return me->wasWarningAsserted;
}
void AlxTpa3255_ClearWasErrAsserted(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	me->wasErrAsserted = false;
}
void AlxTpa3255_ClearWasWarningAsserted(AlxTpa3255* me)
{
	ALX_TPA3255_ASSERT(me->isInit == true);
	ALX_TPA3255_ASSERT(me->wasCtorCalled == true);

	me->wasWarningAsserted = false;
}


#endif	// #if defined(ALX_C_LIB)
