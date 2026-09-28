/**
  ******************************************************************************
  * @file		alxIna228TestHelpers.c
  * @brief		Auralix C Library - ALX INA228/INA238 Module - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * Opaque-handle constructor and destructor over a faked I2C bus, plus getters
  * for the two conversion factors the driver computes from the constructor's
  * arguments - which is what these tests exist to pin.
  *
  * The driver leaves its register configuration to a product, through the WEAK
  * function AlxIna228_RegStruct_SetVal, and this file provides it - see the
  * bottom of the file. It did not until 11.09, on the grounds that lld-link
  * reported a duplicate symbol rather than letting a strong definition displace
  * the weak one. That was true of the build at the time and is not true now:
  * alxGlobal.h has emitted __attribute__((weak)) for clang on Windows since
  * this branch, so the override works the way it does on the target.
  *
  * The override keeps every default the driver set, which is what this group
  * has been measuring all along - the two conversion factors are computed
  * BEFORE the hook is called, so nothing here depends on it. What changes is
  * that the library's default body, ALX_ASSERT(false) with "Implement in APP!"
  * beside it, no longer runs.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIna228.h"
#include <stdlib.h>


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxIna228* AlxIna228Test_New(int32_t adcRange, float shuntRes_Ohm, float shuntResTemp_ppmPerDegC);
void AlxIna228Test_Delete(AlxIna228* me);
float AlxIna228Test_CurrentLsb_A(AlxIna228* me);
uint16_t AlxIna228Test_ShuntCal(AlxIna228* me);
int32_t AlxIna228Test_AdcRange_163_84_mV(void);
int32_t AlxIna228Test_AdcRange_40_96_mV(void);
int32_t AlxIna228Test_Status_Ok(void);
int32_t AlxIna228Test_Status_Err(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxIna228* AlxIna228Test_New(int32_t adcRange, float shuntRes_Ohm, float shuntResTemp_ppmPerDegC)
{
	AlxIna228* me = (AlxIna228*)calloc(1, sizeof(AlxIna228));
	AlxI2c* i2c = (AlxI2c*)calloc(1, sizeof(AlxI2c));
	if ((me == NULL) || (i2c == NULL))
	{
		free(me);
		free(i2c);
		return NULL;
	}

	// a main power sensor, less the parts a fake bus does not care about
	AlxIna228_Ctor(me, i2c, 0x80, true, 3, 1000,
		(AlxIna228_RegEnum_0x00_ADCRANGE)adcRange, shuntRes_Ohm, shuntResTemp_ppmPerDegC);
	if (AlxIna228_InitPeriph(me) != Alx_Ok) { free(i2c); free(me); return NULL; }
	if (AlxIna228_Init(me) != Alx_Ok) { free(i2c); free(me); return NULL; }
	return me;
}
void AlxIna228Test_Delete(AlxIna228* me)
{
	if (me != NULL)
	{
		free(me->i2c);
	}
	free(me);
}
float AlxIna228Test_CurrentLsb_A(AlxIna228* me)
{
	return me->conversionCurrentLsbFactor;
}
uint16_t AlxIna228Test_ShuntCal(AlxIna228* me)
{
	return me->conversionShuntFactor;
}
int32_t AlxIna228Test_AdcRange_163_84_mV(void)
{
	return (int32_t)AdcRange_163_84_mV;
}
int32_t AlxIna228Test_AdcRange_40_96_mV(void)
{
	return (int32_t)AdcRange_40_96_mV;
}
int32_t AlxIna228Test_Status_Ok(void)
{
	return (int32_t)Alx_Ok;
}
int32_t AlxIna228Test_Status_Err(void)
{
	return (int32_t)Alx_Err;
}


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
// AlxIna228_RegStruct_SetVal is ALX_WEAK in the library and its default body is ALX_ASSERT(false)
// with "Implement in APP!" beside it: Init calls SetToDefault and then calls this, so a product is
// expected to write its own register configuration here. This group had no override, and because
// it also compiled its assertions out, every test in it ran a driver whose application hook had
// asserted - invisibly, until 11.09. Keeping the driver's defaults is a legitimate application
// choice and is what these tests have in fact been checking, so the override says so in one line
// rather than changing what any of them measure.
void AlxIna228_RegStruct_SetVal(AlxIna228* me)
{
	(void)me;	// this application keeps every default the driver set
}
