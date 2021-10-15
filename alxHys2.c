/**
  ******************************************************************************
  * @file alxHys2.c
  * @brief Auralix C Library - ALX Hysteresis 2 Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxHys2.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys2_Ctor
(
	AlxHys2* me,
	float topHigh,
	float topLow,
	float botHigh,
	float botLow
)
{
	// Parameters
	me->topHigh = topHigh;
	me->topLow = topLow;
	me->botHigh = botHigh;
	me->botLow = botLow;

	// SM
	me->st = AlxHys2_StMid;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
AlxHys2_St AlxHys2_Process(AlxHys2* me, float in)
{
	switch(me->st)
	{
		case AlxHys2_StTop:
		{
			// #1 Transition
			if(in <= me->topLow)
				me->st = AlxHys2_StMid;
			// #2 Do
			else
				me->st = AlxHys2_StTop;
			// #3 Return
			return me->st;
		}
		case AlxHys2_StMid:
		{
			// #1 Transition
			if(in >= me->topHigh)
				me->st = AlxHys2_StTop;
			// #2 Transition
			else if(in <= me->botLow)
				me->st = AlxHys2_StBot;
			// #3 Do
			else
				me->st = AlxHys2_StMid;
			// #3 Return
			return me->st;
		}
		case AlxHys2_StBot:
		{
			// #1 Transition
			if(in >= me->botHigh)
				me->st = AlxHys2_StMid;
			// #2 Do
			else
				me->st = AlxHys2_StBot;
			// #3 Return
			return me->st;
		}
		default:
		{
			ALX_HYS2_ASSERT(false); // We should never get here
			break;
		}
	}
	
	ALX_HYS2_ASSERT(false); // We should never get here
	return ALX_NULL;
}
