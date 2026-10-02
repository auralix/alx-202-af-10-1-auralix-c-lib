/**
  ******************************************************************************
  * @file		alxPcal6416aTestHelpers.c
  * @brief		Auralix C Library - ALX PCAL6416A IO Expander - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps a POINTER to its I2C bus, so the helper owns one next to
  * the expander. The bus is the I2C fake, a register file the test fills the
  * way the part would; every register of this part is one byte.
  *
  * The driver's weak RegStruct_SetVal default asserts - "Implement in APP!" -
  * so this helper is the application here, and it keeps every default the
  * driver set: the tests measure the driver's own configuration.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPcal6416a.h"
#include <stdlib.h>
#include <string.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxPcal6416a pcal;
	AlxI2c i2c;
} AlxPcal6416aTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxPcal6416aTest_Obj* AlxPcal6416aTest_New(void);
void AlxPcal6416aTest_Delete(AlxPcal6416aTest_Obj* me);
AlxPcal6416a* AlxPcal6416aTest_Pcal(AlxPcal6416aTest_Obj* me);
int32_t AlxPcal6416aTest_TriState(const char* name);
int32_t AlxPcal6416aTest_Status_Ok(void);
int32_t AlxPcal6416aTest_Status_Err(void);

// The application hook the driver declares in its .c and this helper defines
void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxPcal6416aTest_Obj* AlxPcal6416aTest_New(void)
{
	AlxPcal6416aTest_Obj* me = (AlxPcal6416aTest_Obj*)calloc(1, sizeof(AlxPcal6416aTest_Obj));
	if (me == NULL) { return NULL; }
	AlxPcal6416a_Ctor(&me->pcal, &me->i2c, 0x40, false, 3, 10);
	return me;
}

void AlxPcal6416aTest_Delete(AlxPcal6416aTest_Obj* me)
{
	free(me);
}

AlxPcal6416a* AlxPcal6416aTest_Pcal(AlxPcal6416aTest_Obj* me)		{ return &me->pcal; }
int32_t AlxPcal6416aTest_Status_Ok(void)								{ return (int32_t)Alx_Ok; }
int32_t AlxPcal6416aTest_Status_Err(void)								{ return (int32_t)Alx_Err; }

int32_t AlxPcal6416aTest_TriState(const char* name)
{
	if (strcmp(name, "HiZ") == 0) { return (int32_t)AlxIoPin_TriState_HiZ; }
	if (strcmp(name, "Hi") == 0) { return (int32_t)AlxIoPin_TriState_Hi; }
	if (strcmp(name, "Lo") == 0) { return (int32_t)AlxIoPin_TriState_Lo; }
	if (strcmp(name, "Undefined") == 0) { return (int32_t)AlxIoPin_TriState_Undefined; }
	return -1;
}


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
void AlxPcal6416a_RegStruct_SetVal(AlxPcal6416a* me)
{
	(void)me;	// this application keeps every default the driver set
}
