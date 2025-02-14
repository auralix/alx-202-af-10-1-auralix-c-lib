/**
  ******************************************************************************
  * @file		alxTrace_McuSam.c
  * @brief		Auralix C Library - ALX Trace MCU SAM Module
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
#include "alxTrace_McuSam.h"
#include "alxTrace.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && defined(ALX_SAM)


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxTrace_Periph_EnableClk(AlxTrace* me);


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		portPin
  * @param[in]		func
  * @param[in]		hw
  */
void AlxTrace_Ctor
(
	AlxTrace* me,
	uint8_t portPin,
	uint32_t func,
	void* hw
)
{
	// Parameters
	me->portPin = portPin;
	me->func = func;
	me->hw = hw;

	// Variables
	memset(&me->descr, 0, sizeof(me->descr));

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
Alx_Status AlxTrace_Init(AlxTrace* me)
{
	// Init GPIO
	gpio_set_pin_level(me->portPin, false);
	gpio_set_pin_direction(me->portPin, GPIO_DIRECTION_OFF);
	gpio_set_pin_pull_mode(me->portPin, GPIO_PULL_OFF);
	gpio_set_pin_function(me->portPin, me->func);

	// Enable USART clock
	_pm_enable_bus_clock(PM_BUS_APBC, me->hw);
	AlxTrace_Periph_EnableClk(me);

	// Init USART
	usart_sync_init(&me->descr, me->hw, ALX_NULL);	// TV: Always returns OK

	// Enable USART
	usart_sync_enable(&me->descr);					// TV: Always returns OK

	// Set isInit
	me->isInit = true;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_DeInit(AlxTrace* me)
{
	// Disable USART
	usart_sync_disable(&me->descr);	// TV: Always returns OK

	// DeInit USART
	usart_sync_deinit(&me->descr);	// TV: Always returns OK

	// Disable USART clock
	_pm_disable_bus_clock(PM_BUS_APBC, me->hw);

	// Clear isInit
	me->isInit = false;

	// Return
	return Alx_Ok;
}

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		str
  * @retval			Alx_Ok
  * @retval			Alx_Err
  */
Alx_Status AlxTrace_WriteStr(AlxTrace* me, const char* str)
{
	// Prepare
	struct io_descriptor *io = NULL;
	usart_sync_get_io_descriptor(&me->descr, &io);

	// Write
	uint16_t strLen = strlen(str);
	int32_t numOfBytesWritten = io_write(io, (uint8_t*)str, strLen);
	if (numOfBytesWritten != strLen)
	{
		return Alx_Err;
	}

	// Return
	return Alx_Ok;
}


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxTrace_Periph_EnableClk(AlxTrace* me)
{
	#ifdef CONF_GCLK_SERCOM0_CORE_SRC
	if (me->hw == SERCOM0)	{ _gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC); return; }
	#endif
	#ifdef CONF_GCLK_SERCOM1_CORE_SRC
	if (me->hw == SERCOM1)	{ _gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC); return; }
	#endif
	#ifdef CONF_GCLK_SERCOM2_CORE_SRC
	if (me->hw == SERCOM2)	{ _gclk_enable_channel(SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC); return; }
	#endif
	#ifdef CONF_GCLK_SERCOM3_CORE_SRC
	if (me->hw == SERCOM3)	{ _gclk_enable_channel(SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC); return; }
	#endif
	#ifdef CONF_GCLK_SERCOM4_CORE_SRC
	if (me->hw == SERCOM4)	{ _gclk_enable_channel(SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC); return; }
	#endif
	#ifdef CONF_GCLK_SERCOM5_CORE_SRC
	if (me->hw == SERCOM5)	{ _gclk_enable_channel(SERCOM5_GCLK_ID_CORE, CONF_GCLK_SERCOM5_CORE_SRC); return; }
	#endif

	// We should not get here
}


#endif	// #if defined(ALX_C_LIB) && defined(ALX_SAM)
