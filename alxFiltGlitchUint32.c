/**
  ******************************************************************************
  * @file alxFiltGlitchUint32.c
  * @brief Auralix C Library - ALX Fliter Glitch Uint32 Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxFiltGlitchUint32.h"


//******************************************************************************
// Constructor
//******************************************************************************
void AlxFiltGlitchUint32_Ctor
(
	AlxFiltGlitchUint32* me,
	uint32_t valInitial,	// Initial output filtered value
	float stableTime_ms		// Time that val must be stable to change output state to new val
)
{
	// Ctor
	AlxTimSw_Ctor(&me->tim, true);
	
	// Parameters
	me->valInitial = valInitial;
	me->stableTime_ms = stableTime_ms;
	
	// Variables
	me->valOld = valInitial;
	me->valFiltered = valInitial;
	
	// Info
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************
uint32_t AlxFiltGlitchUint32_Process(AlxFiltGlitchUint32* me, uint32_t valNew)
{
	if (me->valOld != valNew) // Change detected
	{
		me->valOld = valNew;
		AlxTimSw_Start(&me->tim);
	}
	float stableTime_ms = AlxTimSw_Get_us(&me->tim) / 1000.f;
	if (stableTime_ms >= me->stableTime_ms)
	{
		me->valFiltered = valNew; // Change output filtered value
	}
	return me->valFiltered;
}
