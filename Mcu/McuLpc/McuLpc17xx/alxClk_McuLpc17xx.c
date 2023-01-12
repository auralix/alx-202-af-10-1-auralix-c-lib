/**
  ******************************************************************************
  * @file		alxClk_McuLpc17xx.c
  * @brief		Auralix C Library - ALX Clock MCU LPC17XX Module
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
#include "alxClk_McuLpc17xx.h"
#include "alxClk.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_LPC17XX)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxClk_Ctor
(
	AlxClk* me
)
{
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxClk_Init(AlxClk* me)
{
	ALX_CLK_ASSERT(me->isInit == false);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_GPIO);

	me->isInit = true;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxClk_DeInit(AlxClk* me)
{
	ALX_CLK_ASSERT(me->isInit == true);
	ALX_CLK_ASSERT(me->wasCtorCalled == true);

	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_GPIO);

	me->isInit = false;
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC17XX)
