/**
  ******************************************************************************
  * @file		alxIoPin_McuZephyr.c
  * @brief		Auralix C Library - ALX IO Pin MCU Zephyr Module
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
#include "alxIoPin_McuZephyr.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		mode
  * @param[in]		pull
  * @param[in]		speed
  * @param[in]		alternate
  * @param[in]		val
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	const char* port,
	gpio_pin_t pin,
	gpio_flags_t flags
)
{
	// Parameters
	me->port = device_get_binding(port);
	me->pin = pin;
	me->flags = flags;

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
	ALX_IO_PIN_ASSERT(me->port != NULL);
	int32_t status = gpio_pin_configure(me->port, me->pin, me->flags);
	ALX_IO_PIN_ASSERT(status == 0);

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
	int32_t status = gpio_pin_configure(me->port, me->pin, GPIO_DISCONNECTED);
	ALX_IO_PIN_ASSERT(status == 0);

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
	return gpio_pin_get(me->port, me->pin);
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
	int32_t status = gpio_pin_set(me->port, me->pin, val);
	ALX_IO_PIN_ASSERT(status == 0);
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
	int32_t status = gpio_pin_set(me->port, me->pin, 1);
	ALX_IO_PIN_ASSERT(status == 0);
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
	int32_t status = gpio_pin_set(me->port, me->pin, 0);
	ALX_IO_PIN_ASSERT(status == 0);
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
	int32_t status = gpio_pin_toggle(me->port, me->pin);
	ALX_IO_PIN_ASSERT(status == 0);
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
	int32_t status = gpio_pin_configure(me->port, me->pin, GPIO_PULL_UP);
	ALX_IO_PIN_ASSERT(status == 0);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	status = gpio_pin_configure(me->port, me->pin, GPIO_PULL_DOWN);
	ALX_IO_PIN_ASSERT(status == 0);

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


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
