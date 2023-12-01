/**
  ******************************************************************************
  * @file		alxWdt_McuStm32.c
  * @brief		Auralix C Library - ALX Watchdog Timer MCU STM32 Module
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
#include "alxWdt_McuStm32.h"
#include "alxWdt.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4) || defined(ALX_STM32U5))


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		config
  */
void AlxWdt_Ctor
(
	AlxWdt* me,
	AlxWdt_Config config
)
{
	// Parameters
	me->config = config;

	// Variables
	me->hiwdg.Instance = IWDG;
	if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_512ms_WdtClk_8kHz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
		me->hiwdg.Init.Reload = 0x0FFF;
	}
	else if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_1024ms_WdtClk_4kHz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_8;
		me->hiwdg.Init.Reload = 0x0FFF;
	}
	else if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_2048ms_WdtClk_2kHz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_16;
		me->hiwdg.Init.Reload = 0x0FFF;
	}
	else if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_4096ms_WdtClk_1kHz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
		me->hiwdg.Init.Reload = 0x0FFF;
	}
	else if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_8192ms_WdtClk_500Hz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
		me->hiwdg.Init.Reload = 0x0FFF;
	}
	else if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_16384ms_WdtClk_250Hz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
		me->hiwdg.Init.Reload = 0x0FFF;
	}
	else if (me->config == AlxWdt_Config_McuStm32_WdtTimeout_32768ms_WdtClk_125Hz_Lsi_32kHz)
	{
		me->hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
		me->hiwdg.Init.Reload = 0x0FFF;
	}

	#if defined(ALX_STM32F7) || defined(ALX_STM32L4)
	me->hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
	#endif

	#if defined(ALX_STM32U5)
	me->hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
	me->hiwdg.Init.EWI = IWDG_EWI_DISABLE;
	#endif

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
Alx_Status AlxWdt_Init(AlxWdt* me)
{
	// Assert
	ALX_WDT_ASSERT(me->wasCtorCalled == true);
	ALX_WDT_ASSERT(me->isInit == false);

	// Init
	if (HAL_IWDG_Init(&me->hiwdg) != HAL_OK) { ALX_WDT_TRACE("Err"); return Alx_Err; };

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
Alx_Status AlxWdt_Refresh(AlxWdt* me)
{
	// Assert
	ALX_WDT_ASSERT(me->wasCtorCalled == true);
	ALX_WDT_ASSERT(me->isInit == true);

	// Refresh
	if (HAL_IWDG_Refresh(&me->hiwdg) != HAL_OK) { ALX_WDT_TRACE("Err"); return Alx_Err; };

	// Return
	return Alx_Ok;
}


#endif	// #if defined(ALX_C_LIB) && (defined(ALX_STM32F4) || defined(ALX_STM32F7) || defined(ALX_STM32L4) || defined(ALX_STM32U5))
