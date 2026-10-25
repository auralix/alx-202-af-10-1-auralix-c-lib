/**
  ******************************************************************************
  * @file		alxPca9539aTestHelpers.c
  * @brief		Auralix C Library - ALX PCA9539A IO Expander - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps a POINTER to its I2C bus, so the helper owns one next to
  * the expander. The bus is the I2C fake, a register file the test fills the
  * way the part would; every register of this part is one byte.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPca9539a.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxPca9539a pca;
	AlxI2c i2c;
} AlxPca9539aTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxPca9539aTest_Obj* AlxPca9539aTest_New(void);
AlxPca9539aTest_Obj* AlxPca9539aTest_NewOnBusOf(AlxPca9539aTest_Obj* other);
void AlxPca9539aTest_SetOpenDrain(AlxPca9539aTest_Obj* me, uint8_t port0, uint8_t port1);
void AlxPca9539aTest_Delete(AlxPca9539aTest_Obj* me);
AlxPca9539a* AlxPca9539aTest_Pca(AlxPca9539aTest_Obj* me);
int32_t AlxPca9539aTest_Status_Ok(void);
int32_t AlxPca9539aTest_Status_Err(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxPca9539aTest_Obj* AlxPca9539aTest_New(void)
{
	AlxPca9539aTest_Obj* me = (AlxPca9539aTest_Obj*)calloc(1, sizeof(AlxPca9539aTest_Obj));
	if (me == NULL) { return NULL; }
	AlxPca9539a_Ctor(&me->pca, &me->i2c, 0xE8, false, 3, 10);
	return me;
}

AlxPca9539aTest_Obj* AlxPca9539aTest_NewOnBusOf(AlxPca9539aTest_Obj* other)
{
	// A second expander on the first one's bus - the case Init has for a bus already initialised (A97)
	AlxPca9539aTest_Obj* me = (AlxPca9539aTest_Obj*)calloc(1, sizeof(AlxPca9539aTest_Obj));
	if (me == NULL) { return NULL; }
	AlxPca9539a_Ctor(&me->pca, &other->i2c, 0xEA, false, 3, 10);
	return me;
}
void AlxPca9539aTest_SetOpenDrain(AlxPca9539aTest_Obj* me, uint8_t port0, uint8_t port1)
{
	// Which pins are driven open-drain, by port mask - what the product's register hook decides (A97)
	me->pca.regVal_OpenDrain_0.raw = port0;
	me->pca.regVal_OpenDrain_1.raw = port1;
}
void AlxPca9539aTest_Delete(AlxPca9539aTest_Obj* me)
{
	free(me);
}

AlxPca9539a* AlxPca9539aTest_Pca(AlxPca9539aTest_Obj* me)		{ return &me->pca; }
int32_t AlxPca9539aTest_Status_Ok(void)							{ return (int32_t)Alx_Ok; }
int32_t AlxPca9539aTest_Status_Err(void)							{ return (int32_t)Alx_Err; }
