/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc55S6x.c
  * @brief		Auralix C Library - ALX IO Pin MCU LPC55S6X Module
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
	(void)me;
	ALX_IO_PIN_ASSERT((port == 0) || (port == 1));
	ALX_IO_PIN_ASSERT((0 <= pin) && (pin <= 31));
	(void)func;
	(void)mode;
	(void)digiMode;
	(void)isOpenDrain;
	(void)dir;
	(void)val;

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
void AlxIoPin_Init(AlxIoPin* me)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Set isInit
	me->isInit = true;

	// #3 Set initial output value, before config
	AlxIoPin_Write(me, me->val);

	// #4 Set IOCON
	// #4.1 Set IOCON Func
	IOCON_PinMuxSet(IOCON, me->port, me->pin, me->func);

	// #4.2 Set IOCON Mode
	AlxIoPin_SetIoconMode(me);

	// #4.3 Set IOCON DigiMode
	if (me->digiMode)		{ IOCON->PIO[me->port][me->pin] |=  (0x1 << 8U); }
	else					{ IOCON->PIO[me->port][me->pin] &= ~(0x1 << 8U); }

	// #4.4 Set IOCON Open Drain
	if (me->isOpenDrain)	{ IOCON->PIO[me->port][me->pin] |=  (0x1 << 9U); }
	else					{ IOCON->PIO[me->port][me->pin] &= ~(0x1 << 9U); }

	// #5 Init if GPIO
	if (me->func == AlxIoPin_Func_0_GPIO)
	{
		gpio_pin_config_t gpioConfig;

		if (me->dir)	gpioConfig.pinDirection = kGPIO_DigitalOutput;
		else			gpioConfig.pinDirection = kGPIO_DigitalInput;

		gpioConfig.outputLogic = 0U;	// MF: Clears output bit

		GPIO_PinInit(GPIO, me->port, me->pin, &gpioConfig);
	}

	// #6 Set initial output value, after config
	AlxIoPin_Write(me, me->val);
}
void AlxIoPin_DeInit(AlxIoPin* me)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Reset IOCON
	AlxIoPin_ResetIocon(me);

	// #3 DeInit if GPIO
	if (me->func == AlxIoPin_Func_0_GPIO)
	{
		GPIO->DIR[me->port] &= ~(1U << me->pin);	// MF: Reset Dir (0)
	}

	// #4 Reset isInit
	me->isInit = false;
}
bool AlxIoPin_Read(AlxIoPin* me)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Read Pin
	if (GPIO_PinRead(GPIO, me->port, me->pin) == 1) { return true; }
	else { return false; }
}
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Write Pin
	if (val) { GPIO_PinWrite(GPIO, me->port, me->pin, 1U); }
	else { GPIO_PinWrite(GPIO, me->port, me->pin, 0U); }
}
void AlxIoPin_Set(AlxIoPin* me)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Set Pin
	GPIO_PortSet(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Reset(AlxIoPin* me)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Reset Pin
	GPIO_PortClear(GPIO, me->port, (1U << me->pin));
}
void AlxIoPin_Toggle(AlxIoPin* me)
{
	// #1 Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #2 Toggle Pin
	GPIO_PortToggle(GPIO, me->port, (1U << me->pin));
}
AlxIoPin_TriState AlxIoPin_Read_TriState(AlxIoPin* me)
{
	// Assert
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	//------------------------------------------------------------------------------
	// #1 Read @ PullUp
	//------------------------------------------------------------------------------

	// Config PullUp
	IOCON->PIO[me->port][me->pin] &= ~(0x1 << 4U);
	IOCON->PIO[me->port][me->pin] |=  (0x1 << 5U);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);

	//------------------------------------------------------------------------------
	// #2 Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	IOCON->PIO[me->port][me->pin] |=  (0x1 << 4U);
	IOCON->PIO[me->port][me->pin] &= ~(0x1 << 5U);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullDown = AlxIoPin_Read(me);

	//------------------------------------------------------------------------------
	// #3 Handle Return
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
	// #1 Set IOCON to Default Value (User Manual page 338)
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
	// #1 Set Iocon Mode
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

	// #2 Assert
	ALX_IO_PIN_ASSERT(false);	// We should never get here
	return;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC55S6X)
