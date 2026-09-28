/**
  ******************************************************************************
  * @file		alxMathTestHelpers.c
  * @brief		Auralix C Library - ALX Arithmetic Modules - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructors and destructors for the library's pure
  * signal-conditioning modules, so the Python test layer (ctypes) never mirrors
  * a struct layout. AlxAvg keeps a CALLER-owned sample buffer and its
  * constructor does not touch it, so two constructors are offered here: one
  * over a zeroed buffer, and one over a buffer deliberately filled first - the
  * difference is the module's warm-up behaviour, which is what P17x measures.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAvg.h"
#include "alxHys1.h"
#include "alxHys2.h"
#include "alxMath.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxHys1* AlxHys1Test_New(float high, float low);
void AlxHys1Test_Delete(AlxHys1* me);
AlxHys2* AlxHys2Test_New(float topHigh, float topLow, float botHigh, float botLow);
void AlxHys2Test_Delete(AlxHys2* me);
AlxAvg* AlxAvgTest_New(uint32_t buffLen, uint32_t shiftThreshold, float fill);
void AlxAvgTest_Delete(AlxAvg* me);
AlxMath* AlxMathTest_New(void);
void AlxMathTest_Delete(AlxMath* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxHys1* AlxHys1Test_New(float high, float low)
{
	AlxHys1* me = (AlxHys1*)malloc(sizeof(AlxHys1));
	if (me == NULL) { return NULL; }
	AlxHys1_Ctor(me, high, low);
	return me;
}
void AlxHys1Test_Delete(AlxHys1* me)
{
	free(me);
}
AlxHys2* AlxHys2Test_New(float topHigh, float topLow, float botHigh, float botLow)
{
	AlxHys2* me = (AlxHys2*)malloc(sizeof(AlxHys2));
	if (me == NULL) { return NULL; }
	AlxHys2_Ctor(me, topHigh, topLow, botHigh, botLow);
	return me;
}
void AlxHys2Test_Delete(AlxHys2* me)
{
	free(me);
}
AlxAvg* AlxAvgTest_New(uint32_t buffLen, uint32_t shiftThreshold, float fill)
{
	AlxAvg* me = (AlxAvg*)malloc(sizeof(AlxAvg));
	float* buff = (float*)malloc(buffLen * sizeof(float));
	if ((me == NULL) || (buff == NULL)) { free(me); free(buff); return NULL; }
	for (uint32_t i = 0; i < buffLen; i++)
	{
		buff[i] = fill;
	}
	AlxAvg_Ctor(me, buff, buffLen, shiftThreshold);
	return me;
}
void AlxAvgTest_Delete(AlxAvg* me)
{
	if (me != NULL)
	{
		free(me->buff);
	}
	free(me);
}
AlxMath* AlxMathTest_New(void)
{
	AlxMath* me = (AlxMath*)malloc(sizeof(AlxMath));
	if (me == NULL) { return NULL; }
	AlxMath_Ctor(me);
	return me;
}
void AlxMathTest_Delete(AlxMath* me)
{
	free(me);
}
