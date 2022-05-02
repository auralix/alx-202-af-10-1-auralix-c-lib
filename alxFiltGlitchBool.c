/**
  ******************************************************************************
  * @file alxFiltGlitchBool.c
  * @brief Auralix C Library - ALX Fliter Glitch Bool Module
  * @version $LastChangedRevision: 5398 $
  * @date $LastChangedDate: 2021-06-11 11:19:51 +0200 (Fri, 11 Jun 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxFiltGlitchBool.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchBool_Ctor
(
	AlxFiltGlitchBool* me,
	bool valInitial,
	float stableTrueTime_ms,
	float stableFalseTime_ms
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);

	// Parameters
	me->stableTrueTime_us = (uint64_t)(stableTrueTime_ms * 1000.f);
	me->stableFalseTime_us = (uint64_t)(stableFalseTime_ms * 1000.f);

	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;

	// Info
	me->wasCtorCalled = true;
}

void AlxFiltGlitchBool_Ctor_us
(
	AlxFiltGlitchBool* me,
	bool valInitial,
	uint64_t stableTrueTime_us,
	uint64_t stableFalseTime_us
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);

	// Parameters
	me->stableTrueTime_us = stableTrueTime_us;
	me->stableFalseTime_us = stableFalseTime_us;

	// Variables
	me->valRaw = valInitial;
	me->valFiltered = valInitial;

	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
bool AlxFiltGlitchBool_Process(AlxFiltGlitchBool* me, bool in)
{
	me->valRaw = in;
	if (me->valRaw != me->valFiltered) // Unstable state detected
	{
		uint64_t unstableTime_us = AlxTimSw_Get_us(&me->tim);

		if (me->valRaw == true) // We are in false state and it becomes unstable, because "in = true"
		{
			if (unstableTime_us > me->stableTrueTime_us)
			{
				me->valFiltered = true; // Change output filtered value
				AlxTimSw_Start(&me->tim);
			}
		}
		else // We are in true state and it becomes unstable, bacause "in = false"
		{
			if (unstableTime_us > me->stableFalseTime_us)
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
