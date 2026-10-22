/**
  ******************************************************************************
  * @file		alxAds114s08TestHelpers.c
  * @brief		Auralix C Library - ALX ADS114S08 ADC - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its SPI bus and its three pins - data ready,
  * start and reset - so the helper owns all four next to the converter. The
  * bus is the SPI fake: it records every byte sent and answers from a queue
  * the test fills the way the part would.
  *
  * Init cannot run on this host until A93: a register's length is the size of
  * its union, which the host makes up to five bytes where the target makes one,
  * and the read-back frame of Init is sized by it. AlxAds114s08Test_MarkInit
  * leaves the converter in the state Init leaves it, so the conversion itself
  * can be proven.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAds114s08.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxAds114s08 ads;
	AlxSpi spi;
	AlxIoPin nDrdy;
	AlxIoPin start;
	AlxIoPin nRst;
} AlxAds114s08Test_Obj;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxAds114s08Test_Obj* AlxAds114s08Test_New(void);
void AlxAds114s08Test_Delete(AlxAds114s08Test_Obj* me);
AlxAds114s08* AlxAds114s08Test_Ads(AlxAds114s08Test_Obj* me);
AlxIoPin* AlxAds114s08Test_NDrdyPin(AlxAds114s08Test_Obj* me);
AlxIoPin* AlxAds114s08Test_StartPin(AlxAds114s08Test_Obj* me);
AlxIoPin* AlxAds114s08Test_NRstPin(AlxAds114s08Test_Obj* me);
Alx_Status AlxAds114s08Test_Perform(AlxAds114s08Test_Obj* me, int16_t* raw);
void AlxAds114s08Test_MarkInit(AlxAds114s08Test_Obj* me);
void AlxAds114s08Test_TimePasses(bool enable);
void AlxDelayFake_SetOnDelay(void (*onDelay)(uint64_t delay_ms));	// alxDelayFake.c
int32_t AlxAds114s08Test_Status_Ok(void);
int32_t AlxAds114s08Test_Status_Err(void);

// Not in the module's header, but defined with external linkage by the module
Alx_Status AlxAds114s08_PerformAdcConversion(AlxAds114s08* me, int16_t* chVoltage_raw, uint32_t timeout_ms);


//******************************************************************************
// Functions
//******************************************************************************
AlxAds114s08Test_Obj* AlxAds114s08Test_New(void)
{
	AlxAds114s08Test_Obj* me = (AlxAds114s08Test_Obj*)calloc(1, sizeof(AlxAds114s08Test_Obj));
	if (me == NULL) { return NULL; }
	AlxAds114s08_Ctor(&me->ads, &me->spi, 3, 10, &me->nDrdy, &me->start, &me->nRst);
	return me;
}

void AlxAds114s08Test_Delete(AlxAds114s08Test_Obj* me)
{
	free(me);
}

AlxAds114s08* AlxAds114s08Test_Ads(AlxAds114s08Test_Obj* me)		{ return &me->ads; }
AlxIoPin* AlxAds114s08Test_NDrdyPin(AlxAds114s08Test_Obj* me)		{ return &me->nDrdy; }
AlxIoPin* AlxAds114s08Test_StartPin(AlxAds114s08Test_Obj* me)		{ return &me->start; }
AlxIoPin* AlxAds114s08Test_NRstPin(AlxAds114s08Test_Obj* me)		{ return &me->nRst; }
int32_t AlxAds114s08Test_Status_Ok(void)							{ return (int32_t)Alx_Ok; }
int32_t AlxAds114s08Test_Status_Err(void)							{ return (int32_t)Alx_Err; }

Alx_Status AlxAds114s08Test_Perform(AlxAds114s08Test_Obj* me, int16_t* raw)
{
	return AlxAds114s08_PerformAdcConversion(&me->ads, raw, me->ads.CONVERSION_TIMEOUT_ms);
}

void AlxAds114s08Test_MarkInit(AlxAds114s08Test_Obj* me)
{
	me->ads.isInit = true;
}
static void AlxAds114s08Test_Advance(uint64_t delay_ms)
{
	AlxTick_IncRange_ns(&alxTick, delay_ms * 1000000ull);
}
void AlxAds114s08Test_TimePasses(bool enable)
{
	// With it on, every millisecond the driver waits moves the clock: a conversion that never becomes
	// ready reaches its timeout instead of waiting forever (A97)
	AlxDelayFake_SetOnDelay(enable ? AlxAds114s08Test_Advance : NULL);
}
