/**
  ******************************************************************************
  * @file		alxRotSw.c
  * @brief		Auralix C Library - ALX Rotary Switch Module
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
#include "alxRotSw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxRotSw_Ctor
(
	AlxRotSw* me,
	AlxIoPin** ioPinArr,
	uint8_t ioPinArrLen,
	AlxRotSw_CodeType codeType
)
{
	// Parameters
	me->ioPinArr = ioPinArr;
	me->ioPinArrLen = ioPinArrLen;
	me->codeType = codeType;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxRotSw_Init(AlxRotSw* me)
{
	// Assert
	ALX_ROT_SW_ASSERT(me->wasCtorCalled == true);
	ALX_ROT_SW_ASSERT(me->isInit == false);

	// Init GPIO
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Set isInit
	me->isInit = true;
}
void AlxRotSw_DeInit(AlxRotSw* me)
{
	// Assert
	ALX_ROT_SW_ASSERT(me->wasCtorCalled == true);
	ALX_ROT_SW_ASSERT(me->isInit == true);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

	// Clear isInit
	me->isInit = false;
}
uint8_t AlxRotSw_GetCode(AlxRotSw* me)
{
	// Assert
	ALX_ROT_SW_ASSERT(me->wasCtorCalled == true);
	ALX_ROT_SW_ASSERT(me->isInit == true);

	// TV: TODO
	ALX_ROT_SW_ASSERT(false);

	// Return
	return 0;
}


#endif	// #if defined(ALX_C_LIB)
