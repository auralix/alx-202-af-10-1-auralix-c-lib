/**
  ******************************************************************************
  * @file		alxVnh7040TestHelpers.c
  * @brief		Auralix C Library - ALX VNH7040 H-Bridge - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its ADC, its PWM and its five pins - the two
  * direction inputs, the two multisense selects and the multisense enable - so
  * the helper owns all of them next to the bridge and hands a test each one.
  * The multisense divider is 1 kOhm over 1 kOhm, so the pin voltage is twice
  * the ADC's and the sense current in uA is the ADC's millivolts.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxVnh7040.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxVnh7040 vnh;
	AlxAdc adc;
	AlxPwm pwm;
	AlxIoPin ina;
	AlxIoPin inb;
	AlxIoPin sel0;
	AlxIoPin sel1;
	AlxIoPin sensEn;
} AlxVnh7040Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxVnh7040Test_Obj* AlxVnh7040Test_New(int32_t breakType);
void AlxVnh7040Test_Delete(AlxVnh7040Test_Obj* me);
AlxVnh7040* AlxVnh7040Test_Vnh(AlxVnh7040Test_Obj* me);
AlxAdc* AlxVnh7040Test_Adc(AlxVnh7040Test_Obj* me);
AlxPwm* AlxVnh7040Test_Pwm(AlxVnh7040Test_Obj* me);
AlxIoPin* AlxVnh7040Test_Pin(AlxVnh7040Test_Obj* me, int32_t index);
uint32_t AlxVnh7040Test_SenseCh(void);
uint32_t AlxVnh7040Test_PwmCh(void);


//******************************************************************************
// Functions
//******************************************************************************
AlxVnh7040Test_Obj* AlxVnh7040Test_New(int32_t breakType)
{
	AlxVnh7040Test_Obj* me = (AlxVnh7040Test_Obj*)calloc(1, sizeof(AlxVnh7040Test_Obj));
	if (me == NULL) { return NULL; }
	AlxVnh7040_Ctor(&me->vnh, &me->adc, Alx_Ch_3, &me->pwm, Alx_Ch_1, &me->ina, &me->inb, &me->sel0, &me->sel1,
		&me->sensEn, (AlxVnh7040_BreakType)breakType, 1000, 1000, 100);
	return me;
}

void AlxVnh7040Test_Delete(AlxVnh7040Test_Obj* me)
{
	free(me);
}

AlxVnh7040* AlxVnh7040Test_Vnh(AlxVnh7040Test_Obj* me)		{ return &me->vnh; }
AlxAdc* AlxVnh7040Test_Adc(AlxVnh7040Test_Obj* me)			{ return &me->adc; }
AlxPwm* AlxVnh7040Test_Pwm(AlxVnh7040Test_Obj* me)			{ return &me->pwm; }
uint32_t AlxVnh7040Test_SenseCh(void)						{ return (uint32_t)Alx_Ch_3; }
uint32_t AlxVnh7040Test_PwmCh(void)							{ return (uint32_t)Alx_Ch_1; }

AlxIoPin* AlxVnh7040Test_Pin(AlxVnh7040Test_Obj* me, int32_t index)
{
	if (index == 0) { return &me->ina; }
	if (index == 1) { return &me->inb; }
	if (index == 2) { return &me->sel0; }
	if (index == 3) { return &me->sel1; }
	return &me->sensEn;
}
