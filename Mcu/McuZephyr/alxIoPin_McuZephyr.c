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
  * @param[in]		deviceName
  * @param[in]		pin
  * @param[in]		flags
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	const char* deviceName,
	gpio_pin_t pin,
	gpio_flags_t flags
)
{
	// Parameters
	me->deviceName = deviceName;
	me->pin = pin;
	me->flags = flags;

	// Variables
	me->device = NULL;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Init(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->device == NULL);

	// Init
	me->device = device_get_binding(me->deviceName);
	ALX_IO_PIN_ASSERT(me->device != NULL);

	// Configure
	int32_t status = gpio_pin_configure(me->device, me->pin, me->flags);
	ALX_IO_PIN_ASSERT(status == 0);

	// Set isInit
	me->isInit = true;

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->device != NULL);

	// DeConfigure
	int32_t status = gpio_pin_configure(me->device, me->pin, GPIO_DISCONNECTED);
	ALX_IO_PIN_ASSERT(status == 0);

	// DeInit
	me->device = NULL;

	// Clear isInit
	me->isInit = false;

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxIoPin_Read(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Read
	int32_t valStatus = gpio_pin_get(me->device, me->pin);
	ALX_IO_PIN_ASSERT(valStatus >= 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u val %lu", me->deviceName, me->pin, valStatus);

	// Return
	return valStatus;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		val
  */
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u val %u", me->deviceName, me->pin, val);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Write
	int32_t status = gpio_pin_set(me->device, me->pin, val);
	ALX_IO_PIN_ASSERT(status == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Set(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Set
	int32_t status = gpio_pin_set(me->device, me->pin, 1);
	ALX_IO_PIN_ASSERT(status == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Reset(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Reset
	int32_t status = gpio_pin_set(me->device, me->pin, 0);
	ALX_IO_PIN_ASSERT(status == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Toggle
	int32_t status = gpio_pin_toggle(me->device, me->pin);
	ALX_IO_PIN_ASSERT(status == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			AlxIoPin_TriState_HiZ
  * @retval			AlxIoPin_TriState_Hi
  * @retval			AlxIoPin_TriState_Lo
  * @retval			AlxIoPin_TriState_Undefined
  */
AlxIoPin_TriState AlxIoPin_Read_TriState(AlxIoPin* me)
{
	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_IO_PIN_TRACE_DBG("ENTER: port %s pin %u", me->deviceName, me->pin);


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	int32_t status = gpio_pin_configure(me->device, me->pin, GPIO_PULL_UP);
	ALX_IO_PIN_ASSERT(status == 0);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	status = gpio_pin_configure(me->device, me->pin, GPIO_PULL_DOWN);
	ALX_IO_PIN_ASSERT(status == 0);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullDown = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	AlxIoPin_TriState triState = AlxIoPin_TriState_Undefined;
	if ((valPullUp == true) && (valPullDown == false))
	{
		triState = AlxIoPin_TriState_HiZ;
	}
	else if ((valPullUp == true) && (valPullDown == true))
	{
		triState = AlxIoPin_TriState_Hi;
	}
	else if ((valPullUp == false) && (valPullDown == false))
	{
		triState = AlxIoPin_TriState_Lo;
	}
	else
	{
		triState = AlxIoPin_TriState_Undefined;
	}


	//------------------------------------------------------------------------------
	// Trace
	//------------------------------------------------------------------------------
	ALX_IO_PIN_TRACE_DBG("EXIT: port %s pin %u triState %u", me->deviceName, me->pin, triState);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return triState;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
