/**
  ******************************************************************************
  * @file		alxRotSwTestHelpers.c
  * @brief		Auralix C Library - ALX Rotary Switch - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A rotary switch is a handful of IO pins read together. The constructor takes
  * a CALLER-owned array of pin pointers and keeps the pointer, so the helper
  * owns both the pins and the array and hands a test one pin at a time by
  * index - the test never builds an array of handles across the ctypes boundary
  * and never learns what an AlxIoPin looks like.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxRotSw.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxRotSw rotSw;
	AlxIoPin* pin[ALX_ROT_SW_IO_PIN_VAL_ARR_LEN];
	AlxIoPin* pinArr[ALX_ROT_SW_IO_PIN_VAL_ARR_LEN];
	uint8_t numOfPins;
} AlxRotSwTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxRotSwTest_Obj* AlxRotSwTest_New(uint8_t numOfPins, int32_t codeType);
void AlxRotSwTest_Delete(AlxRotSwTest_Obj* me);
AlxRotSw* AlxRotSwTest_RotSw(AlxRotSwTest_Obj* me);
AlxIoPin* AlxRotSwTest_Pin(AlxRotSwTest_Obj* me, uint8_t index);
int32_t AlxRotSwTest_CodeType_Real(void);
int32_t AlxRotSwTest_CodeType_Complement(void);
int32_t AlxRotSwTest_CodeType_Gray(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxRotSwTest_Obj* AlxRotSwTest_New(uint8_t numOfPins, int32_t codeType)
{
	if (numOfPins > ALX_ROT_SW_IO_PIN_VAL_ARR_LEN) { return NULL; }

	AlxRotSwTest_Obj* me = (AlxRotSwTest_Obj*)calloc(1, sizeof(AlxRotSwTest_Obj));
	if (me == NULL) { return NULL; }

	for (uint8_t i = 0; i < numOfPins; i++)
	{
		me->pin[i] = (AlxIoPin*)calloc(1, sizeof(AlxIoPin));
		if (me->pin[i] == NULL) { return NULL; }
		me->pinArr[i] = me->pin[i];
	}
	me->numOfPins = numOfPins;

	AlxRotSw_Ctor(&me->rotSw, me->pinArr, numOfPins, (AlxRotSw_CodeType)codeType);
	return me;
}

void AlxRotSwTest_Delete(AlxRotSwTest_Obj* me)
{
	if (me == NULL) { return; }
	for (uint8_t i = 0; i < me->numOfPins; i++) { free(me->pin[i]); }
	free(me);
}

AlxRotSw* AlxRotSwTest_RotSw(AlxRotSwTest_Obj* me)
{
	return &me->rotSw;
}

AlxIoPin* AlxRotSwTest_Pin(AlxRotSwTest_Obj* me, uint8_t index)
{
	if (index >= me->numOfPins) { return NULL; }
	return me->pin[index];
}

int32_t AlxRotSwTest_CodeType_Real(void)		{ return (int32_t)AlxRotSw_CodeType_Real; }
int32_t AlxRotSwTest_CodeType_Complement(void)	{ return (int32_t)AlxRotSw_CodeType_Complement; }
int32_t AlxRotSwTest_CodeType_Gray(void)		{ return (int32_t)AlxRotSw_CodeType_Gray; }
