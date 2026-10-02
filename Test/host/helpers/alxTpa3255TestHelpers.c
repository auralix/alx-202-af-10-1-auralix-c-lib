/**
  ******************************************************************************
  * @file		alxTpa3255TestHelpers.c
  * @brief		Auralix C Library - ALX TPA3255 Audio Amplifier - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its three pins - the reset it drives, and the
  * fault and clip/over-temperature warning it reads - so the helper owns all
  * three and hands a test each of them by name. The re-enable wait is the
  * constructor's argument; the reset pulse and the settle time after it are
  * the driver's own and are readable.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTpa3255.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxTpa3255 tpa;
	AlxIoPin nReset;
	AlxIoPin nFault;
	AlxIoPin nClipOtw;
} AlxTpa3255Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxTpa3255Test_Obj* AlxTpa3255Test_New(float waitTime_ReEnable_ms);
void AlxTpa3255Test_Delete(AlxTpa3255Test_Obj* me);
AlxTpa3255* AlxTpa3255Test_Tpa(AlxTpa3255Test_Obj* me);
AlxIoPin* AlxTpa3255Test_NResetPin(AlxTpa3255Test_Obj* me);
AlxIoPin* AlxTpa3255Test_NFaultPin(AlxTpa3255Test_Obj* me);
AlxIoPin* AlxTpa3255Test_NClipOtwPin(AlxTpa3255Test_Obj* me);
float AlxTpa3255Test_LowLevel_ms(AlxTpa3255Test_Obj* me);
float AlxTpa3255Test_Waiting_ms(AlxTpa3255Test_Obj* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxTpa3255Test_Obj* AlxTpa3255Test_New(float waitTime_ReEnable_ms)
{
	AlxTpa3255Test_Obj* me = (AlxTpa3255Test_Obj*)calloc(1, sizeof(AlxTpa3255Test_Obj));
	if (me == NULL) { return NULL; }
	AlxTpa3255_Ctor(&me->tpa, &me->nReset, &me->nFault, &me->nClipOtw, waitTime_ReEnable_ms);
	return me;
}

void AlxTpa3255Test_Delete(AlxTpa3255Test_Obj* me)
{
	free(me);
}

AlxTpa3255* AlxTpa3255Test_Tpa(AlxTpa3255Test_Obj* me)				{ return &me->tpa; }
AlxIoPin* AlxTpa3255Test_NResetPin(AlxTpa3255Test_Obj* me)			{ return &me->nReset; }
AlxIoPin* AlxTpa3255Test_NFaultPin(AlxTpa3255Test_Obj* me)			{ return &me->nFault; }
AlxIoPin* AlxTpa3255Test_NClipOtwPin(AlxTpa3255Test_Obj* me)		{ return &me->nClipOtw; }
float AlxTpa3255Test_LowLevel_ms(AlxTpa3255Test_Obj* me)			{ return me->tpa.lowLevelTime_ms; }
float AlxTpa3255Test_Waiting_ms(AlxTpa3255Test_Obj* me)				{ return me->tpa.waitTime_Waitning_ms; }
