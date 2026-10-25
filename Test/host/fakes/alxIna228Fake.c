/**
  ******************************************************************************
  * @file		alxIna228Fake.c
  * @brief		Auralix C Library - ALX INA228 Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the INA228 / INA238 current sensor driver, named after
  * the module it fakes, for a consumer that links the driver's callers but not
  * the driver: a sensor answers the readings a test set for it.
  *
  * The driver itself is covered by the library's own host suite, over the I2C
  * fake. It is faked here rather than linked because its I2C transfer lengths
  * come from sizeof() of bit-field register overlays, and the two compiler
  * ABIs disagree about those: under GNU rules the members share one allocation
  * unit, under Microsoft rules a member of a different type starts a new one,
  * so a PC build asks the bus for more bytes than the register has.
  *
  * Sensors are told apart by their ADDRESS; a sensor nobody set reads zero.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIna228.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_INA228_FAKE_NUM_OF_SENSORS 16

typedef struct
{
	const AlxIna228* me;
	float current_A;
	float busVoltage_V;
	float shuntVoltage_V;
	float power_W;
	float temp_degC;
} AlxIna228Fake_Sensor;

static AlxIna228Fake_Sensor alxIna228Fake_sensor[ALX_INA228_FAKE_NUM_OF_SENSORS];
static uint32_t alxIna228Fake_used;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxIna228Fake_Reset(void);
void AlxIna228Fake_SetCurrent_A(const AlxIna228* me, float current_A);
void AlxIna228Fake_SetBusVoltage_V(const AlxIna228* me, float voltage_V);
void AlxIna228Fake_SetShuntVoltage_V(const AlxIna228* me, float voltage_V);
void AlxIna228Fake_SetPower_W(const AlxIna228* me, float power_W);
void AlxIna228Fake_SetTemp_degC(const AlxIna228* me, float temp_degC);


//******************************************************************************
// Private Functions
//******************************************************************************
static AlxIna228Fake_Sensor* AlxIna228Fake_Slot(const AlxIna228* me)
{
	for (uint32_t i = 0; i < alxIna228Fake_used; i++)
	{
		if (alxIna228Fake_sensor[i].me == me) { return &alxIna228Fake_sensor[i]; }
	}
	if (alxIna228Fake_used >= ALX_INA228_FAKE_NUM_OF_SENSORS) { return &alxIna228Fake_sensor[ALX_INA228_FAKE_NUM_OF_SENSORS - 1]; }

	memset(&alxIna228Fake_sensor[alxIna228Fake_used], 0, sizeof(alxIna228Fake_sensor[0]));
	alxIna228Fake_sensor[alxIna228Fake_used].me = me;
	return &alxIna228Fake_sensor[alxIna228Fake_used++];
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxIna228Fake_Reset(void)
{
	alxIna228Fake_used = 0;
}

void AlxIna228Fake_SetCurrent_A(const AlxIna228* me, float current_A)
{
	AlxIna228Fake_Slot(me)->current_A = current_A;
}

void AlxIna228Fake_SetBusVoltage_V(const AlxIna228* me, float voltage_V)
{
	AlxIna228Fake_Slot(me)->busVoltage_V = voltage_V;
}

void AlxIna228Fake_SetShuntVoltage_V(const AlxIna228* me, float voltage_V)
{
	AlxIna228Fake_Slot(me)->shuntVoltage_V = voltage_V;
}

void AlxIna228Fake_SetPower_W(const AlxIna228* me, float power_W)
{
	AlxIna228Fake_Slot(me)->power_W = power_W;
}

void AlxIna228Fake_SetTemp_degC(const AlxIna228* me, float temp_degC)
{
	AlxIna228Fake_Slot(me)->temp_degC = temp_degC;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxIna228_Ctor(AlxIna228* me, AlxI2c* i2c, uint8_t i2cAddr, bool i2cCheckWithRead, uint8_t i2cNumOfTries, uint16_t i2cTimeout_ms, AlxIna228_RegEnum_0x00_ADCRANGE adcRange, float shuntRes_Ohm, float shuntResTemp_ppmPerDegC)
{
	(void)i2c; (void)i2cAddr; (void)i2cCheckWithRead; (void)i2cNumOfTries; (void)i2cTimeout_ms;
	(void)adcRange; (void)shuntRes_Ohm; (void)shuntResTemp_ppmPerDegC;
	AlxIna228Fake_Slot(me);
}

Alx_Status AlxIna228_InitPeriph(AlxIna228* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxIna228_DeInitPeriph(AlxIna228* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxIna228_Init(AlxIna228* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxIna228_GetCurrent_A(AlxIna228* me, float* current_A)
{
	*current_A = AlxIna228Fake_Slot(me)->current_A;
	return Alx_Ok;
}

Alx_Status AlxIna228_GetBusVoltage_V(AlxIna228* me, float* voltage_V)
{
	*voltage_V = AlxIna228Fake_Slot(me)->busVoltage_V;
	return Alx_Ok;
}

Alx_Status AlxIna228_GetShuntVoltage_V(AlxIna228* me, float* voltage_V)
{
	*voltage_V = AlxIna228Fake_Slot(me)->shuntVoltage_V;
	return Alx_Ok;
}

Alx_Status AlxIna228_GetPower_W(AlxIna228* me, float* power_W)
{
	*power_W = AlxIna228Fake_Slot(me)->power_W;
	return Alx_Ok;
}

Alx_Status AlxIna228_GetTemp_degC(AlxIna228* me, float* temp_degC)
{
	*temp_degC = AlxIna228Fake_Slot(me)->temp_degC;
	return Alx_Ok;
}
