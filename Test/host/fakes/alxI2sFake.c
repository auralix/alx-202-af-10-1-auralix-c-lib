/**
  ******************************************************************************
  * @file		alxI2sFake.c
  * @brief		Auralix C Library - ALX I2S Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the I2S interface, named after the module it fakes, for
  * a codec driver that starts and runs its audio interface but whose tests are
  * about its control registers. An audio stream is nothing a PC can carry, so
  * the fake keeps the one thing a test can still ask about: how many times the
  * interface was initialised, de-initialised and handled.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxI2s.h"


//******************************************************************************
// Private Variables
//******************************************************************************
static uint32_t alxI2sFake_initCount;
static uint32_t alxI2sFake_deInitCount;
static uint32_t alxI2sFake_handleCount;


static int32_t alxI2sFake_forcedStatus = Alx_Ok;	// what Init answers (A97)


//******************************************************************************
// Prototypes - the DLL export surface (no separate header for test fakes)
//******************************************************************************
void AlxI2sFake_Reset(void);
uint32_t AlxI2sFake_InitCount(void);
uint32_t AlxI2sFake_DeInitCount(void);
uint32_t AlxI2sFake_HandleCount(void);
void AlxI2sFake_SetForcedStatus(int32_t status);


//******************************************************************************
// The fake's own controls
//******************************************************************************
void AlxI2sFake_Reset(void)
{
	alxI2sFake_initCount = 0;
	alxI2sFake_deInitCount = 0;
	alxI2sFake_handleCount = 0;
	alxI2sFake_forcedStatus = Alx_Ok;
}
uint32_t AlxI2sFake_InitCount(void)
{
	return alxI2sFake_initCount;
}
uint32_t AlxI2sFake_DeInitCount(void)
{
	return alxI2sFake_deInitCount;
}
void AlxI2sFake_SetForcedStatus(int32_t status)
{
	alxI2sFake_forcedStatus = status;
}
uint32_t AlxI2sFake_HandleCount(void)
{
	return alxI2sFake_handleCount;
}


//******************************************************************************
// The faked module's own contract
//******************************************************************************
Alx_Status AlxI2s_Init(AlxI2s* me)
{
	(void)me;
	alxI2sFake_initCount++;
	return (Alx_Status)alxI2sFake_forcedStatus;
}
Alx_Status AlxI2s_DeInit(AlxI2s* me)
{
	(void)me;
	alxI2sFake_deInitCount++;
	return Alx_Ok;
}
void AlxI2s_Foreground_Handle(AlxI2s* me)
{
	(void)me;
	alxI2sFake_handleCount++;
}
