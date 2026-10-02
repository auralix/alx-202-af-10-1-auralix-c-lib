/**
  ******************************************************************************
  * @file		alxAds8678TestHelpers.c
  * @brief		Auralix C Library - ALX ADS8678 ADC - PC Unit Test Helpers
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * The driver keeps POINTERS to its SPI bus and its reset pin, so the helper
  * owns both next to the converter. The bus is the SPI fake: it records every
  * byte sent and answers from a queue the test fills the way the part would.
  *
  * The driver's weak RegStruct_SetVal is the application's configuration, so
  * this helper is the application and takes its configuration from the test:
  * AlxAds8678Test_SetConfig before Init, raw register bytes only, because the
  * named bit-fields are laid out differently by the host compiler (A93).
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAds8678.h"
#include <stdlib.h>


//******************************************************************************
// Private Types
//******************************************************************************
typedef struct
{
	AlxAds8678 ads;
	AlxSpi spi;
	AlxIoPin nRst;
} AlxAds8678Test_Obj;


//******************************************************************************
// Private Variables
//******************************************************************************
static uint8_t alxAds8678Test_autoScan = 0xFF;
static uint8_t alxAds8678Test_featureSelect = 0x00;
static uint8_t alxAds8678Test_range[ALX_ADS8678_NUM_OF_CH] = {};


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test helpers)
//******************************************************************************
AlxAds8678Test_Obj* AlxAds8678Test_New(void);
void AlxAds8678Test_Delete(AlxAds8678Test_Obj* me);
AlxAds8678* AlxAds8678Test_Ads(AlxAds8678Test_Obj* me);
AlxIoPin* AlxAds8678Test_NRstPin(AlxAds8678Test_Obj* me);
void AlxAds8678Test_SetConfig(uint8_t autoScan, uint8_t featureSelect, const uint8_t* range);
Alx_Status AlxAds8678Test_GetChVoltageAll_V(AlxAds8678Test_Obj* me, float* chVoltage_V, bool* isEnabled);
int32_t AlxAds8678Test_Status_Ok(void);
int32_t AlxAds8678Test_Status_Err(void);

// The application hook the driver declares in its .c and this helper defines
void AlxAds8678_RegStruct_SetVal(AlxAds8678* me);


//******************************************************************************
// Functions
//******************************************************************************
AlxAds8678Test_Obj* AlxAds8678Test_New(void)
{
	AlxAds8678Test_Obj* me = (AlxAds8678Test_Obj*)calloc(1, sizeof(AlxAds8678Test_Obj));
	if (me == NULL) { return NULL; }
	AlxAds8678_Ctor(&me->ads, &me->spi, 3, 10, &me->nRst);
	return me;
}

void AlxAds8678Test_Delete(AlxAds8678Test_Obj* me)
{
	free(me);
}

AlxAds8678* AlxAds8678Test_Ads(AlxAds8678Test_Obj* me)			{ return &me->ads; }
AlxIoPin* AlxAds8678Test_NRstPin(AlxAds8678Test_Obj* me)		{ return &me->nRst; }
int32_t AlxAds8678Test_Status_Ok(void)							{ return (int32_t)Alx_Ok; }
int32_t AlxAds8678Test_Status_Err(void)							{ return (int32_t)Alx_Err; }

void AlxAds8678Test_SetConfig(uint8_t autoScan, uint8_t featureSelect, const uint8_t* range)
{
	alxAds8678Test_autoScan = autoScan;
	alxAds8678Test_featureSelect = featureSelect;
	for (uint32_t i = 0; i < ALX_ADS8678_NUM_OF_CH; i++)
	{
		alxAds8678Test_range[i] = range[i];
	}
}

Alx_Status AlxAds8678Test_GetChVoltageAll_V(AlxAds8678Test_Obj* me, float* chVoltage_V, bool* isEnabled)
{
	AlxAds8678_ChVoltageAll_V all = {};
	Alx_Status status = AlxAds8678_GetChVoltageAll_V(&me->ads, &all);
	for (uint32_t i = 0; i < ALX_ADS8678_NUM_OF_CH; i++)
	{
		chVoltage_V[i] = all.chVoltage_V[i];
		isEnabled[i] = all.isEnabled[i];
	}
	return status;
}


//******************************************************************************
// The application hook the driver requires
//******************************************************************************
void AlxAds8678_RegStruct_SetVal(AlxAds8678* me)
{
	me->reg._0x01_AutoScanSequenceEnable.val.raw = alxAds8678Test_autoScan;
	me->reg._0x03_FeatureSelect.val.raw = alxAds8678Test_featureSelect;
	me->reg._0x05_Channel0InputRange.val.raw = alxAds8678Test_range[0];
	me->reg._0x06_Channel1InputRange.val.raw = alxAds8678Test_range[1];
	me->reg._0x07_Channel2InputRange.val.raw = alxAds8678Test_range[2];
	me->reg._0x08_Channel3InputRange.val.raw = alxAds8678Test_range[3];
	me->reg._0x09_Channel4InputRange.val.raw = alxAds8678Test_range[4];
	me->reg._0x0A_Channel5InputRange.val.raw = alxAds8678Test_range[5];
	me->reg._0x0B_Channel6InputRange.val.raw = alxAds8678Test_range[6];
	me->reg._0x0C_Channel7InputRange.val.raw = alxAds8678Test_range[7];
}
