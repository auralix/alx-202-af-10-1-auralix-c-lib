/**
  ******************************************************************************
  * @file		alxPwr.c
  * @brief		Auralix C Library - ALX Power Module
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
#include "alxPwr.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		adc
  * @param[in]		chAdc
  * @param[in]		vdiv_ResHigh_kOhm
  * @param[in]		vdiv_ResLow_kOhm
  * @param[in]		hys2_TopHigh_V
  * @param[in]		hys2_TopLow_V
  * @param[in]		hys2_BotHigh_V
  * @param[in]		hys2_BotLow_V
  * @param[in]		filtGlitchBool_StableTrueTime_ms
  * @param[in]		filtGlitchBool_StableFalseTime_ms
  */
void AlxPwr_Ctor
(
	AlxPwr* me,
	float vdiv_ResHigh_kOhm,
	float vdiv_ResLow_kOhm,
	float hys2_TopHigh_V,
	float hys2_TopLow_V,
	float hys2_BotHigh_V,
	float hys2_BotLow_V,
	float filtGlitchBool_StableTrueTime_ms,
	float filtGlitchBool_StableFalseTime_ms
)
{
	// Parameters
	me->vdiv_ResHigh_kOhm = vdiv_ResHigh_kOhm;
	me->vdiv_ResLow_kOhm = vdiv_ResLow_kOhm;
	me->hys2_TopHigh_V = hys2_TopHigh_V;
	me->hys2_TopLow_V = hys2_TopLow_V;
	me->hys2_BotHigh_V = hys2_BotHigh_V;
	me->hys2_BotLow_V = hys2_BotLow_V;
	me->filtGlitchBool_StableTrueTime_ms = filtGlitchBool_StableTrueTime_ms;
	me->filtGlitchBool_StableFalseTime_ms = filtGlitchBool_StableFalseTime_ms;

	// Variables
	AlxHys2_Ctor(&me->hys2, hys2_TopHigh_V, hys2_TopLow_V, hys2_BotHigh_V, hys2_BotLow_V);
	AlxFiltGlitchBool_Ctor(&me->filtGlitchBool, false, filtGlitchBool_StableTrueTime_ms, filtGlitchBool_StableFalseTime_ms);
	me->adcVal_V = 0.f;
	me->val_V = 0.f;
	me->isInRangeRaw = false;
	me->isInRangeFiltered = false;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out]	me
  * @param[in]		adcVal_V
  * @retval			false
  * @retval			true
  */
bool AlxPwr_Process(AlxPwr* me, float adcVal_V)
{
	// Assert
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// Set ADC voltage
	me->adcVal_V = adcVal_V;

	// Calculate PWR voltage
	me->val_V = AlxVdiv_GetVin_V(me->adcVal_V, me->vdiv_ResHigh_kOhm, me->vdiv_ResLow_kOhm);

	// Process hysteresis
	me->hys2_St = AlxHys2_Process(&me->hys2, me->val_V);

	// Handle hysteresis state
	if (me->hys2_St == AlxHys2_StMid)
	{
		me->isInRangeRaw = true;
	}
	else
	{
		me->isInRangeRaw = false;
	}

	// Process filter
	me->isInRangeFiltered = AlxFiltGlitchBool_Process(&me->filtGlitchBool, me->isInRangeRaw);

	// Return
	return me->isInRangeFiltered;
}


#endif	// #if defined(ALX_C_LIB)
