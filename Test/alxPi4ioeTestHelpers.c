/**
  ******************************************************************************
  * @file		alxPi4ioeTestHelpers.c
  * @brief		Auralix C Library - ALX PI4IOE5V6534Q Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor and destructor over a faked I2C bus and a faked
  * reset pin, plus the pin the test wants to inspect afterwards.
  *
  * The driver's register configuration is left to a product through a WEAK
  * AlxPi4ioe5v6534q_RegStruct_SetVal, and this file provides it - see the bottom
  * of the file. It did not until 11.09, on the grounds that the host linker
  * reported a duplicate rather than displacing the weak symbol; that was true
  * of the build at the time and is not true now (alxGlobal.h emits
  * __attribute__((weak)) for clang on Windows since this branch).
  *
  * The override keeps every default the driver set, which is what these tests
  * were measuring all along - nothing under test depends on a product's choice
  * of register values, the pin mapping and the transfer shape are the driver's
  * own. What changes is that the library's default body, ALX_ASSERT(false) with
  * "Implement in APP!" beside it, no longer runs.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPi4ioe5v6534q.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxPi4ioe5v6534q* AlxPi4ioeTest_New(uint8_t i2cAddr);
void AlxPi4ioeTest_Delete(AlxPi4ioe5v6534q* me);
AlxIoPin* AlxPi4ioeTest_ResetPin(AlxPi4ioe5v6534q* me);
uint8_t AlxPi4ioeTest_InputPortAddr(AlxPi4ioe5v6534q* me);
uint8_t AlxPi4ioeTest_OutputPortAddr(AlxPi4ioe5v6534q* me);
int32_t AlxPi4ioeTest_Status_Ok(void);
int32_t AlxPi4ioeTest_Status_Err(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxPi4ioe5v6534q* AlxPi4ioeTest_New(uint8_t i2cAddr)
{
	AlxPi4ioe5v6534q* me = (AlxPi4ioe5v6534q*)calloc(1, sizeof(AlxPi4ioe5v6534q));
	AlxI2c* i2c = (AlxI2c*)calloc(1, sizeof(AlxI2c));
	AlxIoPin* resetPin = (AlxIoPin*)calloc(1, sizeof(AlxIoPin));
	if ((me == NULL) || (i2c == NULL) || (resetPin == NULL))
	{
		free(me);
		free(i2c);
		free(resetPin);
		return NULL;
	}

	AlxPi4ioe5v6534q_Ctor(me, resetPin, i2c, i2cAddr, true, 3, 1000);
	if (AlxPi4ioe5v6534q_InitPeriph(me) != Alx_Ok) { return NULL; }
	if (AlxPi4ioe5v6534q_Init(me) != Alx_Ok) { return NULL; }
	return me;
}
void AlxPi4ioeTest_Delete(AlxPi4ioe5v6534q* me)
{
	if (me != NULL)
	{
		free(me->i2c);
		free(me->do_nRESET);
	}
	free(me);
}
AlxIoPin* AlxPi4ioeTest_ResetPin(AlxPi4ioe5v6534q* me)
{
	return me->do_nRESET;
}
uint8_t AlxPi4ioeTest_InputPortAddr(AlxPi4ioe5v6534q* me)
{
	return me->reg._0x00_InputPort.addr;
}
uint8_t AlxPi4ioeTest_OutputPortAddr(AlxPi4ioe5v6534q* me)
{
	return me->reg._0x05_OutputPort.addr;
}
int32_t AlxPi4ioeTest_Status_Ok(void)
{
	return (int32_t)Alx_Ok;
}
int32_t AlxPi4ioeTest_Status_Err(void)
{
	return (int32_t)Alx_Err;
}


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
void AlxPi4ioe5v6534q_RegStruct_SetVal(AlxPi4ioe5v6534q* me)
{
	(void)me;	// this application keeps every default the driver set
}
