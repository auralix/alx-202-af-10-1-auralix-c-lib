/**
  ******************************************************************************
  * @file alxPcal6416a.c
  * @brief Auralix C Library - ALX IO Expander PCAL6416A Module
  * @version $LastChangedRevision: 4584 $
  * @date $LastChangedDate: 2021-04-03 03:11:01 +0200 (Sat, 03 Apr 2021) $
  ******************************************************************************
  */

//******************************************************************************
// Includes
//******************************************************************************
#include "alxPcal6416a.h"


//******************************************************************************
// Private Functions
//******************************************************************************
static void AlxPcal6416a_RegStruct_SetAddr(AlxPcal6416a* me);



//******************************************************************************
// Weak Functions
//******************************************************************************
void AlxPcal6416a_RegStruct_SetClkVal(AlxPcal6416a* me);
void AlxPcal6416a_RegStruct_SetNonClkVal(AlxPcal6416a* me);


//******************************************************************************
// Constructor
//******************************************************************************
void AlxPcal6416a_Ctor
(
	AlxPcal6416a* me,
	AlxI2c* i2c,
	uint8_t i2cAddr,
	bool i2cCheckWithRead,
	uint8_t i2cNumOfTries,
	uint16_t i2cTimeout_ms
)
{
	// Parameters - Const


	// Objects - External
	me->i2c = i2c;

	// Parameters
	me->i2cAddr = i2cAddr;
	me->i2cCheckWithRead = i2cCheckWithRead;
	me->i2cNumOfTries = i2cNumOfTries;
	me->i2cTimeout_ms = i2cTimeout_ms;

	// Variables
	AlxPcal6416a_RegStruct_SetAddr(me);
	AlxPcal6416a_RegStruct_SetLen(me);
	AlxPcal6416a_RegStruct_SetValToZero(me);

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


//******************************************************************************
// Functions
//******************************************************************************


//******************************************************************************
// Private Functions
//******************************************************************************


//******************************************************************************
// Weak Functions
//******************************************************************************
