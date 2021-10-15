/**
  ******************************************************************************
  * @file alxBtn.c
  * @brief Auralix C Library - ALX Button Module
  * @version $LastChangedRevision: 4638 $
  * @date $LastChangedDate: 2021-04-08 15:26:20 +0200 (Thu, 08 Apr 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBtn.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxBtn_Ctor
(
	AlxBtn* me,
	bool valInitial,
	float pressedLongTime_ms,	// min time of long button press [ms]
	float filterTime	// time of stable button status to change it's state [ms]
)
{
	// Ctor
	AlxTimSw_Ctor(&me->timPressed, false);
	AlxFiltGlitchBool_Ctor(&me->filtGlitchBool, false, filterTime, filterTime);
	
	// Parameters Const
	me->longTime_ms = pressedLongTime_ms;
	me->filterTime = filterTime;
	
	// Variables
	me->isPressedOld = valInitial;
	me->isPressed			= false;
	me->wasPressed			= false;
	me->wasReleased			= false;
	me->isPressedShort		= false;
	me->wasPressedShort		= false;
	me->isPressedLong		= false;
	me->wasPressedLong		= false;
	
	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxBtn_Handle(AlxBtn* me, bool isPressedRaw)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	// #1 Filtered input
	me->isPressed = AlxFiltGlitchBool_Process(&me->filtGlitchBool, isPressedRaw);
	// #2 isPressed - High line
	if(me->isPressed)
	{
		me->wasPressed = true; // WasPressed	
		// Check timeout
		if(AlxTimSw_IsTimeout_ms(&me->timPressed, me->longTime_ms)) // Long pressed
		{
			me->isPressedShort	= false;	// IsPressedShort
			me->isPressedLong	= true;		// IsPressedLong
			me->wasPressedLong	= true;		// WasPressedLong
		}
		else // Short pressed
		{
			me->isPressedShort	= true;		// IsPressedShort
			me->isPressedLong	= false;	// IsPressedLong
		}
	}
	// #3 isPressed - Low line
	else
	{
		if (me->isPressedOld) // Falling edge detected
		{
			if (AlxTimSw_IsTimeout_ms(&me->timPressed, me->longTime_ms) == false) 
			{
				me->wasPressedShort	= true;		// WasPressedShort
			}
			me->wasReleased		= true;		// WasReleased
			me->isPressedShort	= false;	// IsPressedShort
			me->isPressedLong	= false;	// IsPressedLong
		}
		AlxTimSw_Start(&me->timPressed); // Reset timer
	}
	// #4 Copy value to isPressedOld
	me->isPressedOld = me->isPressed;
}
bool AlxBtn_IsPressed(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->isPressed;
}
bool AlxBtn_WasPressed(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->wasPressed;
}
bool AlxBtn_WasReleased(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->wasReleased;
}
bool AlxBtn_IsPressedShort(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->isPressedShort;
}
bool AlxBtn_WasPressedShort(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->wasPressedShort;
}
bool AlxBtn_IsPressedLong(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->isPressedLong;
}
bool AlxBtn_WasPressedLong(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	return me->wasPressedLong;
}
void AlxBtn_ClearWasPressed(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	me->wasPressed = false;
}
void AlxBtn_ClearWasReleased(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	me->wasReleased = false;
}
void AlxBtn_ClearWasPressedShort(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	me->wasPressedShort = false;
}
void AlxBtn_ClearWasPressedLong(AlxBtn* me)
{
	ALX_BTN_ASSERT(me->wasCtorCalled == true);
	
	me->wasPressedLong = false;
}
