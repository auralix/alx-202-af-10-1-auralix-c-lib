/**
  ******************************************************************************
  * @file		alxLp586xTestHelpers.c
  * @brief		Auralix C Library - ALX LP586x LED Matrix Driver - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its two pins - enable and sync - and to its I2C
  * bus, so the helper owns all three next to the driver. The bus is the I2C
  * fake, a register file keyed by the 8-bit memory address: the part pages
  * its registers through the I2C address, which the fake does not record, so
  * a brightness register and a core register with the same low byte share a
  * slot there.
  *
  * The driver's weak RegStruct_SetVal default asserts - "Implement in APP!" -
  * so this helper is the application here: it enables the chip and keeps every
  * other default. It sets the raw byte, which is what travels on the wire.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxLp586x.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxLp586x lp;
	AlxIoPin en;
	AlxIoPin sync;
	AlxI2c i2c;
} AlxLp586xTest_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxLp586xTest_Obj* AlxLp586xTest_New(uint8_t ledNumUsed);
void AlxLp586xTest_Delete(AlxLp586xTest_Obj* me);
AlxLp586x* AlxLp586xTest_Lp(AlxLp586xTest_Obj* me);
AlxIoPin* AlxLp586xTest_EnPin(AlxLp586xTest_Obj* me);
AlxIoPin* AlxLp586xTest_SyncPin(AlxLp586xTest_Obj* me);
int32_t AlxLp586xTest_Status_Ok(void);
int32_t AlxLp586xTest_Status_Err(void);

// The application hook the driver declares in its .c and this helper defines
void AlxLp586x_RegStruct_SetVal(AlxLp586x* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxLp586xTest_Obj* AlxLp586xTest_New(uint8_t ledNumUsed)
{
	AlxLp586xTest_Obj* me = (AlxLp586xTest_Obj*)calloc(1, sizeof(AlxLp586xTest_Obj));
	if (me == NULL) { return NULL; }
	AlxLp586x_Ctor(&me->lp, &me->en, &me->sync, ledNumUsed, &me->i2c, 0x40, false, 3, 10);
	return me;
}

void AlxLp586xTest_Delete(AlxLp586xTest_Obj* me)
{
	free(me);
}

AlxLp586x* AlxLp586xTest_Lp(AlxLp586xTest_Obj* me)			{ return &me->lp; }
AlxIoPin* AlxLp586xTest_EnPin(AlxLp586xTest_Obj* me)		{ return &me->en; }
AlxIoPin* AlxLp586xTest_SyncPin(AlxLp586xTest_Obj* me)		{ return &me->sync; }
int32_t AlxLp586xTest_Status_Ok(void)						{ return (int32_t)Alx_Ok; }
int32_t AlxLp586xTest_Status_Err(void)						{ return (int32_t)Alx_Err; }


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
void AlxLp586x_RegStruct_SetVal(AlxLp586x* me)
{
	me->reg._0x00_Chip_En.val.raw = 0x01;	// the chip enabled, everything else as the driver set it
}
