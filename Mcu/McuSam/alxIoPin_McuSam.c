/**
  ******************************************************************************
  * @file		alxIoPin_McuSam.c
  * @brief		Auralix C Library - ALX IO Pin MCU SAM Module
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
#include "alxIoPin_McuSam.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		portPin
  * @param[in]		dir
  * @param[in]		pull
  * @param[in]		func
  * @param[in]		val
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t portPin,
	enum gpio_direction dir,
	enum gpio_pull_mode pull,
	uint32_t func,
	bool val
)
{
	// Parameters
	me->portPin = portPin;
	me->dir = dir;
	me->pull = pull;
	me->func = func;
	me->val = val;

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
void AlxIoPin_Init(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == false);

	// Init
	gpio_set_pin_level(me->portPin, me->val);
	gpio_set_pin_direction(me->portPin, me->dir);
	gpio_set_pin_pull_mode(me->portPin, me->pull);
	gpio_set_pin_function(me->portPin, me->func);

	// Set isInit
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// DeInit
	gpio_set_pin_direction(me->portPin, GPIO_DIRECTION_OFF);
	gpio_set_pin_pull_mode(me->portPin, GPIO_PULL_OFF);
	gpio_set_pin_function(me->portPin, GPIO_PIN_FUNCTION_OFF);

	// Clear isInit
	me->isInit = false;
}

/**
  * @brief
  * @param[in,out] me
  * @retval		false
  * @retval		true
  */
bool AlxIoPin_Read(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Read
	return gpio_get_pin_level(me->portPin);
}

/**
  * @brief
  * @param[in,out] me
  * @param[in]	val
  */
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Write
	gpio_set_pin_level(me->portPin, val);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Set(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Set
	gpio_set_pin_level(me->portPin, true);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Reset(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Reset
	gpio_set_pin_level(me->portPin, false);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Toggle
	gpio_toggle_pin_level(me->portPin);
}

/**
  * @brief
  * @param[in,out] me
  * @retval			AlxIoPin_TriState_HiZ
  * @retval			AlxIoPin_TriState_Hi
  * @retval			AlxIoPin_TriState_Lo
  * @retval			AlxIoPin_TriState_Undefined
  */
AlxIoPin_TriState AlxIoPin_Read_TriState(AlxIoPin* me)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	gpio_set_pin_pull_mode(me->portPin, GPIO_PULL_UP);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	gpio_set_pin_pull_mode(me->portPin, GPIO_PULL_DOWN);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullDown = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	if ((valPullUp == true) && (valPullDown == false))
	{
		return AlxIoPin_TriState_HiZ;
	}
	else if ((valPullUp == true) && (valPullDown == true))
	{
		return AlxIoPin_TriState_Hi;
	}
	else if ((valPullUp == false) && (valPullDown == false))
	{
		return AlxIoPin_TriState_Lo;
	}
	else
	{
		return AlxIoPin_TriState_Undefined;
	}
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)
