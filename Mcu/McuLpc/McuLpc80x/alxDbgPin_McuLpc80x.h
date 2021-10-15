/**
  ******************************************************************************
  * @file alxDbgPin_McuLpc80x.h
  * @brief Auralix C Library - ALX Debug Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_DBG_PIN_MCU_LPC80x_H
#define ALX_DBG_PIN_MCU_LPC80x_H

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
#if defined(ALX_LPC80x)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Parameters
	uint8_t port;
	uint8_t pin;
	uint32_t mode;

	// Variables
	gpio_pin_config_t gpioConfig;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxDbgPin;


//******************************************************************************
// Variables
//******************************************************************************
extern AlxDbgPin alxDbgPin;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxDbgPin_Ctor
(
	AlxDbgPin* me,
	uint8_t port,
	uint8_t pin,
	uint32_t mode
);

#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_DBG_PIN_MCU_LPC80x_H
