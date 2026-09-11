/**
  ******************************************************************************
  * @file		alxBts724gTestHelpers.c
  * @brief		Auralix C Library - ALX BTS724G High Side Switch - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its two pins - the one it drives and the one it
  * reads the switch's status from - so the helper owns both and hands a test
  * each of them by name. The filter times the constructor chose are readable
  * too: they are the part's own specification, not the test's, and a test that
  * restated them would be testing its own copy.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBts724g.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxBts724g bts;
	AlxIoPin out;
	AlxIoPin status;
} AlxBts724gTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxBts724gTest_Obj* AlxBts724gTest_New(void);
void AlxBts724gTest_Delete(AlxBts724gTest_Obj* me);
AlxBts724g* AlxBts724gTest_Bts(AlxBts724gTest_Obj* me);
AlxIoPin* AlxBts724gTest_OutPin(AlxBts724gTest_Obj* me);
AlxIoPin* AlxBts724gTest_StatusPin(AlxBts724gTest_Obj* me);
float AlxBts724gTest_OpenLoadTrue_ms(AlxBts724gTest_Obj* me);
float AlxBts724gTest_OverTempTrue_ms(AlxBts724gTest_Obj* me);
float AlxBts724gTest_ClearTime_ms(AlxBts724gTest_Obj* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxBts724gTest_Obj* AlxBts724gTest_New(void)
{
	AlxBts724gTest_Obj* me = (AlxBts724gTest_Obj*)calloc(1, sizeof(AlxBts724gTest_Obj));
	if (me == NULL) { return NULL; }
	AlxBts724g_Ctor(&me->bts, &me->out, &me->status);
	return me;
}

void AlxBts724gTest_Delete(AlxBts724gTest_Obj* me)
{
	free(me);
}

AlxBts724g* AlxBts724gTest_Bts(AlxBts724gTest_Obj* me)			{ return &me->bts; }
AlxIoPin* AlxBts724gTest_OutPin(AlxBts724gTest_Obj* me)			{ return &me->out; }
AlxIoPin* AlxBts724gTest_StatusPin(AlxBts724gTest_Obj* me)		{ return &me->status; }
float AlxBts724gTest_OpenLoadTrue_ms(AlxBts724gTest_Obj* me)	{ return me->bts.stableTrueTime_openLoad_ms; }
float AlxBts724gTest_OverTempTrue_ms(AlxBts724gTest_Obj* me)	{ return me->bts.stableTrueTime_overTemp_ms; }
float AlxBts724gTest_ClearTime_ms(AlxBts724gTest_Obj* me)		{ return me->bts.stableFalseTime_openLoad_ms; }
