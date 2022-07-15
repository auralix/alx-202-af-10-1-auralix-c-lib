/**
  ******************************************************************************
  * @file		alxIoPin_McuLpc17.h
  * @brief		Auralix C Library - ALX IO Pin Module
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

#ifndef ALX_IO_PIN_MCU_LPC17_H
#define ALX_IO_PIN_MCU_LPC17_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	uint32_t mode;
	uint8_t func;
	bool isOpenDrain;
	bool dir;

	// Info
	bool val;
	bool isInit;
	bool wasCtorCalled;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	uint32_t mode,
	uint8_t func,
	bool isOpenDrain,
	bool dir,
	bool val
)
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


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_MCU_LPC17_H
