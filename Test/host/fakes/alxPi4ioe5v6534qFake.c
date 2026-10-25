/**
  ******************************************************************************
  * @file		alxPi4ioe5v6534qFake.c
  * @brief		Auralix C Library - ALX PI4IOE5V6534Q Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the PI4IOE5V6534Q IO expander driver, named after the
  * module it fakes, for a consumer that links the driver's callers but not the
  * driver: an expander is a grid of pin levels a test can set and read, and
  * the driver's own writes land in the same grid.
  *
  * The driver itself is covered by the library's own host suite, over the I2C
  * fake. It is faked here for the same reason as the current sensor: its I2C
  * transfer lengths come from bit-field register overlays whose size differs
  * between the two compiler ABIs.
  *
  * Expanders are told apart by their ADDRESS. A new slot starts with every pin
  * low, and a reset only forgets which addresses were seen - so a slot reused
  * by the next test never answers with the previous test's pins.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPi4ioe5v6534q.h"
#include <string.h>


//******************************************************************************
// Private Variables
//******************************************************************************
#define ALX_PI4IOE5V6534Q_FAKE_NUM_OF_EXPANDERS 8
#define ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PORTS 8
#define ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PINS 8

static const AlxPi4ioe5v6534q* alxPi4ioe5v6534qFake_exp[ALX_PI4IOE5V6534Q_FAKE_NUM_OF_EXPANDERS];
static bool alxPi4ioe5v6534qFake_level[ALX_PI4IOE5V6534Q_FAKE_NUM_OF_EXPANDERS][ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PORTS][ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PINS];
static uint32_t alxPi4ioe5v6534qFake_used;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxPi4ioe5v6534qFake_Reset(void);
void AlxPi4ioe5v6534qFake_SetLevel(const AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin, bool val);
bool AlxPi4ioe5v6534qFake_Level(const AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin);


//******************************************************************************
// Private Functions
//******************************************************************************
static uint32_t AlxPi4ioe5v6534qFake_Slot(const AlxPi4ioe5v6534q* me)
{
	for (uint32_t i = 0; i < alxPi4ioe5v6534qFake_used; i++)
	{
		if (alxPi4ioe5v6534qFake_exp[i] == me) { return i; }
	}
	if (alxPi4ioe5v6534qFake_used >= ALX_PI4IOE5V6534Q_FAKE_NUM_OF_EXPANDERS) { return ALX_PI4IOE5V6534Q_FAKE_NUM_OF_EXPANDERS - 1; }

	memset(alxPi4ioe5v6534qFake_level[alxPi4ioe5v6534qFake_used], 0, sizeof(alxPi4ioe5v6534qFake_level[0]));
	alxPi4ioe5v6534qFake_exp[alxPi4ioe5v6534qFake_used] = me;
	return alxPi4ioe5v6534qFake_used++;
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxPi4ioe5v6534qFake_Reset(void)
{
	alxPi4ioe5v6534qFake_used = 0;
}

void AlxPi4ioe5v6534qFake_SetLevel(const AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin, bool val)
{
	if (port >= ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PORTS || pin >= ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PINS) { return; }
	alxPi4ioe5v6534qFake_level[AlxPi4ioe5v6534qFake_Slot(me)][port][pin] = val;
}

bool AlxPi4ioe5v6534qFake_Level(const AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin)
{
	if (port >= ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PORTS || pin >= ALX_PI4IOE5V6534Q_FAKE_NUM_OF_PINS) { return false; }
	return alxPi4ioe5v6534qFake_level[AlxPi4ioe5v6534qFake_Slot(me)][port][pin];
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxPi4ioe5v6534q_Ctor(AlxPi4ioe5v6534q* me, AlxIoPin* do_nRESET, AlxI2c* i2c, uint8_t i2cAddr, bool i2cCheckWithRead, uint8_t i2cNumOfTries, uint16_t i2cTimeout_ms)
{
	(void)do_nRESET; (void)i2c; (void)i2cAddr; (void)i2cCheckWithRead; (void)i2cNumOfTries; (void)i2cTimeout_ms;
	AlxPi4ioe5v6534qFake_Slot(me);
}

Alx_Status AlxPi4ioe5v6534q_InitPeriph(AlxPi4ioe5v6534q* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxPi4ioe5v6534q_DeInitPeriph(AlxPi4ioe5v6534q* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxPi4ioe5v6534q_Init(AlxPi4ioe5v6534q* me)
{
	(void)me;
	return Alx_Ok;
}

Alx_Status AlxPi4ioe5v6534q_Handle(AlxPi4ioe5v6534q* me, uint8_t inPortNum, uint8_t outPortNum)
{
	(void)me; (void)inPortNum; (void)outPortNum;
	return Alx_Ok;
}

bool AlxPi4ioe5v6534q_IoPin_Read(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin)
{
	return AlxPi4ioe5v6534qFake_Level(me, port, pin);
}

void AlxPi4ioe5v6534q_IoPin_Write(AlxPi4ioe5v6534q* me, uint8_t port, uint8_t pin, bool val)
{
	AlxPi4ioe5v6534qFake_SetLevel(me, port, pin, val);
}
