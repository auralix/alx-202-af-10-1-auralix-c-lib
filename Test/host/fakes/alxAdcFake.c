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
  * A channel can also be given a SEQUENCE. A product reads one converter pin
  * many times while an analog multiplexer walks its channels, so "what is on
  * this input" is not one number - it is the order the mux presents. A sequence
  * is handed out one reading at a time and its last value repeats, so a test
  * says what the mux will see and stops caring how many times the product looks.
  *
  * A channel nobody set reads 0 V, which is what an unconnected input gives.
  *
  * Owned by the library, as every fake of a library module is: a consumer links
  * this file from the library's Test/host/fakes/ and adds nothing of its own.
  * On an MCU family the module's own constructor exists and is defined here
  * under the same guard the module header uses; without a family, as in the
  * library's own host build, the handle is an empty struct and there is none.
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
#define ALX_ADC_FAKE_NUM_OF_CH 128
#define ALX_ADC_FAKE_SEQ_LEN 16

static const AlxAdc* alxAdcFake_adc[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_ch[ALX_ADC_FAKE_NUM_OF_CH];
static float alxAdcFake_voltage_V[ALX_ADC_FAKE_NUM_OF_CH][ALX_ADC_FAKE_SEQ_LEN];
static uint32_t alxAdcFake_seqLen[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_seqIndex[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_readCount[ALX_ADC_FAKE_NUM_OF_CH];
static uint32_t alxAdcFake_initCount;
static uint32_t alxAdcFake_deInitCount;
static uint32_t alxAdcFake_used;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxAdcFake_Reset(void);
void AlxAdcFake_SetVoltage_V(const AlxAdc* me, uint32_t ch, float voltage_V);
void AlxAdcFake_SetVoltageSeq_V(const AlxAdc* me, uint32_t ch, const float* voltage_V, uint32_t len);
uint32_t AlxAdcFake_ReadCount(const AlxAdc* me, uint32_t ch);
uint32_t AlxAdcFake_InitCount(void);
uint32_t AlxAdcFake_DeInitCount(void);


//******************************************************************************
// Private Functions
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
	alxAdcFake_voltage_V[alxAdcFake_used][0] = 0.f;
	alxAdcFake_seqLen[alxAdcFake_used] = 1;
	alxAdcFake_seqIndex[alxAdcFake_used] = 0;
	alxAdcFake_readCount[alxAdcFake_used] = 0;
	return alxAdcFake_used++;
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxAdcFake_Reset(void)
{
	memset(alxAdcFake_adc, 0, sizeof(alxAdcFake_adc));
	memset(alxAdcFake_ch, 0, sizeof(alxAdcFake_ch));
	memset(alxAdcFake_voltage_V, 0, sizeof(alxAdcFake_voltage_V));
	memset(alxAdcFake_seqLen, 0, sizeof(alxAdcFake_seqLen));
	memset(alxAdcFake_seqIndex, 0, sizeof(alxAdcFake_seqIndex));
	memset(alxAdcFake_readCount, 0, sizeof(alxAdcFake_readCount));
	alxAdcFake_initCount = 0;
	alxAdcFake_deInitCount = 0;
	alxAdcFake_used = 0;
}

void AlxAdcFake_SetVoltage_V(const AlxAdc* me, uint32_t ch, float voltage_V)
{
	uint32_t slot = AlxAdcFake_Slot(me, ch);
	alxAdcFake_voltage_V[slot][0] = voltage_V;
	alxAdcFake_seqLen[slot] = 1;
	alxAdcFake_seqIndex[slot] = 0;
}

void AlxAdcFake_SetVoltageSeq_V(const AlxAdc* me, uint32_t ch, const float* voltage_V, uint32_t len)
{
	if (len == 0 || len > ALX_ADC_FAKE_SEQ_LEN) { return; }

	uint32_t slot = AlxAdcFake_Slot(me, ch);
	for (uint32_t i = 0; i < len; i++) { alxAdcFake_voltage_V[slot][i] = voltage_V[i]; }
	alxAdcFake_seqLen[slot] = len;
	alxAdcFake_seqIndex[slot] = 0;
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
#if defined(ALX_STM32)
void AlxAdc_Ctor(AlxAdc* me, ADC_TypeDef* adc, AlxIoPin** ioPinArr, uint8_t numOfIoPins, Alx_Ch* chArr, uint8_t numOfCh, AlxClk* clk, AlxAdc_Clk adcClk, uint32_t samplingTime, bool isVrefInt_V, float vrefExt_V)
{
	(void)me; (void)adc; (void)ioPinArr; (void)numOfIoPins; (void)chArr; (void)numOfCh;
	(void)clk; (void)adcClk; (void)samplingTime; (void)isVrefInt_V; (void)vrefExt_V;
}
#endif

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

	// The reading a sequence is at, and its last value once the sequence has run out: a test says
	// what the multiplexer presents, not how many times the product decides to look.
	uint32_t index = alxAdcFake_seqIndex[slot];
	if (index >= alxAdcFake_seqLen[slot]) { index = alxAdcFake_seqLen[slot] - 1; }
	alxAdcFake_seqIndex[slot]++;
	alxAdcFake_readCount[slot]++;
	return alxAdcFake_voltage_V[slot][index];
}

uint32_t AlxAdc_GetVoltage_mV(AlxAdc* me, Alx_Ch ch)
{
	return (uint32_t)(AlxAdc_GetVoltage_V(me, ch) * 1000.f);
}
