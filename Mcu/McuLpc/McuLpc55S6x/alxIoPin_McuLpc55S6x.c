﻿/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX IO Pin MCU LPC55S6X Module
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
#include "alxIoPin_McuLpc55S6x.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIoPin_ResetIocon(AlxIoPin* me);
static void AlxIoPin_SetIoconMode(AlxIoPin* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		func
  * @param[in]		mode
  * @param[in]		mdigiModee
  * @param[in]		isOpenDrain
  * @param[in]		dir
  * @param[in]		val
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	AlxIoPin_Iocon_Func func,
	uint32_t mode,
	bool digiMode,
	bool isOpenDrain,
	bool dir,
	bool val
)
{
	// Assert
	ALX_IO_PIN_ASSERT((port == 0) || (port == 1));
	ALX_IO_PIN_ASSERT((0 <= pin) && (pin <= 31));

	// Parameters
	me->port = port;
	me->pin = pin;
	me->func = func;
	me->mode = mode;
	me->digiMode = digiMode;
	me->isOpenDrain = isOpenDrain;
	me->dir = dir;
	me->val = val;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
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
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Set isInit
	me->isInit = true;

	// Set initial output value, before config
	AlxIoPin_Write(me, me->val);

	// Set IOCON Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, me->func);

	// Set IOCON Mode
	AlxIoPin_SetIoconMode(me);

	// Set IOCON DigiMode
	if (me->digiMode)		{ IOCON->PIO[me->port][me->pin] |=  (0x1 << 8U); }
	else					{ IOCON->PIO[me->port][me->pin] &= ~(0x1 << 8U); }

	// Set IOCON Open Drain
	if (me->isOpenDrain)	{ IOCON->PIO[me->port][me->pin] |=  (0x1 << 9U); }
	else					{ IOCON->PIO[me->port][me->pin] &= ~(0x1 << 9U); }

	// Init if GPIO
	if (me->func == AlxIoPin_Func_0_GPIO)
	{
		gpio_pin_config_t gpioConfig;

		if (me->dir)	gpioConfig.pinDirection = kGPIO_DigitalOutput;
		else			gpioConfig.pinDirection = kGPIO_DigitalInput;

		gpioConfig.outputLogic = 0U;	// MF: Clears output bit

		GPIO_PinInit(GPIO, me->port, me->pin, &gpioConfig);
	}

	// Set initial output value, after config
	AlxIoPin_Write(me, me->val);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Reset IOCON
	AlxIoPin_ResetIocon(me);

	// DeInit if GPIO
	if (me->func == AlxIoPin_Func_0_GPIO)
	{
		GPIO->DIR[me->port] &= ~(1U << me->pin);	// MF: Reset Dir (0)
	}

	// Reset isInit
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
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Read Pin
	if (GPIO_PinRead(GPIO, me->port, me->pin) == 1) { return true; }
	else { return false; }
}

/**
  * @brief
  * @param[in,out] me
  * @param[in]	val
  */
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Write Pin
	if (val) { GPIO_PinWrite(GPIO, me->port, me->pin, 1U); }
	else { GPIO_PinWrite(GPIO, me->port, me->pin, 0U); }
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Set(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Set Pin
	GPIO_PortSet(GPIO, me->port, (1U << me->pin));
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Reset(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Reset Pin
	GPIO_PortClear(GPIO, me->port, (1U << me->pin));
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// Toggle Pin
	GPIO_PortToggle(GPIO, me->port, (1U << me->pin));
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
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	IOCON->PIO[me->port][me->pin] &= ~(0x1 << 4U);
	IOCON->PIO[me->port][me->pin] |=  (0x1 << 5U);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	IOCON->PIO[me->port][me->pin] |=  (0x1 << 4U);
	IOCON->PIO[me->port][me->pin] &= ~(0x1 << 5U);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullDown = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Handle Return
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


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxIoPin_ResetIocon(AlxIoPin* me)
{
	// Set IOCON to Default Value (User Manual page 338)
	if		(me->port == 0 && me->pin == 2)		{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0110); }
	else if (me->port == 0 && me->pin == 5)		{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0120); }
	else if (me->port == 0 && me->pin == 11)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0116); }
	else if (me->port == 0 && me->pin == 12)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0126); }
	else if (me->port == 0 && me->pin == 13)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x5000); }
	else if (me->port == 0 && me->pin == 14)	{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x5000); }
	else										{ IOCON_PinMuxSet(IOCON, me->port, me->pin, 0x0000); }
}
static void AlxIoPin_SetIoconMode(AlxIoPin* me)
{
	// Set Iocon Mode
	if (me->mode == IOCON_MODE_INACT)
	{
		IOCON->PIO[me->port][me->pin] &= ~(0x1 << 4U);
		IOCON->PIO[me->port][me->pin] &= ~(0x1 << 5U);
		return;
	}
	if (me->mode == IOCON_MODE_PULLDOWN)
	{
		IOCON->PIO[me->port][me->pin] |=  (0x1 << 4U);
		IOCON->PIO[me->port][me->pin] &= ~(0x1 << 5U);
		return;
	}
	if (me->mode == IOCON_MODE_PULLUP)
	{
		IOCON->PIO[me->port][me->pin] &= ~(0x1 << 4U);
		IOCON->PIO[me->port][me->pin] |=  (0x1 << 5U);
		return;
	}
	if (me->mode == IOCON_MODE_REPEATER)
	{
		IOCON->PIO[me->port][me->pin] |=  (0x1 << 4U);
		IOCON->PIO[me->port][me->pin] |=  (0x1 << 5U);
		return;
	}

	// Assert
	ALX_IO_PIN_ASSERT(false);	// We should never get here
	return;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
