/**
  ******************************************************************************
  * @file		alxAdxl355TestHelpers.c
  * @brief		Auralix C Library - ALX ADXL355 Accelerometer - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps a POINTER to its SPI bus, so the helper owns one next to
  * the accelerometer. The bus is the SPI fake: it records every byte sent and
  * answers from a queue the test fills the way the part would.
  *
  * The driver's weak RegStruct_SetVal default asserts, so this helper is the
  * application here and keeps every default the driver set: the +-2 g range,
  * the part's own filter and FIFO settings.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdxl355.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxAdxl355 adxl;
	AlxSpi spi;
} AlxAdxl355Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxAdxl355Test_Obj* AlxAdxl355Test_New(void);
void AlxAdxl355Test_Delete(AlxAdxl355Test_Obj* me);
AlxAdxl355* AlxAdxl355Test_Adxl(AlxAdxl355Test_Obj* me);
Alx_Status AlxAdxl355Test_GetXyz_g(AlxAdxl355Test_Obj* me, float* x_g, float* y_g, float* z_g);
int32_t AlxAdxl355Test_Status_Ok(void);
int32_t AlxAdxl355Test_Status_Err(void);

// The application hook the driver declares in its .c and this helper defines
void AlxAdxl355_RegStruct_SetVal(AlxAdxl355* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxAdxl355Test_Obj* AlxAdxl355Test_New(void)
{
	AlxAdxl355Test_Obj* me = (AlxAdxl355Test_Obj*)calloc(1, sizeof(AlxAdxl355Test_Obj));
	if (me == NULL) { return NULL; }
	AlxAdxl355_Ctor(&me->adxl, &me->spi, 3, 10);
	return me;
}

void AlxAdxl355Test_Delete(AlxAdxl355Test_Obj* me)
{
	free(me);
}

AlxAdxl355* AlxAdxl355Test_Adxl(AlxAdxl355Test_Obj* me)		{ return &me->adxl; }
int32_t AlxAdxl355Test_Status_Ok(void)							{ return (int32_t)Alx_Ok; }
int32_t AlxAdxl355Test_Status_Err(void)						{ return (int32_t)Alx_Err; }

Alx_Status AlxAdxl355Test_GetXyz_g(AlxAdxl355Test_Obj* me, float* x_g, float* y_g, float* z_g)
{
	AlxAdxl355_Xyz_g xyz = {};
	Alx_Status status = AlxAdxl355_GetXyz_g(&me->adxl, &xyz);
	*x_g = xyz.x_g;
	*y_g = xyz.y_g;
	*z_g = xyz.z_g;
	return status;
}


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
void AlxAdxl355_RegStruct_SetVal(AlxAdxl355* me)
{
	(void)me;	// this application keeps every default the driver set
}
