/**
  ******************************************************************************
  * @file		alxIoPin_McuStm32.c
  * @brief		Auralix C Library - ALX IO Pin MCU STM32 Module
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
#include "alxIoPin_McuStm32.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))


//******************************************************************************
// Specific Functions
//******************************************************************************
void AlxIoPin_Ctor
(
	AlxIoPin* me,
	GPIO_TypeDef* port,
	uint16_t pin,
	uint32_t mode,
	uint32_t pull,
	uint32_t speed,
	#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	uint32_t alternate,
	#endif // defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	bool val
)
{
	// Parameters
	me->port = port;
	me->val = val;

	// Variables
	me->igpio.Pin = pin;
	me->igpio.Mode = mode;
	me->igpio.Pull = pull;
	me->igpio.Speed = speed;
	#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)
	me->igpio.Alternate = alternate;
	#endif // defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}
void AlxIoPin_Init(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == false);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 Set isInit attribute
	me->isInit = true;

	// #2 Init GPIO
	AlxIoPin_Write(me, me->val);	// Set initial output value, before config
	HAL_GPIO_Init((GPIO_TypeDef*)me->port, &me->igpio);
	AlxIoPin_Write(me, me->val);	// Set initial output value, after config
}
void AlxIoPin_DeInit(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	HAL_GPIO_DeInit((GPIO_TypeDef*)me->port, me->igpio.Pin);

	// #2 Clear isInit attribute
	me->isInit = false;
}
bool AlxIoPin_Read(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	return HAL_GPIO_ReadPin((GPIO_TypeDef*)me->port, me->igpio.Pin);
}
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	HAL_GPIO_WritePin((GPIO_TypeDef*)me->port, me->igpio.Pin, (GPIO_PinState)val);
}
void AlxIoPin_Set(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	HAL_GPIO_WritePin((GPIO_TypeDef*)me->port, me->igpio.Pin, (GPIO_PinState)true);
}
void AlxIoPin_Reset(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	HAL_GPIO_WritePin((GPIO_TypeDef*)me->port, me->igpio.Pin, (GPIO_PinState)false);
}
void AlxIoPin_Toggle(AlxIoPin* me)
{
	ALX_IO_PIN_ASSERT(me->isInit == true);
	ALX_IO_PIN_ASSERT(me->wasCtorCalled == true);

	HAL_GPIO_TogglePin((GPIO_TypeDef*)me->port, me->igpio.Pin);
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
	LL_GPIO_SetPinPull((GPIO_TypeDef*)me->port, me->igpio.Pin, LL_GPIO_PULL_UP);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);

	//------------------------------------------------------------------------------
	// #2 Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
	LL_GPIO_SetPinPull((GPIO_TypeDef*)me->port, me->igpio.Pin, LL_GPIO_PULL_DOWN);

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


#endif // #if defined(ALX_C_LIB) && (defined(ALX_STM32F1) || defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0))
