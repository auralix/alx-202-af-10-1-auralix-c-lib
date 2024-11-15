/**
  ******************************************************************************
  * @file		alxMux.c
  * @brief		Auralix C Library - ALX Analog Multiplexer Module
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
#include "alxMux.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMux_Ctor
(
	AlxMux* me,
	AlxIoPin* do_nEN,
	AlxIoPin** ioPinArr,
	uint8_t ioPinArrLen
)
{
	// Assert
	ALX_MUX_ASSERT(ioPinArrLen <= ALX_MUX_IO_PIN_VAL_ARR_LEN);

	// Parameters
	me->do_nEN = do_nEN;
	me->ioPinArr = ioPinArr;
	me->ioPinArrLen = ioPinArrLen;


	// Variables
	memset(me->ioPinValArr, 0, sizeof(me->ioPinValArr));

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxMux_Init(AlxMux* me)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == false);

	// Init GPIO
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}
	AlxIoPin_Init(me->do_nEN);

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxMux_DeInit(AlxMux* me)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}
	AlxIoPin_DeInit(me->do_nEN);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
void AlxMux_Enable(AlxMux* me, bool enable)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------
	if (enable == true)
	{
		AlxIoPin_Reset(me->do_nEN);
	}
	else
	{
		AlxIoPin_Set(me->do_nEN);
	}
}

void AlxMux_Select(AlxMux* me, Alx_Ch ch)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);

	//------------------------------------------------------------------------------
	// Set
	//------------------------------------------------------------------------------
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		if ((uint8_t)ch & (1 << i))
		{
			AlxIoPin_Set((me->ioPinArr[i]));
		}
		else
		{
			AlxIoPin_Reset((me->ioPinArr[i]));
		}
	}
}


#endif	// #if defined(ALX_C_LIB)
