/**
  ******************************************************************************
  * @file alxBts724g.c
  * @brief Auralix C Library - ALX HighSide Switch BTS724G Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBts724g.h"


//******************************************************************************
// Constructor
//******************************************************************************
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
void AlxBts724g_SetOut(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);
	AlxIoPin_Set(me->do_HS_IN);

	me->isOutSet = true;
}
void AlxBts724g_ResetOut(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Reset(me->do_HS_IN);
	me->isOutSet = false;
}
void AlxBts724g_WriteOut(AlxBts724g* me, bool val)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	AlxIoPin_Write(me->do_HS_IN, val);
	me->isOutSet = val;
}
bool AlxBts724g_IsOpenLoadDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	return me->isOpenLoadDetected;
}
bool AlxBts724g_IsOverTempDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	return me->isOverTempDetected;
}
bool AlxBts724g_WasOpenLoadDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	bool wasOpenLoadDetected = me->wasOpenLoadDetected;
	me->wasOpenLoadDetected = false;	// Resets flag wasOpenLoadDetectedRaw

	return wasOpenLoadDetected;
}
bool AlxBts724g_WasOverTempDetected(AlxBts724g* me)
{
	ALX_BTS724G_ASSERT(me->isInit == true);
	ALX_BTS724G_ASSERT(me->wasCtorCalled == true);

	bool wasOverTempDetected = me->wasOverTempDetected;
	me->wasOverTempDetected = false;	// Resets flag wasOverTempDetected

	return wasOverTempDetected;
}
