/**
  ******************************************************************************
  * @file		alxTmp1075TestHelpers.c
  * @brief		Auralix C Library - ALX TMP1075 Temperature Sensor - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps a POINTER to its I2C bus, so the helper owns one next to
  * the sensor. The bus is the I2C fake, a register file the test fills the way
  * the part would.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTmp1075.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxTmp1075 tmp;
	AlxI2c i2c;
} AlxTmp1075Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxTmp1075Test_Obj* AlxTmp1075Test_New(void);
void AlxTmp1075Test_Delete(AlxTmp1075Test_Obj* me);
AlxTmp1075* AlxTmp1075Test_Tmp(AlxTmp1075Test_Obj* me);
int32_t AlxTmp1075Test_Status_Ok(void);
int32_t AlxTmp1075Test_Status_Err(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxTmp1075Test_Obj* AlxTmp1075Test_New(void)
{
	AlxTmp1075Test_Obj* me = (AlxTmp1075Test_Obj*)calloc(1, sizeof(AlxTmp1075Test_Obj));
	if (me == NULL) { return NULL; }
	AlxTmp1075_Ctor(&me->tmp, &me->i2c, 0x90, false, 3, 10);
	return me;
}

void AlxTmp1075Test_Delete(AlxTmp1075Test_Obj* me)
{
	free(me);
}

AlxTmp1075* AlxTmp1075Test_Tmp(AlxTmp1075Test_Obj* me)		{ return &me->tmp; }
int32_t AlxTmp1075Test_Status_Ok(void)						{ return (int32_t)Alx_Ok; }
int32_t AlxTmp1075Test_Status_Err(void)						{ return (int32_t)Alx_Err; }
