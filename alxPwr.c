/**
  ******************************************************************************
  * @file		alxPwr.c
  * @brief		Auralix C Library - ALX Power Module
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
void AlxPwr_Ctor
(
	AlxPwr* me,
	AlxAdc* adc,
	Alx_Ch chAdc,
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
	me->adc = adc;
	me->chAdc = chAdc;
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
	me->isGoodRaw = false;
	me->isGoodFiltered = false;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxPwr_Init(AlxPwr* me)
{
	// #1 Assert
	ALX_PWR_ASSERT(me->isInit == false);
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// #2
	// TV: TODO - IsErr Implementation

	// #3 Set isInit
	me->isInit = true;
}
void AlxPwr_DeInit(AlxPwr* me)
{
	// #1 Assert
	ALX_PWR_ASSERT(me->isInit == true);
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// #2
	// TV: TODO - IsErr Implementation

	// #3 Reset isInit
	me->isInit = false;
}
void AlxPwr_Handle(AlxPwr* me)
{
	// #1 Assert
	ALX_PWR_ASSERT(me->isInit == true);
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// #2 Get ADC voltage
	me->adcVal_V = AlxAdc_GetVoltage_V(me->adc, me->chAdc);

	// #3 Calculate PWR voltage
	me->val_V = AlxVdiv_GetVin_V(me->adcVal_V, me->vdiv_ResHigh_kOhm, me->vdiv_ResLow_kOhm);

	// #4 Process Hysteresis
	me->hys2_St = AlxHys2_Process(&me->hys2, me->val_V);

	// #5 Handle Hysteresis State
	if (me->hys2_St == AlxHys2_StMid)
		me->isGoodRaw = true;
	else
		me->isGoodRaw = false;

	// #6 Process Filter
	me->isGoodFiltered = AlxFiltGlitchBool_Process(&me->filtGlitchBool, me->isGoodRaw);
}
bool AlxPwr_IsGood(AlxPwr* me)
{
	// #1 Assert
	ALX_PWR_ASSERT(me->isInit == true);
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	return me->isGoodFiltered;
}
bool AlxPwr_IsNotGood(AlxPwr* me)
{
	// #1 Assert
	ALX_PWR_ASSERT(me->isInit == true);
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	return !AlxPwr_IsGood(me);
}
bool AlxPwr_IsErr(AlxPwr* me)
{
	// #1 Assert
	ALX_PWR_ASSERT(me->isInit == true);
	ALX_PWR_ASSERT(me->wasCtorCalled == true);

	// #2
	// TV: TODO - IsErr Implementation
	ALX_PWR_ASSERT(false);
}


#endif // #if defined(ALX_C_LIB)
