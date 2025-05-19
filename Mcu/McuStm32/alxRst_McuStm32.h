/**
  ******************************************************************************
  * @file		alxRst_McuStm32.h
  * @brief		Auralix C Library - ALX Reset MCU STM32 Module
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
// Include Guard
//******************************************************************************
#ifndef ALX_RST_MCU_STM32_H
#define ALX_RST_MCU_STM32_H

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
#if defined(ALX_C_LIB) && (defined(ALX_STM32F7) || defined(ALX_STM32L4))


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	bool sw;
	bool rstPin;
	bool wwdg;
	bool iwdg;
	bool lowPowerMgmt;
	bool porOrBor;
	#if defined(ALX_STM32F7)
	bool por;
	#endif
	#if defined(ALX_STM32L4)
	bool firewall;
	bool optionByteLoader;
	#endif
} AlxRst_RstReason;

typedef struct
{
	// Variables
	AlxRst_RstReason rr;
	uint32_t csr;

	// Info
	bool wasCtorCalled;
	bool isInit;
} AlxRst;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxRst_Ctor
(
	AlxRst* me
);


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F7) || defined(ALX_STM32L4))

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_RST_MCU_STM32_H
