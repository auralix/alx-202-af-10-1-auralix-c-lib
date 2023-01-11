/**
  ******************************************************************************
  * @file		alxLin.c
  * @brief		Auralix C Library - ALX LIN Module
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
#include "alxLin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Private Functions
//******************************************************************************
static uint8_t AlxLin_GetProtectedId(uint8_t id);
static uint8_t AlxLin_GetEnhancedChecksum(uint8_t id, uint8_t* data, uint8_t len);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxLin_Ctor
(
	AlxLin* me,
	AlxSerialPort* alxSerialPort
)
{
	// Parameters
	me->alxSerialPort = alxSerialPort;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
Alx_Status AlxLin_Init(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == false);

	// Local variables
	Alx_Status status = Alx_Err;

	// Init serial port
	status = AlxSerialPort_Init(me->alxSerialPort);
	if (status != Alx_Ok) { ALX_LIN_TRACE("Err"); return status; }

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLin_DeInit(AlxLin* me)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// DeInit serial port
	status = AlxSerialPort_DeInit(me->alxSerialPort);
	if (status != Alx_Ok) { ALX_LIN_TRACE("Err"); return status; }

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLin_TxFrame(AlxLin* me, AlxLin_Payload* frame)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Return
	return Alx_Ok;
}
Alx_Status AlxLin_RxFrame(AlxLin* me, AlxLin_Payload* frame)
{
	// Assert
	ALX_LIN_ASSERT(me->wasCtorCalled == true);
	ALX_LIN_ASSERT(me->isInit == true);

	// Local variables
	Alx_Status status = Alx_Err;

	// Return
	return Alx_Ok;
}
void AlxLin_IrqHandler(AlxLin* me)
{
	AlxSerialPort_IrqHandler(me->alxSerialPort);
}


#endif	// #if defined(ALX_C_LIB)
