/**
  ******************************************************************************
  * @file		alxClk_McuLpc17.h
  * @brief		Auralix C Library - ALX Clock Module
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

#ifndef ALX_CLK_MCU_LPC17_H
#define ALX_CLK_MCU_LPC17_H

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
	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxClk_Ctor
(
	AlxClk* me
)
{
	me->isInit = false;
	me->wasCtorCalled = true;
}


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_CLK_MCU_LPC17_H
