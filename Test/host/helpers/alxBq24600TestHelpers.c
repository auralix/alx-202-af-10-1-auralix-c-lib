/**
  ******************************************************************************
  * @file		alxBq24600TestHelpers.c
  * @brief		Auralix C Library - ALX BQ24600 Battery Charger - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its three pins - the enable it drives, and the
  * power-good and status pins it reads - so the helper owns all three and hands
  * a test each of them by name. The detection times the constructor chose are
  * readable too: they are the part's own blink specification, not the test's.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxBq24600.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxBq24600 bq;
	AlxIoPin en;
	AlxIoPin npg;
	AlxIoPin stat;
} AlxBq24600Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxBq24600Test_Obj* AlxBq24600Test_New(void);
void AlxBq24600Test_Delete(AlxBq24600Test_Obj* me);
AlxBq24600* AlxBq24600Test_Bq(AlxBq24600Test_Obj* me);
AlxIoPin* AlxBq24600Test_EnPin(AlxBq24600Test_Obj* me);
AlxIoPin* AlxBq24600Test_NpgPin(AlxBq24600Test_Obj* me);
AlxIoPin* AlxBq24600Test_StatPin(AlxBq24600Test_Obj* me);
uint32_t AlxBq24600Test_Window_ms(AlxBq24600Test_Obj* me);
uint32_t AlxBq24600Test_ErrTransitions(AlxBq24600Test_Obj* me);
uint32_t AlxBq24600Test_HalfPeriodMin_ms(AlxBq24600Test_Obj* me);
uint32_t AlxBq24600Test_HalfPeriodMax_ms(AlxBq24600Test_Obj* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxBq24600Test_Obj* AlxBq24600Test_New(void)
{
	AlxBq24600Test_Obj* me = (AlxBq24600Test_Obj*)calloc(1, sizeof(AlxBq24600Test_Obj));
	if (me == NULL) { return NULL; }
	AlxBq24600_Ctor(&me->bq, &me->en, &me->npg, &me->stat);
	return me;
}

void AlxBq24600Test_Delete(AlxBq24600Test_Obj* me)
{
	free(me);
}

AlxBq24600* AlxBq24600Test_Bq(AlxBq24600Test_Obj* me)					{ return &me->bq; }
AlxIoPin* AlxBq24600Test_EnPin(AlxBq24600Test_Obj* me)					{ return &me->en; }
AlxIoPin* AlxBq24600Test_NpgPin(AlxBq24600Test_Obj* me)				{ return &me->npg; }
AlxIoPin* AlxBq24600Test_StatPin(AlxBq24600Test_Obj* me)				{ return &me->stat; }
uint32_t AlxBq24600Test_Window_ms(AlxBq24600Test_Obj* me)				{ return me->bq.STATUS_DETECTION_WINDOW_TIME_ms; }
uint32_t AlxBq24600Test_ErrTransitions(AlxBq24600Test_Obj* me)			{ return me->bq.ERROR_STATUS_DETECTION_TRANSITIONS; }
uint32_t AlxBq24600Test_HalfPeriodMin_ms(AlxBq24600Test_Obj* me)		{ return me->bq.STATUS_BLINK_HALFPERIOD_TIME_MIN_ms; }
uint32_t AlxBq24600Test_HalfPeriodMax_ms(AlxBq24600Test_Obj* me)		{ return me->bq.STATUS_BLINK_HALFPERIOD_TIME_MAX_ms; }
