/**
  ******************************************************************************
  * @file		alxTmp1075Fake.c
  * @brief		Auralix C Library - ALX TMP1075 Module - PC Unit Test Fake
  * @copyright	Copyright (C) Auralix d.o.o. All rights reserved.
  *
  * A link-time fake of the TMP1075 temperature sensor driver, named after the
  * module it fakes, for a consumer that constructs the driver but does not read
  * it on the host. It satisfies the linker for the constructor and nothing
  * else; a consumer that starts reading the sensor gives this fake its model.
  ******************************************************************************
  **/

//******************************************************************************
// Includes
//******************************************************************************
#include "alxTmp1075.h"


//******************************************************************************
// The faked module's own contract
//******************************************************************************
void AlxTmp1075_Ctor(AlxTmp1075* me, AlxI2c* i2c, uint8_t i2cAddr, bool i2cCheckWithRead, uint8_t i2cNumOfTries, uint16_t i2cTimeout_ms)
{
	(void)me; (void)i2c; (void)i2cAddr; (void)i2cCheckWithRead; (void)i2cNumOfTries; (void)i2cTimeout_ms;
}
