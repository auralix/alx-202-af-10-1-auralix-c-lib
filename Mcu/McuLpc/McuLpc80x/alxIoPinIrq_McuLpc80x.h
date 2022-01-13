/**
  ******************************************************************************
  * @file alxIoPinIrq_McuLpc80x.h
  * @brief Auralix C Library - ALX IO Pin Module
  * @version $LastChangedRevision: 4270 $
  * @date $LastChangedDate: 2021-03-05 19:02:52 +0100 (Fri, 05 Mar 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_IRQ_MCU_LPC80X_H
#define ALX_IO_PIN_IRQ_MCU_LPC80X_H

#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include "alxGlobal.h"
#include "alxTrace.h"
#include "alxAssert.h"
#include "alxIoPin.h"


//******************************************************************************
// Module Guard
//******************************************************************************
#if defined(ALX_LPC80X)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin* ioPin;

	// Variables
	pint_pin_int_t irqPin;
	pint_pin_enable_t irqType;
	Alx_IrqPriority irqPriority;

	// Info
	bool isInit;
	bool wasCtorCalled;
} AlxIoPinIrq;


//******************************************************************************
// Constructor
//******************************************************************************
void AlxIoPinIrq_Ctor
(
	AlxIoPinIrq* me,
	AlxIoPin* ioPin,
	pint_pin_int_t irqPin,
	pint_pin_enable_t irqType,
	Alx_IrqPriority irqPriority
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_IRQ_MCU_LPC80X_H
