/**
  ******************************************************************************
  * @file		alxMux.c
  * @brief		Auralix C Library - ALX MUX Module
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

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me		Context
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMux_Init(AlxMux* me)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == false);

	// Init GPIO
	AlxIoPin_Init(me->do_nEN);
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_Init((*(me->ioPinArr + i)));
	}

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me		Context
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMux_DeInit(AlxMux* me)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);

	// DeInit GPIO
	AlxIoPin_DeInit(me->do_nEN);
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief					DeInit only select pins, keep enable (me->do_nEN ) pin initialized,
  *							used when there is multiple Muxes,
  *							which uses different enable pin, but the same select pins.
  * @param[in,out]	me		Context
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxMux_DeInit_Select(AlxMux* me)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);

	// DeInit GPIO
	for (uint32_t i = 0; i < me->ioPinArrLen; i++)
	{
		AlxIoPin_DeInit((*(me->ioPinArr + i)));
	}

	// Return
	return Alx_Ok;
}
/**
  * @brief
  * @param[in,out]	me		Context
  * @param[in]		enable	Enable Mux
  */
void AlxMux_Enable(AlxMux* me, bool enable)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);

	// Write GPIO
	AlxIoPin_Write(me->do_nEN, enable);
}
/**
  * @brief
  * @param[in,out]	me		Context
  * @param[in]		ch		Select Mux Channel
  */
void AlxMux_Select(AlxMux* me, Alx_Ch ch)
{
	// Assert
	ALX_MUX_ASSERT(me->wasCtorCalled == true);
	ALX_MUX_ASSERT(me->isInit == true);

	// Write GPIO
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
