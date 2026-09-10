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
  * function AlxIna228_RegStruct_SetVal. This suite does NOT override it: a weak
  * definition is overridden by a strong one when the target links ELF, and the
  * host linker here (lld-link, COFF) reports a duplicate symbol instead. So the
  * library's own weak default runs, which does nothing, and the registers this
  * group writes to the fake are the driver's defaults rather than a product's
  * choices. The factors under test are computed BEFORE that hook is called, so
  * nothing here depends on it - but it is the reason this group tests the
  * driver's arithmetic and not a product's configuration of it.
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
