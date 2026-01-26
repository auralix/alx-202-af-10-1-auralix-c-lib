/**
  ******************************************************************************
  * @file		alxRst_McuStm32.c
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
// Includes
//******************************************************************************
#include "alxRst_McuStm32.h"
#include "alxRst.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4))


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  */
void AlxRst_Ctor
(
	AlxRst* me
)
{
	// Variables
	memset(&me->rr, 0, sizeof(me->rr));
	me->csr = 0;

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxRst_Init(AlxRst* me)
{
	// Assert
	ALX_RST_ASSERT(me->wasCtorCalled == true);
	ALX_RST_ASSERT(me->isInit == false);

	// Read
//	me->csr = HAL_RCC_GetResetSource();

	// Parse
	me->rr.sw = (me->csr & RCC_CSR_SFTRSTF) != 0;
	me->rr.rstPin = (me->csr & RCC_CSR_PINRSTF) != 0;
	me->rr.wwdg = (me->csr & RCC_CSR_WWDGRSTF) != 0;
	me->rr.iwdg = (me->csr & RCC_CSR_IWDGRSTF) != 0;
	me->rr.lowPowerMgmt = (me->csr & RCC_CSR_LPWRRSTF) != 0;
	me->rr.porOrBor = (me->csr & RCC_CSR_BORRSTF) != 0;
	#if defined(ALX_STM32F7)
	me->rr.por = (me->csr & RCC_CSR_PORRSTF) != 0;
	#endif
	#if defined(ALX_STM32L4)
	me->rr.firewall = (me->csr & RCC_CSR_FWRSTF) != 0;
	me->rr.optionByteLoader = (me->csr & RCC_CSR_OBLRSTF) != 0;
	#endif

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  */
void AlxRst_Trace(AlxRst* me)
{
	// Assert
	ALX_RST_ASSERT(me->wasCtorCalled == true);
	ALX_RST_ASSERT(me->isInit == true);

	// Trace
	ALX_RST_TRACE_INF("AlxRst_Trace - START - STM32 Reset Reason:");
	ALX_RST_TRACE_INF("- Software: %u", me->rr.sw);
	ALX_RST_TRACE_INF("- nRST Pin: %u", me->rr.rstPin);
	ALX_RST_TRACE_INF("- Window Watchdog (WWDG): %u", me->rr.wwdg);
	ALX_RST_TRACE_INF("- Independent Watchdog (IWDG): %u", me->rr.iwdg);
	ALX_RST_TRACE_INF("- Low-power Management: %u", me->rr.lowPowerMgmt);
	ALX_RST_TRACE_INF("- Power-on (POR) / Brown-out (BOR): %u", me->rr.porOrBor);
	#if defined(ALX_STM32F7)
	ALX_RST_TRACE_INF("- Power-on (POR): %u", me->rr.por);
	#endif
	#if defined(ALX_STM32L4)
	ALX_RST_TRACE_INF("- Firewall: %u", me->rr.firewall);
	ALX_RST_TRACE_INF("- Option Byte Loader: %u", me->rr.optionByteLoader);
	#endif
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F7) || defined(ALX_STM32L4))
