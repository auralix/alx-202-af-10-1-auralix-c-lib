/**
  ******************************************************************************
  * @file		alxBq24600.c
  * @brief		Auralix C Library - ALX Battery Charger BQ24600 Module
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
#include "alxBq24600.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBq24600_Ctor
(
	AlxBq24600* me,
	AlxIoPin* do_EN,
	AlxIoPin* di_nPG,
	AlxIoPin* di_STAT
)
{
	// Parameters - Const
	me->STATUS_DETECTION_WINDOW_TIME_ms = 4000;
	me->ERROR_STATUS_DETECTION_TRANSITIONS = 3;
	me->STATUS_BLINK_HALFPERIOD_TIME_MIN_ms = 900;
	me->STATUS_BLINK_HALFPERIOD_TIME_MAX_ms = 1100;
	
	// Ctor
	AlxTimSw_Ctor(&me->timWindow, false);
	AlxTimSw_Ctor(&me->timStatHalfperiod, false);
	AlxTimSw_Ctor(&me->timStableStatus, false);

	// Objects - External
	me->do_EN = do_EN;
	me->di_nPG = di_nPG;
	me->di_STAT = di_STAT;

	// Variables
	me->statusRaw = Bq24600Status_Undef;
	me->status = Bq24600Status_Undef;
	me->oldStatPinState = false;
	me->nOfStatPinTransitions = 0;
	me->isStatBlink = false;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxBq24600_Init(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == false);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	// #1 Init GPIO
	AlxIoPin_Init(me->do_EN);
	AlxIoPin_Init(me->di_nPG);
	AlxIoPin_Init(me->di_STAT);

	// #2 Set isInit
	me->isInit = true;

	// #3 Set variables
	AlxTimSw_Start(&me->timWindow);
	AlxTimSw_Start(&me->timStatHalfperiod);
	AlxTimSw_Start(&me->timStableStatus);

	me->statusRaw = Bq24600Status_Undef;
	me->status = Bq24600Status_Undef;
	me->oldStatPinState = false;
	me->nOfStatPinTransitions = 0;
	me->isStatBlink = false;
}
void AlxBq24600_DeInit(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	// #1 DeInit GPIO
	AlxIoPin_DeInit(me->do_EN);
	AlxIoPin_DeInit(me->di_nPG);
	AlxIoPin_DeInit(me->di_STAT);

	// #2 Reset isInit
	me->isInit = false;
}
void AlxBq24600_Handle (AlxBq24600* me)
{
	ALX_BQ24600_ASSERT (me->isInit == true);
	ALX_BQ24600_ASSERT (me->wasCtorCalled == true);

	// #1 count transitions of STAT pin
	bool statPinState = AlxIoPin_Read (me->di_STAT);
	if (me->oldStatPinState != statPinState)									// if new transition is detected
	{
		me->oldStatPinState = statPinState;

		// count transition only if half period was between set limits meaning it has correct frequency for STAT blinking -> eliminate other transitions
		if (AlxTimSw_Get_ms (&me->timStatHalfperiod) >= me->STATUS_BLINK_HALFPERIOD_TIME_MIN_ms && AlxTimSw_Get_ms (&me->timStatHalfperiod) <= me->STATUS_BLINK_HALFPERIOD_TIME_MAX_ms)
			me->nOfStatPinTransitions++;  										// increment new transition

		AlxTimSw_Start (&me->timStatHalfperiod);   								// restart half period timer
	}

	// #2 restart counting if power good is lost
	if (AlxIoPin_Read (me->di_nPG))
		me->nOfStatPinTransitions = 0;  										// restart transitions counting

	// #3 check for end of the window -> perform blink detection procedure
	if (AlxTimSw_Get_ms (&me->timWindow) >= me->STATUS_DETECTION_WINDOW_TIME_ms)
	{
		AlxTimSw_Start (&me->timWindow);   										// restart window timer

		if (me->nOfStatPinTransitions >= me->ERROR_STATUS_DETECTION_TRANSITIONS)	// if STAT is BLINK
		{
			me->isStatBlink = true;
		}
		else
		{
			me->isStatBlink = false;
		}
		me->nOfStatPinTransitions = 0;  										// restart transitions counting
	}

	// #4 define current status
	Bq24600_status statusRawOld = me->statusRaw;								// used for detection of stable status state

	if (me->isStatBlink)
		me->statusRaw = Bq24600Status_Error;
	else if (AlxIoPin_Read (me->di_STAT) == false)								// if STAT is LOW
		me->statusRaw = Bq24600Status_Charging;
	else if (AlxIoPin_Read (me->di_nPG) == false)								// if STAT is HIGH and PG is LOW
		me->statusRaw = Bq24600Status_BattFull;
	else me->statusRaw = Bq24600Status_Sleep;

	// #5 define stable status
	if (statusRawOld != me->statusRaw)
		AlxTimSw_Start(&me->timStableStatus);

	if (AlxTimSw_Get_ms (&me->timStableStatus) >= me->STATUS_BLINK_HALFPERIOD_TIME_MAX_ms)
		me->status = me->statusRaw;
}
void AlxBq24600_Enable(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Write(me->do_EN, true);
}
void AlxBq24600_Disable(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Write(me->do_EN, false);
}
bool AlxBq24600_IsBatCharging(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	return (me->status == Bq24600Status_Charging) ? true : false;
}
bool AlxBq24600_IsBatFull(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	return (me->status == Bq24600Status_BattFull) ? true : false;
}
bool AlxBq24600_IsSleep(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	return (me->status == Bq24600Status_Sleep) ? true : false;
}
bool AlxBq24600_IsErr(AlxBq24600* me)
{
	ALX_BQ24600_ASSERT(me->isInit == true);
	ALX_BQ24600_ASSERT(me->wasCtorCalled == true);

	return (me->status == Bq24600Status_Error) ? true : false;
}
