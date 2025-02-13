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
//	GPIO_TypeDef* port,
//	uint16_t pin,
//	uint32_t mode,
//	uint32_t pull,
//	uint32_t speed,
//	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
//	uint32_t alternate,
//	#endif
	bool val
)
{
	// Parameters
//	me->port = port;
//	me->pin = pin;
//	me->mode = mode;
//	me->pull = pull;
//	me->speed = speed;
//	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
//	me->alternate = alternate;
//	#endif
	me->val = val;

	// Variables
//	me->igpio.Pin = pin;
//	me->igpio.Mode = mode;
//	me->igpio.Pull = pull;
//	me->igpio.Speed = speed;
//	#if defined(ALX_STM32F0) || defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32G4) || defined(ALX_STM32L0) || defined(ALX_STM32L4) || defined(ALX_STM32U5)
//	me->igpio.Alternate = alternate;
//	#endif

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

	// Set isInit
	me->isInit = true;

	// Init
//	AlxIoPin_Write(me, me->val);							// Set initial output value, before config
//	HAL_GPIO_Init((GPIO_TypeDef*)me->port, &me->igpio);
//	AlxIoPin_Write(me, me->val);							// Set initial output value, after config
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
//	HAL_GPIO_DeInit((GPIO_TypeDef*)me->port, me->igpio.Pin);

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
//	return HAL_GPIO_ReadPin((GPIO_TypeDef*)me->port, me->igpio.Pin);
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
//	HAL_GPIO_WritePin((GPIO_TypeDef*)me->port, me->igpio.Pin, (GPIO_PinState)val);
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
//	HAL_GPIO_WritePin((GPIO_TypeDef*)me->port, me->igpio.Pin, (GPIO_PinState)true);
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
//	HAL_GPIO_WritePin((GPIO_TypeDef*)me->port, me->igpio.Pin, (GPIO_PinState)false);
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
//	HAL_GPIO_TogglePin((GPIO_TypeDef*)me->port, me->igpio.Pin);
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
//	LL_GPIO_SetPinPull((GPIO_TypeDef*)me->port, me->igpio.Pin, LL_GPIO_PULL_UP);

	// Wait
	AlxDelay_ms(1);

	// Read
	bool valPullUp = AlxIoPin_Read(me);


	//------------------------------------------------------------------------------
	// Read @ PullDown
	//------------------------------------------------------------------------------

	// Config PullDown
//	LL_GPIO_SetPinPull((GPIO_TypeDef*)me->port, me->igpio.Pin, LL_GPIO_PULL_DOWN);

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
