/**
  ******************************************************************************
  * @file		alxMemRaw.c
  * @brief		Auralix C Library - ALX Memory Raw Module
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
#include "alxMemRaw.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
ALX_WEAK void AlxMemRaw_Ctor
(
	AlxMemRaw* me
)
{
	(void)me;
}


//******************************************************************************
// Functions
//******************************************************************************
ALX_WEAK Alx_Status AlxMemRaw_Init(AlxMemRaw* me)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
ALX_WEAK Alx_Status AlxMemRaw_DeInit(AlxMemRaw* me)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
ALX_WEAK Alx_Status AlxMemRaw_Read(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}
ALX_WEAK Alx_Status AlxMemRaw_Write(AlxMemRaw* me, uint32_t addr, uint8_t* data, uint32_t len, bool checkWithReadEnable, uint8_t numOfTries, uint16_t timeout_ms)
{
	(void)me;
	ALX_MEM_RAW_ASSERT(false);
	return 0;
}


#endif	// #if defined(ALX_C_LIB)
