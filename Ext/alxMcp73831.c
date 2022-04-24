//******************************************************************************
// @file alxMcp73831.c
// @brief Auralix C Library - ALX Battery Charger MCP73831 Module
// @copyright Copyright (C) 2022 Auralix d.o.o. All rights reserved.
//******************************************************************************


//******************************************************************************
// Includes
//******************************************************************************
#include <alxMcp73831.h>


//******************************************************************************
// Constructor
//******************************************************************************
void AlxMcp73831_Ctor
(
	AlxMcp73831* me,
	AlxIoPin* di_STAT
)
{
	// Parameters
	me->di_STAT = di_STAT;

	// Variables

	// Info
	me->wasCtorCalled = true;
	me->isInit = false;
}


//******************************************************************************
// Functions
//******************************************************************************
void AlxMcp73831_Init(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == false);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Init GPIO
	AlxIoPin_Init(me->di_STAT);

	// #3 Set isInit
	me->isInit = true;
}
void AlxMcp73831_DeInit(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 DeInit GPIO
	AlxIoPin_DeInit(me->di_STAT);

	// #3 Reset isInit
	me->isInit = false;
}
bool AlxMcp73831_IsBatCharging(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	return (AlxIoPin_Read_TriState(me->di_STAT) == AlxIoPin_TriState_Lo) ? true : false;
}
bool AlxMcp73831_IsBatFull(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	return (AlxIoPin_Read_TriState(me->di_STAT) == AlxIoPin_TriState_Hi) ? true : false;
}
bool AlxMcp73831_IsBatNotPresent(AlxMcp73831* me)
{
	// #1 Assert
	ALX_MCP73831_ASSERT(me->isInit == true);
	ALX_MCP73831_ASSERT(me->wasCtorCalled == true);

	// #2 Return
	return (AlxIoPin_Read_TriState(me->di_STAT) == AlxIoPin_TriState_HiZ) ? true : false;
}
