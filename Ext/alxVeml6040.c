/**
  ******************************************************************************
  * @file		alxVeml6040.c
  * @brief		Auralix C Library - ALX Light Sensor VEML604 Module
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
#include "alxVeml6040.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxVeml6040_Ctor
(
	AlxVeml6040* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxVeml6040_Init(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == false);

	// Return
	return Alx_Ok;
}
Alx_Status AlxVeml6040_DeInit(AlxVeml6040* me)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetR_raw(AlxVeml6040* me, uint16_t* R_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*R_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetG_raw(AlxVeml6040* me, uint16_t* G_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*G_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetB_raw(AlxVeml6040* me, uint16_t* B_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*B_raw = 0;
	return Alx_Ok;
}
Alx_Status AlxVeml6040_GetW_raw(AlxVeml6040* me, uint16_t* W_raw)
{
	// Assert
	ALX_VEML6040_ASSERT(me->wasCtorCalled == true);
	ALX_VEML6040_ASSERT(me->isInit == true);

	// Return
	*W_raw = 0;
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB)
