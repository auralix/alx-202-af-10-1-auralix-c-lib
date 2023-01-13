/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc17xx.c
  * @brief		Auralix C Library - ALX IO Pin MCU LPC17XX Module
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
#include "alxIoPin_McuLpc17xx.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC17XX)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		port
  * @param[in]		pin
  * @param[in]		mode
  * @param[in]		func
  * @param[in]		isOpenDrain
  * @param[in]		dir
  * @param[in]		val
  */
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	uint32_t mode,
	uint8_t func,
	bool isOpenDrain,
	bool dir,
	bool val)
{
	// Parameters
	me->port = port;
	me->pin = pin;
	me->mode = mode;
	me->func = func;
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
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Init GPIO
	AlxIoPin_Write(me, me->val);  // Set initial output value, before config

	Chip_IOCON_PinMux(LPC_IOCON, me->port, me->pin, me->mode, me->func);
	if (me->isOpenDrain)	{ Chip_IOCON_EnableOD(LPC_IOCON, me->port, me->pin); }
	else					{ Chip_IOCON_DisableOD(LPC_IOCON, me->port, me->pin); }
	Chip_GPIO_SetPinDIR(LPC_GPIO, me->port, me->pin, me->dir);

	AlxIoPin_Write(me, me->val);  // Set initial output value, after config
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_DeInit(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	Chip_IOCON_PinMux(LPC_IOCON, me->port, me->pin, IOCON_MODE_PULLUP, IOCON_FUNC0);
	Chip_IOCON_DisableOD(LPC_IOCON, me->port, me->pin);
	Chip_GPIO_SetPinDIR(LPC_GPIO, me->port, me->pin, false);

	// #2 Clear isInit attribute
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
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	bool val = Chip_GPIO_GetPinState(LPC_GPIO, me->port, me->pin);
	return val;
}

/**
  * @brief
  * @param[in,out] me
  * @param[in]	val
  */
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	(void)val;
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, val);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Set(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, true);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Reset(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinState(LPC_GPIO, me->port, me->pin, false);
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxIoPin_Toggle(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	Chip_GPIO_SetPinToggle(LPC_GPIO, me->port, me->pin);
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
	// TODO
	ALX_IO_PIN_ASSERT(false);
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC17XX)
