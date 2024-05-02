/**
  ******************************************************************************
  * @file		alxOsCriticalSection.c
  * @brief		Auralix C Library - ALX OS Critical Section Module
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
#include "alxOsCriticalSection.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  */
void AlxOsCriticalSection_Enter(void)
{
	#if defined(ALX_FREE_RTOS)
	taskENTER_CRITICAL();
	#endif
}

/**
  * @brief
  */
void AlxOsCriticalSection_Exit(void)
{
	#if defined(ALX_FREE_RTOS)
	taskEXIT_CRITICAL();
	#endif
}


#endif	// #if defined(ALX_C_LIB)
