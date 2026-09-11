/**
  ******************************************************************************
  * @file		alxIoPinFake.c
  * @brief		Auralix C Library - ALX IO Pin Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the digital pin, named after the module it fakes. A pin
  * is a register write on the target and nothing at all on a PC, so the fake
  * keeps the one thing a test can still ask about: the LEVEL each pin was last
  * driven to, and how many times each call was made. A pin also holds a
  * TRI-STATE of its own, for the identification straps AlxId reads.
  *
  * Pins are told apart by their ADDRESS, because that is all a caller has - the
  * structure is opaque and empty on the host. One pin is enough for the drivers
  * this suite exercises, but counting them separately costs nothing and makes a
  * two-pin driver testable without changing the fake.
  *
  * There are a fixed number of slots, and a test that asks for more than that
  * used to have its extra pins silently FOLDED onto the last slot - two pins
  * reading as one, which is not a failure, just a wrong answer. It cost an hour
  * of reading a correct module looking for a defect that was in the fake. The
  * fold is still there, because aborting inside a DLL takes the test runner
  * with it, but it now RAISES A FLAG that a fixture can assert on.
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
#define ALX_IO_PIN_FAKE_NUM_OF_PINS 32

static const AlxIoPin* alxIoPinFake_pin[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static bool alxIoPinFake_level[ALX_IO_PIN_FAKE_NUM_OF_PINS];
// A tri-state pin is a SECOND thing a pin can be, not a third level of the first: a pin read as
// tri-state answers HiZ/Hi/Lo/Undefined and never touches the bool level, so a test can drive the
// two independently on the same pin. Kept apart for that reason - folding them would make a pin
// driven low as a bool read as Lo as a tri-state, which the hardware does not promise.
static AlxIoPin_TriState alxIoPinFake_triState[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static uint32_t alxIoPinFake_initCount[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static uint32_t alxIoPinFake_deInitCount[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static uint32_t alxIoPinFake_writeCount[ALX_IO_PIN_FAKE_NUM_OF_PINS];
static bool alxIoPinFake_didOverflow;


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxIoPinFake_Reset(void);
bool AlxIoPinFake_Level(const AlxIoPin* me);
void AlxIoPinFake_SetLevel(const AlxIoPin* me, bool val);
void AlxIoPinFake_SetTriState(const AlxIoPin* me, AlxIoPin_TriState val);
uint32_t AlxIoPinFake_InitCount(const AlxIoPin* me);
uint32_t AlxIoPinFake_DeInitCount(const AlxIoPin* me);
bool AlxIoPinFake_DidOverflow(void);
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
	// Out of slots: fold onto the last one and say so. Two pins sharing a slot answer each
	// other's level, so a test that reads this flag as false is the only one whose pin answers
	// mean anything.
	alxIoPinFake_didOverflow = true;
	return ALX_IO_PIN_FAKE_NUM_OF_PINS - 1;
}


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxIoPinFake_Reset(void)
{
	memset(alxIoPinFake_pin, 0, sizeof(alxIoPinFake_pin));
	memset(alxIoPinFake_level, 0, sizeof(alxIoPinFake_level));
	memset(alxIoPinFake_triState, 0, sizeof(alxIoPinFake_triState));	// 0 = AlxIoPin_TriState_HiZ
	memset(alxIoPinFake_initCount, 0, sizeof(alxIoPinFake_initCount));
	memset(alxIoPinFake_deInitCount, 0, sizeof(alxIoPinFake_deInitCount));
	memset(alxIoPinFake_writeCount, 0, sizeof(alxIoPinFake_writeCount));
	alxIoPinFake_didOverflow = false;
}

bool AlxIoPinFake_DidOverflow(void)
{
	// True once more pins have been seen than there are slots. Whatever a test measured
	// after that is two pins' worth of answers coming from one.
	return alxIoPinFake_didOverflow;
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
void AlxIoPinFake_SetTriState(const AlxIoPin* me, AlxIoPin_TriState val)
{
	// What a test uses to drive a pin that is read as a tri-state - a board identification strap,
	// which is left floating, tied high or tied low, and is the only place the third state means
	// anything.
	alxIoPinFake_triState[AlxIoPinFake_Slot(me)] = val;
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
AlxIoPin_TriState AlxIoPin_Read_TriState(AlxIoPin* me)
{
	// On the target this is two reads with the pull resistor swapped between them; here it is
	// whatever the test strapped the pin to. A pin never driven reads HiZ, which is what an
	// unpopulated strap resistor gives on a real board.
	return alxIoPinFake_triState[AlxIoPinFake_Slot(me)];
}
