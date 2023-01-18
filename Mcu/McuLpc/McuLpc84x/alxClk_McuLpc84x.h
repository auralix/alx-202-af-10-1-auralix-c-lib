/**
  ******************************************************************************
  * @file		alxClk_McuLpc84x.h
  * @brief		Auralix C Library - ALX Clock MCU LPC84X Module
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
#ifndef ALX_CLK_MCU_LPC84X_H
#define ALX_CLK_MCU_LPC84X_H

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
#if defined(ALX_C_LIB) && defined(ALX_LPC84X)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	clock_main_clk_src_t mainClkSource;
	uint32_t clkFreq;
	uint8_t divider;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxClk;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxClk_Ctor
(
	AlxClk* me,
	clock_main_clk_src_t mainClkSource,
	uint32_t clkFreq,
	uint8_t divider
);


#endif	// #if defined(ALX_C_LIB) && defined(ALX_LPC84X)

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ALX_CLK_MCU_LPC84X_H
