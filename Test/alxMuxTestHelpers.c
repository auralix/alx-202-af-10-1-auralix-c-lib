/**
  ******************************************************************************
  * @file		alxMuxTestHelpers.c
  * @brief		Auralix C Library - ALX Multiplexer - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A multiplexer is an enable pin and a handful of select pins. The constructor
  * takes a CALLER-owned array of select pin pointers and keeps the pointer, so
  * the helper owns the pins, the array and the object together, and hands a
  * test one pin at a time by index - the test never builds an array of handles
  * across the ctypes boundary and never learns what an AlxIoPin looks like.
  *
  * Same shape as alxRotSwTestHelpers.c, which owns pins for the same reason.
  ******************************************************************************
  **/


//******************************************************************************
// Includes
//******************************************************************************
#include "alxMux.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxMux mux;
	AlxIoPin* enPin;
	AlxIoPin* selPin[ALX_MUX_IO_PIN_VAL_ARR_LEN];
	AlxIoPin* selPinArr[ALX_MUX_IO_PIN_VAL_ARR_LEN];
	uint8_t numOfSelPins;
} AlxMuxTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxMuxTest_Obj* AlxMuxTest_New(uint8_t numOfSelPins);
void AlxMuxTest_Delete(AlxMuxTest_Obj* me);
AlxMux* AlxMuxTest_Mux(AlxMuxTest_Obj* me);
AlxIoPin* AlxMuxTest_EnPin(AlxMuxTest_Obj* me);
AlxIoPin* AlxMuxTest_SelPin(AlxMuxTest_Obj* me, uint8_t index);


//******************************************************************************
// Functions
//******************************************************************************
AlxMuxTest_Obj* AlxMuxTest_New(uint8_t numOfSelPins)
{
	if (numOfSelPins == 0 || numOfSelPins > ALX_MUX_IO_PIN_VAL_ARR_LEN) { return NULL; }

	AlxMuxTest_Obj* me = (AlxMuxTest_Obj*)calloc(1, sizeof(AlxMuxTest_Obj));
	if (me == NULL) { return NULL; }

	me->enPin = (AlxIoPin*)calloc(1, sizeof(AlxIoPin));
	if (me->enPin == NULL) { return NULL; }

	for (uint8_t i = 0; i < numOfSelPins; i++)
	{
		me->selPin[i] = (AlxIoPin*)calloc(1, sizeof(AlxIoPin));
		if (me->selPin[i] == NULL) { return NULL; }
		me->selPinArr[i] = me->selPin[i];
	}
	me->numOfSelPins = numOfSelPins;

	AlxMux_Ctor(&me->mux, me->enPin, me->selPinArr, numOfSelPins);
	return me;
}

void AlxMuxTest_Delete(AlxMuxTest_Obj* me)
{
	if (me == NULL) { return; }
	for (uint8_t i = 0; i < me->numOfSelPins; i++) { free(me->selPin[i]); }
	free(me->enPin);
	free(me);
}

AlxMux* AlxMuxTest_Mux(AlxMuxTest_Obj* me)
{
	return &me->mux;
}

AlxIoPin* AlxMuxTest_EnPin(AlxMuxTest_Obj* me)
{
	return me->enPin;
}

AlxIoPin* AlxMuxTest_SelPin(AlxMuxTest_Obj* me, uint8_t index)
{
	if (index >= me->numOfSelPins) { return NULL; }
	return me->selPin[index];
}
