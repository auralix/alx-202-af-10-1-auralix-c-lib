/**
  ******************************************************************************
  * @file		alxVeml6040TestHelpers.c
  * @brief		Auralix C Library - ALX VEML6040 Colour Sensor - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps a POINTER to its I2C bus, so the helper owns one next to
  * the sensor. The bus is the I2C fake, a register file the test fills the way
  * the part would.
  *
  * The driver's weak RegStruct_SetVal default asserts - "Implement in APP!" -
  * so this helper is the application here and chooses a configuration: the
  * sensor enabled, auto mode, the shortest integration time, all zero bits.
  * It is set through the raw word, which is what travels on the wire; the
  * named bit-fields are laid out differently by the host compiler (A93).
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxVeml6040.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxVeml6040 veml;
	AlxI2c i2c;
} AlxVeml6040Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxVeml6040Test_Obj* AlxVeml6040Test_New(void);
void AlxVeml6040Test_Delete(AlxVeml6040Test_Obj* me);
AlxVeml6040* AlxVeml6040Test_Veml(AlxVeml6040Test_Obj* me);
int32_t AlxVeml6040Test_Status_Ok(void);
int32_t AlxVeml6040Test_Status_Err(void);

// The application hook the driver declares in its .c and this helper defines
void AlxVeml6040_RegStruct_SetVal(AlxVeml6040* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxVeml6040Test_Obj* AlxVeml6040Test_New(void)
{
	AlxVeml6040Test_Obj* me = (AlxVeml6040Test_Obj*)calloc(1, sizeof(AlxVeml6040Test_Obj));
	if (me == NULL) { return NULL; }
	AlxVeml6040_Ctor(&me->veml, &me->i2c, 0x20, false, 3, 10);
	return me;
}

void AlxVeml6040Test_Delete(AlxVeml6040Test_Obj* me)
{
	free(me);
}

AlxVeml6040* AlxVeml6040Test_Veml(AlxVeml6040Test_Obj* me)		{ return &me->veml; }
int32_t AlxVeml6040Test_Status_Ok(void)							{ return (int32_t)Alx_Ok; }
int32_t AlxVeml6040Test_Status_Err(void)							{ return (int32_t)Alx_Err; }


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
void AlxVeml6040_RegStruct_SetVal(AlxVeml6040* me)
{
	me->reg._0x00_CONF.val.raw = 0x0000;	// enabled, auto mode, 40 ms integration
}
