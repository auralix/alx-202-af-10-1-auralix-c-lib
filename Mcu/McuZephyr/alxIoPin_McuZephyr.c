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
  * @param[in]		gpioFlags
  * @param[in]		irqFlags
  * @param[in]		irqCb
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	const char* deviceName,
	gpio_pin_t pin,
	gpio_flags_t gpioFlags,
	gpio_flags_t irqFlags,
	gpio_callback_handler_t irqCb
)
{
	// Parameters
	me->deviceName = deviceName;
	me->pin = pin;
	me->gpioFlags = gpioFlags;
	me->irqFlags = irqFlags;
	me->irqCb = irqCb;

	// Variables
	me->device = NULL;
	memset(&me->irqStruct, 0, sizeof(me->irqStruct));

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
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->device == NULL);

	// Init GPIO
	me->device = device_get_binding(me->deviceName);
	ALX_IO_PIN_ASSERT(me->device != NULL);

	// Configure GPIO
	ALX_IO_PIN_ASSERT(gpio_pin_configure(me->device, me->pin, me->gpioFlags) == 0);

	// Init IRQ
	if (me->irqFlags != 0 && me->irqCb != NULL)
	{
		// Configure IRQ
		ALX_IO_PIN_ASSERT(gpio_pin_interrupt_configure(me->device, me->pin, me->irqFlags) == 0);

		// Init IRQ Callback
		gpio_init_callback(&me->irqStruct, me->irqCb, BIT(me->pin));

		// Add IRQ Callback - Enables IRQ
		ALX_IO_PIN_ASSERT(gpio_add_callback(me->device, &me->irqStruct) == 0);
	}

	// Set isInit
	me->isInit = true;

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->device != NULL);

	// DeInit IRQ
	if (me->irqFlags != 0 && me->irqCb != NULL)
	{
		// DeConfigure IRQ
		ALX_IO_PIN_ASSERT(gpio_pin_interrupt_configure(me->device, me->pin, GPIO_INT_DISABLE) == 0);

		// Remove IRQ Callback
		ALX_IO_PIN_ASSERT(gpio_remove_callback(me->device, &me->irqStruct) == 0);

		// Clear irqStruct
		memset(&me->irqStruct, 0, sizeof(me->irqStruct));
	}

	// DeConfigure GPIO
	ALX_IO_PIN_ASSERT(gpio_pin_configure(me->device, me->pin, GPIO_DISCONNECTED) == 0);

	// DeInit GPIO
	me->device = NULL;

	// Clear isInit
	me->isInit = false;

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
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
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Read
	int32_t valStatus = gpio_pin_get(me->device, me->pin);
	ALX_IO_PIN_ASSERT(valStatus >= 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u val %ld", me->deviceName, me->pin, valStatus);

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
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u val %u", me->deviceName, me->pin, val);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Write
	ALX_IO_PIN_ASSERT(gpio_pin_set(me->device, me->pin, val) == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Set(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Set
	ALX_IO_PIN_ASSERT(gpio_pin_set(me->device, me->pin, 1) == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Reset(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Reset
	ALX_IO_PIN_ASSERT(gpio_pin_set(me->device, me->pin, 0) == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);

	// Toggle
	ALX_IO_PIN_ASSERT(gpio_pin_toggle(me->device, me->pin) == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
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
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);


	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);


	//------------------------------------------------------------------------------
	// Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	ALX_IO_PIN_ASSERT(gpio_pin_configure(me->device, me->pin, GPIO_PULL_UP) == 0);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	ALX_IO_PIN_ASSERT(gpio_pin_configure(me->device, me->pin, GPIO_PULL_DOWN) == 0);

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
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u triState %lu", me->deviceName, me->pin, triState);


	//------------------------------------------------------------------------------
	// Return
	//------------------------------------------------------------------------------
	return triState;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_DisableIrq(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->irqFlags != 0);
	ALX_IO_PIN_ASSERT(me->irqCb != NULL);

	// DeConfigure IRQ
	ALX_IO_PIN_ASSERT(gpio_pin_interrupt_configure(me->device, me->pin, GPIO_INT_DISABLE) == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxIoPin_EnableIrq(AlxIoPin* me)
{
	// Trace
	ALX_IO_PIN_TRACE_DBG("ENTER: deviceName %s pin %u", me->deviceName, me->pin);

	// Assert
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->irqFlags != 0);
	ALX_IO_PIN_ASSERT(me->irqCb != NULL);

	// Configure IRQ
	ALX_IO_PIN_ASSERT(gpio_pin_interrupt_configure(me->device, me->pin, me->irqFlags) == 0);

	// Trace
	ALX_IO_PIN_TRACE_DBG("EXIT: deviceName %s pin %u", me->deviceName, me->pin);
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_ZEPHYR)
