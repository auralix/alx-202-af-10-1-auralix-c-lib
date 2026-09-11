/**
  ******************************************************************************
  * @file		alxAudioVolTestHelpers.c
  * @brief		Auralix C Library - ALX Audio Volume - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * An opaque-handle constructor and destructor, plus the one internal number a
  * test has to know about: the highest percentage the object will accept. It is
  * computed by the constructor out of the volume range, and Set_pct asserts
  * against it, so a test that guessed it would be testing its own guess.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAudioVol.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxAudioVol* AlxAudioVolTest_New(float volMin_dB, float volMax_dB);
void AlxAudioVolTest_Delete(AlxAudioVol* me);
float AlxAudioVolTest_PctMax(AlxAudioVol* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxAudioVol* AlxAudioVolTest_New(float volMin_dB, float volMax_dB)
{
	AlxAudioVol* me = (AlxAudioVol*)malloc(sizeof(AlxAudioVol));
	if (me == NULL) { return NULL; }
	AlxAudioVol_Ctor(me, volMin_dB, volMax_dB);
	return me;
}

void AlxAudioVolTest_Delete(AlxAudioVol* me)
{
	free(me);
}

float AlxAudioVolTest_PctMax(AlxAudioVol* me)
{
	return me->pctMax;
}
