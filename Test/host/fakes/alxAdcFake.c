/**
  ******************************************************************************
  * @file		alxAdcFake.c
  * @brief		Auralix C Library - ALX ADC - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A channel answers the voltage a test set for it, and counts how often it was
  * read. That is the whole model: a module that turns a converter reading into
  * an engineering unit is tested by choosing the reading, which is precisely
  * what a bench cannot do without a calibrated source per channel.
  *
  * A channel nobody set reads 0 V, which is what an unconnected input gives.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxAdc.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_ADC_FAKE_NUM_OF_CH 32

static const AlxAdc* alxAdcFake_adc[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_ch[ALX_ADC_FAKE_NUM_OF_CH];
static float alxAdcFake_voltage_V[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_readCount[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_initCount;
static uint32_t alxAdcFake_deInitCount;
static uint32_t alxAdcFake_used;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxAdcFake_Reset(void);
void AlxAdcFake_SetVoltage_V(const AlxAdc* me, uint32_t ch, float voltage_V);
uint32_t AlxAdcFake_ReadCount(const AlxAdc* me, uint32_t ch);
uint32_t AlxAdcFake_InitCount(void);
uint32_t AlxAdcFake_DeInitCount(void);


//******************************************************************************
// Private functions
//******************************************************************************
static uint32_t AlxAdcFake_Slot(const AlxAdc* me, uint32_t ch)
{
	for (uint32_t i = 0; i < alxAdcFake_used; i++)
	{
		if (alxAdcFake_adc[i] == me && alxAdcFake_ch[i] == ch) { return i; }
	}
	if (alxAdcFake_used >= ALX_ADC_FAKE_NUM_OF_CH) { return ALX_ADC_FAKE_NUM_OF_CH - 1; }

	alxAdcFake_adc[alxAdcFake_used] = me;
	alxAdcFake_ch[alxAdcFake_used] = ch;
	alxAdcFake_voltage_V[alxAdcFake_used] = 0.f;
	alxAdcFake_readCount[alxAdcFake_used] = 0;
	return alxAdcFake_used++;
}


//******************************************************************************
// Test functions
//******************************************************************************
void AlxAdcFake_Reset(void)
{
	memset(alxAdcFake_adc, 0, sizeof(alxAdcFake_adc));
	memset(alxAdcFake_ch, 0, sizeof(alxAdcFake_ch));
	memset(alxAdcFake_voltage_V, 0, sizeof(alxAdcFake_voltage_V));
	memset(alxAdcFake_readCount, 0, sizeof(alxAdcFake_readCount));
	alxAdcFake_initCount = 0;
	alxAdcFake_deInitCount = 0;
	alxAdcFake_used = 0;
}

void AlxAdcFake_SetVoltage_V(const AlxAdc* me, uint32_t ch, float voltage_V)
{
	alxAdcFake_voltage_V[AlxAdcFake_Slot(me, ch)] = voltage_V;
}

uint32_t AlxAdcFake_ReadCount(const AlxAdc* me, uint32_t ch)
{
	return alxAdcFake_readCount[AlxAdcFake_Slot(me, ch)];
}

uint32_t AlxAdcFake_InitCount(void)
{
	return alxAdcFake_initCount;
}

uint32_t AlxAdcFake_DeInitCount(void)
{
	return alxAdcFake_deInitCount;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
Alx_Status AlxAdc_Init(AlxAdc* me)
{
	(void)me;
	alxAdcFake_initCount++;
	return Alx_Ok;
}

Alx_Status AlxAdc_DeInit(AlxAdc* me)
{
	(void)me;
	alxAdcFake_deInitCount++;
	return Alx_Ok;
}

float AlxAdc_GetVoltage_V(AlxAdc* me, Alx_Ch ch)
{
	uint32_t slot = AlxAdcFake_Slot(me, (uint32_t)ch);
	alxAdcFake_readCount[slot]++;
	return alxAdcFake_voltage_V[slot];
}

uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	return (uint32_t)(AlxAdc_GetVoltage_V(me, ch) * 1000.f);
}
