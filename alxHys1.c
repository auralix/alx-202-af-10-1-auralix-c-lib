/**
  ******************************************************************************
  * @file alxHys1.c
  * @brief Auralix C Library - ALX Hysteresis 2 Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxHys1.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxHys1_Ctor
(
	AlxHys1* me,
	float high,
	float low
)
{
	// Parameters
	me->high = high;
	me->low = low;

	// SM
	me->st = AlxHys1_StHigh;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
AlxHys1_St AlxHys1_Process(AlxHys1* me, float in)
{
	switch(me->st)
	{
		case AlxHys1_StHigh:
		{
			// #1 Transition
			if(in <= me->low)
				me->st = AlxHys1_StLow;
			// #2 Do
			else
				me->st = AlxHys1_StHigh;
			// #3 Return
			return me->st;
		}
		case AlxHys1_StLow:
		{
			// #1 Transition
			if(in >= me->high)
				me->st = AlxHys1_StHigh;
			// #2 Do
			else
				me->st = AlxHys1_StLow;
			// #3 Return
			return me->st;
		}
		default:
		{
			ALX_HYS1_ASSERT(false); // We should never get here
			break;
		}
	}
	
	ALX_HYS1_ASSERT(false); // We should never get here
	return ALX_NULL;
}
