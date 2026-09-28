/**
  ******************************************************************************
  * @file		alxLinFunTestHelpers.c
  * @brief		Auralix C Library - ALX Mapping Modules - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructors and destructors plus status enum getters, so the
  * Python test layer (ctypes) never mirrors a struct layout or hardcodes an
  * Alx_Status value. AlxInterpLin keeps CALLER-owned point arrays, so the
  * constructor here copies the caller's points into memory it owns and the
  * destructor frees them with the object.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxInterpLin.h"
#include "alxLinFun.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxLinFun* AlxLinFunTest_New(float x1, float y1, float x2, float y2, float min, float max, bool isLimitOnX);
void AlxLinFunTest_Delete(AlxLinFun* me);
AlxLinFunInt* AlxLinFunIntTest_New(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t min, int32_t max, bool isLimitOnX);
void AlxLinFunIntTest_Delete(AlxLinFunInt* me);
AlxInterpLin* AlxInterpLinTest_New(const float* xPoints, const float* yPoints, uint32_t numOfPoints, bool isRising);
void AlxInterpLinTest_Delete(AlxInterpLin* me);
int32_t AlxLinFunTest_Status_Ok(void);
int32_t AlxLinFunTest_Status_ErrMin(void);
int32_t AlxLinFunTest_Status_ErrMax(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxLinFun* AlxLinFunTest_New(float x1, float y1, float x2, float y2, float min, float max, bool isLimitOnX)
{
	AlxLinFun* me = (AlxLinFun*)malloc(sizeof(AlxLinFun));
	if (me == NULL) { return NULL; }
	AlxLinFun_Ctor(me, x1, y1, x2, y2, min, max, isLimitOnX);
	return me;
}
void AlxLinFunTest_Delete(AlxLinFun* me)
{
	free(me);
}
AlxLinFunInt* AlxLinFunIntTest_New(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t min, int32_t max, bool isLimitOnX)
{
	AlxLinFunInt* me = (AlxLinFunInt*)malloc(sizeof(AlxLinFunInt));
	if (me == NULL) { return NULL; }
	AlxLinFunInt_Ctor(me, x1, y1, x2, y2, min, max, isLimitOnX);
	return me;
}
void AlxLinFunIntTest_Delete(AlxLinFunInt* me)
{
	free(me);
}
AlxInterpLin* AlxInterpLinTest_New(const float* xPoints, const float* yPoints, uint32_t numOfPoints, bool isRising)
{
	AlxInterpLin* me = (AlxInterpLin*)malloc(sizeof(AlxInterpLin));
	float* x = (float*)malloc(numOfPoints * sizeof(float));
	float* y = (float*)malloc(numOfPoints * sizeof(float));
	if ((me == NULL) || (x == NULL) || (y == NULL))
	{
		free(me);
		free(x);
		free(y);
		return NULL;
	}
	for (uint32_t i = 0; i < numOfPoints; i++)
	{
		x[i] = xPoints[i];
		y[i] = yPoints[i];
	}
	AlxInterpLin_Ctor(me, x, y, numOfPoints, isRising);
	return me;
}
void AlxInterpLinTest_Delete(AlxInterpLin* me)
{
	if (me != NULL)
	{
		free(me->xPointArr);
		free(me->yPointArr);
	}
	free(me);
}
int32_t AlxLinFunTest_Status_Ok(void)
{
	return (int32_t)Alx_Ok;
}
int32_t AlxLinFunTest_Status_ErrMin(void)
{
	return (int32_t)AlxRange_ErrMin;
}
int32_t AlxLinFunTest_Status_ErrMax(void)
{
	return (int32_t)AlxRange_ErrMax;
}
