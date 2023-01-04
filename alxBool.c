/**
  ******************************************************************************
  * @file		alxBool.c
  * @brief		Auralix C Library - ALX Bool Module
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
#include "alxBool.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_C_LIB)


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBool_Ctor
(
	AlxBool* me,
	bool valInitial,
	float trueShortTime_ms,
	float trueLongTime_ms,
	float falseShortTime_ms,
	float falseLongTime_ms,
	float filtGlitchBool_StableTrueTime_ms,
	float filtGlitchBool_StableFalseTime_ms
)
{
	// Parameters
	me->valInitial = valInitial;
	me->trueShortTime_ms = trueShortTime_ms;
	me->trueLongTime_ms = trueLongTime_ms;
	me->falseShortTime_ms = falseShortTime_ms;
	me->falseLongTime_ms = falseLongTime_ms;
	me->filtGlitchBool_StableTrueTime_ms = filtGlitchBool_StableTrueTime_ms;
	me->filtGlitchBool_StableFalseTime_ms = filtGlitchBool_StableFalseTime_ms;

	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;
	AlxFiltGlitchBool_Ctor(&me->alxFiltGlitchBool, valInitial, filtGlitchBool_StableTrueTime_ms, filtGlitchBool_StableFalseTime_ms);

	AlxTimSw_Ctor(&me->alxTimSw_True, false);
	me->trueTime_ms = 0.f;
	me->isTrue = valInitial;
	me->isTrueUpToShortTime = valInitial;
	me->isTrueUpToLongTime = valInitial;
	me->isTrueForLongTime = false;
	me->wasTrue = valInitial;
	me->wasTrueForShortTime = false;
	me->wasTrueForLongTime = false;

	AlxTimSw_Ctor(&me->alxTimSw_False, false);
	me->falseTime_ms = 0.f;
	me->isFalse = !valInitial;
	me->isFalseUpToShortTime = !valInitial;
	me->isFalseUpToLongTime = !valInitial;
	me->isFalseForLongTime = false;
	me->wasFalse = !valInitial;
	me->wasFalseForShortTime = false;
	me->wasFalseForLongTime = false;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxBool_Update(AlxBool* me, bool val)
{
	//------------------------------------------------------------------------------
	// Assert
	//------------------------------------------------------------------------------
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);


	//------------------------------------------------------------------------------
	// Filter
	//------------------------------------------------------------------------------
	me->valRaw = val;
	me->valFiltered = AlxFiltGlitchBool_Process(&me->alxFiltGlitchBool, me->valRaw);


	//------------------------------------------------------------------------------
	// Update
	//------------------------------------------------------------------------------
	if(me->valFiltered == true)
	{
		//------------------------------------------------------------------------------
		// Clear False variables
		//------------------------------------------------------------------------------
		AlxTimSw_Stop(&me->alxTimSw_False);
		me->falseTime_ms = 0.f;
		me->isFalse = false;
		me->isFalseUpToShortTime = false;
		me->isFalseUpToLongTime = false;
		me->isFalseForLongTime = false;


		//------------------------------------------------------------------------------
		// Start True timer
		//------------------------------------------------------------------------------
		if (AlxTimSw_IsRunning(&me->alxTimSw_True) == false)
		{
			AlxTimSw_Start(&me->alxTimSw_True);
		}


		//------------------------------------------------------------------------------
		// Update True variables
		//------------------------------------------------------------------------------
		me->trueTime_ms = AlxTimSw_Get_ms(&me->alxTimSw_True);
		me->isTrue = true;
		me->wasTrue = true;

		// Up to short time
		if(me->trueTime_ms < me->trueShortTime_ms)
		{
			me->isTrueUpToShortTime = true;
			me->isTrueUpToLongTime = true;
			me->isTrueForLongTime = false;
		}
		// Up to long time
		else if((me->trueShortTime_ms <= me->trueTime_ms) && (me->trueTime_ms < me->trueLongTime_ms))
		{
			me->isTrueUpToShortTime = false;
			me->isTrueUpToLongTime = true;
			me->isTrueForLongTime = false;
			me->wasTrueForShortTime = true;
		}
		// Long time
		else if(me->trueLongTime_ms <= me->trueTime_ms)
		{
			me->isTrueUpToShortTime = false;
			me->isTrueUpToLongTime = false;
			me->isTrueForLongTime = true;
			me->wasTrueForLongTime = true;
		}
		// Assert
		else
		{
			ALX_BOOL_ASSERT(false);	// We should never get here
		}
	}
	else if(me->valFiltered == false)
	{
		//------------------------------------------------------------------------------
		// Clear True variables
		//------------------------------------------------------------------------------
		AlxTimSw_Stop(&me->alxTimSw_True);
		me->trueTime_ms = 0.f;
		me->isTrue = false;
		me->isTrueUpToShortTime = false;
		me->isTrueUpToLongTime = false;
		me->isTrueForLongTime = false;


		//------------------------------------------------------------------------------
		// Start False timer
		//------------------------------------------------------------------------------
		if (AlxTimSw_IsRunning(&me->alxTimSw_False) == false)
		{
			AlxTimSw_Start(&me->alxTimSw_False);
		}


		//------------------------------------------------------------------------------
		// Update False variables
		//------------------------------------------------------------------------------
		me->falseTime_ms = AlxTimSw_Get_ms(&me->alxTimSw_False);
		me->isFalse = true;
		me->wasFalse = true;

		// Up to short time
		if(me->falseTime_ms < me->falseShortTime_ms)
		{
			me->isFalseUpToShortTime = true;
			me->isFalseUpToLongTime = true;
			me->isFalseForLongTime = false;
		}
		// Up to long time
		else if((me->falseShortTime_ms <= me->falseTime_ms) && (me->falseTime_ms < me->falseLongTime_ms))
		{
			me->isFalseUpToShortTime = false;
			me->isFalseUpToLongTime = true;
			me->isFalseForLongTime = false;
			me->wasFalseForShortTime = true;
		}
		// Long time
		else if(me->falseLongTime_ms <= me->falseTime_ms)
		{
			me->isFalseUpToShortTime = false;
			me->isFalseUpToLongTime = false;
			me->isFalseForLongTime = true;
			me->wasFalseForLongTime = true;
		}
		// Assert
		else
		{
			ALX_BOOL_ASSERT(false);	// We should never get here
		}
	}
	else
	{
		ALX_BOOL_ASSERT(false);	// We should never get here
	}
}
bool AlxBool_IsTrue(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isTrue;
}
bool AlxBool_IsTrueUpToShortTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isTrueUpToShortTime;
}
bool AlxBool_IsTrueUpToLongTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isTrueUpToLongTime;
}
bool AlxBool_IsTrueForLongTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isTrueForLongTime;
}
bool AlxBool_WasTrue(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->wasTrue;
}
bool AlxBool_WasTrueForShortTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->wasTrueForShortTime;
}
bool AlxBool_WasTrueForLongTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->wasTrueForLongTime;
}
void AlxBool_ClearWasTrueFlag(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->wasTrue = false;
}
void AlxBool_ClearWasTrueForShortTimeFlag(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->wasTrueForShortTime = false;
}
void AlxBool_ClearWasTrueForLongTimeFlag(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->wasTrueForLongTime = false;
}
bool AlxBool_IsFalse(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isFalse;
}
bool AlxBool_IsFalseUpToShortTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isFalseUpToShortTime;
}
bool AlxBool_IsFalseUpToLongTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isFalseUpToLongTime;
}
bool AlxBool_IsFalseForLongTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->isFalseForLongTime;
}
bool AlxBool_WasFalse(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->wasFalse;
}
bool AlxBool_WasFalseForShortTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->wasFalseForShortTime;
}
bool AlxBool_WasFalseForLongTime(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Return
	return me->wasFalseForLongTime;
}
void AlxBool_ClearWasFalseFlag(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->wasFalse = false;
}
void AlxBool_ClearWasFalseForShortTimeFlag(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->wasFalseForShortTime = false;
}
void AlxBool_ClearWasFalseForLongTimeFlag(AlxBool* me)
{
	// Assert
	ALX_BOOL_ASSERT(me->wasCtorCalled == true);

	// Clear
	me->wasFalseForLongTime = false;
}


#endif	// #if defined(ALX_C_LIB)
