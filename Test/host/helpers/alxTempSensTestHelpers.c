/**
  ******************************************************************************
  * @file		alxTempSensTestHelpers.c
  * @brief		Auralix C Library - ALX RTD Temperature Sensor - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The sensor is a converter, a divider and a lookup table. The module keeps
  * POINTERS to the converter and the table, and the table keeps a pointer to
  * the caller's points, so the helper owns all four and hands a test one handle:
  * the object, and the converter it reads, by index.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTempSensRtdVdiv.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxTempSensRtdVdiv sens;
	AlxAdc adc;
	AlxInterpLin interpLin;
	float* xPoints;
	float* yPoints;
} AlxTempSensTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxTempSensTest_Obj* AlxTempSensTest_New(const float* resPoints_kOhm, const float* tempPoints_degC, uint32_t numOfPoints, bool isRising, uint32_t chVin, uint32_t chVout, bool isResRtdLow, float resOther_kOhm);
void AlxTempSensTest_Delete(AlxTempSensTest_Obj* me);
AlxTempSensRtdVdiv* AlxTempSensTest_Sens(AlxTempSensTest_Obj* me);
AlxAdc* AlxTempSensTest_Adc(AlxTempSensTest_Obj* me);
int32_t AlxTempSensTest_Status_Ok(void);
int32_t AlxTempSensTest_Status_ErrMin(void);
int32_t AlxTempSensTest_Status_ErrMax(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxTempSensTest_Obj* AlxTempSensTest_New(const float* resPoints_kOhm, const float* tempPoints_degC, uint32_t numOfPoints, bool isRising, uint32_t chVin, uint32_t chVout, bool isResRtdLow, float resOther_kOhm)
{
	AlxTempSensTest_Obj* me = (AlxTempSensTest_Obj*)calloc(1, sizeof(AlxTempSensTest_Obj));
	if (me == NULL) { return NULL; }

	me->xPoints = (float*)calloc(numOfPoints, sizeof(float));
	me->yPoints = (float*)calloc(numOfPoints, sizeof(float));
	if (me->xPoints == NULL || me->yPoints == NULL) { return NULL; }
	memcpy(me->xPoints, resPoints_kOhm, numOfPoints * sizeof(float));
	memcpy(me->yPoints, tempPoints_degC, numOfPoints * sizeof(float));

	AlxInterpLin_Ctor(&me->interpLin, me->xPoints, me->yPoints, numOfPoints, isRising);
	AlxTempSensRtdVdiv_Ctor(&me->sens, &me->adc, &me->interpLin, (Alx_Ch)chVin, (Alx_Ch)chVout, isResRtdLow, resOther_kOhm);
	return me;
}

void AlxTempSensTest_Delete(AlxTempSensTest_Obj* me)
{
	if (me == NULL) { return; }
	free(me->xPoints);
	free(me->yPoints);
	free(me);
}

AlxTempSensRtdVdiv* AlxTempSensTest_Sens(AlxTempSensTest_Obj* me)
{
	return &me->sens;
}

AlxAdc* AlxTempSensTest_Adc(AlxTempSensTest_Obj* me)
{
	return &me->adc;
}

int32_t AlxTempSensTest_Status_Ok(void)		{ return (int32_t)Alx_Ok; }
int32_t AlxTempSensTest_Status_ErrMin(void)	{ return (int32_t)AlxRange_ErrMin; }
int32_t AlxTempSensTest_Status_ErrMax(void)	{ return (int32_t)AlxRange_ErrMax; }
