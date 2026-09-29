/**
  ******************************************************************************
  * @file		alxLp586xFake.c
  * @brief		Auralix C Library - ALX LP586x Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the LP586x LED driver, named after the module it fakes,
  * for a consumer that links the driver's callers but not the driver. The
  * values a caller writes land in the driver instance, where a test can read
  * them; nothing here models I2C transfers or light. A test can also make the
  * driver report the chip as absent, which is what its Init answers then, and
  * read how often each entry point was called - a caller that detects the chip
  * is expected to stop driving it once it is absent.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxLp586x.h"


//******************************************************************************
// Private Variables
//******************************************************************************
static bool alxLp586xFake_present = true;
static uint32_t alxLp586xFake_initPeriphCount;
static uint32_t alxLp586xFake_deInitPeriphCount;
static uint32_t alxLp586xFake_initCount;
static uint32_t alxLp586xFake_handleCount;
static uint32_t alxLp586xFake_ledWriteCount;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxLp586xFake_Reset(void);
void AlxLp586xFake_SetPresent(bool present);
uint32_t AlxLp586xFake_InitPeriphCount(void);
uint32_t AlxLp586xFake_DeInitPeriphCount(void);
uint32_t AlxLp586xFake_InitCount(void);
uint32_t AlxLp586xFake_HandleCount(void);
uint32_t AlxLp586xFake_LedWriteCount(void);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxLp586xFake_Reset(void)
{
	alxLp586xFake_present = true;
	alxLp586xFake_initPeriphCount = 0;
	alxLp586xFake_deInitPeriphCount = 0;
	alxLp586xFake_initCount = 0;
	alxLp586xFake_handleCount = 0;
	alxLp586xFake_ledWriteCount = 0;
}

void AlxLp586xFake_SetPresent(bool present)
{
	alxLp586xFake_present = present;
}

uint32_t AlxLp586xFake_InitPeriphCount(void)
{
	return alxLp586xFake_initPeriphCount;
}

uint32_t AlxLp586xFake_DeInitPeriphCount(void)
{
	return alxLp586xFake_deInitPeriphCount;
}

uint32_t AlxLp586xFake_InitCount(void)
{
	return alxLp586xFake_initCount;
}

uint32_t AlxLp586xFake_HandleCount(void)
{
	return alxLp586xFake_handleCount;
}

uint32_t AlxLp586xFake_LedWriteCount(void)
{
	return alxLp586xFake_ledWriteCount;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxLp586x_Ctor(AlxLp586x* me, AlxIoPin* do_LED_DRV_EN, AlxIoPin* do_LED_DRV_SYNC, uint8_t ledNumUsed, AlxI2c* i2c, uint8_t i2cAddr, bool i2cCheckWithRead, uint8_t i2cNumOfTries, uint16_t i2cTimeout_ms)
{
	(void)me; (void)do_LED_DRV_EN; (void)do_LED_DRV_SYNC; (void)ledNumUsed; (void)i2c;
	(void)i2cAddr; (void)i2cCheckWithRead; (void)i2cNumOfTries; (void)i2cTimeout_ms;
}

Alx_Status AlxLp586x_InitPeriph(AlxLp586x* me)
{
	(void)me;
	alxLp586xFake_initPeriphCount++;
	return Alx_Ok;
}

Alx_Status AlxLp586x_DeInitPeriph(AlxLp586x* me)
{
	(void)me;
	alxLp586xFake_deInitPeriphCount++;
	return Alx_Ok;
}

Alx_Status AlxLp586x_Init(AlxLp586x* me)
{
	(void)me;
	alxLp586xFake_initCount++;
	if (alxLp586xFake_present) { return Alx_Ok; }
	return Alx_Err;
}

Alx_Status AlxLp586x_Handle(AlxLp586x* me)
{
	(void)me;
	alxLp586xFake_handleCount++;
	return Alx_Ok;
}

void AlxLp586x_Led_Write(AlxLp586x* me, uint8_t ledNum, bool val)
{
	// The requested value stays in the driver instance for a test to observe. This models the
	// call boundary only, not the I2C transfer and not the light.
	if (ledNum < sizeof(me->valNew) / sizeof(me->valNew[0])) { me->valNew[ledNum] = val; }
	alxLp586xFake_ledWriteCount++;
}
