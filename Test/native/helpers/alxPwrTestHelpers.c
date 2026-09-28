/**
  ******************************************************************************
  * @file		alxPwrTestHelpers.c
  * @brief		Auralix C Library - ALX Power Supervisor - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * An opaque-handle constructor and destructor, and the three intermediate
  * numbers the supervisor computes on its way to a yes or no: the voltage it
  * reconstructed, the hysteresis state it landed in, and the unfiltered answer.
  * A test asserts the answer; these say WHY when it is not the expected one,
  * and they are the only way to tell "out of range" from "filtered out".
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPwr.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxPwr* AlxPwrTest_New(float resHigh_kOhm, float resLow_kOhm, float topHigh_V, float topLow_V, float botHigh_V, float botLow_V, bool valInitial, float stableTrue_ms, float stableFalse_ms);
void AlxPwrTest_Delete(AlxPwr* me);
float AlxPwrTest_Val_V(AlxPwr* me);
int32_t AlxPwrTest_HysSt(AlxPwr* me);
bool AlxPwrTest_IsInRangeRaw(AlxPwr* me);
int32_t AlxPwrTest_HysSt_Top(void);
int32_t AlxPwrTest_HysSt_Mid(void);
int32_t AlxPwrTest_HysSt_Bot(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxPwr* AlxPwrTest_New(float resHigh_kOhm, float resLow_kOhm, float topHigh_V, float topLow_V, float botHigh_V, float botLow_V, bool valInitial, float stableTrue_ms, float stableFalse_ms)
{
	AlxPwr* me = (AlxPwr*)malloc(sizeof(AlxPwr));
	if (me == NULL) { return NULL; }
	AlxPwr_Ctor(me, resHigh_kOhm, resLow_kOhm, topHigh_V, topLow_V, botHigh_V, botLow_V, valInitial, stableTrue_ms, stableFalse_ms);
	return me;
}

void AlxPwrTest_Delete(AlxPwr* me)
{
	free(me);
}

float AlxPwrTest_Val_V(AlxPwr* me)
{
	return me->val_V;
}

int32_t AlxPwrTest_HysSt(AlxPwr* me)
{
	return (int32_t)me->hys2_St;
}

bool AlxPwrTest_IsInRangeRaw(AlxPwr* me)
{
	return me->isInRangeRaw;
}

int32_t AlxPwrTest_HysSt_Top(void)	{ return (int32_t)AlxHys2_StTop; }
int32_t AlxPwrTest_HysSt_Mid(void)	{ return (int32_t)AlxHys2_StMid; }
int32_t AlxPwrTest_HysSt_Bot(void)	{ return (int32_t)AlxHys2_StBot; }
