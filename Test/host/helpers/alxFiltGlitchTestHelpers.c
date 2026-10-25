/**
  ******************************************************************************
  * @file		alxFiltGlitchTestHelpers.c
  * @brief		Auralix C Library - ALX Glitch Filter Modules - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructors and destructors, so the Python test layer (ctypes)
  * never mirrors either filter's struct layout - both embed an AlxTimSw, whose
  * size is not the test's business. All testing goes through the public
  * alxFiltGlitchBool.h and alxFiltGlitchUint32.h API.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxFiltGlitchBool.h"
#include "alxFiltGlitchUint32.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxFiltGlitchBool* AlxFiltGlitchBoolTest_New(bool valInitial, float stableTrueTime_ms, float stableFalseTime_ms);
AlxFiltGlitchBool* AlxFiltGlitchBoolTest_New_us(bool valInitial, uint64_t stableTrueTime_us, uint64_t stableFalseTime_us);
void AlxFiltGlitchBoolTest_Delete(AlxFiltGlitchBool* me);
AlxFiltGlitchUint32* AlxFiltGlitchUint32Test_New(uint32_t valInitial, float stableTime_ms);
void AlxFiltGlitchUint32Test_Delete(AlxFiltGlitchUint32* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxFiltGlitchBool* AlxFiltGlitchBoolTest_New(bool valInitial, float stableTrueTime_ms, float stableFalseTime_ms)
{
	AlxFiltGlitchBool* me = (AlxFiltGlitchBool*)malloc(sizeof(AlxFiltGlitchBool));
	if (me == NULL) { return NULL; }
	AlxFiltGlitchBool_Ctor(me, valInitial, stableTrueTime_ms, stableFalseTime_ms);
	return me;
}
AlxFiltGlitchBool* AlxFiltGlitchBoolTest_New_us(bool valInitial, uint64_t stableTrueTime_us, uint64_t stableFalseTime_us)
{
	AlxFiltGlitchBool* me = (AlxFiltGlitchBool*)malloc(sizeof(AlxFiltGlitchBool));
	if (me == NULL) { return NULL; }
	AlxFiltGlitchBool_Ctor_us(me, valInitial, stableTrueTime_us, stableFalseTime_us);
	return me;
}
void AlxFiltGlitchBoolTest_Delete(AlxFiltGlitchBool* me)
{
	free(me);
}
AlxFiltGlitchUint32* AlxFiltGlitchUint32Test_New(uint32_t valInitial, float stableTime_ms)
{
	AlxFiltGlitchUint32* me = (AlxFiltGlitchUint32*)malloc(sizeof(AlxFiltGlitchUint32));
	if (me == NULL) { return NULL; }
	AlxFiltGlitchUint32_Ctor(me, valInitial, stableTime_ms);
	return me;
}
void AlxFiltGlitchUint32Test_Delete(AlxFiltGlitchUint32* me)
{
	free(me);
}
