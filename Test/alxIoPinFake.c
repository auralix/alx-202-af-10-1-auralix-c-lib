/**
  ******************************************************************************
  * @file		alxIoPinFake.c
  * @brief		Auralix C Library - ALX IO Pin Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the digital pin, named after the module it fakes. A pin
  * is a register write on the target and nothing at all on a PC, so the fake
  * keeps the one thing a test can still ask about: the LEVEL each pin was last
  * driven to, and how many times each call was made.
  *
  * Pins are told apart by their ADDRESS, because that is all a caller has - the
  * structure is opaque and empty on the host. One pin is enough for the drivers
  * this suite exercises, but counting them separately costs nothing and makes a
  * two-pin driver testable without changing the fake.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxIoPin.h"
#include <string.h>


//******************************************************************************
// Private variables
//******************************************************************************
#define ALX_IO_PIN_FAKE_NUM_OF_PINS 8

static const AlxIoPin* alxIoPinFake_pin[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static bool alxIoPinFake_level[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static uint32_t alxIoPinFake_initCount[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static uint32_t alxIoPinFake_deInitCount[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static uint32_t alxIoPinFake_writeCount[ALX_IO_PIN_FAKE_NUM_OF_PINS];


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxIoPinFake_Reset(void);
bool AlxIoPinFake_Level(const AlxIoPin* me);
void AlxIoPinFake_SetLevel(const AlxIoPin* me, bool val);
uint32_t AlxIoPinFake_InitCount(const AlxIoPin* me);
uint32_t AlxIoPinFake_DeInitCount(const AlxIoPin* me);
uint32_t AlxIoPinFake_WriteCount(const AlxIoPin* me);


//******************************************************************************
// Private functions
//******************************************************************************
static uint32_t AlxIoPinFake_Slot(const AlxIoPin* me)
{
	for (uint32_t i = 0; i < ALX_IO_PIN_FAKE_NUM_OF_PINS; i++)
	{
		if (alxIoPinFake_pin[i] == me)
		{
			return i;
		}
	}
	for (uint32_t i = 0; i < ALX_IO_PIN_FAKE_NUM_OF_PINS; i++)
	{
		if (alxIoPinFake_pin[i] == NULL)
		{
			alxIoPinFake_pin[i] = me;
			return i;
		}
	}
	return ALX_IO_PIN_FAKE_NUM_OF_PINS - 1;	// out of slots: fold onto the last one
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxIoPinFake_Reset(void)
{
	memset(alxIoPinFake_pin, 0, sizeof(alxIoPinFake_pin));
	memset(alxIoPinFake_level, 0, sizeof(alxIoPinFake_level));
	memset(alxIoPinFake_initCount, 0, sizeof(alxIoPinFake_initCount));
	memset(alxIoPinFake_deInitCount, 0, sizeof(alxIoPinFake_deInitCount));
	memset(alxIoPinFake_writeCount, 0, sizeof(alxIoPinFake_writeCount));
}
bool AlxIoPinFake_Level(const AlxIoPin* me)
{
	return alxIoPinFake_level[AlxIoPinFake_Slot(me)];
}
void AlxIoPinFake_SetLevel(const AlxIoPin* me, bool val)
{
	// What a test uses to drive an INPUT. The module under test reads it with AlxIoPin_Read and
	// cannot tell this from the pin being pulled by something outside the board - which is the
	// whole point of reading an input.
	alxIoPinFake_level[AlxIoPinFake_Slot(me)] = val;
}
uint32_t AlxIoPinFake_InitCount(const AlxIoPin* me)
{
	return alxIoPinFake_initCount[AlxIoPinFake_Slot(me)];
}
uint32_t AlxIoPinFake_DeInitCount(const AlxIoPin* me)
{
	return alxIoPinFake_deInitCount[AlxIoPinFake_Slot(me)];
}
uint32_t AlxIoPinFake_WriteCount(const AlxIoPin* me)
{
	return alxIoPinFake_writeCount[AlxIoPinFake_Slot(me)];
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxIoPin_Init(AlxIoPin* me)
{
	alxIoPinFake_initCount[AlxIoPinFake_Slot(me)]++;
}
void AlxIoPin_DeInit(AlxIoPin* me)
{
	alxIoPinFake_deInitCount[AlxIoPinFake_Slot(me)]++;
}
void AlxIoPin_Set(AlxIoPin* me)
{
	uint32_t slot = AlxIoPinFake_Slot(me);
	alxIoPinFake_level[slot] = true;
	alxIoPinFake_writeCount[slot]++;
}
void AlxIoPin_Reset(AlxIoPin* me)
{
	uint32_t slot = AlxIoPinFake_Slot(me);
	alxIoPinFake_level[slot] = false;
	alxIoPinFake_writeCount[slot]++;
}
void AlxIoPin_Write(AlxIoPin* me, bool val)
{
	uint32_t slot = AlxIoPinFake_Slot(me);
	alxIoPinFake_level[slot] = val;
	alxIoPinFake_writeCount[slot]++;
}
bool AlxIoPin_Read(AlxIoPin* me)
{
	// An input reads whatever is on it: what a test drove with AlxIoPinFake_SetLevel, or what the
	// module itself last wrote. The fake keeps one level per pin and does not care which it was.
	return alxIoPinFake_level[AlxIoPinFake_Slot(me)];
}
