/**
  ******************************************************************************
  * @file alxTpl.c
  * @brief Auralix C Library - ALX Template Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTpl.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxTpl_Ctor
(
	AlxTpl* me,
	bool valInitial,
	float stableTrueTime_ms,
	float stableFalseTime_ms
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);
	
	// Parameters
	me->stableTrueTime_ms = stableTrueTime_ms;
	me->stableFalseTime_ms = stableFalseTime_ms;
	
	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;
	
	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
bool AlxTpl_Process(AlxTpl* me, bool in)
{
	me->valRaw = in;
	if (me->valRaw != me->valFiltered) // Unstable state detected
	{
		float unstableTime_ms = AlxTimSw_Get_us(&me->tim) / 1000.f;
		
		if (me->valRaw == true) // We are in false state and it becomes unstable, because "in = true"
		{
			if (unstableTime_ms > me->stableTrueTime_ms)
			{
				me->valFiltered = true; // Change output filtered value
				AlxTimSw_Start(&me->tim);
			}
		}
		else // We are in true state and it becomes unstable, bacause "in = false"
		{
			if (unstableTime_ms > me->stableFalseTime_ms)
			{
				me->valFiltered = false; // Change output filtered value
				AlxTimSw_Start(&me->tim);
			}
		}
	}
	else
	{
		AlxTimSw_Start(&me->tim);
	}
	
	return me->valFiltered;
}
