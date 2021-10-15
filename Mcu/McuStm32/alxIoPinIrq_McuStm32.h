/**
  ******************************************************************************
  * @file alxIoPinIrq_McuStm32.h
  * @brief Auralix C Library - ALX IO Pin IRQ Module
  * @version $LastChangedRevision: 5745 $
  * @date $LastChangedDate: 2021-08-08 19:52:52 +0200 (Sun, 08 Aug 2021) $
  ******************************************************************************
  */

#ifndef ALX_IO_PIN_IRQ_MCU_STM32_H
#define ALX_IO_PIN_IRQ_MCU_STM32_H

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
#if defined(ALX_STM32F4) || defined(ALX_STM32G4) || defined(ALX_STM32L0)


//******************************************************************************
// Types
//******************************************************************************
typedef struct
{
	// Objects - External
	AlxIoPin** ioPinArr;

	// Parameters
	uint8_t numOfIoPins;
	Alx_IrqPriority* irqPriorityArr;

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
	AlxIoPin** ioPinArr,
	uint8_t numOfIoPins,
	Alx_IrqPriority* irqPriorityArr
);


#endif // Module Guard

#ifdef __cplusplus
}
#endif

#endif // ALX_IO_PIN_IRQ_MCU_STM32_H
