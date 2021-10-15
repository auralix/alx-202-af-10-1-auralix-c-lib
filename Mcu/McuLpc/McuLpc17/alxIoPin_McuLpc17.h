/**
  ******************************************************************************
  * @file alxIoPin_McuLpc17.h
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 5455 $
  * @date $LastChangedDate: 2021-06-21 00:16:06 +0200 (Mon, 21 Jun 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_MCU_LPC17_H
#define ALX_IO_PIN_MCU_LPC17_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC17)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	uint32_t mode;
	uint8_t func;
	bool isOpenDrain;
	bool dir;

	// Info
	bool val;
	bool isInit;
	bool wasCtorCalled;
} AlxIoPin;


//******************************************************************************
// Constructor
//******************************************************************************
static inline void AlxIoPin_Ctor
(
	AlxIoPin* me,
	uint8_t port,
	uint8_t pin,
	uint32_t mode,
	uint8_t func,
	bool isOpenDrain,
	bool dir,
	bool val
)
{
	// Parameters
	me->port = port;
	me->pin = pin;
	me->mode = mode;
	me->func = func;
	me->isOpenDrain = isOpenDrain;
	me->dir = dir;
	me->val = val;

	// Info
	me->isInit = false;
	me->wasCtorCalled = true;
}


#endif

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_MCU_LPC17_H
