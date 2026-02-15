/**
  ******************************************************************************
  * @file		alxIrq.c
  * @brief		Auralix C Library - ALX IRQ Module
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
#include "alxIrq.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @return key
  */
uint32_t AlxIrq_Lock(void)
{
	uint32_t key = 0;

	key = __get_PRIMASK();
	__disable_irq();
	__DMB();	// TV: Ensure that all memory ops prior to this point are observed BEFORE memory ops AFTER this point

	return key;
}

/**
  * @brief
  * @param[in] key
  */
void AlxIrq_Unlock(uint32_t key)
{
	__DMB();	// TV: Ensure that all memory ops prior to this point are observed BEFORE memory ops AFTER this point
	__set_PRIMASK(key);
}


#endif	// #if defined(ALX_C_LIB)
