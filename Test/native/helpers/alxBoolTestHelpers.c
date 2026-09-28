/**
  ******************************************************************************
  * @file		alxBoolTestHelpers.c
  * @brief		Auralix C Library - ALX Bool Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor and destructor, so the Python test layer (ctypes)
  * never mirrors the AlxBool struct - it embeds a glitch filter and two
  * software timers, and its size is not the test's business. All testing goes
  * through the public alxBool.h API.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBool.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxBool* AlxBoolTest_New(bool valInitial, float trueShortTime_ms, float trueLongTime_ms, float falseShortTime_ms, float falseLongTime_ms, float stableTrueTime_ms, float stableFalseTime_ms);
void AlxBoolTest_Delete(AlxBool* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxBool* AlxBoolTest_New(bool valInitial, float trueShortTime_ms, float trueLongTime_ms, float falseShortTime_ms, float falseLongTime_ms, float stableTrueTime_ms, float stableFalseTime_ms)
{
	AlxBool* me = (AlxBool*)malloc(sizeof(AlxBool));
	if (me == NULL) { return NULL; }
	AlxBool_Ctor(me, valInitial, trueShortTime_ms, trueLongTime_ms, falseShortTime_ms, falseLongTime_ms, stableTrueTime_ms, stableFalseTime_ms);
	return me;
}
void AlxBoolTest_Delete(AlxBool* me)
{
	free(me);
}
