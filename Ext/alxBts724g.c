/**
  ******************************************************************************
  * @file		alxBts724g.c
  * @brief		Auralix C Library - ALX High Side Power Switch BTS724G Module
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
#include "alxBts724g.h"


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
  * @param[in]		do_HS_IN
  * @param[in]		di_HS_ST
  */
void AlxBts724g_Ctor
(
	AlxBts724g* me,
	AlxIoPin* do_HS_IN,
	AlxIoPin* di_HS_ST
)
{
	// Parameters - const
	me->stableTrueTime_openLoad_ms = 100;
	me->stableFalseTime_openLoad_ms = 100;
	me->stableTrueTime_overTemp_ms = 5;
	me->stableFalseTime_overTemp_ms = 100;

	// Ctor
	AlxFiltGlitchBool_Ctor(&me->filtGlitch_openLoad, true, me->stableTrueTime_openLoad_ms, me->stableFalseTime_openLoad_ms);
	AlxFiltGlitchBool_Ctor(&me->filtGlitch_overTemp, true, me->stableTrueTime_overTemp_ms, me->stableFalseTime_overTemp_ms);

	// Objects - External
	me->do_HS_IN  = do_HS_IN;
	me->di_HS_ST = di_HS_ST;

	// Variables
	me->isOpenLoadDetected = false;
	me->isOverTempDetected = false;
	me->wasOpenLoadDetected = false;
	me->wasOverTempDetected = false;

	//me->nOfStatPinTransitions = 0;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
	me->isOutSet = false;
}


//******************************************************************************
// Functions
//******************************************************************************

/**
  * @brief
  * @param[in,out] me
  */
void AlxBts724g_Init(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == false);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->do_HS_IN);
	AlxIoPin_Init(me->di_HS_ST);

	// #2 Set isInit
	me->isInit = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxBts724g_DeInit(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	AlxIoPin_DeInit(me->do_HS_IN);
	AlxIoPin_DeInit(me->di_HS_ST);

	// #2 Reset isInit
	 me->isInit = false;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxBts724g_Handle(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	// Read status pin
	bool status = AlxIoPin_Read(me->di_HS_ST);

	// detect open load when output is off and status is low
	bool isOpenLoadDetectedRaw =  !status && !me->isOutSet;
	me->isOpenLoadDetected = AlxFiltGlitchBool_Process(&me->filtGlitch_openLoad, isOpenLoadDetectedRaw);
	if (me->isOpenLoadDetected) me->wasOpenLoadDetected = true;	// Sets flag wasOpenLoadDetectedRaw

	// detect over temp when output is on and status is low
	bool isOverTempDetectedRaw = !status && me->isOutSet;
	me->isOverTempDetected = AlxFiltGlitchBool_Process(&me->filtGlitch_overTemp, isOverTempDetectedRaw);
	if(me->isOverTempDetected) me->wasOverTempDetected = true;	// Sets flag wasOverTempDetected
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxBts724g_SetOut(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);
	AlxIoPin_Set(me->do_HS_IN);

	me->isOutSet = true;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxBts724g_ResetOut(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Reset(me->do_HS_IN);
	me->isOutSet = false;
}

/**
  * @brief
  * @param[in,out] me
  */
void AlxBts724g_WriteOut(AlxBts724g* me, bool val)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Write(me->do_HS_IN, val);
	me->isOutSet = val;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxBts724g_IsOpenLoadDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	return me->isOpenLoadDetected;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxBts724g_IsOverTempDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	return me->isOverTempDetected;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxBts724g_WasOpenLoadDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	bool wasOpenLoadDetected = me->wasOpenLoadDetected;
	me->wasOpenLoadDetected = false;	// Resets flag wasOpenLoadDetectedRaw

	return wasOpenLoadDetected;
}

/**
  * @brief
  * @param[in,out]	me
  * @retval			false
  * @retval			true
  */
bool AlxBts724g_WasOverTempDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	bool wasOverTempDetected = me->wasOverTempDetected;
	me->wasOverTempDetected = false;	// Resets flag wasOverTempDetected

	return wasOverTempDetected;
}


#endif	// #if defined(ALX_C_LIB)
